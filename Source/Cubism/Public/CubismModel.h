// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CubismModel.generated.h"

struct csmModel;
class UCanvasRenderTarget2D;
class UCubismDrawAble;
struct FModelInfo;

/**
 * Object asset for cubism model
 */
UCLASS(NotBlueprintType, hidecategories = (Object))
class CUBISM_API UCubismModel : public UObject
{
	GENERATED_BODY()
	
public:
	UCubismModel();
	~UCubismModel();


	UPROPERTY()
		FString mstr_FilePath;

	/**	<Model file data|Load from asset */
	UPROPERTY()
		TArray<uint8> ma_UserData;
	UPROPERTY()
		TArray<uint8> ma_AnimationData;
	UPROPERTY()
		TArray<uint8> ma_PhysicsData;
	
	UPROPERTY()
		TArray<uint8> ma_MocRes;

	UPROPERTY()
		uint64 mu_MocSize;


};
