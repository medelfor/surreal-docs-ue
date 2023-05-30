/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Identifier.h"

class FEnumElement : public FBasicIdentifierInfo
{
public:
    const FString& GetDescription() const
    {
        return Description;
    }

    void SetDescription(FString NewDescription)
    {
        this->Description = MoveTemp(NewDescription);
    }

    bool IsHidden() const
    {
        return bIsHidden;
    }

    void SetIsHidden(bool bNewIsHidden)
    {
        this->bIsHidden = bNewIsHidden;
    }

private:
    FString Description;
    bool bIsHidden = false;
};

class FEnum
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

    bool IsBlueprintType() const
    {
        return bIsBlueprintType;
    }

    void SetIsBlueprintType(bool bNewIsBlueprintType)
    {
        this->bIsBlueprintType = bNewIsBlueprintType;
    }

    const TArray<FEnumElement*>& GetElementsView() const
    {
        return ElementsView;
    }

    void AddElement(TUniquePtr<FEnumElement> Element)
    {
        this->ElementsView.Add(Element.Get());
        this->Elements.Add(MoveTemp(Element));
    }

private:
    FIdentifier Identifier;
    bool bIsBlueprintType = false;
    TArray<FEnumElement*> ElementsView;
    TArray<TUniquePtr<FEnumElement>> Elements;
};
