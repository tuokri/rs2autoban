#pragma once

#include <utility>
#include <vector>
#include <string>

#include <windows.h>
#include <netfw.h>

#include "Utils.hpp"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

namespace Firewall
{

class GenericError : public std::exception
{
public:

    GenericError(std::string msg, HRESULT code) :
        _msg(std::move(msg)),
        _hr(code)
    {};

    const char* what()
    {
        std::stringstream ss;
        ss << _msg << ": " << Utils::HRESULTToString(_hr)
           << "(" << _hr << ")";
        return ss.str().c_str();
    };


private:
    std::string _msg;
    HRESULT _hr;
};

const WCHAR* RS2AUTOBAN_RULE_NAME = L"__RS2AUTOBAN_RULE";
const WCHAR* RS2AUTOBAN_GROUP_NAME = L"__RS2AUTOBAN_GROUP";
const WCHAR* RS2AUTOBAN_RULE_DESC = L"RS2Autoban automatic rule";

class Rule
{
public:
    Rule();

    ~Rule();

private:
    INetFwRule* _pFwRule;
};

class Manager
{
public:
    Manager();

    ~Manager();

    std::vector<Rule> getRules();

    void addRule(Rule rule);

    void removeRule(Rule rule);

private:
    HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

    BSTR _ruleName;
    BSTR _ruleDescription;
    BSTR _ruleGroup;
    BSTR _ruleRemoteAddresses;
    INetFwPolicy2* _pNetFwPolicy2;
    INetFwRules* _pFwRules;
    HRESULT _hrComInit;
};

}
