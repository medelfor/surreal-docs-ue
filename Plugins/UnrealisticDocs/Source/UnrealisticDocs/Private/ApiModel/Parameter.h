/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Types/Type.h"

class FParameter
{
public:
    FType const* GetType() const
    {
        return Type.Get();
    }

    void SetType(TUniquePtr<FType> NewType)
    {
        this->Type = MoveTemp(NewType);
    }

    const FIdentifierName& GetName() const
    {
        return Name;
    }

    void SetName(FIdentifierName NewName)
    {
        this->Name = MoveTemp(NewName);
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

private:
    TUniquePtr<FType> Type;
    FIdentifierName Name;
    FString DefaultValueTransliteration;
};
