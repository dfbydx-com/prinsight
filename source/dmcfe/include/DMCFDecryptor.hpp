#ifndef DMCF_DECRYPTOR
#define DMCF_DECRYPTOR

#include <cstdint>
#include <string>
#include <vector>

#include "DMCFE.hpp"

namespace printsight {
  namespace dmcfe {

    class DMCFDecryptor {
    private:
      /* data */
    public:
      static uint64_t decrypt(const std::vector<std::string> &cipherList,
                              const std::vector<FunctionalDecryptionKey> decryptionKeyList,
                              const std::vector<int64_t> &policy, const std::string &label,
                              uint64_t bound);
    };
  }  // namespace dmcfe
}  // namespace printsight
#endif