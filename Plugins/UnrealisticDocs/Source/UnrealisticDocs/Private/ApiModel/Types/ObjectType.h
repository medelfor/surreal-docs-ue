﻿/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Type.h"
#include "ApiModel/Class.h"

enum class EObjectReference
{
    Object,
    WeakObject,
    LazyObject,
    SoftObject,
    Class,
    SoftClass,
    Undefined
};

class UNREALISTICDOCS_API FObjectType : public FType
{
public:
    EObjectReference GetObjectReference() const
    {
        return ObjectReference;
    }

    void SetObjectReference(EObjectReference NewObjectReference)
    {
        this->ObjectReference = NewObjectReference;
    }

    EClassType GetClassType() const
    {
        return ClassType;
    }

    void SetClassType(EClassType NewClassType)
    {
        this->ClassType = MoveTemp(NewClassType);
    }

    void Accept(ITypeVisitor& Visitor) const override
    {
        Visitor.Visit(*this);
    }

    FString GetAutoGeneratedValueTransliteration() const override
    {
        return AutoGeneratedValue;
    }

    ~FObjectType() override = default;

private:
    static const FString AutoGeneratedValue;

    EObjectReference ObjectReference = EObjectReference::Class;
    EClassType ClassType = EClassType::Class;
};
