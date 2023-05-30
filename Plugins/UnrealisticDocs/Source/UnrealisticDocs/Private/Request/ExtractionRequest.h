/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "ExtractionSettings.h"

class FExtractionRequest
{
public:
    const FExtractionSettings& GetExtractionSettings() const
    {
        return ExtractionSettings;
    }

    void SetExtractionSettings(FExtractionSettings NewExtractionSettings)
    {
        this->ExtractionSettings = MoveTemp(NewExtractionSettings);
    }

private:
    FExtractionSettings ExtractionSettings;
};
