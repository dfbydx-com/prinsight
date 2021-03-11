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

    FunctionalDecryptionKey::FunctionalDecryptionKey(std::string keyParam[2]) {
      key[0] = keyParam[0];
      key[1] = keyParam[1];
    }

    FunctionalDecryptionKey::FunctionalDecryptionKey(const cfe_vec_G2& v) {
      char tmp[4 * MODBYTES_256_56 + 1] = {0};
      octet tmp_oct = {0, sizeof(tmp), tmp};

      if (v.size != 2) {
        return;  // FIXME
      }

      for (size_t i = 0; i < 2; i++) {
        ECP2_BN254_toOctet(&tmp_oct, &v.vec[i]);
        key[i] = std::string(tmp); /* code */
      }
    }

    void FunctionalDecryptionKey::toCfeVecG2(cfe_vec_G2& v) const {
      char tmp[4 * MODBYTES_256_56 + 1] = {0};
      octet tmp_oct = {0, sizeof(tmp), tmp};

      for (size_t i = 0; i < 2; i++) {
        strncpy(tmp, key[i].c_str(), sizeof(tmp));
        ECP2_BN254_fromOctet(&v.vec[i], &tmp_oct);
      }
    }
  }  // namespace dmcfe
}  // namespace printsight