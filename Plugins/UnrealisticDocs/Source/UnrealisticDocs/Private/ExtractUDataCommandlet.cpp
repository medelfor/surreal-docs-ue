/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "ExtractUDataCommandlet.h"

#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"
#include "Containers/Ticker.h"
#include "Containers/UnrealString.h"
#include "Request/ExtractionSettings.h"
#include "UnrealisticDocsModule.h"
#include "Metadata/DefaultMetadataInitializer.h"
#include "Modules/ModuleManager.h"
#include "Request/FileRequestProvider.h"
#include "Request/JsonRequestDeserializer.h"
#include "Request/RequestLoader.h"
#include "Serialization/FileFlusher.h"
#include "Serialization/JsonUDataSerializer.h"
#include "Serialization/UDataSerializer.h"
#include "version.h"

const FString UExtractUDataCommandlet::
    RequestFileParameterName("request-file");

UExtractUDataCommandlet::UExtractUDataCommandlet()
{
}

// todo(artsiom.drapun): isn't tested
int32 UExtractUDataCommandlet::Main(const FString& Params)
{
    Super::Main(Params);

    FString PluginVersionString = FString::Printf(TEXT("%d.%d.%d"),
        VERSION_MAJOR, VERSION_MINOR, VERSION_INDEX);

    UE_LOG(LogUnrealisticDocs, Display,
        TEXT("Welcome to Surreal Docs UE Plugin v%s, brought to you by Medelfor"
             " (https://medelfor.com)"),
             *PluginVersionString);

    TArray<FString> Tokens;
    TArray<FString> Switches;
    TMap<FString, FString> ParsedParams;
    ParseCommandLine(*Params, Tokens, Switches, ParsedParams);

    FString* RequestFileValue = ParsedParams.Find(RequestFileParameterName);

    if (!RequestFileValue)
    {
        UE_LOG(LogUnrealisticDocs, Error,
            TEXT("Provide absolute path to the request file with "
                "-request-file"));
        return 0;
    }

    TUniquePtr<FFileRequestProvider> RequestProvider =
        MakeUnique<FFileRequestProvider>(IFileManager::Get());
    RequestProvider->SetFileName(MoveTemp(*RequestFileValue));

    TUniquePtr<IRequestDeserializer> RequestDeserializer =
        MakeUnique<FJsonRequestDeserializer>();
    TUniquePtr<FRequestLoader> RequestLoader =
        MakeUnique<FRequestLoader>(MoveTemp(RequestProvider),
            MoveTemp(RequestDeserializer));

    TUniquePtr<FExtractionRequest> Request = RequestLoader->LoadRequest();

    if (!Request)
    {
        UE_LOG(LogUnrealisticDocs, Error,
            TEXT("Couldn't load request. Exiting"));
        return 0;
    }

    TUniquePtr<IUDataSerializer> Serializer;

    if (Request->GetExtractionSettings().GetOutputFormat() ==
        EOutputFormat::Json)
    {
        Serializer = MakeUnique<FJsonUDataSerializer>();
    }

    TUniquePtr<IFlusher> Flusher = MakeUnique<FFileFlusher>(
        MoveTemp(Serializer), IFileManager::Get());

    TUniquePtr<FDataExtractionOrchestrator> Orchestrator =
        MakeUnique<FDataExtractionOrchestrator>();
    Orchestrator->SetApiModelExtractor(MakeUnique<FApiModelExtractor>());
    Orchestrator->SetFlusher(MoveTemp(Flusher));
    Orchestrator->SetMetadataInitializer(
        MakeUnique<FDefaultMetadataInitializer>());

    auto& Module = FModuleManager::LoadModuleChecked<FUnrealisticDocsModule>(
        TEXT("UnrealisticDocs"));

    Module.SetDataExtractionOrchestrator(MoveTemp(Orchestrator));
    Module.ExtractUData(Request->GetExtractionSettings());
    while (Module.IsProcessorRunning())
    {
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(
            ENamedThreads::GameThread);
        FTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
        FPlatformProcess::Sleep(0);
    }

    UE_LOG(LogUnrealisticDocs, Display, TEXT("ExitCode=0"));
    return 0;
}
