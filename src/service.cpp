#include "service.h"

BOOL IsWindowsService()
{
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    return !sessionId;
}
