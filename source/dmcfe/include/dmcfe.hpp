#ifndef __DMCFE_H__
#define __DMCFE_H__

#include <gmpxx.h>

#include <string>
extern "C" {
#include "cifer/data/vec_curve.h"
}

#include <iomanip>
#include <iostream>

namespace prinsight {

  constexpr std::size_t kG1BufferSize = MODBYTES_256_56 + 1;
  constexpr std::size_t kG2BufferSize = 4 * MODBYTES_256_56 + 1;

  class G1 {
  private:
    char mBuffer[kG1BufferSize];

  public:
    G1();
    ~G1();
    explicit G1(const ECP_BN254& key);

    void toCiferType(ECP_BN254& p) const;

    void toBase64(std::string& b64String);
    void fromBase64(const std::string& b64String);

    friend std::ostream& operator<<(std::ostream& os, const G1& p);
  };

  class G2 {
  private:
    char mPart1[kG2BufferSize];
    char mPart2[kG2BufferSize];

  public:
    G2();
    ~G2();
    explicit G2(const cfe_vec_G2& v);

    void toCiferType(cfe_vec_G2& v) const;

    void toBase64(std::vector<std::string>& partsB64);
    void fromBase64(const std::vector<std::string>& partsB64);

    friend std::ostream& operator<<(std::ostream& os, const G2& p);
  };

  using PublicKey = G1;
  using Cipher = G1;
  using FunctionalDecryptionKey = G2;

}  // namespace prinsight
#endif  // __DMCFE_H__