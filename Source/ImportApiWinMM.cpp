/*****************************************************************************
 * Xidi
 *      DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016
 *****************************************************************************
 * ImportApiWinMM.cpp
 *      Implementation of importing the API from the WinMM library.
 *****************************************************************************/

#include "ApiWindows.h"
#include "ImportApiWinMM.h"

using namespace Xidi;


// -------- CONSTANTS ------------------------------------------------------ //
// See "ImportApiWinMM.h" for documentation.

const TCHAR* const ImportApiWinMM::kWinMMLibraryName = _T("\\winmm.dll");
const DWORD ImportApiWinMM::kWinMMLibraryLength = 10;


// -------- CLASS VARIABLES ------------------------------------------------ //
// See "ImportApiWinMM.h" for documentation.

ImportApiWinMM::SImportTable ImportApiWinMM::importTable;
BOOL ImportApiWinMM::importTableIsInitialized = FALSE;


// -------- CLASS METHODS -------------------------------------------------- //
// See "ImportApiWinMM.h" for documentation.

MMRESULT ImportApiWinMM::Initialize(void)
{
    if (FALSE == importTableIsInitialized)
    {
        // Initialize the import table.
        ZeroMemory(&importTable, sizeof(importTable));
        
        // Obtain the full library path string.
        // A path must be specified directly since the system has already loaded this DLL of the same name.
        TCHAR libraryName[1024];
        GetSystemDirectory(libraryName, 512);
        _tcsncat_s(libraryName, _countof(libraryName), kWinMMLibraryName, kWinMMLibraryLength);
        
        // Attempt to load the library.
        HMODULE loadedLibrary = LoadLibraryEx(libraryName, NULL, 0);
        if (NULL == loadedLibrary) return MMSYSERR_ERROR;
        
        // Attempt to obtain the addresses of all imported API functions.
        FARPROC procAddress = NULL;
        
        procAddress = GetProcAddress(loadedLibrary, "auxGetDevCapsA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxGetDevCapsA = (MMRESULT(WINAPI *)(UINT_PTR, LPAUXCAPSA, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "auxGetDevCapsW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxGetDevCapsW = (MMRESULT(WINAPI *)(UINT_PTR, LPAUXCAPSW, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "auxGetNumDevs");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxGetNumDevs = (UINT(WINAPI *)(void))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "auxGetVolume");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxGetVolume = (MMRESULT(WINAPI *)(UINT, LPDWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "auxOutMessage");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxOutMessage = (MMRESULT(WINAPI *)(UINT, UINT, DWORD_PTR, DWORD_PTR))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "auxSetVolume");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.auxSetVolume = (MMRESULT(WINAPI *)(UINT, DWORD))procAddress;
        
        // ---------
        
        procAddress = GetProcAddress(loadedLibrary, "joyConfigChanged");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyConfigChanged = (MMRESULT(WINAPI *)(DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetDevCapsA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetDevCapsA = (MMRESULT(WINAPI *)(UINT_PTR, LPJOYCAPSA, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetDevCapsW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetDevCapsW = (MMRESULT(WINAPI *)(UINT_PTR, LPJOYCAPSW, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetNumDevs");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetNumDevs = (UINT(WINAPI *)(void))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetPos");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetPos = (MMRESULT(WINAPI *)(UINT, LPJOYINFO))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetPosEx");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetPosEx = (MMRESULT(WINAPI *)(UINT, LPJOYINFOEX))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyGetThreshold");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyGetThreshold = (MMRESULT(WINAPI *)(UINT, LPUINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joyReleaseCapture");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joyReleaseCapture = (MMRESULT(WINAPI *)(UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joySetCapture");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joySetCapture = (MMRESULT(WINAPI *)(HWND, UINT, UINT, BOOL))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "joySetThreshold");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.joySetThreshold = (MMRESULT(WINAPI *)(UINT, UINT))procAddress;
        
        // ---------
        
        procAddress = GetProcAddress(loadedLibrary, "timeBeginPeriod");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeBeginPeriod = (MMRESULT(WINAPI *)(UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeEndPeriod");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeEndPeriod = (MMRESULT(WINAPI *)(UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeGetDevCaps");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeGetDevCaps = (MMRESULT(WINAPI *)(LPTIMECAPS, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeGetSystemTime");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeGetSystemTime = (MMRESULT(WINAPI *)(LPMMTIME, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeGetTime");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeGetTime = (DWORD(WINAPI *)(void))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeKillEvent");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeKillEvent = (MMRESULT(WINAPI *)(UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "timeSetEvent");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.timeSetEvent = (MMRESULT(WINAPI *)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT))procAddress;
        
        // ---------
        
        procAddress = GetProcAddress(loadedLibrary, "waveInAddBuffer");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInAddBuffer = (MMRESULT(WINAPI *)(HWAVEIN, LPWAVEHDR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInClose");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInClose = (MMRESULT(WINAPI *)(HWAVEIN))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetDevCapsA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetDevCapsA = (MMRESULT(WINAPI *)(UINT_PTR, LPWAVEINCAPSA, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetDevCapsW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetDevCapsW = (MMRESULT(WINAPI *)(UINT_PTR, LPWAVEINCAPSW, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetErrorTextA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetErrorTextA = (MMRESULT(WINAPI *)(MMRESULT, LPCSTR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetErrorTextW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetErrorTextW = (MMRESULT(WINAPI *)(MMRESULT, LPWSTR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetID");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetID = (MMRESULT(WINAPI *)(HWAVEIN, LPUINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetNumDevs");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetNumDevs = (UINT(WINAPI *)(void))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInGetPosition");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInGetPosition = (MMRESULT(WINAPI *)(HWAVEIN, LPMMTIME, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInMessage");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInMessage = (DWORD(WINAPI *)(HWAVEIN, UINT, DWORD_PTR, DWORD_PTR))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInOpen");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInOpen = (MMRESULT(WINAPI *)(LPHWAVEIN, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInPrepareHeader");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInPrepareHeader = (MMRESULT(WINAPI *)(HWAVEIN, LPWAVEHDR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInReset");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInReset = (MMRESULT(WINAPI *)(HWAVEIN))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInStart");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInStart = (MMRESULT(WINAPI *)(HWAVEIN))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInStop");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInStop = (MMRESULT(WINAPI *)(HWAVEIN))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveInUnprepareHeader");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveInUnprepareHeader = (MMRESULT(WINAPI *)(HWAVEIN, LPWAVEHDR, UINT))procAddress;
        
        // ---------
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutBreakLoop");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutBreakLoop = (MMRESULT(WINAPI *)(HWAVEOUT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutClose");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutClose = (MMRESULT(WINAPI *)(HWAVEOUT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetDevCapsA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetDevCapsA = (MMRESULT(WINAPI *)(UINT_PTR, LPWAVEOUTCAPSA, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetDevCapsW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetDevCapsW = (MMRESULT(WINAPI *)(UINT_PTR, LPWAVEOUTCAPSW, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetErrorTextA");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetErrorTextA = (MMRESULT(WINAPI *)(MMRESULT, LPCSTR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetErrorTextW");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetErrorTextW = (MMRESULT(WINAPI *)(MMRESULT, LPWSTR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetID");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetID = (MMRESULT(WINAPI *)(HWAVEOUT, LPUINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetNumDevs");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetNumDevs = (UINT(WINAPI *)(void))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetPitch");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetPitch = (MMRESULT(WINAPI *)(HWAVEOUT, LPDWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetPlaybackRate");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetPlaybackRate = (MMRESULT(WINAPI *)(HWAVEOUT, LPDWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetPosition");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetPosition = (MMRESULT(WINAPI *)(HWAVEOUT, LPMMTIME, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutGetVolume");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutGetVolume = (MMRESULT(WINAPI *)(HWAVEOUT, LPDWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutMessage");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutMessage = (DWORD(WINAPI *)(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutOpen");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutOpen = (MMRESULT(WINAPI *)(LPHWAVEOUT, UINT_PTR, LPWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutPause");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutPause = (MMRESULT(WINAPI *)(HWAVEOUT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutPrepareHeader");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutPrepareHeader = (MMRESULT(WINAPI *)(HWAVEOUT, LPWAVEHDR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutReset");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutReset = (MMRESULT(WINAPI *)(HWAVEOUT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutRestart");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutRestart = (MMRESULT(WINAPI *)(HWAVEOUT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutSetPitch");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutSetPitch = (MMRESULT(WINAPI *)(HWAVEOUT, DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutSetPlaybackRate");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutSetPlaybackRate = (MMRESULT(WINAPI *)(HWAVEOUT, DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutSetVolume");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutSetVolume = (MMRESULT(WINAPI *)(HWAVEOUT, DWORD))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutUnprepareHeader");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutUnprepareHeader = (MMRESULT(WINAPI *)(HWAVEOUT, LPWAVEHDR, UINT))procAddress;
        
        procAddress = GetProcAddress(loadedLibrary, "waveOutWrite");
        if (NULL == procAddress) return MMSYSERR_ERROR;
        importTable.waveOutWrite = (MMRESULT(WINAPI *)(HWAVEOUT, LPWAVEHDR, UINT))procAddress;
        
        // Initialization complete.
        importTableIsInitialized = TRUE;
    }

    return MMSYSERR_NOERROR;
}

// ---------

MMRESULT ImportApiWinMM::auxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPSA lpCaps, UINT cbCaps)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.auxGetDevCapsA(uDeviceID, lpCaps, cbCaps);
}

// ---------

MMRESULT ImportApiWinMM::auxGetDevCapsW(UINT_PTR uDeviceID, LPAUXCAPSW lpCaps, UINT cbCaps)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.auxGetDevCapsW(uDeviceID, lpCaps, cbCaps);
}

// ---------

UINT ImportApiWinMM::auxGetNumDevs(void)
{
    if (MMSYSERR_NOERROR != Initialize())
        return 0;

    return importTable.auxGetNumDevs();
}

// ---------

MMRESULT ImportApiWinMM::auxGetVolume(UINT uDeviceID, LPDWORD lpdwVolume)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.auxGetVolume(uDeviceID, lpdwVolume);
}

// ---------

MMRESULT ImportApiWinMM::auxOutMessage(UINT uDeviceID, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.auxOutMessage(uDeviceID, uMsg, dwParam1, dwParam2);
}

// ---------

MMRESULT ImportApiWinMM::auxSetVolume(UINT uDeviceID, DWORD dwVolume)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.auxSetVolume(uDeviceID, dwVolume);
}

// ---------

MMRESULT ImportApiWinMM::joyConfigChanged(DWORD dwFlags)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyConfigChanged(dwFlags);
}

// ---------

MMRESULT ImportApiWinMM::joyGetDevCapsA(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyGetDevCapsA(uJoyID, pjc, cbjc);
}

// ---------

MMRESULT ImportApiWinMM::joyGetDevCapsW(UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyGetDevCapsW(uJoyID, pjc, cbjc);
}

// ---------

UINT ImportApiWinMM::joyGetNumDevs(void)
{
    if (MMSYSERR_NOERROR != Initialize())
        return 0;

    return importTable.joyGetNumDevs();
}

// ---------

MMRESULT ImportApiWinMM::joyGetPos(UINT uJoyID, LPJOYINFO pji)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyGetPos(uJoyID, pji);
}

// ---------

MMRESULT ImportApiWinMM::joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyGetPosEx(uJoyID, pji);
}

// ---------

MMRESULT ImportApiWinMM::joyGetThreshold(UINT uJoyID, LPUINT puThreshold)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyGetThreshold(uJoyID, puThreshold);
}

// ---------

MMRESULT ImportApiWinMM::joyReleaseCapture(UINT uJoyID)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joyReleaseCapture(uJoyID);
}

// ---------

MMRESULT ImportApiWinMM::joySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joySetCapture(hwnd, uJoyID, uPeriod, fChanged);
}

// ---------

MMRESULT ImportApiWinMM::joySetThreshold(UINT uJoyID, UINT uThreshold)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.joySetThreshold(uJoyID, uThreshold);
}

// ---------

MMRESULT ImportApiWinMM::timeBeginPeriod(UINT uPeriod)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeBeginPeriod(uPeriod);
}

// ---------

MMRESULT ImportApiWinMM::timeEndPeriod(UINT uPeriod)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeEndPeriod(uPeriod);
}

// ---------

MMRESULT ImportApiWinMM::timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeGetDevCaps(ptc, cbtc);
}

// ---------

MMRESULT ImportApiWinMM::timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeGetSystemTime(pmmt, cbmmt);
}

// ---------

DWORD ImportApiWinMM::timeGetTime(void)
{
    if (MMSYSERR_NOERROR != Initialize())
        return 0;

    return importTable.timeGetTime();
}

// ---------

MMRESULT ImportApiWinMM::timeKillEvent(UINT uTimerID)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeKillEvent(uTimerID);
}

// ---------

MMRESULT ImportApiWinMM::timeSetEvent(UINT uDelay, UINT uResolution, LPTIMECALLBACK lpTimeProc, DWORD_PTR dwUser, UINT fuEvent)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.timeSetEvent(uDelay, uResolution, lpTimeProc, dwUser, fuEvent);
}

// ---------

MMRESULT ImportApiWinMM::waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInAddBuffer(hwi, pwh, cbwh);
}

// ---------

MMRESULT ImportApiWinMM::waveInClose(HWAVEIN hwi)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInClose(hwi);
}

// ---------

MMRESULT ImportApiWinMM::waveInGetDevCapsA(UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetDevCapsA(uDeviceID, pwic, cbwic);
}

// ---------

MMRESULT ImportApiWinMM::waveInGetDevCapsW(UINT_PTR uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetDevCapsW(uDeviceID, pwic, cbwic);
}

// ---------

MMRESULT ImportApiWinMM::waveInGetErrorTextA(MMRESULT mmrError, LPCSTR pszText, UINT cchText)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetErrorTextA(mmrError, pszText, cchText);
}

// ---------

MMRESULT ImportApiWinMM::waveInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetErrorTextW(mmrError, pszText, cchText);
}

// ---------

MMRESULT ImportApiWinMM::waveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetID(hwi, puDeviceID);
}

// ---------

UINT ImportApiWinMM::waveInGetNumDevs(void)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetNumDevs();
}

// ---------

MMRESULT ImportApiWinMM::waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInGetPosition(hwi, pmmt, cbmmt);
}

// ---------

DWORD ImportApiWinMM::waveInMessage(HWAVEIN deviceID, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInMessage(deviceID, uMsg, dwParam1, dwParam2);
}

// ---------

MMRESULT ImportApiWinMM::waveInOpen(LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInOpen(phwi, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
}

// ---------

MMRESULT ImportApiWinMM::waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInPrepareHeader(hwi, pwh, cbwh);
}

// ---------

MMRESULT ImportApiWinMM::waveInReset(HWAVEIN hwi)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInReset(hwi);
}

// ---------

MMRESULT ImportApiWinMM::waveInStart(HWAVEIN hwi)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInStart(hwi);
}

// ---------

MMRESULT ImportApiWinMM::waveInStop(HWAVEIN hwi)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInStop(hwi);
}

// ---------

MMRESULT ImportApiWinMM::waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveInUnprepareHeader(hwi, pwh, cbwh);
}

// ---------

MMRESULT ImportApiWinMM::waveOutBreakLoop(HWAVEOUT hwo)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutBreakLoop(hwo);
}

// ---------

MMRESULT ImportApiWinMM::waveOutClose(HWAVEOUT hwo)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutClose(hwo);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetDevCapsA(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetDevCapsA(uDeviceID, pwoc, cbwoc);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetDevCapsW(UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetDevCapsW(uDeviceID, pwoc, cbwoc);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetErrorTextA(MMRESULT mmrError, LPCSTR pszText, UINT cchText)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetErrorTextA(mmrError, pszText, cchText);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetErrorTextW(mmrError, pszText, cchText);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetID(hwo, puDeviceID);
}

// ---------

UINT ImportApiWinMM::waveOutGetNumDevs(void)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetNumDevs();
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetPitch(hwo, pdwPitch);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetPlaybackRate(hwo, pdwRate);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetPosition(hwo, pmmt, cbmmt);
}

// ---------

MMRESULT ImportApiWinMM::waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutGetVolume(hwo, pdwVolume);
}

// ---------

DWORD ImportApiWinMM::waveOutMessage(HWAVEOUT deviceID, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutMessage(deviceID, uMsg, dwParam1, dwParam2);
}

// ---------

MMRESULT ImportApiWinMM::waveOutOpen(LPHWAVEOUT phwo, UINT_PTR uDeviceID, LPWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwCallbackInstance, DWORD fdwOpen)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutOpen(phwo, uDeviceID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);
}

// ---------

MMRESULT ImportApiWinMM::waveOutPause(HWAVEOUT hwo)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutPause(hwo);
}

// ---------

MMRESULT ImportApiWinMM::waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;

    return importTable.waveOutPrepareHeader(hwo, pwh, cbwh);
}

// ---------

MMRESULT ImportApiWinMM::waveOutReset(HWAVEOUT hwo)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutReset(hwo);
}

// ---------

MMRESULT ImportApiWinMM::waveOutRestart(HWAVEOUT hwo)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutRestart(hwo);
}

// ---------

MMRESULT ImportApiWinMM::waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutSetPitch(hwo, dwPitch);
}

// ---------

MMRESULT ImportApiWinMM::waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutSetPlaybackRate(hwo, dwRate);
}

// ---------

MMRESULT ImportApiWinMM::waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutSetVolume(hwo, dwVolume);
}

// ---------

MMRESULT ImportApiWinMM::waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutUnprepareHeader(hwo, pwh, cbwh);
}

// ---------

MMRESULT ImportApiWinMM::waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
    if (MMSYSERR_NOERROR != Initialize())
        return MMSYSERR_ERROR;
    
    return importTable.waveOutWrite(hwo, pwh, cbwh);
}
