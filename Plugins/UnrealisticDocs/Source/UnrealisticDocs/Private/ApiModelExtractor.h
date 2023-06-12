/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ApiModel/Class.h"
#include "ApiModel/Enum.h"
#include "ApiModel/Module.h"
#include "ApiModel/UdProperty.h"
#include "ApiModel/Struct.h"
#include "ApiModel/Types/ArrayType.h"
#include "ApiModel/Types/DelegateType.h"
#include "ApiModel/Types/PrimitiveType.h"
#include "ApiModel/Types/StructType.h"

class UNREALISTICDOCS_API FApiModelExtractor
{
public:
    FApiModelExtractor();

    // expects that SetUpRuntimeReplicationData() was called on Class if it's
    // a native class
    TUniquePtr<FClass> ExtractClass(const UClass* Class) const;

    TUniquePtr<FStruct> ExtractStruct(const UScriptStruct* Struct) const;

    TUniquePtr<FEnum> ExtractEnum(const UEnum* Enum) const;

    TUniquePtr<FModule> ExtractModule(const FString& ObjectPath) const;

private:
    static const FString KeywordsSeparator;
    static const FString FunctionCommentMetadata;
    static const FString FunctionBlueprintProtectedMetadata;
    static const FString FunctionLatent;
    static const FString PropertyDeprecatedMetadata;
    static const FString PropertyClampMin;
    static const FString PropertyClampMax;
    static const FString PropertyUiMin;
    static const FString PropertyUiMax;
    static const FString PathComponentsSeparator;
    static const FString GamePath;
    static const FString ScriptPath;
    static const FString EnginePath;
    static const FString EnumBlueprintTypeMetadata;
    static const FString StructBlueprintTypeMetadata;
    static const FString EnumHiddenMetadata;
    static const FString EnumMetadataSeparator;
    static const FString UberGraphFrame;
    static const FString ObjectsSeparator;
    static const FString True;
    static const FString ClassBlueprintableMetadata;
    static const FString ClassBlueprintTypeMetadata;
    static const FString DefaultCategory;

    struct FPropertyExtractionResult
    {
        TArray<TUniquePtr<FUdProperty>> Extracted;
        TArray<FProperty*> Original;
    };

    TArray<TUniquePtr<FFunction>> ExtractFunctions(const UStruct* Struct) const;

    // expects that SetUpRuntimeReplicationData() was called on Struct if it's
    // a native class
    FPropertyExtractionResult ExtractProperties(const UStruct* Struct) const;

    TUniquePtr<FParameter> ExtractParameter(const UFunction* Function,
        const FProperty* Parameter) const;

    TUniquePtr<FType> ExtractType(const FProperty* Property) const;

    FString ExtractDefaultValueAsStringFromFunctionParameter(
        const UFunction* Function, const FProperty* Parameter,
        const FType& Type) const;

    FString ExtractDefaultValueAsStringFromProperty(
        const UClass* Class, const FProperty* Property) const;

    FString ExtractDefaultValueAsStringFromStructProperty(
        const UScriptStruct* Struct, const FProperty* Property) const;

    void ExtractGraphInformationAboutFunction(
        FFunction& Function,
        const UFunction* OriginalFunction,
        const UBlueprint* Blueprint) const;

    bool ExtractMetadataFlag(const UField* Object,
        const FString& Name, bool Default = false) const;

    bool IsFunctionFromInterface(const UFunction* Function,
        const UClass* Class) const;

    TUniquePtr<FFunction> ExtractFunction(const UFunction* Function) const;

    // expects that SetUpRuntimeReplicationData() was called on Class if it's
    // a native class
    TUniquePtr<FUdProperty> ExtractProperty(
        const FProperty* Property) const;

    bool IsEnumElementHidden(const UEnum* Enum,
        const FString& ElementName) const;

    FString ProcessDefaultValue(FString Value, const FProperty* Property) const;

    class FTypeExtractor
    {
    public:
        FTypeExtractor();

        TUniquePtr<FType> ExtractType(const FProperty* Property) const;

        static const FString ArrayName;
        static const FString SetName;
        static const FString MapName;
        static const FString ByteName;
        static const FString IntName;
        static const FString Int64Name;
        static const FString FloatName;
        static const FString DoubleName;
        static const FString BoolName;
        static const FString NameName;
        static const FString StringName;
        static const FString TextName;
        static const FString UndefinedName;

    private:
        TMap<PrimitiveType::EPrimitiveType, FString> PrimitiveTypesToString;

        TUniquePtr<FType> ExtractDelegateType
            (const FDelegateProperty* Property) const;

        TUniquePtr<FType> ExtractMulticastDelegateType(
            const FMulticastDelegateProperty* Property) const;

        TUniquePtr<FType> ExtractObjectType
            (const FObjectPropertyBase* Property) const;

        TUniquePtr<FType> ExtractPrimitiveType(
            const FProperty* Property) const;

        TUniquePtr<FType> ExtractStructType(
            const FStructProperty* Property) const;

        TUniquePtr<FType> ExtractInterfaceType
            (const FInterfaceProperty* Property) const;

        TUniquePtr<FType> ExtractEnumType(
            const FEnumProperty* Property) const;

        TUniquePtr<FType> ExtractEnumTypeAsByte
            (const FByteProperty* Property) const;

        TUniquePtr<FType> ExtractArrayType(
            const FArrayProperty* Property) const;

        TUniquePtr<FType> ExtractSetType(const FSetProperty* Property) const;

        TUniquePtr<FType> ExtractMapType(const FMapProperty* Property) const;
    };
};
