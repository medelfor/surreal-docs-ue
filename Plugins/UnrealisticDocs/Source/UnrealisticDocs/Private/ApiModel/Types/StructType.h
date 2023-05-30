﻿/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Type.h"
#include "UObject/NoExportTypes.h"

class UNREALISTICDOCS_API FStructType : public FType
{
public:
    void Accept(ITypeVisitor& Visitor) const override
    {
        Visitor.Visit(*this);
    }

    FString GetAutoGeneratedValueTransliteration() const override;

    ~FStructType() override = default;

private:
    static const FString VectorClass;
    static const FString RotatorClass;
    static const FString VectorAutoGeneratedValue;
    static const FString RotatorAutoGeneratedValue;
    static const FString DefaultAutoGeneratedValue;
};
