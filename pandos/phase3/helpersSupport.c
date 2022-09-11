#include "helpersSupport.h"

int getDeviceSemaphoreIndex(int line, int device, int term_is_recv)
{
    return 8 * (line - 3) + (line == 7 ? 2 * device : device) + term_is_recv;
}

size_tt getPTEIndex(memaddr entry_hi)
{
    // processo: prendi EntryHI e ricavi il VPN, poi dal VPN ricavi l'index
    size_tt vpn = entry_hi >> VPNSHIFT;
    size_tt index = vpn - (KUSEG >> VPNSHIFT);

    return index;
}