#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <algorithm>

//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>
#include <prinsight/prinsight.hpp>

using namespace Pistache;

// Participant Model
class Participant {
public:
  Participant(std::string clientId, int index) : mClientId(std::move(clientId)), mIndex(index) {
    mPublicData = "";
    mAnalyticsData = "";
  }

  std::size_t index() const { return mIndex; }
  const std::string& clientId() const { return mClientId; }

  const std::string& publicData() { return mPublicData; }
  void setPublicData(const std::string& publicData) { mPublicData = publicData; }

  const std::string& analyticsData() { return mAnalyticsData; }
  void setAnalyticsData(const std::string& analyticsData) { mAnalyticsData = analyticsData; }

private:
  std::string mClientId;
  std::size_t mIndex;
  std::string mPublicData;
  std::string mAnalyticsData;
};

class AnalyticsService {
public:
  AnalyticsService(Address addr, std::size_t batchSize, std::uint64_t bound)
      : httpEndpoint(std::make_shared<Http::Endpoint>(addr)), batchSize(batchSize), bound(bound) {
    participantsIndex = 0;
  }

  void init(std::size_t thr = 2) {
    auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
    httpEndpoint->init(opts);
    setupRoutes();
  }

  void start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
  }

private:
  void setupRoutes() {
    using namespace Rest;
    Routes::Get(router, "/register", Routes::bind(&AnalyticsService::doRegisterParticipant, this));
    Routes::Get(router, "/analytics/scheme",
                Routes::bind(&AnalyticsService::doGetAnalyticsSchemeParams, this));
    Routes::Get(router, "/analytics/pubdata",
                Routes::bind(&AnalyticsService::doRecordParticipantsPublicData, this));
    Routes::Get(router, "/analytics/pubdata",
                Routes::bind(&AnalyticsService::doGetParticipantsPublicData, this));
    Routes::Get(router, "/analytics/analyticsdata",
                Routes::bind(&AnalyticsService::doGetAnalyticsSchemeParams, this));
    Routes::Get(router, "/analysis", Routes::bind(&AnalyticsService::doGetAnalysis, this));
  }

  void doRegisterParticipant(const Rest::Request& request, Http::ResponseWriter response) {
    Guard guard(participantsLock);

    // batch is full
    if (batchSize == participantsIndex) {
      response.send(Http::Code::Internal_Server_Error, "batch is full");
    }

    try {
      auto j = nlohmann::json::parse(request.body());
      std::string clientId = j["clientId"];
      auto it = std::find_if(
          participants.begin(), participants.end(),
          [&](const Participant& participant) { return participant.clientId() == clientId; });

      if (it == std::end(participants)) {
        int index = AnalyticsService::participantsIndex++;
        participants.push_back(Participant(std::move(clientId), index));
        response.send(Http::Code::Created, std::to_string(index));
      } else {
        auto& participant = *it;
        response.send(Http::Code::Ok, std::to_string(participant.index()));
      }
    } catch (nlohmann::detail::exception& e) {
      // send a 400 error
      response.send(Pistache::Http::Code::Bad_Request, e.what());
      return;
    } catch (std::exception& e) {
      // send a 500 error
      response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
      return;
    }
  }

  void doGetAnalyticsSchemeParams(const Rest::Request& request, Http::ResponseWriter response) {
    auto registrationId = request.param(":registrationId").as<std::size_t>();

    Guard guard(participantsLock);
    auto it = std::find_if(
        participants.begin(), participants.end(),
        [&](const Participant& participant) { return participant.index() == registrationId; });

    if (it == std::end(participants)) {
      response.send(Http::Code::Not_Found, "client is not registered");
    } else {
      auto& participant = *it;
      std::string index = std::to_string(participant.index());
      std::string schemeParams = "{ \"index\":" + std::to_string(participant.index())
                                 + ", \"participantsCount\":" + std::to_string(batchSize)
                                 + ", \"bound\":" + std::to_string(bound) + "}";

      response.send(Http::Code::Ok, schemeParams);
    }
  }

  void doRecordParticipantsPublicData(const Rest::Request& request, Http::ResponseWriter response) {
    std::string pubData = "";
    auto registrationId = request.param(":registrationId").as<std::size_t>();

    if (request.hasParam(":pubData")) {
      pubData = request.param(":pubData").as<std::string>();
    } else {
      response.send(Http::Code::Bad_Request, "parameter is missing");
    }

    Guard guard(participantsLock);
    auto it = std::find_if(
        participants.begin(), participants.end(),
        [&](const Participant& participant) { return participant.index() == registrationId; });

    if (it == std::end(participants)) {
      response.send(Http::Code::Not_Found, "client is not registered");
    } else {
      auto& participant = *it;
      participant.setPublicData(pubData);
      response.send(Http::Code::Ok);
    }
  }

  void doGetParticipantsPublicData(const Rest::Request& request, Http::ResponseWriter response) {
    prinsight::Status status;
    std::vector<std::string> participantsPublicDataList;
    std::string serializedParticipantsPublicData = "";
    auto registrationId = request.param(":registrationId").as<std::size_t>();

    Guard guard(participantsLock);

    // return only if we have enough participants
    if (batchSize != participantsIndex) {
      response.send(Http::Code::Internal_Server_Error, "not enough participants, retry");
    }

    for (auto p : participants) {
      participantsPublicDataList.push_back(p.publicData());
    }
    status = prinsight::serializeParticipantsPublicDataList(participantsPublicDataList,
                                                            serializedParticipantsPublicData);

    if (prinsight::Status::kOk != status) {
      response.send(Http::Code::Internal_Server_Error, "unknown error");
    }

    response.send(Http::Code::Ok, serializedParticipantsPublicData);
  }

  void doRecordAnalyticsData(const Rest::Request& request, Http::ResponseWriter response) {
    std::string analyticsData = "";
    auto registrationId = request.param(":registrationId").as<std::size_t>();

    if (request.hasParam(":analyticsData")) {
      analyticsData = request.param(":analyticsData").as<std::string>();
    } else {
      response.send(Http::Code::Bad_Request, "parameter is missing");
    }

    Guard guard(participantsLock);
    auto it = std::find_if(
        participants.begin(), participants.end(),
        [&](const Participant& participant) { return participant.index() == registrationId; });

    if (it == std::end(participants)) {
      response.send(Http::Code::Not_Found, "client is not registered");
    } else {
      auto& participant = *it;
      participant.setAnalyticsData(analyticsData);
      mParticipantsAnalyticsDataList.push_back(analyticsData);
      response.send(Http::Code::Ok);
    }
  }

  void doGetAnalysis(const Rest::Request& request, Http::ResponseWriter response) {
    const std::vector<std::int64_t> policy(batchSize, 1);
    prinsight::Status status;
    std::vector<std::pair<std::string, std::uint64_t>> result;

    Guard guard(participantsLock);

    // return only if we have enough participants
    if ((batchSize != participantsIndex) || (batchSize != mParticipantsAnalyticsDataList.size())) {
      response.send(Http::Code::Internal_Server_Error, "not enough participants, retry");
    }

    SPDLOG_DEBUG("analysis starts...");
    status
        = prinsight::getInnerProductAnalysis(mParticipantsAnalyticsDataList, policy, bound, result);
    SPDLOG_DEBUG("analysis ends");

    if (prinsight::Status::kOk != status) {
      response.send(Http::Code::Internal_Server_Error, "unknown error");
    }

    nlohmann::json j = result;

    response.send(Http::Code::Ok, j.dump());
  }
  using Lock = std::mutex;
  using Guard = std::lock_guard<Lock>;
  Lock participantsLock;
  std::vector<Participant> participants;

  std::size_t participantsIndex;

  const std::size_t batchSize;
  const std::uint64_t bound;

  // just for testing
  std::vector<std::string> mParticipantsAnalyticsDataList;

  std::shared_ptr<Http::Endpoint> httpEndpoint;
  Rest::Router router;
};

int main(int argc, char* argv[]) {
  Port port(9080);

  int thr = 2;

  if (argc >= 2) {
    port = static_cast<uint16_t>(std::stol(argv[1]));

    if (argc == 3) thr = std::stoi(argv[2]);
  }

  Address addr(Ipv4::any(), port);

  SPDLOG_DEBUG("Cores {}", hardware_concurrency());
  SPDLOG_DEBUG("Using {} threads", thr);

  AnalyticsService ppAnalytics(addr, 2, 100000);

  ppAnalytics.init(thr);
  ppAnalytics.start();
}
