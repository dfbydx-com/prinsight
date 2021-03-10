#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdint>
#include <vector>

#include "cifer/data/vec.h"

class Utils {
public:
  // cfe_vec should be already initialized with from.length() size
  static void Int64tVecToCfeVec(std::vector<int64_t>& from, cfe_vec& to);
  static void G2VecToStringVec(const cfe_vec_G2* vec, std::vector<string>);
};

#endif  // __UTILS_H__