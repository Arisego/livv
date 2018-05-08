// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CubismEditorSettings.h"
#include "Misc/Paths.h"


UCubismEditorSettings::UCubismEditorSettings()
	: BackgroundColor(FLinearColor::White)
	, ForegroundColor(FLinearColor::Black)
	, Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansMono.ttf"), 10))
	, Margin(4.0f)
{ }
