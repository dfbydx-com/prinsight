#ifndef PRINSIGHT_CORE_H

#define PRINSIGHT_CORE_H

#include <prinsight/status.hpp>
#include <string>

namespace prinsight {

  /*
   * get clentIndex, nCient and bound from server::register(clientId)
   * init - index nclient bound
   * getPublicKey
   * post publickey to server::register
   * get participantsPublicKeys from server
   * setParticipantsPublicKeys
   *
   * set labels/count
   * get encrypteddata+fekey
   * post analyticsdata
   */

  class DMCFEncryptor;

  /**
   * @brief Prinsight core APIs
   */
  class Core {
    DMCFEncryptor *mEncryptor;

  public:
    Core();
    ~Core();
    Status getClientRegistrationData(std::string &registrationData);
    Status provideSchemeParamsData(const std::string &schemeParamsData);
    Status getClientPublicData(std::string &clientPublicData);
    Status provideParticipantsPublicData(const std::string &participantsPublicData);
    Status setLabelsValue(const std::string &label, uint64_t value);
    Status getAnalyticsData(std::string &analyticsData);
  };

  static Status getInnerProductAnalysis(const std::string &analyticsData);

}  // namespace prinsight

#endif  // PRINSIGHT_CORE_H
