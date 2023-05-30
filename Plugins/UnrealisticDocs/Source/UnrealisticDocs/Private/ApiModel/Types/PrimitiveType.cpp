﻿/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "PrimitiveType.h"

const FString FPrimitiveType::AutogeneratedBoolValue("false");
const FString FPrimitiveType::AutogeneratedRealValue("0.0");
const FString FPrimitiveType::AutogeneratedIntegralValue("0");
const FString FPrimitiveType::AutogeneratedStringValue("");
const FString FPrimitiveType::AutogeneratedTextValue("");
const FString FPrimitiveType::AutogeneratedNameValue("None");
const FString FPrimitiveType::AutogeneratedUndefinedValue("");

FString FPrimitiveType::GetAutoGeneratedValueTransliteration() const
{
    switch (PrimitiveType)
    {
        case PrimitiveType::EPrimitiveType::Float:
        case PrimitiveType::EPrimitiveType::Double:
            return AutogeneratedRealValue;

        case PrimitiveType::EPrimitiveType::Byte:
        case PrimitiveType::EPrimitiveType::Int:
        case PrimitiveType::EPrimitiveType::Int64:
            return AutogeneratedIntegralValue;

        case PrimitiveType::EPrimitiveType::Bool:
            return AutogeneratedBoolValue;

        case PrimitiveType::EPrimitiveType::String:
            return AutogeneratedStringValue;

        case PrimitiveType::EPrimitiveType::Text:
            return AutogeneratedTextValue;

        case PrimitiveType::EPrimitiveType::Name:
            return AutogeneratedNameValue;

        case PrimitiveType::EPrimitiveType::Undefined:
        default:
            return AutogeneratedUndefinedValue;
    }
}
