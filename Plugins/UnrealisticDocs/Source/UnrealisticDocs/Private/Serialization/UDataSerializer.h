/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class FRoot;

class IUDataSerializer
{
public:
    virtual FString Serialize(const FRoot& Root) = 0;

    virtual ~IUDataSerializer() = default;
};
