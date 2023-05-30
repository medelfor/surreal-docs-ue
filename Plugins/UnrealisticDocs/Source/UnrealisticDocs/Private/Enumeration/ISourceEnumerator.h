/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "CoreMinimal.h"


class UObject;

class ISourceEnumerator
{
public:
    virtual UObject* GetNext() = 0;
    virtual float EstimateProgress() const = 0;
    virtual int32 EstimatedSize() const = 0;

    virtual ~ISourceEnumerator()
    {
    }
};
