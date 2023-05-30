/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ExtractionRequest.h"
#include "FVersion.h"

class IRequestDeserializer
{
public:
    virtual void Deserialize(FString RequestContents) = 0;

    virtual FVersion ExtractVersion() = 0;

    virtual TUniquePtr<FExtractionRequest> ExtractRequest() = 0;

    virtual ~IRequestDeserializer() = default;
};
