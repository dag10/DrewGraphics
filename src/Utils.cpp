//
//  Utils.cpp
//

#include "dg/Utils.h"

std::vector<wchar_t> dg::ToLPCWSTR(const std::string& str) {
  std::vector<wchar_t> ret(4096);
  MultiByteToWideChar(
    CP_ACP, 0, str.c_str(), -1, ret.data(), (int)ret.size());
  return ret;
}
