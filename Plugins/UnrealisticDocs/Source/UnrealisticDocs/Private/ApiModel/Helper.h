/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class FHelper
{
public:
    static const FString EnumDisplayNameMetadata;

    static FString ExtractDisplayNameOfEnum(const UEnum* Enum);
};
