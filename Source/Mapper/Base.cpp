/*****************************************************************************
 * XinputControllerDirectInput
 *      Hook and helper for older DirectInput games.
 *      Fixes issues associated with certain Xinput-based controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016
 *****************************************************************************
 * Base.cpp
 *      Abstract base class for supported control mapping schemes.
 *      Provides common implementations of most core functionality.
 *****************************************************************************/

#include "Mapper/Base.h"

using namespace XinputControllerDirectInput;
using namespace XinputControllerDirectInput::Mapper;


// -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //
// See "Mapper/Base.h" for documentation.

Base::Base() : instanceToOffset(), offsetToInstance(), mapsValid(FALSE), axisProperties(NULL) {}

// ---------

Base::~Base()
{
    instanceToOffset.clear();
    offsetToInstance.clear();

    if (NULL != axisProperties) delete[] axisProperties;
}


// -------- CLASS METHODS -------------------------------------------------- //
// See "Mapper/Base.h" for documentation.

DWORD Base::SizeofInstance(const EInstanceType type)
{
    DWORD szInstance = 0;
    
    switch (type)
    {
    case InstanceTypeAxis:
    case InstanceTypePov:
        szInstance = sizeof(LONG);
        break;

    case InstanceTypeButton:
        szInstance = sizeof(BYTE);
        break;
    }

    return szInstance;
}


// -------- HELPERS -------------------------------------------------------- //
// See "Mapper/Base.h" for documentation.

LPTSTR Base::AxisTypeToString(REFGUID axisTypeGUID)
{
    if (axisTypeGUID == GUID_XAxis)
        return _T("X Axis");
    if (axisTypeGUID == GUID_YAxis)
        return _T("Y Axis");
    if (axisTypeGUID == GUID_ZAxis)
        return _T("Z Axis");
    if (axisTypeGUID == GUID_RxAxis)
        return _T("X Rotation");
    if (axisTypeGUID == GUID_RyAxis)
        return _T("Y Rotation");
    if (axisTypeGUID == GUID_RzAxis)
        return _T("Z Rotation");

    return _T("Unknown Axis");
}

// ---------

BOOL Base::CheckAndSetOffsets(BOOL* base, const DWORD count)
{
    for (DWORD i = 0; i < count; ++i)
        if (base[i] != FALSE) return FALSE;

    for (DWORD i = 0; i < count; ++i)
        base[i] = TRUE;

    return TRUE;
}

// ---------

void Base::FillObjectInstanceInfo(LPDIDEVICEOBJECTINSTANCE instanceInfo, EInstanceType instanceType, TInstanceIdx instanceNumber)
{
    // Obtain the number of objects of each type.
    const TInstanceCount numAxes = NumInstancesOfType(EInstanceType::InstanceTypeAxis);
    const TInstanceCount numPov = NumInstancesOfType(EInstanceType::InstanceTypePov);
    const TInstanceCount numButtons = NumInstancesOfType(EInstanceType::InstanceTypeButton);

    // Initialize the structure and fill out common parts.
    ZeroMemory(instanceInfo, sizeof(*instanceInfo));
    instanceInfo->dwSize = sizeof(*instanceInfo);
    instanceInfo->dwType = DIDFT_MAKEINSTANCE(instanceNumber);
    instanceInfo->dwFlags = 0;
    
    // Fill in the rest of the structure based on the instance type.
    switch (instanceType)
    {
    case EInstanceType::InstanceTypeAxis:
        instanceInfo->dwOfs = (instanceNumber * SizeofInstance(instanceType));
        instanceInfo->guidType = AxisTypeFromInstanceNumber(instanceNumber);
        instanceInfo->dwType |= DIDFT_ABSAXIS;
        _tcscpy_s(instanceInfo->tszName, _countof(instanceInfo->tszName), AxisTypeToString(instanceInfo->guidType));
        break;
    
    case EInstanceType::InstanceTypePov:
        instanceInfo->dwOfs = (numAxes * SizeofInstance(EInstanceType::InstanceTypeAxis)) + (instanceNumber * SizeofInstance(instanceType));
        instanceInfo->guidType = GUID_POV;
        instanceInfo->dwType |= DIDFT_POV;
        _stprintf_s(instanceInfo->tszName, _countof(instanceInfo->tszName), _T("POV %u"), (unsigned)instanceNumber);
        break;
    
    case EInstanceType::InstanceTypeButton:
        instanceInfo->dwOfs = (numAxes * SizeofInstance(EInstanceType::InstanceTypeAxis)) + (numPov * SizeofInstance(EInstanceType::InstanceTypePov)) + (instanceNumber * SizeofInstance(instanceType));
        instanceInfo->guidType = GUID_Button;
        instanceInfo->dwType |= DIDFT_PSHBUTTON;
        _stprintf_s(instanceInfo->tszName, _countof(instanceInfo->tszName), _T("Button %u"), (unsigned)instanceNumber);
        break;
    }
}

// ---------

void Base::InitializeAxisProperties(void)
{
    if (NULL == axisProperties)
    {
        const TInstanceCount numAxes = NumInstancesOfType(EInstanceType::InstanceTypeAxis);

        axisProperties = new SAxisProperties[numAxes];

        for (TInstanceIdx i = 0; i < (TInstanceIdx)numAxes; ++i)
        {
            axisProperties[i].rangeMin = kDefaultAxisRangeMin;
            axisProperties[i].rangeMax = kDefaultAxisRangeMax;
            axisProperties[i].deadzone = kDefaultAxisDeadzone;
            axisProperties[i].saturation = kDefaultAxisSaturation;
        }
    }
}

// ---------

TInstance Base::InstanceIdentifierFromDirectInputIdentifier(DWORD diIdentifier)
{
    EInstanceType type = (EInstanceType)-1;
    TInstanceIdx idx = (TInstanceIdx)DIDFT_GETINSTANCE(diIdentifier);

    switch (DIDFT_GETTYPE(diIdentifier))
    {
    case DIDFT_ABSAXIS:
        type = EInstanceType::InstanceTypeAxis;
        break;

    case DIDFT_PSHBUTTON:
        type = EInstanceType::InstanceTypeButton;
        break;

    case DIDFT_POV:
        type = EInstanceType::InstanceTypePov;
        break;
    }

    if (type < 0 || idx >= NumInstancesOfType(type))
        return (TInstance)-1;

    return MakeInstanceIdentifier(type, idx);
}

// ---------

TInstance Base::InstanceIdentifierFromDirectInputSpec(DWORD dwObj, DWORD dwHow)
{
    TInstance instance = (TInstance)-1;

    // Select an instance based on the specifics provided by the application.
    // The methods called to get this instance also check for validity of the input and the current object state.
    // Only these methods for "dwHow" are supported, others return an invalid result.
    switch (dwHow)
    {
    case DIPH_BYOFFSET:
        instance = InstanceForOffset(dwObj);
        break;

    case DIPH_BYID:
        instance = InstanceIdentifierFromDirectInputIdentifier(dwObj);
        break;
    }

    return instance;
}

// ---------

TInstance Base::SelectInstance(const EInstanceType instanceType, BOOL* instanceUsed, const TInstanceCount instanceCount, const TInstanceIdx instanceToSelect)
{
    TInstance selectedInstance = (TInstance)-1;

    if ((instanceToSelect < instanceCount) && (FALSE == instanceUsed[instanceToSelect]))
    {
        instanceUsed[instanceToSelect] = TRUE;
        selectedInstance = Base::MakeInstanceIdentifier(instanceType, instanceToSelect);
    }
    
    return selectedInstance;
}


// -------- INSTANCE METHODS ----------------------------------------------- //
// See "Mapper/Base.h" for documentation.

HRESULT Base::EnumerateMappedObjects(LPDIENUMDEVICEOBJECTSCALLBACK appCallback, LPVOID appCbParam, DWORD enumerationFlags)
{
    // Obtain the number of objects of each type.
    const TInstanceCount numAxes = NumInstancesOfType(EInstanceType::InstanceTypeAxis);
    const TInstanceCount numPov = NumInstancesOfType(EInstanceType::InstanceTypePov);
    const TInstanceCount numButtons = NumInstancesOfType(EInstanceType::InstanceTypeButton);
    
    // If requested, enumerate axes.
    if (DIDFT_ALL == enumerationFlags || enumerationFlags & DIDFT_AXIS)
    {
        for (TInstanceCount i = 0; i < numAxes; ++i)
        {
            // Allocate and fill a structure to submit to the application, using the heap for security purposes.
            DIDEVICEOBJECTINSTANCE* objectDescriptor = new DIDEVICEOBJECTINSTANCE();
            FillObjectInstanceInfo(objectDescriptor, EInstanceType::InstanceTypeAxis, (TInstanceIdx)i);
            
            // Submit the button to the application.
            BOOL appResponse = appCallback(objectDescriptor, appCbParam);
            
            // Free previously-allocated memory.
            delete objectDescriptor;
            
            // See if the application requested that the enumeration stop and, if so, honor that request
            switch (appResponse)
            {
            case DIENUM_CONTINUE:
                break;
            case DIENUM_STOP:
                return DI_OK;
            default:
                return DIERR_INVALIDPARAM;
            }
        }
    }

    // If requested, enumerate POVs.
    if (DIDFT_ALL == enumerationFlags || enumerationFlags & DIDFT_POV)
    {
        for (TInstanceCount i = 0; i < numPov; ++i)
        {
            // Allocate and fill a structure to submit to the application, using the heap for security purposes.
            DIDEVICEOBJECTINSTANCE* objectDescriptor = new DIDEVICEOBJECTINSTANCE();
            FillObjectInstanceInfo(objectDescriptor, EInstanceType::InstanceTypePov, (TInstanceIdx)i);
            
            // Submit the button to the application.
            BOOL appResponse = appCallback(objectDescriptor, appCbParam);
            
            // Free previously-allocated memory.
            delete objectDescriptor;
            
            // See if the application requested that the enumeration stop and, if so, honor that request
            switch (appResponse)
            {
            case DIENUM_CONTINUE:
                break;
            case DIENUM_STOP:
                return DI_OK;
            default:
                return DIERR_INVALIDPARAM;
            }
        }
    }

    // If requested, enumerate buttons.
    if (DIDFT_ALL == enumerationFlags || enumerationFlags & DIDFT_BUTTON)
    {
        for (TInstanceCount i = 0; i < numButtons; ++i)
        {
            // Allocate and fill a structure to submit to the application, using the heap for security purposes.
            DIDEVICEOBJECTINSTANCE* objectDescriptor = new DIDEVICEOBJECTINSTANCE();
            FillObjectInstanceInfo(objectDescriptor, EInstanceType::InstanceTypeButton, (TInstanceIdx)i);
            
            // Submit the button to the application.
            BOOL appResponse = appCallback(objectDescriptor, appCbParam);
            
            // Free previously-allocated memory.
            delete objectDescriptor;
            
            // See if the application requested that the enumeration stop and, if so, honor that request
            switch (appResponse)
            {
            case DIENUM_CONTINUE:
                break;
            case DIENUM_STOP:
                return DI_OK;
            default:
                return DIERR_INVALIDPARAM;
            }
        }
    }
    
    return DI_OK;
}

// ---------

void Base::FillDeviceCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
    lpDIDevCaps->dwAxes = (DWORD)NumInstancesOfType(EInstanceType::InstanceTypeAxis);
    lpDIDevCaps->dwButtons = (DWORD)NumInstancesOfType(EInstanceType::InstanceTypeButton);
    lpDIDevCaps->dwPOVs = (DWORD)NumInstancesOfType(EInstanceType::InstanceTypePov);
}

HRESULT Base::GetMappedObjectInfo(LPDIDEVICEOBJECTINSTANCE pdidoi, DWORD dwObj, DWORD dwHow)
{
    TInstance instance = InstanceIdentifierFromDirectInputSpec(dwObj, dwHow);
    
    // Verify that the structure size is corect, as required by the DirectInput API.
    if (pdidoi->dwSize != sizeof(*pdidoi)) return DIERR_INVALIDPARAM;
    
    // Check if an instance was identifiable above, if not then the object could not be located
    if (instance < 0)
        return DIERR_OBJECTNOTFOUND;
    
    // Fill the specified structure with information about the specified object.
    FillObjectInstanceInfo(pdidoi, ExtractIdentifierInstanceType(instance), ExtractIdentifierInstanceIndex(instance));
    
    return DI_OK;
}

// ---------

HRESULT Base::GetMappedProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
    // Lazily initialize the axis properties (this is idempotent).
    InitializeAxisProperties();
    
    // First verify that this property is handled by this mapper.
    if (!IsPropertyHandledByMapper(rguidProp))
        return DIERR_UNSUPPORTED;

    // Verify the correct header size.
    if (pdiph->dwHeaderSize != sizeof(DIPROPHEADER))
        return DIERR_INVALIDPARAM;
    
    // Branch based on the property requested.
    if (&DIPROP_AXISMODE == &rguidProp)
    {
        // Axis mode is easy: there is only one mode supported by the mapper.
        
        // Verify correct size. This one needs to be DIPROPDWORD.
        if (pdiph->dwSize != sizeof(DIPROPDWORD))
            return DIERR_INVALIDPARAM;
        
        // Provide output that the axis mode is absolute.
        ((LPDIPROPDWORD)pdiph)->dwData = DIPROPAXISMODE_ABS;
    }
    else if (&DIPROP_DEADZONE == &rguidProp || &DIPROP_SATURATION == &rguidProp || &DIPROP_RANGE == &rguidProp)
    {
        // Either deadzone, saturation, or range, and the logic is substantially similar for all of them.

        // Expected size depends on the actual property, so get that here.
        DWORD expectedSize = 0;

        if (&DIPROP_DEADZONE == &rguidProp || &DIPROP_SATURATION == &rguidProp)
            expectedSize = sizeof(DIPROPDWORD);
        else
            expectedSize = sizeof(DIPROPRANGE);

        // Verify correct size.
        if (pdiph->dwSize != expectedSize)
            return DIERR_INVALIDPARAM;

        // Verify that the target is specific, not the whole device.
        if (DIPH_DEVICE == pdiph->dwHow)
            return DIERR_UNSUPPORTED;

        // Attempt to locate the instance.
        TInstance instance = InstanceIdentifierFromDirectInputSpec(pdiph->dwObj, pdiph->dwHow);
        if (instance < 0)
            return DIERR_OBJECTNOTFOUND;

        // Verify that the instance target is an axis.
        if (EInstanceType::InstanceTypeAxis != ExtractIdentifierInstanceType(instance))
            return DIERR_UNSUPPORTED;
        
        // Provide the requested data, branching by specific property.
        if (&DIPROP_DEADZONE == &rguidProp)
            ((LPDIPROPDWORD)pdiph)->dwData = axisProperties[ExtractIdentifierInstanceIndex(instance)].deadzone;
        else if (&DIPROP_SATURATION == &rguidProp)
            ((LPDIPROPDWORD)pdiph)->dwData = axisProperties[ExtractIdentifierInstanceIndex(instance)].saturation;
        else
        {
            ((LPDIPROPRANGE)pdiph)->lMin = axisProperties[ExtractIdentifierInstanceIndex(instance)].rangeMin;
            ((LPDIPROPRANGE)pdiph)->lMax = axisProperties[ExtractIdentifierInstanceIndex(instance)].rangeMax;
        }
    }
    else
    {
        // Should never get here.
        return DIERR_UNSUPPORTED;
    }

    return DI_OK;
}

// ---------

TInstance Base::InstanceForOffset(DWORD offset)
{
    TInstance result = (TInstance)-1;
    
    if (IsApplicationDataFormatSet())
    {
        auto it = offsetToInstance.find(offset);

        if (offsetToInstance.end() != it)
            result = it->second;
    }

    return result;
}

// ---------

BOOL Base::IsApplicationDataFormatSet(void)
{
    return mapsValid;
}

// ---------

BOOL Base::IsPropertyHandledByMapper(REFGUID guidProperty)
{
    BOOL propertyHandled = FALSE;

    if (&guidProperty == &DIPROP_AXISMODE || &guidProperty == &DIPROP_DEADZONE || &guidProperty == &DIPROP_RANGE || &guidProperty == &DIPROP_SATURATION)
        propertyHandled = TRUE;
    
    return propertyHandled;
}

// ---------

DWORD Base::OffsetForInstance(TInstance instance)
{
    DWORD result = (DWORD)~0;

    if (IsApplicationDataFormatSet())
    {
        auto it = instanceToOffset.find(instance);

        if (instanceToOffset.end() != it)
            result = it->second;
    }

    return result;
}

// ---------

HRESULT Base::SetApplicationDataFormat(LPCDIDATAFORMAT lpdf)
{
    // Obtain the number of instances of each type in the mapping by asking the subclass.
    const TInstanceCount numButtons = NumInstancesOfType(EInstanceType::InstanceTypeButton);
    const TInstanceCount numAxes = NumInstancesOfType(EInstanceType::InstanceTypeAxis);
    const TInstanceCount numPov = NumInstancesOfType(EInstanceType::InstanceTypePov);
    
    // Track the next unused instance of each, essentially allowing a "dequeue" operation when the application does not specify a specific instance.
    TInstanceIdx nextUnusedButton = 0;
    TInstanceIdx nextUnusedAxis = 0;
    TInstanceIdx nextUnusedPov = 0;
    
    // Keep track of which instances were added to the mapping of each type as well as each offset.
    // It is an error to specify an instance multiple times, specify a non-existant instance, or specify multiple pieces of information at the same offset.
    BOOL* buttonUsed = new BOOL[numButtons];
    BOOL* axisUsed = new BOOL[numAxes];
    BOOL* povUsed = new BOOL[numPov];
    BOOL* offsetUsed = new BOOL[lpdf->dwDataSize];
    for (TInstanceCount i = 0; i < numButtons; ++i) buttonUsed[i] = FALSE;
    for (TInstanceCount i = 0; i < numAxes; ++i) axisUsed[i] = FALSE;
    for (TInstanceCount i = 0; i < numPov; ++i) povUsed[i] = FALSE;
    for (DWORD i = 0; i < lpdf->dwDataSize; ++i) offsetUsed[i] = FALSE;

    // Initialize the maps by clearing them and marking them invalid
    instanceToOffset.clear();
    offsetToInstance.clear();
    mapsValid = FALSE;
    
    // Iterate over each of the object specifications provided by the application.
    for (DWORD i = 0; i < lpdf->dwNumObjs; ++i)
    {
        LPDIOBJECTDATAFORMAT dataFormat = &lpdf->rgodf[i];
        BOOL invalidParamsDetected = FALSE;

        // Extract information about the instance specified by the application.
        // If any instance is allowed, the specific instance is irrelevant.
        const BOOL allowAnyInstance = ((dataFormat->dwType & DIDFT_INSTANCEMASK) == DIDFT_ANYINSTANCE);
        const TInstanceIdx specificInstance = (TInstanceIdx)DIDFT_GETINSTANCE(dataFormat->dwType);

        if ((dataFormat->dwType & DIDFT_ABSAXIS) && (nextUnusedAxis < numAxes))
        {
            // Pick an axis

            // First check the offsets for overlap with something previously selected
            if (FALSE == CheckAndSetOffsets(&offsetUsed[dataFormat->dwOfs], SizeofInstance(EInstanceType::InstanceTypeAxis)))
                invalidParamsDetected = TRUE;
            else
            {
                if (NULL == dataFormat->pguid)
                {
                    // Any axis type allowed

                    const TInstanceIdx instanceToSelect = allowAnyInstance ? nextUnusedAxis : specificInstance;
                    const TInstance selectedInstance = SelectInstance(EInstanceType::InstanceTypeAxis, axisUsed, numAxes, instanceToSelect);

                    if (selectedInstance > 0)
                    {
                        // Instance was selected successfully, add a mapping
                        instanceToOffset.insert({ selectedInstance, dataFormat->dwOfs });
                        offsetToInstance.insert({ dataFormat->dwOfs, selectedInstance });
                    }
                    else if (!allowAnyInstance)
                    {
                        // Instance was unable to be selected, and a specific instance was specified, so this is an error
                        invalidParamsDetected = TRUE;
                    }
                }
                else
                {
                    // Specific axis type required

                    if (0 != AxisTypeCount(*dataFormat->pguid))
                    {
                        // Axis type exists in the mapping

                        if (allowAnyInstance)
                        {
                            // Any instance allowed, so find the first of this type that is unused, if any
                            TInstanceIdx instanceIndex = 0;
                            TInstanceIdx axisIndex = AxisInstanceIndex(*dataFormat->pguid, instanceIndex++);
                            TInstance selectedInstance = SelectInstance(EInstanceType::InstanceTypeAxis, axisUsed, numAxes, axisIndex);

                            while (selectedInstance < 0 && axisIndex >= 0)
                            {
                                axisIndex = AxisInstanceIndex(*dataFormat->pguid, instanceIndex++);
                                selectedInstance = SelectInstance(EInstanceType::InstanceTypeAxis, axisUsed, numAxes, axisIndex);
                            }

                            if (selectedInstance >= 0)
                            {
                                // Unused instance found, create a mapping
                                instanceToOffset.insert({ MakeInstanceIdentifier(EInstanceType::InstanceTypeAxis, axisIndex), dataFormat->dwOfs });
                                offsetToInstance.insert({ dataFormat->dwOfs, MakeInstanceIdentifier(EInstanceType::InstanceTypeAxis, axisIndex) });
                            }
                        }
                        else
                        {
                            // Specific instance required, so check if it is available
                            TInstanceIdx axisIndex = AxisInstanceIndex(*dataFormat->pguid, specificInstance);

                            if (axisIndex >= 0 && FALSE == axisUsed[axisIndex])
                            {
                                // Axis available, use it
                                axisUsed[axisIndex] = TRUE;
                                instanceToOffset.insert({ MakeInstanceIdentifier(EInstanceType::InstanceTypeAxis, axisIndex), dataFormat->dwOfs });
                                offsetToInstance.insert({ dataFormat->dwOfs, MakeInstanceIdentifier(EInstanceType::InstanceTypeAxis, axisIndex) });
                            }
                            else
                            {
                                // Axis unavailable, this is an error
                                invalidParamsDetected = TRUE;
                            }
                        }
                    }
                    else
                    {
                        // Axis type does not exist in the mapping

                        if (!allowAnyInstance)
                        {
                            // Specified an instance of a non-existant axis, this is an error
                            invalidParamsDetected = TRUE;
                        }
                    }
                }
            }
        }
        else if ((dataFormat->dwType & DIDFT_PSHBUTTON) && (nextUnusedButton < numButtons))
        {
            // Pick a button

            if (NULL == dataFormat->pguid || GUID_Button == *dataFormat->pguid)
            {
                // Type unspecified or specified as a button

                const TInstanceIdx instanceToSelect = allowAnyInstance ? nextUnusedButton : specificInstance;
                const TInstance selectedInstance = SelectInstance(EInstanceType::InstanceTypeButton, buttonUsed, numButtons, instanceToSelect);

                if (selectedInstance > 0)
                {
                    // Instance was selected successfully, add a mapping
                    instanceToOffset.insert({ selectedInstance, dataFormat->dwOfs });
                    offsetToInstance.insert({ dataFormat->dwOfs, selectedInstance });
                }
                else if (!allowAnyInstance)
                {
                    // Instance was unable to be selected, and a specific instance was specified, so this is an error
                    invalidParamsDetected = TRUE;
                }
            }
            else
            {
                // Type specified as a non-button, this is an error
                invalidParamsDetected = TRUE;
            }

        }
        else if ((dataFormat->dwType & DIDFT_POV) && (nextUnusedPov < numPov))
        {
            // Pick a POV

            if (NULL == dataFormat->pguid || GUID_POV == *dataFormat->pguid)
            {
                // Type unspecified or specified as a POV

                const TInstanceIdx instanceToSelect = allowAnyInstance ? nextUnusedPov : specificInstance;
                const TInstance selectedInstance = SelectInstance(EInstanceType::InstanceTypePov, povUsed, numPov, instanceToSelect);

                if (selectedInstance > 0)
                {
                    // Instance was selected successfully, add a mapping
                    instanceToOffset.insert({ selectedInstance, dataFormat->dwOfs });
                    offsetToInstance.insert({ dataFormat->dwOfs, selectedInstance });
                }
                else if (!allowAnyInstance)
                {
                    // Instance was unable to be selected, and a specific instance was specified, so this is an error
                    invalidParamsDetected = TRUE;
                }
            }
            else
            {
                // Type specified as a non-POV, this is an error
                invalidParamsDetected = TRUE;
            }

        }
        else if (allowAnyInstance)
        {
            // No objects available, but no instance specified, so do not do anything this iteration
        }
        else
        {
            // An instance was specified of an object that is not available, this is an error
            invalidParamsDetected = TRUE;
        }

        // Bail in the event of an error
        if (invalidParamsDetected)
        {
            delete[] buttonUsed;
            delete[] axisUsed;
            delete[] povUsed;
            delete[] offsetUsed;

            return DIERR_INVALIDPARAM;
        }

        // Increment all next-unused indices
        while (TRUE == axisUsed[nextUnusedAxis] && nextUnusedAxis < numAxes) nextUnusedAxis += 1;
        while (TRUE == buttonUsed[nextUnusedButton] && nextUnusedButton < numButtons) nextUnusedButton += 1;
        while (TRUE == povUsed[nextUnusedPov] && nextUnusedPov < numPov) nextUnusedPov += 1;
    }
    
    delete[] buttonUsed;
    delete[] axisUsed;
    delete[] povUsed;
    delete[] offsetUsed;

    mapsValid = TRUE;
    return S_OK;
}

// ---------

HRESULT Base::SetMappedProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
    // Lazily initialize the axis properties (this is idempotent).
    InitializeAxisProperties();

    // First verify that this property is handled by this mapper.
    if (!IsPropertyHandledByMapper(rguidProp))
        return DIERR_UNSUPPORTED;

    // Verify the correct header size.
    if (pdiph->dwHeaderSize != sizeof(DIPROPHEADER))
        return DIERR_INVALIDPARAM;

    // Branch based on the property requested.
    if (&DIPROP_AXISMODE == &rguidProp)
    {
        // Axis mode is easy: it is read-only, so just reject it.
        return DIERR_UNSUPPORTED;
    }
    else if (&DIPROP_DEADZONE == &rguidProp || &DIPROP_SATURATION == &rguidProp || &DIPROP_RANGE == &rguidProp)
    {
        // Either deadzone, saturation, or range, and the logic is substantially similar for all of them.

        // Expected size depends on the actual property, so get that here.
        DWORD expectedSize = 0;

        if (&DIPROP_DEADZONE == &rguidProp || &DIPROP_SATURATION == &rguidProp)
            expectedSize = sizeof(DIPROPDWORD);
        else
            expectedSize = sizeof(DIPROPRANGE);

        // Verify correct size.
        if (pdiph->dwSize != expectedSize)
            return DIERR_INVALIDPARAM;

        // Verify that the target is specific, not the whole device.
        if (DIPH_DEVICE == pdiph->dwHow)
            return DIERR_UNSUPPORTED;

        // Attempt to locate the instance.
        TInstance instance = InstanceIdentifierFromDirectInputSpec(pdiph->dwObj, pdiph->dwHow);
        if (instance < 0)
            return DIERR_OBJECTNOTFOUND;

        // Verify that the instance target is an axis.
        if (EInstanceType::InstanceTypeAxis != ExtractIdentifierInstanceType(instance))
            return DIERR_UNSUPPORTED;

        // Verify the provided data and, if valid, write it.
        if (&DIPROP_DEADZONE == &rguidProp)
        {
            DWORD newDeadzone = ((LPDIPROPDWORD)pdiph)->dwData;

            if (newDeadzone < kMinAxisDeadzoneSaturation || newDeadzone > kMaxAxisDeadzoneSaturation)
                return DIERR_INVALIDPARAM;
            
            axisProperties[ExtractIdentifierInstanceIndex(instance)].deadzone = newDeadzone;
        }
        else if (&DIPROP_SATURATION == &rguidProp)
        {
            DWORD newSaturation = ((LPDIPROPDWORD)pdiph)->dwData;

            if (newSaturation < kMinAxisDeadzoneSaturation || newSaturation > kMaxAxisDeadzoneSaturation)
                return DIERR_INVALIDPARAM;

            axisProperties[ExtractIdentifierInstanceIndex(instance)].saturation = newSaturation;
        }
        else
        {
            LONG newRangeMin = ((LPDIPROPRANGE)pdiph)->lMin;
            LONG newRangeMax = ((LPDIPROPRANGE)pdiph)->lMax;
            
            if (!(newRangeMin < newRangeMax))
                return DIERR_INVALIDPARAM;

            axisProperties[ExtractIdentifierInstanceIndex(instance)].rangeMin = newRangeMin;
            axisProperties[ExtractIdentifierInstanceIndex(instance)].rangeMax = newRangeMax;
        }
    }
    else
    {
        // Should never get here.
        return DIERR_UNSUPPORTED;
    }

    return DI_OK;
}

// ---------

void Base::ResetApplicationDataFormat(void)
{
    instanceToOffset.clear();
    offsetToInstance.clear();

    mapsValid = FALSE;
}
