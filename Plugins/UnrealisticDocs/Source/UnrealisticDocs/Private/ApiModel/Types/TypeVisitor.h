/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class FObjectType;
class FDelegateType;
class FPrimitiveType;
class FInterfaceType;
class FEnumType;
class FStructType;
class FArrayType;
class FSetType;
class FMapType;

class ITypeVisitor
{
public:
    virtual void Visit(const FPrimitiveType& Type) = 0;

    virtual void Visit(const FStructType& Type) = 0;

    virtual void Visit(const FEnumType& Type) = 0;

    virtual void Visit(const FArrayType& Type) = 0;

    virtual void Visit(const FSetType& Type) = 0;

    virtual void Visit(const FMapType& Type) = 0;

    virtual void Visit(const FInterfaceType& Type) = 0;

    virtual void Visit(const FObjectType& Type) = 0;

    virtual void Visit(const FDelegateType& Type) = 0;

    virtual ~ITypeVisitor() = default;
};
