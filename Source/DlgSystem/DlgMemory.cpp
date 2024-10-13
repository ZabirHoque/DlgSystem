// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgMemory.h"
#include "DlgHelper.h"

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

