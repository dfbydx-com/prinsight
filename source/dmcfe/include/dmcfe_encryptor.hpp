#ifndef DMCF_ENCRYPTOR
#define DMCF_ENCRYPTOR

#include <cstdint>
#include <string>
#include <vector>

#include "dmcfe.hpp"

namespace prinsight {

  class DMCFEncryptor {
  private:
    std::size_t mClientId;
    std::size_t mDataVectorLength;
    std::uint64_t mBound;
    void *mDmcfeClient;
    PublicKey mPublicKey;

  public:
    DMCFEncryptor(std::size_t clientId, std::size_t dataVectorLength, std::uint64_t bound);

    PublicKey getPublicKey() const;

    std::size_t getClientId() const;

    std::size_t getDataVectorLength() const;

    void setParticipantsPublicKeys(const std::vector<PublicKey> &publicKeyList);

    Cipher encrypt(std::int64_t data, const std::string &label);

    FunctionalDecryptionKey getFunctionalDecryptionKey(const std::vector<std::int64_t> &policy);

    ~DMCFEncryptor();
  };

}  // namespace prinsight
#endif