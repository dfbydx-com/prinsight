#include <gmpxx.h>
#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/internal/common.h"
}

#include "include/DMCFDecryptor.hpp"
#include "include/Utils.hpp"

namespace printsight {
  namespace dmcfe {

    uint64_t DMCFDecryptor::decrypt(const std::vector<std::string> &cipherList,
                                    const std::vector<FunctionalDecryptionKey> decryptionKeyList,
                                    const std::vector<int64_t> &policy, const std::string &label,
                                    uint64_t bound) {
      // check if length of ciphers, keys and policies are same
      // decrypt the inner product with the corresponding label
      const size_t numClients = decryptionKeyList.size();
      cfe_vec_G2 key_shares[numClients];
      for (size_t i = 0; i < numClients; i++) {
        cfe_vec_G2_init(&key_shares[i], 2);
        decryptionKeyList[i].toCfeVecG2(key_shares[i]);
      }

      char tmp[2 * MODBYTES_256_56 + 1] = {0};
      octet tmp_oct = {0, sizeof(tmp), tmp};
      ECP_BN254 ciphers[numClients];
      for (size_t i = 0; i < numClients; i++) {
        strncpy(tmp, cipherList[i].c_str(), sizeof(tmp));
        ECP_BN254_fromOctet(&ciphers[i], &tmp_oct);
      }

      cfe_vec y;
      cfe_vec_init(&y, policy.size());
      Utils::Int64tVecToCfeVec(policy, &y);

      std::vector<char> writable(label.begin(), label.end());
      writable.push_back('\0');

      mpz_t res, resBound;
      mpz_inits(res, resBound, NULL);
      mpz_set_si(resBound, bound);

      cfe_error err = cfe_dmcfe_decrypt(res, ciphers, key_shares, &writable[0], writable.size(), &y,
                                        resBound);

      uint64_t result = mpz_get_ui(res);
      mpz_clears(res, resBound, NULL);

      return result;
    }

  }  // namespace dmcfe
}  // namespace printsight