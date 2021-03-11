#ifndef __DMCFE_H__
#define __DMCFE_H__

#include <gmpxx.h>

#include <string>
extern "C" {
#include "cifer/data/vec_curve.h"
}

namespace printsight {
  namespace dmcfe {

    class FunctionalDecryptionKey {
      std::string key[2];

    public:
      FunctionalDecryptionKey(std::string key[2]);
      FunctionalDecryptionKey(const cfe_vec_G2& v);
      void toCfeVecG2(cfe_vec_G2& v) const;
    };
  }  // namespace dmcfe
}  // namespace printsight
#endif  // __DMCFE_H__