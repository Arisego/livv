// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CubsimViewer.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class CUBISM_API UCubismViewer : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CubismModel")
		void UpdateModel();

	UFUNCTION(BlueprintCallable, Category = "CubismModel")
		void RenderModel();

	UFUNCTION(BlueprintCallable, Category = "CubismModel")
		UCanvasRenderTarget2D* Get_RenderTarget();
private:
	UFUNCTION()
		void Do_PaintModel(UCanvas* Canvas, int32 Width, int32 Height);

	void UpdateCsm(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubism", DisplayName = "Model Texture")
		UTexture2D* mp_RespTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubism|Debug", DisplayName = "Canvas color")
		FLinearColor mc_BgColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubism|Debug", DisplayName = "BlendMode")
		TEnumAsByte<EBlendMode> ms_BlendMode;

public:
	csmModel * Model;

	FVector2D csmCanvasSize;

	FVector2D csmOrigPos;

	/**	<Store drawables pointer for sort */
	TArray<UCubismDrawAble*> marr_pDrawOrder;

	UPROPERTY()
		UCubismModel* mp_CubismModel;

#if WITH_EDITOR
	uint8_t	mu_IsEditing;
#endif

private:
	UPROPERTY()
		TArray<UCubismDrawAble*> marr_pDrawAbles;

	UCanvasRenderTarget2D* mp_RenderCanv;

	FModelInfo* mp_ModelInfo;

	TArray<uint8> ma_ModelRes;

public:
	UCubismViewer();
	~UCubismViewer();

	void Init_Model();

	void Init_CsmFromAsset();

	/**	<Tick object */
	virtual bool IsTickableInEditor() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	virtual void BeginDestroy() override;

	void Do_ReOrder();

	/**	Gets/Sets */
	FORCEINLINE FModelInfo*& Get_ModelInfoRef() { return mp_ModelInfo; };
	FORCEINLINE FModelInfo* Get_ModelInfo() { return mp_ModelInfo; };

	FORCEINLINE TArray<UCubismDrawAble*>& Get_DrawAblesRef();

	void Form_Model();

};
