// Fill out your copyright notice in the Description page of Project Settings.

#include "CubismAssetFactory.h"
#include "CubismModel.h"
#include "CubismBpLib.h"



UCubismAssetFactory::UCubismAssetFactory()
{
	Formats.Add(FString(TEXT("moc3;")) + NSLOCTEXT("UCubismModel", "CubismModel", "Model of cubism").ToString());
	SupportedClass = UCubismModel::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject* UCubismAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	//FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);
	bOutOperationCanceled = false;

	UCubismModel* tp_CreatedModel = NewObject<UCubismModel>(InParent, InClass, InName, Flags); 
	if (!UCubismBpLib::ReadCubism(tp_CreatedModel, Filename))
	{
		return nullptr;
	}

	return tp_CreatedModel;
}

