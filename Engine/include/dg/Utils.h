//
//  Utils.h
//

#pragma once

#if defined(_WIN32)
#include <Windows.h>
#endif
#include <vector>

namespace dg {

  inline char PathSeparator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
  }

#if defined(_WIN32)
  // Converts c string to wide string.
  std::vector<wchar_t> ToLPCWSTR(const std::string& str);
#endif

} // namespace dg

namespace std {

  template <class T, typename H>
  inline void hash_combine(H& seed, const T& v) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

} // namespace std
