#pragma once

#include <string>

#include "C2SProtocol.hpp"

namespace geblaat {
class HTTP : public Protocol {
  public:
    virtual ~HTTP() {};
    virtual int get(std::string url, int version = 11) = 0;
};
} // namespace geblaat
