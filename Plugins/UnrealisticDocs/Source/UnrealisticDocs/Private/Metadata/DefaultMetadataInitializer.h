/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "MetadataInitializer.h"

class FDefaultMetadataInitializer : public IMetadataInitializer
{
public:
    TUniquePtr<FMetadata> Instance() const override;
};
