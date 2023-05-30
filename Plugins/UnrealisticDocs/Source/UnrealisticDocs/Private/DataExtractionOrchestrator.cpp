/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "DataExtractionOrchestrator.h"
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"
#include "Enumeration/CompositeEnumerator.h"
#include "Enumeration/ContentPathEnumerator.h"
#include "Enumeration/ISourceEnumerator.h"
#include "Enumeration/NativeModuleEnumerator.h"
#include "UnrealisticDocsModule.h"
#include "ApiModelExtractor.h"
#include "Serialization/FileFlusher.h"
#include "Serialization/JsonUDataSerializer.h"

FDataExtractionOrchestrator::FDataExtractionOrchestrator()
{
    Root = MakeUnique<FRoot>();
}

void FDataExtractionOrchestrator::SetSettings(
    FExtractionSettings NewSettings)
{
    Settings = MoveTemp(NewSettings);
}

const FExtractionSettings& FDataExtractionOrchestrator::GetSettings() const
{
    return Settings;
}

void FDataExtractionOrchestrator::SetApiModelExtractor(
    TUniquePtr<FApiModelExtractor> ModelExtractor)
{
    DataExtractor = MoveTemp(ModelExtractor);
}

void FDataExtractionOrchestrator::AddEnumerator(
    TSharedPtr<ISourceEnumerator> Enumerator)
{
    Enumerators.Enqueue(Enumerator);
}

void FDataExtractionOrchestrator::SetFlusher(TUniquePtr<IFlusher> NewFlusher)
{
    this->Flusher = MoveTemp(NewFlusher);
}

void FDataExtractionOrchestrator::SetMetadataInitializer(
    TUniquePtr<IMetadataInitializer> NewMetadataInitializer)
{
    MetadataInitializer = MoveTemp(NewMetadataInitializer);
}

void FDataExtractionOrchestrator::Run()
{
    AsyncThread(
        [this]()
        {
            StartExtraction();
        });
}

void FDataExtractionOrchestrator::FireExtractingFinished()
{
    ExtractingFinished.Broadcast();
}

void FDataExtractionOrchestrator::StartExtraction()
{
    TFunction<void()> GameThread_EnqueueEnumerators =
        [this]()
        {
            GameThread_GenerateEnumeratorsFromSettings();
        };

    // run on Game Thread
    Async(EAsyncExecution::TaskGraphMainThread,
        GameThread_EnqueueEnumerators).Get();

    // enumerate all objects of every enumerator
    while (Enumerators.Dequeue(CurrentEnumerator))
    {
        TFunction<bool()> NextObject =
            [this]()
            {
                return GameThread_StartProcessingOfNextObject();
            };

        // when .Get() returns true, it means the required UObjects were
        // loaded and extraction of data out of them has been started, we can
        // move to the next object
        while (Async(EAsyncExecution::TaskGraphMainThread,
            NextObject).Get()) {}
    }

    // so the flush would not happen until all objects got counted
    bIsStartingFinished = true;

    // if all the threads that have been started above are already finished
    // this check will call flush and ExtractingFinished.Broadcast()
    CheckIfAllObjectsAreFinished();
}

void FDataExtractionOrchestrator::GameThread_GenerateEnumeratorsFromSettings()
{
    if (!Enumerators.IsEmpty()) return;

    // TODO(artsiom.drapun): Specific class enumerator
    Enumerators.Enqueue(
        MakeShared<FCompositeEnumerator<FNativeModuleEnumerator>>(
            Settings.GetNativeModules()));

    TArray<FName> ContentPackagePaths;
    for (auto const& Path : Settings.GetContentPaths())
    {
        ContentPackagePaths.AddUnique(Path);
    }
    Enumerators.Enqueue(
        MakeShared<FCompositeEnumerator<FContentPathEnumerator>>(
            ContentPackagePaths));
}

void FDataExtractionOrchestrator::FinishObject()
{
    FScopeLock Lock(&FinishingObjectDataLock);

    ++ProcessedCountOfObjects;

    UE_LOG(LogUnrealisticDocs, Display, TEXT("%d/%d objects are processed"),
        ProcessedCountOfObjects, TotalCountOfObjects);

    CheckIfAllObjectsAreFinished();
}

void FDataExtractionOrchestrator::CheckIfAllObjectsAreFinished()
{
    FScopeLock Lock(&HasBeenFlushedLock);

    if (ProcessedCountOfObjects == TotalCountOfObjects &&
        bIsStartingFinished && !HasBeenFlushed)
    {
        // so in case when the last object got processed right after
        // bIsStartingFinished was set to true (in ProcessTask), the flush
        // would not happen twice

        HasBeenFlushed = true;

        UE_LOG(LogUnrealisticDocs, Display,
            TEXT("All UData is extracted, flushing..."));

        Async(EAsyncExecution::Thread,
        [this]()
            {
                Root->SetMetadata(MetadataInitializer->Instance());
                FlushUData();

                FireExtractingFinished();
            });
    }
}

void FDataExtractionOrchestrator::FlushUData() const
{
    Flusher->Flush(*Root, Settings.GetOutputDirectory(),
                   Settings.DoCleanOutputDirectory());
}

bool FDataExtractionOrchestrator::GameThread_StartProcessingOfNextObject()
{
    while (auto Obj = CurrentEnumerator->GetNext())
    {
        if (!Obj || Processed.Contains(Obj))
        {
            continue;
        }

        FSoftObjectPath ObjectPath(Obj);
        bool bIsRecognized = false;

        // determine type: class/blueprint, struct, enum
        UClass* Class = Cast<UClass>(Obj);
        UBlueprint* Blueprint = Cast<UBlueprint>(Obj);

        // todo(artsiom.drapun): add ability to add more supported types w/o
        // changes to the code below
        if (Class || Blueprint)
        {
            if (Blueprint)
            {
                // preload BlueprintGeneratedClass if the Obj is a BP
                UE_LOG(LogUnrealisticDocs, Display,
                    TEXT("Found Blueprint '%s'"), *ObjectPath.ToString());

                Class = LoadClass<UObject>(NULL,
                    *(ObjectPath.ToString() + TEXT("_C")));
            }
            else
            {
                // requirement of FApiModelExtractor
                Class->SetUpRuntimeReplicationData();

                UE_LOG(LogUnrealisticDocs, Display,
                    TEXT("Found native class '%s'"), *ObjectPath.ToString());
            }

            ++TotalCountOfObjects;
            bIsRecognized = true;

            Class->AddToRoot();
            ProcessClass(Class);
        }
        else if (UScriptStruct* Struct = Cast<UScriptStruct>(Obj))
        {
            UE_LOG(LogUnrealisticDocs, Display,
                    TEXT("Found struct '%s'"), *ObjectPath.ToString());

            ++TotalCountOfObjects;
            bIsRecognized = true;

            Struct->AddToRoot();
            ProcessStruct(Struct);
        }
        else if (UEnum* Enum = Cast<UEnum>(Obj))
        {
            UE_LOG(LogUnrealisticDocs, Display,
                    TEXT("Found enum '%s'"), *ObjectPath.ToString());

            ++TotalCountOfObjects;
            bIsRecognized = true;

            Enum->AddToRoot();
            ProcessEnum(Enum);
        }

        if (bIsRecognized)
        {
            Processed.Add(Obj);
        }

        return true;
    }

    // this enumerator is finished
    return false;
}

void FDataExtractionOrchestrator::ProcessClass(UClass* Class)
{
    Async(EAsyncExecution::LargeThreadPool,
        [this, Class]()
        {
            TUniquePtr<FClass> ExtractedClass =
                DataExtractor.Get()->ExtractClass(Class);

            // get/create module then add the object to the module
            FModule* Module = SafeGetModule(Class->GetPathName());
            Module->AddClass(MoveTemp(ExtractedClass));

            // free the memory used by Class
            AsyncTask(ENamedThreads::GameThread,
                [Class]()
                {
                    Class->RemoveFromRoot();
                });

            // notify the orchestrator that the object was processed
            FinishObject();
        });
}

void FDataExtractionOrchestrator::ProcessStruct(UScriptStruct* Struct)
{
    Async(EAsyncExecution::LargeThreadPool,
        [this, Struct]()
        {
            TUniquePtr<FStruct> ExtractedStruct =
                DataExtractor.Get()->ExtractStruct(Struct);

            // get/create module then add the object to the module
            FModule* Module = SafeGetModule(Struct->GetPathName());
            Module->AddStruct(MoveTemp(ExtractedStruct));

            // free the memory used by Struct
            AsyncTask(ENamedThreads::GameThread,
                [Struct]()
                {
                    Struct->RemoveFromRoot();
                });

            // notify the orchestrator that the object was processed
            FinishObject();
        });
}

void FDataExtractionOrchestrator::ProcessEnum(UEnum* Enum)
{
    Async(EAsyncExecution::LargeThreadPool,
        [this, Enum]()
        {
            TUniquePtr<FEnum> ExtractedEnum =
                DataExtractor.Get()->ExtractEnum(Enum);

            // get/create module then add the object to the module
            FModule* Module = SafeGetModule(Enum->GetPathName());
            Module->AddEnum(MoveTemp(ExtractedEnum));

            // free the memory used by Enum
            AsyncTask(ENamedThreads::GameThread,
                [Enum]()
                {
                    Enum->RemoveFromRoot();
                });

            // notify the orchestrator that the object was processed
            FinishObject();
        });
}

FModule* FDataExtractionOrchestrator::SafeGetModule(FString SamplePath)
{
    FScopeLock Lock(&RootLock);

    if (!SamplePath.EndsWith("/"))
    {
        SamplePath += "/";
    }

    for (FModule* Module : Root->GetModulesView())
    {
        if (SamplePath.StartsWith(Module->GetIdentifier().GetFullPath()))
        {
            return Module;
        }
    }

    TUniquePtr<FModule> NewModule =
        DataExtractor.Get()->ExtractModule(SamplePath);
    FModule* ToReturn = NewModule.Get();

    Root->AddModule(MoveTemp(NewModule));

    return ToReturn;
}
