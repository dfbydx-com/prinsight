#ifndef __UTILS_H__
#define __UTILS_H__

#include <gmpxx.h>

#include <cstdint>
#include <vector>

extern "C" {
#include "cifer/data/vec.h"
}

class Utils {
public:
  // cfe_vec should be already initialized with from.length() size
  static void Int64tVecToCfeVec(const std::vector<int64_t>& from, cfe_vec* to);
};

#endif  // __UTILS_H__