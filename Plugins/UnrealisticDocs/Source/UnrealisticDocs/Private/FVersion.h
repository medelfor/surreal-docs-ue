/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

struct FVersion
{
    int Major = 0;
    int Minor = 0;
    int Index = 0;
    FString Tag;
    int32 Build = 0;
    FString BuildId;
    int64 Timestamp = 0;
};
