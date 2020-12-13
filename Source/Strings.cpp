/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2020
 *************************************************************************//**
 * @file Strings.cpp
 *   Implementation of functions for manipulating Xidi-specific strings.
 *****************************************************************************/

#include "ApiWindows.h"
#include "Globals.h"
#include "Strings.h"
#include "TemporaryBuffer.h"

#include <cstdlib>
#include <intrin.h>
#include <mutex>
#include <psapi.h>
#include <shlobj.h>
#include <sstream>
#include <string>
#include <string_view>


namespace Xidi
{
    namespace Strings
    {
        // -------- INTERNAL CONSTANTS ------------------------------------- //

        /// File extension for a configuration file.
        static constexpr std::wstring_view kStrConfigurationFileExtension = L".ini";

        /// File extension for a log file.
        static constexpr std::wstring_view kStrLogFileExtension = L".log";


        // -------- INTERNAL FUNCTIONS ------------------------------------- //

        /// Generates the value for kStrProductName; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetProductName(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                const wchar_t* stringStart = nullptr;
                const int stringLength = LoadString(Globals::GetInstanceHandle(), IDS_XIDI_PRODUCT_NAME, (wchar_t*)&stringStart, 0);

                if (0 < stringLength)
                    initString.assign(stringStart, &stringStart[stringLength]);
            });

            return initString;
        }

        /// Generates the value for kStrVersionName; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetVersionName(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                const wchar_t* stringStart = nullptr;
                const int stringLength = LoadString(Globals::GetInstanceHandle(), IDS_XIDI_VERSION_NAME, (wchar_t*)&stringStart, 0);

                if (0 != stringLength)
                    initString.assign(stringStart, &stringStart[stringLength]);
            });

            return initString;
        }

        /// Generates the value for kStrExecutableCompleteFilename; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetExecutableCompleteFilename(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                TemporaryBuffer<wchar_t> buf;
                GetModuleFileName(nullptr, buf, (DWORD)buf.Count());

                initString.assign(buf);
            });

            return initString;
        }

        /// Generates the value for kStrExecutableBaseName; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetExecutableBaseName(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                const wchar_t* const executableCompleteFilename = GetExecutableCompleteFilename().c_str();
                const wchar_t* executableBaseName = wcsrchr(executableCompleteFilename, L'\\');
                if (nullptr == executableBaseName)
                    executableBaseName = executableCompleteFilename;
                else
                    executableBaseName += 1;

                initString.assign(executableBaseName);
            });

            return initString;
        }

        /// Generates the value for kStrExecutableDirectoryName; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetExecutableDirectoryName(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                const wchar_t* const executableCompleteFilename = GetExecutableCompleteFilename().c_str();
                const wchar_t* const lastBackslash = wcsrchr(executableCompleteFilename, L'\\');
                if (nullptr != lastBackslash)
                    initString.assign(executableCompleteFilename, &lastBackslash[1]);
            });

            return initString;
        }

        /// Generates the value for kStrSystemDirectoryName; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetSystemDirectoryName(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                TemporaryBuffer<wchar_t> buf;
                const UINT numChars = GetSystemDirectory(buf, buf.Count() - 1);

                if (L'\\' != buf[numChars - 1])
                {
                    buf[numChars] = L'\\';
                    buf[numChars + 1] = L'\0';
                }

                initString.assign(buf);
            });

            return initString;
        }

        /// Generates the value for kStrSystemLibraryFilenameDirectInput; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetSystemLibraryFilenameDirectInput(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                std::wstring_view pieces[] = {GetSystemDirectoryName(), kStrLibraryNameDirectInput};

                int totalLength = 0;
                for (int i = 0; i < _countof(pieces); ++i)
                    totalLength += pieces[i].length();

                initString.reserve(1 + totalLength);

                for (int i = 0; i < _countof(pieces); ++i)
                    initString.append(pieces[i]);
            });

            return initString;
        }

        /// Generates the value for kStrSystemLibraryFilenameDirectInput8; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetSystemLibraryFilenameDirectInput8(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                std::wstring_view pieces[] = {GetSystemDirectoryName(), kStrLibraryNameDirectInput8};

                int totalLength = 0;
                for (int i = 0; i < _countof(pieces); ++i)
                    totalLength += pieces[i].length();

                initString.reserve(1 + totalLength);

                for (int i = 0; i < _countof(pieces); ++i)
                    initString.append(pieces[i]);
            });

            return initString;
        }

        /// Generates the value for kStrSystemLibraryFilenameWinMM; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetSystemLibraryFilenameWinMM(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                std::wstring_view pieces[] = {GetSystemDirectoryName(), kStrLibraryNameWinMM};

                int totalLength = 0;
                for (int i = 0; i < _countof(pieces); ++i)
                    totalLength += pieces[i].length();

                initString.reserve(1 + totalLength);

                for (int i = 0; i < _countof(pieces); ++i)
                    initString.append(pieces[i]);
            });

            return initString;
        }

        /// Generates the value for kStrConfigurationFilename; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static std::wstring GetConfigurationFilename(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                std::wstring_view pieces[] = {GetExecutableDirectoryName(), GetProductName(), kStrConfigurationFileExtension};

                int totalLength = 0;
                for (int i = 0; i < _countof(pieces); ++i)
                    totalLength += pieces[i].length();

                initString.reserve(1 + totalLength);

                for (int i = 0; i < _countof(pieces); ++i)
                    initString.append(pieces[i]);
            });

            return initString;
        }

        /// Generates the value for kStrLogFilename; see documentation of this run-time constant for more information.
        /// @return Corresponding run-time constant value.
        static const std::wstring& GetLogFilename(void)
        {
            static std::wstring initString;
            static std::once_flag initFlag;

            std::call_once(initFlag, []() -> void {
                std::wstringstream logFilename;

                PWSTR knownFolderPath;
                const HRESULT result = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &knownFolderPath);

                if (S_OK == result)
                {
                    logFilename << knownFolderPath << L'\\';
                    CoTaskMemFree(knownFolderPath);
                }

                logFilename << GetProductName() << L'_' << GetVersionName() << L'_' << GetExecutableBaseName() << L'_' << Globals::GetCurrentProcessId() << kStrLogFileExtension;

                initString.assign(logFilename.str());
            });

            return initString;
        }


        // -------- RUN-TIME CONSTANTS ------------------------------------- //
        // See "Strings.h" for documentation.

        extern const std::wstring_view kStrProductName(GetProductName());
        extern const std::wstring_view kStrVersionName(GetVersionName());
        extern const std::wstring_view kStrExecutableCompleteFilename(GetExecutableCompleteFilename());
        extern const std::wstring_view kStrExecutableBaseName(GetExecutableBaseName());
        extern const std::wstring_view kStrExecutableDirectoryName(GetExecutableDirectoryName());
        extern const std::wstring_view kStrSystemDirectoryName(GetSystemDirectoryName());
        extern const std::wstring_view kStrSystemLibraryFilenameDirectInput(GetSystemLibraryFilenameDirectInput());
        extern const std::wstring_view kStrSystemLibraryFilenameDirectInput8(GetSystemLibraryFilenameDirectInput8());
        extern const std::wstring_view kStrSystemLibraryFilenameWinMM(GetSystemLibraryFilenameWinMM());
        extern const std::wstring_view kStrConfigurationFilename(GetConfigurationFilename());
        extern const std::wstring_view kStrLogFilename(GetLogFilename());


        // -------- FUNCTIONS ---------------------------------------------- //
        // See "Strings.h" for documentation.

        std::wstring SystemErrorCodeString(const unsigned long systemErrorCode)
        {
            TemporaryBuffer<wchar_t> systemErrorString;
            DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, systemErrorCode, 0, systemErrorString, systemErrorString.Count(), nullptr);

            if (0 == systemErrorLength)
            {
                swprintf_s(systemErrorString, systemErrorString.Count(), L"System error %u.", (unsigned int)systemErrorCode);
            }
            else
            {
                for (; systemErrorLength > 0; --systemErrorLength)
                {
                    if (L'\0' != systemErrorString[systemErrorLength] && !iswspace(systemErrorString[systemErrorLength]))
                        break;

                    systemErrorString[systemErrorLength] = L'\0';
                }
            }

            return std::wstring(systemErrorString);
        }
    }
}
