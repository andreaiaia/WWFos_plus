#include "helpersSupport.h"

int getDeviceSemaphoreIndex(int line, int device, int term_is_recv)
{
    return 8 * (line - 3) + (line == 7 ? 2 * device : device) + term_is_recv;
}