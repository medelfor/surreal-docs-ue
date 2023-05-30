/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ISourceEnumerator.h"


template <typename TChildEnum>
class FCompositeEnumerator : public ISourceEnumerator
{
public:
    FCompositeEnumerator(
        TArray<FName> const& InNames
    )
    {
        CurEnumIndex = 0;
        TotalSize = 0;
        Completed = 0;

        Prepass(InNames);
    }

public:
    UObject* GetNext() override
    {
        while (CurEnumIndex < ChildEnumList.Num())
        {
            if (auto Obj = ChildEnumList[CurEnumIndex]->GetNext())
            {
                return Obj;
            }
            else
            {
                Completed += ChildEnumList[CurEnumIndex]->EstimatedSize();
                ChildEnumList[CurEnumIndex].Reset();
                ++CurEnumIndex;
                continue;
            }
        }

        return nullptr;
    }

    float EstimateProgress() const override
    {
        if (CurEnumIndex < ChildEnumList.Num())
        {
            return (Completed + ChildEnumList[CurEnumIndex]->
                EstimateProgress() * ChildEnumList[CurEnumIndex]->
                EstimatedSize()) / TotalSize;
        }
        else
        {
            return 1.0f;
        }
    }

    int32 EstimatedSize() const override
    {
        return TotalSize;
    }

protected:
    void Prepass(TArray<FName> const& Names)
    {
        for (auto Name : Names)
        {
            auto Child = MakeUnique<TChildEnum>(Name);
            TotalSize += Child->EstimatedSize();

            ChildEnumList.Add(MoveTemp(Child));
        }
    }

protected:
    TArray<TUniquePtr<ISourceEnumerator>> ChildEnumList;
    int32 CurEnumIndex;
    int32 TotalSize;
    int32 Completed;
};
