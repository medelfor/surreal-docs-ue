﻿/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ApiModel/Identifier.h"
#include "TypeVisitor.h"

class FType
{
public:
    const FBasicIdentifierInfo& GetIdentifierInfo() const
    {
        return IdentifierInfo;
    }

    void SetIdentifierInfo(FBasicIdentifierInfo NewIdentifierInfo)
    {
        this->IdentifierInfo = MoveTemp(NewIdentifierInfo);
    }

    bool IsReference() const
    {
        return bIsReference;
    }

    void SetIsReference(bool bNewIsReference)
    {
        this->bIsReference = bNewIsReference;
    }

    bool IsConst() const
    {
        return bIsConst;
    }

    void SetIsConst(bool bNewIsConst)
    {
        this->bIsConst = bNewIsConst;
    }

    virtual FString GetAutoGeneratedValueTransliteration() const = 0;

    virtual void Accept(ITypeVisitor& Visitor) const = 0;

    virtual ~FType() = default;

private:
    FBasicIdentifierInfo IdentifierInfo;
    bool bIsReference = false;
    bool bIsConst = false;
};
