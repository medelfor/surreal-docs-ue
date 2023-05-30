/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Identifier.h"
#include "Types/Type.h"

enum class EReplication
{
    None,
    Replicate,
    RepNotify
};

enum EMulticastDelegateFlags : uint32
{
    UDMC_NoFlags = 0x00000000,
    UDMC_BlueprintAssignable = 0x00000001,
    UDMC_BlueprintCallable = 0x00000002,
    UDMC_BlueprintAuthorityOnly = 0x00000004,
};

inline EMulticastDelegateFlags operator|(
    EMulticastDelegateFlags A, EMulticastDelegateFlags B)
{
    return static_cast<EMulticastDelegateFlags>(
        static_cast<int>(A) | static_cast<int>(B));
}

inline void operator|=(
    EMulticastDelegateFlags& A, EMulticastDelegateFlags B)
{
    A = A | B;
}

class FUdProperty
{
public:
    const FIdentifier& GetIdentifier() const
    {
        return Identifier;
    }

    void SetIdentifier(FIdentifier NewIdentifier)
    {
        this->Identifier = MoveTemp(NewIdentifier);
    }

    // todo(artsiom.drapun): why is it even pointer
    FType const* GetType() const
    {
        return Type.Get();
    }

    void SetType(TUniquePtr<FType> NewType)
    {
        this->Type = MoveTemp(NewType);
    }

    bool IsInstanceEditable() const
    {
        return bIsInstanceEditable;
    }

    void SetIsInstanceEditable(bool bNewInstanceEditable)
    {
        this->bIsInstanceEditable = bNewInstanceEditable;
    }

    bool IsArchetypeEditable() const
    {
        return bIsArchetypeEditable;
    }

    void SetIsArchetypeEditable(bool bNewArchetypeEditable)
    {
        this->bIsArchetypeEditable = bNewArchetypeEditable;
    }

    bool IsBlueprintReadOnly() const
    {
        return bIsBlueprintReadOnly;
    }

    void SetIsBlueprintReadOnly(bool bNewBlueprintReadOnly)
    {
        this->bIsBlueprintReadOnly = bNewBlueprintReadOnly;
    }

    bool IsExposeOnSpawn() const
    {
        return bIsExposeOnSpawn;
    }

    void SetIsExposeOnSpawn(bool bNewExposeOnSpawn)
    {
        this->bIsExposeOnSpawn = bNewExposeOnSpawn;
    }

    bool IsPrivate() const
    {
        return bIsPrivate;
    }

    void SetIsPrivate(bool bNewPrivate)
    {
        this->bIsPrivate = bNewPrivate;
    }

    bool IsExposeToCinematic() const
    {
        return bIsExposeToCinematic;
    }

    void SetIsExposeToCinematic(bool bNewExposeToCinematic)
    {
        this->bIsExposeToCinematic = bNewExposeToCinematic;
    }

    bool IsConfigVariable() const
    {
        return bIsConfigVariable;
    }

    void SetIsConfigVariable(bool bNewIsConfigVariable)
    {
        this->bIsConfigVariable = bNewIsConfigVariable;
    }

    bool IsBlueprintVisible() const
    {
        return bIsBlueprintVisible;
    }

    void SetIsBlueprintVisible(bool bNewIsBlueprintVisible)
    {
        this->bIsBlueprintVisible = bNewIsBlueprintVisible;
    }

    const FString& GetClampMin() const
    {
        return ClampMin;
    }

    void SetClampMin(FString NewClampMin)
    {
        this->ClampMin = MoveTemp(NewClampMin);
    }

    const FString& GetClampMax() const
    {
        return ClampMax;
    }

    void SetClampMax(FString NewClampMax)
    {
        this->ClampMax = MoveTemp(NewClampMax);
    }

    EReplication GetReplication() const
    {
        return Replication;
    }

    void SetReplication(EReplication NewReplication)
    {
        this->Replication = NewReplication;
    }

    const ELifetimeCondition& GetReplicationCondition() const
    {
        return ReplicationCondition;
    }

    void SetReplicationCondition(ELifetimeCondition NewReplicationCondition)
    {
        this->ReplicationCondition = MoveTemp(NewReplicationCondition);
    }

    const FString& GetDefaultValueTransliteration() const
    {
        return DefaultValueTransliteration;
    }

    void SetDefaultValueTransliteration(FString NewDefaultValueTransliteration)
    {
        this->DefaultValueTransliteration = MoveTemp(
            NewDefaultValueTransliteration);
    }

    EMulticastDelegateFlags GetMulticastDelegateFlags() const
    {
        return MulticastDelegateFlags;
    }

    void SetMulticastDelegateFlags(
        EMulticastDelegateFlags NewMulticastDelegateFlags)
    {
        this->MulticastDelegateFlags = NewMulticastDelegateFlags;
    }

private:
    FIdentifier Identifier;
    TUniquePtr<FType> Type;
    bool bIsInstanceEditable = false;
    bool bIsArchetypeEditable = false;
    bool bIsBlueprintReadOnly = false;
    bool bIsExposeOnSpawn = false;
    bool bIsPrivate = false;
    bool bIsExposeToCinematic = false;
    bool bIsConfigVariable = false;
    bool bIsBlueprintVisible = false;
    FString ClampMin;
    FString ClampMax;
    EReplication Replication = EReplication::None;
    ELifetimeCondition ReplicationCondition = COND_None;
    FString DefaultValueTransliteration;
    EMulticastDelegateFlags MulticastDelegateFlags = UDMC_NoFlags;
};
