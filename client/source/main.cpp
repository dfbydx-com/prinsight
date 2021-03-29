#include <prinsight/version.h>

#include <iostream>
#include <prinsight/prinsight.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
//#define RESTINCURL_USE_SYSLOG 0
#define RESTINCURL_ENABLE_DEFAULT_LOGGER 1
#include <restincurl/restincurl.h>

int makeGETRequest(const std::string &uri, std::string &response) {
  restincurl::Client client;

  SPDLOG_DEBUG("GET request uri: {}", uri);

  client.Build()
      ->Get(uri)

      // Tell the server that we accept Json payloads
      .AcceptJson()
      .WithCompletion([&](const restincurl::Result &result) {
        if (result.curl_code == 0 && result.http_response_code == 200) {
          SPDLOG_DEBUG("got reponse from server");
          // Parse the response body we got as Json.
          SPDLOG_DEBUG("returned body was {}", result.body);
          response = result.body;
        } else {
          SPDLOG_ERROR("request failed: {} HTTP code: {}", result.msg, result.http_response_code);
          return -1;
        }
      })
      .Execute();

  client.CloseWhenFinished();
  client.WaitForFinish();

  // Done
  return 0;
}

int makePOSTRequest(const std::string &uri, const std::string &request) {
  restincurl::Client client;

  SPDLOG_DEBUG("POST request uri: {}", uri);

  client.Build()
      ->Post(uri)

      // Tell the server that we accept Json payloads
      .AcceptJson()

      // Tell the server that we have a Json body, and
      // hand that body to the request.
      .WithJson(request)

      // Call this lambda when the request is done
      .WithCompletion([&](const restincurl::Result &result) {
        // Check if the request was successful
        if (!result.isOk()) {
          SPDLOG_ERROR("POST Request failed: {} HTTP code: {}", result.msg,
                       result.http_response_code);
          return -1;
        }
      })
      .Execute();

  client.CloseWhenFinished();
  client.WaitForFinish();
  // Done
  return 0;
}

auto main(int argc, char **argv) -> int {
  const std::string baseUri = "";
  prinsight::Core core;
  std::string clientRegistrationData = "";
  std::string schemeParams = "";
  std::string clientPublicData = "";
  std::string analyticsData = "";
  std::string participantsPublicData = "";

  SPDLOG_DEBUG("application started");

  SPDLOG_DEBUG("get registration data...");
  core.getRegistrationData(clientRegistrationData);

  SPDLOG_DEBUG("send client registration data to server...");
  if (makePOSTRequest(baseUri, clientRegistrationData)) {
    SPDLOG_ERROR("posting encrypted data to analytics engined failed");
    exit(EXIT_FAILURE);
  }

  SPDLOG_DEBUG("fetch scheme and participants information from server...");
  if (makeGETRequest(baseUri, schemeParams)) {
    SPDLOG_ERROR("retrieving public data from trusted entity failed");
    exit(EXIT_FAILURE);
  }

  SPDLOG_DEBUG("initialize analytics scheme...");
  core.initializeAnalyticsScheme(schemeParams);

  SPDLOG_DEBUG("get public data...");
  core.getPublicData(clientPublicData);

  SPDLOG_DEBUG("send public data to server...");
  if (makePOSTRequest(baseUri, clientPublicData)) {
    SPDLOG_ERROR("posting encrypted data to analytics engined failed");
    exit(EXIT_FAILURE);
  }

  SPDLOG_DEBUG("fetch participants public data from server...");
  if (makeGETRequest(baseUri, participantsPublicData)) {
    SPDLOG_ERROR("posting encrypted data to analytics engined failed");
    exit(EXIT_FAILURE);
  }

  // encrypt some data
  core.provideParticipantsPublicData(participantsPublicData);
  core.setClearAnalyticsData("label1", 1);
  core.setClearAnalyticsData("label2", 5);
  core.getEncryptedAnalyticsData(analyticsData);

  SPDLOG_DEBUG("send encrypted data to server...");
  // post encrypted data to server
  if (makePOSTRequest(baseUri, analyticsData)) {
    SPDLOG_ERROR("posting encrypted data to server failed");
    exit(EXIT_FAILURE);
  }

  return 0;
}
