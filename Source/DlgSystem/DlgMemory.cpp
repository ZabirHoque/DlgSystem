// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgMemory.h"
#include "DlgHelper.h"


#if 0
void FDlgHistory::Add(int32 NodeIndex, const FGuid& NodeGUID)
{
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
#if 0
	if (NodeIndex >= 0)
	{
		VisitedNodeIndices.Add(NodeIndex);
	}
#endif
	// Torbie End Change
	//-----------------------------------------------------------------------------

	if (NodeGUID.IsValid())
	{
		VisitedNodeGUIDs.Add(NodeGUID);
	}
}

bool FDlgHistory::Contains(int32 NodeIndex, const FGuid& NodeGUID) const
{
	// Use GUID
	if (CanUseGUIDForSearch() && NodeGUID.IsValid())
	{
		return VisitedNodeGUIDs.Contains(NodeGUID);
	}

	// FallBack to Node Index
	return VisitedNodeIndices.Contains(NodeIndex);
}

bool FDlgHistory::operator==(const FDlgHistory& Other) const
{
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	bool bCanUseGUID       = CanUseGUIDForSearch();
	bool bCanUseGUID_Other = Other.CanUseGUIDForSearch();

	if (bCanUseGUID != bCanUseGUID_Other)
	{
		return false;
	}

	if (bCanUseGUID)
	{
		return FDlgHelper::IsSetEqual(VisitedNodeGUIDs, Other.VisitedNodeGUIDs);
	}

	return FDlgHelper::IsSetEqual(VisitedNodeIndices, Other.VisitedNodeIndices);
	// Torbie End Change
	//-----------------------------------------------------------------------------
}

FDlgNodeSavedData& FDlgHistory::GetNodeData(const FGuid& NodeGUID)
{
	return NodeData.FindOrAdd(NodeGUID);
}

#else

#include "Net/UnrealNetwork.h"

void FDlgNodeSavedData::Add(
	const FGuid& NodeGUID
	)
{
	if (NodeGUID.IsValid())
	{
		GUIDList.AddUnique(NodeGUID);
	}
}

void FDlgHistory::Add(
	int32 NodeIndex,
	const FGuid& NodeGUID
	)
{
	if (NodeGUID.IsValid())
	{
		VisitedNodeGUIDs.AddUnique(NodeGUID);
	}
}

bool FDlgHistory::Contains(
	int32 NodeIndex,
	const FGuid& NodeGUID
	) const
{
	return NodeGUID.IsValid() ? VisitedNodeGUIDs.Contains(NodeGUID) : false;
}

FDlgNodeSavedData& FDlgHistory::GetNodeData(
	const FGuid& NodeGUID
	)
{
	FDlgNodeSavedData* foundSavedData = Algo::FindBy(VisitedNodeDatas, NodeGUID, &FDlgNodeSavedData::ParentNode);
	return foundSavedData ? *foundSavedData : VisitedNodeDatas.Emplace_GetRef(NodeGUID);
}

void UDlgMemory::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDlgMemory, DialogueHistories);
}

void UDlgMemory::Reset(
	)
{
	DialogueHistories.Empty();
}

const FDlgHistory* UDlgMemory::Find(
	const FGuid& DialogueGUID
	) const
{
	return Algo::FindBy(DialogueHistories, DialogueGUID, &FDlgHistory::ParentDlg);
}

FDlgHistory& UDlgMemory::FindOrAddEntry(
	const FGuid& DialogueGUID
	)
{
	FDlgHistory* history = Algo::FindBy(DialogueHistories, DialogueGUID, &FDlgHistory::ParentDlg);
	return history ? *history : DialogueHistories.Emplace_GetRef(DialogueGUID);
}

void UDlgMemory::SetNodeVisited(
	const FGuid& DialogueGUID,
	int32 NodeIndex,
	const FGuid& NodeGUID
	)
{
	// Add it if it does not exist already
	FDlgHistory& History = FindOrAddEntry(DialogueGUID);
	History.Add(NodeIndex, NodeGUID);
}

bool UDlgMemory::IsNodeVisited(
	const FGuid& DialogueGUID,
	int32 NodeIndex,
	const FGuid& NodeGUID
	) const
{
	const FDlgHistory* History = Find(DialogueGUID);
	return History ? History->Contains(NodeIndex, NodeGUID) : false;
}

#endif
