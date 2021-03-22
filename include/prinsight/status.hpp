#ifndef PRINSIGHT_STATUS_H

#define PRINSIGHT_STATUS_H

#include <string>

namespace prinsight {

  enum class Status : int { kOk = 0, kUnknown = 1, kBadParams = 2 };
  std::string StatusToString(Status status);
  std::ostream& operator<<(std::ostream& os, Status code);

}  // namespace prinsight
#endif  // PRINSIGHT_STATUS_H
