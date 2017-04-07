/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file MapperFactory.h
 *   Declaration of a class that creates mappers of different types.
 *****************************************************************************/

#pragma once

#include "ApiStdString.h"
#include "Mapper/Base.h"

#include <unordered_map>


namespace Xidi
{
    // Enumerates the known types of mappers that can be created.
    enum EMapper
    {
        DefaultMapper,
        XInputNativeMapper,
        XInputSharedTriggersMapper,
        StandardGamepadMapper,
        ExtendedGamepadMapper
    };
    
    // Creates and returns pointers to new mapper objects on request.
    // Intended to be used along with other logic that configures the types of mappers that should be created.
    // Specifies a default mapper type but allows a configuration change that would alter the type of mappers returned.
    // All methods are class methods.
    class MapperFactory
    {
    public:
        // -------- CONSTANTS ------------------------------------------------------ //
        
        // Specifies the default mapper type that, absent any other changes, will be created upon request.
        static const EMapper kDefaultMapperType = EMapper::ExtendedGamepadMapper;
        
        
    private:
        // -------- CLASS VARIABLES ------------------------------------------------ //
        
        // Specifies the currently-configured mapper type.
        static EMapper configuredMapperType;

        // Maps strings to mapper types.
        // Used for accepting configuration setting values.
        static std::unordered_map<StdString, EMapper> mapperTypeStrings;
        
        
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
        
        // Default constructor. Should never be invoked.
        MapperFactory();
        
        
    public:
        // -------- CLASS METHODS -------------------------------------------------- //
        
        // Applies a configuration setting that configures the type of mapper to create.
        static bool ApplyConfigurationMapperType(const StdString& value);
        
        // Creates a new mapper of the configured type, using the "new" operator.
        // Returns NULL in the event of an error (i.e. invalid or unrecognized configured type).
        static Mapper::Base* CreateMapper(void);

        // Creates a new mapper of the specified type, using the "new" operator.
        // Returns NULL in the event of an error (i.e. invalid or unrecognized type).
        static Mapper::Base* CreateMapperOfType(EMapper type);
        
        // Resets the mapper configuration to default.
        static void ResetMapperType(void);
        
        // Configures a new type of mapper to create.
        static void SetMapperType(EMapper type);
    };
}
