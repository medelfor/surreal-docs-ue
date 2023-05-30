/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "BasicIdentifierInfo.h"

class FIdentifier : public FBasicIdentifierInfo
{
public:
    const FString& GetTooltip() const
    {
        return Tooltip;
    }

    void SetTooltip(FString NewTooltip)
    {
        this->Tooltip = MoveTemp(NewTooltip);
    }

    const FString& GetShortTooltip() const
    {
        return ShortTooltip;
    }

    void SetShortTooltip(FString NewShortTooltip)
    {
        this->ShortTooltip = MoveTemp(NewShortTooltip);
    }

    const FString& GetCategory() const
    {
        return Category;
    }

    void SetCategory(FString NewCategory)
    {
        this->Category = MoveTemp(NewCategory);
    }

    bool IsDeprecated() const
    {
        return bIsDeprecated;
    }

    void SetIsDeprecated(bool bNewIsDeprecated)
    {
        this->bIsDeprecated = bNewIsDeprecated;
    }

    const FString& GetDeprecationMessage() const
    {
        return DeprecationMessage;
    }

    void SetDeprecationMessage(FString NewDeprecationMessage)
    {
        this->DeprecationMessage = MoveTemp(NewDeprecationMessage);
    }

private:
    FString Tooltip;
    FString ShortTooltip;
    FString Category;
    bool bIsDeprecated = false;
    FString DeprecationMessage;
};
