/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "UDataSerializer.h"
#include "ApiModel/Class.h"

class FEnumElement;
class FFunction;
class FUdProperty;
class FStruct;
class FEnum;
class FModule;
class FRoot;
class FIdentifierName;
class FBasicIdentifierInfo;
class FIdentifier;
class FMetadata;
class FParameter;
class FType;

class UNREALISTICDOCS_API FJsonUDataSerializer : public IUDataSerializer
{
public:
    friend class FTypeSerializer;

    FJsonUDataSerializer();

    FString Serialize(const FRoot& Root) override;

private:
    static const FString IdentifierField;
    static const FString UndefinedValue;
    static const FString TypeField;

    static const FString RootModulesField;
    static const FString RootMetadataField;

    static const FString ModuleClassesField;
    static const FString ModuleStructsField;
    static const FString ModuleEnumsField;

    static const FString ClassTypeClass;
    static const FString ClassTypeSubsystem;
    static const FString ClassTypeDevSettings;
    static const FString ClassTypeComponent;
    static const FString ClassTypeWidget;
    static const FString ClassTypeFunctionLibrary;
    static const FString ClassTypeActor;

    static const FString ClassIsAbstractField;
    static const FString ClassIsInterfaceField;
    static const FString ClassIsBlueprintTypeField;
    static const FString ClassIsBlueprintableField;
    static const FString ClassTypeField;
    static const FString ClassSuperClassField;
    static const FString ClassNamespaceField;
    static const FString ClassConfigNameField;
    static const FString ClassInterfacesField;
    static const FString ClassFunctionsField;
    static const FString ClassPropertiesField;

    static const FString StructIsBlueprintTypeField;
    static const FString StructPropertiesField;

    static const FString EnumIsBlueprintTypeField;
    static const FString EnumElementsField;
    static const FString EnumElementIsHiddenField;
    static const FString EnumElementDescriptionField;

    static const FString PropertyIsInstanceEditableField;
    static const FString PropertyIsArchetypeEditableField;
    static const FString PropertyIsBlueprintReadOnlyField;
    static const FString PropertyIsExposeOnSpawnField;
    static const FString PropertyIsPrivateField;
    static const FString PropertyIsExposeToCinematicField;
    static const FString PropertyIsConfigVariableField;
    static const FString PropertyIsBlueprintVisibleField;
    static const FString PropertyClampMinField;
    static const FString PropertyClampMaxField;
    static const FString PropertyDefaultValueField;
    static const FString PropertyReplicationModeField;
    static const FString PropertyReplicationConditionField;
    static const FString PropertyMulticastDelegateFlagsField;

    static const FString BlueprintAssignableMulticastDelegateFlag;
    static const FString BlueprintCallableMulticastDelegateFlag;
    static const FString BlueprintAuthorityOnlyMulticastDelegateFlag;

    static const FString ReplicationModeNone;
    static const FString ReplicationModeReplicate;
    static const FString ReplicationModeRepNotify;

    static const FString ReplicationConditionNone;
    static const FString ReplicationConditionInitialOnly;
    static const FString ReplicationConditionOwnerOnly;
    static const FString ReplicationConditionSkipOwner;
    static const FString ReplicationConditionSimulatedOnly;
    static const FString ReplicationConditionAutonomousOnly;
    static const FString ReplicationConditionSimulatedOrPhysics;
    static const FString ReplicationConditionInitialOrOwner;
    static const FString ReplicationConditionCustom;
    static const FString ReplicationConditionReplayOrOwner;
    static const FString ReplicationConditionReplayOnly;
    static const FString ReplicationConditionSimulatedOnlyNoReplay;
    static const FString ReplicationConditionSimulatedOrPhysicsNoReplay;
    static const FString ReplicationConditionSkipReplay;
    static const FString ReplicationConditionNever;

    static const FString NameNameField;
    static const FString NameDisplayNameField;

    static const FString BasicIdentifierInfoNameField;
    static const FString BasicIdentifierInfoFullPathField;
    static const FString BasicIdentifierInfoIsNativeDefinedField;

    static const FString IdentifierTooltipField;
    static const FString IdentifierShortTooltipField;
    static const FString IdentifierCategoryField;
    static const FString IdentifierDeprecationMessageField;
    static const FString IdentifierIsDeprecatedField;

    static const FString FunctionKeywordsField;
    static const FString FunctionCompactNodeTitleField;
    static const FString FunctionRawCommentField;
    static const FString FunctionNodeCommentField;
    static const FString FunctionAccessModifierField;
    static const FString FunctionRpcModeField;
    static const FString FunctionIsPureField;
    static const FString FunctionIsCallInEditorField;
    static const FString FunctionIsOverrideField;
    static const FString FunctionIsEventField;
    static const FString FunctionIsReliableField;
    static const FString FunctionIsStaticField;
    static const FString FunctionIsConstField;
    static const FString FunctionIsBlueprintCallableField;
    static const FString FunctionIsOverridableField;
    static const FString FunctionIsBlueprintProtectedField;
    static const FString FunctionIsLatent;
    static const FString FunctionIsAuthorityOnly;
    static const FString FunctionIsCosmetic;
    static const FString FunctionParametersField;
    static const FString FunctionReturnParametersField;

    static const FString ParameterNameField;
    static const FString ParameterDefaultValueField;

    static const FString PrivateAccessModifier;
    static const FString ProtectedAccessModifier;
    static const FString PublicAccessModifier;

    static const FString ClientRpcMode;
    static const FString ServerRpcMode;
    static const FString NetMulticastRpcMode;
    static const FString NotReplicatedRpcMode;

    static const FString MetadataVersionMajorField;
    static const FString MetadataVersionMinorField;
    static const FString MetadataVersionIndexField;
    static const FString MetadataVersionTagField;
    static const FString MetadataVersionBuildField;
    static const FString MetadataVersionBuildIdField;
    static const FString MetadataVersionTimestampField;

    static const FString MetadataGraphStyleMarginsField;
    static const FString MetadataGraphStyleNodeColorsField;
    static const FString MetadataGraphStyleTypeColorsField;

    static const FString MetadataVersionField;
    static const FString MetadataGraphStyleField;

    static const FString MetadataGraphStyleMarginsInputPinMarginField;
    static const FString MetadataGraphStyleMarginsOutputPinMarginField;
    static const FString MetadataGraphStyleMarginsNonPinNodeBodyMarginField;

    static const FString MetadataGraphStyleNodeColorsEventNodeTitleColorField;
    static const FString
        MetadataGraphStyleNodeColorsFunctionCallNodeTitleColorField;
    static const FString
        MetadataGraphStyleNodeColorsPureFunctionCallNodeTitleColorField;

    static const FString MetadataGraphStyleNodeColorsDefaultPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsExecutionPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsBooleanPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsBytePinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsClassPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsIntPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsInt64PinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsFloatPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsNamePinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsDelegatePinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsObjectPinTypeColorField;
    static const FString
        MetadataGraphStyleNodeColorsSoftObjectPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsSoftClassPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsInterfacePinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsStringPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsTextPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsStructPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsWildcardPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsVectorPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsRotatorPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsTransformPinTypeColorField;
    static const FString MetadataGraphStyleNodeColorsIndexPinTypeColorField;

    static const FString MarginTopField;
    static const FString MarginRightField;
    static const FString MarginBottomField;
    static const FString MarginLeftField;

    static const FString ColorRField;
    static const FString ColorGField;
    static const FString ColorBField;
    static const FString ColorAField;

    TMap<EReplication, FString> ReplicationModes;
    TMap<ELifetimeCondition, FString> ReplicationConditions;
    TMap<EMulticastDelegateFlags, FString> MulticastDelegateFlags;

    TSharedPtr<FJsonObject> SerializeModule(const FModule& Module) const;

    TSharedPtr<FJsonObject> SerializeClass(const FClass& Class) const;

    TSharedPtr<FJsonObject> SerializeStruct(const FStruct& Struct) const;

    TSharedPtr<FJsonObject> SerializeEnum(const FEnum& Enum) const;

    TSharedPtr<FJsonObject> SerializeEnumElement(
        const FEnumElement& EnumElement) const;

    TSharedPtr<FJsonObject> SerializeProperty(
        const FUdProperty& Property) const;

    TSharedPtr<FJsonObject> SerializeMetadata(
        const FMetadata& Metadata) const;

    static TSharedPtr<FJsonObject> SerializeFunction(const FFunction& Function);

    static TSharedPtr<FJsonObject> SerializeParameter(
        const FParameter& Parameter);

    static TSharedPtr<FJsonObject> SerializeType(const FType& Type);

    static TSharedPtr<FJsonObject> SerializeName(
        const FIdentifierName& Name);

    static TSharedPtr<FJsonObject> SerializeBasicIdentifierInfo(
        const FBasicIdentifierInfo& BasicIdentifierInfo);

    static TSharedPtr<FJsonObject> SerializeIdentifier(
        const FIdentifier& Identifier);

    static FString SerializeClassType(EClassType ClassType);

    TSharedPtr<FJsonObject> SerializeMargin(const FMargin& Margin) const;

    TSharedPtr<FJsonObject> SerializeLinearColor(
        const FLinearColor& Color) const;
};
