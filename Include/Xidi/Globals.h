/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file Globals.h
 *   Declaration of a namespace for storing and retrieving global data.
 *   Intended for miscellaneous data elements with no other suitable place.
 *****************************************************************************/

#pragma once

#include "ApiStdString.h"
#include "ApiWindows.h"


namespace Xidi
{
    /// Encapsulates all miscellaneous global data elements with no other suitable location.
    /// Not intended to be instantiated.
    class Globals
    {
    private:
        // -------- CONSTANTS ---------------------------------------------- //
        
        /// Holds the name of the DirectInput library to load from the system directory.
        static const StdString kDInputLibraryName;

        /// Holds the name of the DirectInput8 library to load from the system directory.
        static const StdString kDInput8LibraryName;

        /// Holds the name of the WinMM library to load from the system directory.
        static const StdString kWinMMLibraryName;

        /// Maximum length of the system directory name.
        static const size_t kMaximumSystemDirectoryNameLength = 2048;
        
        
        // -------- CLASS VARIABLES ---------------------------------------- //

        /// Handle of the instance that represents the running form of Xidi, be it the library or the test application.
        static HINSTANCE gInstanceHandle;

        /// Holds the path to a custom library that overrides the default import library for DirectInput functions.
        static StdString gOverrideImportDirectInput;

        /// Holds the path to a custom library that overrides the default import library for DirectInput8 functions.
        static StdString gOverrideImportDirectInput8;

        /// Holds the path to a custom library that overrides the default import library for WinMM functions.
        static StdString gOverrideImportWinMM;


        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Default constructor. Should never be invoked.
        Globals(void);


    public:
        // -------- CLASS METHODS ------------------------------------------ //

        /// Applies a setting that specifies a custom path for the import library for DirectInput functions.
        /// @param [in] value Setting value to be applied.
        /// @return `true` if setting could be applied, `false` otherwise.
        static bool ApplyOverrideImportDirectInput(StdString& value);

        /// Applies a setting that specifies a custom path for the import library for DirectInput8 functions.
        /// @param [in] value Setting value to be applied.
        /// @return `true` if setting could be applied, `false` otherwise.
        static bool ApplyOverrideImportDirectInput8(StdString& value);

        /// Applies a setting that specifies a custom path for the import library for WinMM functions.
        /// @param [in] value Setting value to be applied.
        /// @return `true` if setting could be applied, `false` otherwise.
        static bool ApplyOverrideImportWinMM(StdString& value);
        
        /// Sets the supplied string reference to be equal to the library path for the DirectInput library that should be used for importing functions.
        /// @param [out] stringToFill String to be filled with the path.
        static void FillDirectInputLibraryPath(StdString& stringToFill);

        /// Sets the supplied string reference to be equal to the library path for the DirectInput8 library that should be used for importing functions.
        /// @param [out] stringToFill String to be filled with the path.
        static void FillDirectInput8LibraryPath(StdString& stringToFill);

        /// Sets the supplied string reference to be equal to the library path for the WinMM library that should be used for importing functions.
        /// @param [out] stringToFill String to be filled with the path.
        static void FillWinMMLibraryPath(StdString& stringToFill);
        
        /// Retrieves the handle of the instance that represents the current running form of Xidi, be it the library or the test application.
        /// @return Instance handle for Xidi.
        static HINSTANCE GetInstanceHandle(void);
        
        /// Sets the handle of the instance that represents the current running form of Xidi, be it the library or the test application.
        /// Intended to be called only once during initialization.
        /// @param [in] newInstanceHandle Instance handle to set.
        static void SetInstanceHandle(HINSTANCE newInstanceHandle);
        
        
    private:
        // -------- HELPERS ------------------------------------------------ //

        /// Sets the supplied string reference equal to the correct library path.
        /// If the override path is not empty, uses that, otherwise uses the system directory path plus the default library file name.
        /// @param [out] stringToFill String to be filled.
        /// @param [in] overridePath Non-default override path to be considered.
        /// @param [in] defaultLibraryFileName Default name to be used if the override path is not specified.
        static void FillLibraryPath(StdString& stringToFill, const StdString& overridePath, const StdString& defaultLibraryFileName);
        
        /// Sets the supplied string reference equal to the system directory path.
        /// @param [out] stringToFill String to be filled.
        static void FillSystemDirectoryPath(StdString& stringToFill);
    };
}
