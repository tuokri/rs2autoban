# rs2autoban

Rising Storm 2: Vietnam dedicated server DDoS protection.

**WORK IN PROGRESS!**

## Operation

Watches server logs, automatically blocking suspicious inbound IP addresses
using Windows firewall. IP addresses are interpreted suspicious if they
cannot be associated with a valid Steam ID. RS2: Vietnam WebAdmin login IPs are
ignored and never interpreted suspicious.

When a log line containing an IP address is found, there is a configurable
grace period before the address is blocked. If a valid Steam ID is found for the
address during the grace period, the address will not be blocked. Lately seen
Steam IDs and the valid IP addresses associated with them are cached to avoid
false positives.

When an IP address is blocked, the block rule remains enabled for a configurable
duration, after which the block rule is automatically removed for the address.

The number of times an address must be seen in a log file before triggering
a block may also be configured. If this threshold is set, suspicious addresses
are not blocked until they exceed the threshold.
