#include <gmpxx.h>

//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#include <spdlog/spdlog.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <prinsight/prinsight.hpp>

#include "dmcfe/include/dmcfe_decryptor.hpp"
#include "dmcfe/include/dmcfe_encryptor.hpp"
#include "dmcfe/include/utils.hpp"
#include "models/analytics_data_model.hpp"
#include "models/client_public_data_model.hpp"
#include "models/client_registration_model.hpp"
#include "models/scheme_params_model.hpp"

#if defined(ENABLE_PRINSIGHT_TESTING)
#  include <doctest/doctest.h>
#endif

using namespace prinsight;

Core::Core() { mEncryptor = nullptr; }
Core::~Core() {
  if (nullptr != mEncryptor) {
    // delete mEncryptor;
  }
}

Status Core::getRegistrationData(std::string& registrationData) {
  ClientRegistrationModel c(Utils::randomString(16));
  nlohmann::json j = c;
  registrationData = j.dump();
  SPDLOG_DEBUG("client registration data {}", registrationData);
  return Status::kOk;
}

Status Core::initializeAnalyticsScheme(const std::string& schemeParamsData) {
  const auto j = nlohmann::json::parse(schemeParamsData);
  SchemeParamsModel s = j.get<SchemeParamsModel>();
  SPDLOG_DEBUG("received {}", s);
  mEncryptor = new DMCFEncryptor(s.getIndex(), s.getParticipantsCount(), s.getBound());

  return Status::kOk;
}

Status Core::getPublicData(std::string& publicData) {
  PublicKey pubKey;
  std::string pubKeyB64 = "";
  ClientPublicDataModel pubData;
  nlohmann::json j;

  if (nullptr == mEncryptor) {
    return Status::kSchemeUninitialized;
  }
  pubKey = mEncryptor->publicKey();
  pubKey.toBase64(pubKeyB64);
  pubData = ClientPublicDataModel(mEncryptor->clientId(), pubKeyB64);
  j = pubData;
  publicData = j.dump();

  SPDLOG_DEBUG("client public data {}", publicData);

  return Status::kOk;
}

Status Core::provideParticipantsPublicData(const std::string& participantsPublicData) {
  std::vector<PublicKey> pubKeyList;

  if (nullptr == mEncryptor) {
    return Status::kSchemeUninitialized;
  }

  SPDLOG_DEBUG("input: {}", participantsPublicData);
  const auto j = nlohmann::json::parse(participantsPublicData);
  std::vector<ClientPublicDataModel> clientPublicDataList
      = j.get<std::vector<ClientPublicDataModel>>();

  std::size_t currentParticipantsCount = clientPublicDataList.size();
  std::size_t initializedParticipantsCount = mEncryptor->dataVectorLength();
  if (currentParticipantsCount != initializedParticipantsCount) {
    return Status::kBadParams;
  }

  pubKeyList.reserve(currentParticipantsCount);
  // server gives the list of pub-keys of all clients including this one
  // we need to make sure pub-key list passed to cifer is correctly indexed
  for (auto c : clientPublicDataList) {
    PublicKey p;
    // extract index
    // FIXME what to do with already visited index
    std::size_t i = c.getIndex();
    if (i < 0 || i >= initializedParticipantsCount) {
      return Status::kBadParams;
    }

    std::string pubKeyB64 = c.publicKey();
    p.fromBase64(pubKeyB64);
    pubKeyList.insert(pubKeyList.begin() + i, p);
  }

  return mEncryptor->setParticipantsPublicKeys(pubKeyList);
}

Status Core::setClearAnalyticsData(const std::string& label, std::uint64_t value) {
  mAnalyticsData[label] = value;
  return Status::kOk;
}

Status Core::getEncryptedAnalyticsData(std::string& analyticsData) {
  std::unordered_map<std::string, std::string> encData;
  Cipher cipher;
  FunctionalDecryptionKey fdKey;
  std::vector<std::string> fdKeyStr;
  Status status = Status::kUnknownError;

  if (nullptr == mEncryptor) {
    return Status::kSchemeUninitialized;
  }

  // encrypt the data
  for (auto d : mAnalyticsData) {
    std::string cipherB64;
    status = mEncryptor->encrypt(d.first, d.second, cipher);
    if (Status::kOk != status) {
      return status;
    }
    // base64 conversion
    cipher.toBase64(cipherB64);
    // store label and encrypted data
    encData[d.first] = cipherB64;
  }

  // get the functional decryption key
  // FIXME, support for arbitrary policy/weights
  const std::vector<std::int64_t> policy(mEncryptor->dataVectorLength(), 1);
  status = mEncryptor->getFunctionalDecryptionKey(policy, fdKey);
  if (Status::kOk != status) {
    return status;
  }
  // base64 conversion
  fdKey.toBase64(fdKeyStr);

  // json serialization
  AnalyticsData a(mEncryptor->clientId(), encData, fdKeyStr);
  nlohmann::json j = a;
  analyticsData = j.dump();
  SPDLOG_DEBUG("client analytics data {}", analyticsData);

  return Status::kOk;
}

Status prinsight::serializeParticipantsPublicDataList(
    const std::vector<std::string>& participantsPublicDataList,
    std::string& serializedParticipantsPublicDataList) {
  nlohmann::json j = nlohmann::json::array();
  for (auto clientPublicData : participantsPublicDataList) {
    const auto k = nlohmann::json::parse(clientPublicData);
    j.push_back(k);
  }
  serializedParticipantsPublicDataList = j.dump();
  SPDLOG_DEBUG("json {}", serializedParticipantsPublicDataList);
  return Status::kOk;
}

Status prinsight::getInnerProductAnalysis(
    const std::vector<std::string>& participantsAnalyticsDataList,
    const std::vector<std::int64_t> policy, std::uint64_t bound,
    std::vector<std::pair<std::string, std::uint64_t>>& result) {
  Status status = Status::kUnknownError;
  std::unordered_map<std::string, std::vector<Cipher>> ciphersMap;
  std::vector<FunctionalDecryptionKey> fdKeys;

  SPDLOG_DEBUG("getInnerProductAnalysis");
  std::size_t numParticiants = participantsAnalyticsDataList.size();
  if (0 == numParticiants) {
    return Status::kBadParams;
  }

  // parse json-message and extract the ciphers and functional decryption keys for each participants
  for (auto d : participantsAnalyticsDataList) {
    FunctionalDecryptionKey fdKey;
    Cipher cipher;
    const auto j = nlohmann::json::parse(d);
    AnalyticsData clientData = j;
    SPDLOG_DEBUG("clientData: {}", clientData);

    // extract index
    // FIXME what to do with already visited index
    std::size_t i = clientData.getIndex();
    if (i < 0 || i >= numParticiants) {
      return Status::kBadParams;
    }

    // extract labels and encrypted data
    auto encData = clientData.getEncData();
    for (auto e : encData) {
      if (ciphersMap.find(e.first) == ciphersMap.end()) {
        ciphersMap[e.first].reserve(numParticiants);
      }
      std::string cipherB64 = e.second;
      cipher.fromBase64(cipherB64);
      ciphersMap[e.first].insert(ciphersMap[e.first].begin() + i, cipher);
    }

    // extract functional decryption key
    std::vector<std::string> fdKeyB64 = clientData.getFdKey();
    if (2 != fdKeyB64.size()) {
      return Status::kBadParams;
    }
    fdKey.fromBase64(fdKeyB64);
    fdKeys.insert(fdKeys.begin() + i, fdKey);
  }

  // std::size_t numLabels = ciphersMap.size();
  for (auto c : ciphersMap) {
    auto label = c.first;
    auto ciphers = c.second;
    std::uint64_t r;
    status = DMCFDecryptor::decrypt(ciphers, fdKeys, policy, label, bound, r);
    if (Status::kOk != status) {
      return status;
    }
    result.emplace_back(std::make_pair(label, r));
  }

  return Status::kOk;
}

#if defined(ENABLE_PRINSIGHT_TESTING)

TEST_CASE("e2e encrypt-decrypt test on DMCFE APIs") {
  //
  const std::size_t nClients = 50;
  const std::uint64_t bound = 10000;
  std::vector<DMCFEncryptor> clients;
  std::vector<PublicKey> pubKeys;
  std::vector<Cipher> ciphers;
  std::vector<FunctionalDecryptionKey> decKeys;
  const std::vector<std::int64_t> policy(nClients, 1);

  SPDLOG_DEBUG("create clients");

  for (std::size_t i = 0; i < nClients; i++) {
    auto client = DMCFEncryptor(i, nClients, bound);
    clients.push_back(client);
    pubKeys.push_back(client.publicKey());
  }

  for (std::size_t i = 0; i < nClients; i++) {
    clients[i].setParticipantsPublicKeys(pubKeys);
  }

  std::string label = "test label";

  for (std::size_t i = 0; i < nClients; i++) {
    auto cipher = clients[i].encrypt(label, i * 100);
    ciphers.push_back(cipher);
    auto decKey = clients[i].getFunctionalDecryptionKey(policy);
    decKeys.push_back(decKey);
  }

  SPDLOG_DEBUG("decrypt starts");
  auto result = DMCFDecryptor::decrypt(ciphers, decKeys, policy, label, bound);
  SPDLOG_DEBUG("decrypt ends, result = {}", result);
}

#endif