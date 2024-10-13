// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once
#include "DlgObject.h"

#include "DlgNodeData.generated.h"

// Abstract base class for node data
// Defining class via inheriting from UDlgNodeData outside of the plugin is possible both in Blueprint and C++
// Extend this class to define additional data you want to store on your nodes
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class DLGSYSTEM_API UDlgNodeData : public UDlgObject
{
	GENERATED_BODY()

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dialogue")
	FString GetEditorDisplayString(UDlgDialogue* OwnerDialogue) const;

	virtual FString GetEditorDisplayString_Implementation(UDlgDialogue* OwnerDialogue) const
	{
#if WITH_EDITOR
		return GetClass()->GetDisplayNameText().ToString();
#else
		return GetName();
#endif
	}
	// Torbie End Change
	//-----------------------------------------------------------------------------
};

// This is the same as UDlgNodeData but it does NOT show any categories
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew, CollapseCategories)
class DLGSYSTEM_API UDlgNodeDataHideCategories : public UDlgNodeData
{
	GENERATED_BODY()
};
