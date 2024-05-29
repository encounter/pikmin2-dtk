#include "types.h"
#include "Vector3.h"

namespace Game {
namespace NsMathExp {
/**
 * Calculates the Lagrange interpolation for a given set of control points and a parameter t.
 * The result is stored in the output vector.
 *
 * @param controlPoints The array of control points.
 * @param t The parameter value.
 * @param output The vector to store the result.
 *
 * @note Address: 0x8023D7B0
 * @note Size: 0xA8
 */
void calcLagrange(const Vector3f* controlPoints, f32 t, Vector3f& output)
{
	f32 tMinusOne = t - 1.0f;
	f32 tMinusTwo = t - 2.0f;

	output.x = tMinusOne * (controlPoints[2].x * 0.5f * t)
	         + (tMinusTwo * (controlPoints[0].x * 0.5f * tMinusOne) - (tMinusTwo * (controlPoints[1].x * t)));
	output.y = tMinusOne * (controlPoints[2].y * 0.5f * t)
	         + (tMinusTwo * (controlPoints[0].y * 0.5f * tMinusOne) - (tMinusTwo * (controlPoints[1].y * t)));
	output.z = tMinusOne * (controlPoints[2].z * 0.5f * t)
	         + (tMinusTwo * (controlPoints[0].z * 0.5f * tMinusOne) - (tMinusTwo * (controlPoints[1].z * t)));
}

/**
 * @note Address: 0x8023D858
 * @note Size: 0x1D8
 */
/**
 * Calculates the position of the middle joint and bottom joint based on the given parameters.
 *
 * @param topPosition The position of the top joint.
 * @param bottomPosition The position of the bottom joint.
 * @param topToMiddleDistance The distance between the top joint and the middle joint.
 * @param middleToBottomDistance The distance between the middle joint and the bottom joint.
 * @param middleJointPos [out] The calculated position of the middle joint.
 * @param bottomJointPosition [out] The calculated position of the bottom joint.
 */
void calcJointPos(const Vector3f& topPosition, const Vector3f& bottomPosition, f32 topToMiddleDistance, f32 middleToBottomDistance,
                  Vector3f& middleJointPos, Vector3f& bottomJointPosition)
{
	f32 distanceTopMiddle    = SQUARE(topToMiddleDistance);
	f32 distanceMiddleBottom = SQUARE(middleToBottomDistance);

	Vector3f targetXyz         = bottomPosition;
	Vector3f topToTargetVector = bottomPosition - topPosition;

	f32 distanceTopToTarget = topToTargetVector.sqrMagnitude(); // f11
	if (!(distanceTopToTarget < 0.000001f)) {
		f32 factor = (0.5f / distanceTopToTarget) * (distanceTopToTarget + (distanceTopMiddle - distanceMiddleBottom)); // f3

		Vector3f scaledTopToTarget(factor * topToTargetVector.x + topPosition.x, factor * topToTargetVector.y + topPosition.y,
		                           factor * topToTargetVector.z + topPosition.z);

		Vector3f offsetFromTop = scaledTopToTarget - topPosition;

		f32 distanceAdjustment = distanceTopMiddle - SQUARE(offsetFromTop.x) - SQUARE(offsetFromTop.y) - SQUARE(offsetFromTop.z); // f30

		if (!(distanceAdjustment <= 0.0f)) {
			Vector3f cross1 = cross(middleJointPos, topToTargetVector);
			middleJointPos  = cross(cross1, topToTargetVector);

			f32 outSqr = middleJointPos.sqrMagnitude();
			if (outSqr != 0.0f) {
				f32 len               = _sqrtf2(distanceAdjustment / outSqr);
				bottomJointPosition.x = len * middleJointPos.x + cross1.x;
				bottomJointPosition.y = len * middleJointPos.y + cross1.y;
				bottomJointPosition.z = len * middleJointPos.z + cross1.z;
				return;
			}
		}
	}

	f32 dtm           = _sqrtf2(distanceTopMiddle);    // f7
	f32 dmb           = _sqrtf2(distanceMiddleBottom); // f3
	f32 distanceRatio = dtm / (dtm + dmb);             // f6

	bottomJointPosition = Vector3f(distanceRatio * targetXyz.x + topPosition.x, distanceRatio * targetXyz.y + topPosition.y,
	                               distanceRatio * targetXyz.z + topPosition.z);

	/*
	.loc_0x0:
	  stwu      r1, -0x30(r1)
	  stfd      f31, 0x20(r1)
	  psq_st    f31,0x28(r1),0,0
	  stfd      f30, 0x10(r1)
	  psq_st    f30,0x18(r1),0,0
	  lfs       f0, 0x4(r4)
	  fmuls     f5, f1, f1
	  lfs       f8, 0x4(r3)
	  fmuls     f4, f2, f2
	  lfs       f3, 0x0(r4)
	  fsubs     f1, f0, f8
	  lfs       f7, 0x0(r3)
	  lfs       f2, 0x8(r4)
	  fsubs     f0, f3, f7
	  lfs       f10, 0x8(r3)
	  fmuls     f6, f1, f1
	  fsubs     f2, f2, f10
	  lfs       f3, -0x3D54(r2)
	  fmadds    f6, f0, f0, f6
	  fmadds    f11, f2, f2, f6
	  fcmpo     cr0, f11, f3
	  blt-      .loc_0x154
	  lfs       f6, -0x3D58(r2)
	  fsubs     f3, f5, f4
	  lfs       f9, -0x3D50(r2)
	  fdivs     f6, f6, f11
	  fadds     f3, f11, f3
	  fmuls     f3, f6, f3
	  fmadds    f12, f3, f0, f7
	  fmadds    f13, f3, f1, f8
	  fmadds    f3, f3, f2, f10
	  fsubs     f6, f12, f7
	  fsubs     f7, f13, f8
	  fsubs     f8, f3, f10
	  fnmsubs   f6, f6, f6, f5
	  fnmsubs   f6, f7, f7, f6
	  fnmsubs   f30, f8, f8, f6
	  fcmpo     cr0, f30, f9
	  cror      2, 0, 0x2
	  beq-      .loc_0x154
	  lfs       f31, 0x0(r5)
	  lfs       f11, 0x4(r5)
	  fmuls     f7, f31, f2
	  lfs       f10, 0x8(r5)
	  fmuls     f6, f11, f0
	  fmuls     f8, f10, f1
	  fmsubs    f10, f10, f0, f7
	  fmsubs    f31, f31, f1, f6
	  fmsubs    f11, f11, f2, f8
	  fmuls     f8, f2, f10
	  fmuls     f7, f0, f31
	  fmuls     f6, f1, f11
	  fmsubs    f8, f1, f31, f8
	  fmsubs    f7, f2, f11, f7
	  fmsubs    f6, f0, f10, f6
	  stfs      f8, 0x0(r5)
	  stfs      f7, 0x4(r5)
	  stfs      f6, 0x8(r5)
	  lfs       f10, 0x0(r5)
	  lfs       f6, 0x4(r5)
	  fmuls     f7, f10, f10
	  lfs       f8, 0x8(r5)
	  fmuls     f6, f6, f6
	  fmuls     f8, f8, f8
	  fadds     f6, f7, f6
	  fadds     f6, f8, f6
	  fcmpu     cr0, f9, f6
	  beq-      .loc_0x154
	  fdivs     f1, f30, f6
	  fcmpo     cr0, f1, f9
	  ble-      .loc_0x12C
	  ble-      .loc_0x130
	  fsqrte    f0, f1
	  fmuls     f1, f0, f1
	  b         .loc_0x130

	.loc_0x12C:
	  fmr       f1, f9

	.loc_0x130:
	  fmadds    f0, f1, f10, f12
	  stfs      f0, 0x0(r6)
	  lfs       f0, 0x4(r5)
	  fmadds    f0, f1, f0, f13
	  stfs      f0, 0x4(r6)
	  lfs       f0, 0x8(r5)
	  fmadds    f0, f1, f0, f3
	  stfs      f0, 0x8(r6)
	  b         .loc_0x1C0

	.loc_0x154:
	  lfs       f7, -0x3D50(r2)
	  fcmpo     cr0, f5, f7
	  ble-      .loc_0x174
	  ble-      .loc_0x170
	  fsqrte    f3, f5
	  fmuls     f7, f3, f5
	  b         .loc_0x174

	.loc_0x170:
	  fmr       f7, f5

	.loc_0x174:
	  lfs       f3, -0x3D50(r2)
	  fcmpo     cr0, f4, f3
	  ble-      .loc_0x194
	  ble-      .loc_0x190
	  fsqrte    f3, f4
	  fmuls     f3, f3, f4
	  b         .loc_0x194

	.loc_0x190:
	  fmr       f3, f4

	.loc_0x194:
	  fadds     f6, f7, f3
	  lfs       f5, 0x0(r3)
	  lfs       f4, 0x4(r3)
	  lfs       f3, 0x8(r3)
	  fdivs     f6, f7, f6
	  fmadds    f5, f6, f0, f5
	  fmadds    f1, f6, f1, f4
	  fmadds    f0, f6, f2, f3
	  stfs      f5, 0x0(r6)
	  stfs      f1, 0x4(r6)
	  stfs      f0, 0x8(r6)

	.loc_0x1C0:
	  psq_l     f31,0x28(r1),0,0
	  lfd       f31, 0x20(r1)
	  psq_l     f30,0x18(r1),0,0
	  lfd       f30, 0x10(r1)
	  addi      r1, r1, 0x30
	  blr
	*/
}
} // namespace NsMathExp
} // namespace Game
