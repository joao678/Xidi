/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2021
 *************************************************************************//**
 * @file Strings.h
 *   Declaration of common strings and functions to manipulate them.
 *****************************************************************************/

#pragma once

#include "ControllerTypes.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>


// -------- MACROS --------------------------------------------------------- //

// Strings that need to be available in multiple formats (ASCII and Unicode).
#define XIDI_AXIS_NAME_X                    "X Axis"
#define XIDI_AXIS_NAME_Y                    "Y Axis"
#define XIDI_AXIS_NAME_Z                    "Z Axis"
#define XIDI_AXIS_NAME_RX                   "RotX Axis"
#define XIDI_AXIS_NAME_RY                   "RotY Axis"
#define XIDI_AXIS_NAME_RZ                   "RotZ Axis"
#define XIDI_AXIS_NAME_UNKNOWN              "Unknown Axis"
#define XIDI_BUTTON_NAME_FORMAT             "Button %u"
#define XIDI_POV_NAME                       "POV"
#define XIDI_WHOLE_CONTROLLER_NAME          "Whole Controller"


namespace Xidi
{
    namespace Strings
    {
        // -------- COMPILE-TIME CONSTANTS --------------------------------- //
        // Can safely be used at any time, including to perform static initialization.
        // Views are guaranteed to be null-terminated.

        /// Base name of the DirectInput library to import.
        inline constexpr std::wstring_view kStrLibraryNameDirectInput = L"dinput.dll";

        /// Base name of the DirectInput8 library to import.
        inline constexpr std::wstring_view kStrLibraryNameDirectInput8 = L"dinput8.dll";

        /// Base name of the WinMM library to import.
        inline constexpr std::wstring_view kStrLibraryNameWinMM = L"winmm.dll";
        
        /// Configuration file section name for overriding import libraries.
        inline constexpr std::wstring_view kStrConfigurationSectionImport = L"Import";

        /// Configuration file setting for overriding import for DirectInput.
        inline constexpr std::wstring_view kStrConfigurationSettingImportDirectInput = kStrLibraryNameDirectInput;

        /// Configuration file setting for overriding import for DirectInput8.
        inline constexpr std::wstring_view kStrConfigurationSettingImportDirectInput8 = kStrLibraryNameDirectInput8;

        /// Configuration file setting for overriding import for WinMM.
        inline constexpr std::wstring_view kStrConfigurationSettingImportWinMM = kStrLibraryNameWinMM;

        /// Configuration file setting separator for generating per-controller setting strings.
        inline constexpr wchar_t kCharConfigurationSettingSeparator = L'.';

        /// Configuration file section name for log-related settings.
        inline constexpr std::wstring_view kStrConfigurationSectionLog = L"Log";

        /// Configuration file setting for specifying if the log is enabled.
        inline constexpr std::wstring_view kStrConfigurationSettingLogEnabled = L"Enabled";

        /// Configuration file setting for specifying the logging verbosity level.
        inline constexpr std::wstring_view kStrConfigurationSettingLogLevel = L"Level";

        /// Configuration file section name for mapper-related settings.
        inline constexpr std::wstring_view kStrConfigurationSectionMapper = L"Mapper";

        /// Configuration file setting for specifying the mapper type.
        inline constexpr std::wstring_view kStrConfigurationSettingMapperType = L"Type";


        // -------- RUN-TIME CONSTANTS ------------------------------------- //
        // Not safe to access before run-time, and should not be used to perform dynamic initialization.
        // Views are guaranteed to be null-terminated.

        /// Product name.
        /// Use this to identify Xidi in areas of user interaction.
        extern const std::wstring_view kStrProductName;

        /// Form name.
        /// Use this to identify Xidi's form (dinput, dinput8, winmm) in areas of user interaction.
        extern const std::wstring_view kStrFormName;

        /// Complete path and filename of the currently-running executable.
        extern const std::wstring_view kStrExecutableCompleteFilename;

        /// Base name of the currently-running executable.
        extern const std::wstring_view kStrExecutableBaseName;

        /// Directory name of the currently-running executable, including trailing backslash if available.
        extern const std::wstring_view kStrExecutableDirectoryName;

        /// Complete path and filename of the currently-running form of Xidi.
        extern const std::wstring_view kStrXidiCompleteFilename;

        /// Base name of the currently-running form of Xidi.
        extern const std::wstring_view kStrXidiBaseName;

        /// Directory name of the currently-running form of Xidi, including trailing backslash if available.
        extern const std::wstring_view kStrXidiDirectoryName;

        /// Directory name in which system-supplied libraries are found.
        extern const std::wstring_view kStrSystemDirectoryName;

        /// Complete path and filename of the system-supplied DirectInput library.
        extern const std::wstring_view kStrSystemLibraryFilenameDirectInput;

        /// Complete path and filename of the system-supplied DirectInput8 library.
        extern const std::wstring_view kStrSystemLibraryFilenameDirectInput8;

        /// Complete path and filename of the system-supplied WinMM library.
        extern const std::wstring_view kStrSystemLibraryFilenameWinMM;

        /// Expected filename of a configuration file.
        /// Xidi configuration filename = (executable directory)\Xidi.ini
        extern const std::wstring_view kStrConfigurationFilename;

        /// Expected filename for the log file.
        /// Xidi log filename = (current user's desktop)\Xidi_(Xidi Version)_(base name of the running executable)_(process ID).log
        extern const std::wstring_view kStrLogFilename;


        // -------- FUNCTIONS ---------------------------------------------- //

        /// Retrieves a string used to represent a per-controller mapper type configuration setting.
        /// These are initialized on first invocation and returned subsequently as read-only views.
        /// An empty view is returned if an invalid controller identifier is specified.
        /// @param [in] controllerIdentifier Controller identifier for which a string is desired.
        /// @return Corresponding configuration setting string, or an empty view if the controller identifier is out of range.
        std::wstring_view MapperTypeConfigurationNameString(Controller::TControllerIdentifier controllerIdentifier);

        /// Generates a string representation of a system error code.
        /// @param [in] systemErrorCode System error code for which to generate a string.
        /// @return String representation of the system error code.
        std::wstring SystemErrorCodeString(const unsigned long systemErrorCode);
    }
}
