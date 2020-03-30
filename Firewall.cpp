#include <iostream>
#include <exception>
#include <sstream>

#include "Firewall.hpp"
#include "Utils.hpp"

Firewall::Rule::Rule()
{
    HRESULT hr;

    hr = CoCreateInstance(
        __uuidof(NetFwRule),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),
        (void**) &_pFwRule);

    if (FAILED(hr))
    {
        std::wcout << "CoCreateInstance for Firewall Rule failed: "
                   << Utils::HRESULTToWString(hr);
    }
}

Firewall::Rule::~Rule()
{
    if (_pFwRule != nullptr)
    {
        _pFwRule->Release();
    }
}


Firewall::Manager::Manager()
{
    _ruleDescription = SysAllocString(RS2AUTOBAN_RULE_DESC);
    if (_ruleDescription == nullptr)
    {
        throw std::bad_alloc();
    }

    _ruleName = SysAllocString(RS2AUTOBAN_RULE_NAME);
    if (_ruleName == nullptr)
    {
        throw std::bad_alloc();
    }

    _ruleGroup = SysAllocString(RS2AUTOBAN_GROUP_NAME);
    if (_ruleGroup == nullptr)
    {
        throw std::bad_alloc();
    }

    HRESULT hr = S_OK;
    long currentProfilesBitMask = 0;
    std::wstringstream ss;

    _hrComInit = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED
    );

    hr = WFCOMInitialize(&_pNetFwPolicy2);
    if (FAILED(hr))
    {
        throw Firewall::GenericError("WFCOMInitialize failed", hr);
    }

    hr = _pNetFwPolicy2->get_Rules(&_pFwRules);
    if (FAILED(hr))
    {
        throw Firewall::GenericError("get_Rules failed", hr);
    }

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (_hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(_hrComInit))
        {
            throw Firewall::GenericError("CoInitializeEx failed", _hrComInit);
        }
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile,
    // we remove it from the bitmask.
    if ((currentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (currentProfilesBitMask != NET_FW_PROFILE2_PUBLIC))
    {
        currentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }
}

Firewall::Manager::~Manager()
{
    SysFreeString(_ruleName);
    SysFreeString(_ruleDescription);
    SysFreeString(_ruleGroup);
    SysFreeString(_ruleRemoteAddresses);

    if (_pFwRules != nullptr)
    {
        _pFwRules->Release();
    }

    if (_pNetFwPolicy2 != nullptr)
    {
        _pNetFwPolicy2->Release();
    }

    if (SUCCEEDED(_hrComInit))
    {
        CoUninitialize();
    }
}


HRESULT Firewall::Manager::WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**) ppNetFwPolicy2);

    return hr;
}
