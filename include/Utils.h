//
//  Utils.h
//

#pragma once

#if defined(_WIN32)
#include <Windows.h>
#endif
#include <vector>

namespace dg {

#if defined(_WIN32)
  // Converts c string to wide string.
  std::vector<wchar_t> ToLPCWSTR(const std::string& str);
#endif

}

