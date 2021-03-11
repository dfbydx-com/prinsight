#ifndef __UTILS_H__
#define __UTILS_H__

#include <gmpxx.h>

#include <cstdint>
#include <vector>

extern "C" {
#include "cifer/data/vec.h"
}

namespace printsight {
  namespace dmcfe {

    class Utils {
    public:
      // cfe_vec should be already initialized with from.length() size
      static void Int64tVecToCfeVec(const std::vector<int64_t>& from, cfe_vec* to);
    };
  }  // namespace dmcfe
}  // namespace printsight
#endif  // __UTILS_H__