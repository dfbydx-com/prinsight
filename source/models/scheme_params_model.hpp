#ifndef __SCHEME_PARAMS_MODEL_H__
#define __SCHEME_PARAMS_MODEL_H__

#include <spdlog/fmt/ostr.h>

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace prinsight {
  class SchemeParamsModel {
  private:
    /* data */
    std::size_t index;
    std::size_t participantsCount;
    std::uint64_t bound;

  public:
    SchemeParamsModel() : index(0), participantsCount(0), bound(0) {}
    SchemeParamsModel(std::size_t index, std::size_t participantsCount, std::uint64_t bound)
        : index(0), participantsCount(0), bound(0) {}
    ~SchemeParamsModel() = default;

    inline std::size_t getIndex() const { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::size_t getParticipantsCount() const { return participantsCount; }
    inline void setParticipantsCount(std::size_t participantsCount) {
      this->participantsCount = participantsCount;
    }

    inline std::size_t getBound() const { return bound; }
    inline void setBound(std::uint64_t bound) { this->bound = bound; }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const SchemeParamsModel &s) {
      return os << "scheme params data [index:" << s.index
                << ", participants-count:" << s.participantsCount << ", bound:" << s.bound << "]";
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SchemeParamsModel, index, participantsCount, bound)
  };

}  // namespace prinsight

#endif  // __SCHEME_PARAMS_MODEL_H__