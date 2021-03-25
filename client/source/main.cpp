#include <prinsight/version.h>

#include <iostream>
#include <prinsight/prinsight.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "spdlog/spdlog.h"

auto main(int argc, char **argv) -> int {
  std::size_t nClients = 2;
  std::vector<prinsight::Core> clients;
  std::string clientRegistrationData = "";
  std::string clientPublicData = "";
  std::vector<std::string> clientPublicDataList;
  std::string serializedClientPublicData = "";

  std::string schmeParamsData1 = R"({ "index": 0, "participantsCount": 2, "bound": 1000 })";
  std::string schmeParamsData2 = R"({ "index": 1, "participantsCount": 2, "bound": 1000 })";
  std::vector<std::string> schemeParams = {schmeParamsData1, schmeParamsData2};

  spdlog::info("Client started, fetching keys");

  for (size_t i = 0; i < nClients; i++) {
    prinsight::Core core = prinsight::Core();
    core.getRegistrationData(clientRegistrationData);
    core.initializeAnalyticsScheme(schemeParams[i]);
    core.getPublicData(clientPublicData);
    clientPublicDataList.push_back(clientPublicData);
    clients.push_back(core);
  }

  prinsight::serializeClientPublicDataList(clientPublicDataList, serializedClientPublicData);

  for (size_t i = 0; i < nClients; i++) {
    clients[i].provideParticipantsPublicData(serializedClientPublicData);
  }

  return 0;
}
