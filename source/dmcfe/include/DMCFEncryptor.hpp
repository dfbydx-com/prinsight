#ifndef DMCF_ENCRYPTOR
#define DMCF_ENCRYPTOR

#include <cstdint>
#include <string>
#include <vector>

#include "DMCFE.hpp"

namespace printsight {
  namespace dmcfe {

    class DMCFEncryptor {
    private:
      size_t _clientId;
      size_t _dataVectorLength;
      uint64_t _bound;
      void *_dmcfeClient;
      std::string _publicKey;

    public:
      DMCFEncryptor(size_t clientId, size_t dataVectorLength, uint64_t bound);
      std::string getPublicKey();
      void setParticipantsPublicKeys(const std::vector<std::string> &publicKey);
      std::string encrypt(int64_t data, const std::string &label);
      FunctionalDecryptionKey getFunctionalDecryptionKey(const std::vector<int64_t> &policy);
      ~DMCFEncryptor();
    };
  }  // namespace dmcfe
}  // namespace printsight
#endif