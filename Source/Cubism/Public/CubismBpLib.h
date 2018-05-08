// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CubismBpLib.generated.h"

class UCubismModel;

/**
 * 
 */
UCLASS()
class CUBISM_API UCubismBpLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		static void InitCubism();

	UFUNCTION(BlueprintCallable)
		static UCubismViewer*  CreateCubism(UObject* Outer, UCubismModel* InModelData);

	UFUNCTION(BlueprintCallable)
		static bool ReadCubism(UCubismModel* Outer, const FString InFilePath);
};
