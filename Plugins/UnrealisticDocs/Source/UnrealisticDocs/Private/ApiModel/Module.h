/* Copyright © 2022, Medelför OÜ. All rights reserved. */

#pragma once

#include "Class.h"
#include "Enum.h"
#include "Identifier.h"
#include "Struct.h"

class FModule
{
public:
    const FIdentifier& GetIdentifier() const
    {
        return Identifier;
    }

    void SetIdentifier(FIdentifier NewIdentifier)
    {
        this->Identifier = MoveTemp(NewIdentifier);
    }

    const TArray<FClass*>& GetClassesView() const
    {
        return ClassesView;
    }

    void AddClass(TUniquePtr<FClass> Class)
    {
        FScopeLock Lock(&ClassesLock);

        ClassesView.Add(Class.Get());
        Classes.Add(MoveTemp(Class));
    }

    const TArray<FStruct*>& GetStructsView() const
    {
        return StructsView;
    }

    void AddStruct(TUniquePtr<FStruct> Struct)
    {
        FScopeLock Lock(&StructsLock);

        StructsView.Add(Struct.Get());
        Structs.Add(MoveTemp(Struct));
    }

    const TArray<FEnum*>& GetEnumsView() const
    {
        return EnumsView;
    }

    void AddEnum(TUniquePtr<FEnum> Enum)
    {
        FScopeLock Lock(&EnumsLock);

        EnumsView.Add(Enum.Get());
        Enums.Add(MoveTemp(Enum));
    }

private:
    FIdentifier Identifier;

    FCriticalSection ClassesLock;
    TArray<FClass*> ClassesView;
    TArray<TUniquePtr<FClass>> Classes;

    FCriticalSection StructsLock;
    TArray<FStruct*> StructsView;
    TArray<TUniquePtr<FStruct>> Structs;

    FCriticalSection EnumsLock;
    TArray<FEnum*> EnumsView;
    TArray<TUniquePtr<FEnum>> Enums;
};
