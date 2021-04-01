#include <prinsight/status.hpp>

namespace prinsight {

  std::string StatusToString(Status status) {
    switch (status) {
      case Status::kOk:
        return "OK";
      case Status::kUnknownError:
        return "Unknown error";
      case Status::kBadParams:
        return "Bad parameters";
      case Status::kBadState:
        return "Bad state";
      case Status::kSchemeUninitialized:
        return "Scheme uninitialized";
      case Status::kOutOfMemory:
        return "Out of memory";
      case Status::kCryptoOperationError:
        return "Crypto operation error";
      default:
        return "";
    }
  }
  std::ostream& operator<<(std::ostream& os, Status code) { return os << StatusToString(code); }

}  // namespace prinsight
