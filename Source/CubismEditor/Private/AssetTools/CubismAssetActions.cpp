// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CubismAssetActions.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/SlateStyle.h"

//#include "CubismAssetEditorToolkit.h"

#include "CubismModel.h"
#include "CubismEditorToolkit.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"


/* FCubismAssetActions constructors
 *****************************************************************************/

FCubismAssetActions::FCubismAssetActions(const TSharedRef<ISlateStyle>& InStyle)
	: Style(InStyle)
{ }


/* FAssetTypeActions_Base overrides
 *****************************************************************************/

bool FCubismAssetActions::CanFilter()
{
	return true;
}


void FCubismAssetActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);

	auto CubismModels = GetTypedWeakObjectPtrs<UCubismModel>(InObjects);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("CubismAsset_ReverseText", "Reverse Text"),
		LOCTEXT("CubismAsset_ReverseTextToolTip", "Reverse the text stored in the selected text asset(s)."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateLambda([=]{
				for (auto& CubismModel : CubismModels)
				{
					if (CubismModel.IsValid())
					{
						//CubismAsset->PostEditChange();
						//CubismAsset->MarkPackageDirty();
						//UE_LOG(LogTemp, Log, TEXT("Actor reserve text, test %s"), CubismAsset->GetName());
					}
				}
			}),
			FCanExecuteAction::CreateLambda([=] {
				return true;
			})
		)
	);
}


uint32 FCubismAssetActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}


FText FCubismAssetActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_CubismModel", "Cubism Model");
}


UClass* FCubismAssetActions::GetSupportedClass() const
{
	return UCubismModel::StaticClass();
}


FColor FCubismAssetActions::GetTypeColor() const
{
	return FColor::White;
}


bool FCubismAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}


void FCubismAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
		? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto CubismAsset = Cast<UCubismModel>(*ObjIt);

		if (CubismAsset != nullptr)
		{
			TSharedRef<FCubismAssetEditorToolkit> EditorToolkit = MakeShareable(new FCubismAssetEditorToolkit(Style));
			EditorToolkit->Initialize(CubismAsset, Mode, EditWithinLevelEditor);
		}
	}
}


#undef LOCTEXT_NAMESPACE
