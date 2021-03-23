#ifndef __CLIENT_PUBLIC_DATA_MODEL_H__
#define __CLIENT_PUBLIC_DATA_MODEL_H__

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace prinsight {
  class ClientPublicDataModel {
  private:
    /* data */
    std::size_t index;
    std::string pubKey;

  public:
    ClientPublicDataModel() : pubKey(""), index(0) {}

    ClientPublicDataModel(std::size_t index, const std::string pubKey)
        : index(index), pubKey(pubKey) {}
    ~ClientPublicDataModel() = default;

    inline std::size_t getIndex() { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::string getPublicKey() { return pubKey; }
    inline void setPublicKey(std::string pubKey) { this->pubKey = pubKey; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientPublicDataModel, index, pubKey)
  };

}  // namespace prinsight

#endif  // __CLIENT_PUBLIC_DATA_MODEL_H__