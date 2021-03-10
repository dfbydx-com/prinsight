#include <prinsight/prinsight.h>
#include <spdlog/spdlog.h>
#include <time.h>

#include <cstdio>
#include <iostream>

#include "gmpxx.h"
extern "C" {
#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/sample/uniform.h"
}

using namespace prinsight;

DecMultiClientIPFuncEnc::DecMultiClientIPFuncEnc(std::string _name) : name(std::move(_name)) {
  spdlog::info("Hello, {}!", name);
}

void DecMultiClientIPFuncEnc::smokeTest() {
  const size_t num_clients = 50;
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
  cfe_uniform_sample_vec(&x, bound);
  cfe_uniform_sample_range_vec(&y, bound_neg, bound);
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
