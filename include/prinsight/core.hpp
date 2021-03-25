#ifndef PRINSIGHT_CORE_H

#define PRINSIGHT_CORE_H

#include <prinsight/status.hpp>
#include <string>
#include <vector>

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

    Status initializeClientAnalyticsScheme(const std::string &schemeParamsData);

    Status getClientPublicData(std::string &clientPublicData);

    Status provideParticipantsPublicData(const std::string &participantsPublicData);

    Status setLabelsValue(const std::string &label, std::uint64_t value);

    Status getClientAnalyticsData(std::string &analyticsData);
  };

  Status serializeClientPublicDataList(const std::vector<std::string> &clientPublicDataList,
                                       std::string &serializedClientPublicDataList);

  /* Status serializeClientAnalyticsDataDataList(
        const std::vector<const std::string &> &clientAnalyticsDataDataList,
        std::string &serializedClientAnalyticsDataDataList);

     Status getInnerProductAnalysis(const std::string &analyticsData); */

}  // namespace prinsight

#endif  // PRINSIGHT_CORE_H
