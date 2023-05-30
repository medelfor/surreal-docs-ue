/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "JsonUDataSerializer.h"

#include "ApiModel/Module.h"
#include "ApiModel/Root.h"
#include "Dom/JsonObject.h"

#include "ApiModel/Types/Type.h"
#include "ApiModel/Types/PrimitiveType.h"
#include "ApiModel/Types/StructType.h"
#include "ApiModel/Types/EnumType.h"
#include "ApiModel/Types/ArrayType.h"
#include "ApiModel/Types/SetType.h"
#include "ApiModel/Types/MapType.h"
#include "ApiModel/Types/InterfaceType.h"
#include "ApiModel/Types/ObjectType.h"
#include "ApiModel/Types/DelegateType.h"
#include "Serialization/JsonSerializer.h"

class FTypeSerializer : public ITypeVisitor
{
public:
    FTypeSerializer()
    {
        // primitive types (PrimitiveType::EPrimitiveType)
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Float,
            FloatPrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Double,
            DoublePrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Byte,
            BytePrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Int,
            IntPrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Int64,
            Int64PrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Bool,
            BoolPrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::String,
            StringPrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Text,
            TextPrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Name,
            NamePrimitiveType);
        PrimitiveTypes.Add(PrimitiveType::EPrimitiveType::Undefined,
            UndefinedPrimitiveType);

        // object references (EObjectReferences)
        ObjectReferences.Add(EObjectReference::Object,
            ObjectObjectReference);
        ObjectReferences.Add(EObjectReference::WeakObject,
            WeakObjectObjectReference);
        ObjectReferences.Add(EObjectReference::LazyObject,
            LazyObjectObjectReference);
        ObjectReferences.Add(EObjectReference::SoftObject,
            SoftObjectObjectReference);
        ObjectReferences.Add(EObjectReference::Class,
            ClassObjectReference);
        ObjectReferences.Add(EObjectReference::SoftClass,
            SoftClassObjectReference);
        ObjectReferences.Add(EObjectReference::Undefined,
            UndefinedObjectReference);
    }

    void Visit(const FPrimitiveType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        // primitive subtype
        const FString* PrimitiveTypeIt = PrimitiveTypes
            .Find(Type.GetPrimitiveType());
        TypeJson->SetStringField(PrimitiveTypeField,
            PrimitiveTypeIt ? *PrimitiveTypeIt : UndefinedValue);

        TypeJson->SetStringField(TypeField, PrimitiveType);

        ResultType = TypeJson;
    }

    void Visit(const FStructType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        TypeJson->SetStringField(TypeField, StructType);

        ResultType = TypeJson;
    }

    void Visit(const FEnumType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        TypeJson->SetStringField(TypeField, EnumType);

        ResultType = TypeJson;
    }

    void Visit(const FArrayType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        // element type
        FTypeSerializer TypeSerializer;
        Type.GetElementType()->Accept(TypeSerializer);
        TypeJson->SetObjectField(ElementTypeField,
            TypeSerializer.GetSerializationResult());

        TypeJson->SetStringField(TypeField, ArrayType);

        ResultType = TypeJson;
    }

    void Visit(const FSetType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        // element type
        FTypeSerializer TypeSerializer;
        Type.GetElementType()->Accept(TypeSerializer);
        TypeJson->SetObjectField(ElementTypeField,
            TypeSerializer.GetSerializationResult());

        TypeJson->SetStringField(TypeField, SetType);

        ResultType = TypeJson;
    }

    void Visit(const FMapType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        FTypeSerializer TypeSerializer;

        // key type
        Type.GetKeyType()->Accept(TypeSerializer);
        TypeJson->SetObjectField(KeyTypeField,
            TypeSerializer.GetSerializationResult());

        // value type
        Type.GetValueType()->Accept(TypeSerializer);
        TypeJson->SetObjectField(ValueTypeField,
            TypeSerializer.GetSerializationResult());

        TypeJson->SetStringField(TypeField, MapType);

        ResultType = TypeJson;
    }

    void Visit(const FInterfaceType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        TypeJson->SetStringField(TypeField, InterfaceType);

        ResultType = TypeJson;
    }

    void Visit(const FObjectType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        // reference type
        const FString* ObjectReferenceIt = ObjectReferences
            .Find(Type.GetObjectReference());
        TypeJson->SetStringField(ObjectReferenceField,
            ObjectReferenceIt ? *ObjectReferenceIt : UndefinedValue);

        // class type
        TypeJson->SetStringField(ClassTypeField,
            FJsonUDataSerializer::SerializeClassType(
                Type.GetClassType()));
        TypeJson->SetStringField(TypeField, ObjectType);

        ResultType = TypeJson;
    }

    void Visit(const FDelegateType& Type) override
    {
        TSharedPtr<FJsonObject> TypeJson = SerializeTypeBase(Type);

        TypeJson->SetObjectField(SignatureField,
            FJsonUDataSerializer::SerializeFunction(
                *Type.GetSignature()));
        TypeJson->SetBoolField(IsMulticastField, Type.IsMulticast());
        TypeJson->SetStringField(TypeField, DelegateType);

        ResultType = TypeJson;
    }

    TSharedPtr<FJsonObject> GetSerializationResult() const
    {
        return ResultType;
    }

private:
    static const FString UndefinedValue;

    static const FString BasicIdentifierInfoField;
    static const FString IsReferenceField;
    static const FString IsConstField;
    static const FString TypeField;

    static const FString ElementTypeField;

    static const FString KeyTypeField;
    static const FString ValueTypeField;

    static const FString PrimitiveTypeField;

    static const FString IsMulticastField;
    static const FString SignatureField;

    static const FString ClassTypeField;
    static const FString ObjectReferenceField;

    static const FString ObjectObjectReference;
    static const FString WeakObjectObjectReference;
    static const FString LazyObjectObjectReference;
    static const FString SoftObjectObjectReference;
    static const FString ClassObjectReference;
    static const FString SoftClassObjectReference;
    static const FString UndefinedObjectReference;

    static const FString PrimitiveType;
    static const FString StructType;
    static const FString EnumType;
    static const FString ArrayType;
    static const FString SetType;
    static const FString MapType;
    static const FString InterfaceType;
    static const FString ObjectType;
    static const FString DelegateType;

    static const FString FloatPrimitiveType;
    static const FString DoublePrimitiveType;
    static const FString BytePrimitiveType;
    static const FString IntPrimitiveType;
    static const FString Int64PrimitiveType;
    static const FString BoolPrimitiveType;
    static const FString StringPrimitiveType;
    static const FString TextPrimitiveType;
    static const FString NamePrimitiveType;
    static const FString UndefinedPrimitiveType;

    TMap<PrimitiveType::EPrimitiveType, FString> PrimitiveTypes;
    TMap<EObjectReference, FString> ObjectReferences;
    TSharedPtr<FJsonObject> ResultType;

    TSharedPtr<FJsonObject> SerializeTypeBase(const FType& Type)
    {
        TSharedPtr<FJsonObject> TypeJson = MakeShared<FJsonObject>();

        TypeJson->SetObjectField(BasicIdentifierInfoField,
            FJsonUDataSerializer::SerializeBasicIdentifierInfo(
                Type.GetIdentifierInfo()));
        TypeJson->SetBoolField(IsReferenceField, Type.IsReference());
        TypeJson->SetBoolField(IsConstField, Type.IsConst());

        return TypeJson;
    }
};

const FString FTypeSerializer::UndefinedValue("");

const FString FTypeSerializer::BasicIdentifierInfoField("basic_info");
const FString FTypeSerializer::IsReferenceField("is_reference");
const FString FTypeSerializer::IsConstField("is_const");
const FString FTypeSerializer::TypeField("type");

const FString FTypeSerializer::ElementTypeField("element_type");

const FString FTypeSerializer::KeyTypeField("key_type");
const FString FTypeSerializer::ValueTypeField("value_type");

const FString FTypeSerializer::IsMulticastField("is_multicast");
const FString FTypeSerializer::SignatureField("signature");

const FString FTypeSerializer::ClassTypeField("class_type");
const FString FTypeSerializer::ObjectReferenceField("reference");

const FString FTypeSerializer::PrimitiveTypeField("primitive_type");

const FString FTypeSerializer::ObjectObjectReference("object");
const FString FTypeSerializer::WeakObjectObjectReference("weak_object");
const FString FTypeSerializer::LazyObjectObjectReference("lazy_object");
const FString FTypeSerializer::SoftObjectObjectReference("soft_object");
const FString FTypeSerializer::ClassObjectReference("class");
const FString FTypeSerializer::SoftClassObjectReference("soft_class");
const FString FTypeSerializer::UndefinedObjectReference("undefined");

const FString FTypeSerializer::PrimitiveType("primitive");
const FString FTypeSerializer::StructType("struct");
const FString FTypeSerializer::EnumType("enum");
const FString FTypeSerializer::ArrayType("array");
const FString FTypeSerializer::SetType("set");
const FString FTypeSerializer::MapType("map");
const FString FTypeSerializer::InterfaceType("interface");
const FString FTypeSerializer::ObjectType("object");
const FString FTypeSerializer::DelegateType("delegate");

const FString FTypeSerializer::FloatPrimitiveType("float");
const FString FTypeSerializer::DoublePrimitiveType("double");
const FString FTypeSerializer::BytePrimitiveType("byte");
const FString FTypeSerializer::IntPrimitiveType("int");
const FString FTypeSerializer::Int64PrimitiveType("int64");
const FString FTypeSerializer::BoolPrimitiveType("bool");
const FString FTypeSerializer::StringPrimitiveType("string");
const FString FTypeSerializer::TextPrimitiveType("text");
const FString FTypeSerializer::NamePrimitiveType("name");
const FString FTypeSerializer::UndefinedPrimitiveType("undefined");

const FString FJsonUDataSerializer::IdentifierField("identifier");
const FString FJsonUDataSerializer::UndefinedValue("");
const FString FJsonUDataSerializer::TypeField("type");

const FString FJsonUDataSerializer::RootModulesField("modules");
const FString FJsonUDataSerializer::RootMetadataField("metadata");

const FString FJsonUDataSerializer::ModuleClassesField("classes");
const FString FJsonUDataSerializer::ModuleStructsField("structs");
const FString FJsonUDataSerializer::ModuleEnumsField("enums");

const FString FJsonUDataSerializer::ClassTypeClass("class");
const FString FJsonUDataSerializer::ClassTypeComponent("component");
const FString FJsonUDataSerializer::ClassTypeWidget("widget");
const FString FJsonUDataSerializer::
    ClassTypeFunctionLibrary("function_library");
const FString FJsonUDataSerializer::ClassTypeActor("actor");

const FString FJsonUDataSerializer::ClassIsAbstractField("is_abstract");
const FString FJsonUDataSerializer::ClassIsInterfaceField("is_interface");
const FString FJsonUDataSerializer::
    ClassIsBlueprintTypeField("is_blueprint_type");
const FString FJsonUDataSerializer::
    ClassIsBlueprintableField("is_blueprintable");
const FString FJsonUDataSerializer::ClassTypeField("class_type");
const FString FJsonUDataSerializer::ClassSuperClassField("super_class");
const FString FJsonUDataSerializer::ClassNamespaceField("namespace");
const FString FJsonUDataSerializer::ClassConfigNameField("config_name");
const FString FJsonUDataSerializer::ClassInterfacesField("interfaces");
const FString FJsonUDataSerializer::ClassFunctionsField("functions");
const FString FJsonUDataSerializer::ClassPropertiesField("properties");

const FString FJsonUDataSerializer::
    StructIsBlueprintTypeField("is_blueprint_type");
const FString FJsonUDataSerializer::StructPropertiesField("properties");

const FString FJsonUDataSerializer::
    EnumIsBlueprintTypeField("is_blueprint_type");
const FString FJsonUDataSerializer::EnumElementsField("elements");
const FString FJsonUDataSerializer::EnumElementIsHiddenField("is_hidden");
const FString FJsonUDataSerializer::
    EnumElementDescriptionField("description");

const FString FJsonUDataSerializer::
    PropertyIsInstanceEditableField("is_instance_editable");
const FString FJsonUDataSerializer::
    PropertyIsArchetypeEditableField("is_archetype_editable");
const FString FJsonUDataSerializer::
    PropertyIsBlueprintReadOnlyField("is_read_only");
const FString FJsonUDataSerializer::
    PropertyIsExposeOnSpawnField("is_expose_on_spawn");
const FString FJsonUDataSerializer::PropertyIsPrivateField("is_private");
const FString FJsonUDataSerializer::
    PropertyIsExposeToCinematicField("is_expose_to_cinematic");
const FString FJsonUDataSerializer::
    PropertyIsConfigVariableField("is_config_variable");
const FString FJsonUDataSerializer::
    PropertyIsBlueprintVisibleField("is_blueprint_visible");
const FString FJsonUDataSerializer::PropertyClampMinField("min");
const FString FJsonUDataSerializer::PropertyClampMaxField("max");
const FString FJsonUDataSerializer::
    PropertyDefaultValueField("default_value");
const FString FJsonUDataSerializer::
    PropertyReplicationModeField("replication");
const FString FJsonUDataSerializer::
    PropertyReplicationConditionField("replication_condition");
const FString FJsonUDataSerializer::
    PropertyMulticastDelegateFlagsField("multicast_delegate_flags");

const FString FJsonUDataSerializer::
    BlueprintAssignableMulticastDelegateFlag("assignable");
const FString FJsonUDataSerializer::
    BlueprintCallableMulticastDelegateFlag("callable");
const FString FJsonUDataSerializer::
    BlueprintAuthorityOnlyMulticastDelegateFlag("authority_only");

const FString FJsonUDataSerializer::
    ReplicationModeNone("none");
const FString FJsonUDataSerializer::
    ReplicationModeReplicate("replicate");
const FString FJsonUDataSerializer::
    ReplicationModeRepNotify("repnotify");

const FString FJsonUDataSerializer::
    ReplicationConditionNone("none");
const FString FJsonUDataSerializer::
    ReplicationConditionInitialOnly("initial_only");
const FString FJsonUDataSerializer::
    ReplicationConditionOwnerOnly("owner_only");
const FString FJsonUDataSerializer::
    ReplicationConditionSkipOwner("skip_owner");
const FString FJsonUDataSerializer::
    ReplicationConditionSimulatedOnly("simulated_only");
const FString FJsonUDataSerializer::
    ReplicationConditionAutonomousOnly("autonomous_only");
const FString FJsonUDataSerializer::
    ReplicationConditionSimulatedOrPhysics("simulated_or_physics");
const FString FJsonUDataSerializer::
    ReplicationConditionInitialOrOwner("initial_or_owner");
const FString FJsonUDataSerializer::
    ReplicationConditionCustom("custom");
const FString FJsonUDataSerializer::
    ReplicationConditionReplayOrOwner("replay_or_owner");
const FString FJsonUDataSerializer::
    ReplicationConditionReplayOnly("replay_only");
const FString FJsonUDataSerializer::
    ReplicationConditionSimulatedOnlyNoReplay("simulated_only_no_replay");
const FString FJsonUDataSerializer::
    ReplicationConditionSimulatedOrPhysicsNoReplay(
        "simulated_or_physics_no_replay");
const FString FJsonUDataSerializer::
    ReplicationConditionSkipReplay("skip_replay");
const FString FJsonUDataSerializer::
    ReplicationConditionNever("never");

const FString FJsonUDataSerializer::NameNameField("name");
const FString FJsonUDataSerializer::NameDisplayNameField("display_name");

const FString FJsonUDataSerializer::BasicIdentifierInfoNameField("name");
const FString FJsonUDataSerializer::BasicIdentifierInfoFullPathField("path");
const FString FJsonUDataSerializer::
    BasicIdentifierInfoIsNativeDefinedField("is_native");

const FString FJsonUDataSerializer::IdentifierTooltipField("tooltip");
const FString FJsonUDataSerializer::
    IdentifierShortTooltipField("short_tooltip");
const FString FJsonUDataSerializer::IdentifierCategoryField("category");
const FString FJsonUDataSerializer::
    IdentifierDeprecationMessageField("deprecation_message");
const FString FJsonUDataSerializer::
    IdentifierIsDeprecatedField("is_deprecated");

const FString FJsonUDataSerializer::FunctionKeywordsField("keywords");
const FString FJsonUDataSerializer::
    FunctionCompactNodeTitleField("compact_node_title");
const FString FJsonUDataSerializer::FunctionRawCommentField("raw_comment");
const FString FJsonUDataSerializer::FunctionNodeCommentField("node_comment");
const FString FJsonUDataSerializer::FunctionAccessModifierField("access");
const FString FJsonUDataSerializer::FunctionRpcModeField("rpc");
const FString FJsonUDataSerializer::FunctionIsPureField("is_pure");
const FString FJsonUDataSerializer::
    FunctionIsCallInEditorField("is_call_in_editor");
const FString FJsonUDataSerializer::FunctionIsOverrideField("is_override");
const FString FJsonUDataSerializer::FunctionIsEventField("is_event");
const FString FJsonUDataSerializer::FunctionIsReliableField("is_reliable");
const FString FJsonUDataSerializer::FunctionIsStaticField("is_static");
const FString FJsonUDataSerializer::FunctionIsConstField("is_const");
const FString FJsonUDataSerializer::
    FunctionIsBlueprintCallableField("is_blueprint_callable");
const FString FJsonUDataSerializer::
    FunctionIsOverridableField("is_overridable");
const FString FJsonUDataSerializer::
    FunctionIsBlueprintProtectedField("is_blueprint_protected");
const FString FJsonUDataSerializer::FunctionIsLatent("is_latent");
const FString FJsonUDataSerializer::
    FunctionIsAuthorityOnly("is_authority_only");
const FString FJsonUDataSerializer::FunctionIsCosmetic("is_cosmetic");
const FString FJsonUDataSerializer::FunctionParametersField("parameters");
const FString FJsonUDataSerializer::FunctionReturnParametersField("return");

const FString FJsonUDataSerializer::ParameterNameField("name");
const FString FJsonUDataSerializer::
    ParameterDefaultValueField("default_value");

const FString FJsonUDataSerializer::PrivateAccessModifier("private");
const FString FJsonUDataSerializer::ProtectedAccessModifier("protected");
const FString FJsonUDataSerializer::PublicAccessModifier("public");

const FString FJsonUDataSerializer::ClientRpcMode("client");
const FString FJsonUDataSerializer::ServerRpcMode("server");
const FString FJsonUDataSerializer::NetMulticastRpcMode("net_multicast");
const FString FJsonUDataSerializer::NotReplicatedRpcMode("not_replicated");

const FString FJsonUDataSerializer::MetadataVersionMajorField("major");
const FString FJsonUDataSerializer::MetadataVersionMinorField("minor");
const FString FJsonUDataSerializer::MetadataVersionIndexField("index");
const FString FJsonUDataSerializer::MetadataVersionTagField("tag");
const FString FJsonUDataSerializer::MetadataVersionBuildField("build");
const FString FJsonUDataSerializer::MetadataVersionBuildIdField("build_id");
const FString FJsonUDataSerializer::
    MetadataVersionTimestampField("timestamp");

const FString FJsonUDataSerializer::
    MetadataGraphStyleMarginsField("margins");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsField("node_colors");
const FString FJsonUDataSerializer::
    MetadataGraphStyleTypeColorsField("type_colors");

const FString FJsonUDataSerializer::MetadataVersionField("version");
const FString FJsonUDataSerializer::MetadataGraphStyleField("style");

const FString FJsonUDataSerializer::
    MetadataGraphStyleMarginsInputPinMarginField("input_pin");
const FString FJsonUDataSerializer::
    MetadataGraphStyleMarginsOutputPinMarginField("output_pin");
const FString FJsonUDataSerializer::
    MetadataGraphStyleMarginsNonPinNodeBodyMarginField(
        "non_pin_node_body_pin");

const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsEventNodeTitleColorField("event_title");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsFunctionCallNodeTitleColorField(
        "function_call_title");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsPureFunctionCallNodeTitleColorField(
        "pure_function_call_title");

const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsDefaultPinTypeColorField("default");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsExecutionPinTypeColorField("execution");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsBooleanPinTypeColorField("bool");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsBytePinTypeColorField("byte");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsClassPinTypeColorField("class");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsIntPinTypeColorField("int");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsInt64PinTypeColorField("int64");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsFloatPinTypeColorField("float");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsNamePinTypeColorField("name");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsDelegatePinTypeColorField("delegate");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsObjectPinTypeColorField("object");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsSoftObjectPinTypeColorField("soft_object");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsSoftClassPinTypeColorField("soft_class");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsInterfacePinTypeColorField("interface");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsStringPinTypeColorField("string");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsTextPinTypeColorField("text");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsStructPinTypeColorField("struct");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsWildcardPinTypeColorField("wildcard");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsVectorPinTypeColorField("vector");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsRotatorPinTypeColorField("rotator");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsTransformPinTypeColorField("transform");
const FString FJsonUDataSerializer::
    MetadataGraphStyleNodeColorsIndexPinTypeColorField("index");

const FString FJsonUDataSerializer::MarginTopField("top");
const FString FJsonUDataSerializer::MarginRightField("right");
const FString FJsonUDataSerializer::MarginBottomField("bottom");
const FString FJsonUDataSerializer::MarginLeftField("left");

const FString FJsonUDataSerializer::ColorRField("r");
const FString FJsonUDataSerializer::ColorGField("g");
const FString FJsonUDataSerializer::ColorBField("b");
const FString FJsonUDataSerializer::ColorAField("a");

FJsonUDataSerializer::FJsonUDataSerializer()
{
    // replication modes (EReplication)
    ReplicationModes.Add(EReplication::None, ReplicationModeNone);
    ReplicationModes.Add(EReplication::Replicate, ReplicationModeReplicate);
    ReplicationModes.Add(EReplication::RepNotify, ReplicationModeRepNotify);

    // replication conditions (ELifetimeCondition)
    ReplicationConditions.Add(COND_None, ReplicationConditionNone);
    ReplicationConditions.Add(COND_InitialOnly,
        ReplicationConditionInitialOnly);
    ReplicationConditions.Add(COND_OwnerOnly,
        ReplicationConditionOwnerOnly);
    ReplicationConditions.Add(COND_SkipOwner,
        ReplicationConditionSkipOwner);
    ReplicationConditions.Add(COND_SimulatedOnly,
        ReplicationConditionSimulatedOnly);
    ReplicationConditions.Add(COND_AutonomousOnly,
        ReplicationConditionAutonomousOnly);
    ReplicationConditions.Add(COND_SimulatedOrPhysics,
        ReplicationConditionSimulatedOrPhysics);
    ReplicationConditions.Add(COND_InitialOrOwner,
        ReplicationConditionInitialOrOwner);
    ReplicationConditions.Add(COND_Custom,
        ReplicationConditionCustom);
    ReplicationConditions.Add(COND_ReplayOrOwner,
        ReplicationConditionReplayOrOwner);
    ReplicationConditions.Add(COND_ReplayOnly,
        ReplicationConditionReplayOnly);
    ReplicationConditions.Add(COND_SimulatedOnlyNoReplay,
        ReplicationConditionSimulatedOnlyNoReplay);
    ReplicationConditions.Add(COND_SimulatedOrPhysicsNoReplay,
        ReplicationConditionSimulatedOrPhysicsNoReplay);
    ReplicationConditions.Add(COND_SkipReplay,
        ReplicationConditionSkipReplay);
    ReplicationConditions.Add(COND_Never,
        ReplicationConditionNever);

    // multicast delegate flags (EMulticastDelegateFlags)
    MulticastDelegateFlags.Add(UDMC_BlueprintAssignable,
        BlueprintAssignableMulticastDelegateFlag);
    MulticastDelegateFlags.Add(UDMC_BlueprintCallable,
        BlueprintCallableMulticastDelegateFlag);
    MulticastDelegateFlags.Add(UDMC_BlueprintAuthorityOnly,
        BlueprintAuthorityOnlyMulticastDelegateFlag);
}

FString FJsonUDataSerializer::Serialize(const FRoot& Root)
{
    TSharedRef<FJsonObject> RootJson = MakeShared<FJsonObject>();

    // modules
    TArray<TSharedPtr<FJsonValue>> Modules;
    for (auto Module : Root.GetModulesView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeModule(*Module));

        Modules.Add(Value);
    }
    RootJson->SetArrayField(RootModulesField, Modules);

    RootJson->SetObjectField(RootMetadataField,
        SerializeMetadata(Root.GetMetadata()));

    FString SerializedRoot;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriter =
        TJsonWriterFactory<TCHAR>::Create(&SerializedRoot);
    FJsonSerializer::Serialize(RootJson, JsonWriter);

    return MoveTemp(SerializedRoot);  // move just in case
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeModule(
    const FModule& Module) const
{
    TSharedPtr<FJsonObject> ModuleJson = MakeShared<FJsonObject>();

    ModuleJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Module.GetIdentifier()));

    TArray<TSharedPtr<FJsonValue>> Classes;
    for (auto Class : Module.GetClassesView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeClass(*Class));

        Classes.Add(Value);
    }
    ModuleJson->SetArrayField(ModuleClassesField, Classes);

    TArray<TSharedPtr<FJsonValue>> Structs;
    for (auto Struct : Module.GetStructsView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeStruct(*Struct));

        Structs.Add(Value);
    }
    ModuleJson->SetArrayField(ModuleStructsField, Structs);

    TArray<TSharedPtr<FJsonValue>> Enums;
    for (auto Enum : Module.GetEnumsView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeEnum(*Enum));

        Enums.Add(Value);
    }
    ModuleJson->SetArrayField(ModuleEnumsField, Enums);

    return ModuleJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeClass(
    const FClass& Class) const
{
    TSharedPtr<FJsonObject> ClassJson = MakeShared<FJsonObject>();

    ClassJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Class.GetIdentifier()));
    ClassJson->SetBoolField(ClassIsAbstractField, Class.IsAbstract());
    ClassJson->SetBoolField(ClassIsInterfaceField, Class.IsInterface());
    ClassJson->SetBoolField(ClassIsBlueprintTypeField,
        Class.IsBlueprintType());
    ClassJson->SetBoolField(ClassIsBlueprintableField,
        Class.IsBlueprintable());

    ClassJson->SetStringField(ClassTypeField,
        SerializeClassType(Class.GetClassType()));

    ClassJson->SetObjectField(ClassSuperClassField,
        SerializeBasicIdentifierInfo(Class.GetSuperClass()));
    ClassJson->SetStringField(ClassNamespaceField,
        Class.GetNamespace());
    ClassJson->SetStringField(ClassConfigNameField,
        Class.GetConfigName());

    TArray<TSharedPtr<FJsonValue>> Interfaces;
    for (auto Interface : Class.GetInterfaces())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(
                SerializeBasicIdentifierInfo(Interface));

        Interfaces.Add(Value);
    }
    ClassJson->SetArrayField(ClassInterfacesField, Interfaces);

    TArray<TSharedPtr<FJsonValue>> Functions;
    for (auto Function : Class.GetFunctionsView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeFunction(*Function));

        Functions.Add(Value);
    }
    ClassJson->SetArrayField(ClassFunctionsField, Functions);

    TArray<TSharedPtr<FJsonValue>> Properties;
    for (auto Property : Class.GetPropertiesView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeProperty(*Property));

        Properties.Add(Value);
    }
    ClassJson->SetArrayField(ClassPropertiesField, Properties);

    return ClassJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeStruct(
    const FStruct& Struct) const
{
    TSharedPtr<FJsonObject> StructJson = MakeShared<FJsonObject>();

    StructJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Struct.GetIdentifier()));
    StructJson->SetBoolField(StructIsBlueprintTypeField,
        Struct.IsBlueprintType());

    TArray<TSharedPtr<FJsonValue>> Properties;
    for (auto Property : Struct.GetPropertiesView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeProperty(*Property));

        Properties.Add(Value);
    }
    StructJson->SetArrayField(StructPropertiesField, Properties);

    return StructJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeEnum(
    const FEnum& Enum) const
{
    TSharedPtr<FJsonObject> EnumJson = MakeShared<FJsonObject>();

    EnumJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Enum.GetIdentifier()));
    EnumJson->SetBoolField(EnumIsBlueprintTypeField,
        Enum.IsBlueprintType());

    TArray<TSharedPtr<FJsonValue>> Elements;
    for (auto Element : Enum.GetElementsView())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeEnumElement(*Element));

        Elements.Add(Value);
    }
    EnumJson->SetArrayField(EnumElementsField, Elements);

    return EnumJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeEnumElement(
    const FEnumElement& EnumElement) const
{
    TSharedPtr<FJsonObject> EnumElementJson =
        SerializeBasicIdentifierInfo(EnumElement);

    EnumElementJson->SetBoolField(EnumElementIsHiddenField,
        EnumElement.IsHidden());
    EnumElementJson->SetStringField(EnumElementDescriptionField,
        EnumElement.GetDescription());

    return EnumElementJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeProperty(
    const FUdProperty& Property) const
{
    TSharedPtr<FJsonObject> PropertyJson = MakeShared<FJsonObject>();

    PropertyJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Property.GetIdentifier()));
    PropertyJson->SetObjectField(TypeField,
        SerializeType(*Property.GetType()));
    PropertyJson->SetBoolField(PropertyIsInstanceEditableField,
        Property.IsInstanceEditable());
    PropertyJson->SetBoolField(PropertyIsArchetypeEditableField,
        Property.IsArchetypeEditable());
    PropertyJson->SetBoolField(PropertyIsBlueprintReadOnlyField,
        Property.IsBlueprintReadOnly());
    PropertyJson->SetBoolField(PropertyIsExposeOnSpawnField,
        Property.IsExposeOnSpawn());
    PropertyJson->SetBoolField(PropertyIsPrivateField,
        Property.IsPrivate());
    PropertyJson->SetBoolField(PropertyIsExposeToCinematicField,
        Property.IsExposeToCinematic());
    PropertyJson->SetBoolField(PropertyIsConfigVariableField,
        Property.IsConfigVariable());
    PropertyJson->SetBoolField(PropertyIsBlueprintVisibleField,
        Property.IsBlueprintVisible());
    PropertyJson->SetStringField(PropertyClampMinField,
        Property.GetClampMin());
    PropertyJson->SetStringField(PropertyClampMaxField,
        Property.GetClampMax());
    PropertyJson->SetStringField(PropertyDefaultValueField,
        Property.GetDefaultValueTransliteration());

    // replication
    const FString* ReplicationModeIt = ReplicationModes
        .Find(Property.GetReplication());
    PropertyJson->SetStringField(PropertyReplicationModeField,
        ReplicationModeIt ? *ReplicationModeIt : UndefinedValue);

    // replication condition
    const FString* ReplicationConditionIt = ReplicationConditions
        .Find(Property.GetReplicationCondition());
    PropertyJson->SetStringField(PropertyReplicationConditionField,
        ReplicationConditionIt
                    ? *ReplicationConditionIt
                    : UndefinedValue);

    TArray<TSharedPtr<FJsonValue>> MulticastDelegateFlagsSet;
    TArray<EMulticastDelegateFlags> MulticastDelegateFlagsAvailable;
    MulticastDelegateFlags.GetKeys(MulticastDelegateFlagsAvailable);

    for (auto Flag : MulticastDelegateFlagsAvailable)
    {
        if (Property.GetMulticastDelegateFlags() & Flag)
        {
            TSharedPtr<FJsonValue> Value
                = MakeShared<FJsonValueString>(
                    *MulticastDelegateFlags.Find(Flag));

            MulticastDelegateFlagsSet.Add(Value);
        }
    }
    PropertyJson->SetArrayField(PropertyMulticastDelegateFlagsField,
        MulticastDelegateFlagsSet);

    return PropertyJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeFunction(
    const FFunction& Function)
{
    TMap<EAccessModifier, FString> AccessModifiers;
    TMap<ERpcMode, FString> RpcModes;

    // access modifiers (EAccessModifier)
    AccessModifiers.Add(EAccessModifier::Private,
        PrivateAccessModifier);
    AccessModifiers.Add(EAccessModifier::Protected,
        ProtectedAccessModifier);
    AccessModifiers.Add(EAccessModifier::Public,
        PublicAccessModifier);

    // rpc modes (ERpcMode)
    RpcModes.Add(ERpcMode::Client, ClientRpcMode);
    RpcModes.Add(ERpcMode::Server, ServerRpcMode);
    RpcModes.Add(ERpcMode::NetMulticast, NetMulticastRpcMode);
    RpcModes.Add(ERpcMode::NotReplicated, NotReplicatedRpcMode);

    TSharedPtr<FJsonObject> FunctionJson = MakeShared<FJsonObject>();

    FunctionJson->SetObjectField(IdentifierField,
        SerializeIdentifier(Function.GetIdentifier()));

    // keywords
    TArray<TSharedPtr<FJsonValue>> Keywords;
    for (const auto& Keyword : Function.GetKeywords())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueString>(Keyword);

        Keywords.Add(Value);
    }
    FunctionJson->SetArrayField(FunctionKeywordsField, Keywords);

    FunctionJson->SetStringField(FunctionCompactNodeTitleField,
        Function.GetCompactNodeTitle());
    FunctionJson->SetStringField(FunctionRawCommentField,
        Function.GetRawComment());
    FunctionJson->SetStringField(FunctionNodeCommentField,
        Function.GetNodeComment());

    // access modifier
    const FString* AccessModifierIt = AccessModifiers
        .Find(Function.GetAccessModifier());
    FunctionJson->SetStringField(FunctionAccessModifierField,
        AccessModifierIt ? *AccessModifierIt : UndefinedValue);

    // RPC mode
    const FString* RpcModeIt = RpcModes.Find(Function.GetRpcMode());
    FunctionJson->SetStringField(FunctionRpcModeField,
        RpcModeIt ? *RpcModeIt : UndefinedValue);

    FunctionJson->SetBoolField(FunctionIsPureField, Function.IsPure());
    FunctionJson->SetBoolField(FunctionIsCallInEditorField,
        Function.IsCallInEditor());
    FunctionJson->SetBoolField(FunctionIsOverrideField,
        Function.IsOverride());
    FunctionJson->SetBoolField(FunctionIsEventField, Function.IsEvent());
    FunctionJson->SetBoolField(FunctionIsReliableField,
        Function.IsReliable());
    FunctionJson->SetBoolField(FunctionIsStaticField,
        Function.IsStatic());
    FunctionJson->SetBoolField(FunctionIsConstField, Function.IsConst());
    FunctionJson->SetBoolField(FunctionIsBlueprintCallableField,
        Function.IsBlueprintCallable());
    FunctionJson->SetBoolField(FunctionIsOverridableField,
        Function.IsOverridable());
    FunctionJson->SetBoolField(FunctionIsBlueprintProtectedField,
        Function.IsBlueprintProtected());
    FunctionJson->SetBoolField(FunctionIsLatent,
        Function.IsLatent());
    FunctionJson->SetBoolField(FunctionIsCosmetic,
        Function.IsCosmetic());
    FunctionJson->SetBoolField(FunctionIsAuthorityOnly,
        Function.IsAuthorityOnly());

    // parameters
    TArray<TSharedPtr<FJsonValue>> Parameters;
    for (auto Parameter : Function.GetParameters())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(SerializeParameter(*Parameter));

        Parameters.Add(Value);
    }
    FunctionJson->SetArrayField(FunctionParametersField, Parameters);

    // return parameters
    TArray<TSharedPtr<FJsonValue>> ReturnParameters;
    for (auto ReturnParameter : Function.GetReturnParameters())
    {
        TSharedPtr<FJsonValue> Value
            = MakeShared<FJsonValueObject>(
                SerializeParameter(*ReturnParameter));

        ReturnParameters.Add(Value);
    }
    FunctionJson->SetArrayField(FunctionReturnParametersField,
        ReturnParameters);

    return FunctionJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeParameter(
    const FParameter& Parameter)
{
    TSharedPtr<FJsonObject> ParameterJson = MakeShared<FJsonObject>();

    ParameterJson->SetObjectField(ParameterNameField,
        SerializeName(Parameter.GetName()));
    ParameterJson->SetObjectField(TypeField,
        SerializeType(*Parameter.GetType()));
    ParameterJson->SetStringField(ParameterDefaultValueField,
        Parameter.GetDefaultValueTransliteration());

    return ParameterJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeType(
    const FType& Type)
{
    FTypeSerializer TypeSerializer;
    Type.Accept(TypeSerializer);

    return TypeSerializer.GetSerializationResult();
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeName(
    const FIdentifierName& Name)
{
    TSharedPtr<FJsonObject> NameJson = MakeShared<FJsonObject>();

    NameJson->SetStringField(NameNameField, Name.GetName());
    NameJson->SetStringField(NameDisplayNameField,
        Name.GetDisplayName());

    return NameJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeBasicIdentifierInfo(
    const FBasicIdentifierInfo& BasicIdentifierInfo)
{
    TSharedPtr<FJsonObject> BasicIdentifierInfoJson = MakeShared<FJsonObject>();

    BasicIdentifierInfoJson->SetObjectField(BasicIdentifierInfoNameField,
        SerializeName(BasicIdentifierInfo.GetName()));
    BasicIdentifierInfoJson->SetStringField(BasicIdentifierInfoFullPathField,
        BasicIdentifierInfo.GetFullPath());
    BasicIdentifierInfoJson->SetBoolField(
        BasicIdentifierInfoIsNativeDefinedField,
        BasicIdentifierInfo.IsNativeDefined());

    return BasicIdentifierInfoJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeIdentifier(
    const FIdentifier& Identifier)
{
    TSharedPtr<FJsonObject> IdentifierJson =
        SerializeBasicIdentifierInfo(Identifier);

    IdentifierJson->SetStringField(IdentifierTooltipField,
        Identifier.GetTooltip());
    IdentifierJson->SetStringField(IdentifierShortTooltipField,
        Identifier.GetShortTooltip());
    IdentifierJson->SetStringField(IdentifierCategoryField,
        Identifier.GetCategory());
    IdentifierJson->SetStringField(IdentifierDeprecationMessageField,
        Identifier.GetDeprecationMessage());
    IdentifierJson->SetBoolField(IdentifierIsDeprecatedField,
        Identifier.IsDeprecated());

    return IdentifierJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeMetadata(
    const FMetadata& Metadata) const
{
    TSharedPtr<FJsonObject> MetadataJson = MakeShared<FJsonObject>();

    TSharedPtr<FJsonObject> VersionJson = MakeShared<FJsonObject>();
    VersionJson->SetNumberField(MetadataVersionMajorField,
        Metadata.GetVersion().Major);
    VersionJson->SetNumberField(MetadataVersionMinorField,
        Metadata.GetVersion().Minor);
    VersionJson->SetNumberField(MetadataVersionIndexField,
        Metadata.GetVersion().Index);
    VersionJson->SetStringField(MetadataVersionTagField,
        Metadata.GetVersion().Tag);
    VersionJson->SetNumberField(MetadataVersionBuildField,
        Metadata.GetVersion().Build);
    VersionJson->SetStringField(MetadataVersionBuildIdField,
        Metadata.GetVersion().BuildId);
    VersionJson->SetNumberField(MetadataVersionTimestampField,
        Metadata.GetVersion().Timestamp);

    MetadataJson->SetObjectField(MetadataVersionField, VersionJson);

    // style

    const FGraphStyle& Style = Metadata.GetGraphStyle();
    TSharedPtr<FJsonObject> GraphStyleJson = MakeShared<FJsonObject>();

    TSharedPtr<FJsonObject> MarginsJson = MakeShared<FJsonObject>();
    MarginsJson->SetObjectField(MetadataGraphStyleMarginsInputPinMarginField,
        SerializeMargin(Style.GetInputPinMargin()));
    MarginsJson->SetObjectField(MetadataGraphStyleMarginsOutputPinMarginField,
        SerializeMargin(Style.GetOutputPinMargin()));
    MarginsJson->SetObjectField(
        MetadataGraphStyleMarginsNonPinNodeBodyMarginField,
        SerializeMargin(Style.GetNonPinNodeBodyMargin()));
    GraphStyleJson->SetObjectField(MetadataGraphStyleMarginsField, MarginsJson);

    TSharedPtr<FJsonObject> NodeColorsJson = MakeShared<FJsonObject>();
    NodeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsEventNodeTitleColorField,
        SerializeLinearColor(Style.GetEventNodeTitleColor()));
    NodeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsFunctionCallNodeTitleColorField,
        SerializeLinearColor(Style.GetFunctionCallNodeTitleColor()));
    NodeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsPureFunctionCallNodeTitleColorField,
        SerializeLinearColor(
            Style.GetPureFunctionCallNodeTitleColor()));
    GraphStyleJson->SetObjectField(MetadataGraphStyleNodeColorsField,
        NodeColorsJson);

    TSharedPtr<FJsonObject> TypeColorsJson = MakeShared<FJsonObject>();
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsDefaultPinTypeColorField,
        SerializeLinearColor(Style.GetDefaultPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsExecutionPinTypeColorField,
        SerializeLinearColor(Style.GetExecutionPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsBooleanPinTypeColorField,
        SerializeLinearColor(Style.GetBooleanPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsBytePinTypeColorField,
        SerializeLinearColor(Style.GetBytePinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsClassPinTypeColorField,
        SerializeLinearColor(Style.GetClassPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsIntPinTypeColorField,
        SerializeLinearColor(Style.GetIntPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsInt64PinTypeColorField,
        SerializeLinearColor(Style.GetInt64PinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsFloatPinTypeColorField,
        SerializeLinearColor(Style.GetFloatPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsNamePinTypeColorField,
        SerializeLinearColor(Style.GetNamePinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsDelegatePinTypeColorField,
        SerializeLinearColor(Style.GetDelegatePinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsObjectPinTypeColorField,
        SerializeLinearColor(Style.GetObjectPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsSoftObjectPinTypeColorField,
        SerializeLinearColor(Style.GetSoftObjectPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsSoftClassPinTypeColorField,
        SerializeLinearColor(Style.GetSoftClassPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsInterfacePinTypeColorField,
        SerializeLinearColor(Style.GetInterfacePinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsStringPinTypeColorField,
        SerializeLinearColor(Style.GetStringPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsTextPinTypeColorField,
        SerializeLinearColor(Style.GetTextPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsStructPinTypeColorField,
        SerializeLinearColor(Style.GetStructPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsWildcardPinTypeColorField,
        SerializeLinearColor(Style.GetWildcardPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsVectorPinTypeColorField,
        SerializeLinearColor(Style.GetVectorPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsRotatorPinTypeColorField,
        SerializeLinearColor(Style.GetRotatorPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsTransformPinTypeColorField,
        SerializeLinearColor(Style.GetTransformPinTypeColor()));
    TypeColorsJson->SetObjectField(
        MetadataGraphStyleNodeColorsIndexPinTypeColorField,
        SerializeLinearColor(Style.GetIndexPinTypeColor()));
    GraphStyleJson->SetObjectField(MetadataGraphStyleTypeColorsField,
        TypeColorsJson);

    MetadataJson->SetObjectField(MetadataGraphStyleField, GraphStyleJson);

    return MetadataJson;
}

FString FJsonUDataSerializer::SerializeClassType(EClassType ClassType)
{
    // class types (EClassType)
    TMap<EClassType, FString> ClassTypes;

    ClassTypes.Add(EClassType::Class, ClassTypeClass);
    ClassTypes.Add(EClassType::Component, ClassTypeComponent);
    ClassTypes.Add(EClassType::Widget, ClassTypeWidget);
    ClassTypes.Add(EClassType::FunctionLibrary,
        ClassTypeFunctionLibrary);
    ClassTypes.Add(EClassType::Actor, ClassTypeActor);

    const FString* ClassTypeIt = ClassTypes.Find(ClassType);

    return ClassTypeIt ? *ClassTypeIt : UndefinedValue;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeMargin(
    const FMargin& Margin) const
{
    TSharedPtr<FJsonObject> MarginJson = MakeShared<FJsonObject>();

    MarginJson->SetNumberField(MarginTopField, Margin.Top);
    MarginJson->SetNumberField(MarginRightField, Margin.Right);
    MarginJson->SetNumberField(MarginBottomField, Margin.Bottom);
    MarginJson->SetNumberField(MarginLeftField, Margin.Left);

    return MarginJson;
}

TSharedPtr<FJsonObject> FJsonUDataSerializer::SerializeLinearColor(
    const FLinearColor& Color) const
{
    TSharedPtr<FJsonObject> ColorJson = MakeShared<FJsonObject>();

    ColorJson->SetNumberField(ColorRField, Color.R);
    ColorJson->SetNumberField(ColorGField, Color.G);
    ColorJson->SetNumberField(ColorBField, Color.B);
    ColorJson->SetNumberField(ColorAField, Color.A);

    return ColorJson;
}
