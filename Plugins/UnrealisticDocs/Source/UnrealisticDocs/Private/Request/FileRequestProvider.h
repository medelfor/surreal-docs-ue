/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "RequestProvider.h"

class UNREALISTICDOCS_API FFileRequestProvider : public IRequestProvider
{
public:
    explicit FFileRequestProvider(IFileManager& NewFileManager);

    void SetFileName(FString NewFileName);

    FString RetrieveContents() override;
private:
    IFileManager& FileManager;
    FString FileName;
};
