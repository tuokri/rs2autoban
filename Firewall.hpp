#pragma once

#ifndef RS2AUTOBAN_FIREWALL_HPP
#define RS2AUTOBAN_FIREWALL_HPP

#include <utility>
#include <vector>
#include <string>
#include <sstream>

#include <windows.h>
#include <netfw.h>

#include "Utils.hpp"

namespace Firewall
{

inline static const WCHAR* RS2AUTOBAN_RULE_NAME = L"__RS2AUTOBAN_RULE";
inline static const WCHAR* RS2AUTOBAN_GROUP_NAME = L"__RS2AUTOBAN_GROUP";
inline static const WCHAR* RS2AUTOBAN_RULE_DESC = L"RS2Autoban automatic rule.";

class GenericError : public std::exception
{
public:
    GenericError(std::wstring msg, HRESULT code) :
        _message(std::move(msg)),
        _hr(code)
    {};

    GenericError(std::string msg, HRESULT code) :
        _message(Utils::Utf8ToWide(std::move(msg))),
        _hr(code)
    {};

    [[nodiscard]] const char* what() const override
    {
        std::stringstream ss;
        ss << Utils::WideToUtf8(_message) << ": "
           << Utils::HRESULTToString(_hr)
           << "(" << std::hex << _hr << ")";
        return ss.str().c_str();
    };

    [[nodiscard]] const WCHAR* w_what() const
    {
        std::wstringstream ss;
        ss << _message << ": " << Utils::HRESULTToWString(_hr)
           << "(" << std::hex << _hr << ")";
        return ss.str().c_str();
    };

private:
    std::wstring _message;
    HRESULT _hr;
};

class Manager
{
public:
    Manager();

    ~Manager();

    void addDenyIPRule(const std::wstring& ipAddr);

    void pruneRules();

private:
    HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

    BSTR _ruleName;
    BSTR _ruleDescription;
    BSTR _ruleGroup;
    BSTR _ruleRemoteAddresses;
    INetFwPolicy2* _pNetFwPolicy2 = nullptr;
    INetFwRules* _pNetFwRules = nullptr;
    HRESULT _hrComInit;
    long _currentProfilesBitMask = 0;
};

}

#endif //RS2AUTOBAN_FIREWALL_HPP
