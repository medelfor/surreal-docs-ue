/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "FileRequestProvider.h"

#include "Misc/FileHelper.h"

DEFINE_LOG_CATEGORY_STATIC(LogFileRequestProvider, All, All);

FFileRequestProvider::FFileRequestProvider(IFileManager& NewFileManager)
    : FileManager(NewFileManager) {}

FString FFileRequestProvider::RetrieveContents()
{
    FString Path = FileName;

    if (!FileManager.FileExists(*Path))
    {
        UE_LOG(LogFileRequestProvider, Error,
            TEXT("File doesn't exist or it isn't a file"));

        return "";
    }

    TUniquePtr<FArchive> Reader =
        TUniquePtr<FArchive>(FileManager.CreateFileReader(
            *Path, FILEREAD_NoFail));

    if (!Reader)
    {
        UE_LOG(LogFileRequestProvider, Error, TEXT("Unable to read the file"));

        return "";
    }

    FString Contents;
    FFileHelper::LoadFileToString(Contents, *Reader.Get());

    return Contents;
}

void FFileRequestProvider::SetFileName(FString NewFileName)
{
    FileName = MoveTemp(NewFileName);
}
