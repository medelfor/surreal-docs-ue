/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class FRoot;

class IFlusher
{
public:
    virtual void Flush(const FRoot& Root, const FString& OutputDirectory,
                       bool bDoClean = true) = 0;

    virtual ~IFlusher() = default;
};
