// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SCubismEditor.h"

#include "Fonts/SlateFontInfo.h"
#include "Internationalization/Text.h"
#include "CubismModel.h"
#include "UObject/Class.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

#include "CubismEditorSettings.h"


#define LOCTEXT_NAMESPACE "SCubismAssetEditor"


/* SCubismAssetEditor interface
 *****************************************************************************/

SCubismAssetEditor::~SCubismAssetEditor()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}


void SCubismAssetEditor::Construct(const FArguments& InArgs, UCubismModel* InCubismModel, const TSharedRef<ISlateStyle>& InStyle)
{
	CubismModel = InCubismModel;

	auto Settings = GetDefault<UCubismEditorSettings>();

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SAssignNew(EditableTextBox, SMultiLineEditableTextBox)
					.BackgroundColor((Settings != nullptr) ? Settings->BackgroundColor : FLinearColor::White)
					.Font((Settings != nullptr) ? Settings->Font : FSlateFontInfo())
					.ForegroundColor((Settings != nullptr) ? Settings->ForegroundColor : FLinearColor::Black)
					.Margin((Settings != nullptr) ? Settings->Margin : 4.0f)
					.OnTextChanged(this, &SCubismAssetEditor::HandleEditableTextBoxTextChanged)
					.OnTextCommitted(this, &SCubismAssetEditor::HandleEditableTextBoxTextCommitted)
					.Text(FText::FromString(TEXT("TODO")))
			]
	];

	FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &SCubismAssetEditor::HandleCubismModelPropertyChanged);
}


/* SCubismAssetEditor callbacks
 *****************************************************************************/

void SCubismAssetEditor::HandleEditableTextBoxTextChanged(const FText& NewText)
{
	CubismModel->MarkPackageDirty();
}


void SCubismAssetEditor::HandleEditableTextBoxTextCommitted(const FText& Comment, ETextCommit::Type CommitType)
{
	//CubismModel->Text = EditableTextBox->GetText();
}


void SCubismAssetEditor::HandleCubismModelPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Object == CubismModel)
	{
		//EditableTextBox->SetText(CubismModel->Text);
	}
}


#undef LOCTEXT_NAMESPACE
