/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "FileFlusher.h"

#include "Misc/FileHelper.h"

DEFINE_LOG_CATEGORY_STATIC(LogFileFlusher, All, All);

const FString FFileFlusher::OutputFileName("udata.json");

FFileFlusher::FFileFlusher(TUniquePtr<IUDataSerializer> NewSerializer,
                           IFileManager& NewFileManager)
    : Serializer(MoveTemp(NewSerializer)),
      FileManager(NewFileManager) {}

void FFileFlusher::Flush(const FRoot& Root,
                         const FString& OutputDirectory,
                         bool bDoClean)
{
    // start serializing and flushing
    // clean the dir
    if (bDoClean)
    {
        FileManager.DeleteDirectory(*OutputDirectory,
            true, true);
    }

    if (!FPaths::ValidatePath(OutputDirectory)
        || FileManager.FileExists(*OutputDirectory))
    {
        UE_LOG(LogFileFlusher, Error, TEXT("Invalid output directory"));
        return;
    }

    FString SerializedRoot = Serializer->Serialize(Root);

    FFileHelper::SaveStringToFile(SerializedRoot,
        *(OutputDirectory / OutputFileName),
        FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM, &FileManager);
}
