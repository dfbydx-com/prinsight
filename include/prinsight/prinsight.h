#pragma once

#include <string>

namespace prinsight {

  /**
   * @brief Decentralized Multi-Client Functional Encryption
   */
  class DecMultiClientIPFuncEnc {
    std::string name;

  public:
    /**
     * @brief Creates a new DecMultiClientIPFuncEnc
     * @param name the name of client
     */
    DecMultiClientIPFuncEnc(std::string name);

    void smokeTest();
  };

}  // namespace prinsight
