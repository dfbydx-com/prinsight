#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <algorithm>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
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
    auto flags = Tcp::Options::ReuseAddr;

    auto opts = Http::Endpoint::options().threads(static_cast<int>(thr)).flags(flags);
    ;
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
    Routes::Post(router, "/register", Routes::bind(&AnalyticsService::doRegisterParticipant, this));
    Routes::Get(router, "/analytics/scheme/:regId",
                Routes::bind(&AnalyticsService::doGetAnalyticsSchemeParams, this));
    Routes::Post(router, "/analytics/pubdata/",
                 Routes::bind(&AnalyticsService::doRecordParticipantsPublicData, this));
    Routes::Get(router, "/analytics/allpubdata",
                Routes::bind(&AnalyticsService::doGetParticipantsPublicData, this));
    Routes::Post(router, "/analytics/analyticsdata",
                 Routes::bind(&AnalyticsService::doRecordAnalyticsData, this));
    Routes::Get(router, "/analysis", Routes::bind(&AnalyticsService::doGetAnalysis, this));
  }

  void doRegisterParticipant(const Rest::Request& request, Http::ResponseWriter response) {
    Guard guard(participantsLock);

    // batch is full
    if (batchSize == participantsIndex) {
      response.send(Http::Code::Internal_Server_Error, "batch is full");
    }

    try {
      SPDLOG_TRACE("request body {}", request.body());
      auto j = nlohmann::json::parse(request.body());
      std::string clientId = j["clientId"];
      auto it = std::find_if(
          participants.begin(), participants.end(),
          [&](const Participant& participant) { return participant.clientId() == clientId; });

      if (it == std::end(participants)) {
        int index = AnalyticsService::participantsIndex;
        AnalyticsService::participantsIndex++;
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
    auto registrationId = request.param(":regId").as<std::size_t>();

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
    Guard guard(participantsLock);

    try {
      SPDLOG_TRACE("request body {}", request.body());
      auto j = nlohmann::json::parse(request.body());
      std::size_t index = j["index"];
      auto it = std::find_if(
          participants.begin(), participants.end(),
          [&](const Participant& participant) { return participant.index() == index; });

      if (it == std::end(participants)) {
        response.send(Http::Code::Not_Found, "client is not registered");
      } else {
        auto& participant = *it;
        participant.setPublicData(request.body());
        response.send(Http::Code::Ok);
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

  void doGetParticipantsPublicData(const Rest::Request& request, Http::ResponseWriter response) {
    prinsight::Status status;
    std::vector<std::string> participantsPublicDataList;
    std::string serializedParticipantsPublicData = "";

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
    Guard guard(participantsLock);
    try {
      SPDLOG_TRACE("request body {}", request.body());
      auto j = nlohmann::json::parse(request.body());
      std::size_t index = j["index"];
      auto it = std::find_if(
          participants.begin(), participants.end(),
          [&](const Participant& participant) { return participant.index() == index; });

      if (it == std::end(participants)) {
        response.send(Http::Code::Not_Found, "client is not registered");
      } else {
        auto& participant = *it;
        participant.setAnalyticsData(request.body());
        mParticipantsAnalyticsDataList.push_back(request.body());

        response.send(Http::Code::Ok);
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

  void doGetAnalysis(const Rest::Request& request, Http::ResponseWriter response) {
    const std::vector<std::int64_t> policy(batchSize, 1);
    prinsight::Status status;
    std::vector<std::pair<std::string, std::uint64_t>> result;

    Guard guard(participantsLock);

    // return only if we have enough participants
    if ((batchSize != participantsIndex) || (batchSize != mParticipantsAnalyticsDataList.size())) {
      response.send(Http::Code::Internal_Server_Error, "not enough participants, retry");
      return;
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

  spdlog::set_level(spdlog::level::trace);
  SPDLOG_DEBUG("Cores {}", hardware_concurrency());
  SPDLOG_DEBUG("Using {} threads", thr);

  AnalyticsService ppAnalytics(addr, 2, 100000);

  ppAnalytics.init(thr);
  ppAnalytics.start();
}
