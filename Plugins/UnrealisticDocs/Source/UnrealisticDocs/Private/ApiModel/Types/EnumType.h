﻿/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Type.h"

class UNREALISTICDOCS_API FEnumType : public FType
{
public:
    void Accept(ITypeVisitor& Visitor) const override
    {
        Visitor.Visit(*this);
    }

    FString GetAutoGeneratedValueTransliteration() const override
    {
        return AutoGeneratedValue;
    }

    ~FEnumType() override = default;

private:
    static const FString AutoGeneratedValue;
};
