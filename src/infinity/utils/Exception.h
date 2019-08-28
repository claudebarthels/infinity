/**
 * Exception - thrown when an assert fails.
 */

#ifndef UTILS_EXCEPTION_H
#define UTILS_EXCEPTION_H

#include <stdexcept>

namespace infinity::utils {
  struct Exception : public std::runtime_error {
    using runtime_error::runtime_error;
  };
}

#endif
