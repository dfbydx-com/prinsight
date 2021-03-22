#include <prinsight/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <prinsight/prinsight.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "spdlog/spdlog.h"
//#define RESTINCURL_USE_SYSLOG 0
#define RESTINCURL_ENABLE_DEFAULT_LOGGER 1
#include "nlohmann/json.hpp"
#include "restincurl/restincurl.h"

using namespace restincurl;
using json = nlohmann::json;

int64_t clientId = 0;
std::string bound = "";
size_t dataVectorLength = 0;
size_t modulusLength = 0;
std::vector<std::string> publicKey;

int getFEPublicParamsFromTrustedServer() {
  restincurl::Client client;

  spdlog::info("GET public key of other participnts from server");

  client.Build()
      ->Get("http://localhost:9080/register")

      // Tell the server that we accept Json payloads
      .AcceptJson()
      .WithCompletion([&](const Result &result) {
        if (result.curl_code == 0 && result.http_response_code == 200) {
          try {
            spdlog::info("Got reponse from server");
            // Parse the response body we got as Json.
            spdlog::info("Returned body was {}", result.body);
            const auto j = json::parse(result.body);

            clientId = j["ClientID"].get<int64_t>();
            // auto params = j["Params"];
            dataVectorLength = j["Params"]["L"].get<int>();
            bound = j["Params"]["Bound"].get<std::string>();
            modulusLength = j["Params"]["modulusLength"];
            auto mpk = j["mpk"];
            for (json::iterator it = mpk.begin(); it != mpk.end(); ++it) {
              publicKey.push_back(*it);
              spdlog::debug("{}", *it);
            }

          } catch (const std::exception &ex) {
            spdlog::error("Caught exception: {}", ex.what());
            return -1;
          }

        } else {
          spdlog::error("GetKey Request failed: {} HTTP code: {}", result.msg,
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

int postEncryptedDataToAnalyticsServer(const std::vector<std::string> &encryptdData) {
  restincurl::Client client;

  json j;
  j["ClientID"] = clientId;
  j["encData"] = encryptdData;

  client.Build()
      ->Post("http://localhost:9080/data")

      // Tell the server that we accept Json payloads
      .AcceptJson()

      // Tell the server that we have a Json body, and
      // hand that body to the request.
      // json::dump() will serialize the data in j as a Json string.
      .WithJson(j.dump())

      // Call this lambda when the request is done
      .WithCompletion([&](const Result &result) {
        // Check if the request was successful
        if (result.isOk()) {
          spdlog::info("Posting to analytics server successful");

          spdlog::debug("Returned body was {}", result.body);
          try {
            // Parse the response body we got as Json.
            // const auto j = json::parse(result.body);

          } catch (const std::exception &ex) {
            spdlog::error("Caught exception: ", ex.what());
            return -1;
          }

        } else {
          spdlog::error("PostData Request failed: {} HTTP code: {}", result.msg,
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
  spdlog::info("Client started, fetching keys");

  // get fe scheme public params from server
  if (getFEPublicParamsFromTrustedServer()) {
    spdlog::error("retrieving public data from trusted entity failed");
    exit(EXIT_FAILURE);
  }

  std::vector<std::string> encryptedData;
  encryptedData.push_back("encdata 1");
  encryptedData.push_back("encdata 2");

  // print encrypted data
  spdlog::info("Sending encrypted data...");
  // post encrypted data to analytics engine
  if (postEncryptedDataToAnalyticsServer(encryptedData)) {
    spdlog::error("posting encrypted data to analytics engined failed");
    exit(EXIT_FAILURE);
  }

  prinsight::Core core;
  return 0;
}
