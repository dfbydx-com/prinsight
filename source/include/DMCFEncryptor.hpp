#ifndef DMCF_ENCRYPTOR
#define DMCF_ENCRYPTOR

#include <cstdint>
#include <string>
#include <vector>

#include "DMCFE.hpp"

class DMCFEncryptor {
private:
  size_t _clientId;
  size_t _dataVectorLength;
  std::string _bound;
  size_t _modulusLen;
  void **_dmcfeClient;
  std::string _publicKey;

public:
  DMCFEncryptor(size_t clientId, size_t dataVectorLength, size_t modulusLen,
                const std::string &bound);
  std::string getPublicKey();
  void setParticipantsPublicKeys(const std::vector<std::string> &publicKey);
  std::string encrypt(int64_t data, const std::string &label);
  FunctionalDecryptionKey getFunctionalDecryptionKey(const std::vector<int64_t> &policy);
  ~DMCFEncryptor();
};

#endif