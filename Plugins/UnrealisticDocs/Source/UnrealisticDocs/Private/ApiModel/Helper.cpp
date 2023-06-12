/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "Helper.h"

const FString FHelper::EnumDisplayNameMetadata("DisplayName");

FString FHelper::ExtractDisplayNameOfEnum(const UEnum* Enum)
{
    if (!IsValid(Enum)) return "";

    if (Enum->HasMetaData(*EnumDisplayNameMetadata))
    {
        return Enum->GetMetaData(*EnumDisplayNameMetadata);
    }

    return FName::NameToDisplayString(Enum->GetName(), false);
}
