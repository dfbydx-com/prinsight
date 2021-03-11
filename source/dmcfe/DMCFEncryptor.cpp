#include <gmpxx.h>
#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {

#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/internal/common.h"
}

#include "include/DMCFEncryptor.hpp"
#include "include/Utils.hpp"

namespace printsight {
  namespace dmcfe {

    DMCFEncryptor::DMCFEncryptor(size_t clientId, size_t dataVectorLength, uint64_t bound) {
      _clientId = clientId;
      _dataVectorLength = dataVectorLength;
      _bound = bound;

      // initiate the scheme
      cfe_dmcfe_client *dmcfeClient
          = (cfe_dmcfe_client *)cfe_malloc(sizeof(cfe_dmcfe_client));  // FIXME bad_alloc
      cfe_dmcfe_client_init(dmcfeClient, _clientId);

      // extract public key
      char tmp[MODBYTES_256_56 + 1];
      octet tmp_oct = {0, sizeof(tmp), tmp};
      ECP_BN254_toOctet(&tmp_oct, &dmcfeClient->client_pub_key, true);
      _publicKey = std::string(tmp);

      _dmcfeClient = (void *)dmcfeClient;
    }

    std::string DMCFEncryptor::getPublicKey() { return _publicKey; }

    // CiFEr expects index of every participants including this encryptor should be provided in
    // cfe_dmcfe_set_share call. Although it ignores the public key of this client.
    void DMCFEncryptor::setParticipantsPublicKeys(const std::vector<std::string> &publicKey) {
      char tmp[MODBYTES_256_56 + 1] = {0};
      octet tmp_oct = {0, sizeof(tmp), tmp};

      cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(_dmcfeClient);
      if (NULL == dmcfeClient) {
        return;
      }

      size_t numClients = publicKey.size();
      ECP_BN254 *pubKeyList = (ECP_BN254 *)cfe_malloc(numClients * sizeof(ECP_BN254));

      // convert std::string->c-strig->octet->ECP_BN254
      for (size_t i = 0; i < numClients; i++) {
        strncpy(tmp, publicKey[i].c_str(), sizeof(tmp));
        ECP_BN254_fromOctet(&pubKeyList[i], &tmp_oct);
      }

      cfe_dmcfe_set_share(dmcfeClient, pubKeyList, numClients);
    }

    std::string DMCFEncryptor::encrypt(int64_t data, const std::string &label) {
      ECP_BN254 cipher;
      mpz_t x;
      mpz_init_set_ui(x, data);

      cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(_dmcfeClient);
      if (NULL == dmcfeClient) {
        return NULL;
      }

      // CiFEr expects label param as char* not const char*
      // cfe_dmcfe_encrypt(&cipher, dmcfeClient, x, label.c_str(), label.length());
      // https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
      std::vector<char> writable(label.begin(), label.end());
      writable.push_back('\0');
      cfe_dmcfe_encrypt(&cipher, dmcfeClient, x, &writable[0], writable.size());

      char tmp[MODBYTES_256_56 + 1] = {0};
      octet tmp_oct = {0, sizeof(tmp), tmp};
      ECP_BN254_toOctet(&tmp_oct, &cipher, true);
      return std::string(tmp);
    }

    FunctionalDecryptionKey DMCFEncryptor::getFunctionalDecryptionKey(
        const std::vector<int64_t> &policy) {
      cfe_vec tempPolicy;
      cfe_vec_G2 decryptionKey;

      cfe_vec_init(&tempPolicy, policy.size());

      cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(_dmcfeClient);
      // if (NULL == dmcfeClient) {  // FIXME
      //   return error;
      // }

      Utils::Int64tVecToCfeVec(policy, &tempPolicy);

      cfe_dmcfe_fe_key_part_init(&decryptionKey);
      cfe_dmcfe_derive_fe_key_part(&decryptionKey, dmcfeClient, &tempPolicy);
      return FunctionalDecryptionKey(decryptionKey);
    }

    DMCFEncryptor::~DMCFEncryptor() {}
  }  // namespace dmcfe
}  // namespace printsight