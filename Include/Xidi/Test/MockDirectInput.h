/***************************************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 ***************************************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2023
 ***********************************************************************************************//**
 * @file MockDirectInput.h
 *   Declaration of a mock version of the system-supplied DirectInput interface object along with
 *   additional testing-specific functions.
 **************************************************************************************************/

#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "ApiDirectInput.h"
#include "MockDirectInputDevice.h"
#include "WrapperIDirectInput.h"

namespace XidiTest
{
  using ::Xidi::DirectInputType;

  /// Mock version of the IDirectInput interface, used to test interaction with system-supplied
  /// DirectInput objects. Not all methods are fully implemented based on the requirements of the
  /// test cases that exist.
  class MockDirectInput : public DirectInputType<kDirectInputTestCharMode>::LatestIDirectInputType
  {
  public:

    inline MockDirectInput(void) = default;

    inline MockDirectInput(std::set<SDirectInputDeviceInfo, std::less<>>&& mockSystemDevices)
        : kMockSystemDevices(std::move(mockSystemDevices))
    {}

    /// Retrieves and returns the number of system devices held by this object.
    /// @return Number of system devices present.
    inline size_t GetSystemDeviceCount(void) const
    {
      return kMockSystemDevices.size();
    }

    /// Retrieves and returns the number of system devices held by this object that match a
    /// specified filter predicate.
    /// @return Number of matching system devices present.
    template <typename FilterPredicate> inline size_t GetSystemDeviceCountFiltered(
        FilterPredicate predicate) const
    {
      size_t numDevices = 0;

      for (const auto& kMockSystemDevice : kMockSystemDevices)
      {
        if (true == predicate(kMockSystemDevice)) numDevices += 1;
      }

      return numDevices;
    }

    // IDirectInput
    HRESULT __stdcall CreateDevice(
        REFGUID rguid,
        DirectInputType<kDirectInputTestCharMode>::EarliestIDirectInputDeviceType**
            lplpDirectInputDevice,
        LPUNKNOWN pUnkOuter) override;
    HRESULT __stdcall EnumDevices(
        DWORD dwDevType,
        DirectInputType<kDirectInputTestCharMode>::EnumDevicesCallbackType lpCallback,
        LPVOID pvRef,
        DWORD dwFlags) override;
    HRESULT __stdcall FindDevice(
        REFGUID rguidClass,
        DirectInputType<kDirectInputTestCharMode>::ConstStringType ptszName,
        LPGUID pguidInstance) override;
    HRESULT __stdcall GetDeviceStatus(REFGUID rguidInstance) override;
    HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion) override;
    HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) override;
#if DIRECTINPUT_VERSION >= 0x0800
    HRESULT __stdcall ConfigureDevices(
        LPDICONFIGUREDEVICESCALLBACK lpdiCallback,
        DirectInputType<kDirectInputTestCharMode>::ConfigureDevicesParamsType lpdiCDParams,
        DWORD dwFlags,
        LPVOID pvRefData) override;
    HRESULT __stdcall EnumDevicesBySemantics(
        DirectInputType<kDirectInputTestCharMode>::ConstStringType ptszUserName,
        DirectInputType<kDirectInputTestCharMode>::ActionFormatType lpdiActionFormat,
        DirectInputType<kDirectInputTestCharMode>::EnumDevicesBySemanticsCallbackType lpCallback,
        LPVOID pvRef,
        DWORD dwFlags) override;
#else
    HRESULT __stdcall CreateDeviceEx(
        REFGUID rguid, REFIID riid, LPVOID* lplpDirectInputDevice, LPUNKNOWN pUnkOuter) override;
#endif

    // IUnknown
    HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj) override;
    ULONG __stdcall AddRef(void) override;
    ULONG __stdcall Release(void) override;

  private:

    /// All devices known to the simulated system.
    /// These are the devices that are available to be created and enumerated.
    /// Set once at compile-time and never updated.
    const std::set<SDirectInputDeviceInfo, std::less<>> kMockSystemDevices;

    /// Registry of all device objects created via method calls to this object.
    /// All such objects are automatically destroyed when this object is destroyed.
    std::set<std::unique_ptr<MockDirectInputDevice>> createdDevices;
  };
} // namespace XidiTest
