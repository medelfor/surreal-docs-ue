/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "UnrealisticDocsModule.h"

#include "Request/ExtractionSettings.h"
#include "ISettingsModule.h"

#include "Async/Async.h"

IMPLEMENT_MODULE(FUnrealisticDocsModule, UnrealisticDocs)

DEFINE_LOG_CATEGORY(LogUnrealisticDocs);

#define NOLINT()

void FUnrealisticDocsModule::StartupModule()
{
}

void FUnrealisticDocsModule::ShutdownModule()
{
}

bool FUnrealisticDocsModule::IsProcessorRunning()
{
    return bIsRunning;
}

void FUnrealisticDocsModule::SetDataExtractionOrchestrator(
    TUniquePtr<FDataExtractionOrchestrator> NewOrchestrator)
{
    Orchestrator = MoveTemp(NewOrchestrator);
}

void FUnrealisticDocsModule::ExtractUData(
    NOLINT() FExtractionSettings const& Settings)
{
    bIsRunning = true;

    Orchestrator.Get()->OnExtractingFinished().AddLambda(
        [this]()
        {
            bIsRunning = false;
        });

    Orchestrator->SetSettings(Settings);
    Orchestrator->Run();
}
