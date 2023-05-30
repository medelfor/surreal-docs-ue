/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "IdentifierName.h"

class FBasicIdentifierInfo
{
public:
    const FIdentifierName& GetName() const
    {
        return Name;
    }

    void SetName(FIdentifierName NewName)
    {
        this->Name = MoveTemp(NewName);
    }

    const FString& GetFullPath() const
    {
        return FullPath;
    }

    void SetFullPath(FString NewFullPath)
    {
        this->FullPath = MoveTemp(NewFullPath);
    }

    bool IsNativeDefined() const
    {
        return bIsNativeDefined;
    }

    void SetIsNativeDefined(bool bNewIsNativeDefined)
    {
        this->bIsNativeDefined = bNewIsNativeDefined;
    }

private:
    FIdentifierName Name;
    FString FullPath;
    bool bIsNativeDefined = false;
};
