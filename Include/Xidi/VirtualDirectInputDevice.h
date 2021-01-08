/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2020
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
    template <bool useUnicode> struct DirectInputDeviceType
    {
        typedef LPCTSTR ConstStringType;
        typedef DIDEVICEINSTANCE DeviceInstanceType;
        typedef DIDEVICEOBJECTINSTANCE DeviceObjectInstanceType;
        typedef DIEFFECTINFO EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACK EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACK EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMAT ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADER DeviceImageInfoHeaderType;
#endif
    };

    template <> struct DirectInputDeviceType<false> : public LatestIDirectInputDeviceA
    {
        typedef LPCSTR ConstStringType;
        typedef DIDEVICEINSTANCEA DeviceInstanceType;
        typedef DIDEVICEOBJECTINSTANCEA DeviceObjectInstanceType;
        typedef DIEFFECTINFOA EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACKA EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACKA EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMATA ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADERA DeviceImageInfoHeaderType;
#endif
    };

    template <> struct DirectInputDeviceType<true> : public LatestIDirectInputDeviceW
    {
        typedef LPCWSTR ConstStringType;
        typedef DIDEVICEINSTANCEW DeviceInstanceType;
        typedef DIDEVICEOBJECTINSTANCEW DeviceObjectInstanceType;
        typedef DIEFFECTINFOW EffectInfoType;
        typedef LPDIENUMEFFECTSCALLBACKW EnumEffectsCallbackType;
        typedef LPDIENUMDEVICEOBJECTSCALLBACKW EnumObjectsCallbackType;
#if DIRECTINPUT_VERSION >= 0x0800
        typedef DIACTIONFORMATW ActionFormatType;
        typedef DIDEVICEIMAGEINFOHEADERW DeviceImageInfoHeaderType;
#endif
    };

    /// Inherits from whatever IDirectInputDevice version is appropriate.
    /// @tparam useUnicode Specifies whether to use underlying Unicode interfaces (i.e. the "A" versions of interfaces and types).
    template <bool useUnicode> class VirtualDirectInputDevice : public DirectInputDeviceType<useUnicode>
    {
    private:
        // -------- INSTANCE VARIABLES --------------------------------------------- //

        /// Virtual controller with which to interface.
        std::unique_ptr<Controller::VirtualController> controller;

        /// Data format specification for communicating with the DirectInput application.
        std::unique_ptr<DataFormat> dataFormat;

        /// Reference count.
        std::atomic<unsigned long> refCount;

        /// State change event notification handle, optionally provided by applications.
        /// The underlying event object is owned by the application, not by this object.
        HANDLE stateChangeEventHandle;

    public:
        // -------- CONSTRUCTION AND DESTRUCTION ----------------------------------- //

        /// Initialization constructor.
        /// @param [in] controller Virtual controller object to associate with this object.
        VirtualDirectInputDevice(std::unique_ptr<Controller::VirtualController>&& controller);


    public:
        // -------- INSTANCE METHODS ----------------------------------------------- //

        /// Identifies a controller element, given a DirectInput-style element identifier.
        /// Parameters are named after common DirectInput field and method parameters that are used for this purpose.
        /// @param [in] dwObj Object identifier, whose semantics depends on identification method. See DirectInput documentation for more information.
        /// @param [in] dwHow Identification method. See DirectInput documentation for more information.
        /// @return Virtual controller element identifier that matches the DirectInput-style element identifier, if such a match exists.
        std::optional<Controller::SElementIdentifier> IdentifyElement(DWORD dwObj, DWORD dwHow) const;

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
        HRESULT STDMETHODCALLTYPE EnumEffects(DirectInputDeviceType<useUnicode>::EnumEffectsCallbackType lpCallback, LPVOID pvRef, DWORD dwEffType) override;
        HRESULT STDMETHODCALLTYPE EnumEffectsInFile(DirectInputDeviceType<useUnicode>::ConstStringType lptszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE EnumObjects(DirectInputDeviceType<useUnicode>::EnumObjectsCallbackType lpCallback, LPVOID pvRef, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE Escape(LPDIEFFESCAPE pesc) override;
        HRESULT STDMETHODCALLTYPE GetCapabilities(LPDIDEVCAPS lpDIDevCaps) override;
        HRESULT STDMETHODCALLTYPE GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE GetDeviceInfo(DirectInputDeviceType<useUnicode>::DeviceInstanceType* pdidi) override;
        HRESULT STDMETHODCALLTYPE GetDeviceState(DWORD cbData, LPVOID lpvData) override;
        HRESULT STDMETHODCALLTYPE GetEffectInfo(DirectInputDeviceType<useUnicode>::EffectInfoType* pdei, REFGUID rguid) override;
        HRESULT STDMETHODCALLTYPE GetForceFeedbackState(LPDWORD pdwOut) override;
        HRESULT STDMETHODCALLTYPE GetObjectInfo(DirectInputDeviceType<useUnicode>::DeviceObjectInstanceType* pdidoi, DWORD dwObj, DWORD dwHow) override;
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
        HRESULT STDMETHODCALLTYPE WriteEffectToFile(DirectInputDeviceType<useUnicode>::ConstStringType lptszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags) override;

#if DIRECTINPUT_VERSION >= 0x0800
        // -------- METHODS: IDirectInputDevice8 ONLY ------------------------------ //
        HRESULT STDMETHODCALLTYPE BuildActionMap(DirectInputDeviceType<useUnicode>::ActionFormatType* lpdiaf, DirectInputDeviceType<useUnicode>::ConstStringType lpszUserName, DWORD dwFlags) override;
        HRESULT STDMETHODCALLTYPE GetImageInfo(DirectInputDeviceType<useUnicode>::DeviceImageInfoHeaderType* lpdiDevImageInfoHeader) override;
        HRESULT STDMETHODCALLTYPE SetActionMap(DirectInputDeviceType<useUnicode>::ActionFormatType* lpdiActionFormat, DirectInputDeviceType<useUnicode>::ConstStringType lptszUserName, DWORD dwFlags) override;
#endif
    };
}
