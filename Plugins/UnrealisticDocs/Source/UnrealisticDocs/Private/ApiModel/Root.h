/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Module.h"
#include "Metadata/Metadata.h"

class FRoot
{
public:
    const TArray<FModule*>& GetModulesView() const
    {
        return ModulesView;
    }

    void AddModule(TUniquePtr<FModule> Module)
    {
        this->ModulesView.Add(Module.Get());
        this->Modules.Add(MoveTemp(Module));
    }

    void SetMetadata(TUniquePtr<FMetadata> NewMetadata)
    {
        Metadata = MoveTemp(NewMetadata);
    }

    const FMetadata& GetMetadata() const
    {
        return *Metadata;
    }

private:
    TArray<FModule*> ModulesView;
    TArray<TUniquePtr<FModule>> Modules;
    TUniquePtr<FMetadata> Metadata;
};
