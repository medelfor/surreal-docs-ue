/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "DataExtractionOrchestrator.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUnrealisticDocs, Log, All);

class FUICommandList;

/*
    Module implementation
*/
class UNREALISTICDOCS_API FUnrealisticDocsModule : public FDefaultGameModuleImpl
{
public:
    FUnrealisticDocsModule()
    {
    }

public:
    /** IModuleInterface implementation */
    void StartupModule() override;
    void ShutdownModule() override;

    /// @brief Check if we are currently processing documentation.
    /// Intended only for use in commandlets
    /// @return true if the processor is valid and currently running
    bool IsProcessorRunning();

    void SetDataExtractionOrchestrator(
        TUniquePtr<FDataExtractionOrchestrator> NewOrchestrator);

    void ExtractUData(const class FExtractionSettings& Settings);

protected:
    TUniquePtr<FDataExtractionOrchestrator> Orchestrator;

    bool bIsRunning = false;
};
