/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ISourceEnumerator.h"


class UNREALISTICDOCS_API FNativeModuleEnumerator : public ISourceEnumerator
{
public:
    FNativeModuleEnumerator(
        FName const& InModuleName);

public:
    UObject* GetNext() override;
    float EstimateProgress() const override;
    int32 EstimatedSize() const override;

protected:
    void Prepass(FName const& ModuleName);

protected:
    TArray<TWeakObjectPtr<UObject>> ObjectList;
    int32 CurIndex;
};
