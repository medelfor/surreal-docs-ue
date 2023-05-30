/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ISourceEnumerator.h"
#include "AssetData.h"


class UNREALISTICDOCS_API FContentPathEnumerator : public ISourceEnumerator
{
public:
    explicit FContentPathEnumerator(FName const& InPath);

public:
    UObject* GetNext() override;
    float EstimateProgress() const override;
    int32 EstimatedSize() const override;

protected:
    void Prepass(FName const& Path);

protected:
    TArray<FAssetData> AssetList;
    int32 CurIndex;
};
