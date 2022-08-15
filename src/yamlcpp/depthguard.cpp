#include "yaml-cpp/depthguard.h"

namespace LHAPDF_YAML {

DeepRecursion::DeepRecursion(int depth, const Mark& mark_,
                             const std::string& msg_)
    : ParserException(mark_, msg_), m_depth(depth) {}

}  // namespace LHAPDF_YAML
