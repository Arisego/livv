#pragma once

extern "C"
{
#include "FrameWork/include/Live2DCubismFramework.h"
#include "FrameWork/include/Helper.h"
}

struct FModelInfo
{
	csmMoc* Moc;

	csmModel* Model;

	csmModelHashTable* Table;

	csmUserData* UserData;

	csmPhysicsRig* Physics;

	csmAnimation* Animation;

	csmAnimationState AnimationState;

	csmAnimationUserDataCallbackState AnimationUserDataCallback;

	FModelInfo();
	~FModelInfo();
};
