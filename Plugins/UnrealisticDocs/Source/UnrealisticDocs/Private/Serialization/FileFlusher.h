/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Flusher.h"
#include "UDataSerializer.h"

class UNREALISTICDOCS_API FFileFlusher : public IFlusher
{
public:
    FFileFlusher(TUniquePtr<IUDataSerializer> NewSerializer,
        IFileManager& NewFileManager);

    void Flush(const FRoot& Root, const FString& OutputDirectory,
               bool bDoClean) override;

private:
    static const FString OutputFileName;

    TUniquePtr<IUDataSerializer> Serializer;
    IFileManager& FileManager;
};
