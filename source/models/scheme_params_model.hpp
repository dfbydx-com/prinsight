#ifndef __SCHEME_PARAMS_MODEL_H__
#define __SCHEME_PARAMS_MODEL_H__

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

    inline std::size_t getIndex() { return index; }
    inline void setIndex(std::size_t index) { this->index = index; }

    inline std::size_t getParticipantsCount() { return participantsCount; }
    inline void setParticipantsCount(std::size_t participantsCount) {
      this->participantsCount = participantsCount;
    }

    inline std::size_t getBound() { return bound; }
    inline void setBound(uint64_t bound) { this->bound = bound; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SchemeParamsModel, index, participantsCount, bound)
  };

}  // namespace prinsight

#endif  // __SCHEME_PARAMS_MODEL_H__