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
        _wmsg(std::move(msg)),
        _hr(code)
    {};

    GenericError(std::string msg, HRESULT code) :
        _wmsg(Utils::Utf8ToWide(std::move(msg))),
        _hr(code)
    {};

    [[nodiscard]] const char* what() const override
    {
       return "Firewall::GenericError "
              "(call w_what() for detailed information)";
    };

    [[nodiscard]] std::wstring w_what() const
    {
        std::wstringstream ss;
        ss << _wmsg << " "
           << "(" << std::hex << "0x" << _hr << ") "
           << Utils::HRESULTToWString(_hr);
        return ss.str();
    };

private:
    std::wstring _wmsg;
    HRESULT _hr;
};

class Manager
{
public:
    Manager();

    ~Manager();

    void addBlockInboundAddressRule(const WCHAR* ipAddr);

    void pruneRules();

private:
    static HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

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
