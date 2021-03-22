#ifndef DMCF_ENCRYPTOR
#define DMCF_ENCRYPTOR

#include <cstdint>
#include <string>
#include <vector>

#include "dmcfe.hpp"

namespace prinsight {

  class DMCFEncryptor {
  private:
    size_t mClientId;
    size_t mDataVectorLength;
    uint64_t mBound;
    void *mDmcfeClient;
    PublicKey *mPublicKey;

  public:
    DMCFEncryptor(size_t clientId, size_t dataVectorLength, uint64_t bound);
    PublicKey getPublicKey() const;
    void setParticipantsPublicKeys(const std::vector<PublicKey> &publicKeyList);
    Cipher encrypt(int64_t data, const std::string &label);
    FunctionalDecryptionKey getFunctionalDecryptionKey(const std::vector<int64_t> &policy);
    ~DMCFEncryptor();
  };
}  // namespace prinsight
#endif