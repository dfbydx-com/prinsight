#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include "cifer/internal/common.h"
}

#include "include/DMCFE.hpp"

namespace printsight {
  namespace dmcfe {

    G1::G1(const ECP_BN254& key) {
      memset(mBuffer, 0, sizeof(mBuffer));
      octet tmp_oct = {0, sizeof(mBuffer), mBuffer};
      ECP_BN254_toOctet(&tmp_oct, const_cast<ECP_BN254*>(&key), true);
    }

    void G1::toCiferType(ECP_BN254& p) const {
      octet tmp_oct = {0, sizeof(mBuffer), const_cast<char*>(mBuffer)};
      ECP_BN254_fromOctet(&p, &tmp_oct);
    }

    G2::G2(const cfe_vec_G2& v) {
      memset(mPart1, 0, sizeof(mPart1));
      memset(mPart2, 0, sizeof(mPart2));

      if (v.size != 2) {
        return;  // FIXME
      }

      octet tmp_oct = {0, sizeof(mPart1), mPart1};
      ECP2_BN254_toOctet(&tmp_oct, &v.vec[0]);

      tmp_oct.len = 0;
      tmp_oct.max = sizeof(mPart2);
      tmp_oct.val = mPart2;
      ECP2_BN254_toOctet(&tmp_oct, &v.vec[1]);
    }

    void G2::toCiferType(cfe_vec_G2& v) const {
      if (v.size != 2) {
        return;  // FIXME
      }

      octet tmp_oct = {0, sizeof(mPart1), const_cast<char*>(mPart1)};
      ECP2_BN254_fromOctet(&v.vec[0], &tmp_oct);

      tmp_oct.len = 0;
      tmp_oct.max = sizeof(mPart2);
      tmp_oct.val = const_cast<char*>(mPart2);
      ECP2_BN254_fromOctet(&v.vec[1], &tmp_oct);
    }

  }  // namespace dmcfe
}  // namespace printsight