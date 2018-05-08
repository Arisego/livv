// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CubismEditorToolkit.h"

#include "Editor.h"
#include "EditorReimportHandler.h"
#include "EditorStyleSet.h"
#include "UObject/NameTypes.h"
#include "Widgets/Docking/SDockTab.h"
#include "CubismModel.h"
#include "SCubismEditor.h"
#include <Engine/CanvasRenderTarget2D.h>
#include <SSafeZone.h>
#include <SDPIScaler.h>
#include "CubsimViewer.h"

#define LOCTEXT_NAMESPACE "FCubismAssetEditorToolkit"

DEFINE_LOG_CATEGORY_STATIC(LogCubismAssetEditor, Log, All);


/* Local constants
 *****************************************************************************/

namespace CubismModelEditor
{
	static const FName AppIdentifier("CubismModelEditorApp");

	static const FName ViewportTabId("CubismEditor_Viewport");
	static const FName PropertiesTabId("CubismEditor_Properties");
}


/* FCubismAssetEditorToolkit structors
 *****************************************************************************/

FCubismAssetEditorToolkit::FCubismAssetEditorToolkit(const TSharedRef<ISlateStyle>& InStyle)
	: mp_CubismModel(nullptr)
	, Style(InStyle)
{ }


FCubismAssetEditorToolkit::~FCubismAssetEditorToolkit()
{
	FReimportManager::Instance()->OnPreReimport().RemoveAll(this);
	FReimportManager::Instance()->OnPostReimport().RemoveAll(this);

	GEditor->UnregisterForUndo(this);
}

bool FCubismAssetEditorToolkit::OnRequestClose()
{
	/**	TODO. Clean up while editor window closed */
	UE_LOG(LogTemp, Log, TEXT("FCubismAssetEditorToolkit::OnRequestClose"));

	if (mp_CubismView)
	{
		mp_CubismView->mu_IsEditing = 1;
	}
	mp_CubismView = nullptr;

	return FAssetEditorToolkit::OnRequestClose();
}

/* FCubismAssetEditorToolkit interface
 *****************************************************************************/

void FCubismAssetEditorToolkit::Initialize(UCubismModel* InCubismModel, const EToolkitMode::Type InMode, const TSharedPtr<IToolkitHost>& InToolkitHost)
{
	mp_CubismModel = InCubismModel;

	mp_CubismView = NewObject<UCubismViewer>();
	mp_CubismView->mp_CubismModel = mp_CubismModel;
	mp_CubismView->Init_CsmFromAsset();
	mp_CubismView->mu_IsEditing = 2;

	// Support undo/redo
	mp_CubismModel->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	CreateInternelWidgets();

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("Standalone_CubismModelViewer_v5")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.66f)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.1f)

				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(CubismModelEditor::ViewportTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.9f)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(CubismModelEditor::PropertiesTabId, ETabState::OpenedTab)
				->SetSizeCoefficient(0.33f)
			)
		);

	FAssetEditorToolkit::InitAssetEditor(
		InMode,
		InToolkitHost,
		CubismModelEditor::AppIdentifier,
		Layout,
		true /*bCreateDefaultStandaloneMenu*/,
		true /*bCreateDefaultToolbar*/,
		InCubismModel
	);

	RegenerateMenusAndToolbars();
}


/* FAssetEditorToolkit interface
 *****************************************************************************/

FString FCubismAssetEditorToolkit::GetDocumentationLink() const
{
	return FString(TEXT("https://github.com/ue4plugins/CubismModel"));
}


void FCubismAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_CubismModelEditor", "Cubism Model Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(CubismModelEditor::PropertiesTabId, FOnSpawnTab::CreateSP(this, &FCubismAssetEditorToolkit::HandleTabManagerSpawnTab, CubismModelEditor::PropertiesTabId))
		.SetDisplayName(LOCTEXT("CubismEditorTabName", "CubismEditor"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(CubismModelEditor::ViewportTabId, FOnSpawnTab::CreateSP(this, &FCubismAssetEditorToolkit::HandleTabManager_ViewPort, CubismModelEditor::ViewportTabId))
		.SetDisplayName(LOCTEXT("CubismEditorTabName", "CubismViewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
}


void FCubismAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(CubismModelEditor::PropertiesTabId);
	InTabManager->UnregisterTabSpawner(CubismModelEditor::ViewportTabId);
}


/* IToolkit interface
 *****************************************************************************/

FText FCubismAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Cubism Model Asset Editor");
}


FName FCubismAssetEditorToolkit::GetToolkitFName() const
{
	return FName("CubismModelEditor");
}


FLinearColor FCubismAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}


FString FCubismAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "CubismModel ").ToString();
}


/* FGCObject interface
 *****************************************************************************/

void FCubismAssetEditorToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(mp_CubismModel);
}


/* FEditorUndoClient interface
*****************************************************************************/

void FCubismAssetEditorToolkit::PostUndo(bool bSuccess)
{ }


void FCubismAssetEditorToolkit::PostRedo(bool bSuccess)
{
	PostUndo(bSuccess);
}

/* FCubismAssetEditorToolkit callbacks
 *****************************************************************************/

TSharedRef<SDockTab> FCubismAssetEditorToolkit::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (TabIdentifier == CubismModelEditor::PropertiesTabId)
	{
		TabWidget = SNew(SCubismAssetEditor, mp_CubismModel, Style);
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			TabWidget.ToSharedRef()
		];
}


TSharedRef<SDockTab> FCubismAssetEditorToolkit::HandleTabManager_ViewPort(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	check(Args.GetTabId() == CubismModelEditor::ViewportTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("TextureViewportTitle", "Viewport"))
		.TabRole(ETabRole::PanelTab)
		[
			TextureCanvas.ToSharedRef()
		];
}

void FCubismAssetEditorToolkit::CreateInternelWidgets()
{
	mp_SlateBrush = new FSlateBrush();
	mp_SlateBrush->SetResourceObject(mp_CubismView->Get_RenderTarget());
	mp_SlateBrush->ImageSize = mp_CubismView->csmCanvasSize;
	mp_SlateBrush->DrawAs = ESlateBrushDrawType::Image;

	TextureCanvas = 
		SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.5f, 0.5f))
		.Alignment(FVector2D(0.5f, 0.5f))
		.AutoSize(true)
		.ZOrder(0)
		[
			SNew(SImage)
			.Image(mp_SlateBrush)
		];

}

#undef LOCTEXT_NAMESPACE
