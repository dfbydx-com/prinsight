#include <gmpxx.h>
#include <string.h>

#include <cstdint>
#include <string>
#include <vector>

extern "C" {
#include "cifer/innerprod/fullysec/dmcfe.h"
#include "cifer/internal/common.h"
}

#include "include/dmcfe_decryptor.hpp"
#include "include/utils.hpp"

namespace prinsight {

  std::uint64_t DMCFDecryptor::decrypt(const std::vector<Cipher> &cipherList,
                                       const std::vector<FunctionalDecryptionKey> decryptionKeyList,
                                       const std::vector<std::int64_t> &policy,
                                       const std::string &label, std::uint64_t bound) {
    // check if length of ciphers, keys and policies are same
    // decrypt the inner product with the corresponding label
    const std::size_t num_clients = decryptionKeyList.size();
    cfe_vec_G2 key_shares[num_clients];
    for (std::size_t i = 0; i < num_clients; i++) {
      cfe_vec_G2_init(&key_shares[i], 2);
      decryptionKeyList[i].toCiferType(key_shares[i]);
    }

    ECP_BN254 ciphers[num_clients];
    for (std::size_t i = 0; i < num_clients; i++) {
      cipherList[i].toCiferType(ciphers[i]);
    }

    cfe_vec y;
    cfe_vec_init(&y, policy.size());
    Utils::Int64tVecToCfeVec(policy, &y);

    std::vector<char> writable(label.begin(), label.end());
    writable.push_back('\0');

    mpz_t res, resBound;
    mpz_inits(res, resBound, nullptr);
    mpz_set_si(resBound, bound);

    cfe_error err
        = cfe_dmcfe_decrypt(res, ciphers, key_shares, &writable[0], writable.size(), &y, resBound);

    std::uint64_t result = mpz_get_ui(res);
    mpz_clears(res, resBound, nullptr);

    return result;
  }
}  // namespace prinsight