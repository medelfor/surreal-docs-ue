/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "ExtractUDataCommandlet.generated.h"

/**
 *
 */
UCLASS()
class UExtractUDataCommandlet : public UCommandlet
{
public:
    GENERATED_BODY()

    UExtractUDataCommandlet();

    int32 Main(const FString& Params) override;

private:
    static const FString RequestFileParameterName;
};
