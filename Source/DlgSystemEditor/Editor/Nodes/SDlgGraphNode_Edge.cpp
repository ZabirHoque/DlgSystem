// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "SDlgGraphNode_Edge.h"

#include "IDocumentation.h"
#include "SGraphPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Layout/SBox.h"
#include "Runtime/Launch/Resources/Version.h"
//-----------------------------------------------------------------------------
// Torbie Begin Change
#include "Framework/Text/ITextDecorator.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "DlgSystem/DlgHelper.h"
// Torbie End Change
//-----------------------------------------------------------------------------

#include "DialogueGraphNode.h"
#include "SDlgNodeOverlayWidget.h"
#include "DlgSystemEditor/DlgStyle.h"

#define LOCTEXT_NAMESPACE "DialogueEditor"

/////////////////////////////////////////////////////
// SDlgGraphNode_Edge
void SDlgGraphNode_Edge::Construct(const FArguments& InArgs, UDialogueGraphNode_Edge* InNode)
{
	Super::Construct(Super::FArguments(), InNode);
	DialogueGraphNode_Edge = InNode;

	UpdateGraphNode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin SWidget interface
void SDlgGraphNode_Edge::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// NOTE: adding the OutputPin to the hover set will not work, only the input pin makes the connection
	// to be hovered
	TSharedPtr<SGraphPanel> GraphPanel = GetOwnerPanel();
	GraphPanel->AddPinToHoverSet(DialogueGraphNode_Edge->GetInputPin());
	Super::OnMouseEnter(MyGeometry, MouseEvent);
}

void SDlgGraphNode_Edge::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	TSharedPtr<SGraphPanel> GraphPanel = GetOwnerPanel();
	GraphPanel->RemovePinFromHoverSet(DialogueGraphNode_Edge->GetInputPin());
	Super::OnMouseLeave(MouseEvent);
}
// End SWidget interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin SNodePanel::SNode Interface
void SDlgGraphNode_Edge::PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& NodeToWidgetLookup) const
{
	// Find the geometry of the nodes we're connecting
	FGeometry StartGeom;
	FGeometry EndGeom;
	static constexpr int32 NodeIndex = 0;
	static constexpr int32 NumberOfEdges = 1;

	// Get the widgets the input/output node and find the geometries and the node inde and number of edges
	UDialogueGraphNode* ParentNode = DialogueGraphNode_Edge->GetParentNode();
	UDialogueGraphNode* ChildNode = DialogueGraphNode_Edge->GetChildNode();
	const TSharedRef<SNode>* pPrevNodeWidget = NodeToWidgetLookup.Find(ParentNode);
	const TSharedRef<SNode>* pNextNodeWidget = NodeToWidgetLookup.Find(ChildNode);
	if (pPrevNodeWidget != nullptr && pNextNodeWidget != nullptr)
	{
		const TSharedRef<SNode>& PrevNodeWidget = *pPrevNodeWidget;
		const TSharedRef<SNode>& NextNodeWidget = *pNextNodeWidget;

		StartGeom = FGeometry(FVector2D(ParentNode->NodePosX, ParentNode->NodePosY), FVector2D::ZeroVector, PrevNodeWidget->GetDesiredSize(), 1.0f);
		EndGeom = FGeometry(FVector2D(ChildNode->NodePosX, ChildNode->NodePosY), FVector2D::ZeroVector, NextNodeWidget->GetDesiredSize(), 1.0f);

		// TODO do we need this? seems pointless as we can have only one edge between the same two nodes
//		// Get the input node graph edges, and filter those that only point to tour outputnode
//		TArray<UDialogueGraphNode_Edge*> GraphNode_Edges = InputNode->GetGraphNodeEdges();
//		GraphNode_Edges = GraphNode_Edges.FilterByPredicate([OutputNode](const UDialogueGraphNode_Edge* InEdge) -> bool
//		{
//			return InEdge->GetOutputNode() == OutputNode;
//		});
//		NodeIndex = GraphNode_Edges.IndexOfByKey(DialogueGraphNode_Edge);
//		NumberOfEdges = GraphNode_Edges.Num();
	}

	// Position Node
	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, NodeIndex, NumberOfEdges);
}

TArray<FOverlayWidgetInfo> SDlgGraphNode_Edge::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	static constexpr float DistanceBetweenWidgetsX = 1.0f;
	FVector2D              OriginOverlay(0.0f, 0.0f);
	// Torbie End Change
	//-----------------------------------------------------------------------------

	// This is called after PerformSecondPassLayout, so the Edge should be in it's final Position Already
	TArray<FOverlayWidgetInfo> Widgets;

	if (ConditionOverlayWidget.IsValid())
	{
		if (Settings->bShowEdgeHasConditionsIcon && DialogueGraphNode_Edge->HasConditions())
		{
			FOverlayWidgetInfo Overlay(ConditionOverlayWidget);
			// Position on the top/right of the node
			const FVector2D& NewDesiredSize = ConditionOverlayWidget->GetDesiredSize();
			Overlay.OverlayOffset = FVector2D(WidgetSize.X - NewDesiredSize.X / 2.0f, -NewDesiredSize.Y / 2.0f);
			Widgets.Add(Overlay);

			OriginOverlay.X += NewDesiredSize.X + DistanceBetweenWidgetsX;
		}
	}

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	if (TextOverlayWidget.IsValid())
	{
		if (Settings->bShowEdgeHasTextIcon && !Settings->bShowEdgeText && DialogueGraphNode_Edge->HasDialogueEdgeText())
		{
			FOverlayWidgetInfo Overlay(TextOverlayWidget);
			// Position on the top/right of the node
			const FVector2D& NewDesiredSize = TextOverlayWidget->GetDesiredSize();
			Overlay.OverlayOffset = FVector2D((WidgetSize.X - NewDesiredSize.X / 2.0f) + OriginOverlay.X, -NewDesiredSize.Y / 2.0f);
			Widgets.Add(Overlay);

			OriginOverlay.X += NewDesiredSize.X + DistanceBetweenWidgetsX;
		}
	}
	// Torbie End Change
	//-----------------------------------------------------------------------------

	return Widgets;
}
// End SNodePanel::SNode Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin SGraphNode Interface
void SDlgGraphNode_Edge::UpdateGraphNode()
{
	Super::UpdateGraphNode();

	// Fit to content
	constexpr int WidthOverride = 16;
	constexpr int HeightOverride = 16;
	ConditionOverlayWidget = SNew(SDlgNodeOverlayWidget)
		.OverlayBody(
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.WidthOverride(WidthOverride)
			.HeightOverride(HeightOverride)
			[
				SNew(SImage)
				.Image(FDlgStyle::Get()->GetBrush(FDlgStyle::PROPERTY_QuestionMarkIcon))
			]
		)
		.ToolTipText(this, &Self::GetConditionOverlayTooltipText)
		.Visibility(this, &Self::GetOverlayWidgetVisibility)
		.OnGetBackgroundColor(this, &Self::GetOverlayWidgetBackgroundColor);

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	TextOverlayWidget = SNew(SDlgNodeOverlayWidget)
		.OverlayBody(
		    SNew(STextBlock)
			.Text(FText::FromString(TEXT("T")))
			.ColorAndOpacity(FLinearColor::White)
			.Font(FNYAppStyle::GetFontStyle("BTEditor.Graph.BTNode.IndexText"))
		)
		.ToolTipText(this, &Self::GetTextOverlayTooltipText)
		.Visibility(this, &Self::GetOverlayWidgetVisibility)
		.OnGetBackgroundColor(this, &Self::GetOverlayWidgetBackgroundColor);
	// Torbie End Change
	//-----------------------------------------------------------------------------

	// Set Default tooltip
	if (!SWidget::GetToolTip().IsValid())
	{
		const TSharedRef<SToolTip> DefaultToolTip = IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &Super::GetNodeTooltip), nullptr,
			GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName());
		SetToolTip(DefaultToolTip);
	}

	TSharedPtr<SVerticalBox> EdgeTextVerticalBox;

	ContentScale.Bind(this, &Super::GetContentScale);
	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)

#if NY_ENGINE_VERSION >= 424
			// >= 4.24
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
				.ColorAndOpacity(this, &Self::GetTransitionColor)
				.Visibility(this, &Self::GetEdgeIconVisibility)
			]
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.Icon"))
				.Visibility(this, &Self::GetEdgeIconVisibility)
			]
			+SOverlay::Slot()
			[
				SNew(SBorder)
				.BorderImage(FNYAppStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
				.BorderBackgroundColor(Settings->GraphEdgeTextBackgroundColor)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Visibility(this, &Self::GetEdgeTextVisibility)
				[
					SAssignNew(EdgeTextVerticalBox, SVerticalBox)
				]
			]
#else
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.Body"))
			]
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
				.ColorAndOpacity(this, &Self::GetTransitionColor)
			]
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.Icon"))
			]
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FNYAppStyle::GetBrush("Graph.TransitionNode.Gloss"))
			]
#endif // NY_ENGINE_VERSION >= 424
		];

	CreateEventAndConditionWidgets(EdgeTextVerticalBox);

	//-----------------------------------------------------------------------------
	// Torbie Begin Change
	StyleInstance = FDlgHelper::CreateTextDecoratorStyle();

	FDlgHelper::CreateTextDecorators(TextDecorators);
	// Torbie End Change
	//-----------------------------------------------------------------------------

	EdgeTextVerticalBox->AddSlot()
	[
		//-----------------------------------------------------------------------------
		// Torbie Begin Change
		SNew(SRichTextBlock)
		.Decorators(TextDecorators)
		.DecoratorStyleSet(StyleInstance.Get())
		.Text(this, &Self::GetEdgeText)
		.WrapTextAt(Settings->GraphEdgeTextWrapAt)
		.Margin(Settings->GraphEdgeTextMargin)
		// Torbie End Change
		//-----------------------------------------------------------------------------
	];
}

//  End SGraphNode Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin own functions
void SDlgGraphNode_Edge::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	check(NodeIndex >= 0);
	check(MaxNodes > 0);

	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) / 2.0f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	static constexpr float Height = 30.0f;
	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);
	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();
	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);
	const FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// TODO do we need this?
	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.
	static constexpr float MultiNodeSpace = 0.2f; // Space between multiple edge nodes (in units of <size of node> )
	static constexpr float MultiNodeStep = 1.f + MultiNodeSpace; // Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}


const TArray<FDlgCondition>* SDlgGraphNode_Edge::GetEnterConditions() const
{
	return &DialogueGraphNode_Edge->GetDialogueEdge().Conditions;
}


FText SDlgGraphNode_Edge::GetConditionOverlayTooltipText() const
{
	return LOCTEXT("NodeConditionTooltip", "Edge has conditions.\nOnly if these conditions are satisfied then this edge is considered as an option.");
}

//-----------------------------------------------------------------------------
// Torbie Begin Change
FText SDlgGraphNode_Edge::GetTextOverlayTooltipText() const
{
	return LOCTEXT("NodeTextTooltip", "Edge contains text that can be presented.");
}
// Torbie End Change
//-----------------------------------------------------------------------------

FText SDlgGraphNode_Edge::GetEdgeText() const
{
	if (DialogueGraphNode_Edge != nullptr)
	{
		const FText EdgeText = DialogueGraphNode_Edge->GetDialogueEdge().GetText();
		if (Settings->GraphEdgeTextCharLimit <= 0)
		{
			return EdgeText;
		}

		FString AsString = EdgeText.ToString();
		if (AsString.Len() <= Settings->GraphEdgeTextCharLimit)
		{
			return EdgeText;
		}

		AsString.LeftChopInline(AsString.Len() - Settings->GraphEdgeTextCharLimit + 3);
		AsString.Append(TEXT("..."));
		return FText::FromString(AsString);
	}

	return FText::GetEmpty();
}

EVisibility SDlgGraphNode_Edge::GetOverlayWidgetVisibility() const
{
	// LOD this out once things get too small
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return !MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SDlgGraphNode_Edge::GetEdgeTextVisibility() const
{
	const bool bVisible = Settings->bShowEdgeText && !DialogueGraphNode_Edge->GetDialogueEdge().GetUnformattedText().IsEmpty();
	return bVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SDlgGraphNode_Edge::GetEdgeIconVisibility() const
{
	const bool bTextVisible = Settings->bShowEdgeText && !DialogueGraphNode_Edge->GetDialogueEdge().GetUnformattedText().IsEmpty();
	return bTextVisible ? EVisibility::Collapsed : EVisibility::Visible;
}

// End own functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
