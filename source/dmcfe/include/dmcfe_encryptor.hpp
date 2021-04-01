#ifndef DMCF_ENCRYPTOR
#define DMCF_ENCRYPTOR

#include <cstdint>
#include <prinsight/status.hpp>
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

    PublicKey publicKey() const;

    std::size_t clientId() const;

    std::size_t dataVectorLength() const;

    Status setParticipantsPublicKeys(const std::vector<PublicKey> &publicKeyList);

    Status encrypt(const std::string &label, std::int64_t data, Cipher &cipher);

    Status getFunctionalDecryptionKey(const std::vector<std::int64_t> &policy,
                                      FunctionalDecryptionKey &functionalDecryptionKey);

    ~DMCFEncryptor();
  };

}  // namespace prinsight
#endif