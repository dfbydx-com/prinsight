#ifndef __UTILS_H__
#define __UTILS_H__

#include <gmpxx.h>

#include <cstdint>
#include <vector>

extern "C" {
#include "cifer/data/vec.h"
}

namespace prinsight {

  class Utils {
  public:
    // cfe_vec should be already initialized with from.length() size
    static void Int64tVecToCfeVec(const std::vector<std::int64_t>& from, cfe_vec* to);
    static std::string randomString(std::size_t length);

    static void zeroize(void* buffer, std::size_t len);
  };

}  // namespace prinsight
#endif  // __UTILS_H__