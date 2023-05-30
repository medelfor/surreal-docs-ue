/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#include "DefaultMetadataInitializer.h"
#include "GraphEditorSettings.h"
#include "version.h"

TUniquePtr<FMetadata> FDefaultMetadataInitializer::Instance() const
{
    TUniquePtr<FMetadata> Metadata = MakeUnique<FMetadata>();

    FVersion Version;
    Version.Major = VERSION_MAJOR;
    Version.Minor = VERSION_MINOR;
    Version.Index = VERSION_INDEX;
    Version.Tag = VERSION_TAG;
    Version.Build = VERSION_BUILD;
    Version.BuildId = VERSION_BUILD_ID;
    Version.Timestamp = VERSION_BUILD_TIMESTAMP;
    Metadata->SetVersion(MoveTemp(Version));

    FGraphStyle GraphStyle;
    const UGraphEditorSettings* GraphEditorSettings =
        GetDefault<UGraphEditorSettings>();

    GraphStyle.SetInputPinMargin(GraphEditorSettings->GetInputPinPadding());
    GraphStyle.SetOutputPinMargin(GraphEditorSettings->GetOutputPinPadding());
    GraphStyle.SetNonPinNodeBodyMargin(
        GraphEditorSettings->GetNonPinNodeBodyPadding());

    GraphStyle.SetEventNodeTitleColor(GraphEditorSettings->EventNodeTitleColor);
    GraphStyle.SetFunctionCallNodeTitleColor(
        GraphEditorSettings->FunctionCallNodeTitleColor);
    GraphStyle.SetPureFunctionCallNodeTitleColor(
        GraphEditorSettings->PureFunctionCallNodeTitleColor);

    GraphStyle.SetDefaultPinTypeColor(GraphEditorSettings->DefaultPinTypeColor);
    GraphStyle.SetExecutionPinTypeColor(
        GraphEditorSettings->ExecutionPinTypeColor);
    GraphStyle.SetBooleanPinTypeColor(GraphEditorSettings->BooleanPinTypeColor);
    GraphStyle.SetBytePinTypeColor(GraphEditorSettings->BytePinTypeColor);
    GraphStyle.SetClassPinTypeColor(GraphEditorSettings->ClassPinTypeColor);
    GraphStyle.SetIntPinTypeColor(GraphEditorSettings->IntPinTypeColor);
    GraphStyle.SetInt64PinTypeColor(GraphEditorSettings->Int64PinTypeColor);
    GraphStyle.SetFloatPinTypeColor(GraphEditorSettings->FloatPinTypeColor);
    GraphStyle.SetNamePinTypeColor(GraphEditorSettings->NamePinTypeColor);
    GraphStyle.SetDelegatePinTypeColor(
        GraphEditorSettings->DelegatePinTypeColor);
    GraphStyle.SetObjectPinTypeColor(GraphEditorSettings->ObjectPinTypeColor);
    GraphStyle.SetSoftObjectPinTypeColor(
        GraphEditorSettings->SoftObjectPinTypeColor);
    GraphStyle.SetSoftClassPinTypeColor(
        GraphEditorSettings->SoftClassPinTypeColor);
    GraphStyle.SetInterfacePinTypeColor(
        GraphEditorSettings->InterfacePinTypeColor);
    GraphStyle.SetStringPinTypeColor(GraphEditorSettings->StringPinTypeColor);
    GraphStyle.SetTextPinTypeColor(GraphEditorSettings->TextPinTypeColor);
    GraphStyle.SetStructPinTypeColor(GraphEditorSettings->StructPinTypeColor);
    GraphStyle.SetWildcardPinTypeColor(
        GraphEditorSettings->WildcardPinTypeColor);
    GraphStyle.SetVectorPinTypeColor(GraphEditorSettings->VectorPinTypeColor);
    GraphStyle.SetRotatorPinTypeColor(GraphEditorSettings->RotatorPinTypeColor);
    GraphStyle.SetTransformPinTypeColor(
        GraphEditorSettings->TransformPinTypeColor);
    GraphStyle.SetIndexPinTypeColor(GraphEditorSettings->IndexPinTypeColor);

    Metadata->SetGraphStyle(MoveTemp(GraphStyle));

    return Metadata;
}
