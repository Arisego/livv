#include "CubismDrawAble.h"
#include "Live2DCubismCore.h"
#include "CubismModel.h"
#include <CanvasItem.h>
#include <Engine/Canvas.h>
#include <Engine/CanvasRenderTarget2D.h>
#include "CubsimViewer.h"



UCubismDrawAble::UCubismDrawAble()
	: mb_IsVisible(true)
	, mp_Model(nullptr)
	, mp_BaseDrawer(nullptr)
	, mp_MaskedMat(nullptr)
{

}

FORCEINLINE void UCubismDrawAble::Clear_Flags()
{
	mb_DrawOrderChanged = false;
	mb_RenderOrderChanged = false;
}

void UCubismDrawAble::DoUpdate(csmModel* Model)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_DRAWABLE_DoUpdate);
	Clear_Flags();
	const csmFlags* dynamicFlas = csmGetDrawableDynamicFlags(Model);
	ms_DynamicFlag = csmGetDrawableDynamicFlags(Model)[mu_SequencIdx];

	bool tb_NeedReTriangle = false;

	if (ms_DynamicFlag & csmVisibilityDidChange)
	{
		mb_IsVisible = ms_DynamicFlag & csmIsVisible;
		if (mb_IsVisible)
		{
			/**	Visibility change, recalculate triangles */
			tb_NeedReTriangle = true;
		}
	}

	if (ms_DynamicFlag & csmOpacityDidChange) 
	{
		mf_Opacity = csmGetDrawableOpacities(Model)[mu_SequencIdx];
		tb_NeedReTriangle = true;
	}

	if (ms_DynamicFlag & csmDrawOrderDidChange) 
	{
		md_DrawOrder = csmGetDrawableDrawOrders(Model)[mu_SequencIdx];
		mb_DrawOrderChanged = true;
	}

	if (ms_DynamicFlag & csmRenderOrderDidChange) 
	{
		md_RenderOrder = csmGetDrawableRenderOrders(Model)[mu_SequencIdx];
		mb_RenderOrderChanged = true;
	}

	if (ms_DynamicFlag & csmVertexPositionsDidChange) 
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_DRAWABLE_DoUpdate_ReadVertex);
		/**	Vertex */
		const size_t ti_VertCnt = csmGetDrawableVertexCounts(Model)[mu_SequencIdx];
		const csmVector2* tarr_VertPos = csmGetDrawableVertexPositions(Model)[mu_SequencIdx];
		const csmVector2* tarr_VertUv = csmGetDrawableVertexUvs(Model)[mu_SequencIdx];

		if (ti_VertCnt != VertexCount)
		{
			VertexCount = ti_VertCnt;
			marr_v2VertexPositions.SetNum(VertexCount);
			marr_v2VertexUvs.SetNum(VertexCount);
		}

		for (int i = 0; i < VertexCount; ++i)
		{
			marr_v2VertexPositions[i].X = tarr_VertPos[i].X;
			marr_v2VertexPositions[i].Y = -tarr_VertPos[i].Y;

			marr_v2VertexPositions[i] = marr_v2VertexPositions[i] * mf_Scale + mv2_Origin;

			marr_v2VertexUvs[i].X = tarr_VertUv[i].X;
			marr_v2VertexUvs[i].Y = 1 - tarr_VertUv[i].Y;
		}

		/**	Index */
		const size_t ti_IndiceCnt = csmGetDrawableIndexCounts(Model)[mu_SequencIdx];
		const unsigned short* vertexIndices = csmGetDrawableIndices(Model)[mu_SequencIdx];

		if (ti_IndiceCnt != md_IndexCount)
		{
			md_IndexCount = ti_IndiceCnt;
			marr_uDrawIndices.SetNum(md_IndexCount);
		}

		for (int i = 0; i < md_IndexCount; ++i)
		{
			marr_uDrawIndices[i] = vertexIndices[i];
		}

		if (md_IndexCount % 3)
		{
			UE_LOG(LogTemp, Error, TEXT("[FCubismDrawAble] DoUpdate(): Index count %d is not trianglable."), md_IndexCount);
		}
		else
		{
			tb_NeedReTriangle = true;
		}
	}

	/**	Do not do further update if not visible */
	if (!mb_IsVisible)
	{
		return;
	}

	//mf_Opacity = 0.0f; /*	This will hide the draw able */
	if (tb_NeedReTriangle)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_DRAWABLE_DoUpdate_ReTriangle);
		marr_sTriangles.SetNum(FMath::FloorToInt(md_IndexCount/3));

		size_t tu_TriCnt = 0;
		for (FCanvasUVTri& ts_CanvasTri : marr_sTriangles)
		{
			//UE_LOG(LogTemp, Log, TEXT("--- %u ---"), tu_TriCnt);

			unsigned short tu_Idx;
			tu_Idx = marr_uDrawIndices[tu_TriCnt++];
			//UE_LOG(LogTemp, Log, TEXT("%u"), tu_Idx);
			ts_CanvasTri.V0_Pos = marr_v2VertexPositions[tu_Idx]; ts_CanvasTri.V0_UV = marr_v2VertexUvs[tu_Idx]; ts_CanvasTri.V0_Color = FLinearColor(1.0f, 1.0f, 1.0f, mf_Opacity);

			tu_Idx = marr_uDrawIndices[tu_TriCnt++];
			//UE_LOG(LogTemp, Log, TEXT("%u"), tu_Idx);
			ts_CanvasTri.V1_Pos = marr_v2VertexPositions[tu_Idx]; ts_CanvasTri.V1_UV = marr_v2VertexUvs[tu_Idx]; ts_CanvasTri.V1_Color = FLinearColor(1.0f, 1.0f, 1.0f, mf_Opacity);

			tu_Idx = marr_uDrawIndices[tu_TriCnt++];
			//UE_LOG(LogTemp, Log, TEXT("%u"), tu_Idx);
			ts_CanvasTri.V2_Pos = marr_v2VertexPositions[tu_Idx]; ts_CanvasTri.V2_UV = marr_v2VertexUvs[tu_Idx]; ts_CanvasTri.V2_Color = FLinearColor(1.0f, 1.0f, 1.0f, mf_Opacity);
		}
	}

	/**	Update mask */
	if (md_MaskCount > 0)
	{
		Do_UpdateMask();
	}
}

void UCubismDrawAble::On_DrawMask(UCanvas* Canvas, int32 Width, int32 Height)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_DRAWABLE_DrawMask);

	Canvas->Canvas->Clear(mp_Model->mc_BgColor);

	TArray<UCubismDrawAble*>& ta_DrawAbles = mp_Model->Get_DrawAblesRef();

	for (const int& ti_Mask : marr_dMasks)
	{
		UCubismDrawAble* tp_MaskDraw = ta_DrawAbles[ti_Mask];
		
		FCanvasTriangleItem TriangleItem(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, mp_Model->mp_RespTexture->Resource);
		TriangleItem.TriangleList = CopyTemp(tp_MaskDraw->marr_sTriangles);
		TriangleItem.BlendMode = SE_BLEND_TranslucentAlphaOnlyWriteAlpha;
		Canvas->DrawItem(TriangleItem);
	}
}

void UCubismDrawAble::On_DrawBase(UCanvas* Canvas, int32 Width, int32 Height)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_DRAWABLE_DrawBase);

	Canvas->Canvas->Clear(mp_Model->mc_BgColor);

	FCanvasTriangleItem TriangleItem(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, mp_Model->mp_RespTexture->Resource);
	TriangleItem.TriangleList = CopyTemp(marr_sTriangles);
	TriangleItem.BlendMode = SE_BLEND_Opaque;
	Canvas->DrawItem(TriangleItem);
}

void UCubismDrawAble::Init_Mask()
{
	{
		UCanvasRenderTarget2D* tp_CanvasRender = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),
			FMath::TruncToInt(mp_Model->csmCanvasSize.X),
			FMath::TruncToInt(mp_Model->csmCanvasSize.Y)
		);

		tp_CanvasRender->ClearColor = mp_Model->mc_BgColor;
		tp_CanvasRender->OnCanvasRenderTargetUpdate.AddUniqueDynamic(this, &UCubismDrawAble::On_DrawBase);
		mp_BaseDrawer = tp_CanvasRender;
	}

	{
		UCanvasRenderTarget2D* tp_CanvasRender = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),
			FMath::TruncToInt(mp_Model->csmCanvasSize.X),
			FMath::TruncToInt(mp_Model->csmCanvasSize.Y)
		);

		tp_CanvasRender->ClearColor = mp_Model->mc_BgColor;
		tp_CanvasRender->OnCanvasRenderTargetUpdate.AddUniqueDynamic(this, &UCubismDrawAble::On_DrawMask);
		mp_MaskDrawer = tp_CanvasRender;
	}

	auto tp_MatDraw = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Csm/MatMask"));
	mp_MaskedMat = UMaterialInstanceDynamic::Create(tp_MatDraw, this, *ms_ID);
	mp_MaskedMat->SetTextureParameterValue("BaseTexture", mp_BaseDrawer);
	mp_MaskedMat->SetTextureParameterValue("MaskTexture", mp_MaskDrawer);
}

void UCubismDrawAble::Do_UpdateMask()
{
	mp_BaseDrawer->FastUpdateResource();
	mp_MaskDrawer->FastUpdateResource();
}

