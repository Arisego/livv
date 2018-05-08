// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include <Text.h>

class ISlateStyle;
class UCubismModel;


/**
 * Implements the UCubismModel asset editor widget.
 */
class SCubismAssetEditor
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCubismAssetEditor) { }
	SLATE_END_ARGS()

public:

	/** Virtual destructor. */
	virtual ~SCubismAssetEditor();

	/**
	 * Construct this widget
	 *
	 * @param InArgs The declaration data for this widget.
	 * @param InCubismModel The UCubismModel asset to edit.
	 * @param InStyleSet The style set to use.
	 */
	void Construct(const FArguments& InArgs, UCubismModel* InCubismModel, const TSharedRef<ISlateStyle>& InStyle);

private:

	/** Callback for text changes in the editable text box. */
	void HandleEditableTextBoxTextChanged(const FText& NewText);

	/** Callback for committed text in the editable text box. */
	void HandleEditableTextBoxTextCommitted(const FText& Comment, ETextCommit::Type CommitType);

	/** Callback for property changes in the text asset. */
	void HandleCubismModelPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent);

private:

	/** Holds the editable text box widget. */
	TSharedPtr<SMultiLineEditableTextBox> EditableTextBox;

	/** Pointer to the text asset that is being edited. */
	UCubismModel* CubismModel;
};
