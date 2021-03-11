#pragma once

#include <string>

namespace prinsight {

  /**
   * @brief Decentralized Multi-Client Functional Encryption
   */
  class SampleApp {
    std::string name;

  public:
    /**
     * @brief Creates a new DecMultiClientIPFuncEnc
     * @param name the name of client
     */
    SampleApp(std::string name);

    void smokeTest();
    void smokeTest1();
  };

}  // namespace prinsight
