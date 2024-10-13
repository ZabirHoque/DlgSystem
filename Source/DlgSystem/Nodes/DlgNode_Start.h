// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "DlgSystem/Nodes/DlgNode.h"

#include "DlgNode_Start.generated.h"


/**
 * Possible entry point of the Dialogue.
 * Does not have text, the first satisfied child is picked if there is any.
 * Start nodes are evaluated from left to right.
 */
UCLASS(BlueprintType, ClassGroup = "Dialogue")
class DLGSYSTEM_API UDlgNode_Start : public UDlgNode
{
	GENERATED_BODY()

public:
	// Begin UObject Interface.

	/** @return a one line description of an object. */
	FString GetDesc() override;

#if WITH_EDITOR
	FString GetNodeTypeString() const override { return TEXT("Start"); }
#endif

	//-----------------------------------------------------------------------------
    // Torbie Begin Change
    static FName GetMemberNameNodeData()
    {
        return GET_MEMBER_NAME_CHECKED(UDlgNode_Start, NodeData);
    }

    UDlgNodeData* GetNodeData() const override
    {
        return NodeData;
    }

    // User Defined Custom Node data you can customize yourself with your own data types
    //
    // Create a new Blueprint derived from DlgNodeData (or DlgNodeDataHideCategories)
    UPROPERTY(EditAnywhere, Instanced, Category = "Dialogue|Node")
    UDlgNodeData* NodeData = nullptr;
    // Torbie End Change
    //-----------------------------------------------------------------------------
};
