// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Cubism.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "CubismBpLib.h"

#define LOCTEXT_NAMESPACE "FCubismModule"

void FCubismModule::StartupModule()
{
	UCubismBpLib::InitCubism();
}

void FCubismModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCubismModule, Cubism)