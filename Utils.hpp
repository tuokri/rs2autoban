#pragma once

#ifndef RS2AUTOBAN_UTILS_HPP
#define RS2AUTOBAN_UTILS_HPP

#include <string>

#include <windows.h>

namespace Utils
{

std::wstring HRESULTToWString(HRESULT hr);

std::wstring Utf8ToWide(std::string str);

}

#endif //RS2AUTOBAN_UTILS_HPP
