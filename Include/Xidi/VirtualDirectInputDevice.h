/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2021
 *************************************************************************//**
 * @file VirtualDirectInputDevice.h
 *   Declaration of an IDirectInputDevice interface wrapper around virtual
 *   controllers.
 *****************************************************************************/

#pragma once

#include "ApiDirectInput.h"
#include "DataFormat.h"
#include "VirtualController.h"

#include <atomic>
#include <memory>
#include <optional>


namespace Xidi
{
    /// Helper types for differentiating between Unicode and ASCII interface versions.
    template <ECharMode charMode> struct DirectInputDeviceType
    {
        typedef LPCTSTR ConstStringType;
        typedef DIDEVICEINSTANCE DeviceInstanceType;
        typedef DIDEVICEINSTANCE_DX3 DeviceInstanceCompatType;
        typedef DIDEVICEOBJECTINSTANCE DeviceObjectInstanceType;
        typedef DIDEVICEOBJECTINSTANCE_DX3 DeviceObjectInstanceCompatType;
        typedef DIEFFECTINFO EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACK EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACK EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMAT ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADER DeviceImageInfoHeaderType;
#endif
    };

    template <> struct DirectInputDeviceType<ECharMode::A> : public LatestIDirectInputDeviceA
    {
        typedef LPCSTR ConstStringType;
        typedef DIDEVICEINSTANCEA DeviceInstanceType;
        typedef DIDEVICEINSTANCE_DX3A DeviceInstanceCompatType;
        typedef DIDEVICEOBJECTINSTANCEA DeviceObjectInstanceType;
        typedef DIDEVICEOBJECTINSTANCE_DX3A DeviceObjectInstanceCompatType;
        typedef DIEFFECTINFOA EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACKA EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACKA EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMATA ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADERA DeviceImageInfoHeaderType;
#endif
    };

    template <> struct DirectInputDeviceType<ECharMode::W> : public LatestIDirectInputDeviceW
    {
        typedef LPCWSTR ConstStringType;
        typedef DIDEVICEINSTANCEW DeviceInstanceType;
        typedef DIDEVICEINSTANCE_DX3W DeviceInstanceCompatType;
        typedef DIDEVICEOBJECTINSTANCEW DeviceObjectInstanceType;
        typedef DIDEVICEOBJECTINSTANCE_DX3W DeviceObjectInstanceCompatType;
        typedef DIEFFECTINFOW EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACKW EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACKW EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMATW ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADERW DeviceImageInfoHeaderType;
#endif
    };

    /// Enumerates possible access modes for DirectInput devices.
    enum class ECooperativeLevel
    {
        Shared,                                                             ///< Shared mode, also known as non-exclusive mode. Any number of shared mode acquisitions are allowed to the same physical device, even if another acquisition already exists in exclusive mode.
        Exclusive                                                           ///< Exclusive mode. Only a single acquisition in exclusive mode is permitted per physical device.
    };

    /// Inherits from whatever IDirectInputDevice version is appropriate.
    /// @tparam charMode Selects between ASCII ("A" suffix) and Unicode ("W") suffix versions of types and interfaces.
    template <ECharMode charMode> class VirtualDirectInputDevice : public DirectInputDeviceType<charMode>
    {
    private:
        // -------- INSTANCE VARIABLES --------------------------------------------- //

        /// Virtual controller with which to interface.
        std::unique_ptr<Controller::VirtualController> controller;

        /// Cooperative level that defines the desired level of access to the underlying physical device.
        /// Shared by default, but applications can request exclusive mode.
        /// Force feedback requires that an applicaton acquire the device in exclusive mode.
        ECooperativeLevel cooperativeLevel;

        /// Data format specification for communicating with the DirectInput application.
        std::unique_ptr<DataFormat> dataFormat;

        /// Reference count.
        std::atomic<unsigned long> refCount;

    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Initialization constructor.
        /// @param [in] controller Virtual controller object to associate with this object.
        VirtualDirectInputDevice(std::unique_ptr<Controller::VirtualController>&& controller);

        /// Default destructor.
        ~VirtualDirectInputDevice(void);


        // -------- INSTANCE METHODS ----------------------------------------------- //

        /// Retrieves a reference to the underlying virtual controller object.
        /// Returned reference remains valid only as long as this object exists.
        /// Primarily intended for testing.
        /// @return Reference to the underlying virtual controller object.
        inline Controller::VirtualController& GetVirtualController(void)
        {
            return *controller;
        }

        /// Identifies a controller element, given a DirectInput-style element identifier.
        /// Parameters are named after common DirectInput field and method parameters that are used for this purpose.
        /// @param [in] dwObj Object identifier, whose semantics depends on identification method. See DirectInput documentation for more information.
        /// @param [in] dwHow Identification method. See DirectInput documentation for more information.
        /// @return Virtual controller element identifier that matches the DirectInput-style element identifier, if such a match exists.
        std::optional<Controller::SElementIdentifier> IdentifyElement(DWORD dwObj, DWORD dwHow) const;

        /// Identifies a controller element using a DirectInput-style object ID.
        /// @param [in] element Controller element to be identified.
        /// @return Resulting DirectInput-style identifier, if it exists.
        std::optional<DWORD> IdentifyObjectById(Controller::SElementIdentifier element) const;

        /// Identifies a controller element using a DirectInput-style offset into the application's data format.
        /// @param [in] element Controller element to be identified.
        /// @return Resulting DirectInput-style identifier, if it exists.
        std::optional<TOffset> IdentifyObjectByOffset(Controller::SElementIdentifier element) const;

        /// Specifies if the application's data format is set.
        /// @return `true` if the application's data format has been successfully set, `false` otherwise.
        inline bool IsApplicationDataFormatSet(void) const
        {
            return (nullptr != dataFormat);
        }


        // -------- METHODS: IUnknown ---------------------------------------------- //
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppvObj) override;
        ULONG STDMETHODCALLTYPE AddRef(void) override;
        ULONG STDMETHODCALLTYPE Release(void) override;


        // -------- METHODS: IDirectInputDevice COMMON ----------------------------- //
        HRESULT STDMETHODCALLTYPE Acquire(void) override;
        HRESULT STDMETHODCALLTYPE CreateEffect(REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter) override;
        HRESULT STDMETHODCALLTYPE EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl) override;
        HRESULT STDMETHODCALLTYPE EnumEffects(DirectInputDeviceType<charMode>::EnumEffectsCallbackType lpCallback, LPVOID pvRef, DWORD dwEffType) override;
        HRESULT STDMETHODCALLTYPE EnumEffectsInFile(DirectInputDeviceType<charMode>::ConstStringType lptszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE EnumObjects(DirectInputDeviceType<charMode>::EnumObjectsCallbackType lpCallback, LPVOID pvRef, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE Escape(LPDIEFFESCAPE pesc) override;
        HRESULT STDMETHODCALLTYPE GetCapabilities(LPDIDEVCAPS lpDIDevCaps) override;
        HRESULT STDMETHODCALLTYPE GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE GetDeviceInfo(DirectInputDeviceType<charMode>::DeviceInstanceType* pdidi) override;
        HRESULT STDMETHODCALLTYPE GetDeviceState(DWORD cbData, LPVOID lpvData) override;
        HRESULT STDMETHODCALLTYPE GetEffectInfo(DirectInputDeviceType<charMode>::EffectInfoType* pdei, REFGUID rguid) override;
        HRESULT STDMETHODCALLTYPE GetForceFeedbackState(LPDWORD pdwOut) override;
        HRESULT STDMETHODCALLTYPE GetObjectInfo(DirectInputDeviceType<charMode>::DeviceObjectInstanceType* pdidoi, DWORD dwObj, DWORD dwHow) override;
        HRESULT STDMETHODCALLTYPE GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph) override;
        HRESULT STDMETHODCALLTYPE Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) override;
        HRESULT STDMETHODCALLTYPE Poll(void) override;
        HRESULT STDMETHODCALLTYPE RunControlPanel(HWND hwndOwner, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE SendDeviceData(DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl) override;
        HRESULT STDMETHODCALLTYPE SendForceFeedbackCommand(DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND hwnd, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE SetDataFormat(LPCDIDATAFORMAT lpdf) override;
        HRESULT STDMETHODCALLTYPE SetEventNotification(HANDLE hEvent) override;
        HRESULT STDMETHODCALLTYPE SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph) override;
        HRESULT STDMETHODCALLTYPE Unacquire(void) override;
        HRESULT STDMETHODCALLTYPE WriteEffectToFile(DirectInputDeviceType<charMode>::ConstStringType lptszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) override;

#if DIRECTINPUT_VERSION >= 0x0800
        // -------- METHODS: IDirectInputDevice8 ONLY ------------------------------ //
        HRESULT STDMETHODCALLTYPE BuildActionMap(DirectInputDeviceType<charMode>::ActionFormatType* lpdiaf, DirectInputDeviceType<charMode>::ConstStringType lpszUserName, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE GetImageInfo(DirectInputDeviceType<charMode>::DeviceImageInfoHeaderType* lpdiDevImageInfoHeader) override;
        HRESULT STDMETHODCALLTYPE SetActionMap(DirectInputDeviceType<charMode>::ActionFormatType* lpdiActionFormat, DirectInputDeviceType<charMode>::ConstStringType lptszUserName, DWORD dwFlags) override;
#endif
    };
}
