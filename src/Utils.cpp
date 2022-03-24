#include <stdexcept>
#include <cinttypes>
#include <exception>

#include "Utils.hpp"

#define MAX_DATE_LEN 26

namespace Utils
{

std::wstring HRESULTToWString(HRESULT hr)
{
    LPWSTR messageBuffer = nullptr;
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER
                    | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    size_t size = FormatMessageW(
        dwFlags,
        nullptr,
        hr,
        MAKELANGID
        (LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR) &messageBuffer,
        0,
        nullptr);

    std::wstring message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

std::wstring Utf8ToWide(std::string var)
{
    std::string str = std::move(var);

    int count = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        nullptr,
        0);

    std::wstring wstr(count, 0);

    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        static_cast<int>(str.length()),
        &wstr[0],
        count);

    return wstr;
}

std::tm DateNow()
{
    std::tm tmNow{};
    int64_t now;
    errno_t err;

    _time64(&now);
    _gmtime64_s(&tmNow, &now);

    err = _gmtime64_s(&tmNow, &now);
    if (err)
    {
        throw std::invalid_argument("invalid _gmtime64_s argument");
    }
    return tmNow;
}

std::wstring TmToISO8601WStr(std::tm tm)
{
    WCHAR buf[MAX_DATE_LEN] = {L'\0'};

    if (!IsValidTm(tm))
    {
        throw std::invalid_argument("invalid tm argument");
    }

    wcsftime(buf, MAX_DATE_LEN, L"%FT%TZ", &tm);

    return {buf};
}

std::wstring ISO8601DateNow()
{
    return TmToISO8601WStr(DateNow());
}

std::tm ISO8601WStrToTime(const WCHAR* str)
{
    std::tm tm{0};

    // TODO: returns number of converted args...
    int converted = swscanf_s(str, L"%d-%d-%dT%d:%d:%dZ",
                              &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                              &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    if (converted != 6)
    {
        throw std::invalid_argument("invalid ISO8601 date string");
    }

    tm.tm_year -= 1900;
    tm.tm_isdst = -1;
    --tm.tm_mon;

    return tm;
}

std::tm ISO8601WStrToTime(std::wstring str)
{
    std::wstring var = std::move(str);
    return Utils::ISO8601WStrToTime(var.c_str());
}

bool IsValidTm(const std::tm& tm)
{
    auto cpy = tm;

    // http://en.cppreference.com/w/cpp/chrono/c/mktime
    //   Note: "The values in time are permitted to be outside their normal ranges."
    const auto as_time_t = std::mktime(&cpy);

    localtime_s(&cpy, &as_time_t);

    return ((tm.tm_mday == cpy.tm_mday)
            && (tm.tm_mon == cpy.tm_mon)
            && (tm.tm_year == cpy.tm_year)
            && (tm.tm_wday == cpy.tm_wday));
}

}
