#pragma once

#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "Live2DCubismCore.h"

#include "CubismDrawAble.generated.h"

struct csmModel;
class UCubismViewer;
class UCanvasRenderTarget2D;

enum csmBlendType
{
	/// Normal blending mode.
	csmNormalBlending = 0,

	/// Additive blending mode.
	csmAdditiveBlending = 1,

	/// Multiplicative blending mode.
	csmMultiplicativeBlending = 2,
};

UCLASS()
class UCubismDrawAble:public UObject
{
	GENERATED_BODY()

public:

	int md_TextureIndex;
	csmBlendType ms_BlendMode;
	bool mb_IsDoubleSided;

	int VertexCount;
	TArray<FVector2D> marr_v2VertexPositions;
	TArray<FVector2D> marr_v2VertexUvs;

	int md_IndexCount;
	TArray<unsigned short> marr_uDrawIndices;

	FString ms_ID;
	int md_DrawOrder;
	float mf_Opacity;
	int md_RenderOrder;
	csmFlags ms_DynamicFlag;

	int md_MaskCount;
	TArray<int> marr_dMasks;

	/**	<Index of draw able */
	size_t mu_SequencIdx;

	bool mb_IsVisible;

	bool mb_DrawOrderChanged;
	bool mb_RenderOrderChanged;

	TArray<FCanvasUVTri> marr_sTriangles;

	FVector2D mv2_Origin;
	float mf_Scale;

	UCubismViewer* mp_Model;

public:
	/**	<Do the update job */
	void DoUpdate(csmModel* Model);

	/**	<delegate called by canvas */
	UFUNCTION()
	void On_DrawMask(UCanvas* Canvas, int32 Width, int32 Height);

	UFUNCTION()
	void On_DrawBase(UCanvas* Canvas, int32 Width, int32 Height);

	/**	<init for mask */
	void Init_Mask();

	UCubismDrawAble();

	/**	Gets */
	FORCEINLINE UMaterialInstanceDynamic* Get_MaskMat() { return mp_MaskedMat; };

private:
	FORCEINLINE void Clear_Flags();

	/**	<Update the mask base */
	void Do_UpdateMask();

	/**	<Draw the base of drawable for mask */
	UPROPERTY()
		UCanvasRenderTarget2D* mp_BaseDrawer;

	/**	<Draw the mask */
	UPROPERTY()
		UCanvasRenderTarget2D* mp_MaskDrawer;

	UPROPERTY()
		UMaterialInstanceDynamic* mp_MaskedMat;
};