/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Metadata.h"

class IMetadataInitializer
{
public:
    virtual TUniquePtr<FMetadata> Instance() const = 0;

    virtual ~IMetadataInitializer() = default;
};
