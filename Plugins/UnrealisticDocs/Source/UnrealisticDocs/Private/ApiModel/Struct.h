/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Identifier.h"

class FStruct
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

    const TArray<FUdProperty*>& GetPropertiesView() const
    {
        return PropertiesView;
    }

    void AddProperty(TUniquePtr<FUdProperty> Property)
    {
        this->PropertiesView.Add(Property.Get());
        Properties.Add(MoveTemp(Property));
    }

private:
    FIdentifier Identifier;
    bool bIsBlueprintType = false;
    TArray<FUdProperty*> PropertiesView;
    TArray<TUniquePtr<FUdProperty>> Properties;
};
