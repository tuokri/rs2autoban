#pragma once

#ifndef RS2AUTOBAN_FIREWALL_HPP
#define RS2AUTOBAN_FIREWALL_HPP

#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <cinttypes>
#include <regex>

#include <Windows.h>
#include <netfw.h>

#include <QObject>
#include <QTimer>
#include <QLoggingCategory>

#include "Utils.hpp"

Q_DECLARE_LOGGING_CATEGORY(fwGeneric)

namespace Firewall
{

inline static const WCHAR* RS2AUTOBAN_GROUP_NAME = L"__RS2AUTOBAN_GROUP";
inline static const WCHAR* RS2AUTOBAN_RULE_DESC = L"RS2Autoban automatic rule.";
inline static const WCHAR* RS2AUTOBAN_RULE_NAME = L"__RS2AUTOBAN_RULE";

class GenericError: public std::exception
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
        ss << _wmsg << L" "
           << L"(0x" << std::hex << _hr << L") "
           << Utils::HRESULTToWString(_hr);
        return ss.str();
    };

private:

    std::wstring _wmsg;
    HRESULT _hr;
};

class Manager: public QObject
{
Q_OBJECT

public:
    explicit Manager(uint64_t ttl = DEFAULT_TTL,
                     uint64_t gracePeriod = DEFAULT_GRACE_PERIOD,
                     QObject* parent = nullptr);

    ~Manager() override;

    void setTtl(uint64_t ttl);

    void setGracePeriod(uint64_t gracePeriod);

    /**
     * Add rule blocking inbound connections from address.
     *
     * @param address Remote address.
     */
    void addBlockInboundAddressRule(const WCHAR* address);

    /**
     * Add rule blocking inbound connections from address.
     *
     * @param address Remote address.
     */
    void addBlockInboundAddressRule(const std::wstring& address);

    /**
     * Add rule blocking inbound connections from address.
     *
     * @param address Remote address.
     */
    void addBlockInboundAddressRule(const QString& address);

    static const unsigned DEFAULT_TTL = 3600U;
    static const unsigned DEFAULT_GRACE_PERIOD = 15U;

private:

    static HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

public slots:

    void startPruneTimer();

    void stopPruneTimer();

    /**
     * Prune old rules.
     *
     * @param ttl Time to live. Autoban rules older than ttl
     * are removed from the firewall.
     */
    void pruneRules(uint64_t ttl = DEFAULT_TTL);

private:
    BSTR _ruleGroup;
    INetFwPolicy2* _pNetFwPolicy2 = nullptr;
    INetFwRules* _pNetFwRules = nullptr;
    HRESULT _hrComInit;
    long _currentProfilesBitMask = 0;
    const std::wregex _descDatePattern{L".*\\[(.*)\\].*"};
    uint64_t _ttl = DEFAULT_TTL;
    uint64_t _gracePeriod = DEFAULT_GRACE_PERIOD;
    QTimer* _pruneTimer;
};

}

#endif //RS2AUTOBAN_FIREWALL_HPP
