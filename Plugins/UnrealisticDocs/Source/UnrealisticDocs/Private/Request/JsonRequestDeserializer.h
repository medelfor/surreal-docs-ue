/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "RequestDeserializer.h"

class UNREALISTICDOCS_API FJsonRequestDeserializer : public IRequestDeserializer
{
public:
    FJsonRequestDeserializer();

    void Deserialize(FString RequestContents) override;

    FVersion ExtractVersion() override;

    TUniquePtr<FExtractionRequest> ExtractRequest() override;

private:
    //
    static const FString RequiredVersionField;
    static const FString ExtractionField;

    // extraction.
    static const FString FormatField;
    static const FString RestPortField;
    static const FString FiltersField;
    static const FString OutputField;

    // extraction.output.
    static const FString OutputDirectoryField;
    static const FString OutputDoCleanField;

    // extraction.filters.
    static const FString FiltersContentField;
    static const FString FiltersNativeModulesField;

    static const FString FilterIncludeField;
    static const FString FilterExcludeField;

    static const FString VersionComponentSeparator;

    static const FString JsonOutputFormat;

    static const EOutputFormat FallbackOutputFormat = EOutputFormat::Json;

    TMap<FString, EOutputFormat> OutputFormats;
    TSharedPtr<FJsonObject> RequestJson;

    EOutputFormat DeserializeOutputFormat(const FString& OutputFormat) const;
};
