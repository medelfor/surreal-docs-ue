/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "JsonRequestDeserializer.h"

#include <regex>
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define NOLINT()

DEFINE_LOG_CATEGORY_STATIC(LogUnrealisticDocsJsonRequestDeserializer, All, All);

const FString FJsonRequestDeserializer::
    RequiredVersionField("required_version");
const FString FJsonRequestDeserializer::ExtractionField("extraction");

const FString FJsonRequestDeserializer::FormatField("format");
const FString FJsonRequestDeserializer::RestPortField("rest_port");
const FString FJsonRequestDeserializer::FiltersField("filters");
const FString FJsonRequestDeserializer::OutputField("output");

const FString FJsonRequestDeserializer::OutputDirectoryField("dir");
const FString FJsonRequestDeserializer::OutputDoCleanField("do_clean");

const FString FJsonRequestDeserializer::FiltersContentField("content");
const FString FJsonRequestDeserializer::
    FiltersNativeModulesField("native_modules");

const FString FJsonRequestDeserializer::FilterIncludeField("include");
const FString FJsonRequestDeserializer::FilterExcludeField("exclude");

const FString FJsonRequestDeserializer::VersionComponentSeparator(".");

const FString FJsonRequestDeserializer::JsonOutputFormat("json");

FJsonRequestDeserializer::FJsonRequestDeserializer()
{
    OutputFormats.Add(JsonOutputFormat, EOutputFormat::Json);
}

void FJsonRequestDeserializer::Deserialize(FString RequestContents)
{
    RequestJson = MakeShared<FJsonObject>();

    TSharedRef<TJsonReader<TCHAR>> JsonReader =
        TJsonReaderFactory<TCHAR>::Create(RequestContents);
    bool bSuccess = FJsonSerializer::Deserialize(JsonReader, RequestJson);

    if (!bSuccess)
    {
        RequestJson = nullptr;
    }
}

FVersion FJsonRequestDeserializer::ExtractVersion()
{
    if (!RequestJson)
    {
        UE_LOG(LogUnrealisticDocsJsonRequestDeserializer, Error,
            TEXT("No JSON were provided or it was invalid"));
        return {};
    }

    FVersion Version;
    FString StringVersion;

    if (RequestJson->TryGetStringField(RequiredVersionField, StringVersion))
    {
        std::regex VersionPattern(R"(\d+(\.\d+(\.\d+)?)?)");
        const char* Utf8StringVersion = TCHAR_TO_UTF8(*StringVersion);

        if (std::regex_match(Utf8StringVersion, VersionPattern))
        {
            TArray<FString> Components;
            StringVersion.ParseIntoArray(Components,
                *VersionComponentSeparator);

            int* VersionComponents[3] =
                {&Version.Major, &Version.Minor, &Version.Index};

            for (int i = 0; i < Components.Num(); ++i)
            {
                *VersionComponents[i] = FCString::Atoi(*Components[i]);
            }
        }
        else
        {
            UE_LOG(LogUnrealisticDocsJsonRequestDeserializer, Error,
                TEXT("Version (%s) is in invalid format"), *StringVersion);
        }
    }
    else
    {
        UE_LOG(LogUnrealisticDocsJsonRequestDeserializer, Error,
            TEXT("No field with required version (`%s`) was found"),
            *RequiredVersionField);
    }

    return Version;
}

using ObjJson = const TSharedPtr<FJsonObject>*;

#define EXTRACT_FIELD(Object, FieldName, Type, TypeName, JsonType)\
    Type FieldName##Json;\
    if (!Object->TryGet##TypeName##Field(FieldName, FieldName##Json) ||\
        !Object->HasTypedField<JsonType>(FieldName))\
    { NOLINT() \
        return nullptr;\
    }

#define EXTRACT_OBJECT_FIELD(JsonObject, FieldName)\
    EXTRACT_FIELD(JsonObject, FieldName, ObjJson, Object, EJson::Object)

#define EXTRACT_NUMBER_FIELD(JsonObject, FieldName)\
    EXTRACT_FIELD(JsonObject, FieldName, int, Number, EJson::Number)

#define EXTRACT_STRING_FIELD(JsonObject, FieldName)\
    EXTRACT_FIELD(JsonObject, FieldName, FString, String, EJson::String)

#define EXTRACT_BOOL_FIELD(JsonObject, FieldName)\
    EXTRACT_FIELD(JsonObject, FieldName, bool, Bool, EJson::Boolean)

#define EXTRACT_STRING_ARRAY_FIELD(JsonObject, FieldName)\
    EXTRACT_FIELD(JsonObject, FieldName, TArray<FString>, StringArray, \
        EJson::Array)\
    TArray<TSharedPtr<FJsonValue>> FieldName##StringArrayJson\
        = JsonObject->GetArrayField(FieldName);\
    for (auto Value : FieldName##StringArrayJson)\
    { NOLINT() \
        if (Value->Type != EJson::String)\
        { NOLINT() \
            return nullptr;\
        }\
    }

TUniquePtr<FExtractionRequest> FJsonRequestDeserializer::ExtractRequest()
{
    if (!RequestJson)
    {
        return nullptr;
    }

    TUniquePtr<FExtractionRequest> Request = MakeUnique<FExtractionRequest>();
    FExtractionSettings ExtractionSettings;

    // extraction
    EXTRACT_OBJECT_FIELD(RequestJson, ExtractionField)

    // extraction.
    EXTRACT_OBJECT_FIELD((*ExtractionFieldJson), FiltersField)
    EXTRACT_OBJECT_FIELD((*ExtractionFieldJson), OutputField)
    EXTRACT_STRING_FIELD((*ExtractionFieldJson), FormatField)
    EXTRACT_NUMBER_FIELD((*ExtractionFieldJson), RestPortField)

    // extraction.filters.
    EXTRACT_OBJECT_FIELD((*FiltersFieldJson), FiltersContentField)
    EXTRACT_OBJECT_FIELD((*FiltersFieldJson), FiltersNativeModulesField)

    {
        // extraction.filters.content.
        EXTRACT_STRING_ARRAY_FIELD((*FiltersContentFieldJson),
            FilterIncludeField)
        EXTRACT_STRING_ARRAY_FIELD((*FiltersContentFieldJson),
            FilterExcludeField)

        // put content paths into the settings
        for (FString& ContentInclude : FilterIncludeFieldJson)
        {
            ExtractionSettings.AddContentPath(
                FName(MoveTemp(ContentInclude)));
        }

        // todo: exclude
    }

    {
        // extraction.filters.native_modules.
        EXTRACT_STRING_ARRAY_FIELD((*FiltersNativeModulesFieldJson),
            FilterIncludeField)
        EXTRACT_STRING_ARRAY_FIELD((*FiltersNativeModulesFieldJson),
            FilterExcludeField)

        // put native modules paths into the settings
        for (FString& NativeModuleInclude : FilterIncludeFieldJson)
        {
            ExtractionSettings.AddNativeModule(
                FName(MoveTemp(NativeModuleInclude)));
        }

        // todo: exclude
    }

    // extraction.output.
    EXTRACT_STRING_FIELD((*OutputFieldJson), OutputDirectoryField)
    EXTRACT_BOOL_FIELD((*OutputFieldJson), OutputDoCleanField)

    // if we've made it here, then all the necessary fields are presented

    ExtractionSettings.SetRestPort(RestPortFieldJson);
    ExtractionSettings.SetOutputFormat(
        DeserializeOutputFormat(FormatFieldJson));
    ExtractionSettings.SetOutputDirectory(OutputDirectoryFieldJson);
    ExtractionSettings.SetDoCleanOutputDirectory(OutputDoCleanFieldJson);

    Request->SetExtractionSettings(MoveTemp(ExtractionSettings));

    return Request;
}

EOutputFormat FJsonRequestDeserializer::DeserializeOutputFormat(
    const FString& OutputFormat) const
{
    const EOutputFormat* OutputFormatIt = OutputFormats.Find(OutputFormat);

    return OutputFormatIt ? *OutputFormatIt : FallbackOutputFormat;
}
