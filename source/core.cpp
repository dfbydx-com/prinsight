#include <gmpxx.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <prinsight/prinsight.hpp>
extern "C" {
#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/sample/uniform.h"
}

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
Core::~Core() {}

Status Core::getRegistrationData(std::string& registrationData) {
  ClientRegistrationModel c(Utils::randomString(16));
  nlohmann::json j = c;
  registrationData = j.dump();
  spdlog::info("client registration data {}", registrationData);
  return Status::kOk;
}

Status Core::initializeAnalyticsScheme(const std::string& schemeParamsData) {
  const auto j = nlohmann::json::parse(schemeParamsData);
  SchemeParamsModel s = j.get<SchemeParamsModel>();
  spdlog::info("received {}", s);
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
  pubKey = mEncryptor->getPublicKey();
  pubKey.toBase64(pubKeyB64);
  pubData = ClientPublicDataModel(mEncryptor->getClientId(), pubKeyB64);
  j = pubData;
  publicData = j.dump();

  spdlog::info("client public data {}", publicData);

  return Status::kOk;
}

Status Core::provideParticipantsPublicData(const std::string& participantsPublicData) {
  std::vector<PublicKey> pubKeyList;

  if (nullptr == mEncryptor) {
    return Status::kSchemeUninitialized;
  }

  spdlog::info("input: {}", participantsPublicData);
  const auto j = nlohmann::json::parse(participantsPublicData);
  std::vector<ClientPublicDataModel> clientPublicDataList
      = j.get<std::vector<ClientPublicDataModel>>();

  std::size_t currentParticipantsCount = clientPublicDataList.size();
  std::size_t initializedParticipantsCount = mEncryptor->getDataVectorLength();
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

    std::string pubKeyB64 = c.getPublicKey();
    p.fromBase64(pubKeyB64);
    pubKeyList.insert(pubKeyList.begin() + i, p);
  }

  mEncryptor->setParticipantsPublicKeys(pubKeyList);

  return Status::kOk;
}

Status Core::setClearAnalyticsData(const std::string& label, std::uint64_t value) {
  mAnalyticsData[label] = value;
  return Status::kOk;
}

Status Core::getEncryptedAnalyticsData(std::string& analyticsData) {
  std::unordered_map<std::string, std::string> encData;
  std::vector<std::string> fdKey;

  if (nullptr == mEncryptor) {
    return Status::kSchemeUninitialized;
  }

  // encrypt the data
  for (auto d : mAnalyticsData) {
    std::string cipherB64;
    auto cipher = mEncryptor->encrypt(d.first, d.second);
    // base64 conversion
    cipher.toBase64(cipherB64);
    // store label and encrypted data
    encData[d.first] = cipherB64;
  }

  // get the functional decryption key
  // FIXME, support for arbitrary policy/weights
  const std::vector<std::int64_t> policy(mEncryptor->getDataVectorLength(), 1);
  auto decKey = mEncryptor->getFunctionalDecryptionKey(policy);
  // base64 conversion
  decKey.toBase64(fdKey);

  // json serialization
  AnalyticsData a(mEncryptor->getClientId(), encData, fdKey);
  nlohmann::json j = a;
  analyticsData = j.dump();
  spdlog::info("client analytics data {}", analyticsData);

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
  spdlog::info("json {}", serializedParticipantsPublicDataList);
  return Status::kOk;
}

Status prinsight::getInnerProductAnalysis(
    const std::vector<std::string>& participantsAnalyticsDataList,
    const std::vector<std::int64_t> policy, std::uint64_t bound,
    std::vector<std::pair<std::string, std::uint64_t>>& result) {
  std::unordered_map<std::string, std::vector<Cipher>> ciphersMap;
  std::vector<FunctionalDecryptionKey> fdKeys;

  spdlog::info("getInnerProductAnalysis");
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
    spdlog::info("clientData: {}", clientData);

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
    /*
        spdlog::info("decrypt for label {}", label);
        for (auto fk : fdKeys) {
          std::cout << "fdKeys " << fk << std::endl;
        }
        for (auto ci : ciphers) {
          std::cout << "ciphers " << ci << std::endl;
        } */

    auto r = DMCFDecryptor::decrypt(ciphers, fdKeys, policy, label, bound);

    // spdlog::info("result {}", r);

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

  spdlog::info("create clients");

  for (std::size_t i = 0; i < nClients; i++) {
    auto client = DMCFEncryptor(i, nClients, bound);
    clients.push_back(client);
    pubKeys.push_back(client.getPublicKey());
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

  spdlog::info("decrypt starts");
  auto result = DMCFDecryptor::decrypt(ciphers, decKeys, policy, label, bound);
  spdlog::info("decrypt ends, result = {}", result);
}

TEST_CASE("e2e encrypt-decrypt test on cifer APIs") {
  const std::size_t num_clients = 5;
  mpz_t bound, bound_neg, xy_check, xy;
  mpz_inits(bound, bound_neg, xy_check, xy, nullptr);
  mpz_set_ui(bound, 10000);
  // mpz_pow_ui(bound, bound, 10);
  mpz_neg(bound_neg, bound);

  // create clients and make an array of their public keys
  cfe_dmcfe_client clients[num_clients];
  ECP_BN254 pub_keys[num_clients];
  for (std::size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_client_init(&(clients[i]), i);
    pub_keys[i] = clients[i].client_pub_key;
  }

  // based on public values of each client create private matrices T_i summing
  // to 0
  for (std::size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_set_share(&(clients[i]), pub_keys, num_clients);
  }

  // now that the clients have agreed on secret keys they can encrypt a vector
  // in a decentralized way and create partial keys such that only with all of
  // them the decryption of the inner product is possible
  cfe_vec x, y;
  cfe_vec_inits(num_clients, &x, &y, nullptr);
  // cfe_uniform_sample_vec(&x, bound);
  // cfe_uniform_sample_range_vec(&y, bound_neg, bound);
  mpz_t temp, one;
  mpz_inits(temp, one, nullptr);
  mpz_set_ui(one, 1);
  for (std::size_t i = 0; i < num_clients; i++) {
    mpz_set_ui(temp, i * 10);
    cfe_vec_set(&x, temp, i);
    cfe_vec_set(&y, one, i);
  }
  char label[] = "some label";
  std::size_t label_len = 10;  // length of the label string
  ECP_BN254 ciphers[num_clients];
  cfe_vec_G2 fe_key[num_clients];

  for (std::size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_encrypt(&(ciphers[i]), &(clients[i]), x.vec[i], label, label_len);
    cfe_dmcfe_fe_key_part_init(&(fe_key[i]));
    cfe_dmcfe_derive_fe_key_part(&(fe_key[i]), &(clients[i]), &y);
  }

  // decrypt the inner product with the corresponding label
  printf("\ndecrypt starts");
  cfe_error err = cfe_dmcfe_decrypt(xy, ciphers, fe_key, label, label_len, &y, bound);
  printf("\ndecrypt ends %x", err);

  // check correctness
  cfe_vec_dot(xy_check, &x, &y);

  printf("\nvector x = [");
  for (std::size_t i = 0; i < num_clients; i++) {
    gmp_printf("%Zd ", x.vec[i]);
  }
  printf("]\n");

  printf("\nvector y = [");
  for (std::size_t i = 0; i < num_clients; i++) {
    gmp_printf("%Zd  ", y.vec[i]);
  }
  printf("]\n");

  gmp_printf(
      "\nThe inner product of a random encrypted vector x and y = [1, "
      "1,...,1] is %Zd\n",
      xy);
  // free the memory
  mpz_clears(bound, bound_neg, xy_check, xy, nullptr);
  for (std::size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_client_free(&(clients[i]));
    cfe_vec_G2_free(&(fe_key[i]));
  }
  cfe_vec_frees(&x, &y, nullptr);

  return;
}
#endif