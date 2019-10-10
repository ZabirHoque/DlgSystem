// Copyright 2017-2018 Csaba Molnar, Daniel Butum
#pragma once

#include "CoreTypes.h"

#include "DlgCondition.h"
#include "DlgEvent.h"
#include "DlgTextArgument.h"

#include "DlgEdge.generated.h"


class UDlgContextInternal;
class UDlgNode;

/**
 * The representation of a child in a node. Defined by a TargetIndex which points to the index array in the Dialogue.Nodes
 */
USTRUCT(BlueprintType, Blueprintable)
struct DLGSYSTEM_API FDlgEdge
{
	GENERATED_USTRUCT_BODY()

public:
	bool operator==(const FDlgEdge& Other) const
	{
		return TargetIndex == Other.TargetIndex &&
			Text.EqualTo(Other.Text) &&
			Conditions == Other.Conditions;
	}

	bool operator!=(const FDlgEdge& Other) const
	{
		return !(*this == Other);
	}

	// Operator overload for serialization
	friend FArchive& operator<<(FArchive &Ar, FDlgEdge& DlgEdge);

	/** Creates a simple edge without text, without conditions */
	FDlgEdge(int32 InTargetIndex = INDEX_NONE) : TargetIndex(InTargetIndex) {}

	// Rebuilds TextArguments
	void RebuildTextArgumentsArray() { FDlgTextArgument::UpdateTextArgumentArray(Text, TextArguments); }

	/** Returns with true if every condition attached to the edge and every enter condition of the target node are satisfied */
	bool Evaluate(const UDlgContextInternal* DlgContext, TSet<const UDlgNode*> AlreadyVisitedNodes) const;

	/** Constructs the ConstructedText. */
	void ConstructTextFromArguments(const UDlgContextInternal* DlgContext, FName NodeOwnerName);

	const TArray<FDlgTextArgument>& GetTextArguments() const { return TextArguments; }
	
	// Sets the text and rebuilds the formatted constructed text
	void SetText(const FText& NewText)
	{
		SetRawText(NewText);
		RebuildTextArgumentsArray();
	}

	// Sets the unformatted text, this is the text that includes the {identifier}
	// NOTE: this is not call RebuildTextArgumentsArray(), use SetText for that
	void SetRawText(const FText& NewText)
	{
		Text = NewText;
	}
	
	/** Gets the edge text. Empty text, or Text formatted with the text arguments if there is any, when the parent node is entered. */
	const FText& GetText() const
	{
		if (TextArguments.Num() > 0 && !ConstructedText.IsEmpty())
		{
			return ConstructedText;
		}
		
		return Text;
	}

	// This always returns the unformatted text, if you want the formatted text use GetText()
	const FText& GetRawText() const { return Text; }
	
	/** Returns if the Edge is valid, has the TargetIndex non negative  */
	bool IsValid() const
	{
		return TargetIndex > INDEX_NONE;
	}

	static const FDlgEdge& GetInvalidEdge();

	/** Helper functions to get the names of some properties. Used by the DlgSystemEditor module. */
	static FName GetMemberNameText() { return GET_MEMBER_NAME_CHECKED(FDlgEdge, Text); }
	static FName GetMemberNameTextArguments() { return GET_MEMBER_NAME_CHECKED(FDlgEdge, TextArguments); }
	
public:
	/** Index of the node in the Nodes TArray of the dialogue this edge is leading to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DialogueEdgeData, Meta = (ClampMin = -1))
	int32 TargetIndex = INDEX_NONE;

	/** Required but not sufficient conditions - target node's enter conditions are checked too */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DialogueEdgeData)
	TArray<FDlgCondition> Conditions;

	/** player emotion/state attached to this player choice */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DialogueEdgeData)
	FName SpeakerState;

	/** Set this to false in order to skip this edge in the AllChildren array (which lists both satisfied and not satisfied player choices */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DialogueEdgeData)
	bool bIncludeInAllOptionListIfUnsatisfied = true;

protected:
	// Some Variables are here to stop misuse
	
	/** Text associated with the child, can be used for user choices */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DialogueEdgeData, Meta = (MultiLine = true))
	FText Text;

	// If you want replaceable portions inside your Text nodes just add {identifier} inside it and set the value it should have at runtime.
	UPROPERTY(EditAnywhere, EditFixedSize, Category = DialogueEdgeData)
	TArray<FDlgTextArgument> TextArguments;
	
	/** Constructed at runtime from the original text and the arguments if there is any.*/
	FText ConstructedText;
};
