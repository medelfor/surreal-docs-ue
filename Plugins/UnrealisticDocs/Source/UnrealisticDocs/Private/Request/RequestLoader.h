/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ExtractionRequest.h"
#include "RequestDeserializer.h"
#include "RequestProvider.h"

class UNREALISTICDOCS_API FRequestLoader
{
public:
    FRequestLoader(TUniquePtr<IRequestProvider> RequestProvider,
        TUniquePtr<IRequestDeserializer> RequestDeserializer);

    TUniquePtr<FExtractionRequest> LoadRequest() const;

private:
    bool CheckVersionsCompatibility(
        const FVersion& RequiredMinimalVersion) const;

    TUniquePtr<IRequestProvider> RequestProvider;
    TUniquePtr<IRequestDeserializer> RequestDeserializer;
};
