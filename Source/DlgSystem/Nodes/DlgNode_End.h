// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "DlgSystem/Nodes/DlgNode.h"

#include "DlgNode_End.generated.h"


/**
 * Node ending the Dialogue.
 * Does not have text, if it is entered the Dialogue is over.
 * Events and enter conditions are taken into account.
 */
UCLASS(BlueprintType, ClassGroup = "Dialogue")
class DLGSYSTEM_API UDlgNode_End : public UDlgNode
{
	GENERATED_BODY()

public:
	// Begin UObject Interface.

	/** @return a one line description of an object. */
	FString GetDesc() override;

	// Begin UDlgNode Interface.
	bool ReevaluateChildren(UDlgContext& Context, TSet<const UDlgNode*> AlreadyEvaluated) override { return false; }
	bool OptionSelected(int32 OptionIndex, bool bFromAll, UDlgContext& Context) override { return false; }

#if WITH_EDITOR
	FString GetNodeTypeString() const override { return TEXT("End"); }
#endif

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	static FName GetMemberNameNodeData() { return GET_MEMBER_NAME_CHECKED(UDlgNode_End, NodeData); }

	UDlgNodeData* GetNodeData() const override { return NodeData; }

	// User Defined Custom Node data you can customize yourself with your own data types
	//
	// Create a new Blueprint derived from DlgNodeData (or DlgNodeDataHideCategories)
	UPROPERTY(EditAnywhere, Instanced, Category = "Dialogue|Node")
	UDlgNodeData* NodeData = nullptr;
	// Torbie End Change
	//-----------------------------------------------------------------------------
};
