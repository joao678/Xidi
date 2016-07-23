/*****************************************************************************
 * Xidi
 *      DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016
 *****************************************************************************
 * ImportApiDirectInput.cpp
 *      Implementation of importing the API from the DirectInput library.
 *****************************************************************************/

#include "ApiDirectInput.h"
#include "ImportApiDirectInput.h"

using namespace Xidi;


// -------- CONSTANTS ------------------------------------------------------ //
// See "ImportApiDirectInput.h" for documentation.

#if DIRECTINPUT_VERSION >= 0x0800
const TCHAR* const ImportApiDirectInput::kDirectInputLibraryName = _T("\\dinput8.dll");
const DWORD ImportApiDirectInput::kDirectInputLibraryLength = 12;
#else
const TCHAR* const ImportApiDirectInput::kDirectInputLibraryName = _T("\\dinput.dll");
const DWORD ImportApiDirectInput::kDirectInputLibraryLength = 11;
#endif


// -------- CLASS VARIABLES ------------------------------------------------ //
// See "ImportApiDirectInput.h" for documentation.

ImportApiDirectInput::SImportTable ImportApiDirectInput::importTable;
BOOL ImportApiDirectInput::importTableIsInitialized = FALSE;


// -------- CLASS METHODS -------------------------------------------------- //
// See "ImportApiDirectInput.h" for documentation.

HRESULT ImportApiDirectInput::Initialize(void)
{
    if (FALSE == importTableIsInitialized)
    {
        // Initialize the import table.
        ZeroMemory(&importTable, sizeof(importTable));
        
        // Obtain the full library path string.
        // A path must be specified directly since the system has already loaded this DLL of the same name.
        TCHAR libraryName[1024];
        GetSystemDirectory(libraryName, 512);
        _tcsncat_s(libraryName, _countof(libraryName), kDirectInputLibraryName, kDirectInputLibraryLength);
        
        // Attempt to load the library.
        HMODULE loadedLibrary = LoadLibraryEx(libraryName, NULL, 0);
        if (NULL == loadedLibrary) return E_FAIL;

        // Attempt to obtain the addresses of all imported API functions.
        FARPROC procAddress = NULL;
        
#if DIRECTINPUT_VERSION >= 0x0800
        procAddress = GetProcAddress(loadedLibrary, "DirectInput8Create");
        if (NULL == procAddress) return E_FAIL;
        importTable.DirectInput8Create = (HRESULT(STDMETHODCALLTYPE *)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN))procAddress;
#else
        procAddress = GetProcAddress(loadedLibrary, "DirectInputCreateA");
        if (NULL == procAddress) return E_FAIL;
        importTable.DirectInputCreateA = (HRESULT(STDMETHODCALLTYPE *)(HINSTANCE, DWORD, LPDIRECTINPUTA*, LPUNKNOWN))procAddress;

        procAddress = GetProcAddress(loadedLibrary, "DirectInputCreateW");
        if (NULL == procAddress) return E_FAIL;
        importTable.DirectInputCreateW = (HRESULT(STDMETHODCALLTYPE *)(HINSTANCE, DWORD, LPDIRECTINPUTW*, LPUNKNOWN))procAddress;

        procAddress = GetProcAddress(loadedLibrary, "DirectInputCreateEx");
        if (NULL == procAddress) return E_FAIL;
        importTable.DirectInputCreateEx = (HRESULT(STDMETHODCALLTYPE *)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN))procAddress;
#endif

        procAddress = GetProcAddress(loadedLibrary, "DllRegisterServer");
        if (NULL == procAddress) return E_FAIL;
        importTable.DllRegisterServer = (HRESULT(STDMETHODCALLTYPE *)(void))procAddress;

        procAddress = GetProcAddress(loadedLibrary, "DllUnregisterServer");
        if (NULL == procAddress) return E_FAIL;
        importTable.DllUnregisterServer = (HRESULT(STDMETHODCALLTYPE *)(void))procAddress;

        procAddress = GetProcAddress(loadedLibrary, "DllCanUnloadNow");
        if (NULL == procAddress) return E_FAIL;
        importTable.DllCanUnloadNow = (HRESULT(STDMETHODCALLTYPE *)(void))procAddress;

        procAddress = GetProcAddress(loadedLibrary, "DllGetClassObject");
        if (NULL == procAddress) return E_FAIL;
        importTable.DllGetClassObject = (HRESULT(STDMETHODCALLTYPE *)(_In_ REFCLSID, _In_ REFIID, _Out_ LPVOID*))procAddress;
        
        // Initialization complete.
        importTableIsInitialized = TRUE;
    }

    return S_OK;
}

// ---------

#if DIRECTINPUT_VERSION >= 0x0800
HRESULT ImportApiDirectInput::DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;
    
    return importTable.DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
#else
HRESULT ImportApiDirectInput::DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA*ppDI, LPUNKNOWN punkOuter)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DirectInputCreateA(hinst, dwVersion, ppDI, punkOuter);
}

// ---------

HRESULT ImportApiDirectInput::DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW*ppDI, LPUNKNOWN punkOuter)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DirectInputCreateW(hinst, dwVersion, ppDI, punkOuter);
}

// ---------

HRESULT ImportApiDirectInput::DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DirectInputCreateEx(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
#endif

// ---------

HRESULT ImportApiDirectInput::DllRegisterServer(void)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DllRegisterServer();
}

// ---------

HRESULT ImportApiDirectInput::DllUnregisterServer(void)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DllUnregisterServer();
}

// ---------

HRESULT ImportApiDirectInput::DllCanUnloadNow(void)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DllCanUnloadNow();
}

// ---------

HRESULT ImportApiDirectInput::DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (S_OK != Initialize())
        return E_NOT_VALID_STATE;

    return importTable.DllGetClassObject(rclsid, riid, ppv);
}