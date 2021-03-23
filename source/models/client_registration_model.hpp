#ifndef __CLIENT_REGISTRATION_H__
#define __CLIENT_REGISTRATION_H__

#include <nlohmann/json.hpp>
#include <string>

namespace prinsight {
  class ClientRegistrationModel {
  private:
    /* data */
    std::string clientId;

  public:
    ClientRegistrationModel() : clientId("") {}
    explicit ClientRegistrationModel(const std::string clientId) : clientId(clientId) {}
    ~ClientRegistrationModel() = default;
    inline std::string getClientId() { return clientId; }
    inline void setClientId(const std::string clientId) { this->clientId = clientId; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientRegistrationModel, clientId)
  };

}  // namespace prinsight
#endif  // __CLIENT_REGISTRATION_H__