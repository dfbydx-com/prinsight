#include <gmpxx.h>
#include <spdlog/spdlog.h>
#include <time.h>

#include <cstdio>
#include <iostream>
#include <prinsight/prinsight.hpp>
extern "C" {
#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/sample/uniform.h"
}

#include "dmcfe/include/dmcfe_decryptor.hpp"
#include "dmcfe/include/dmcfe_encryptor.hpp"

#if defined(ENABLE_PRINSIGHT_TESTING)
#  include <doctest/doctest.h>
#endif

using namespace prinsight;

Core::Core() { mEncryptor = new DMCFEncryptor(0, 0, 0); }
Core::~Core() {}

#if defined(ENABLE_PRINSIGHT_TESTING)

TEST_CASE("e2e encrypt-decrypt test on DMCFE APIs") {
  //
  const size_t nClients = 50;
  const uint64_t bound = 10000;
  std::vector<DMCFEncryptor> clients;
  std::vector<PublicKey> pubKeys;
  std::vector<Cipher> ciphers;
  std::vector<FunctionalDecryptionKey> decKeys;
  const std::vector<int64_t> policy(nClients, 1);

  spdlog::info("create clients");

  for (size_t i = 0; i < nClients; i++) {
    auto client = DMCFEncryptor(i, nClients, bound);
    clients.push_back(client);
    pubKeys.push_back(client.getPublicKey());
  }

  for (size_t i = 0; i < nClients; i++) {
    clients[i].setParticipantsPublicKeys(pubKeys);
  }

  std::string label = "test label";

  for (size_t i = 0; i < nClients; i++) {
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
  const size_t num_clients = 5;
  mpz_t bound, bound_neg, xy_check, xy;
  mpz_inits(bound, bound_neg, xy_check, xy, NULL);
  mpz_set_ui(bound, 10000);
  // mpz_pow_ui(bound, bound, 10);
  mpz_neg(bound_neg, bound);

  // create clients and make an array of their public keys
  cfe_dmcfe_client clients[num_clients];
  ECP_BN254 pub_keys[num_clients];
  for (size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_client_init(&(clients[i]), i);
    pub_keys[i] = clients[i].client_pub_key;
  }

  // based on public values of each client create private matrices T_i summing
  // to 0
  for (size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_set_share(&(clients[i]), pub_keys, num_clients);
  }

  // now that the clients have agreed on secret keys they can encrypt a vector
  // in a decentralized way and create partial keys such that only with all of
  // them the decryption of the inner product is possible
  cfe_vec x, y;
  cfe_vec_inits(num_clients, &x, &y, NULL);
  // cfe_uniform_sample_vec(&x, bound);
  // cfe_uniform_sample_range_vec(&y, bound_neg, bound);
  mpz_t temp, one;
  mpz_inits(temp, one, NULL);
  mpz_set_ui(one, 1);
  for (size_t i = 0; i < num_clients; i++) {
    mpz_set_ui(temp, i * 10);
    cfe_vec_set(&x, temp, i);
    cfe_vec_set(&y, one, i);
  }
  char label[] = "some label";
  size_t label_len = 10;  // length of the label string
  ECP_BN254 ciphers[num_clients];
  cfe_vec_G2 fe_key[num_clients];

  for (size_t i = 0; i < num_clients; i++) {
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
  for (size_t i = 0; i < num_clients; i++) {
    gmp_printf("%Zd ", x.vec[i]);
  }
  printf("]\n");

  printf("\nvector y = [");
  for (size_t i = 0; i < num_clients; i++) {
    gmp_printf("%Zd  ", y.vec[i]);
  }
  printf("]\n");

  gmp_printf(
      "\nThe inner product of a random encrypted vector x and y = [1, "
      "1,...,1] is %Zd\n",
      xy);
  // free the memory
  mpz_clears(bound, bound_neg, xy_check, xy, NULL);
  for (size_t i = 0; i < num_clients; i++) {
    cfe_dmcfe_client_free(&(clients[i]));
    cfe_vec_G2_free(&(fe_key[i]));
  }
  cfe_vec_frees(&x, &y, NULL);

  return;
}
#endif