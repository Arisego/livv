// Fill out your copyright notice in the Description page of Project Settings.

#include "CubismBpLib.h"

#include "Live2DCubismCore.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Comps/CubismDrawAble.h"
#include "CubismModel.h"
#include "Comps/ModelInfo.h"
#include "CubsimViewer.h"


/**	Log out func register to cubism */
static void Cubism_Log(const char* message)
{
	printf("%s\n", message);
	UE_LOG(LogTemp, Log, TEXT("[UCubismBpLib] %s"), ANSI_TO_TCHAR(message));
}

void AnimCallback(float time, const char* value)
{
	UE_LOG(LogTemp, Log, TEXT("[UCubismBpLib] AnimCallback: %f -> %s"), time, ANSI_TO_TCHAR(value));
}

/////////////////////////////////////////////////////////////////////////////////////
void UCubismBpLib::InitCubism()
{
	csmVersion version = csmGetVersion();
	UE_LOG(LogTemp, Log, TEXT("csm Version is: %u"), version);

	csmSetLogFunction(Cubism_Log);
}

UCubismViewer* UCubismBpLib::CreateCubism(UObject* Outer, UCubismModel* InModelData)
{
	//UCubismModel* InModel = LoadObject<UCubismModel>(Outer, TEXT("/Game/Mark.Mark"));
	//InModel->Init_CsmFromAsset();
	//InModel->mb_IsInstance = true;
	//UE_LOG(LogTemp, Log, TEXT("--Ptrs %p, %p, %p|%p"), InModel, Outer, Outer->GetWorld(), InModel->GetWorld());

	//return InModel;
	if (!InModelData)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateCubism: No model data"));
		return nullptr;
	}

	UCubismViewer* tp_Viewer = NewObject<UCubismViewer>(Outer);
	tp_Viewer->mp_CubismModel = InModelData;
	tp_Viewer->Init_CsmFromAsset();
	return tp_Viewer;
}

bool UCubismBpLib::ReadCubism(UCubismModel* IoModel, const FString InFilePath)
{
	/**	<Load moc file */
	FString tstr_FilePath = InFilePath;

	FArchive* Reader = IFileManager::Get().CreateFileReader(*tstr_FilePath);
	if (!Reader)
	{
		if (!(0 & FILEREAD_Silent))
		{
			UE_LOG(LogStreaming, Error, TEXT("Failed to read file '%s' error."), *tstr_FilePath);
		}
		return false;
	}

	int64 TotalSize = Reader->TotalSize();

	void* FileMem = AllocateAligned(TotalSize, csmAlignofMoc);

	Reader->Serialize(FileMem, TotalSize);
	bool Success = Reader->Close();
	delete Reader;

	const FString tstr_Name = FPaths::GetBaseFilename(tstr_FilePath);
	const FString tstr_FilePreFix = FPaths::GetPath(tstr_FilePath) / tstr_Name;

	UE_LOG(LogStreaming, Log, TEXT("read file '%s' error."), *tstr_FilePath);
	IoModel->ma_MocRes = TArray<uint8>((uint8*)FileMem, TotalSize);
	IoModel->mu_MocSize = TotalSize;

	void* tpp = IoModel->ma_MocRes.GetData();
	UE_LOG(LogTemp,Log,TEXT("%p %p"), tpp, FileMem);

	/**	<Load User Data */
	tstr_FilePath = tstr_FilePreFix + TEXT(".userdata3.json");
	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): Read user data from %s"), *tstr_FilePath);
	if (!FFileHelper::LoadFileToArray(IoModel->ma_UserData, *tstr_FilePath))
	{
		IoModel->ma_UserData.Empty();
		UE_LOG(LogTemp, Warning, TEXT("OnDidStart(): No user data found"));
	}

	/**	<Load Physics */
	tstr_FilePath = tstr_FilePreFix + TEXT(".physics3.json"); 
	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): Read physics data from %s"), *tstr_FilePath);
	if (!FFileHelper::LoadFileToArray(IoModel->ma_PhysicsData, *tstr_FilePath))
	{
		IoModel->ma_PhysicsData.Empty();
		UE_LOG(LogTemp, Warning, TEXT("OnDidStart(): No physics data found"));
	}

	/**	<Load Animation */
	tstr_FilePath = tstr_FilePreFix + TEXT(".motion3.json");
	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): Read animation data from %s"), *tstr_FilePath);
	if (!FFileHelper::LoadFileToArray(IoModel->ma_AnimationData, *tstr_FilePath))
	{
		IoModel->ma_AnimationData.Empty();
		UE_LOG(LogTemp, Warning, TEXT("OnDidStart(): No animation data found"));
	}

	UE_LOG(LogTemp, Log, TEXT("OnDidStart(): Load Complete"));

	return true;
}
