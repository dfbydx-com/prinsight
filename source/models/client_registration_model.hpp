#ifndef __CLIENT_REGISTRATION_H__
#define __CLIENT_REGISTRATION_H__

#include <spdlog/fmt/ostr.h>

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
    inline std::string getClientId() const { return clientId; }
    inline void setClientId(const std::string &clientId) { this->clientId = clientId; }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const ClientRegistrationModel &c) {
      return os << "client registration data [client-id:" << c.clientId << "]";
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientRegistrationModel, clientId)
  };

}  // namespace prinsight
#endif  // __CLIENT_REGISTRATION_H__