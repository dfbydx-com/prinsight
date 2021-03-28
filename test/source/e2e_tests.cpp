#include <doctest/doctest.h>
#include <prinsight/version.h>

#include <prinsight/prinsight.hpp>
#include <string>
#include <unordered_map>
#include <vector>

TEST_CASE("e2e encrypt-decrypt test on prinsight APIs") {
  std::size_t numClients = 2;
  const std::uint64_t bound = 1000;
  const std::vector<std::int64_t> policy(numClients, 1);
  std::vector<prinsight::Core> clients;
  std::string clientRegistrationData = "";
  std::string clientPublicData = "";
  std::vector<std::string> participantsPublicDataList;
  std::string serializedParticipantsPublicData = "";
  std::vector<std::string> participantsAnalyticsDataList;
  std::vector<std::pair<std::string, std::uint64_t>> result;

  std::string schmeParamsData1 = R"({ "index": 0, "participantsCount": 2, "bound": 1000 })";
  std::string schmeParamsData2 = R"({ "index": 1, "participantsCount": 2, "bound": 1000 })";
  std::vector<std::string> schemeParams = {schmeParamsData1, schmeParamsData2};

  for (std::size_t i = 0; i < numClients; i++) {
    prinsight::Core core = prinsight::Core();
    core.getRegistrationData(clientRegistrationData);
    core.initializeAnalyticsScheme(schemeParams[i]);
    core.getPublicData(clientPublicData);
    participantsPublicDataList.push_back(clientPublicData);
    clients.push_back(core);
  }

  prinsight::serializeParticipantsPublicDataList(participantsPublicDataList,
                                                 serializedParticipantsPublicData);

  for (std::size_t i = 0; i < numClients; i++) {
    std::string analyticsData = "";
    clients[i].provideParticipantsPublicData(serializedParticipantsPublicData);
    clients[i].setClearAnalyticsData("label1", 1);
    clients[i].setClearAnalyticsData("label2", 5);
    clients[i].getEncryptedAnalyticsData(analyticsData);
    participantsAnalyticsDataList.push_back(analyticsData);
  }

  prinsight::getInnerProductAnalysis(participantsAnalyticsDataList, policy, bound, result);

  CHECK(result[0].first == "label1");
  CHECK(result[0].second == 2);

  CHECK(result[1].first == "label2");
  CHECK(result[1].second == 10);

  return;
}
