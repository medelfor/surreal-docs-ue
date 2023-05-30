/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

enum class EOutputFormat
{
    Json
};

class FExtractionSettings
{
public:
    void AddNativeModule(FName NativeModule)
    {
        NativeModules.Add(MoveTemp(NativeModule));
    }

    const TArray<FName>& GetNativeModules() const
    {
        return NativeModules;
    }

    void AddContentPath(FName ContentPath)
    {
        ContentPaths.Add(MoveTemp(ContentPath));
    }

    const TArray<FName>& GetContentPaths() const
    {
        return ContentPaths;
    }

    const FString& GetOutputDirectory() const
    {
        return OutputDirectory;
    }

    void SetOutputDirectory(FString NewOutputDirectory)
    {
        this->OutputDirectory = MoveTemp(NewOutputDirectory);
    }

    bool DoCleanOutputDirectory() const
    {
        return bCleanOutputDirectory;
    }

    void SetDoCleanOutputDirectory(bool bNewCleanOutputDirectory)
    {
        this->bCleanOutputDirectory = bNewCleanOutputDirectory;
    }

    int32 GetRestPort() const
    {
        return RestPort;
    }

    void SetRestPort(int32 NewRestPort)
    {
        this->RestPort = NewRestPort;
    }

    EOutputFormat GetOutputFormat() const
    {
        return OutputFormat;
    }

    void SetOutputFormat(EOutputFormat NewOutputFormat)
    {
        this->OutputFormat = NewOutputFormat;
    }

private:
    TArray<FName> NativeModules;
    TArray<FName> ContentPaths;
    FString OutputDirectory;
    bool bCleanOutputDirectory = false;
    int32 RestPort = 0;
    EOutputFormat OutputFormat = EOutputFormat::Json;
};
