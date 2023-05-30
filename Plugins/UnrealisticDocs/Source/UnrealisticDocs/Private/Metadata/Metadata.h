/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "FVersion.h"

class FGraphStyle
{
public:
    const FMargin& GetInputPinMargin() const
    {
        return InputPinMargin;
    }

    void SetInputPinMargin(FMargin NewInputPinMargin)
    {
        this->InputPinMargin = MoveTemp(NewInputPinMargin);
    }

    const FMargin& GetOutputPinMargin() const
    {
        return OutputPinMargin;
    }

    void SetOutputPinMargin(FMargin NewOutputPinMargin)
    {
        this->OutputPinMargin = MoveTemp(NewOutputPinMargin);
    }

    const FMargin& GetNonPinNodeBodyMargin() const
    {
        return NonPinNodeBodyMargin;
    }

    void SetNonPinNodeBodyMargin(FMargin NewNonPinNodeBodyMargin)
    {
        this->NonPinNodeBodyMargin = MoveTemp(NewNonPinNodeBodyMargin);
    }

    const FLinearColor& GetEventNodeTitleColor() const
    {
        return EventNodeTitleColor;
    }

    void SetEventNodeTitleColor(FLinearColor NewEventNodeTitleColor)
    {
        this->EventNodeTitleColor = MoveTemp(NewEventNodeTitleColor);
    }

    const FLinearColor& GetFunctionCallNodeTitleColor() const
    {
        return FunctionCallNodeTitleColor;
    }

    void SetFunctionCallNodeTitleColor(
        FLinearColor NewFunctionCallNodeTitleColor)
    {
        this->FunctionCallNodeTitleColor =
            MoveTemp(NewFunctionCallNodeTitleColor);
    }

    const FLinearColor& GetPureFunctionCallNodeTitleColor() const
    {
        return PureFunctionCallNodeTitleColor;
    }

    void SetPureFunctionCallNodeTitleColor(
        FLinearColor NewPureFunctionCallNodeTitleColor)
    {
        this->PureFunctionCallNodeTitleColor = MoveTemp(
            NewPureFunctionCallNodeTitleColor);
    }

    const FLinearColor& GetDefaultPinTypeColor() const
    {
        return DefaultPinTypeColor;
    }

    void SetDefaultPinTypeColor(FLinearColor NewDefaultPinTypeColor)
    {
        this->DefaultPinTypeColor = MoveTemp(NewDefaultPinTypeColor);
    }

    const FLinearColor& GetExecutionPinTypeColor() const
    {
        return ExecutionPinTypeColor;
    }

    void SetExecutionPinTypeColor(FLinearColor NewExecutionPinTypeColor)
    {
        this->ExecutionPinTypeColor = MoveTemp(NewExecutionPinTypeColor);
    }

    const FLinearColor& GetBooleanPinTypeColor() const
    {
        return BooleanPinTypeColor;
    }

    void SetBooleanPinTypeColor(FLinearColor NewBooleanPinTypeColor)
    {
        this->BooleanPinTypeColor = MoveTemp(NewBooleanPinTypeColor);
    }

    const FLinearColor& GetBytePinTypeColor() const
    {
        return BytePinTypeColor;
    }

    void SetBytePinTypeColor(FLinearColor NewBytePinTypeColor)
    {
        this->BytePinTypeColor = MoveTemp(NewBytePinTypeColor);
    }

    const FLinearColor& GetClassPinTypeColor() const
    {
        return ClassPinTypeColor;
    }

    void SetClassPinTypeColor(FLinearColor NewClassPinTypeColor)
    {
        this->ClassPinTypeColor = MoveTemp(NewClassPinTypeColor);
    }

    const FLinearColor& GetIntPinTypeColor() const
    {
        return IntPinTypeColor;
    }

    void SetIntPinTypeColor(FLinearColor NewIntPinTypeColor)
    {
        this->IntPinTypeColor = MoveTemp(NewIntPinTypeColor);
    }

    const FLinearColor& GetInt64PinTypeColor() const
    {
        return Int64PinTypeColor;
    }

    void SetInt64PinTypeColor(FLinearColor NewInt64PinTypeColor)
    {
        this->Int64PinTypeColor = MoveTemp(NewInt64PinTypeColor);
    }

    const FLinearColor& GetFloatPinTypeColor() const
    {
        return FloatPinTypeColor;
    }

    void SetFloatPinTypeColor(FLinearColor NewFloatPinTypeColor)
    {
        this->FloatPinTypeColor = MoveTemp(NewFloatPinTypeColor);
    }

    const FLinearColor& GetNamePinTypeColor() const
    {
        return NamePinTypeColor;
    }

    void SetNamePinTypeColor(FLinearColor NewNamePinTypeColor)
    {
        this->NamePinTypeColor = MoveTemp(NewNamePinTypeColor);
    }

    const FLinearColor& GetDelegatePinTypeColor() const
    {
        return DelegatePinTypeColor;
    }

    void SetDelegatePinTypeColor(FLinearColor NewDelegatePinTypeColor)
    {
        this->DelegatePinTypeColor = MoveTemp(NewDelegatePinTypeColor);
    }

    const FLinearColor& GetObjectPinTypeColor() const
    {
        return ObjectPinTypeColor;
    }

    void SetObjectPinTypeColor(FLinearColor NewObjectPinTypeColor)
    {
        this->ObjectPinTypeColor = MoveTemp(NewObjectPinTypeColor);
    }

    const FLinearColor& GetSoftObjectPinTypeColor() const
    {
        return SoftObjectPinTypeColor;
    }

    void SetSoftObjectPinTypeColor(FLinearColor NewSoftObjectPinTypeColor)
    {
        this->SoftObjectPinTypeColor = MoveTemp(NewSoftObjectPinTypeColor);
    }

    const FLinearColor& GetSoftClassPinTypeColor() const
    {
        return SoftClassPinTypeColor;
    }

    void SetSoftClassPinTypeColor(FLinearColor NewSoftClassPinTypeColor)
    {
        this->SoftClassPinTypeColor = MoveTemp(NewSoftClassPinTypeColor);
    }

    const FLinearColor& GetInterfacePinTypeColor() const
    {
        return InterfacePinTypeColor;
    }

    void SetInterfacePinTypeColor(FLinearColor NewInterfacePinTypeColor)
    {
        this->InterfacePinTypeColor = MoveTemp(NewInterfacePinTypeColor);
    }

    const FLinearColor& GetStringPinTypeColor() const
    {
        return StringPinTypeColor;
    }

    void SetStringPinTypeColor(FLinearColor NewStringPinTypeColor)
    {
        this->StringPinTypeColor = MoveTemp(NewStringPinTypeColor);
    }

    const FLinearColor& GetTextPinTypeColor() const
    {
        return TextPinTypeColor;
    }

    void SetTextPinTypeColor(FLinearColor NewTextPinTypeColor)
    {
        this->TextPinTypeColor = MoveTemp(NewTextPinTypeColor);
    }

    const FLinearColor& GetStructPinTypeColor() const
    {
        return StructPinTypeColor;
    }

    void SetStructPinTypeColor(FLinearColor NewStructPinTypeColor)
    {
        this->StructPinTypeColor = MoveTemp(NewStructPinTypeColor);
    }

    const FLinearColor& GetWildcardPinTypeColor() const
    {
        return WildcardPinTypeColor;
    }

    void SetWildcardPinTypeColor(FLinearColor NewWildcardPinTypeColor)
    {
        this->WildcardPinTypeColor = MoveTemp(NewWildcardPinTypeColor);
    }

    const FLinearColor& GetVectorPinTypeColor() const
    {
        return VectorPinTypeColor;
    }

    void SetVectorPinTypeColor(FLinearColor NewVectorPinTypeColor)
    {
        this->VectorPinTypeColor = MoveTemp(NewVectorPinTypeColor);
    }

    const FLinearColor& GetRotatorPinTypeColor() const
    {
        return RotatorPinTypeColor;
    }

    void SetRotatorPinTypeColor(FLinearColor NewRotatorPinTypeColor)
    {
        this->RotatorPinTypeColor = MoveTemp(NewRotatorPinTypeColor);
    }

    const FLinearColor& GetTransformPinTypeColor() const
    {
        return TransformPinTypeColor;
    }

    void SetTransformPinTypeColor(FLinearColor NewTransformPinTypeColor)
    {
        this->TransformPinTypeColor = MoveTemp(NewTransformPinTypeColor);
    }

    const FLinearColor& GetIndexPinTypeColor() const
    {
        return IndexPinTypeColor;
    }

    void SetIndexPinTypeColor(FLinearColor NewIndexPinTypeColor)
    {
        this->IndexPinTypeColor = MoveTemp(NewIndexPinTypeColor);
    }

private:
    FMargin InputPinMargin;
    FMargin OutputPinMargin;
    FMargin NonPinNodeBodyMargin;

    FLinearColor EventNodeTitleColor;
    FLinearColor FunctionCallNodeTitleColor;
    FLinearColor PureFunctionCallNodeTitleColor;

    FLinearColor DefaultPinTypeColor;
    FLinearColor ExecutionPinTypeColor;
    FLinearColor BooleanPinTypeColor;
    FLinearColor BytePinTypeColor;
    FLinearColor ClassPinTypeColor;
    FLinearColor IntPinTypeColor;
    FLinearColor Int64PinTypeColor;
    FLinearColor FloatPinTypeColor;
    FLinearColor NamePinTypeColor;
    FLinearColor DelegatePinTypeColor;
    FLinearColor ObjectPinTypeColor;
    FLinearColor SoftObjectPinTypeColor;
    FLinearColor SoftClassPinTypeColor;
    FLinearColor InterfacePinTypeColor;
    FLinearColor StringPinTypeColor;
    FLinearColor TextPinTypeColor;
    FLinearColor StructPinTypeColor;
    FLinearColor WildcardPinTypeColor;
    FLinearColor VectorPinTypeColor;
    FLinearColor RotatorPinTypeColor;
    FLinearColor TransformPinTypeColor;
    FLinearColor IndexPinTypeColor;
};

class FMetadata
{
public:
    const FVersion& GetVersion() const
    {
        return Version;
    }

    void SetVersion(FVersion NewVersion)
    {
        this->Version = MoveTemp(NewVersion);
    }

    const FGraphStyle& GetGraphStyle() const
    {
        return GraphStyle;
    }

    void SetGraphStyle(FGraphStyle NewGraphStyle)
    {
        this->GraphStyle = MoveTemp(NewGraphStyle);
    }

private:
    FVersion Version;
    FGraphStyle GraphStyle;
};
