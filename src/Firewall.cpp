#include <exception>
#include <vector>
#include <iostream>
#include <regex>

#include <Windows.h>
#include <atlcomcli.h>
#include <netfw.h>

#include "Firewall.hpp"
#include "Utils.hpp"

Firewall::Manager::Manager()
{
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

void Firewall::Manager::addBlockInboundAddressRule(const WCHAR* address)
{
    std::wstring extra;
    std::wstringstream ss;
    HRESULT hr = S_OK;
    INetFwRule* pNetFwRule = nullptr;
    BSTR bstrRuleName = nullptr;
    BSTR bstrRemoteAddr = nullptr;
    BSTR bstrRuleDescription = nullptr;

    bstrRemoteAddr = SysAllocString(address);
    if (bstrRemoteAddr == nullptr)
    {
        extra = L"bstrRemoteAddr SysAllocString failed";
        goto Cleanup;
    }

    ss << RS2AUTOBAN_RULE_NAME << L" " << address;
    bstrRuleName = SysAllocString(ss.str().c_str());
    ss.str(std::wstring());
    ss.clear();
    if (bstrRuleName == nullptr)
    {
        extra = L"bstrRuleName SysAllocString failed";
        goto Cleanup;
    }

    ss << RS2AUTOBAN_RULE_DESC << L" [" << Utils::ISO8601DateNow() << L"]";
    bstrRuleDescription = SysAllocString(ss.str().c_str());
    ss.str(std::wstring());
    ss.clear();
    if (bstrRuleDescription == nullptr)
    {
        extra = L"bstrRuleDescription SysAllocString failed";
        goto Cleanup;
    }

    hr = CoCreateInstance(
        __uuidof(NetFwRule),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),
        reinterpret_cast<void**>(&pNetFwRule));

    if (FAILED(hr))
    {
        extra = L"CoCreateInstance failed";
        goto Cleanup;
    }

    pNetFwRule->put_Name(bstrRuleName);
    pNetFwRule->put_Description(bstrRuleDescription);
    pNetFwRule->put_Grouping(_ruleGroup);
    pNetFwRule->put_Profiles(_currentProfilesBitMask);
    pNetFwRule->put_Action(NET_FW_ACTION_BLOCK);
    pNetFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    pNetFwRule->put_Enabled(VARIANT_TRUE);
    pNetFwRule->put_RemoteAddresses(bstrRemoteAddr);

    hr = _pNetFwRules->Add(pNetFwRule);
    if (FAILED(hr))
    {
        extra = L"Add failed";
        goto Cleanup;
    }

    Cleanup:

    if (pNetFwRule != nullptr)
    {
        pNetFwRule->Release();
    }

    SysFreeString(bstrRemoteAddr);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleName);

    if (FAILED(hr))
    {
        ss.str(std::wstring());
        ss.clear();
        ss << L"adding rule failed: " << extra;
        throw Firewall::GenericError(ss.str(), hr);
    }
}

void Firewall::Manager::addBlockInboundAddressRule(std::wstring address)
{
    std::wstring var = std::move(address);
    Firewall::Manager::addBlockInboundAddressRule(var.c_str());
}

// TODO: Refactor.
void Firewall::Manager::pruneRules(int64_t ttl)
{
    HRESULT hr = S_OK;

    ULONG cFetched = 0;
    CComVariant var;
    BSTR bstrName = nullptr;
    BSTR bstrGroup = nullptr;
    BSTR bstrDesc = nullptr;
    std::wstring wstrDesc;

    IUnknown* pEnumerator = nullptr;
    IEnumVARIANT* pVariant = nullptr;

    INetFwRule* pNetFwRule = nullptr;

    long netFwRuleCount;

    _pNetFwPolicy2->get_Rules(&_pNetFwRules);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    hr = _pNetFwRules->get_Count(&netFwRuleCount);
    std::wcout << L"firewall contains total " << netFwRuleCount
               << L" rules\n";
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    _pNetFwRules->get__NewEnum(&pEnumerator);
    if (pEnumerator)
    {
        hr = pEnumerator->QueryInterface(
            __uuidof(IEnumVARIANT),
            reinterpret_cast<void**>(&pVariant));
    }

    while (SUCCEEDED(hr) && hr != S_FALSE)
    {
        var.Clear();
        hr = pVariant->Next(1, &var, &cFetched);

        if (S_FALSE != hr)
        {
            if (SUCCEEDED(hr))
            {
                hr = var.ChangeType(VT_DISPATCH);
            }
            if (SUCCEEDED(hr))
            {
                hr = (V_DISPATCH(&var))->QueryInterface(
                    __uuidof(INetFwRule),
                    reinterpret_cast<void**>(&pNetFwRule));
            }

            if (SUCCEEDED(hr))
            {
                pNetFwRule->get_Grouping(&bstrGroup);
                if (bstrGroup)
                {
                    if (0 == wcscmp(bstrGroup, _ruleGroup))
                    {
                        pNetFwRule->get_Description(&bstrDesc);
                        if (bstrDesc)
                        {
                            std::tm ruleDate{};
                            wstrDesc = std::wstring{bstrDesc, SysStringLen(bstrDesc)};

                            std::wsmatch match;
                            std::regex_search(wstrDesc, match, _descDatePattern);
                            std::wstring matchStr = match[1].str();
                            ruleDate = Utils::ISO8601WStrToTime(matchStr);

                            auto tBegin = std::mktime(&ruleDate);
                            std::tm now = Utils::DateNow();
                            auto tNow = std::mktime(&now);

                            if ((tBegin + ttl) < tNow)
                            {
                                pNetFwRule->get_Name(&bstrName);
                                hr = _pNetFwRules->Remove(bstrName);
                            }
                        }
                    }
                }
                SysFreeString(bstrName);
                SysFreeString(bstrGroup);
                SysFreeString(bstrDesc);
            }
        }
    }

    hr = _pNetFwRules->get_Count(&netFwRuleCount);
    std::wcout << L"after pruning firewall contains total "
               << netFwRuleCount << L" rules\n";
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    Cleanup:

    SysFreeString(bstrName);
    SysFreeString(bstrGroup);
    SysFreeString(bstrDesc);

    if (pNetFwRule != nullptr)
    {
        pNetFwRule->Release();
    }

    if (FAILED(hr))
    {
        throw Firewall::GenericError("pruneRules failed", hr);
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
        reinterpret_cast<void**>(ppNetFwPolicy2));

    return hr;
}
