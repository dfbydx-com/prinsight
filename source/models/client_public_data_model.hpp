#ifndef __CLIENT_PUBLIC_DATA_MODEL_H__
#define __CLIENT_PUBLIC_DATA_MODEL_H__

#include <spdlog/fmt/ostr.h>

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
    ClientPublicDataModel() : index(0), pubKey("") {}

    ClientPublicDataModel(std::size_t index, const std::string pubKey)
        : index(index), pubKey(pubKey) {}
    ~ClientPublicDataModel() = default;

    inline std::size_t getIndex() const { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::string getPublicKey() const { return pubKey; }
    inline void setPublicKey(const std::string &pubKey) { this->pubKey = pubKey; }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const ClientPublicDataModel &c) {
      return os << "client public data [index:" << c.index << ", pub-key:" << c.pubKey << "]";
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientPublicDataModel, index, pubKey)
  };

}  // namespace prinsight

#endif  // __CLIENT_PUBLIC_DATA_MODEL_H__