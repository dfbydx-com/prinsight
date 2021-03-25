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
#include "models/client_public_data_model.hpp"
#include "models/client_registration_model.hpp"
#include "models/scheme_params_model.hpp"

#if defined(ENABLE_PRINSIGHT_TESTING)
#  include <doctest/doctest.h>
#endif

using namespace prinsight;

Core::Core() {
  /*   PublicKey pk = mEncryptor->getPublicKey();
    std::cout << pk << std::endl;

    std::string str64, dec;
    pk.toBase64(str64);
    std::cout << std::endl << str64 << std::endl;

    PublicKey pk2;
    pk2.fromBase64(str64);
    std::cout << pk2 << std::endl; */

  /*   dec = Utils::b64decode(str64);
    char cstr[dec.size() + 1];
    dec.copy(cstr, dec.size() + 1);
    cstr[dec.size()] = '\0';
    // std::cout << cstr << std::endl;
    for (std::size_t i = 0; i < dec.size(); i++) {
      printf("%02hhx", cstr[i]);
    } */

  // FIXME   gen random client id
  mEncryptor = nullptr;
}
Core::~Core() {}

Status Core::getClientRegistrationData(std::string& registrationData) {
  ClientRegistrationModel c(Utils::randomString(16));
  nlohmann::json j = c;
  registrationData = j.dump();
  spdlog::info("client registration data {}", registrationData);
  return Status::kOk;
}

Status Core::initializeClientAnalyticsScheme(const std::string& schemeParamsData) {
  const auto j = nlohmann::json::parse(schemeParamsData);
  SchemeParamsModel s = j.get<SchemeParamsModel>();
  spdlog::info("received {}", s);
  mEncryptor = new DMCFEncryptor(s.getIndex(), s.getParticipantsCount(), s.getBound());

  return Status::kOk;
}

Status Core::getClientPublicData(std::string& clientPublicData) {
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
  clientPublicData = j.dump();

  spdlog::info("client public data {}", clientPublicData);

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
  std::size_t initialParticipantsCount = mEncryptor->getDataVectorLength();
  if (currentParticipantsCount != initialParticipantsCount) {
    return Status::kBadParams;
  }

  pubKeyList.reserve(currentParticipantsCount);
  // server gives the list of pub-keys of all clients including this one
  // we need to make sure pub-key list passed to cifer is correctly indexed
  for (auto c : clientPublicDataList) {
    PublicKey p;

    std::size_t i = c.getIndex();
    if (i >= initialParticipantsCount) {
      return Status::kBadParams;
    }

    std::string pubKeyB64 = c.getPublicKey();
    p.fromBase64(pubKeyB64);
    pubKeyList.insert(pubKeyList.begin() + i, p);
  }

  mEncryptor->setParticipantsPublicKeys(pubKeyList);

  return Status::kOk;
}

Status Core::setLabelsValue(const std::string& label, std::uint64_t value) { return Status::kOk; }

Status Core::getClientAnalyticsData(std::string& analyticsData) { return Status::kOk; }

Status prinsight::serializeClientPublicDataList(
    const std::vector<std::string>& clientPublicDataList,
    std::string& serializedClientPublicDataList) {
  nlohmann::json j = nlohmann::json::array();
  for (auto clientPublicData : clientPublicDataList) {
    const auto k = nlohmann::json::parse(clientPublicData);
    j.push_back(k);
  }
  serializedClientPublicDataList = j.dump();
  spdlog::info("json {}", serializedClientPublicDataList);
  return Status::kOk;
}

Status getInnerProductAnalysis(const std::string& analyticsData) { return Status::kOk; }

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
    auto cipher = clients[i].encrypt(i * 100, label);
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