#include <exception>

#include "Firewall.hpp"
#include "Utils.hpp"

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

    hr = _pNetFwPolicy2->get_Rules(&_pNetFwRules);
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

    hr = _pNetFwPolicy2->get_CurrentProfileTypes(&_currentProfilesBitMask);
    if (FAILED(hr))
    {
        throw Firewall::GenericError("get_CurrentProfileTypes failed", hr);
    }

    // When possible we avoid adding firewall rules to the Public profile.
    // If Public is currently active and it is not the only active profile,
    // we remove it from the bitmask.
    if ((_currentProfilesBitMask & NET_FW_PROFILE2_PUBLIC) &&
        (_currentProfilesBitMask != NET_FW_PROFILE2_PUBLIC))
    {
        _currentProfilesBitMask ^= NET_FW_PROFILE2_PUBLIC;
    }
}

Firewall::Manager::~Manager()
{
    SysFreeString(_ruleName);
    SysFreeString(_ruleDescription);
    SysFreeString(_ruleGroup);

    if (_pNetFwRules != nullptr)
    {
        _pNetFwRules->Release();
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

void Firewall::Manager::addBlockInboundAddressRule(const WCHAR* ipAddr)
{
    std::string extra;
    HRESULT hr = S_OK;
    INetFwRule* pNetFwRule = nullptr;
    BSTR remoteAddr;

    remoteAddr = SysAllocString(ipAddr);
    if (remoteAddr == nullptr)
    {
        extra = "SysAllocString failed";
        goto Cleanup;
    }

    hr = CoCreateInstance(
        __uuidof(NetFwRule),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),
        (void**) &pNetFwRule);

    if (FAILED(hr))
    {
        extra = "CoCreateInstance failed";
        goto Cleanup;
    }

    pNetFwRule->put_Name(_ruleName);
    pNetFwRule->put_Description(_ruleDescription);
    pNetFwRule->put_Grouping(_ruleGroup);
    pNetFwRule->put_Profiles(_currentProfilesBitMask);
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);
    pNetFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    pNetFwRule->put_RemoteAddresses(remoteAddr);

    hr = _pNetFwRules->Add(pNetFwRule);
    if (FAILED(hr))
    {
        extra = "Add failed";
        goto Cleanup;
    }

    Cleanup:

    if (pNetFwRule != nullptr)
    {
        pNetFwRule->Release();
    }

    SysFreeString(remoteAddr);

    if (FAILED(hr))
    {
        std::stringstream ss;
        ss << "adding rule failed: " << extra;
        throw Firewall::GenericError(ss.str(), hr);
    }
}

void pruneRules()
{

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
