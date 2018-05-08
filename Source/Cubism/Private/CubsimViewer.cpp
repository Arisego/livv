// Fill out your copyright notice in the Description page of Project Settings.

#include "CubsimViewer.h"


UCubismViewer::UCubismViewer()
	: mp_RespTexture()
	, mp_ModelInfo(nullptr)
	, mp_CubismModel(nullptr)
#if WITH_EDITOR
	, mu_IsEditing(0)
#endif
{
	mp_RespTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Engine/ArtTools/RenderToTexture/Textures/127grey"));
}

UCubismViewer::~UCubismViewer()
{
	if (mp_ModelInfo)
	{
		delete mp_ModelInfo;
		mp_ModelInfo = nullptr;
	}
}

FORCEINLINE TArray<UCubismDrawAble*>& UCubismViewer::Get_DrawAblesRef()
{
	return marr_pDrawAbles;
}

void UCubismViewer::Form_Model()
{
	/**
	*	<Assume that all needed data is ready, either from file read or asset load
	*	mp_ModelInfo->Moc| should be ready, for the case of loading from asset, find a way to load and form the moc
	*	ma_UserData|ma_PhysicsData|ma_AnimationData| should be already, or they will be ignored
	*/


	/** Model */
	uint32_t modelSize = csmGetSizeofModel(mp_ModelInfo->Moc);
	TArray<uint8>& ta_ModelRes = ma_ModelRes;
	ta_ModelRes.SetNum(modelSize);
	void* modelMemory = ta_ModelRes.GetData();

	mp_ModelInfo->Model = csmInitializeModelInPlace(mp_ModelInfo->Moc, modelMemory, modelSize);

	uint32_t tableSize = 0;
	tableSize = csmGetSizeofModelHashTable(mp_ModelInfo->Model);

	void* tableMemory = malloc(tableSize);
	mp_ModelInfo->Table = csmInitializeModelHashTableInPlace(mp_ModelInfo->Model, tableMemory, tableSize);

	/**	Load User Data */
	if (mp_CubismModel->ma_UserData.Num() > 0)
	{
		uint32_t userDataSize = csmGetDeserializedSizeofUserData((char*)mp_CubismModel->ma_UserData.GetData());
		void* userDataMemory = Allocate(userDataSize);

		mp_ModelInfo->UserData = csmDeserializeUserDataInPlace((char*)mp_CubismModel->ma_UserData.GetData(), userDataMemory, userDataSize);

		// Try to getting user data.
		csmUserDataSink userDataSink;
		for (int32_t i = 0; i < csmGetUserDataCount(mp_ModelInfo->UserData); ++i)
		{
			csmGetUserData(mp_ModelInfo->UserData, i, &userDataSink);

			UE_LOG(LogTemp, Log, TEXT("%s"), ANSI_TO_TCHAR(userDataSink.Value));
		}
	}

	/**	<Load Physics Data */
	if (mp_CubismModel->ma_PhysicsData.Num() > 0)
	{
		uint32_t physicsSize = csmGetDeserializedSizeofPhysics((char*)mp_CubismModel->ma_PhysicsData.GetData());
		void* physicsMemory = Allocate(physicsSize);

		mp_ModelInfo->Physics = csmDeserializePhysicsInPlace((char*)mp_CubismModel->ma_PhysicsData.GetData(), physicsMemory, physicsSize);
	}

	/**	<Load Animation Data */
	if (mp_CubismModel->ma_AnimationData.Num() > 0)
	{
		uint32_t animationSize = csmGetDeserializedSizeofAnimation((char*)mp_CubismModel->ma_AnimationData.GetData());
		void* animationMemory = Allocate(animationSize);

		mp_ModelInfo->Animation = csmDeserializeAnimationInPlace((char*)mp_CubismModel->ma_AnimationData.GetData(), animationMemory, animationSize);

		// Try to getting animation user data.
		csmAnimationUserDataSink animationSink;
		for (int32_t i = 0; i < csmGetAnimationUserDataCount(mp_ModelInfo->Animation); ++i)
		{
			csmGetAnimationUserData(mp_ModelInfo->Animation, i, &animationSink);

			UE_LOG(LogTemp, Log, TEXT("%s"), ANSI_TO_TCHAR(animationSink.Value));
		}

		// Initializes animation user data callback.
		csmInitializeAnimationUserDataCallback(&mp_ModelInfo->AnimationUserDataCallback, AnimCallback);

		// Initialize animation state by resetting it.
		csmResetAnimationState(&mp_ModelInfo->AnimationState);
	}

	/**	<Init the model */
	int td_DrawCnt = csmGetDrawableCount(mp_ModelInfo->Model);

	csmVector2 tcv_size;
	csmVector2 tcv_orig;
	float tf_Ppu;

	csmReadCanvasInfo(mp_ModelInfo->Model, &tcv_size, &tcv_orig, &tf_Ppu);
	csmCanvasSize = FVector2D(tcv_size.X, tcv_size.Y);
	csmOrigPos = FVector2D(tcv_orig.X, tcv_orig.Y);

	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): %d DrawCount"), td_DrawCnt);
	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): Canvas info: %f,%f||%f,%f||%f"),
		tcv_size.X, tcv_size.Y,
		tcv_orig.X, tcv_orig.Y,
		tf_Ppu);

	//drawables = Allocate(sizeof(Drawable) * drawableCount);
	const int* textureIndices = csmGetDrawableTextureIndices(mp_ModelInfo->Model);
	const csmFlags* constantFlags = csmGetDrawableConstantFlags(mp_ModelInfo->Model);
	const int* vertexCounts = csmGetDrawableVertexCounts(mp_ModelInfo->Model);
	const csmVector2** vertexPositons = csmGetDrawableVertexPositions(mp_ModelInfo->Model);
	const csmVector2** vertexUvs = csmGetDrawableVertexUvs(mp_ModelInfo->Model);
	const int* indexCounts = csmGetDrawableIndexCounts(mp_ModelInfo->Model);
	const unsigned short** vertexIndices = csmGetDrawableIndices(mp_ModelInfo->Model);
	const char** ids = csmGetDrawableIds(mp_ModelInfo->Model);
	const float* opacities = csmGetDrawableOpacities(mp_ModelInfo->Model);
	const int* drawOrders = csmGetDrawableDrawOrders(mp_ModelInfo->Model);
	const int* renderOrders = csmGetDrawableRenderOrders(mp_ModelInfo->Model);
	const csmFlags* dynamicFlas = csmGetDrawableDynamicFlags(mp_ModelInfo->Model);
	const int* maskCounts = csmGetDrawableMaskCounts(mp_ModelInfo->Model);
	const int** masks = csmGetDrawableMasks(mp_ModelInfo->Model);

	TArray<UCubismDrawAble*>& ta_DrawAbles = Get_DrawAblesRef();
	ta_DrawAbles.SetNumZeroed(td_DrawCnt);
	size_t ti_Iter = 0;
	for (UCubismDrawAble*& ts_DrawAble : ta_DrawAbles)
	{
		UE_LOG(LogTemp, Log, TEXT("Drawable %u|%s:"), ti_Iter, ANSI_TO_TCHAR(ids[ti_Iter]));

		ts_DrawAble = NewObject<UCubismDrawAble>(this);
		ts_DrawAble->mf_Scale = tf_Ppu;
		ts_DrawAble->mv2_Origin = csmOrigPos;
		ts_DrawAble->mb_IsVisible = true;

		ts_DrawAble->mu_SequencIdx = ti_Iter;
		/**	Basic infos */
		ts_DrawAble->md_TextureIndex = textureIndices[ti_Iter];
		if ((constantFlags[ti_Iter] & csmBlendAdditive) == csmBlendAdditive)
		{
			ts_DrawAble->ms_BlendMode = csmAdditiveBlending;
		}
		else if ((constantFlags[ti_Iter] & csmBlendMultiplicative) == csmBlendMultiplicative)
		{
			ts_DrawAble->ms_BlendMode = csmMultiplicativeBlending;
		}
		else
		{
			ts_DrawAble->ms_BlendMode = csmNormalBlending;
		}
		ts_DrawAble->mb_IsDoubleSided =
			(constantFlags[ti_Iter] & csmIsDoubleSided) == csmIsDoubleSided;

		/**	Vertex and index */
		ts_DrawAble->VertexCount = vertexCounts[ti_Iter];
		ts_DrawAble->marr_v2VertexPositions.SetNumZeroed(vertexCounts[ti_Iter]);
		ts_DrawAble->marr_v2VertexUvs.SetNumZeroed(vertexCounts[ti_Iter]);

		for (int i = 0; i < vertexCounts[ti_Iter]; ++i)
		{
			ts_DrawAble->marr_v2VertexPositions[i].X = vertexPositons[ti_Iter][i].X;
			ts_DrawAble->marr_v2VertexPositions[i].Y = vertexPositons[ti_Iter][i].Y;

			ts_DrawAble->marr_v2VertexUvs[i].X = vertexUvs[ti_Iter][i].X;
			ts_DrawAble->marr_v2VertexUvs[i].Y = vertexUvs[ti_Iter][i].Y;

			UE_LOG(LogTemp, Log, TEXT("-- %s|%s"),
				*ts_DrawAble->marr_v2VertexPositions[i].ToString(),
				*ts_DrawAble->marr_v2VertexUvs[i].ToString());
		}

		ts_DrawAble->md_IndexCount = indexCounts[ti_Iter];
		ts_DrawAble->marr_uDrawIndices.SetNumZeroed(indexCounts[ti_Iter]);
		for (int i = 0; i < indexCounts[ti_Iter]; ++i)
		{
			ts_DrawAble->marr_uDrawIndices[i] = vertexIndices[ti_Iter][i];
		}

		/**	Drawable infos */
		ts_DrawAble->ms_ID = ANSI_TO_TCHAR(ids[ti_Iter]);
		ts_DrawAble->md_DrawOrder = drawOrders[ti_Iter];

		ts_DrawAble->mf_Opacity = opacities[ti_Iter];
		ts_DrawAble->md_RenderOrder = renderOrders[ti_Iter];
		ts_DrawAble->ms_DynamicFlag = dynamicFlas[ti_Iter];
		ts_DrawAble->md_MaskCount = maskCounts[ti_Iter];
		for (int i = 0; i < ts_DrawAble->md_MaskCount; ++i)
		{
			ts_DrawAble->marr_dMasks.Add(masks[ti_Iter][i]);
		}

		marr_pDrawOrder.Add(ts_DrawAble);
		ts_DrawAble->mp_Model = this;

		++ti_Iter;
	}

	/**	<Print out mask infos */
	UE_LOG(LogTemp, Log, TEXT("OnDidStart: Print out mask infos"));
	for (UCubismDrawAble* ts_DrawAble : ta_DrawAbles)
	{
		if (ts_DrawAble->md_MaskCount <= 0) continue;
		UE_LOG(LogTemp, Log, TEXT("Mask base: %s"), *ts_DrawAble->ms_ID);

		for (const int& ti_Mask : ts_DrawAble->marr_dMasks)
		{
			UE_LOG(LogTemp, Log, TEXT("- %d|%s"), ti_Mask, *ta_DrawAbles[ti_Mask]->ms_ID);
		}
	}

	Model = mp_ModelInfo->Model;
	Init_Model();

	UE_LOG(LogTemp, Log, TEXT("OnDidStart: Complete"));

	/**	List parameter */
	{
		auto parameterCount = csmGetParameterCount(mp_ModelInfo->Model);
		auto parameterIds = csmGetParameterIds(mp_ModelInfo->Model);
		auto parameterValues = csmGetParameterValues(mp_ModelInfo->Model);
		auto parameterMaximumValues = csmGetParameterMaximumValues(mp_ModelInfo->Model);
		auto parameterMinimumValues = csmGetParameterMinimumValues(mp_ModelInfo->Model);
		auto parameterDefaultValues = csmGetParameterDefaultValues(mp_ModelInfo->Model);
		for (int32_t i = 0; i < parameterCount; ++i)
		{
			UE_LOG(LogTemp, Log, TEXT("id:%s|min:%3.1f max : %3.1f default:%3.1f value: %f"),
				ANSI_TO_TCHAR(parameterIds[i]),
				parameterMinimumValues[i],
				parameterMaximumValues[i],
				parameterDefaultValues[i],
				parameterValues[i]);
		}
	}

}

void UCubismViewer::Init_Model()
{
	/**	Init render canvas */
	mp_RenderCanv = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),
		FMath::TruncToInt(csmCanvasSize.X),
		FMath::TruncToInt(csmCanvasSize.Y)
	);
	mp_RenderCanv->OnCanvasRenderTargetUpdate.AddUniqueDynamic(this, &UCubismViewer::Do_PaintModel);

	/**	Init mask render */
	int ti_Cnt = 0;
	for (UCubismDrawAble* tp_DrawAble : marr_pDrawAbles)
	{
		if (tp_DrawAble->md_MaskCount > 0)
		{
			tp_DrawAble->Init_Mask();
		}
		++ti_Cnt;
	}

	/**	ReOrder while init */
	Do_ReOrder();
}

void UCubismViewer::Init_CsmFromAsset()
{
	if (!mp_ModelInfo)
	{
		void* FileMem = AllocateAligned(mp_CubismModel->mu_MocSize, csmAlignofMoc);
		FMemory::Memcpy(FileMem, mp_CubismModel->ma_MocRes.GetData(), mp_CubismModel->mu_MocSize);

		mp_ModelInfo = new FModelInfo();

		mp_ModelInfo->Moc = csmReviveMocInPlace(FileMem, mp_CubismModel->mu_MocSize);
		Form_Model();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCubsimViewer::PostLoad()| Alread modeled."));
	}
}

bool UCubismViewer::IsTickableInEditor() const
{
	return true;
}

void UCubismViewer::Tick(float DeltaTime)
{
	UpdateCsm(DeltaTime);
	UpdateModel();
	RenderModel();
}

TStatId UCubismViewer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCubismViewer, STATGROUP_Tickables);
}

bool UCubismViewer::IsTickable() const
{
	if (HasAnyFlags(RF_ClassDefaultObject)) return  false;

	return mp_ModelInfo != nullptr
#if WITH_EDITOR
		&& mu_IsEditing != 1
#endif
		;
}

void UCubismViewer::BeginDestroy()
{
	UObject::BeginDestroy();
	UE_LOG(LogTemp, Log, TEXT("UCubsimViewer::BeginDestroy"));
}

void UCubismViewer::UpdateModel()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_MODEL_UpdateModel);

	for (UCubismDrawAble* tp_DrawAble : marr_pDrawAbles)
	{
		tp_DrawAble->DoUpdate(Model);
	}

	Do_ReOrder();
}

void UCubismViewer::RenderModel()
{
	if (!mp_RespTexture) return;
	if (!mp_RenderCanv) return;

	mp_RenderCanv->ClearColor = mc_BgColor;
	mp_RenderCanv->UpdateResource();
}

UCanvasRenderTarget2D* UCubismViewer::Get_RenderTarget()
{
	return mp_RenderCanv;
}

void UCubismViewer::Do_PaintModel(UCanvas* Canvas, int32 Width, int32 Height)
{
	check(Canvas);
	check(Width != 0);
	check(Height != 0);

	for (UCubismDrawAble* tp_DrawAble : marr_pDrawOrder)
	{
		if (!tp_DrawAble->mb_IsVisible)
		{
			continue;
		}

		if (tp_DrawAble->md_MaskCount <= 0)
		{
			ESimpleElementBlendMode ts_BlendMode = SE_BLEND_AlphaBlend;
			switch (tp_DrawAble->ms_BlendMode)
			{
			case csmNormalBlending:
				ts_BlendMode = SE_BLEND_AlphaBlend;
				break;
			case csmAdditiveBlending:
				ts_BlendMode = SE_BLEND_Additive;
				break;
			case csmMultiplicativeBlending:
				ts_BlendMode = SE_BLEND_Translucent;
				break;
			}

			FCanvasTriangleItem TriangleItem(FVector2D::ZeroVector, FVector2D::ZeroVector, FVector2D::ZeroVector, mp_RespTexture->Resource);
			TriangleItem.TriangleList = CopyTemp(tp_DrawAble->marr_sTriangles);
			TriangleItem.BlendMode = ts_BlendMode;
			Canvas->DrawItem(TriangleItem);
		}
		else
		{
			Canvas->K2_DrawMaterial(tp_DrawAble->Get_MaskMat(), FVector2D(0, 0), FVector2D(Width, Height), FVector2D(0, 0));
		}

	}
}

void UCubismViewer::UpdateCsm(float DeltaTime)
{
	//UE_LOG(LogTemp, Log, TEXT("UCubsimViewer::UpdateCsm %f"), DeltaTime);
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_MODEL_UpdateCubism);

	float animationBlendWeight;
	csmPhysicsOptions phyicsOptions;


	phyicsOptions.Wind.X = 0.0f;
	phyicsOptions.Wind.Y = 0.0f;

	phyicsOptions.Gravity.X = 0.0f;
	phyicsOptions.Gravity.Y = -1.0f;


	animationBlendWeight = 1.0f;


	// Update animation.
	csmUpdateAnimationState(&mp_ModelInfo->AnimationState, DeltaTime);
	csmEvaluateAnimationFAST(mp_ModelInfo->Animation,
		&mp_ModelInfo->AnimationState,
		csmOverrideFloatBlendFunction,
		animationBlendWeight,
		mp_ModelInfo->Model,
		mp_ModelInfo->Table,
		0,
		0,
		0);

	// Update animation userdata callback.
	csmUpdateAnimationUserDataCallbackUpdate(&mp_ModelInfo->AnimationUserDataCallback, &mp_ModelInfo->AnimationState, mp_ModelInfo->Animation);


	// Update physics.
	csmPhysicsEvaluate(mp_ModelInfo->Model, mp_ModelInfo->Physics, &phyicsOptions, DeltaTime);

	// Reset dynamic drawable flags.
	csmResetDrawableDynamicFlags(mp_ModelInfo->Model);

	// Update model.
	csmUpdateModel(mp_ModelInfo->Model);
}

void UCubismViewer::Do_ReOrder()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_CUBISM_MODEL_ReOrder);
	Algo::Sort(marr_pDrawOrder, [](const UCubismDrawAble* A, const UCubismDrawAble* B) {
		return A->md_RenderOrder < B->md_RenderOrder;
	});
}
