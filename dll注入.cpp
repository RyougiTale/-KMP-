#include <windows.h>
 
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
    DWORD dwCurProcessId = *((DWORD*)lParam); 
    DWORD dwProcessId = 0; 
 
    GetWindowThreadProcessId(hwnd, &dwProcessId); 
    if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
    { 
        *((HWND *)lParam) = hwnd;
        return FALSE; 
    } 
    return TRUE; 
} 
 
 
HWND GetMainWindow() 
{ 
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if(!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
    {     
        return (HWND)dwCurrentProcessId; 
    } 
    return NULL; 
} 
 
BOOLEAN WINAPI DllMain(
        IN HINSTANCE hDllHandle, 
        IN DWORD     nReason,    
        IN LPVOID    Reserved)
{
    BOOLEAN bSuccess = TRUE;
 
    switch ( nReason )
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(GetMainWindow(), TEXT("OMG~ You are Attacked!"), TEXT("Warning"), MB_ICONWARNING);
        break;
 
    case DLL_PROCESS_DETACH:
        break;
    }
 
    return bSuccess;
}
