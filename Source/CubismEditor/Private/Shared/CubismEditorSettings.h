// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateColor.h"
#include "UObject/ObjectMacros.h"

#include "CubismEditorSettings.generated.h"


UCLASS(config=Editor)
class CUBISMEDITOR_API UCubismEditorSettings
	: public UObject
{
	GENERATED_BODY()

public:

	/** Color of the CubismModel editor's background. */
	UPROPERTY(config, EditAnywhere, Category=Appearance)
	FSlateColor BackgroundColor;

	/** Color of the CubismModel editor's text. */
	UPROPERTY(config, EditAnywhere, Category=Appearance)
	FSlateColor ForegroundColor;

	/** The font to use in the CubismModel editor window. */
	UPROPERTY(config, EditAnywhere, Category=Appearance)
	FSlateFontInfo Font;

	/** The margin around the CubismModel editor window (in pixels). */
	UPROPERTY(config, EditAnywhere, Category=Appearance)
	float Margin;

public:

	/** Default constructor. */
	UCubismEditorSettings();
};
