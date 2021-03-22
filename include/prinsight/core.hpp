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
    Status getRegistrationData();
    Status getPublicData();
    Status provideParticipantsPublicData();
    Status setLabels();
    Status getAnalyticsData();
    static Status getIP();
  };

}  // namespace prinsight

#endif  // PRINSIGHT_CORE_H
