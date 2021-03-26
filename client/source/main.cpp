#include <prinsight/version.h>

#include <iostream>
#include <prinsight/prinsight.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "spdlog/spdlog.h"

auto main(int argc, char **argv) -> int {
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

  spdlog::info("Client started, fetching keys");

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

  spdlog::info("analysis starts...");
  prinsight::getInnerProductAnalysis(participantsAnalyticsDataList, policy, bound, result);
  spdlog::info("analysis ends");

  for (auto r : result) {
    spdlog::info("{} : {}", r.first, r.second);
  }

  return 0;
}
