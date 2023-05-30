/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "ContentPathEnumerator.h"
#include "UnrealisticDocsModule.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "AssetTypeCategories.h"
#include "Engine/Blueprint.h"
#include "Animation/AnimBlueprint.h"

#define ISUE5_1 ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1

FContentPathEnumerator::FContentPathEnumerator(
    FName const& InPath)
{
    CurIndex = 0;

    FName Path = InPath;
    FString StringPath = Path.ToString();
    if (StringPath.EndsWith("/"))
    {
        Path = FName(StringPath.Left(StringPath.Len() - 1));
    }

    Prepass(Path);
}

void FContentPathEnumerator::Prepass(FName const& Path)
{
    auto& AssetRegistryModule = FModuleManager::GetModuleChecked<
        FAssetRegistryModule>("AssetRegistry");
    auto& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.bRecursiveClasses = true;

#if ISUE5_1
    Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
    Filter.ClassPaths.Add(UScriptStruct::StaticClass()->GetClassPathName());
    Filter.ClassPaths.Add(UEnum::StaticClass()->GetClassPathName());
#else
    Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
    Filter.ClassNames.Add(UScriptStruct::StaticClass()->GetFName());
    Filter.ClassNames.Add(UEnum::StaticClass()->GetFName());
#endif

    // TODO(artsiom.drapun): Not sure about this, but for some reason was
    // generating docs for 'AnimInstance' itself.

#if ISUE5_1
    Filter.RecursiveClassPathsExclusionSet.Add(
        UAnimBlueprint::StaticClass()->GetClassPathName());
#else
    Filter.RecursiveClassesExclusionSet.Add(
        UAnimBlueprint::StaticClass()->GetFName());
#endif

    AssetRegistry.GetAssetsByPath(Path, AssetList, true);
    AssetRegistry.RunAssetsThroughFilter(AssetList, Filter);
}

UObject* FContentPathEnumerator::GetNext()
{
    UObject* Result = nullptr;

    while (CurIndex < AssetList.Num())
    {
        auto const& AssetData = AssetList[CurIndex];
        ++CurIndex;

        Result = AssetData.GetAsset();

        if (Result)
        {
            FString Path;
#if ISUE5_1
            Path = AssetData.GetSoftObjectPath().ToString();
#else
            Path = AssetData.ObjectPath.ToString();
#endif

            UE_LOG(LogUnrealisticDocs, Log,
               TEXT("Enumerating object '%s' at '%s'"),
               *Result->GetName(), *Path);

            break;
        }
    }

    return Result;
}

float FContentPathEnumerator::EstimateProgress() const
{
    return static_cast<float>(CurIndex / (AssetList.Num() - 1));
}

int32 FContentPathEnumerator::EstimatedSize() const
{
    return AssetList.Num();
}
