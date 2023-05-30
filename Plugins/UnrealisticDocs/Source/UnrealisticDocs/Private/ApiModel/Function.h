/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Identifier.h"
#include "Parameter.h"

enum class EAccessModifier
{
    Public,
    Protected,
    Private
};

enum class ERpcMode
{
    NotReplicated,
    NetMulticast,
    Server,
    Client
};

class FFunction
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

    const TArray<FString>& GetKeywords() const
    {
        return Keywords;
    }

    void SetKeywords(TArray<FString> NewKeywords)
    {
        this->Keywords = MoveTemp(NewKeywords);
    }

    const FString& GetCompactNodeTitle() const
    {
        return CompactNodeTitle;
    }

    void SetCompactNodeTitle(FString NewCompactNodeTitle)
    {
        this->CompactNodeTitle = MoveTemp(NewCompactNodeTitle);
    }

    // @deprecated
    const FString& GetRawComment() const
    {
        return RawComment;
    }

    // @deprecated
    void SetRawComment(FString NewRawComment)
    {
    }

    const FString& GetNodeComment() const
    {
        return NodeComment;
    }

    void SetNodeComment(FString NewNodeComment)
    {
        this->NodeComment = MoveTemp(NewNodeComment);
    }

    EAccessModifier GetAccessModifier() const
    {
        return AccessModifier;
    }

    void SetAccessModifier(EAccessModifier NewAccessModifier)
    {
        this->AccessModifier = NewAccessModifier;
    }

    bool IsPure() const
    {
        return bIsPure;
    }

    void SetIsPure(bool bNewIsPure)
    {
        this->bIsPure = bNewIsPure;
    }

    bool IsCallInEditor() const
    {
        return bCallInEditor;
    }

    void SetCallInEditor(bool bNewCallInEditor)
    {
        this->bCallInEditor = bNewCallInEditor;
    }

    bool IsOverride() const
    {
        return bIsOverride;
    }

    void SetIsOverride(bool bNewIsOverride)
    {
        this->bIsOverride = bNewIsOverride;
    }

    bool IsEvent() const
    {
        return bIsEvent;
    }

    void SetIsEvent(bool bNewIsEvent)
    {
        this->bIsEvent = bNewIsEvent;
    }

    bool IsReliable() const
    {
        return bIsReliable;
    }

    void SetIsReliable(bool bNewIsReliable)
    {
        this->bIsReliable = bNewIsReliable;
    }

    bool IsStatic() const
    {
        return bIsStatic;
    }

    void SetIsStatic(bool bNewIsStatic)
    {
        this->bIsStatic = bNewIsStatic;
    }

    bool IsConst() const
    {
        return bIsConst;
    }

    void SetIsConst(bool bNewIsConst)
    {
        this->bIsConst = bNewIsConst;
    }

    bool IsBlueprintCallable() const
    {
        return bIsBlueprintCallable;
    }

    void SetIsBlueprintCallable(bool bNewIsBlueprintCallable)
    {
        this->bIsBlueprintCallable = bNewIsBlueprintCallable;
    }

    // relatively to blueprints
    bool IsOverridable() const
    {
        return bIsOverridable;
    }

    void SetIsOverridable(bool bNewIsOverridable)
    {
        this->bIsOverridable = bNewIsOverridable;
    }

    bool IsBlueprintProtected() const
    {
        return bIsBlueprintProtected;
    }

    void SetIsBlueprintProtected(bool bNewIsBlueprintProtected)
    {
        this->bIsBlueprintProtected = bNewIsBlueprintProtected;
    }

    bool IsLatent() const
    {
        return bIsLatent;
    }

    void SetIsLatent(bool bNewIsLatent)
    {
        this->bIsLatent = bNewIsLatent;
    }

    bool IsCosmetic() const
    {
        return bIsCosmetic;
    }

    void SetIsCosmetic(bool bNewIsCosmetic)
    {
        this->bIsCosmetic = bNewIsCosmetic;
    }

    bool IsAuthorityOnly() const
    {
        return bIsAuthorityOnly;
    }

    void SetIsAuthorityOnly(bool bNewIsAuthorityOnly)
    {
        bIsAuthorityOnly = bNewIsAuthorityOnly;
    }

    ERpcMode GetRpcMode() const
    {
        return RpcMode;
    }

    void SetRpcMode(ERpcMode NewRpcMode)
    {
        this->RpcMode = NewRpcMode;
    }

    const TArray<FParameter*>& GetParameters() const
    {
        return ParametersViews;
    }

    void AddParameter(TUniquePtr<FParameter> Parameter)
    {
        this->ParametersViews.Add(Parameter.Get());
        this->Parameters.Add(MoveTemp(Parameter));
    }

    const TArray<FParameter*>& GetReturnParameters() const
    {
        return ReturnParametersViews;
    }

    void AddReturnParameter(TUniquePtr<FParameter> ReturnParameter)
    {
        this->ReturnParametersViews.Add(ReturnParameter.Get());
        this->ReturnParameters.Add(MoveTemp(ReturnParameter));
    }

private:
    FIdentifier Identifier;
    TArray<FString> Keywords;
    FString CompactNodeTitle;
    FString RawComment;
    FString NodeComment;
    EAccessModifier AccessModifier = EAccessModifier::Public;
    bool bIsPure = false;
    bool bCallInEditor = false;
    bool bIsOverride = false;
    bool bIsEvent = false;
    bool bIsReliable = false;
    bool bIsStatic = false;
    bool bIsConst = false;
    bool bIsBlueprintCallable = false;
    bool bIsOverridable = false;
    bool bIsBlueprintProtected = false;
    bool bIsLatent = false;
    bool bIsCosmetic = false;
    bool bIsAuthorityOnly = false;
    ERpcMode RpcMode = ERpcMode::NotReplicated;
    TArray<FParameter*> ParametersViews;
    TArray<TUniquePtr<FParameter>> Parameters;
    TArray<FParameter*> ReturnParametersViews;
    TArray<TUniquePtr<FParameter>> ReturnParameters;
};
