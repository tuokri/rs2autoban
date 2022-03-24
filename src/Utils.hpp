#pragma once

#ifndef RS2AUTOBAN_UTILS_HPP
#define RS2AUTOBAN_UTILS_HPP

#include <string>
#include <ctime>

#include <Windows.h>

namespace Utils
{

std::wstring HRESULTToWString(HRESULT hr);

std::wstring Utf8ToWide(std::string str);

std::wstring TmToISO8601WStr(std::tm tm);

std::wstring ISO8601DateNow();

std::tm DateNow();

std::tm ISO8601WStrToTime(const WCHAR* str);

std::tm ISO8601WStrToTime(std::wstring str);

bool IsValidTm(const std::tm& tm);

}

#endif //RS2AUTOBAN_UTILS_HPP
