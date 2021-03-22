#include <prinsight/status.hpp>

namespace prinsight {

  std::string StatusToString(Status status) {
    switch (status) {
      case Status::kOk:
        return "OK";
      case Status::kUnknown:
        return "Unknown error";
      case Status::kBadParams:
        return "Bad parameters";
      default:
        return "";
    }
  }
  std::ostream& operator<<(std::ostream& os, Status code) { return os << StatusToString(code); }

}  // namespace prinsight
