#ifndef __DMCFE_H__
#define __DMCFE_H__

#include <gmpxx.h>

#include <string>
extern "C" {
#include "cifer/data/vec_curve.h"
}

#include <iomanip>
#include <iostream>

namespace printsight {
  namespace dmcfe {

    class G1 {
    private:
      char mBuffer[MODBYTES_256_56 + 1];

    public:
      G1(const ECP_BN254& key);
      void toCiferType(ECP_BN254& p) const;
      friend std::ostream& operator<<(std::ostream& os, const G1& p) {
        os << "[ 0x";
        for (auto c : p.mBuffer) {
          os << std::setw(2) << std::setfill('0') << std::hex << (int)c;
        }
        os << " ]";
        return os;
      }
    };

    class G2 {
    private:
      char mPart1[4 * MODBYTES_256_56 + 1];
      char mPart2[4 * MODBYTES_256_56 + 1];

    public:
      G2(const cfe_vec_G2& v);
      void toCiferType(cfe_vec_G2& v) const;
      friend std::ostream& operator<<(std::ostream& os, const G2& p) {
        os << "[ 0x";
        for (auto c : p.mPart1) {
          os << std::setw(2) << std::setfill('0') << std::hex << (int)c;
        }

        os << ", ";
        for (auto c : p.mPart2) {
          os << std::setw(2) << std::setfill('0') << std::hex << (int)c;
        }
        os << " ]";
        return os;
      }
    };

    using PublicKey = G1;
    using Cipher = G1;
    using FunctionalDecryptionKey = G2;

  }  // namespace dmcfe
}  // namespace printsight
#endif  // __DMCFE_H__