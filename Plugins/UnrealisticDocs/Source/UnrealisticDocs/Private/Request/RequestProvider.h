/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

class IRequestProvider
{
public:
    virtual FString RetrieveContents() = 0;

    virtual ~IRequestProvider() = default;
};
