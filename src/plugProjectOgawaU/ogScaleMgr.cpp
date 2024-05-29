#include "og/Screen/ScaleMgr.h"
#include "sysMath.h"
#include "System.h"
#include "trig.h"

namespace og {
namespace Screen {

/**
 * @note Address: 0x80328E04
 * @note Size: 0x44
 */
ScaleMgr::ScaleMgr()
    : mState(SCM_Inactive)
{
	setParam(1.0f, 1.0f, 1.0f);
}

/**
 * @note Address: 0x80328E48
 * @note Size: 0x48
 */
void ScaleMgr::up()
{
	mState = SCM_Growing;
	setParam(0.5f, 30.0f, 0.8f);
	mWaitTimer = 0.0f;
}

/**
 * @note Address: 0x80328E90
 * @note Size: 0x48
 */
void ScaleMgr::down()
{
	mState = SCM_Shrinking;
	setParam(0.25f, 35.0f, 0.8f);
	mWaitTimer = 0.0f;
}

/**
 * @note Address: 0x80328ED8
 * @note Size: 0x64
 */
void ScaleMgr::up(f32 p1, f32 periodModifier, f32 durationInSeconds, f32 time)
{
	setParam(p1, periodModifier, durationInSeconds);
	mWaitTimer = time;
	if (time < 0.01f) {
		mState = SCM_Growing;
	} else {
		mState = SCM_GrowWait;
	}
}

/**
 * @note Address: 0x80328F3C
 * @note Size: 0x28
 */
void ScaleMgr::down(f32 p1, f32 periodModifier, f32 durationInSeconds)
{
	mState = SCM_Shrinking;
	setParam(p1, periodModifier, durationInSeconds);
}

/**
 * @note Address: 0x80328F64
 * @note Size: 0x20
 */
void ScaleMgr::setParam(f32 strength, f32 periodModifier, f32 durationInSeconds)
{
	mElapsedSeconds    = 0.0f;
	mScale             = 1.0f;
	mScaleChangeLevel  = strength;
	mPeriodModifier    = periodModifier;
	mDurationInSeconds = durationInSeconds;
}

/**
 * @note Address: 0x80328F84
 * @note Size: 0x20C
 */
f32 ScaleMgr::calc()
{
	switch (mState) {
	case SCM_Inactive:
		mScale = 1.0f;
		break;

	case SCM_Growing:
		mElapsedSeconds += sys->mDeltaTime;
		if (mElapsedSeconds > mDurationInSeconds) {
			mState          = SCM_Inactive;
			mScale          = 1.0f;
			mElapsedSeconds = 0.0f;
		} else {
			f32 sin = sinf(mElapsedSeconds * mPeriodModifier);
			mScale  = (mDurationInSeconds - mElapsedSeconds) * (mScaleChangeLevel * sin + mScaleChangeLevel) + 1.0f;
		}
		break;

	case SCM_Shrinking:
		mElapsedSeconds += sys->mDeltaTime;
		if (mElapsedSeconds > mDurationInSeconds) {
			mState          = SCM_Inactive;
			mScale          = 1.0f;
			mElapsedSeconds = 0.0f;
		} else {
			f32 sin = sinf(mElapsedSeconds * mPeriodModifier);
			mScale  = -((mDurationInSeconds - mElapsedSeconds) * (mScaleChangeLevel * sin + mScaleChangeLevel) - 1.0f);
		}
		break;

	case SCM_GrowWait:
		mScale = 1.0f;
		mWaitTimer -= sys->mDeltaTime;
		if (mWaitTimer < 0.0f) {
			mState     = SCM_Growing;
			mWaitTimer = 0.0f;
		}
		break;

	default:
		mScale = 1.0f;
	}

	return mScale;
}

} // namespace Screen
} // namespace og
