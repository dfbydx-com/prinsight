#ifndef PRINSIGHT_CORE_H

#define PRINSIGHT_CORE_H

#include <prinsight/status.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace prinsight {

  class DMCFEncryptor;

  /**
   * @brief Prinsight core APIs
   */
  class Core {
  private:
    DMCFEncryptor *mEncryptor;
    std::unordered_map<std::string, std::uint64_t> mAnalyticsData;

  public:
    Core();

    ~Core();

    Status getRegistrationData(std::string &registrationData);

    Status initializeAnalyticsScheme(const std::string &schemeParamsData);

    Status getPublicData(std::string &publicData);

    Status provideParticipantsPublicData(const std::string &participantsPublicData);

    Status setClearAnalyticsData(const std::string &label, std::uint64_t value);

    Status getEncryptedAnalyticsData(std::string &analyticsData);
  };

  Status serializeParticipantsPublicDataList(
      const std::vector<std::string> &participantsPublicDataList,
      std::string &serializedParticipantsPublicDataList);

  Status getInnerProductAnalysis(const std::vector<std::string> &participantsAnalyticsDataList,
                                 const std::vector<std::int64_t> policy, std::uint64_t bound,
                                 std::vector<std::pair<std::string, std::uint64_t>> &result);

}  // namespace prinsight

#endif  // PRINSIGHT_CORE_H
