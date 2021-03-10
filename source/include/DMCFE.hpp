#ifndef __DMCFE_H__
#define __DMCFE_H__

#include <string>

extern "C" {
#include "cifer/data/vec_curve.h"
}

class FunctionalDecryptionKey {
  std::string key[2];

public:
  FunctionalDecryptionKey(std::string key[2]);
  FunctionalDecryptionKey(const cfe_vec_G2& v);
};

#endif  // __DMCFE_H__