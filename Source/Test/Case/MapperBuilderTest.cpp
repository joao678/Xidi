/*****************************************************************************
 * Xidi
 *   DirectInput interface for XInput controllers.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Copyright (c) 2016-2021
 *************************************************************************//**
 * @file MapperBuilderTest.cpp
 *   Unit tests for run-time mapper object building functionality.
 *****************************************************************************/

#include "ControllerTypes.h"
#include "ElementMapper.h"
#include "Mapper.h"
#include "MapperBuilder.h"
#include "TestCase.h"

#include <memory>
#include <optional>
#include <set>
#include <string_view>


namespace XidiTest
{
    using namespace ::Xidi::Controller;


    // -------- INTERNAL FUNCTIONS ----------------------------------------- //

    /// Verifies that the two supplied element mappers are equivalent to one another and flags a test failure if not.
    /// Only works for simple element mappers that uniquely target zero or one specific controller elements and have no side effects.
    /// @param [in] kElementMapperA One side of the comparison.
    /// @param [in] kElementMapperB Another side of the comparison.
    static void VerifyElementMappersAreEquivalent(const IElementMapper& kElementMapperA, const IElementMapper& kElementMapperB)
    {
        TEST_ASSERT(kElementMapperA.GetTargetElementCount() == kElementMapperB.GetTargetElementCount());

        for (int i = 0; i < kElementMapperA.GetTargetElementCount(); ++i)
            TEST_ASSERT(kElementMapperA.GetTargetElementAt(i) == kElementMapperB.GetTargetElementAt(i));
    }

    /// Verifies that the two supplied element maps are equivalent to one another and flags a test failure if not.
    /// Only works for simple element mappers that uniquely target zero or one specific controller elements and have no side effects.
    /// @param [in] kElementMapA One side of the comparison.
    /// @param [in] kElementMapB Another side of the comparison.
    static void VerifyElementMapsAreEquivalent(const Mapper::UElementMap& kElementMapA, const Mapper::UElementMap& kElementMapB)
    {
        for (unsigned int i = 0; i < _countof(Mapper::UElementMap::all); ++i)
        {
            if (nullptr != kElementMapA.all[i])
            {
                TEST_ASSERT(nullptr != kElementMapB.all[i]);
                VerifyElementMappersAreEquivalent(*kElementMapA.all[i], *kElementMapB.all[i]);
            }
            else
            {
                TEST_ASSERT(nullptr == kElementMapB.all[i]);
            }
        }
    }

    /// Verifies that the supplied element map is empty and flags a test failure if not.
    static void VerifyElementMapIsEmpty(const Mapper::UElementMap& kElementMapToCheck)
    {
        for (const auto& elementMapper : kElementMapToCheck.all)
            TEST_ASSERT(elementMapper == nullptr);
    }

    /// Verifies that the specified element map blueprint specification matches a test specification.
    /// Test specification consists of a searchable container holding element map indices, all of which hold an element mapper equivalent to the supplied element mapper.
    /// Only works for simple element mappers that uniquely target zero or one specific controller elements and have no side effects.
    /// Any element map indices not present in the container are expected to be empty.
    /// A test failure is flagged if a mismatch is found.
    /// @tparam StdIndexContainer Standard searchable container type holding the element map indices to check. Any indices less than 0 are ignored.
    /// @param [in] kElementMapLayout Container holding the element map indices and acting as a layout descriptor.
    /// @param [in] kElementMapper Element mapper to be used for equivalence checking at each index of the layout descriptor.
    /// @param [in] kElementMapSpecToCheck Element map descriptor object being checked for matching with the expected spec.
    template <typename StdIndexContainer> static void VerifyElementMapSpecMatchesSpec(const StdIndexContainer& kElementMapLayout, const IElementMapper& kElementMapper, const MapperBuilder::TElementMapSpec& kElementMapSpecToCheck)
    {
        for (unsigned int i = 0; i < _countof(Mapper::UElementMap::all); ++i)
        {
            if (kElementMapLayout.contains(i))
            {
                TEST_ASSERT(true == kElementMapSpecToCheck.contains(i));
                VerifyElementMappersAreEquivalent(kElementMapper, *kElementMapSpecToCheck.at(i));
            }
            else
            {
                TEST_ASSERT(false == kElementMapSpecToCheck.contains(i));
            }
        }
    }

    /// Verifies that the specified element map matches a test specification.
    /// Test specification consists of a searchable container holding element map indices, all of which hold an element mapper equivalent to the supplied element mapper.
    /// Only works for simple element mappers that uniquely target zero or one specific controller elements and have no side effects.
    /// Any element map indices not present in the container are expected to be empty.
    /// A test failure is flagged if a mismatch is found.
    /// @tparam StdIndexContainer Standard searchable container type holding the element map indices to check. Any indices less than 0 are ignored.
    /// @param [in] kElementMapLayout Container holding the element map indices and acting as a layout descriptor.
    /// @param [in] kElementMapper Element mapper to be used for equivalence checking at each index of the layout descriptor.
    /// @param [in] kElementMapToCheck Element map object being checked for matching with the spec.
    template <typename StdIndexContainer> static void VerifyElementMapMatchesSpec(const StdIndexContainer& kElementMapLayout, const IElementMapper& kElementMapper, const Mapper::UElementMap& kElementMapToCheck)
    {
        for (unsigned int i = 0; i < _countof(Mapper::UElementMap::all); ++i)
        {
            if (kElementMapLayout.contains(i))
            {
                TEST_ASSERT(nullptr != kElementMapToCheck.all[i]);
                VerifyElementMappersAreEquivalent(kElementMapper, *kElementMapToCheck.all[i]);
            }
            else
            {
                TEST_ASSERT(nullptr == kElementMapToCheck.all[i]);
            }
        }
    }


    // -------- TEST CASES ------------------------------------------------- //

    // Verifies that blueprints can be created and successfully identified.
    TEST_CASE(MapperBuilder_BlueprintName_Nominal)
    {
        constexpr std::wstring_view kMapperNames[] = {L"TestMapper", L"testMapper", L"TestMapper2", L"testMapper2"};

        MapperBuilder builder;

        for (auto mapperName : kMapperNames)
            TEST_ASSERT(false == builder.DoesBlueprintNameExist(mapperName));

        for (auto mapperName : kMapperNames)
            TEST_ASSERT(true == builder.CreateBlueprint(mapperName));

        for (auto mapperName : kMapperNames)
            TEST_ASSERT(true == builder.DoesBlueprintNameExist(mapperName));
    }

    // Verifies that attempts to create blueprints with the same name are rejected.
    TEST_CASE(MapperBuilder_BlueprintName_DuplicatesRejected)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr int kRepeatTimes = 10;

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (int i = 0; i < kRepeatTimes; ++i)
        {
            TEST_ASSERT(false == builder.CreateBlueprint(kMapperName));
            TEST_ASSERT(true == builder.DoesBlueprintNameExist(kMapperName));
        }
    }

    // Verifies that attempts to create blueprints with the same name as existing mapper objects are rejected.
    // This test uses the names of known documented mappers.
    TEST_CASE(MapperBuilder_BlueprintName_ExistingMapperNameRejected)
    {
        constexpr std::wstring_view kMapperNames[] = {L"StandardGamepad", L"DigitalGamepad", L"ExtendedGamepad", L"XInputNative", L"XInputSharedTriggers"};

        MapperBuilder builder;

        for (auto mapperName : kMapperNames)
            TEST_ASSERT(false == builder.DoesBlueprintNameExist(mapperName));

        for (auto mapperName : kMapperNames)
            TEST_ASSERT(false == builder.CreateBlueprint(mapperName));
    }

    // Verifies that new blueprints are empty upon creation.
    TEST_CASE(MapperBuilder_CreateBlueprint_Empty)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        TEST_ASSERT(true == kElementMapSpec->empty());

        const std::optional<std::wstring_view> kMaybeTemplateName = builder.GetBlueprintTemplate(kMapperName);
        TEST_ASSERT(true == kMaybeTemplateName.has_value());
        TEST_ASSERT(true == kMaybeTemplateName.value().empty());
    }

    // Verifies that element mappers can be set in the nominal case of valid controller elements being specified.
    TEST_CASE(MapperBuilder_ElementMap_Nominal)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr AxisMapper kTestElementMapper(EAxis::X);
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(stickLeftY), ELEMENT_MAP_INDEX_OF(triggerLT)};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        VerifyElementMapSpecMatchesSpec(kControllerElements, kTestElementMapper, *kElementMapSpec);
    }

    // Verifies that element mappers can be set and then cleared, leading to an empty element map specification.
    TEST_CASE(MapperBuilder_ElementMap_EmptyAfterSetAndClear)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr AxisMapper kTestElementMapper(EAxis::X);
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(stickLeftY), ELEMENT_MAP_INDEX_OF(triggerLT)};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
        {
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));
            TEST_ASSERT(true == builder.ClearBlueprintElementMapper(kMapperName, controllerElement));
        }

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        TEST_ASSERT(true == kElementMapSpec->empty());
    }

    // Similar to the nominal case but with the addition of clear attempts which fail.
    TEST_CASE(MapperBuilder_ElementMap_IneffectiveClearNoEffect)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr AxisMapper kTestElementMapper(EAxis::X);
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(stickLeftY), ELEMENT_MAP_INDEX_OF(triggerLT)};
        constexpr int kControllerElementsToClear[] = {ELEMENT_MAP_INDEX_OF(stickLeftX), ELEMENT_MAP_INDEX_OF(stickRightY), ELEMENT_MAP_INDEX_OF(dpadLeft), ELEMENT_MAP_INDEX_OF(triggerRT), ELEMENT_MAP_INDEX_OF(buttonBack)};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));

        for (auto controllerElementToClear : kControllerElementsToClear)
            TEST_ASSERT(false == builder.ClearBlueprintElementMapper(kMapperName, controllerElementToClear));

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        VerifyElementMapSpecMatchesSpec(kControllerElements, kTestElementMapper, *kElementMapSpec);
    }

    // Verifies that element mappers can be set with some being valid and some being invalid.
    TEST_CASE(MapperBuilder_ElementMap_SomeInvalid)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr AxisMapper kTestElementMapper(EAxis::X);

        // Same as above, but with negative indices to indicate invalid controller elements.
        // The condition for successful insertion uses comparison-with-0 to decide whether to expect success or failure.
        // Similarly, the loop that verifies null vs non-null element mappers skips over all indices less than 0.
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(stickLeftY), -1, ELEMENT_MAP_INDEX_OF(triggerLT), -2};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT((controllerElement >= 0) == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        VerifyElementMapSpecMatchesSpec(kControllerElements, kTestElementMapper, *kElementMapSpec);
    }

    // Verifies that element mappers cannot be set on unknown mappers.
    // The element mappers themselves are valid, but the mapper name is unknown.
    TEST_CASE(MapperBuilder_ElementMap_UnknownMapper)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr std::wstring_view kUnknownMapperName = L"UnknownMapper";
        constexpr AxisMapper kTestElementMapper(EAxis::X);
        const std::wstring_view kControllerElements[] = {L"StickLeftY", L"TriggerLT"};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(false == builder.SetBlueprintElementMapper(kUnknownMapperName, controllerElement, kTestElementMapper.Clone()));

        TEST_ASSERT(nullptr == builder.GetBlueprintElementMapSpec(kUnknownMapperName));

        const MapperBuilder::TElementMapSpec* const kElementMapSpec = builder.GetBlueprintElementMapSpec(kMapperName);
        TEST_ASSERT(nullptr != kElementMapSpec);
        TEST_ASSERT(true == kElementMapSpec->empty());
    }

    // Verifies that template names can be set regardless of whether or not they refer to existing mappers, mapper blueprints, or even the mapper blueprint itself.
    // These should all be successful because template names are not checked for semantic correctness until an attempt is made to construct a mapper object.
    TEST_CASE(MapperBuilder_TemplateName_Nominal)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr std::wstring_view kTemplateNames[] = {kMapperName, L"RandomMapper", L"StandardGamepad"};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto templateName : kTemplateNames)
        {
            TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, templateName));
            TEST_ASSERT(true == builder.GetBlueprintTemplate(kMapperName).has_value());
            TEST_ASSERT(templateName == builder.GetBlueprintTemplate(kMapperName).value());
        }
    }

    // Verifies that template name setting attempts are rejected if the mapper name is unknown.
    TEST_CASE(MapperBuilder_TemplateName_UnknownMapper)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr std::wstring_view kUnknownMapperName = L"UnknownMapper";
        constexpr std::wstring_view kTemplateNames[] = {kMapperName, L"RandomMapper", L"StandardGamepad"};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto templateName : kTemplateNames)
        {
            TEST_ASSERT(false == builder.SetBlueprintTemplate(kUnknownMapperName, templateName));
            TEST_ASSERT(false == builder.GetBlueprintTemplate(kUnknownMapperName).has_value());
        }

        TEST_ASSERT(true == builder.GetBlueprintTemplate(kMapperName).has_value());
        TEST_ASSERT(true == builder.GetBlueprintTemplate(kMapperName).value().empty());
    }

    // Verifies that an empty mapper can be built and registered. This is the trivial case.
    // Element map is expected to be empty.
    TEST_CASE(MapperBuilder_Build_NoTemplate_Trivial)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());
        VerifyElementMapIsEmpty(mapper->ElementMap());
    }

    // Verifies that a simple mapper without a template can be built and registered.
    TEST_CASE(MapperBuilder_Build_NoTemplate_Nominal)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr ButtonMapper kTestElementMapper(EButton::B15);
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(buttonA), ELEMENT_MAP_INDEX_OF(triggerLT)};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());
        VerifyElementMapMatchesSpec(kControllerElements, kTestElementMapper, mapper->ElementMap());
    }

    // Verifies that a mapper without a template and with elements marked for removal can be built and registered, the result being an empty element map.
    TEST_CASE(MapperBuilder_Build_NoTemplate_EmptyAfterElementsRemoved)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(buttonA), ELEMENT_MAP_INDEX_OF(triggerLT)};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, nullptr));

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());
        VerifyElementMapIsEmpty(mapper->ElementMap());
    }

    // Verifies that a mapper with a template and no modification can be built and registered.
    // After build is completed, checks that the element mappers all match.
    // For this test the template is a known and documented mapper.
    TEST_CASE(MapperBuilder_Build_Template_NoModifications)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";

        const Mapper* const kTemplateMapper = Mapper::GetByName(L"StandardGamepad");
        TEST_ASSERT(nullptr != kTemplateMapper);

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kTemplateMapper->GetName()));

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());

        VerifyElementMapsAreEquivalent(mapper->ElementMap(), kTemplateMapper->ElementMap());
    }

    // Verifies that a mapper with a template and some changes applied can be built and registered, in this case the changes being element modification.
    // After build is completed, checks that the element mappers all match.
    // For this test the template is a known and documented mapper, and the changes involve switching the triggers to use button 15.
    TEST_CASE(Mapper_Build_Template_WithModification)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr ButtonMapper kTestElementMapper(EButton::B15);
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(triggerLT), ELEMENT_MAP_INDEX_OF(triggerRT)};

        const Mapper* const kTemplateMapper = Mapper::GetByName(L"StandardGamepad");
        TEST_ASSERT(nullptr != kTemplateMapper);

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kTemplateMapper->GetName()));

        Mapper::UElementMap expectedElementMap = kTemplateMapper->CloneElementMap();

        for (auto controllerElement : kControllerElements)
        {
            expectedElementMap.all[controllerElement] = kTestElementMapper.Clone();
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, kTestElementMapper.Clone()));
        }

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());

        const Mapper::UElementMap& kActualElementMap = mapper->ElementMap();
        VerifyElementMapsAreEquivalent(kActualElementMap, expectedElementMap);
    }

    // Verifies that a mapper with a template and some changes applied can be built and registered, in this case the changes being element removal.
    // After build is completed, checks that the element mappers all match.
    // For this test the template is a known and documented mapper, and the changes involve removing the POV.
    TEST_CASE(Mapper_Build_Template_WithRemoval)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        const std::set<int> kControllerElements= {ELEMENT_MAP_INDEX_OF(dpadUp), ELEMENT_MAP_INDEX_OF(dpadDown), ELEMENT_MAP_INDEX_OF(dpadLeft), ELEMENT_MAP_INDEX_OF(dpadRight)};

        const Mapper* const kTemplateMapper = Mapper::GetByName(L"StandardGamepad");
        TEST_ASSERT(nullptr != kTemplateMapper);

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kTemplateMapper->GetName()));

        Mapper::UElementMap expectedElementMap = kTemplateMapper->CloneElementMap();

        for (auto controllerElement: kControllerElements)
        {
            expectedElementMap.all[controllerElement] = nullptr;
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, nullptr));
        }

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());

        const Mapper::UElementMap& kActualElementMap = mapper->ElementMap();
        VerifyElementMapsAreEquivalent(kActualElementMap, expectedElementMap);
    }

    // Verifies that a mapper with a template and no modification can be built and registered.
    // In this test there are changes applied but then cleared before mapper object build.
    // After build is completed, checks that the element mappers all match.
    // For this test the template is a known and documented mapper.
    TEST_CASE(MapperBuilder_Build_Template_WithClearedModifications)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        const std::set<int> kControllerElements = {ELEMENT_MAP_INDEX_OF(dpadUp), ELEMENT_MAP_INDEX_OF(dpadDown), ELEMENT_MAP_INDEX_OF(dpadLeft), ELEMENT_MAP_INDEX_OF(dpadRight)};

        const Mapper* const kTemplateMapper = Mapper::GetByName(L"StandardGamepad");
        TEST_ASSERT(nullptr != kTemplateMapper);

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kTemplateMapper->GetName()));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.SetBlueprintElementMapper(kMapperName, controllerElement, nullptr));

        for (auto controllerElement : kControllerElements)
            TEST_ASSERT(true == builder.ClearBlueprintElementMapper(kMapperName, controllerElement));

        std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
        TEST_ASSERT(nullptr != mapper);
        TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());

        VerifyElementMapsAreEquivalent(mapper->ElementMap(), kTemplateMapper->ElementMap());
    }

    // Verifies that a mapper fails to be built if it refers to itself as its own template.
    TEST_CASE(Mapper_Build_Template_SelfReference)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kMapperName));
        TEST_ASSERT(nullptr == builder.Build(kMapperName));
    }

    // Verifies that a mapper fails to be built if it refers to an unkown mapper as its template.
    TEST_CASE(Mapper_Build_Template_InvalidReference)
    {
        constexpr std::wstring_view kMapperName = L"TestMapper";
        constexpr std::wstring_view kTemplateName = L"UnknownMapper";

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
        TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kTemplateName));
        TEST_ASSERT(nullptr == builder.Build(kMapperName));
    }

    // Verifies that mapper build succeeds in the presence of an acyclic chain of template dependencies.
    // This test will pollute the global mapper registry, so the mapper names are made especially unique.
    TEST_CASE(Mapper_Build_Template_Chain)
    {
        constexpr std::wstring_view kMapperNames[] = {L"TestMapperTemplateChainA", L"TestMapperTemplateChainB", L"TestMapperTemplateChainC", L"TestMapperTemplateChainD", L"TestMapperTemplateChainE", L"TestMapperTemplateChainF", L"TestMapperTemplateChainG"};

        MapperBuilder builder;

        for (int i = 0; i < _countof(kMapperNames); ++i)
        {
            const int kNameIndex = i;
            const int kTemplateIndex = i + 1;

            TEST_ASSERT(true == builder.CreateBlueprint(kMapperNames[kNameIndex]));

            if (kTemplateIndex < _countof(kMapperNames))
                TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperNames[kNameIndex], kMapperNames[kTemplateIndex]));
        }

        TEST_ASSERT(nullptr != builder.Build(kMapperNames[0]));
        for (auto kMapperName : kMapperNames)
            TEST_ASSERT(true == Mapper::IsMapperNameKnown(kMapperName));
    }

    // Verifies that mapper build succeeds in the presence of an acyclic forking chain of template dependencies.
    // This test will pollute the global mapper registry with the common dependency mapper, so its name is especially unique.
    TEST_CASE(Mapper_Build_Template_Fork)
    {
        constexpr std::wstring_view kMapperNameCommonDependency = L"TestMapperTemplateForkCommonDep";
        constexpr std::wstring_view kMapperNames[] = {L"TestMapperA", L"TestMapperB"};

        MapperBuilder builder;
        TEST_ASSERT(true == builder.CreateBlueprint(kMapperNameCommonDependency));

        for (auto kMapperName : kMapperNames)
        {
            TEST_ASSERT(true == builder.CreateBlueprint(kMapperName));
            TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperName, kMapperNameCommonDependency));
        }

        for (auto kMapperName : kMapperNames)
        {
            std::unique_ptr<const Mapper> mapper(builder.Build(kMapperName));
            TEST_ASSERT(nullptr != mapper);
            TEST_ASSERT(Mapper::GetByName(kMapperName) == mapper.get());
        }

        TEST_ASSERT(true == Mapper::IsMapperNameKnown(kMapperNameCommonDependency));
    }

    // Verifies that mapper build fails if there is a cycle in the template dependence graph.
    TEST_CASE(Mapper_Build_Template_Cycle)
    {
        constexpr std::wstring_view kMapperNames[] = {L"TestMapperA", L"TestMapperB", L"TestMapperC", L"TestMapperD", L"TestMapperE", L"TestMapperF", L"TestMapperG"};

        MapperBuilder builder;

        for (int i = 0; i < _countof(kMapperNames); ++i)
        {
            const int kNameIndex = i;
            const int kTemplateIndex = (i + 1) % _countof(kMapperNames);

            TEST_ASSERT(true == builder.CreateBlueprint(kMapperNames[kNameIndex]));
            TEST_ASSERT(true == builder.SetBlueprintTemplate(kMapperNames[kNameIndex], kMapperNames[kTemplateIndex]));
        }

        for (int i = 0; i < _countof(kMapperNames); ++i)
            TEST_ASSERT(nullptr == builder.Build(kMapperNames[i]));
    }
}