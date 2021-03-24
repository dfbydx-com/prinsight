#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include "base64.h"
#include "cifer/internal/common.h"
}

#include "include/dmcfe.hpp"
#include "include/utils.hpp"

namespace prinsight {
  G1::G1() { Utils::zeroize(mBuffer, sizeof(mBuffer)); }

  G1::~G1() { Utils::zeroize(mBuffer, sizeof(mBuffer)); }

  G1::G1(const ECP_BN254& key) {
    octet tmp_oct = {0, sizeof(mBuffer), mBuffer};
    ECP_BN254_toOctet(&tmp_oct, const_cast<ECP_BN254*>(&key), true);
  }

  void G1::toCiferType(ECP_BN254& p) const {
    octet tmp_oct = {0, sizeof(mBuffer), const_cast<char*>(mBuffer)};
    ECP_BN254_fromOctet(&p, &tmp_oct);
  }

  void G1::toBase64(std::string& b64String) {
    const std::size_t b64BufferLength = BASE64_ENCODE_OUT_SIZE(kG1BufferSize);
    char b64Buffer[b64BufferLength] = {0};
    base64_encode((const unsigned char*)mBuffer, sizeof(mBuffer), b64Buffer);
    b64String = std::string(b64Buffer);
  }

  void G1::fromBase64(const std::string& b64String) {
    base64_decode(&(b64String[0]), b64String.length(), (unsigned char*)mBuffer);  // FIXME
  }

  std::ostream& operator<<(std::ostream& os, const G1& p) {
    os << "[ 0x";
    for (auto c : p.mBuffer) {
      os << std::setw(2) << std::setfill('0') << std::hex << (int)c;
    }
    os << " ]";
    return os;
  }

  G2::G2() {
    Utils::zeroize(mPart1, sizeof(mPart1));
    Utils::zeroize(mPart2, sizeof(mPart2));
  }

  G2::~G2() {
    Utils::zeroize(mPart1, sizeof(mPart1));
    Utils::zeroize(mPart2, sizeof(mPart2));
  }

  G2::G2(const cfe_vec_G2& v) {
    Utils::zeroize(mPart1, sizeof(mPart1));
    Utils::zeroize(mPart2, sizeof(mPart2));

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

  void G2::toBase64(std::vector<std::string>& partsB64) {
    const std::size_t b64BufferLength = BASE64_ENCODE_OUT_SIZE(kG2BufferSize);
    char b64Buffer[b64BufferLength] = {0};
    base64_encode((const unsigned char*)mPart1, sizeof(mPart1), mPart1);
    partsB64.push_back(std::string(b64Buffer));
    base64_encode((const unsigned char*)mPart2, sizeof(mPart2), mPart2);
    partsB64.push_back(std::string(b64Buffer));
  }

  void G2::fromBase64(const std::vector<std::string>& partsB64) {
    if (2 != partsB64.size()) return;  // FIXME
    base64_decode(&(partsB64[0][0]), partsB64[0].length(), (unsigned char*)mPart1);
    base64_decode(&(partsB64[1][0]), partsB64[1].length(), (unsigned char*)mPart2);
  }

  std::ostream& operator<<(std::ostream& os, const G2& p) {
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
}  // namespace prinsight