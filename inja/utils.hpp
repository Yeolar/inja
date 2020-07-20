// Copyright (c) 2020 Pantor. All rights reserved.

#ifndef INCLUDE_INJA_UTILS_HPP_
#define INCLUDE_INJA_UTILS_HPP_

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>

#include <accelerator/Range.h>

#include "exceptions.hpp"

namespace inja {

inline void open_file_or_throw(const std::string &path, std::ifstream &file) {
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
  } catch (const std::ios_base::failure & /*e*/) {
    throw FileError("failed accessing file at '" + path + "'");
  }
}

inline acc::StringPiece slice(acc::StringPiece view, size_t start, size_t end) {
  start = std::min(start, view.size());
  end = std::min(std::max(start, end), view.size());
  return view.subpiece(start, end - start);
}

inline SourceLocation get_source_location(acc::StringPiece content, size_t pos) {
  // Get line and offset position (starts at 1:1)
  auto sliced = slice(content, 0, pos);
  std::size_t last_newline = sliced.rfind('\n');

  if (last_newline == acc::StringPiece::npos) {
    return {1, sliced.size() + 1};
  }

  // Count newlines
  size_t count_lines = 0;
  size_t search_start = 0;
  while (search_start <= sliced.size()) {
    search_start = sliced.find('\n', search_start) + 1;
    if (search_start <= 0) {
      break;
    }
    count_lines += 1;
  }

  return {count_lines + 1, sliced.size() - last_newline};
}

} // namespace inja

#endif // INCLUDE_INJA_UTILS_HPP_
