/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "ApiModelExtractor.h"

#include "JsonObjectConverter.h"
#include "K2Node_Composite.h"
#include "K2Node_Event.h"
#include "K2Node_FunctionEntry.h"
#include "ApiModel/Helper.h"
#include "ApiModel/Types/DelegateType.h"
#include "ApiModel/UdProperty.h"
#include "Misc/ScopeExit.h"
#include "UObject/MetaData.h"

#include "ApiModel/Types/ArrayType.h"
#include "ApiModel/Types/EnumType.h"
#include "ApiModel/Types/InterfaceType.h"
#include "ApiModel/Types/MapType.h"
#include "ApiModel/Types/ObjectType.h"
#include "ApiModel/Types/PrimitiveType.h"
#include "ApiModel/Types/SetType.h"
#include "ApiModel/Types/StructType.h"

template<typename T>
FIdentifierName ExtractName(T* Object)
{
    FIdentifierName Name;

    Name.SetName(Object->GetName());
    Name.SetDisplayName(Object->GetDisplayNameText().ToString());

    return Name;
}

template<typename T>
bool IsNative(const T* Object)
{
    return Object->GetPathName().StartsWith("/Script/");
}

template<typename T>
FBasicIdentifierInfo ExtractBasicIdentifierInfo(T* Object)
{
    FBasicIdentifierInfo BasicIdentifierInfo;

    BasicIdentifierInfo.SetName(ExtractName(Object));
    BasicIdentifierInfo.SetIsNativeDefined(IsNative(Object));
    BasicIdentifierInfo.SetFullPath(Object->GetPathName());

    return BasicIdentifierInfo;
}

// todo(artsiom.drapun): add info about UFUNCTIONs and UPROPERTYs
// there's a little mess with deprecation in UE. AFAIK there are 3 ways to
// deprecate (in C++):
// 1. UE_DEPRECATE - does not affect BP, just being replaced by
// [[deprecated]]
// 2. UCLASS's flag `Deprecated` - UE wants to rename the class to
// UDEPRECATED_<name> - affects UClass's EClassFlags::CLASS_Deprecated
// can't deprecate a native interface
// Speaking about BP:
// 1. When we check `Deprecated` checkbox in any UBlueprint derived assets,
// the BP generated class gets EClassFlags::CLASS_Deprecated
// there's no way to deprecate enum/struct
// function: Metadata: FBlueprintMetadata::MD_FunctionCategory
// property: Metadata: DeprecatedProperty -> CPF_Deprecated
// meta: DeprecationMessage - works for class/function/property

template<typename T>
FIdentifier ExtractIdentifier(const T* Object)
{
    FIdentifier Identifier;
    FBasicIdentifierInfo BasicIdentifierInfo =
        ExtractBasicIdentifierInfo(Object);

    Identifier.SetName(BasicIdentifierInfo.GetName());

    static const FName TooltipMetadata(TEXT("Tooltip"));
    static const FName ShortTooltipMetadata(TEXT("ShortTooltip"));

    FString Tooltip = Object->GetMetaData(TooltipMetadata);
    FString ShortTooltip = Object->GetMetaData(ShortTooltipMetadata);

    Identifier.SetTooltip(MoveTemp(Tooltip));
    Identifier.SetShortTooltip(MoveTemp(ShortTooltip));

    Identifier.SetCategory(
        Object->GetMetaData(FBlueprintMetadata::MD_FunctionCategory));
    Identifier.SetDeprecationMessage(
        Object->GetMetaData(FBlueprintMetadata::MD_DeprecationMessage));
    Identifier.SetIsNativeDefined(BasicIdentifierInfo.IsNativeDefined());
    Identifier.SetFullPath(BasicIdentifierInfo.GetFullPath());

    return Identifier;
}

FString MakeDelegateDisplayName(const FString& NativeName, bool bIsNative)
{
    if (!bIsNative)
    {
        return "";
    }

    static const TCHAR Prefix = 'F';

    FString DisplayName = FName::NameToDisplayString(NativeName, false);
    if (!DisplayName.IsEmpty() && DisplayName[0] == Prefix)
    {
        return DisplayName.Len() > 1 ? DisplayName.Mid(1) : "";
    }
    else
    {
        return DisplayName;
    }
}

const FString FApiModelExtractor::FTypeExtractor::ArrayName("array");
const FString FApiModelExtractor::FTypeExtractor::SetName("set");
const FString FApiModelExtractor::FTypeExtractor::MapName("map");
const FString FApiModelExtractor::FTypeExtractor::ByteName("byte");
const FString FApiModelExtractor::FTypeExtractor::IntName("int");
const FString FApiModelExtractor::FTypeExtractor::Int64Name("int64");
const FString FApiModelExtractor::FTypeExtractor::FloatName("float");
const FString FApiModelExtractor::FTypeExtractor::DoubleName("double");
const FString FApiModelExtractor::FTypeExtractor::BoolName("bool");
const FString FApiModelExtractor::FTypeExtractor::NameName("name");
const FString FApiModelExtractor::FTypeExtractor::StringName("string");
const FString FApiModelExtractor::FTypeExtractor::TextName("text");
const FString FApiModelExtractor::FTypeExtractor::UndefinedName("");

FApiModelExtractor::FTypeExtractor::FTypeExtractor()
{
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Byte, ByteName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Int, IntName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Int64, Int64Name);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Float, FloatName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Double, DoubleName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Bool, BoolName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Name, NameName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::String, StringName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Text, TextName);
    PrimitiveTypesToString.Add(
        PrimitiveType::EPrimitiveType::Undefined, UndefinedName);
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractType(
    const FProperty* Property) const
{
    TUniquePtr<FType> Type;

    // todo: no one likes these casts but what else could I do?
    // if you know how to improve this, contact me at
    // artsiom.drapun@medelfor.com
    auto Byte = CastField<FByteProperty>(Property);

    if (auto Delegate = CastField<FDelegateProperty>(Property))
    {
        Type = ExtractDelegateType(Delegate);
    }
    else if (auto MulticastDelegate =
        CastField<FMulticastDelegateProperty>(Property))
    {
        Type = ExtractMulticastDelegateType(MulticastDelegate);
    }
    else if (auto Object = CastField<FObjectPropertyBase>(Property))
    {
        Type = ExtractObjectType(Object);
    }
    else if (auto Array = CastField<FArrayProperty>(Property))
    {
        Type = ExtractArrayType(Array);
    }
    else if (auto Set = CastField<FSetProperty>(Property))
    {
        Type = ExtractSetType(Set);
    }
    else if (auto Map = CastField<FMapProperty>(Property))
    {
        Type = ExtractMapType(Map);
    }
    else if (auto Struct = CastField<FStructProperty>(Property))
    {
        Type = ExtractStructType(Struct);
    }
    else if (auto Interface = CastField<FInterfaceProperty>(Property))
    {
        Type = ExtractInterfaceType(Interface);
    }
    // enum prop. declared as  enum
    else if (auto Enum = CastField<FEnumProperty>(Property))
    {
        Type = ExtractEnumType(Enum);
    }
    // enum declared as TEnumAsByte<>
    else if (Byte && Byte->Enum)
    {
        Type = ExtractEnumTypeAsByte(Byte);
    }
    // primitives and undefined properties inherited directly
    // from FProperty
    else
    {
        Type = ExtractPrimitiveType(Property);
    }

    Type->SetIsReference(Property->HasAnyPropertyFlags(CPF_ReferenceParm));
    Type->SetIsConst(Property->HasAnyPropertyFlags(CPF_ConstParm));

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractDelegateType(
    const FDelegateProperty* Property) const
{
    TUniquePtr<FDelegateType> Type = MakeUnique<FDelegateType>();
    TUniquePtr<FApiModelExtractor> ApiModelExtractor =
        MakeUnique<FApiModelExtractor>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;

    // the nativeness of a delegate type is determined via the nativeness of its
    // signature function
    Info.SetIsNativeDefined(IsNative(Property->SignatureFunction));

    // name should be the class of delegate (if native) and undefined if not
    Name.SetName(Property->GetCPPType
        (nullptr, EPropertyExportCPPFlags::CPPF_BlueprintCppBackend));
    Name.SetDisplayName(
        MakeDelegateDisplayName(Name.GetName(), Info.IsNativeDefined()));
    Info.SetName(MoveTemp(Name));

    Type->SetIdentifierInfo(MoveTemp(Info));
    Type->SetSignature(ApiModelExtractor->
        ExtractFunction(Property->SignatureFunction));
    Type->SetIsMulticast(false);

    return Type;
}

TUniquePtr<FType>
    FApiModelExtractor::FTypeExtractor::ExtractMulticastDelegateType(
    const FMulticastDelegateProperty* Property) const
{
    TUniquePtr<FDelegateType> Type = MakeUnique<FDelegateType>();
    TUniquePtr<FApiModelExtractor> ApiModelExtractor =
        MakeUnique<FApiModelExtractor>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;

    // the nativeness of a delegate type is determined via the nativeness of its
    // signature function
    Info.SetIsNativeDefined(IsNative(Property->SignatureFunction));

    // name should be the class of delegate (if native) and undefined if not
    Name.SetName(Property->GetCPPType
        (nullptr, EPropertyExportCPPFlags::CPPF_BlueprintCppBackend));
    Name.SetDisplayName(
        MakeDelegateDisplayName(Name.GetName(), Info.IsNativeDefined()));
    Info.SetName(MoveTemp(Name));

    Type->SetIdentifierInfo(MoveTemp(Info));
    Type->SetSignature(ApiModelExtractor->
        ExtractFunction(Property->SignatureFunction));
    Type->SetIsMulticast(true);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractObjectType(
    const FObjectPropertyBase* Property) const
{
    TUniquePtr<FObjectType> Type = MakeUnique<FObjectType>();

    FString Wrapper;
    FBasicIdentifierInfo Info;

    // determine wrapper around object, like soft object, soft class, etc
    EObjectReference Reference = EObjectReference::Undefined;

    const UClass* ContainerClass = Property->PropertyClass;

    // todo: no one likes these casts but what else could I do?
    // if you know how to improve this, contact me at
    // artsiom.drapun@medelfor.com
    if (auto SoftClassProperty = CastField<FSoftClassProperty>(Property))
    {
        Reference = EObjectReference::SoftClass;
        ContainerClass = SoftClassProperty->MetaClass;
    }
    else if (auto ClassProperty = CastField<FClassProperty>(Property))
    {
        Reference = EObjectReference::Class;
        ContainerClass = ClassProperty->MetaClass;
    }
    else if (Property->IsA<FSoftObjectProperty>())
    {
        Reference = EObjectReference::SoftObject;
    }
    else if (Property->IsA<FLazyObjectProperty>())
    {
        Reference = EObjectReference::LazyObject;
    }
    else if (Property->IsA<FWeakObjectProperty>())
    {
        Reference = EObjectReference::WeakObject;
    }
    else if (Property->IsA<FObjectProperty>())
    {
        Reference = EObjectReference::Object;
    }

    Type->SetClassType(FClass::DetermineClassType(ContainerClass));
    Type->SetObjectReference(Reference);

    // the name that is taken, is of the contained class itself
    Info.SetName(ExtractName(ContainerClass));
    Info.SetFullPath(ContainerClass->GetPathName());
    Info.SetIsNativeDefined(IsNative(ContainerClass));
    Type->SetIdentifierInfo(Info);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractPrimitiveType(
    const FProperty* Property) const
{
    TUniquePtr<FPrimitiveType> Type = MakeUnique<FPrimitiveType>();

    // todo: no one likes these casts but what else could I do?
    // if you know how to improve this, contact me at
    // artsiom.drapun@medelfor.com
    if (Property->IsA<FByteProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Byte);
    }
    else if (Property->IsA<FInt64Property>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Int64);
    }
    else if (Property->IsA<FUInt64Property>() ||
             Property->IsA<FUInt32Property>() ||
             Property->IsA<FUInt16Property>() ||
             Property->IsA<FInt16Property>() ||
             Property->IsA<FInt8Property>() ||
             Property->IsA<FIntProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Int);
    }
    else if (Property->IsA<FFloatProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Float);
    }
    else if (Property->IsA<FDoubleProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Double);
    }
    else if (Property->IsA<FBoolProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Bool);
    }
    else if (Property->IsA<FNameProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Name);
    }
    else if (Property->IsA<FTextProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::Text);
    }
    else if (Property->IsA<FStrProperty>())
    {
        Type->SetPrimitiveType(PrimitiveType::EPrimitiveType::String);
    }
    // if nothing - then primitive type is
    // PrimitiveType::EPrimitiveType::Undefined which is set by default

    FBasicIdentifierInfo Info;
    FIdentifierName Name;
    // be careful when you add new enum types
    auto It =
        PrimitiveTypesToString.Find(Type->GetPrimitiveType());

    if (It)
    {
        Name.SetName(*It);
    }

    Info.SetName(MoveTemp(Name));
    Info.SetIsNativeDefined(true);

    Type->SetIdentifierInfo(Info);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractStructType(
    const FStructProperty* Property) const
{
    TUniquePtr<FStructType> Type = MakeUnique<FStructType>();

    FBasicIdentifierInfo Info;

    // the name that is taken, is of the contained struct itself
    Info.SetName(ExtractName(Property->Struct));
    Info.SetFullPath(Property->Struct->GetPathName());
    Info.SetIsNativeDefined(IsNative(Property->Struct));
    Type->SetIdentifierInfo(Info);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractInterfaceType(
    const FInterfaceProperty* Property) const
{
    TUniquePtr<FInterfaceType> Type = MakeUnique<FInterfaceType>();

    FBasicIdentifierInfo Info;

    // the name that is taken, is of the contained interface itself
    Info.SetName(ExtractName(Property->InterfaceClass));
    Info.SetFullPath(Property->InterfaceClass->GetPathName());
    Info.SetIsNativeDefined(IsNative(Property->InterfaceClass));
    Type->SetIdentifierInfo(Info);

    return Type;
}

// todo: consider merging of ExtractEnumType and ExtractEnumTypeAsByte
TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractEnumType(
    const FEnumProperty* Property) const
{
    TUniquePtr<FEnumType> Type = MakeUnique<FEnumType>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;

    Name.SetName(Property->GetEnum()->GetName());
    Name.SetDisplayName(FHelper::ExtractDisplayNameOfEnum(Property->GetEnum()));

    // the name that is taken, is of the contained enum itself
    Info.SetName(MoveTemp(Name));
    Info.SetFullPath(Property->GetEnum()->GetPathName());
    Info.SetIsNativeDefined(IsNative(Property->GetEnum()));
    Type->SetIdentifierInfo(Info);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractEnumTypeAsByte(
    const FByteProperty* Property) const
{
    TUniquePtr<FEnumType> Type = MakeUnique<FEnumType>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;

    Name.SetName(Property->Enum->GetName());
    Name.SetDisplayName(FHelper::ExtractDisplayNameOfEnum(Property->Enum));

    // the name that is taken, is of the contained enum itself
    Info.SetName(MoveTemp(Name));
    Info.SetFullPath(Property->Enum->GetPathName());
    Info.SetIsNativeDefined(IsNative(Property->Enum));
    Type->SetIdentifierInfo(Info);

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractArrayType(
    const FArrayProperty* Property) const
{
    TUniquePtr<FArrayType> Type = MakeUnique<FArrayType>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;
    Name.SetName(ArrayName);
    Info.SetName(MoveTemp(Name));
    Info.SetIsNativeDefined(true);
    Type->SetIdentifierInfo(Info);

    Type->SetElementType(ExtractType(Property->Inner));

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractSetType(
    const FSetProperty* Property) const
{
    TUniquePtr<FSetType> Type = MakeUnique<FSetType>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;
    Name.SetName(SetName);
    Info.SetName(MoveTemp(Name));
    Info.SetIsNativeDefined(true);
    Type->SetIdentifierInfo(Info);

    Type->SetElementType(ExtractType(Property->ElementProp));

    return Type;
}

TUniquePtr<FType> FApiModelExtractor::FTypeExtractor::ExtractMapType(
    const FMapProperty* Property) const
{
    TUniquePtr<FMapType> Type = MakeUnique<FMapType>();

    FBasicIdentifierInfo Info;
    FIdentifierName Name;
    Name.SetName(MapName);
    Info.SetName(MoveTemp(Name));
    Info.SetIsNativeDefined(true);
    Type->SetIdentifierInfo(Info);

    Type->SetKeyType(ExtractType(Property->KeyProp));
    Type->SetValueType(ExtractType(Property->ValueProp));

    return Type;
}

const FString FApiModelExtractor::KeywordsSeparator(",");
const FString FApiModelExtractor::FunctionCommentMetadata("Comment");
const FString FApiModelExtractor::
    FunctionBlueprintProtectedMetadata("BlueprintProtected");
const FString FApiModelExtractor::FunctionLatent("Latent");
const FString FApiModelExtractor::
    PropertyDeprecatedMetadata("DeprecatedProperty");
const FString FApiModelExtractor::PropertyClampMin("ClampMin");
const FString FApiModelExtractor::PropertyClampMax("ClampMax");
const FString FApiModelExtractor::PropertyUiMin("UiMin");
const FString FApiModelExtractor::PropertyUiMax("UiMax");
const FString FApiModelExtractor::PathComponentsSeparator("/");
const FString FApiModelExtractor::GamePath("Game");
const FString FApiModelExtractor::ScriptPath("Script");
const FString FApiModelExtractor::EnginePath("Engine");
const FString FApiModelExtractor::EnumBlueprintTypeMetadata("BlueprintType");
const FString FApiModelExtractor::
    StructBlueprintTypeMetadata("BlueprintType");
const FString FApiModelExtractor::EnumHiddenMetadata("Hidden");
const FString FApiModelExtractor::EnumMetadataSeparator(".");
const FString FApiModelExtractor::UberGraphFrame("UberGraphFrame");
const FString FApiModelExtractor::ObjectsSeparator(".");
const FString FApiModelExtractor::True("true");
const FString FApiModelExtractor::
    ClassBlueprintableMetadata("IsBlueprintBase");
const FString FApiModelExtractor::
    ClassBlueprintTypeMetadata("BlueprintType");
const FString FApiModelExtractor::DefaultCategory("Default");

FApiModelExtractor::FApiModelExtractor() { }

TUniquePtr<FClass> FApiModelExtractor::ExtractClass(const UClass* Class) const
{
    TUniquePtr<FClass> ExtractedClass = MakeUnique<FClass>();

    FIdentifier Identifier = ExtractIdentifier(Class);
    Identifier.SetIsDeprecated(Class->HasAnyClassFlags(CLASS_Deprecated));
    ExtractedClass->SetIdentifier(MoveTemp(Identifier));

    if (auto Blueprint = Cast<UBlueprint>(Class->ClassGeneratedBy))
    {
        ExtractedClass->SetNamespace(Blueprint->BlueprintNamespace);
    }

    ExtractedClass->SetIsAbstract(Class->HasAnyClassFlags(CLASS_Abstract));
    ExtractedClass->SetIsInterface(Class->HasAnyClassFlags(CLASS_Interface));
    ExtractedClass->SetSuperClass(
        ExtractBasicIdentifierInfo(Class->GetSuperClass()));
    ExtractedClass->SetConfigName(Class->ClassConfigName.ToString());

    for (const FImplementedInterface& Interface : Class->Interfaces)
    {
        ExtractedClass->AddInterface(
            ExtractBasicIdentifierInfo(Interface.Class));
    }

    TArray<TUniquePtr<FFunction>> Functions = ExtractFunctions(Class);
    for (TUniquePtr<FFunction>& Function : Functions)
    {
        ExtractedClass->AddFunction(MoveTemp(Function));
    }

    FPropertyExtractionResult Properties = ExtractProperties(Class);
    for (int i = 0; i < Properties.Extracted.Num(); ++i)
    {
        FString DefaultValue = ExtractDefaultValueAsStringFromProperty(
            Class, Properties.Original[i]);
        Properties.Extracted[i]->
            SetDefaultValueTransliteration(MoveTemp(DefaultValue));
        ExtractedClass->AddProperty(MoveTemp(Properties.Extracted[i]));
    }

    ExtractedClass->SetClassType(FClass::DetermineClassType(Class));
    ExtractedClass->SetIsBlueprintType(
        UEdGraphSchema_K2::IsAllowableBlueprintVariableType(Class));

    // if a native class is marked as Blueprintable it's reflected in metadata
    // Just a heuristic:
    // if a class isn't native then I guess it can be considered Blueprintable
    // in case when it isn't interface or function library
    ExtractedClass->SetIsBlueprintable(
        Class->GetBoolMetaDataHierarchical(
            FName(ClassBlueprintableMetadata)) ||
        (!ExtractedClass->GetIdentifier().IsNativeDefined() &&
            !ExtractedClass->IsInterface() &&
            ExtractedClass->GetClassType() != EClassType::FunctionLibrary));

    return ExtractedClass;
}

TUniquePtr<FStruct> FApiModelExtractor::ExtractStruct(
    const UScriptStruct* Struct) const
{
    TUniquePtr<FStruct> ExtractedStruct = MakeUnique<FStruct>();

    FPropertyExtractionResult Properties = ExtractProperties(Struct);
    for (int i = 0; i < Properties.Extracted.Num(); ++i)
    {
        FString DefaultValue = ExtractDefaultValueAsStringFromStructProperty(
            Struct, Properties.Original[i]);
        Properties.Extracted[i]->
            SetDefaultValueTransliteration(MoveTemp(DefaultValue));
        ExtractedStruct->AddProperty(MoveTemp(Properties.Extracted[i]));
    }

    FIdentifier Identifier = ExtractIdentifier(Struct);
    ExtractedStruct->SetIdentifier(MoveTemp(Identifier));

    // For uclasses the following works: if super class has BlueprintType and
    // its child has NotBlueprintType then IsAllowableBlueprintVariableType for
    // the child return `true`.
    // For the structs it's the opposite. In other words if somewhere in the
    // hierarchy there was a BlueprintType, then child no matter whether it has
    // NotBlueprintType or not is considered Allowable Blueprint Variable Type.
    ExtractedStruct->SetIsBlueprintType(
        UEdGraphSchema_K2::IsAllowableBlueprintVariableType(Struct));

    return ExtractedStruct;
}

TUniquePtr<FEnum> FApiModelExtractor::ExtractEnum(const UEnum* Enum) const
{
    TUniquePtr<FEnum> ExtractedEnum = MakeUnique<FEnum>();

    // looks like meta=(ScriptName="") does nothing, meanwhile meta's
    // DisplayName works. GetAuthoredName from ExtractName returns wrong stuff
    FIdentifier Identifier = ExtractIdentifier<UField>(Enum);
    FIdentifierName IdentifierName;
    IdentifierName.SetName(Identifier.GetName().GetName());
    IdentifierName.SetDisplayName(FHelper::ExtractDisplayNameOfEnum(Enum));
    Identifier.SetName(MoveTemp(IdentifierName));

    ExtractedEnum->SetIdentifier(MoveTemp(Identifier));
    // BlueprintType affects metadata and no enum flags (there are none)
    ExtractedEnum->SetIsBlueprintType(
        ExtractMetadataFlag(Enum, EnumBlueprintTypeMetadata));

    for (int i = 0; i < Enum->NumEnums(); ++i)
    {
        TUniquePtr<FEnumElement> Element = MakeUnique<FEnumElement>();
        FIdentifierName Name;
        Name.SetName(Enum->GetNameStringByIndex(i));
        Name.SetDisplayName(Enum->GetDisplayNameTextByIndex(i).ToString());
        Element->SetName(MoveTemp(Name));
        Element->SetDescription(Enum->GetToolTipTextByIndex(i).ToString());
        Element->SetIsNativeDefined(
            ExtractedEnum->GetIdentifier().IsNativeDefined());

        // UMETA's Hidden adds metadata to the enum under name ValueName.Hidden
        Element->SetIsHidden(IsEnumElementHidden(Enum,
            Element->GetName().GetName()));
        ExtractedEnum->AddElement(MoveTemp(Element));
    }

    return ExtractedEnum;
}

bool FApiModelExtractor::IsEnumElementHidden(const UEnum* Enum,
    const FString& ElementName) const
{
    return Enum->HasMetaData(*(ElementName + EnumMetadataSeparator +
        EnumHiddenMetadata));
}

TUniquePtr<FModule> FApiModelExtractor::ExtractModule(
    const FString& ObjectPath) const
{
    TUniquePtr<FModule> Module = MakeUnique<FModule>();

    bool bIsNative = false;
    FString Prefix;
    FString Name;

    if (ObjectPath.StartsWith(*PathComponentsSeparator))
    {
        TArray<FString> PathComponents;
        ObjectPath.ParseIntoArray(PathComponents,
            *PathComponentsSeparator);

        if (PathComponents.Num() > 0)
        {
            // game and engine content
            if (PathComponents[0] == GamePath ||
                PathComponents[0] == EnginePath)
            {
                Name = PathComponents[0];
                Prefix = PathComponentsSeparator + PathComponents[0] +
                    PathComponentsSeparator;
                // bIsNative = false;
            }
            // native classes/structs/enums
            else if (PathComponents[0] == ScriptPath)
            {
                bIsNative = true;

                if (PathComponents.Num() > 1)
                {
                    FString Component1 = PathComponents[1];

                    TArray<FString> Objects;
                    Component1.ParseIntoArray(Objects, *ObjectsSeparator);

                    if (Objects.Num() > 0)
                    {
                        Name = Objects[0];
                        Prefix = PathComponentsSeparator + PathComponents[0] +
                            PathComponentsSeparator + Name + ObjectsSeparator;
                    }
                }
            }
            // plugins content
            // todo: not sure about engine plugins content. does it go here?
            else
            {
                Name = PathComponents[0];
                Prefix = PathComponentsSeparator + PathComponents[0] +
                    PathComponentsSeparator;
                // bIsNative = false;
            }
        }
    }

    FIdentifier Identifier;
    Identifier.SetIsNativeDefined(bIsNative);
    FIdentifierName IdentifierName;
    IdentifierName.SetName(Name);
    IdentifierName.SetDisplayName(Name);
    Identifier.SetName(MoveTemp(IdentifierName));
    Identifier.SetFullPath(MoveTemp(Prefix));
    Module->SetIdentifier(MoveTemp(Identifier));

    return Module;
}

TArray<TUniquePtr<FFunction>> FApiModelExtractor::ExtractFunctions(
    const UStruct* Struct) const
{
    TArray<TUniquePtr<FFunction>> Result;

    for (TFieldIterator<UFunction> FunctionsIterator(Struct,
            EFieldIteratorFlags::SuperClassFlags::ExcludeSuper);
        FunctionsIterator; ++FunctionsIterator)
    {
        if (!FunctionsIterator->
            HasAnyFunctionFlags(FUNC_Delegate | FUNC_UbergraphFunction))
        {
            Result.Add(ExtractFunction(*FunctionsIterator));
        }
    }

    return Result;
}

FApiModelExtractor::FPropertyExtractionResult
    FApiModelExtractor::ExtractProperties(
    const UStruct* Struct) const
{
    FPropertyExtractionResult Result;

    for (TFieldIterator<FProperty> PropertiesIterator(Struct,
            EFieldIteratorFlags::SuperClassFlags::ExcludeSuper);
        PropertiesIterator; ++PropertiesIterator)
    {
        if (!(*PropertiesIterator)->GetName()
            .Equals(UberGraphFrame, ESearchCase::IgnoreCase))
        {
            Result.Extracted.Add(ExtractProperty(*PropertiesIterator,
                Cast<UClass>(Struct)));
            Result.Original.Add(*PropertiesIterator);
        }
    }

    return Result;
}

TUniquePtr<FFunction> FApiModelExtractor::ExtractFunction(
    const UFunction* Function) const
{
    TUniquePtr<FFunction> ExtractedFunction = MakeUnique<FFunction>();

    FIdentifier Identifier = ExtractIdentifier(Function);
    Identifier.SetIsDeprecated(
        Function->HasMetaData(FBlueprintMetadata::MD_DeprecatedFunction));

    if (!Identifier.GetCategory().Len())
    {
        Identifier.SetCategory(DefaultCategory);
    }

    ExtractedFunction->SetIdentifier(MoveTemp(Identifier));

    ExtractedFunction->SetCompactNodeTitle(
        Function->GetMetaData(FBlueprintMetadata::MD_CompactNodeTitle));

    auto md = Function->GetPackage()->GetMetaData();
    TArray<FString> Keywords;
    FString KeywordsString =
        Function->GetMetaData(FBlueprintMetadata::MD_FunctionKeywords);
    KeywordsString.ParseIntoArray(Keywords, *KeywordsSeparator);
    for (FString& Keyword : Keywords)
    {
        Keyword = Keyword.TrimStartAndEnd();
    }
    ExtractedFunction->SetKeywords(Keywords);

    // comment from C++
    // removed because it's unstable: some of the funcs had this thing and some
    // did not
    /*ExtractedFunction->SetRawComment(
        Function->GetMetaData(*FunctionCommentMetadata));*/

    bool bIsFunctionFromInterface = false;

    if (auto Class = Cast<UClass>(Function->GetOuter()))
    {
        if (auto Blueprint = Cast<UBlueprint>(Class->ClassGeneratedBy))
        {
            ExtractGraphInformationAboutFunction(*ExtractedFunction,
                Function, Blueprint);
        }

        bIsFunctionFromInterface = IsFunctionFromInterface(Function, Class);
    }

    EAccessModifier AccessModifier = EAccessModifier::Public;
    if (Function->HasAnyFunctionFlags(FUNC_Public))
    {
        AccessModifier = EAccessModifier::Public;
    }
    else if (Function->HasAnyFunctionFlags(FUNC_Protected))
    {
        AccessModifier = EAccessModifier::Protected;
    }
    else if (Function->HasAnyFunctionFlags(FUNC_Private))
    {
        AccessModifier = EAccessModifier::Private;
    }
    ExtractedFunction->SetAccessModifier(AccessModifier);

    // if we have FUNC_BlueprintEvent (the function can be overriden/implemented
    // in BP) and there's no return type and it's native then it's
    // shown as event
    if (ExtractedFunction->GetIdentifier().IsNativeDefined())
    {
        ExtractedFunction->SetIsEvent(
            Function->HasAllFunctionFlags(FUNC_BlueprintEvent) &&
            !Function->GetReturnProperty());
    }

    ExtractedFunction->SetIsPure(
        Function->HasAnyFunctionFlags(FUNC_BlueprintPure));
    // todo(artsiom.drapun): is this inherited when overriden?
    ExtractedFunction->SetCallInEditor(
        Function->HasMetaData(FBlueprintMetadata::MD_CallInEditor));
    ExtractedFunction->SetIsStatic(
        Function->HasAnyFunctionFlags(FUNC_Static));

    // Function->GetSuperFunction():
    // won't work for native classes/interfaces and there's no way to make this
    // work. The data about whether a native func is overriden or not must be
    // extracted from Doxygen output
    ExtractedFunction->SetIsOverride(Function->GetSuperFunction() ||
         bIsFunctionFromInterface);
    ExtractedFunction->SetIsConst(
        Function->HasAnyFunctionFlags(FUNC_Const));

    // let's consider every function that is defined in BP as overridable
    // for native-defines ones, the overridable one is the one which has
    // FUNC_Event
    ExtractedFunction->SetIsOverridable(
        Function->HasAnyFunctionFlags(FUNC_Event) ||
        !ExtractedFunction->GetIdentifier().IsNativeDefined());
    ExtractedFunction->SetIsBlueprintCallable(
        Function->HasAnyFunctionFlags(FUNC_BlueprintCallable));

    ExtractedFunction->SetIsCosmetic(
        Function->HasAnyFunctionFlags(FUNC_BlueprintCosmetic));
    ExtractedFunction->SetIsLatent(
        Function->HasMetaData(*FunctionLatent));
    ExtractedFunction->SetIsAuthorityOnly(
        Function->HasAnyFunctionFlags(FUNC_BlueprintAuthorityOnly));

    // todo(artsiom.drapun): is this inherited when overriden?
    ExtractedFunction->SetIsBlueprintProtected(
        Function->HasMetaData(*FunctionBlueprintProtectedMetadata));

    // RPC
    if (!Function->HasAnyFunctionFlags(FUNC_Net))
    {
        ExtractedFunction->SetRpcMode(ERpcMode::NotReplicated);
    }
    else if (Function->HasAnyFunctionFlags(FUNC_NetClient))
    {
        ExtractedFunction->SetRpcMode(ERpcMode::Client);
    }
    else if (Function->HasAnyFunctionFlags(FUNC_NetServer))
    {
        ExtractedFunction->SetRpcMode(ERpcMode::Server);
    }
    else if (Function->HasAnyFunctionFlags(FUNC_NetMulticast))
    {
        ExtractedFunction->SetRpcMode(ERpcMode::NetMulticast);
    }

    ExtractedFunction->SetIsReliable(
        Function->HasAnyFunctionFlags(FUNC_NetReliable));

    // parameters
    for (TFieldIterator<FProperty> PropertiesIterator(Function,
            EFieldIteratorFlags::SuperClassFlags::ExcludeSuper);
            PropertiesIterator; ++PropertiesIterator)
    {
        // param will be on the left side of a node if
        // 1) it doesn't have CPF_OutParm
        // 2) it has CPF_OutParm but it also has CPF_ReferenceParm (which is
        // affected by UPARAM(ref) or pass-by-reference checkbox)
        // CPF_ReturnParm is only for return params from c++ (the one which is
        // returned by return)
        // BTW: array params from editor always have CPF_ReferenceParm no matter
        // the checkbox is checked or not
        // BTW 2: c++ param like "...TArray<int>& x..." will not have
        // CPF_ReferenceParm (add UPARAM(ref) if you want it to have it), it'll
        // just have CPF_OutParm
        if (PropertiesIterator->HasAnyPropertyFlags(CPF_Parm) &&
            (!PropertiesIterator->HasAnyPropertyFlags(CPF_OutParm) ||
              PropertiesIterator->HasAnyPropertyFlags(CPF_ReferenceParm)))
        {
            ExtractedFunction->AddParameter(
                ExtractParameter(Function, *PropertiesIterator));
        }
    }

    // return parameters
    for (TFieldIterator<FProperty> PropertiesIterator(Function,
            EFieldIteratorFlags::SuperClassFlags::ExcludeSuper);
            PropertiesIterator; ++PropertiesIterator)
    {
        // params that have CPF_OutParam and do not have CPF_ReferenceParm
        // will be on the right side of the node
        if (PropertiesIterator->HasAllPropertyFlags(CPF_OutParm | CPF_Parm) &&
            !PropertiesIterator->HasAnyPropertyFlags(CPF_ReferenceParm))
        {
            ExtractedFunction->AddReturnParameter(
                ExtractParameter(Function, *PropertiesIterator));
        }
    }

    if (bIsFunctionFromInterface &&
        !ExtractedFunction->GetReturnParameters().Num())
    {
        // if from interface and has no return parameters
        ExtractedFunction->SetIsEvent(true);
    }

    return ExtractedFunction;
}

TUniquePtr<FUdProperty> FApiModelExtractor::ExtractProperty(
    const FProperty* Property, const UClass* Class) const
{
    TUniquePtr<FUdProperty> ExtractedProperty = MakeUnique<FUdProperty>();

    FIdentifier Identifier = ExtractIdentifier(Property);
    // todo (artsiom.drapun): by some reason the CPF_Deprecated is not set when
    // the property is marked with meta=(DeprecatedProperty). But it's set when
    // the property in BP was checked as "Deprecated"
    Identifier.SetIsDeprecated(Property->HasAnyPropertyFlags(CPF_Deprecated) ||
        Property->HasMetaData(*PropertyDeprecatedMetadata));

    if (!Identifier.GetCategory().Len())
    {
        Identifier.SetCategory(DefaultCategory);
    }

    ExtractedProperty->SetIdentifier(MoveTemp(Identifier));

    ExtractedProperty->SetIsInstanceEditable(
        Property->HasAnyPropertyFlags(CPF_Edit) &&
        !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance));
    ExtractedProperty->SetIsArchetypeEditable(
        Property->HasAnyPropertyFlags(CPF_Edit) &&
        !Property->HasAnyPropertyFlags(CPF_DisableEditOnTemplate));
    ExtractedProperty->SetIsBlueprintReadOnly(
        Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly));
    ExtractedProperty->SetIsExposeOnSpawn(
        Property->HasAnyPropertyFlags(CPF_ExposeOnSpawn));
    ExtractedProperty->SetIsPrivate(
        Property->HasMetaData(FBlueprintMetadata::MD_Private));
    ExtractedProperty->SetIsConfigVariable(
        Property->HasAnyPropertyFlags(CPF_Config));
    ExtractedProperty->SetIsExposeToCinematic(
        Property->HasAnyPropertyFlags(CPF_Interp));
    ExtractedProperty->SetIsBlueprintVisible(
        Property->HasAnyPropertyFlags(CPF_BlueprintVisible));

    FString UiMin = Property->GetMetaData(*PropertyUiMin);
    FString UiMax = Property->GetMetaData(*PropertyUiMax);
    FString ClampMin = Property->GetMetaData(*PropertyClampMin);
    FString ClampMax = Property->GetMetaData(*PropertyClampMax);

    ExtractedProperty->SetClampMin(ClampMin.Len() > 0 ? ClampMin : UiMin);
    ExtractedProperty->SetClampMax(ClampMax.Len() > 0 ? ClampMax : UiMax);

    bool bIsClass = !!Property->GetOwnerVariant().IsA(UClass::StaticClass());
    bool bIsStruct = !!Property->GetOwnerVariant().
        IsA(UScriptStruct::StaticClass());

    ExtractedProperty->SetReplicationCondition(COND_None);
    ExtractedProperty->SetReplication(EReplication::None);

    if (bIsClass)
    {
        UClass* ClassOwner = Property->GetOwnerVariant().Get<UClass>();

        if (Property->HasAllPropertyFlags(CPF_Net))
        {
            ExtractedProperty->SetReplication(EReplication::Replicate);

            if (Property->HasAllPropertyFlags(CPF_RepNotify))
            {
                ExtractedProperty->SetReplication(EReplication::RepNotify);
            }
        }

        if (ExtractedProperty->GetIdentifier().IsNativeDefined())
        {
            if (ClassOwner->HasAnyClassFlags(CLASS_ReplicationDataIsSetUp))
            {
                TArray<FLifetimeProperty> LifetimeProperties;
                ClassOwner->GetDefaultObject()->
                    GetLifetimeReplicatedProps(LifetimeProperties);

                for (const auto& Prop : LifetimeProperties)
                {
                    if (Prop.RepIndex == Property->RepIndex)
                    {
                        ExtractedProperty->
                            SetReplicationCondition(Prop.Condition);

                        break;
                    }
                }
            }
        }
        else
        {
            ExtractedProperty->SetReplicationCondition(
                Property->GetBlueprintReplicationCondition());
        }
    }
    else if (bIsStruct)
    {
        ExtractedProperty->SetReplication(
            Property->HasAnyPropertyFlags(CPF_RepSkip)
            ? EReplication::None
            : EReplication::Replicate);
    }

    ExtractedProperty->SetType(ExtractType(Property));

    // this is applicable for MC delegates only
    EMulticastDelegateFlags MulticastDelegateFlags = UDMC_NoFlags;
    if (Property->HasAnyPropertyFlags(CPF_BlueprintAssignable))
    {
        MulticastDelegateFlags |= UDMC_BlueprintAssignable;
    }
    if (Property->HasAnyPropertyFlags(CPF_BlueprintCallable))
    {
        MulticastDelegateFlags |= UDMC_BlueprintCallable;
    }
    if (Property->HasAnyPropertyFlags(CPF_BlueprintAuthorityOnly))
    {
        MulticastDelegateFlags |= UDMC_BlueprintAuthorityOnly;
    }
    ExtractedProperty->SetMulticastDelegateFlags(MulticastDelegateFlags);

    return ExtractedProperty;
}

TUniquePtr<FParameter> FApiModelExtractor::ExtractParameter(
    const UFunction* Function,
    const FProperty* Parameter) const
{
    TUniquePtr<FParameter> ExtractedParameter = MakeUnique<FParameter>();

    ExtractedParameter->SetName(ExtractName(Parameter));
    ExtractedParameter->SetType(ExtractType(Parameter));
    ExtractedParameter->SetDefaultValueTransliteration(
        ExtractDefaultValueAsStringFromFunctionParameter(
        Function, Parameter, *ExtractedParameter->GetType()));

    return ExtractedParameter;
}

TUniquePtr<FType> FApiModelExtractor::ExtractType(
    const FProperty* Property) const
{
    TUniquePtr<FTypeExtractor> TypeExtractor = MakeUnique<FTypeExtractor>();

    return TypeExtractor->ExtractType(Property);
}

// UFUnction -
// Cast<UClass>(Function)->GetDefaultObject() - for properties of classes
// (generated or not)
// Cast<UUserDefinedStruct>(Function)->GetDefaultInstance() - BP-defined
// structs
FString FApiModelExtractor::ExtractDefaultValueAsStringFromFunctionParameter(
    const UFunction* Function,
    const FProperty* Parameter, const FType& Type) const
{
    FString ValueAsString;

    UEdGraphSchema_K2::FindFunctionParameterDefaultValue(Function,
        Parameter, ValueAsString);

    // is required because only those values appear in metadata
    // (FindFunctionParameterDefaultValue uses metadata) that are different
    // from auto generated ones. so default values that are equal to auto-
    // generated ones are not presented in metadata, so they need to be filled
    // manually
    if (ValueAsString.Len() == 0)
    {
        auto Enum = CastField<FEnumProperty>(Parameter);
        auto ByteEnum = CastField<FByteProperty>(Parameter);

        if (Enum || (ByteEnum && ByteEnum->Enum))
        {
            UEnum* EnumItself = (Enum ? Enum->GetEnum() : ByteEnum->Enum);
            if (IsValid(EnumItself))
            {
                // so, no default value was specified, then use he name of first
                // element of enum that is not hidden
                FString Name;
                for (int i = 0; i < EnumItself->NumEnums(); ++i)
                {
                    FString ElementName = EnumItself->GetNameStringByIndex(i);
                    if (!IsEnumElementHidden(EnumItself, ElementName))
                    {
                        Name = MoveTemp(ElementName);
                        break;
                    }
                }

                ValueAsString = MoveTemp(Name);
            }
        }
        else
        {
            ValueAsString = Type.GetAutoGeneratedValueTransliteration();
        }
    }

    ValueAsString = ProcessDefaultValue(MoveTemp(ValueAsString), Parameter);

    return ValueAsString;
}

FString FApiModelExtractor::ExtractDefaultValueAsStringFromProperty(
    const UClass* Class, const FProperty* Property) const
{
    FString ValueAsString;
    EPropertyPortFlags Flags = EPropertyPortFlags::PPF_None;

    if (Property->IsA<FTextProperty>())
    {
        // export clean text, no string table references
        Flags = static_cast<EPropertyPortFlags>(Flags | PPF_PropertyWindow);
    }

    const uint8* PropertyAddr =
        Property->ContainerPtrToValuePtr<uint8>(Class->GetDefaultObject());
    Property->ExportTextItem(ValueAsString, PropertyAddr,
        nullptr, nullptr, Flags);

    ValueAsString = ProcessDefaultValue(MoveTemp(ValueAsString), Property);

    return ValueAsString;
}

FString FApiModelExtractor::ExtractDefaultValueAsStringFromStructProperty(
    const UScriptStruct* Struct, const FProperty* Property) const
{
    FStructOnScope StructCopy{Struct};

    Struct->InitializeDefaultValue(StructCopy.GetStructMemory());

    EPropertyPortFlags Flags = EPropertyPortFlags::PPF_None;

    if (Property->IsA<FTextProperty>())
    {
        // export clean text, no string table references
        Flags = static_cast<EPropertyPortFlags>(Flags | PPF_PropertyWindow);
    }

    FString ValueAsString;
    const uint8* PropertyAddr =
        Property->ContainerPtrToValuePtr<uint8>(StructCopy.GetStructMemory());
    Property->ExportTextItem(ValueAsString, PropertyAddr,
        nullptr, nullptr, Flags);

    ValueAsString = ProcessDefaultValue(MoveTemp(ValueAsString), Property);

    return ValueAsString;
}

FString FApiModelExtractor::ProcessDefaultValue(
    FString Value, const FProperty* Property) const
{
    FString Result = MoveTemp(Value);

    if (Property->IsA<FBoolProperty>())
    {
        Result = Result.ToLower();
    }

    return Result;
}

void FApiModelExtractor::ExtractGraphInformationAboutFunction(
    FFunction& Function,
    const UFunction* OriginalFunction,
    const UBlueprint* Blueprint) const
{
    TArray<UEdGraph*> Graphs;
    // all functions definitions are here
#if ENGINE_MAJOR_VERSION >= 5
    for (auto Graph : Blueprint->FunctionGraphs)
    {
        Graphs.Insert(Graph, 0);
    }

    // user construction script and event graph are here
    for (auto Graph : Blueprint->UbergraphPages)
    {
        Graphs.Insert(Graph, 0);
    }
#else
    Graphs.Insert(Blueprint->FunctionGraphs, 0);
    // user construction script and event graph are here
    Graphs.Insert(Blueprint->UbergraphPages, 0);
#endif
    // now we need to find a UK2Node_Event/UK2Node_FunctionEntry which contains
    // whose FindSignatureFunction() returns a function equal to Function
    for (UEdGraph* Graph : Graphs)
    {
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            FName FunctionName;
            bool bIsEvent = false;

            if (auto Event = Cast<UK2Node_Event>(Node))
            {
                FunctionName = Event->GetFunctionName();
                bIsEvent = true;
            }
            else if (auto CastedFunction = Cast<UK2Node_FunctionEntry>(Node))
            {
                FunctionName =
                    CastedFunction->FindSignatureFunction()->GetFName();
            }

            // be suspicious with this part because there's no 100% confidence
            // that name extracted from graph node corresponding to a
            // function and the function's name always match
            if (FunctionName.IsEqual(OriginalFunction->GetFName()))
            {
                Function.SetNodeComment(Node->NodeComment);
                // AFAIK there's no way to find out how a function defined
                // in blueprint will be displayed (as an event or as a function)
                // by only looking at its UFunction. So the way we distinguish
                // it here is by looking by which UK2Node-derived class it's
                // represented in the graph - if it's FunctionEntry - it's a
                // function, if it's Event - it's an event
                // Note: it's applied only to functions declared (not overriden
                // or implemented but first mentioned) in blueprints. Native-
                // declared functions are processed above in ExtractFunction
                Function.SetIsEvent(bIsEvent);

                break;
            }
        }
    }
}

bool FApiModelExtractor::ExtractMetadataFlag(const UField* Object,
    const FString& Name, bool Default) const
{
    if (!Object->HasMetaData(*Name))
    {
        return Default;
    }

    return Object->GetMetaData(*Name)
        .Equals(True, ESearchCase::IgnoreCase);
}

bool FApiModelExtractor::IsFunctionFromInterface(const UFunction* Function,
    const UClass* Class) const
{
    FString Name = ExtractName(Function).GetName();

    for (const auto& Interface : Class->Interfaces)
    {
        for (TFieldIterator<UFunction> FunctionsIterator(Interface.Class,
                EFieldIteratorFlags::SuperClassFlags::ExcludeSuper);
            FunctionsIterator; ++FunctionsIterator)
        {
            if (!FunctionsIterator->
                HasAnyFunctionFlags(FUNC_Delegate | FUNC_UbergraphFunction))
            {
                if (Name.Equals(ExtractName(*FunctionsIterator)
                    .GetName()))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
