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


void csmInitializeAnimationUserDataCallback(csmAnimationUserDataCallbackState *state, csmAnimationUserDataCallback callbackFunction)
{
  Ensure(state, "\"state\" is invalid.", return);


  state->CallbackFunction = callbackFunction;
  state->LastTime = -1.0f;
}

void csmUpdateAnimationUserDataCallbackUpdate(csmAnimationUserDataCallbackState *state, const csmAnimationState* animationState, const csmAnimation* animation)
{
  csmAnimationUserDataSink sink;
  int i;
  float time;

  Ensure(state, "\"state\" is invalid.", return);

  
  time = animationState->Time;

  if (animation->Loop)
  {
    while (time > animation->Duration)
    {
      time -= animation->Duration;
    }
  }


  for (i = 0; i < animation->UserDataCount; ++i)
  {
    csmGetAnimationUserData(animation, i, &sink);


    if ((state->LastTime >= sink.Time) || (sink.Time > time) || (state->LastTime >= time))
    {
      continue;
    }


    state->CallbackFunction(sink.Time, sink.Value);
  }


  state->LastTime = time;
}