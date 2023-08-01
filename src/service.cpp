#include "service.h"

// This function borrowed from here: https://www.appsloveworld.com/cplus/100/230/how-to-check-that-the-current-process-is-running-as-windows-service-using-winapi
// This solution works because all Windows services run in Session 0.
BOOL IsWindowsService()
{
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    return sessionId == 0;
}
