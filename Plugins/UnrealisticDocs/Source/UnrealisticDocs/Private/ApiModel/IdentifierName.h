/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class FIdentifierName
{
public:
    const FString& GetName() const
    {
        return Name;
    }

    void SetName(FString NewName)
    {
        this->Name = MoveTemp(NewName);
    }

    const FString& GetDisplayName() const
    {
        return DisplayName;
    }

    void SetDisplayName(FString NewDisplayName)
    {
        this->DisplayName = MoveTemp(NewDisplayName);
    }

private:
    FString Name;
    FString DisplayName;
};
