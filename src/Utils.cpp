#include <stdexcept>
#include <cinttypes>

#include "Utils.hpp"

#define MAX_DATE_LEN 26

std::wstring Utils::HRESULTToWString(HRESULT hr)
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

std::wstring Utils::Utf8ToWide(std::string var)
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

std::tm Utils::DateNow()
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

std::wstring Utils::ISO8601DateNow()
{
    WCHAR buf[MAX_DATE_LEN] = {L'\0'};

    std::tm timeNow = Utils::DateNow();
    wcsftime(buf, MAX_DATE_LEN, L"%FT%TZ", &timeNow);

    return std::wstring(buf);
}

std::tm Utils::ISO8601WStrToTime(const WCHAR* str)
{
    std::tm tm{0};
    swscanf_s(str, L"%d-%d-%dT%d:%d:%dZ",
              &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
              &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    tm.tm_year -= 1900;
    tm.tm_isdst = -1;
    --tm.tm_mon;

    return tm;
}

std::tm Utils::ISO8601WStrToTime(std::wstring str)
{
    std::wstring var = std::move(str);
    return Utils::ISO8601WStrToTime(var.c_str());
}
