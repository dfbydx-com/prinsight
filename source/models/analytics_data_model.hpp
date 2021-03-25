#ifndef __ANALYTICS_DATA_MODEL_H__
#define __ANALYTICS_DATA_MODEL_H__

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace prinsight {
  class AnalyticsData {
  private:
    /* data */
    std::size_t index;
    std::string encData;
    std::vector<std::string> feKey;

  public:
    AnalyticsData() : index(0), encData("") {
      feKey.push_back("");
      feKey.push_back("");
    }

    AnalyticsData(std::size_t index, const std::string encData,
                  const std::vector<std::string> feKey)
        : index(index), encData(encData), feKey(feKey) {}
    ~AnalyticsData() = default;

    inline std::size_t getIndex() const { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::string getEncData() const { return encData; }
    inline void setEncData(const std::string& encData) { this->encData = encData; }

    inline std::vector<std::string> getFeKey() const { return feKey; }
    inline void setFeKey(const std::vector<std::string>& feKey) { this->feKey = feKey; }

    // friend deSerializeAnalyticsDataList();
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyticsData, index, encData, feKey)
  };
}  // namespace prinsight

#endif  // __ANALYTICS_DATA_MODEL_H__