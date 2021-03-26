#ifndef __ANALYTICS_DATA_MODEL_H__
#define __ANALYTICS_DATA_MODEL_H__

#include <spdlog/fmt/ostr.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace prinsight {
  class AnalyticsData {
  private:
    /* data */
    std::size_t index;
    std::unordered_map<std::string, std::string> encData;
    std::vector<std::string> fdKey;

  public:
    AnalyticsData() = default;

    AnalyticsData(std::size_t index, const std::unordered_map<std::string, std::string> encData,
                  const std::vector<std::string> feKey)
        : index(index), encData(encData), fdKey(feKey) {}
    ~AnalyticsData() = default;

    inline std::size_t getIndex() const { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::unordered_map<std::string, std::string> getEncData() const { return encData; }
    inline void setEncData(const std::unordered_map<std::string, std::string> &encData) {
      this->encData = encData;
    }

    inline std::vector<std::string> fdKey() const { return fdKey; }
    inline void setFeKey(const std::vector<std::string> &feKey) { this->fdKey = feKey; }

    template <typename OStream> friend OStream &operator<<(OStream &os, const AnalyticsData &a) {
      os << "analytics data [index:" << a.index << ", enc-data:{";
      for (auto element : a.encData) {
        os << "{" << element.first << ", " << element.second << "},";
      }
      os << "}, fekey:{";
      for (auto element : a.fdKey) {
        os << element << ",";
      }
      os << "}]" << std::endl;

      return os;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyticsData, index, encData, fdKey)
  };
}  // namespace prinsight

#endif  // __ANALYTICS_DATA_MODEL_H__