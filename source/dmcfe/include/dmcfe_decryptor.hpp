#ifndef DMCF_DECRYPTOR
#define DMCF_DECRYPTOR

#include <cstdint>
#include <prinsight/status.hpp>
#include <string>
#include <vector>

#include "dmcfe.hpp"

namespace prinsight {

  class DMCFDecryptor {
  private:
    /* data */
  public:
    static Status decrypt(const std::vector<Cipher> &cipherList,
                          const std::vector<FunctionalDecryptionKey> decryptionKeyList,
                          const std::vector<std::int64_t> &policy, const std::string &label,
                          std::uint64_t bound, std::uint64_t &result);
  };
}  // namespace prinsight
#endif