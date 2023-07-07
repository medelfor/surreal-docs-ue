/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Function.h"
#include "UdProperty.h"
#include "Components/Widget.h"

enum class EClassType
{
    Class,
    Widget,
    FunctionLibrary,
    Component,
    Actor,
    Subsystem,
    DeveloperSettings
};

class FClass
{
public:
    const FIdentifier& GetIdentifier() const
    {
        return Identifier;
    }

    void SetIdentifier(const FIdentifier& NewIdentifier)
    {
        this->Identifier = NewIdentifier;
    }

    bool IsBlueprintType() const
    {
        return bIsBlueprintType;
    }

    void SetIsBlueprintType(bool bNewIsBlueprintType)
    {
        this->bIsBlueprintType = bNewIsBlueprintType;
    }

    bool IsBlueprintable() const
    {
        return bIsBlueprintable;
    }

    void SetIsBlueprintable(bool bNewIsBlueprintable)
    {
        this->bIsBlueprintable = bNewIsBlueprintable;
    }

    bool IsAbstract() const
    {
        return bIsAbstract;
    }

    void SetIsAbstract(bool bNewIsAbstract)
    {
        this->bIsAbstract = bNewIsAbstract;
    }

    bool IsInterface() const
    {
        return bIsInterface;
    }

    void SetIsInterface(bool bNewIsInterface)
    {
        this->bIsInterface = bNewIsInterface;
    }

    EClassType GetClassType() const
    {
        return ClassType;
    }

    void SetClassType(EClassType NewClassType)
    {
        this->ClassType = MoveTemp(NewClassType);
    }

    const TArray<FFunction*>& GetFunctionsView() const
    {
        return FunctionsView;
    }

    void AddFunction(TUniquePtr<FFunction> Function)
    {
        FunctionsView.Add(Function.Get());
        Functions.Add(MoveTemp(Function));
    }

    const TArray<FUdProperty*>& GetPropertiesView() const
    {
        return PropertiesView;
    }

    void AddProperty(TUniquePtr<FUdProperty> Property)
    {
        PropertiesView.Add(Property.Get());
        Properties.Add(MoveTemp(Property));
    }

    const TArray<FBasicIdentifierInfo>& GetInterfaces() const
    {
        return Interfaces;
    }

    void AddInterface(FBasicIdentifierInfo Interface)
    {
        this->Interfaces.Add(MoveTemp(Interface));
    }

    const FBasicIdentifierInfo& GetSuperClass() const
    {
        return SuperClass;
    }

    void SetSuperClass(FBasicIdentifierInfo NewSuperClass)
    {
        this->SuperClass = MoveTemp(NewSuperClass);
    }

    const FString& GetNamespace() const
    {
        return Namespace;
    }

    void SetNamespace(FString NewNamespace)
    {
        this->Namespace = MoveTemp(NewNamespace);
    }

    const FString& GetConfigName() const
    {
        return ConfigName;
    }

    void SetConfigName(FString NewConfigName)
    {
        this->ConfigName = MoveTemp(NewConfigName);
    }

    // todo(artsiom.drapun): is not tested
    static EClassType DetermineClassType(const UClass* Class)
    {
        if (!IsValid(Class)) return EClassType::Class;

        if (Class->IsChildOf<UActorComponent>())
        {
            return EClassType::Component;
        }
        if (Class->IsChildOf<UWidget>())
        {
            return EClassType::Widget;
        }
        if (Class->IsChildOf<UBlueprintFunctionLibrary>())
        {
            return EClassType::FunctionLibrary;
        }
        if (Class->IsChildOf<UDeveloperSettings>())
        {
            return EClassType::DeveloperSettings;
        }
        if (Class->IsChildOf<USubsystem>())
        {
            return EClassType::Subsystem;
        }
        if (Class->IsChildOf<AActor>())
        {
            return EClassType::Actor;
        }

        return EClassType::Class;
    }

private:
    FIdentifier Identifier;
    bool bIsAbstract = false;
    bool bIsInterface = false;
    bool bIsBlueprintType = false;
    bool bIsBlueprintable = false;
    EClassType ClassType = EClassType::Class;
    TArray<FFunction*> FunctionsView;
    TArray<TUniquePtr<FFunction>> Functions;
    TArray<FUdProperty*> PropertiesView;
    TArray<TUniquePtr<FUdProperty>> Properties;
    TArray<FBasicIdentifierInfo> Interfaces;
    FBasicIdentifierInfo SuperClass;
    FString Namespace;
    FString ConfigName;
};
