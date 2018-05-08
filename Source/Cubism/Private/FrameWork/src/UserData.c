/*
* Copyright(c) Live2D Inc. All rights reserved.
*
* Use of this source code is governed by the Live2D Open Software license
* that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
*/


#include <Live2DCubismFramework.h>
#include <Live2DCubismFrameworkINTERNAL.h>


// -------- //
// REQUIRES //
// -------- //

#include "Local.h"

#include <Live2DCubismCore.h>


unsigned int csmGetDeserializedSizeofUserData(const char* userDataJson)
{
  UserDataJsonMeta meta;


  // Validate argument.
  Ensure(userDataJson, "\"userDataJson\" is invalid.", return 0);


  ReadUserDataJsonMeta(userDataJson, &meta);


  return (unsigned int)(sizeof(csmUserData)
    + (sizeof(csmUserDataTag) * meta.UserDataCount)
    + (sizeof(char) * meta.TotalUserDataSize));
}


csmUserData* csmDeserializeUserDataInPlace(const char *userDataJson, void* address, const unsigned int size)
{
  csmUserData* userData;


  // 'Patch' pointer.
  userData = (csmUserData*)address;


  // Deserialize animation.
  ReadUserDataJson(userDataJson, userData);


  return userData;
}


int csmGetUserDataCount(const csmUserData* userData)
{
  Ensure(userData, "\"userData\" is invalid.", return 0);


  return userData->TagCount;
}


void csmGetUserData(const csmUserData* userData, int index, csmUserDataSink* destination)
{
  csmUserDataTag* tag;


  Ensure(userData, "\"userData\" is invalid.", return);
  Ensure(destination, "\"destination\" is invalid.", return);


  tag = &userData->Tags[index];


  destination->Id = tag->Id;
  destination->Value = (const char*)(userData->Values + tag->BaseValueIndex);
}


int csmGetAnimationUserDataCount(const csmAnimation* animation)
{
  Ensure(animation, "\"animation\" is invalid.", return 0);


  return animation->UserDataCount;
}


void csmGetAnimationUserData(const csmAnimation* animation, int index, csmAnimationUserDataSink* destination)
{
  csmAnimationUserData* userData;


  Ensure(animation, "\"animation\" is invalid.", return);
  Ensure(destination, "\"destination\" is invalid.", return);


  userData = &animation->UserData[index];


  destination->Time = userData->Time;
  destination->Value = (const char*)(animation->UserDataValues + userData->BaseValueIndex);
}
