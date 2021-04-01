#include <gmpxx.h>
#include <spdlog/spdlog.h>
#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {

#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/internal/common.h"
}

#include "include/dmcfe.hpp"
#include "include/dmcfe_encryptor.hpp"
#include "include/utils.hpp"

namespace prinsight {

  DMCFEncryptor::DMCFEncryptor(std::size_t clientId, std::size_t dataVectorLength,
                               std::uint64_t bound) {
    mClientId = clientId;
    mDataVectorLength = dataVectorLength;
    mBound = bound;

    // initiate the scheme
    cfe_dmcfe_client *dmcfeClient
        = (cfe_dmcfe_client *)cfe_malloc(sizeof(cfe_dmcfe_client));  // FIXME bad_alloc
    cfe_dmcfe_client_init(dmcfeClient, mClientId);

    // extract public key
    mPublicKey = PublicKey(dmcfeClient->client_pub_key);
    mDmcfeClient = (void *)dmcfeClient;
  }

  std::size_t DMCFEncryptor::clientId() const { return mClientId; }

  PublicKey DMCFEncryptor::publicKey() const { return mPublicKey; }

  std::size_t DMCFEncryptor::dataVectorLength() const { return mDataVectorLength; }

  // CiFEr expects index of every participants including this encryptor should be provided in
  // cfe_dmcfe_set_share call. Although it ignores the public key of this client.
  Status DMCFEncryptor::setParticipantsPublicKeys(const std::vector<PublicKey> &publicKeyList) {
    cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(mDmcfeClient);
    if (nullptr == dmcfeClient) {
      return Status::kBadState;
    }

    std::size_t num_clients = publicKeyList.size();
    ECP_BN254 *pub_keys = (ECP_BN254 *)cfe_malloc(num_clients * sizeof(ECP_BN254));
    if (nullptr == pub_keys) {
      return Status::kOutOfMemory;
    }

    for (std::size_t i = 0; i < num_clients; i++) {
      publicKeyList[i].toCiferType(pub_keys[i]);
    }

    cfe_dmcfe_set_share(dmcfeClient, pub_keys, num_clients);
    return Status::kOk;
  }

  Status DMCFEncryptor::encrypt(const std::string &label, std::int64_t data, Cipher &cipher) {
    ECP_BN254 enc;
    mpz_t x;
    mpz_init_set_ui(x, data);

    cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(mDmcfeClient);
    if (nullptr == dmcfeClient) {
      return Status::kBadState;
    }

    // CiFEr expects label param as char* not const char*
    // cfe_dmcfe_encrypt(&cipher, dmcfeClient, x, label.c_str(), label.length());
    // https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
    std::vector<char> writable(label.begin(), label.end());
    writable.push_back('\0');
    cfe_dmcfe_encrypt(&enc, dmcfeClient, x, &writable[0], writable.size());
    cipher = Cipher(enc);
    return Status::kOk;
  }

  Status DMCFEncryptor::getFunctionalDecryptionKey(
      const std::vector<std::int64_t> &policy, FunctionalDecryptionKey &functionalDecryptionKey) {
    cfe_vec tempPolicy;
    cfe_vec_G2 decryptionKey;

    cfe_vec_init(&tempPolicy, policy.size());

    cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(mDmcfeClient);
    if (nullptr == dmcfeClient) {
      return Status::kBadState;
    }

    Utils::Int64tVecToCfeVec(policy, &tempPolicy);

    cfe_dmcfe_fe_key_part_init(&decryptionKey);
    cfe_dmcfe_derive_fe_key_part(&decryptionKey, dmcfeClient, &tempPolicy);
    functionalDecryptionKey = FunctionalDecryptionKey(decryptionKey);
    return Status::kOk;
  }

  DMCFEncryptor::~DMCFEncryptor() {
    cfe_dmcfe_client *dmcfeClient = (cfe_dmcfe_client *)(mDmcfeClient);
    if (nullptr == dmcfeClient) {  // FIXME
      free(dmcfeClient);
    }
  }
}  // namespace prinsight