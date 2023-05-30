/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Containers/Queue.h"
#include "CoreMinimal.h"

#include "ApiModelExtractor.h"
#include "Request/ExtractionSettings.h"
#include "ApiModel/Root.h"
#include "Metadata/MetadataInitializer.h"
#include "Serialization/Flusher.h"
#include "UObject/WeakObjectPtrTemplates.h"

class ISourceEnumerator;
class UBlueprintNodeSpawner;
class FModule;

DECLARE_EVENT(FDataExtractionOrchestrator, FExtractingFinished);


// the class' instances are not supposed to be used more than once
class UNREALISTICDOCS_API FDataExtractionOrchestrator
{
public:
    FDataExtractionOrchestrator();

    void SetSettings(FExtractionSettings NewSettings);

    const FExtractionSettings& GetSettings() const;

    void SetApiModelExtractor(TUniquePtr<FApiModelExtractor> ModelExtractor);

    void AddEnumerator(TSharedPtr<ISourceEnumerator> Enumerator);

    void SetFlusher(TUniquePtr<IFlusher> NewFlusher);

    void SetMetadataInitializer(
        TUniquePtr<IMetadataInitializer> NewMetadataInitializer);

    FExtractingFinished& OnExtractingFinished()
    {
        return ExtractingFinished;
    }

    virtual void Run();

    virtual ~FDataExtractionOrchestrator() = default;

protected:
    void FireExtractingFinished();

private:
    void StartExtraction();

    FExtractingFinished ExtractingFinished;

    void GameThread_GenerateEnumeratorsFromSettings();

    TQueue<TSharedPtr<ISourceEnumerator>> Enumerators;
    TSet<FName> Excluded;
    TSet<TWeakObjectPtr<UObject>> Processed;

    // Root Lock
    TUniquePtr<FRoot> Root;
    FCriticalSection RootLock;

    TSharedPtr<ISourceEnumerator> CurrentEnumerator;

    // Has Been Flushed Lock
    volatile bool HasBeenFlushed = false;
    FCriticalSection HasBeenFlushedLock;

    volatile bool bIsStartingFinished = false;
    volatile int32 ProcessedCountOfObjects = 0;

    // Finishing Object Data Lock
    volatile int32 TotalCountOfObjects = 0;
    FCriticalSection FinishingObjectDataLock;

    void FinishObject();
    void CheckIfAllObjectsAreFinished();

    void FlushUData() const;

    bool GameThread_StartProcessingOfNextObject();

    void ProcessClass(UClass* Class);
    void ProcessStruct(UScriptStruct* Struct);
    void ProcessEnum(UEnum* Enum);

    FModule* SafeGetModule(FString SamplePath);

    FExtractionSettings Settings;

    TUniquePtr<FApiModelExtractor> DataExtractor;

    TUniquePtr<IFlusher> Flusher;
    TUniquePtr<IMetadataInitializer> MetadataInitializer;
};
