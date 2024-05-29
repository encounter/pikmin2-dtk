#include "og/newScreen/SMenu.h"
#include "og/newScreen/ogUtil.h"
#include "og/Screen/MapCounter.h"
#include "og/Screen/anime.h"
#include "og/Screen/ogScreen.h"
#include "og/Sound.h"
#include "Game/Navi.h"
#include "Game/CameraMgr.h"
#include "Game/Cave/RandMapMgr.h"
#include "Radar.h"
#include "nans.h"

static const u32 padding[] = { 0, 0, 0 };

namespace og {

namespace newScreen {

u64 map_icon_tag[22]
    = { 'oniyon_r', 'oniyon_b', 'oniyon_y', 'piki_r',   'piki__b', 'piki_y',   'piki_bl',  'piki_w',   'piki_fr', 'piki_me', 'kanketu',
	    'cave',     'pot',      'luji_bs',  'orima_bs', 'ufo',     'takar_bs', 'takar_bs', 'takar_bs', 'cave',    'comp_c',  'cave' };

/**
 * @note Address: N/A
 * @note Size: 0x228
 */
Game::Navi* getNaviPtr(int index)
{
	Iterator<Game::Navi> iter(Game::naviMgr);
	Game::Navi* navi;
	CI_LOOP(iter)
	{
		navi = (*iter);
		if (navi->isAlive() && (int)navi->mNaviIndex == index) {
			return navi;
		}
	}
	return nullptr;
}

/**
 * @note Address: 0x8030F6F0
 * @note Size: 0x114
 */
ObjSMenuMap::ObjSMenuMap(char const* name)
{
	mMapPosition.x          = 0.0f;
	mMapPosition.y          = 0.0f;
	mCurrentZoom            = 1.0f;
	mMapAngle               = 0.0f;
	mMapTexScale.x          = 1.0f;
	mMapTexScale.y          = 1.0f;
	mMapTextureDimensions.x = 0.0f;
	mMapTextureDimensions.y = 0.0f;
	mMapBounds.x            = 0.0f;
	mMapBounds.y            = 0.0f;
	_108.x                  = 1.0f;
	_108.y                  = 1.0f;
	mMapRotationOrigin.x    = 0.0f;
	mMapRotationOrigin.y    = 0.0f;
	mDisp                   = nullptr;
	mMapCounter             = nullptr;
	mAnimGroup              = nullptr;
	mName                   = name;
	mPane_map               = nullptr;
	_B8                     = 0;
	mIconScreen             = nullptr;
	mRadarMapTexture        = nullptr;
	mRootPane               = nullptr;
	mPane_Ncompas           = nullptr;
	mMapTexPane             = nullptr;
	mRadarPaneList          = nullptr;
	mOlimarArrow            = nullptr;
	mOlimarObj              = nullptr;
	mLouieArrow             = nullptr;
	mLouieObj               = nullptr;
	mMapIconNum             = 0;
	mUpdateCaveTex          = false;
	mController             = nullptr;
	mIconScreen2            = nullptr;
	mCompassPic             = nullptr;
	mOlimarGlow             = nullptr;
	mLouieGlow              = nullptr;
	mStartZoom              = 1.0f;
	mZoomCaveTextAlpha      = 255;
	mCaveLabelCount         = 0;
	mCaveLabelTextBoxes[0]  = nullptr;
	mCaveLabelTextBoxes[1]  = nullptr;
	mCaveLabelTextBoxes[2]  = nullptr;
	mCaveLabelTextBoxes[3]  = nullptr;
	mCaveLabelTextBoxes[4]  = nullptr;
}

/**
 * @note Address: 0x8030F8B0
 * @note Size: 0xC4
 */
ObjSMenuMap::~ObjSMenuMap() { }

/**
 * @note Address: N/A
 * @note Size: 0x24
 */
void ObjSMenuMap::calcMapScale()
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0xAC
 */
void ObjSMenuMap::calcMapPos(Vector2f pos, Vector2f* outPos)
{
	Vector2f newPos(0.0f);
	if (mDisp->mInCave) {
		Vector2f offset;
		offset.x = -0.2f + pos.x * 0.047f;
		offset.y = -0.6f + pos.y * 0.047f;
		newPos.x += offset.x;
		newPos.y += offset.y;

	} else {
		f32 xOffset = newPos.x;
		if (mDisp->mCourseIndex == og::Screen::DispMemberSMenuMap::COURSE_Last) {
			xOffset = (mMapTextureDimensions.x * 1400.0f) / 4705.6f;
		}
		Vector2f offset;
		offset.x = 24.5f + (mMapTextureDimensions.x / 2 + pos.x * 0.058f) + xOffset;
		offset.y = -8.85f + (mMapTextureDimensions.y / 2 + pos.y * 0.058f);
		newPos.x += offset.x;
		newPos.y += offset.y;
	}

	*outPos = newPos;
}

/**
 * @note Address: 0x8030F974
 * @note Size: 0x26C
 */
void ObjSMenuMap::setMapTexture()
{
	mMapTexPane = og::Screen::CopyPictureToPane(mPane_map, mRootPane, 0.0f, 0.0f, 'new_map');
	mMapTexPane->setAlpha(255);
	mPane_map->hide();

	if (mDisp->mInCave && mDisp->mActiveNavi) {
		if (Game::Cave::randMapMgr) {
			Game::Cave::randMapMgr->setCaptureOn();
			mRadarMapTexture = Game::Cave::randMapMgr->getRadarMapTexture();
			mMapTexPane->changeTexture(mRadarMapTexture->mTexInfo, 0);
			mUpdateCaveTex = 1;
		} else {
			JUT_PANICLINE(390, "SMenuMap : randMapMgr is not found!!!\n");
		}
	} else {
		switch (mDisp->mCourseIndex) {
		case 0:
			mMapTexPane->changeTexture("map_tutorial.bti", 0);
			break;
		case 1:
			mMapTexPane->changeTexture("map_forest.bti", 0);
			break;
		case 2:
			mMapTexPane->changeTexture("map_yakushima.bti", 0);
			break;
		case 3:
			mMapTexPane->changeTexture("map_last.bti", 0);
			break;
		}
	}

	mMapTextureDimensions.x = mMapTexPane->getTexture(0)->getSizeX();
	mMapTextureDimensions.y = mMapTexPane->getTexture(0)->getSizeY();
	mMapBounds.x            = mMapTexPane->getWidth();
	mMapBounds.y            = mMapTexPane->getHeight();
	mMapTexPane->resize(mMapTextureDimensions.x, mMapTextureDimensions.y);

	FORCE_DONT_INLINE;
}

/**
 * @note Address: N/A
 * @note Size: 0x1B0
 */
void ObjSMenuMap::setMapPos()
{
	mMapPosition.x = -mMapTextureDimensions.x / 2;
	mMapPosition.y = -mMapTextureDimensions.y / 2;

	if (mDisp->mActiveNavi) {
		Vector3f aNaviPos = Game::naviMgr->getActiveNavi()->getPosition();
		if (mDisp->mInCave) {
			if (Game::Cave::randMapMgr) {
				f32 x, y;
				Game::Cave::randMapMgr->getPositionOnTex(aNaviPos, x, y);
				mMapPosition.x = -(x + -0.2f);
				mMapPosition.y = -(y + -0.6f);
			}
		} else {
			Vector2f naviPos(aNaviPos.x, aNaviPos.z);
			Vector2f mapPos;
			calcMapPos(naviPos, &mapPos);

			mMapPosition.x = -mapPos.x;
			mMapPosition.y = -mapPos.y;
		}
	}
	mMapTexScale.x = mMapBounds.x / mMapTextureDimensions.x;
	mMapTexScale.y = mMapBounds.y / mMapTextureDimensions.y;
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0xBC
 */
void ObjSMenuMap::setCompass()
{
	mPane_Ncompas = mMapCounter->search('Ncompas');
	mCompassPic   = static_cast<J2DPictureEx*>(mIconScreen2->search('compass'));

	J2DPane* iconPane   = mIconScreen->search('compass');
	J2DPane* iconParent = iconPane->getParentPane();
	if (iconParent) {
		iconParent = iconPane->getParentPane();
		iconParent->removeChild(iconPane);
	}
}

/**
 * @note Address: 0x8030FBE0
 * @note Size: 0x1B8
 */
void ObjSMenuMap::tuningIcon()
{
	if (mDisp->mActiveNavi) {
		Game::Navi* navi = Game::naviMgr->getActiveNavi();
		if ((int)navi->mNaviIndex == NAVIID_Olimar) {
			if (mLouieArrow)
				mMapTexPane->appendChild(mLouieArrow);
			if (mOlimarArrow)
				mMapTexPane->appendChild(mOlimarArrow);
		} else {
			if (mOlimarArrow)
				mMapTexPane->appendChild(mOlimarArrow);
			if (mLouieArrow)
				mMapTexPane->appendChild(mLouieArrow);
		}
	}

	for (int i = 0; i < 22; i++) {
		u64 tag        = map_icon_tag[i];
		J2DPane* pane1 = mIconScreen->search(tag);
		if (pane1) {
			pane1->removeFromParent();
		}

		J2DPane* pane2 = mIconScreen2->search(tag);
		if (pane2) {
			pane2->removeFromParent();
		}
	}

	u64 naviTags[3] = { 'orima_l', 'luji_l', 'takara_l' };

	for (int i = 0; i < 3; i++) {
		J2DPane* pane = mIconScreen2->search(naviTags[i]);
		if (pane) {
			pane->removeFromParent();
		}
	}
}

/**
 * @note Address: 0x8030FD98
 * @note Size: 0xC74
 */
void ObjSMenuMap::initMapIcon(JKRArchive* arc)
{
	OSInitFastCast();

	mMapCounter->search('map')->removeFromParent();

	mIconScreen = new P2DScreen::Mgr_tuning;
	mIconScreen->set("map_icon.blo", 0x40000, arc);

	mRootPane = mIconScreen->search('ROOT');

	J2DPictureEx* pic = static_cast<J2DPictureEx*>(og::Screen::TagSearch(mIconScreen, 'piki__b'));
	pic->setWhite(msVal.mTempPikiColorWhite);
	pic->setBlack(msVal.mTempPikiColorBlack);

	mIconScreen2 = new P2DScreen::Mgr_tuning;
	mIconScreen2->set("map_icon.blo", 0x40000, arc);

	setMapTexture();
	setMapPos();
	setCompass();

	mRadarPaneList = new J2DPane**[MAX_RADAR_COUNT];
	for (int i = 0; i < MAX_RADAR_COUNT; i++) {
		J2DPane** pane = new J2DPane*;
		if (pane) {
			*pane = nullptr;
		}
		mRadarPaneList[i] = pane;
	}

	int count       = 0;
	mCaveLabelCount = 0;
	if (Radar::mgr) {
		if (!mDisp->mActiveNavi) {
			Radar::mgr->ogDummpyInit();
		}

		FOREACH_NODE(Radar::Point, Radar::mgr->mPointNode1.mChild, cPoint)
		{
			int id = cPoint->mObjType;
			JUT_ASSERTLINE(569, id >= 0 && id < 22, "Radar type ERR!! (%d)\n", id);
			Vector2f cPos = cPoint->getPosition();
			Vector2f newPos;
			calcMapPos(cPos, &newPos);

			u64 tag             = map_icon_tag[id];
			J2DPictureEx* cPane = static_cast<J2DPictureEx*>(og::Screen::TagSearch(mIconScreen, tag));
			cPane->getTypeID();
			char buf[28];
			og::Screen::TagToName(tag, buf);

			switch (id) {
			case Radar::MAP_OLIMAR:
				Game::Navi* olimar = getNaviPtr(NAVIID_Olimar);
				mOlimarObj         = olimar;
				if (mOlimarObj) {
					J2DPictureEx* olimarPane = static_cast<J2DPictureEx*>(og::Screen::TagSearch(mIconScreen, 'orima_l'));
					mOlimarGlow              = og::Screen::CopyPictureToPane(olimarPane, mMapTexPane, newPos.x, newPos.y, 'ie_Orima');
					mOlimarArrow             = og::Screen::CopyPictureToPane(cPane, mMapTexPane, newPos.x, newPos.y, 'ic_Orima');
				}
				break;

			case Radar::MAP_LOUIE_PRESIDENT:
				Game::Navi* louie = getNaviPtr(NAVIID_Louie);
				mLouieObj         = louie;
				if (mLouieObj) {
					J2DPictureEx* louiePane = static_cast<J2DPictureEx*>(og::Screen::TagSearch(mIconScreen, 'luji_l'));
					mLouieGlow              = og::Screen::CopyPictureToPane(louiePane, mMapTexPane, newPos.x, newPos.y, 'ie_Luji');
					mLouieArrow             = og::Screen::CopyPictureToPane(cPane, mMapTexPane, newPos.x, newPos.y, 'ic_Luji');
				}
				break;

			case Radar::MAP_TREASURE:
			case Radar::MAP_SWALLOWED_TREASURE:
			case Radar::MAP_UPGRADE:
			case Radar::MAP_UNENTERED_CAVE:
				// don't mark these on the map
				break;

			default:
				// mark everything else - pikmin, ship/pod, geyser/hole, complete/incomplete cave
				u64 tag = '_000' + (((count / 1000) * 0x1000 + ((count / 100) * 0x100 + ((count / 10) * 0x10 + (count % 10)))));
				J2DPictureEx* copyPic = og::Screen::CopyPictureToPane(cPane, mMapTexPane, newPos.x, newPos.y, tag);
				if (copyPic) {
					if (id == Radar::MAP_UPGRADE) {
						JUtility::TColor white(msVal.mItemPelletWhiteColor.r, msVal.mItemPelletWhiteColor.g, msVal.mItemPelletWhiteColor.b,
						                       msVal.mItemPelletWhiteColor.a);
						JUtility::TColor black(msVal.mItemPelletBlackColor.r, msVal.mItemPelletBlackColor.g, msVal.mItemPelletBlackColor.b,
						                       msVal.mItemPelletBlackColor.a);
						copyPic->setWhite(white);
						copyPic->setBlack(black);
					}
					if (id == Radar::MAP_INCOMPLETE_CAVE || id == Radar::MAP_COMPLETED_CAVE) {
						appendCaveName(copyPic, (u16)count, og::Screen::maskTag(caveIDtoMsgID(cPoint->mCaveID), 1, 3));
					}
					mRadarPaneList[count][0] = copyPic;
					count++;
				}
				break;
			}
			if (count >= MAX_RADAR_COUNT)
				break;
		}
	}

	mMapIconNum = count;
	tuningIcon();
	/*
	stwu     r1, -0xf0(r1)
	mflr     r0
	stw      r0, 0xf4(r1)
	stfd     f31, 0xe0(r1)
	psq_st   f31, 232(r1), 0, qr0
	stfd     f30, 0xd0(r1)
	psq_st   f30, 216(r1), 0, qr0
	stmw     r21, 0xa4(r1)
	mr       r31, r3
	lis      r5, lbl_8048E1A8@ha
	li       r3, 4
	mr       r23, r4
	addi     r25, r5, lbl_8048E1A8@l
	oris     r3, r3, 4
	mtspr    0x392, r3
	li       r3, 5
	oris     r3, r3, 5
	mtspr    0x393, r3
	li       r3, 6
	oris     r3, r3, 6
	mtspr    0x394, r3
	li       r3, 7
	oris     r3, r3, 7
	mtspr    0x395, r3
	lwz      r3, 0xac(r31)
	lis      r4, 0x006D6170@ha
	li       r5, 0
	lwz      r12, 0(r3)
	addi     r6, r4, 0x006D6170@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r24, r3
	bl       getParentPane__7J2DPaneFv
	cmplwi   r3, 0
	beq      lbl_8030FE38
	mr       r3, r24
	bl       getParentPane__7J2DPaneFv
	mr       r4, r24
	bl       removeChild__7J2DPaneFP7J2DPane

lbl_8030FE38:
	li       r3, 0x148
	bl       __nw__FUl
	or.      r0, r3, r3
	beq      lbl_8030FE50
	bl       __ct__Q29P2DScreen10Mgr_tuningFv
	mr       r0, r3

lbl_8030FE50:
	stw      r0, 0xbc(r31)
	mr       r6, r23
	addi     r4, r25, 0xa8
	lis      r5, 4
	lwz      r3, 0xbc(r31)
	bl       set__9J2DScreenFPCcUlP10JKRArchive
	lwz      r3, 0xbc(r31)
	lis      r4, 0x524F4F54@ha
	addi     r6, r4, 0x524F4F54@l
	li       r5, 0
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	stw      r3, 0xc4(r31)
	lis      r5, 0x695F5F62@ha
	lis      r4, 0x0070696B@ha
	lwz      r3, 0xbc(r31)
	addi     r6, r5, 0x695F5F62@l
	addi     r5, r4, 0x0070696B@l
	bl       TagSearch__Q22og6ScreenFP9J2DScreenUx
	lis      r4, msVal__Q32og9newScreen11ObjSMenuMap@ha
	mr       r24, r3
	addi     r5, r4, msVal__Q32og9newScreen11ObjSMenuMap@l
	lwz      r0, 0x48(r5)
	addi     r4, r1, 0x1c
	stw      r0, 0x1c(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x12c(r12)
	mtctr    r12
	bctrl
	lis      r4, msVal__Q32og9newScreen11ObjSMenuMap@ha
	mr       r3, r24
	addi     r5, r4, msVal__Q32og9newScreen11ObjSMenuMap@l
	addi     r4, r1, 0x18
	lwz      r0, 0x4c(r5)
	stw      r0, 0x18(r1)
	lwz      r12, 0(r24)
	lwz      r12, 0x128(r12)
	mtctr    r12
	bctrl
	li       r3, 0x148
	bl       __nw__FUl
	or.      r0, r3, r3
	beq      lbl_8030FF0C
	bl       __ct__Q29P2DScreen10Mgr_tuningFv
	mr       r0, r3

lbl_8030FF0C:
	stw      r0, 0x124(r31)
	mr       r6, r23
	addi     r4, r25, 0xa8
	lis      r5, 4
	lwz      r3, 0x124(r31)
	bl       set__9J2DScreenFPCcUlP10JKRArchive
	mr       r3, r31
	bl       setMapTexture__Q32og9newScreen11ObjSMenuMapFv
	lfs      f0, 0xf8(r31)
	lfs      f1, lbl_8051D760@sda21(r2)
	fneg     f0, f0
	fmuls    f0, f0, f1
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xfc(r31)
	fneg     f0, f0
	fmuls    f0, f0, f1
	stfs     f0, 0xe4(r31)
	lwz      r4, 0xa8(r31)
	lbz      r0, 0x4a(r4)
	cmplwi   r0, 0
	beq      lbl_803100B0
	lwz      r3, naviMgr__4Game@sda21(r13)
	bl       getActiveNavi__Q24Game7NaviMgrFv
	mr       r4, r3
	addi     r3, r1, 0x7c
	lwz      r12, 0(r4)
	lwz      r12, 8(r12)
	mtctr    r12
	bctrl
	lfs      f2, 0x7c(r1)
	lfs      f1, 0x80(r1)
	lfs      f0, 0x84(r1)
	stfs     f2, 0x70(r1)
	stfs     f1, 0x74(r1)
	stfs     f0, 0x78(r1)
	lwz      r5, 0xa8(r31)
	lbz      r0, 0x49(r5)
	cmplwi   r0, 0
	beq      lbl_8030FFF0
	lwz      r3, randMapMgr__Q24Game4Cave@sda21(r13)
	cmplwi   r3, 0
	beq      lbl_803100B0
	addi     r4, r1, 0x70
	addi     r5, r1, 8
	addi     r6, r1, 0xc
	bl       "getPositionOnTex__Q34Game4Cave10RandMapMgrFR10Vector3<f>RfRf"
	lfs      f2, lbl_8051D748@sda21(r2)
	lfs      f0, 8(r1)
	lfs      f1, lbl_8051D750@sda21(r2)
	fadds    f0, f2, f0
	fneg     f0, f0
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xc(r1)
	fadds    f0, f1, f0
	fneg     f0, f0
	stfs     f0, 0xe4(r31)
	b        lbl_803100B0

lbl_8030FFF0:
	stfs     f0, 0x2c(r1)
	lfs      f8, lbl_8051D740@sda21(r2)
	stfs     f2, 0x28(r1)
	lwz      r0, 0x2c(r1)
	fmr      f7, f8
	lwz      r4, 0x28(r1)
	stw      r0, 0x34(r1)
	stw      r4, 0x30(r1)
	lfs      f4, 0x34(r1)
	beq      lbl_8031003C
	lfs      f3, lbl_8051D74C@sda21(r2)
	lfs      f0, lbl_8051D750@sda21(r2)
	lfs      f2, 0x30(r1)
	lfs      f1, lbl_8051D748@sda21(r2)
	fmadds   f0, f3, f4, f0
	fmadds   f1, f3, f2, f1
	fadds    f7, f7, f0
	fadds    f8, f8, f1
	b        lbl_803100A0

lbl_8031003C:
	lwz      r0, 0x3c(r5)
	fmr      f6, f8
	cmpwi    r0, 3
	bne      lbl_80310060
	lfs      f2, lbl_8051D754@sda21(r2)
	lfs      f1, 0xf8(r31)
	lfs      f0, lbl_8051D758@sda21(r2)
	fmuls    f1, f2, f1
	fdivs    f6, f1, f0

lbl_80310060:
	lfs      f2, lbl_8051D764@sda21(r2)
	lfs      f1, 0x30(r1)
	lfs      f3, 0xf8(r31)
	fmuls    f0, f2, f4
	fmuls    f2, f2, f1
	lfs      f4, lbl_8051D760@sda21(r2)
	lfs      f1, 0xfc(r31)
	lfs      f5, lbl_8051D75C@sda21(r2)
	fmadds   f3, f3, f4, f2
	lfs      f2, lbl_8051D768@sda21(r2)
	fmadds   f0, f1, f4, f0
	fadds    f1, f5, f3
	fadds    f0, f2, f0
	fadds    f1, f6, f1
	fadds    f7, f7, f0
	fadds    f8, f8, f1

lbl_803100A0:
	fneg     f1, f8
	fneg     f0, f7
	stfs     f1, 0xe0(r31)
	stfs     f0, 0xe4(r31)

lbl_803100B0:
	lfs      f1, 0x100(r31)
	lis      r5, 0x6D706173@ha
	lfs      f0, 0xf8(r31)
	lis      r4, 0x004E636F@ha
	addi     r6, r5, 0x6D706173@l
	fdivs    f0, f1, f0
	addi     r5, r4, 0x004E636F@l
	stfs     f0, 0xf0(r31)
	lfs      f1, 0x104(r31)
	lfs      f0, 0xfc(r31)
	fdivs    f0, f1, f0
	stfs     f0, 0xf4(r31)
	lwz      r3, 0xac(r31)
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x128(r31)
	lis      r5, 0x70617373@ha
	lis      r4, 0x00636F6D@ha
	lwz      r3, 0x124(r31)
	addi     r6, r5, 0x70617373@l
	addi     r5, r4, 0x00636F6D@l
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x12c(r31)
	lis      r5, 0x70617373@ha
	lis      r4, 0x00636F6D@ha
	lwz      r3, 0xbc(r31)
	addi     r6, r5, 0x70617373@l
	addi     r5, r4, 0x00636F6D@l
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r23, r3
	bl       getParentPane__7J2DPaneFv
	cmplwi   r3, 0
	beq      lbl_80310164
	mr       r3, r23
	bl       getParentPane__7J2DPaneFv
	mr       r4, r23
	bl       removeChild__7J2DPaneFP7J2DPane

lbl_80310164:
	li       r3, 0x320
	bl       __nwa__FUl
	li       r24, 0
	stw      r3, 0xcc(r31)
	mr       r23, r24
	li       r26, 0

lbl_8031017C:
	li       r3, 4
	bl       __nw__FUl
	cmplwi   r3, 0
	beq      lbl_80310190
	stw      r23, 0(r3)

lbl_80310190:
	lwz      r4, 0xcc(r31)
	addi     r26, r26, 1
	cmpwi    r26, 0xc8
	stwx     r3, r4, r24
	addi     r24, r24, 4
	blt      lbl_8031017C
	li       r0, 0
	li       r29, 0
	stw      r0, 0x154(r31)
	lwz      r3, mgr__5Radar@sda21(r13)
	cmplwi   r3, 0
	beq      lbl_803109DC
	lwz      r4, 0xa8(r31)
	lbz      r0, 0x4a(r4)
	cmplwi   r0, 0
	bne      lbl_803101D4
	bl       ogDummpyInit__Q25Radar3MgrFv

lbl_803101D4:
	lwz      r6, mgr__5Radar@sda21(r13)
	lis      r5, msVal__Q32og9newScreen11ObjSMenuMap@ha
	lis      r4, map_icon_tag__Q22og9newScreen@ha
	li       r26, 0
	lwz      r28, 0x10(r6)
	addi     r30, r5, msVal__Q32og9newScreen11ObjSMenuMap@l
	addi     r24, r4, map_icon_tag__Q22og9newScreen@l
	b        lbl_803109D4

lbl_803101F4:
	lwz      r23, 0x18(r28)
	cmpwi    r23, 0
	blt      lbl_80310208
	cmpwi    r23, 0x16
	blt      lbl_80310220

lbl_80310208:
	mr       r6, r23
	addi     r3, r25, 0xc
	addi     r5, r25, 0xb8
	li       r4, 0x239
	crclr    6
	bl       panic_f__12JUTExceptionFPCciPCce

lbl_80310220:
	mr       r4, r28
	addi     r3, r1, 0x40
	bl       getPosition__Q25Radar5PointFv
	lfs      f0, 0x44(r1)
	lfs      f1, 0x40(r1)
	stfs     f0, 0x4c(r1)
	lwz      r5, 0xa8(r31)
	stfs     f1, 0x48(r1)
	lfs      f30, lbl_8051D740@sda21(r2)
	stfs     f1, 0x4c(r1)
	lbz      r0, 0x49(r5)
	fmr      f31, f30
	stfs     f0, 0x48(r1)
	lwz      r4, 0x4c(r1)
	cmplwi   r0, 0
	lwz      r0, 0x48(r1)
	stw      r4, 0x3c(r1)
	stw      r0, 0x38(r1)
	lfs      f4, 0x3c(r1)
	beq      lbl_80310294
	lfs      f3, lbl_8051D74C@sda21(r2)
	lfs      f0, lbl_8051D750@sda21(r2)
	lfs      f2, 0x38(r1)
	lfs      f1, lbl_8051D748@sda21(r2)
	fmadds   f0, f3, f4, f0
	fmadds   f1, f3, f2, f1
	fadds    f31, f31, f0
	fadds    f30, f30, f1
	b        lbl_803102F8

lbl_80310294:
	lwz      r0, 0x3c(r5)
	fmr      f6, f30
	cmpwi    r0, 3
	bne      lbl_803102B8
	lfs      f2, lbl_8051D754@sda21(r2)
	lfs      f1, 0xf8(r31)
	lfs      f0, lbl_8051D758@sda21(r2)
	fmuls    f1, f2, f1
	fdivs    f6, f1, f0

lbl_803102B8:
	lfs      f2, lbl_8051D764@sda21(r2)
	lfs      f1, 0x38(r1)
	lfs      f3, 0xf8(r31)
	fmuls    f0, f2, f4
	fmuls    f2, f2, f1
	lfs      f4, lbl_8051D760@sda21(r2)
	lfs      f1, 0xfc(r31)
	lfs      f5, lbl_8051D75C@sda21(r2)
	fmadds   f3, f3, f4, f2
	lfs      f2, lbl_8051D768@sda21(r2)
	fmadds   f0, f1, f4, f0
	fadds    f1, f5, f3
	fadds    f0, f2, f0
	fadds    f1, f6, f1
	fadds    f31, f31, f0
	fadds    f30, f30, f1

lbl_803102F8:
	slwi     r0, r23, 3
	lwz      r3, 0xbc(r31)
	add      r4, r24, r0
	lwz      r22, 0(r4)
	lwz      r21, 4(r4)
	mr       r5, r22
	mr       r6, r21
	bl       TagSearch__Q22og6ScreenFP9J2DScreenUx
	lwz      r12, 0(r3)
	mr       r27, r3
	lwz      r12, 0xc(r12)
	mtctr    r12
	bctrl
	mr       r4, r21
	mr       r3, r22
	addi     r5, r1, 0x88
	bl       TagToName__Q22og6ScreenFUxPc
	cmpwi    r23, 0xf
	beq      lbl_80310828
	bge      lbl_80310358
	cmpwi    r23, 0xd
	beq      lbl_803105C8
	bge      lbl_80310364
	b        lbl_80310828

lbl_80310358:
	cmpwi    r23, 0x14
	bge      lbl_80310828
	b        lbl_803109C8

lbl_80310364:
	li       r0, 0
	lwz      r3, naviMgr__4Game@sda21(r13)
	lis      r4, "__vt__22Iterator<Q24Game4Navi>"@ha
	stw      r0, 0x6c(r1)
	addi     r4, r4, "__vt__22Iterator<Q24Game4Navi>"@l
	cmplwi   r0, 0
	stw      r4, 0x60(r1)
	stw      r0, 0x64(r1)
	stw      r3, 0x68(r1)
	bne      lbl_803103A4
	lwz      r12, 0(r3)
	lwz      r12, 0x18(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)
	b        lbl_8031052C

lbl_803103A4:
	lwz      r12, 0(r3)
	lwz      r12, 0x18(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)
	b        lbl_80310410

lbl_803103BC:
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	mr       r4, r3
	lwz      r3, 0x6c(r1)
	lwz      r12, 0(r3)
	lwz      r12, 8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	bne      lbl_8031052C
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)

lbl_80310410:
	lwz      r12, 0x60(r1)
	addi     r3, r1, 0x60
	lwz      r12, 0x10(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_803103BC
	b        lbl_8031052C

lbl_80310430:
	lwz      r3, 0x68(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	lwz      r12, 0(r3)
	mr       r23, r3
	lwz      r12, 0xa8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_80310470
	lhz      r0, 0x2dc(r23)
	cmpwi    r0, 0
	bne      lbl_80310470
	b        lbl_80310550

lbl_80310470:
	lwz      r0, 0x6c(r1)
	cmplwi   r0, 0
	bne      lbl_8031049C
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)
	b        lbl_8031052C

lbl_8031049C:
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)
	b        lbl_80310510

lbl_803104BC:
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	mr       r4, r3
	lwz      r3, 0x6c(r1)
	lwz      r12, 0(r3)
	lwz      r12, 8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	bne      lbl_8031052C
	lwz      r3, 0x68(r1)
	lwz      r4, 0x64(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x64(r1)

lbl_80310510:
	lwz      r12, 0x60(r1)
	addi     r3, r1, 0x60
	lwz      r12, 0x10(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_803104BC

lbl_8031052C:
	lwz      r3, 0x68(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x1c(r12)
	mtctr    r12
	bctrl
	lwz      r4, 0x64(r1)
	cmplw    r4, r3
	bne      lbl_80310430
	li       r23, 0

lbl_80310550:
	stw      r23, 0xd4(r31)
	lwz      r0, 0xd4(r31)
	cmplwi   r0, 0
	beq      lbl_803109C8
	lis      r5, 0x6D615F6C@ha
	lis      r4, 0x006F7269@ha
	lwz      r3, 0xbc(r31)
	addi     r6, r5, 0x6D615F6C@l
	addi     r5, r4, 0x006F7269@l
	bl       TagSearch__Q22og6ScreenFP9J2DScreenUx
	fmr      f1, f30
	lis      r6, 0x72696D61@ha
	lis      r5, 0x69655F4F@ha
	fmr      f2, f31
	lwz      r4, 0xc8(r31)
	addi     r6, r6, 0x72696D61@l
	addi     r5, r5, 0x69655F4F@l
	bl       CopyPictureToPane__Q22og6ScreenFP12J2DPictureExP7J2DPaneffUx
	stw      r3, 0x130(r31)
	lis      r6, 0x72696D61@ha
	fmr      f1, f30
	lis      r5, 0x69635F4F@ha
	fmr      f2, f31
	lwz      r4, 0xc8(r31)
	mr       r3, r27
	addi     r6, r6, 0x72696D61@l
	addi     r5, r5, 0x69635F4F@l
	bl       CopyPictureToPane__Q22og6ScreenFP12J2DPictureExP7J2DPaneffUx
	stw      r3, 0xd0(r31)
	b        lbl_803109C8

lbl_803105C8:
	li       r0, 0
	lwz      r3, naviMgr__4Game@sda21(r13)
	lis      r4, "__vt__22Iterator<Q24Game4Navi>"@ha
	stw      r0, 0x5c(r1)
	addi     r4, r4, "__vt__22Iterator<Q24Game4Navi>"@l
	cmplwi   r0, 0
	stw      r4, 0x50(r1)
	stw      r0, 0x54(r1)
	stw      r3, 0x58(r1)
	bne      lbl_80310608
	lwz      r12, 0(r3)
	lwz      r12, 0x18(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)
	b        lbl_80310790

lbl_80310608:
	lwz      r12, 0(r3)
	lwz      r12, 0x18(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)
	b        lbl_80310674

lbl_80310620:
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	mr       r4, r3
	lwz      r3, 0x5c(r1)
	lwz      r12, 0(r3)
	lwz      r12, 8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	bne      lbl_80310790
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)

lbl_80310674:
	lwz      r12, 0x50(r1)
	addi     r3, r1, 0x50
	lwz      r12, 0x10(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_80310620
	b        lbl_80310790

lbl_80310694:
	lwz      r3, 0x58(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	lwz      r12, 0(r3)
	mr       r23, r3
	lwz      r12, 0xa8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_803106D4
	lhz      r0, 0x2dc(r23)
	cmpwi    r0, 1
	bne      lbl_803106D4
	b        lbl_803107B4

lbl_803106D4:
	lwz      r0, 0x5c(r1)
	cmplwi   r0, 0
	bne      lbl_80310700
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)
	b        lbl_80310790

lbl_80310700:
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)
	b        lbl_80310774

lbl_80310720:
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x20(r12)
	mtctr    r12
	bctrl
	mr       r4, r3
	lwz      r3, 0x5c(r1)
	lwz      r12, 0(r3)
	lwz      r12, 8(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	bne      lbl_80310790
	lwz      r3, 0x58(r1)
	lwz      r4, 0x54(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x14(r12)
	mtctr    r12
	bctrl
	stw      r3, 0x54(r1)

lbl_80310774:
	lwz      r12, 0x50(r1)
	addi     r3, r1, 0x50
	lwz      r12, 0x10(r12)
	mtctr    r12
	bctrl
	clrlwi.  r0, r3, 0x18
	beq      lbl_80310720

lbl_80310790:
	lwz      r3, 0x58(r1)
	lwz      r12, 0(r3)
	lwz      r12, 0x1c(r12)
	mtctr    r12
	bctrl
	lwz      r4, 0x54(r1)
	cmplw    r4, r3
	bne      lbl_80310694
	li       r23, 0

lbl_803107B4:
	stw      r23, 0xdc(r31)
	lwz      r0, 0xdc(r31)
	cmplwi   r0, 0
	beq      lbl_803109C8
	lis      r4, 0x6A695F6C@ha
	lwz      r3, 0xbc(r31)
	addi     r6, r4, 0x6A695F6C@l
	li       r5, 0x6c75
	bl       TagSearch__Q22og6ScreenFP9J2DScreenUx
	fmr      f1, f30
	lis      r6, 0x4C756A69@ha
	lis      r5, 0x0069655F@ha
	fmr      f2, f31
	lwz      r4, 0xc8(r31)
	addi     r6, r6, 0x4C756A69@l
	addi     r5, r5, 0x0069655F@l
	bl       CopyPictureToPane__Q22og6ScreenFP12J2DPictureExP7J2DPaneffUx
	stw      r3, 0x134(r31)
	lis      r6, 0x4C756A69@ha
	fmr      f1, f30
	lis      r5, 0x0069635F@ha
	fmr      f2, f31
	lwz      r4, 0xc8(r31)
	mr       r3, r27
	addi     r6, r6, 0x4C756A69@l
	addi     r5, r5, 0x0069635F@l
	bl       CopyPictureToPane__Q22og6ScreenFP12J2DPictureExP7J2DPaneffUx
	stw      r3, 0xd8(r31)
	b        lbl_803109C8

lbl_80310828:
	lis      r4, 0x66666667@ha
	lis      r5, 0x5F303030@ha
	addi     r0, r4, 0x66666667@l
	lis      r6, 0x69636F6E@ha
	mulhw    r11, r0, r29
	addi     r7, r5, 0x5F303030@l
	lis      r4, 0x51EB851F@ha
	fmr      f1, f30
	addi     r6, r6, 0x69636F6E@l
	fmr      f2, f31
	srawi    r8, r11, 2
	addi     r5, r4, 0x51EB851F@l
	srwi     r9, r8, 0x1f
	lwz      r4, 0xc8(r31)
	add      r8, r8, r9
	mr       r3, r27
	mulli    r8, r8, 0xa
	subf     r9, r8, r29
	srawi    r8, r9, 0x1f
	addc     r10, r9, r7
	adde     r9, r8, r6
	srawi    r6, r11, 2
	srwi     r7, r6, 0x1f
	add      r8, r6, r7
	mulhw    r6, r0, r8
	srawi    r6, r6, 2
	srwi     r7, r6, 0x1f
	add      r6, r6, r7
	mulli    r6, r6, 0xa
	subf     r6, r6, r8
	slwi     r7, r6, 8
	srawi    r6, r7, 0x1f
	addc     r8, r10, r7
	mulhw    r5, r5, r29
	adde     r7, r9, r6
	srawi    r5, r5, 5
	srwi     r6, r5, 0x1f
	add      r6, r5, r6
	mulhw    r0, r0, r6
	srawi    r0, r0, 2
	srwi     r5, r0, 0x1f
	add      r0, r0, r5
	mulli    r0, r0, 0xa
	subf     r0, r0, r6
	slwi     r5, r0, 0x10
	srawi    r0, r5, 0x1f
	addc     r6, r8, r5
	adde     r5, r7, r0
	bl       CopyPictureToPane__Q22og6ScreenFP12J2DPictureExP7J2DPaneffUx
	or.      r27, r3, r3
	beq      lbl_803109C8
	cmpwi    r23, 0x12
	bne      lbl_80310978
	lbz      r7, 0x13(r30)
	addi     r4, r1, 0x14
	lbz      r6, 0x14(r30)
	lbz      r5, 0x15(r30)
	lbz      r0, 0x16(r30)
	stb      r7, 0x24(r1)
	lbz      r8, 0x17(r30)
	stb      r6, 0x25(r1)
	lbz      r7, 0x18(r30)
	stb      r5, 0x26(r1)
	lbz      r6, 0x19(r30)
	stb      r0, 0x27(r1)
	lbz      r5, 0x1a(r30)
	lwz      r0, 0x24(r1)
	stb      r8, 0x20(r1)
	stw      r0, 0x14(r1)
	lwz      r12, 0(r3)
	stb      r7, 0x21(r1)
	lwz      r12, 0x12c(r12)
	stb      r6, 0x22(r1)
	stb      r5, 0x23(r1)
	mtctr    r12
	bctrl
	lwz      r0, 0x20(r1)
	mr       r3, r27
	addi     r4, r1, 0x10
	stw      r0, 0x10(r1)
	lwz      r12, 0(r27)
	lwz      r12, 0x128(r12)
	mtctr    r12
	bctrl

lbl_80310978:
	cmpwi    r23, 0xb
	beq      lbl_80310988
	cmpwi    r23, 0x14
	bne      lbl_803109B4

lbl_80310988:
	lwz      r3, 0x20(r28)
	bl       caveIDtoMsgID__Q22og9newScreenFUl
	li       r5, 1
	li       r6, 3
	bl       maskTag__Q22og6ScreenFUxUsUs
	mr       r7, r3
	mr       r8, r4
	mr       r3, r31
	mr       r4, r27
	clrlwi   r5, r29, 0x10
	bl       appendCaveName__Q32og9newScreen11ObjSMenuMapFP7J2DPaneUsUx

lbl_803109B4:
	lwz      r4, 0xcc(r31)
	addi     r29, r29, 1
	lwzx     r4, r4, r26
	addi     r26, r26, 4
	stw      r27, 0(r4)

lbl_803109C8:
	cmpwi    r29, 0xc8
	bge      lbl_803109DC
	lwz      r28, 4(r28)

lbl_803109D4:
	cmplwi   r28, 0
	bne      lbl_803101F4

lbl_803109DC:
	stw      r29, 0x11c(r31)
	mr       r3, r31
	bl       tuningIcon__Q32og9newScreen11ObjSMenuMapFv
	psq_l    f31, 232(r1), 0, qr0
	lfd      f31, 0xe0(r1)
	psq_l    f30, 216(r1), 0, qr0
	lfd      f30, 0xd0(r1)
	lmw      r21, 0xa4(r1)
	lwz      r0, 0xf4(r1)
	mtlr     r0
	addi     r1, r1, 0xf0
	blr
	*/
}

/**
 * @note Address: 0x80310A0C
 * @note Size: 0x1E4
 */
void ObjSMenuMap::appendCaveName(J2DPane* parent, u16 caveIndex, u64 tag)
{
	char buf[32];
	u64 newtag = og::Screen::maskTag2('caveTx??', caveIndex);
	og::Screen::TagToName(tag, buf);
	const JGeometry::TBox2f box(30.0f, 0.0f, 40.0f, 10.0f);

	J2DTextBox* pane = new J2DTextBox(newtag, box, (const ResFONT*)nullptr, "", -1, J2DHBIND_Left, J2DVBIND_Top);
	pane->mFontSize  = 24.0f;

	pane->setCharColor(JUtility::TColor(255, 255, 255, 255));
	pane->setGradientColor(JUtility::TColor(255, 255, 255, 255));
	pane->setBlackWhite(JUtility::TColor(0, 0, 0, 0), JUtility::TColor(255, 255, 255, 255));

	parent->appendChild(pane);
	pane->setMsgID(tag);

	if (mCaveLabelCount < MAX_CAVEDISP_NAME) {
		mCaveLabelTextBoxes[mCaveLabelCount] = pane;
		mCaveLabelCount++;
	} else {
		JUT_PANICLINE(745, "cave name number is overflow!!\n");
	}

	/*
	.loc_0x0:
	  stwu      r1, -0x70(r1)
	  mflr      r0
	  lis       r9, 0x5478
	  lis       r6, 0x6361
	  stw       r0, 0x74(r1)
	  stmw      r25, 0x54(r1)
	  mr        r27, r3
	  mr        r28, r4
	  mr        r30, r7
	  mr        r29, r8
	  addi      r4, r9, 0x3F3F
	  addi      r3, r6, 0x7665
	  bl        -0xE228
	  mr        r25, r4
	  mr        r26, r3
	  mr        r4, r29
	  mr        r3, r30
	  addi      r5, r1, 0x40
	  bl        -0xDE88
	  lfs       f3, -0xBE8(r2)
	  li        r3, 0x138
	  lfs       f2, -0xC20(r2)
	  lfs       f1, -0xBE4(r2)
	  lfs       f0, -0xBE0(r2)
	  stfs      f3, 0x30(r1)
	  stfs      f2, 0x34(r1)
	  stfs      f1, 0x38(r1)
	  stfs      f0, 0x3C(r1)
	  bl        -0x2ECBD8
	  mr.       r31, r3
	  beq-      .loc_0xA8
	  li        r0, 0x2
	  mr        r6, r25
	  stw       r0, 0x8(r1)
	  mr        r5, r26
	  addi      r7, r1, 0x30
	  li        r8, 0
	  stw       r0, 0xC(r1)
	  subi      r9, r2, 0xBDC
	  li        r10, -0x1
	  bl        -0x2CF5C0
	  mr        r31, r3

	.loc_0xA8:
	  li        r8, 0xFF
	  li        r0, 0
	  stb       r8, 0x2C(r1)
	  mr        r3, r31
	  lfs       f0, -0xBD8(r2)
	  addi      r4, r1, 0x14
	  stb       r8, 0x2D(r1)
	  addi      r5, r1, 0x10
	  stfs      f0, 0x11C(r31)
	  stb       r8, 0x2E(r1)
	  stb       r8, 0x2F(r1)
	  lwz       r6, 0x2C(r1)
	  stfs      f0, 0x120(r31)
	  stw       r6, 0x1C(r1)
	  lbz       r7, 0x1C(r1)
	  stb       r8, 0x28(r1)
	  lbz       r6, 0x1D(r1)
	  stb       r7, 0x104(r31)
	  lbz       r7, 0x1E(r1)
	  stb       r6, 0x105(r31)
	  lbz       r6, 0x1F(r1)
	  stb       r7, 0x106(r31)
	  stb       r8, 0x29(r1)
	  stb       r8, 0x2A(r1)
	  stb       r8, 0x2B(r1)
	  lwz       r7, 0x28(r1)
	  stb       r6, 0x107(r31)
	  stw       r7, 0x18(r1)
	  lbz       r6, 0x18(r1)
	  lbz       r7, 0x19(r1)
	  stb       r6, 0x108(r31)
	  lbz       r6, 0x1A(r1)
	  stb       r7, 0x109(r31)
	  lbz       r7, 0x1B(r1)
	  stb       r6, 0x10A(r31)
	  stb       r8, 0x24(r1)
	  stb       r8, 0x25(r1)
	  stb       r8, 0x26(r1)
	  stb       r8, 0x27(r1)
	  stb       r0, 0x20(r1)
	  lwz       r6, 0x24(r1)
	  stb       r7, 0x10B(r31)
	  stb       r0, 0x21(r1)
	  stb       r0, 0x22(r1)
	  stb       r0, 0x23(r1)
	  lwz       r0, 0x20(r1)
	  stw       r6, 0x10(r1)
	  stw       r0, 0x14(r1)
	  lwz       r12, 0x0(r31)
	  lwz       r12, 0xAC(r12)
	  mtctr     r12
	  bctrl
	  mr        r3, r28
	  mr        r4, r31
	  bl        -0x2D94C8
	  stw       r29, 0x1C(r31)
	  stw       r30, 0x18(r31)
	  lwz       r0, 0x154(r27)
	  cmpwi     r0, 0x5
	  bge-      .loc_0x1B4
	  rlwinm    r0,r0,2,0,29
	  add       r3, r27, r0
	  stw       r31, 0x140(r3)
	  lwz       r3, 0x154(r27)
	  addi      r0, r3, 0x1
	  stw       r0, 0x154(r27)
	  b         .loc_0x1D0

	.loc_0x1B4:
	  lis       r3, 0x8049
	  lis       r5, 0x8049
	  subi      r3, r3, 0x1E4C
	  li        r4, 0x2E9
	  subi      r5, r5, 0x1D88
	  crclr     6, 0x6
	  bl        -0x2E6598

	.loc_0x1D0:
	  lmw       r25, 0x54(r1)
	  lwz       r0, 0x74(r1)
	  mtlr      r0
	  addi      r1, r1, 0x70
	  blr
	*/
}

// /**
//  * @note Address: N/A
//  * @note Size: 0x4
//  */
// void ObjSMenuMap::rotateMap()
// {
// 	// UNUSED FUNCTION
// }

/**
 * @note Address: 0x80310BF0
 * @note Size: 0x2E8
 */
void ObjSMenuMap::transMap()
{
	f32 factor = msVal.mMapMoveRate;
	if (mRadarMapTexture) {
		factor = msVal.mMapMoveRate2;
	}

	f32 angle    = (mMapAngle * TAU) / 360.0f;
	f32 anglecos = cosf(angle);
	f32 anglesin = sinf(angle);
	int buttons  = mController->getButton();
	f32 inputx   = mController->getMainStickX();
	f32 inputz   = mController->getMainStickY();

	if (buttons & JUTGamePad::PRESS_DPAD_UP) {
		inputz = 1.0f;
	}
	if (buttons & JUTGamePad::PRESS_DPAD_DOWN) {
		inputz = -1.0f;
	}
	if (buttons & JUTGamePad::PRESS_DPAD_LEFT) {
		inputx = -1.0f;
	}
	if (buttons & JUTGamePad::PRESS_DPAD_RIGHT) {
		inputx = 1.0f;
	}
	f32 diff = _sqrtf(inputx * inputx + inputz * inputz);
	if (diff > 1.0f)
		diff = 1.0f;
	diff *= msVal.mMapMoveInputReduction;
	if (diff < 0.1f)
		diff = 0.0f;
	f32 move = factor * diff * (1.0f / mCurrentZoom);

	if (inputx > 0.1f) {
		mMapPosition.x -= move * anglesin;
		mMapPosition.y -= move * anglecos;
		ogSound->setScroll();
	}
	if (inputx < -0.1f) {
		mMapPosition.x += move * anglesin;
		mMapPosition.y += move * anglecos;
		ogSound->setScroll();
	}
	if (inputz > 0.1f) {
		mMapPosition.x -= move * anglecos;
		mMapPosition.y += move * anglesin;
		ogSound->setScroll();
	}
	if (inputx < -0.1f) {
		mMapPosition.x += move * anglecos;
		mMapPosition.y -= move * anglesin;
		ogSound->setScroll();
	}

	if (mMapPosition.x < -mMapTextureDimensions.x)
		mMapPosition.x = -mMapTextureDimensions.x;
	if (mMapPosition.x > 0.0f)
		mMapPosition.x = 0.0f;

	if (mMapPosition.y < -mMapTextureDimensions.y)
		mMapPosition.y = -mMapTextureDimensions.y;
	if (mMapPosition.y > 0.0f)
		mMapPosition.y = 0.0f;
	/*
	stwu     r1, -0x80(r1)
	mflr     r0
	stw      r0, 0x84(r1)
	stfd     f31, 0x70(r1)
	psq_st   f31, 120(r1), 0, qr0
	stfd     f30, 0x60(r1)
	psq_st   f30, 104(r1), 0, qr0
	stfd     f29, 0x50(r1)
	psq_st   f29, 88(r1), 0, qr0
	stfd     f28, 0x40(r1)
	psq_st   f28, 72(r1), 0, qr0
	stfd     f27, 0x30(r1)
	psq_st   f27, 56(r1), 0, qr0
	stw      r31, 0x2c(r1)
	mr       r31, r3
	lis      r3, msVal__Q32og9newScreen11ObjSMenuMap@ha
	lwz      r0, 0xc0(r31)
	addi     r3, r3, msVal__Q32og9newScreen11ObjSMenuMap@l
	lfs      f0, 0x1c(r3)
	cmplwi   r0, 0
	beq      lbl_80310C48
	lfs      f0, 0x20(r3)

lbl_80310C48:
	lfs      f3, lbl_8051D78C@sda21(r2)
	lfs      f1, 0xec(r31)
	lfs      f2, lbl_8051D790@sda21(r2)
	fmuls    f3, f3, f1
	lfs      f1, lbl_8051D740@sda21(r2)
	fdivs    f4, f3, f2
	fmr      f2, f4
	fcmpo    cr0, f4, f1
	bge      lbl_80310C70
	fneg     f2, f4

lbl_80310C70:
	lfs      f3, lbl_8051D794@sda21(r2)
	lis      r3, sincosTable___5JMath@ha
	lfs      f1, lbl_8051D740@sda21(r2)
	addi     r4, r3, sincosTable___5JMath@l
	fmuls    f2, f2, f3
	fcmpo    cr0, f4, f1
	fctiwz   f1, f2
	stfd     f1, 8(r1)
	lwz      r0, 0xc(r1)
	rlwinm   r0, r0, 3, 0x12, 0x1c
	add      r3, r4, r0
	lfs      f31, 4(r3)
	bge      lbl_80310CC8
	lfs      f1, lbl_8051D798@sda21(r2)
	fmuls    f1, f4, f1
	fctiwz   f1, f1
	stfd     f1, 0x10(r1)
	lwz      r0, 0x14(r1)
	rlwinm   r0, r0, 3, 0x12, 0x1c
	lfsx     f1, r4, r0
	fneg     f30, f1
	b        lbl_80310CE0

lbl_80310CC8:
	fmuls    f1, f4, f3
	fctiwz   f1, f1
	stfd     f1, 0x18(r1)
	lwz      r0, 0x1c(r1)
	rlwinm   r0, r0, 3, 0x12, 0x1c
	lfsx     f30, r4, r0

lbl_80310CE0:
	lwz      r3, 0x118(r31)
	lwz      r4, 0x18(r3)
	lfs      f28, 0x48(r3)
	rlwinm.  r0, r4, 0, 0x1c, 0x1c
	lfs      f27, 0x4c(r3)
	beq      lbl_80310CFC
	lfs      f27, lbl_8051D744@sda21(r2)

lbl_80310CFC:
	rlwinm.  r0, r4, 0, 0x1d, 0x1d
	beq      lbl_80310D08
	lfs      f27, lbl_8051D79C@sda21(r2)

lbl_80310D08:
	clrlwi.  r0, r4, 0x1f
	beq      lbl_80310D14
	lfs      f28, lbl_8051D79C@sda21(r2)

lbl_80310D14:
	rlwinm.  r0, r4, 0, 0x1e, 0x1e
	beq      lbl_80310D20
	lfs      f28, lbl_8051D744@sda21(r2)

lbl_80310D20:
	fmuls    f3, f28, f28
	lfs      f1, lbl_8051D740@sda21(r2)
	fmuls    f2, f27, f27
	fadds    f2, f3, f2
	fcmpo    cr0, f2, f1
	ble      lbl_80310D48
	ble      lbl_80310D4C
	frsqrte  f1, f2
	fmuls    f2, f1, f2
	b        lbl_80310D4C

lbl_80310D48:
	fmr      f2, f1

lbl_80310D4C:
	lfs      f1, lbl_8051D744@sda21(r2)
	fmr      f3, f2
	fcmpo    cr0, f2, f1
	ble      lbl_80310D60
	fmr      f3, f1

lbl_80310D60:
	lis      r3, msVal__Q32og9newScreen11ObjSMenuMap@ha
	lfs      f1, lbl_8051D7A0@sda21(r2)
	addi     r3, r3, msVal__Q32og9newScreen11ObjSMenuMap@l
	lfs      f2, 0x3c(r3)
	fmuls    f4, f3, f2
	fcmpo    cr0, f4, f1
	bge      lbl_80310D80
	lfs      f4, lbl_8051D740@sda21(r2)

lbl_80310D80:
	lfs      f3, lbl_8051D744@sda21(r2)
	lfs      f2, 0xe8(r31)
	lfs      f1, lbl_8051D7A0@sda21(r2)
	fdivs    f2, f3, f2
	fmuls    f0, f0, f2
	fcmpo    cr0, f28, f1
	fmuls    f29, f4, f0
	ble      lbl_80310DC0
	lfs      f0, 0xe0(r31)
	fnmsubs  f0, f29, f31, f0
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xe4(r31)
	fnmsubs  f0, f29, f30, f0
	stfs     f0, 0xe4(r31)
	lwz      r3, ogSound__2og@sda21(r13)
	bl       setScroll__Q22og5SoundFv

lbl_80310DC0:
	lfs      f0, lbl_8051D7A4@sda21(r2)
	fcmpo    cr0, f28, f0
	bge      lbl_80310DEC
	lfs      f0, 0xe0(r31)
	fmadds   f0, f29, f31, f0
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xe4(r31)
	fmadds   f0, f29, f30, f0
	stfs     f0, 0xe4(r31)
	lwz      r3, ogSound__2og@sda21(r13)
	bl       setScroll__Q22og5SoundFv

lbl_80310DEC:
	lfs      f0, lbl_8051D7A0@sda21(r2)
	fcmpo    cr0, f27, f0
	ble      lbl_80310E18
	lfs      f0, 0xe0(r31)
	fnmsubs  f0, f29, f30, f0
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xe4(r31)
	fmadds   f0, f29, f31, f0
	stfs     f0, 0xe4(r31)
	lwz      r3, ogSound__2og@sda21(r13)
	bl       setScroll__Q22og5SoundFv

lbl_80310E18:
	lfs      f0, lbl_8051D7A4@sda21(r2)
	fcmpo    cr0, f27, f0
	bge      lbl_80310E44
	lfs      f0, 0xe0(r31)
	fmadds   f0, f29, f30, f0
	stfs     f0, 0xe0(r31)
	lfs      f0, 0xe4(r31)
	fnmsubs  f0, f29, f31, f0
	stfs     f0, 0xe4(r31)
	lwz      r3, ogSound__2og@sda21(r13)
	bl       setScroll__Q22og5SoundFv

lbl_80310E44:
	lfs      f0, 0xf8(r31)
	lfs      f1, 0xe0(r31)
	fneg     f0, f0
	fcmpo    cr0, f1, f0
	bge      lbl_80310E5C
	stfs     f0, 0xe0(r31)

lbl_80310E5C:
	lfs      f1, 0xe0(r31)
	lfs      f0, lbl_8051D740@sda21(r2)
	fcmpo    cr0, f1, f0
	ble      lbl_80310E70
	stfs     f0, 0xe0(r31)

lbl_80310E70:
	lfs      f0, 0xfc(r31)
	lfs      f1, 0xe4(r31)
	fneg     f0, f0
	fcmpo    cr0, f1, f0
	bge      lbl_80310E88
	stfs     f0, 0xe4(r31)

lbl_80310E88:
	lfs      f1, 0xe4(r31)
	lfs      f0, lbl_8051D740@sda21(r2)
	fcmpo    cr0, f1, f0
	ble      lbl_80310E9C
	stfs     f0, 0xe4(r31)

lbl_80310E9C:
	psq_l    f31, 120(r1), 0, qr0
	lfd      f31, 0x70(r1)
	psq_l    f30, 104(r1), 0, qr0
	lfd      f30, 0x60(r1)
	psq_l    f29, 88(r1), 0, qr0
	lfd      f29, 0x50(r1)
	psq_l    f28, 72(r1), 0, qr0
	lfd      f28, 0x40(r1)
	psq_l    f27, 56(r1), 0, qr0
	lfd      f27, 0x30(r1)
	lwz      r0, 0x84(r1)
	lwz      r31, 0x2c(r1)
	mtlr     r0
	addi     r1, r1, 0x80
	blr
	*/
}

/**
 * @note Address: N/A
 * @note Size: 0xC4
 */
void ObjSMenuMap::scaleMap()
{
	// i will fix this sdata2 if it kills me
	mMapTexScale.x = 0.4f;
	mMapTexScale.x = -0.4f;
	mMapTexScale.x = 0.03f;
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0xB0
 */
void ObjSMenuMap::setMapColor()
{
	JUtility::TColor color(msVal.mMapTexColorWhite.r, msVal.mMapTexColorWhite.g, msVal.mMapTexColorWhite.b, msVal.mMapTexColorWhite.a);
	JUtility::TColor color2(msVal.mMapTexColorBlack.r, msVal.mMapTexColorBlack.g, msVal.mMapTexColorBlack.b, msVal.mMapTexColorBlack.a);
	mMapTexPane->setWhite(color);
	mMapTexPane->setBlack(color2);
}

/**
 * @note Address: N/A
 * @note Size: 0x54
 */
u8 ObjSMenuMap::calcCaveNameAlpha()
{
	u8 alpha = 255;

	if (mCurrentZoom < mStartZoom) {
		alpha = (1.0 - (mStartZoom - mCurrentZoom) / (mStartZoom - msVal.mMinZoom)) * 255.0f;
	}
	mZoomCaveTextAlpha = alpha;
}

/**
 * @note Address: 0x80310ED8
 * @note Size: 0x960
 */
void ObjSMenuMap::doCreate(JKRArchive* arc)
{
	og::Screen::DispMemberSMenuAll* dispfull = static_cast<og::Screen::DispMemberSMenuAll*>(getDispMember());
	mDisp = static_cast<og::Screen::DispMemberSMenuMap*>(dispfull->getSubMember(OWNER_OGA, MEMBER_START_MENU_MAP));
	if (!mDisp) {
		dispfull = new og::Screen::DispMemberSMenuAll;
		mDisp    = static_cast<og::Screen::DispMemberSMenuMap*>(dispfull->getSubMember(OWNER_OGA, MEMBER_START_MENU_MAP));
	}

	mMapCounter = new og::Screen::MapCounter(&mDisp->mDataMap);
	mMapCounter->set("s_menu_map_l.blo", 0x1040000, arc);
	mAnimGroup = new og::Screen::AnimGroup(3);
	og::Screen::registAnimGroupScreen(mAnimGroup, arc, mMapCounter, "s_menu_map_l.btk", msBaseVal.mAnimSpeed);
	og::Screen::registAnimGroupScreen(mAnimGroup, arc, mMapCounter, "s_menu_map_l_02.btk", msBaseVal.mAnimSpeed);
	og::Screen::registAnimGroupScreen(mAnimGroup, arc, mMapCounter, "s_menu_map_l_03.btk", msBaseVal.mAnimSpeed);
	mMapCounter->setCallBack(arc);
	mPane_map = static_cast<J2DPictureEx*>(og::Screen::TagSearch(mMapCounter, 'map_cent'));

	if (mDisp->mInCave && mDisp->mActiveNavi) {
		mStartZoom = msVal.mCaveZoom;
	} else {
		mStartZoom = msVal.mGroundZoom;
	}
	mCurrentZoom = mStartZoom;
	mMapAngle    = 0.0f;
	if (mDisp->mActiveNavi) {
		Game::Navi* navi = Game::naviMgr->getActiveNavi();
		Vector3f vec     = Game::cameraMgr->mCameraObjList[navi->mNaviIndex]->getViewVector();
		mMapAngle        = 180.0f * (JMAAtan2Radian(vec.x, -vec.z) / PI);
	}
	initMapIcon(arc);
	setMapColor();

	u64 tag = 0;
	if (mDisp->mInCave && mDisp->mActiveNavi) {
		char buf[20];
		tag = caveIDtoMsgID(mDisp->mCurrentCave);
		tag = og::Screen::maskTag(tag, 1, 3);
		og::Screen::TagToName(tag, buf);
	} else {
		int stage = mDisp->mCourseIndex;
		switch (stage) {
		case 0:
			tag = '8390_03';
			break;
		case 1:
			tag = '8391_03';
			break;
		case 2:
			tag = '8392_03';
			break;
		case 3:
			tag = '8393_03';
			break;
		case 4:
			tag = '8394_03';
			break;
		default:
			break;
		}
	}
	J2DPane* pane    = mMapCounter->search('Tmapti');
	pane->mMessageID = tag;
	og::Screen::setCallBackMessage(mIconScreen);

	J2DPane* pane_red    = mMapCounter->search('Npk01');
	J2DPane* pane_yellow = mMapCounter->search('Npk02');
	J2DPane* pane_blue   = mMapCounter->search('Npk03');
	J2DPane* pane_white  = mMapCounter->search('Npk04');
	J2DPane* pane_purple = mMapCounter->search('Npk05');

	J2DPane* pane_red2    = mMapCounter->search('Npk06');
	J2DPane* pane_yellow2 = mMapCounter->search('Npk07');
	J2DPane* pane_blue2   = mMapCounter->search('Npk08');
	J2DPane* pane_white2  = mMapCounter->search('Npk09');
	J2DPane* pane_purple2 = mMapCounter->search('Npk10');
	J2DPane* pane_free    = mMapCounter->search('Npk11');

	if (!mDisp->mUnlockedReds) {
		pane_red->hide();
		pane_red2->hide();
		mMapCounter->dispRed(false);
	}
	if (!mDisp->mUnlockedYellows) {
		pane_yellow->hide();
		pane_yellow2->hide();
		mMapCounter->dispYellow(false);
	}
	if (!mDisp->mUnlockedBlues) {
		pane_blue->hide();
		pane_blue2->hide();
		mMapCounter->dispBlue(false);
	}
	if (!mDisp->mUnlockedWhites) {
		pane_white->hide();
		pane_white2->hide();
		mMapCounter->dispWhite(false);
	}
	if (!mDisp->mUnlockedPurples) {
		pane_purple->hide();
		pane_purple2->hide();
		mMapCounter->dispBlack(false);
	}
	if (!mDisp->mUnlockedReds && !mDisp->mUnlockedYellows && !mDisp->mUnlockedBlues && !mDisp->mUnlockedWhites
	    && !mDisp->mUnlockedPurples) {
		pane_free->hide();
		mMapCounter->dispFree(false);
	}

	J2DPane* pane_rocket = mMapCounter->search('Nrocket');
	if (mDisp->mUnlockedWhites || mDisp->mUnlockedPurples) {
		pane_rocket->show();
		J2DPane* pane_rock1 = mMapCounter->search('Nrock_1');
		J2DPane* pane_rock2 = mMapCounter->search('Nrock_2');
		pane_rock1->hide();
		pane_rock2->hide();
		og::Screen::DispMemberSMenuPause* disp2
		    = static_cast<og::Screen::DispMemberSMenuPause*>(dispfull->getSubMember(OWNER_OGA, MEMBER_START_MENU_PAUSE));
		if (disp2->mPokoCount >= 10000) {
			pane_rock2->show();
		} else {
			pane_rock1->show();
		}
	} else {
		pane_rocket->hide();
		pane_rocket = mMapCounter->search('Ntairetu');
		pane_rocket->add(0.0f, -50.0f);
	}
	J2DPane* pane_onyn1 = mMapCounter->search('Nonyn_1');
	J2DPane* pane_onyn2 = mMapCounter->search('Nonyn_2');
	J2DPane* pane_onyn3 = mMapCounter->search('Nonyn_3');
	J2DPane* pane_onyn4 = mMapCounter->search('Nonyn_4');
	pane_onyn1->hide();
	pane_onyn2->hide();
	pane_onyn3->hide();
	pane_onyn4->hide();
	if (mDisp->mUnlockedReds && mDisp->mUnlockedYellows && mDisp->mUnlockedBlues) {
		pane_onyn4->show();
	} else if (mDisp->mUnlockedReds && mDisp->mUnlockedBlues) {
		pane_onyn3->show();
	} else if (mDisp->mUnlockedReds && mDisp->mUnlockedYellows) {
		pane_onyn2->show();
	} else if (mDisp->mUnlockedReds) {
		pane_onyn1->show();
	}

	J2DPane* pane_ntai1 = mMapCounter->search('Ntai_1');
	J2DPane* pane_ntai2 = mMapCounter->search('Ntai_2');
	pane_ntai1->hide();
	pane_ntai2->hide();
	if (mDisp->mUnlockedBlues) {
		pane_ntai1->show();
	}
	if (mDisp->mUnlockedYellows) {
		pane_ntai2->show();
	}
	doCreateAfter(arc, mMapCounter);

	/*
	stwu     r1, -0x70(r1)
	mflr     r0
	lis      r5, lbl_8048E1A8@ha
	stw      r0, 0x74(r1)
	stmw     r18, 0x38(r1)
	mr       r29, r3
	mr       r30, r4
	addi     r18, r5, lbl_8048E1A8@l
	bl       getDispMember__Q26Screen7ObjBaseFv
	lis      r4, 0x004F4741@ha
	lis      r6, 0x5F4D4150@ha
	li       r5, 0x534d
	mr       r31, r3
	addi     r4, r4, 0x004F4741@l
	addi     r6, r6, 0x5F4D4150@l
	bl       getSubMember__Q32og6Screen14DispMemberBaseFUlUx
	stw      r3, 0xa8(r29)
	lwz      r0, 0xa8(r29)
	cmplwi   r0, 0
	bne      lbl_80310F64
	li       r3, 0xc4
	bl       __nw__FUl
	or.      r0, r3, r3
	beq      lbl_80310F40
	bl       __ct__Q32og6Screen18DispMemberSMenuAllFv
	mr       r0, r3

lbl_80310F40:
	lis      r4, 0x004F4741@ha
	lis      r5, 0x5F4D4150@ha
	mr       r31, r0
	mr       r3, r0
	addi     r4, r4, 0x004F4741@l
	addi     r6, r5, 0x5F4D4150@l
	li       r5, 0x534d
	bl       getSubMember__Q32og6Screen14DispMemberBaseFUlUx
	stw      r3, 0xa8(r29)

lbl_80310F64:
	li       r3, 0x1a8
	bl       __nw__FUl
	or.      r0, r3, r3
	beq      lbl_80310F84
	lwz      r4, 0xa8(r29)
	addi     r4, r4, 8
	bl       __ct__Q32og6Screen10MapCounterFPQ32og6Screen7DataMap
	mr       r0, r3

lbl_80310F84:
	stw      r0, 0xac(r29)
	mr       r6, r30
	addi     r4, r18, 0xf0
	lis      r5, 0x104
	lwz      r3, 0xac(r29)
	bl       set__9J2DScreenFPCcUlP10JKRArchive
	li       r3, 0x1c
	bl       __nw__FUl
	or.      r0, r3, r3
	beq      lbl_80310FB8
	li       r4, 3
	bl       __ct__Q32og6Screen9AnimGroupFi
	mr       r0, r3

lbl_80310FB8:
	stw      r0, 0xb0(r29)
	lis      r3, msBaseVal__Q32og9newScreen12ObjSMenuBase@ha
	addi     r7, r3, msBaseVal__Q32og9newScreen12ObjSMenuBase@l
	mr       r4, r30
	lwz      r3, 0xb0(r29)
	addi     r6, r18, 0x104
	lwz      r5, 0xac(r29)
	lfs      f1, 0(r7)
	bl
registAnimGroupScreen__Q22og6ScreenFPQ32og6Screen9AnimGroupP10JKRArchiveP9J2DScreenPcf
	lis      r4, msBaseVal__Q32og9newScreen12ObjSMenuBase@ha
	lwz      r3, 0xb0(r29)
	lfs      f1, msBaseVal__Q32og9newScreen12ObjSMenuBase@l(r4)
	mr       r4, r30
	lwz      r5, 0xac(r29)
	addi     r6, r18, 0x118
	bl
registAnimGroupScreen__Q22og6ScreenFPQ32og6Screen9AnimGroupP10JKRArchiveP9J2DScreenPcf
	lis      r4, msBaseVal__Q32og9newScreen12ObjSMenuBase@ha
	lwz      r3, 0xb0(r29)
	lfs      f1, msBaseVal__Q32og9newScreen12ObjSMenuBase@l(r4)
	mr       r4, r30
	lwz      r5, 0xac(r29)
	addi     r6, r18, 0x12c
	bl
registAnimGroupScreen__Q22og6ScreenFPQ32og6Screen9AnimGroupP10JKRArchiveP9J2DScreenPcf
	lwz      r3, 0xac(r29)
	mr       r4, r30
	bl       setCallBack__Q32og6Screen10MapCounterFP10JKRArchive
	lis      r5, 0x63656E74@ha
	lis      r4, 0x6D61705F@ha
	lwz      r3, 0xac(r29)
	addi     r6, r5, 0x63656E74@l
	addi     r5, r4, 0x6D61705F@l
	bl       TagSearch__Q22og6ScreenFP9J2DScreenUx
	stw      r3, 0xb4(r29)
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x49(r3)
	cmplwi   r0, 0
	beq      lbl_8031106C
	lbz      r0, 0x4a(r3)
	cmplwi   r0, 0
	beq      lbl_8031106C
	lis      r3, msVal__Q32og9newScreen11ObjSMenuMap@ha
	addi     r3, r3, msVal__Q32og9newScreen11ObjSMenuMap@l
	lfs      f0, 0x30(r3)
	stfs     f0, 0x138(r29)
	b        lbl_8031107C

lbl_8031106C:
	lis      r3, msVal__Q32og9newScreen11ObjSMenuMap@ha
	addi     r3, r3, msVal__Q32og9newScreen11ObjSMenuMap@l
	lfs      f0, 0x2c(r3)
	stfs     f0, 0x138(r29)

lbl_8031107C:
	lfs      f1, 0x138(r29)
	lfs      f0, lbl_8051D740@sda21(r2)
	stfs     f1, 0xe8(r29)
	stfs     f0, 0xec(r29)
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x4a(r3)
	cmplwi   r0, 0
	beq      lbl_803110EC
	lwz      r3, naviMgr__4Game@sda21(r13)
	bl       getActiveNavi__Q24Game7NaviMgrFv
	lwz      r4, cameraMgr__4Game@sda21(r13)
	lhz      r0, 0x2dc(r3)
	addi     r3, r1, 0x18
	lwz      r4, 0x24(r4)
	slwi     r0, r0, 2
	lwzx     r4, r4, r0
	bl       getViewVector__11CullFrustumFv
	lfs      f0, 0x20(r1)
	lis      r3, atanTable___5JMath@ha
	lfs      f1, 0x18(r1)
	addi     r3, r3, atanTable___5JMath@l
	fneg     f2, f0
	bl       "atan2___Q25JMath18TAtanTable<1024,f>CFff"
	lfs      f0, lbl_8051D7CC@sda21(r2)
	lfs      f2, lbl_8051D7C8@sda21(r2)
	fdivs    f0, f1, f0
	fmuls    f0, f2, f0
	stfs     f0, 0xec(r29)

lbl_803110EC:
	mr       r3, r29
	mr       r4, r30
	bl       initMapIcon__Q32og9newScreen11ObjSMenuMapFP10JKRArchive
	lis      r3, msVal__Q32og9newScreen11ObjSMenuMap@ha
	addi     r4, r1, 0x10
	addi     r8, r3, msVal__Q32og9newScreen11ObjSMenuMap@l
	lbz      r3, 0xb(r8)
	lbz      r6, 0xc(r8)
	lbz      r5, 0xd(r8)
	lbz      r0, 0xe(r8)
	stb      r3, 8(r1)
	lbz      r3, 0xf(r8)
	stb      r6, 9(r1)
	lbz      r7, 0x10(r8)
	stb      r5, 0xa(r1)
	lbz      r6, 0x11(r8)
	stb      r0, 0xb(r1)
	lbz      r5, 0x12(r8)
	lwz      r0, 8(r1)
	stb      r3, 0xc(r1)
	stw      r0, 0x10(r1)
	lwz      r3, 0xc8(r29)
	stb      r7, 0xd(r1)
	lwz      r12, 0(r3)
	stb      r6, 0xe(r1)
	lwz      r12, 0x12c(r12)
	stb      r5, 0xf(r1)
	mtctr    r12
	bctrl
	lwz      r0, 0xc(r1)
	addi     r4, r1, 0x14
	stw      r0, 0x14(r1)
	lwz      r3, 0xc8(r29)
	lwz      r12, 0(r3)
	lwz      r12, 0x128(r12)
	mtctr    r12
	bctrl
	lwz      r3, 0xa8(r29)
	li       r19, 0
	mr       r18, r19
	lbz      r0, 0x49(r3)
	cmplwi   r0, 0
	beq      lbl_803111CC
	lbz      r0, 0x4a(r3)
	cmplwi   r0, 0
	beq      lbl_803111CC
	lwz      r3, 0x40(r3)
	bl       caveIDtoMsgID__Q22og9newScreenFUl
	li       r5, 1
	li       r6, 3
	bl       maskTag__Q22og6ScreenFUxUsUs
	mr       r19, r4
	addi     r5, r1, 0x24
	mr       r18, r3
	bl       TagToName__Q22og6ScreenFUxPc
	b        lbl_8031125C

lbl_803111CC:
	lwz      r0, 0x3c(r3)
	cmpwi    r0, 2
	beq      lbl_80311224
	bge      lbl_803111EC
	cmpwi    r0, 0
	beq      lbl_803111FC
	bge      lbl_80311210
	b        lbl_8031125C

lbl_803111EC:
	cmpwi    r0, 4
	beq      lbl_8031124C
	bge      lbl_8031125C
	b        lbl_80311238

lbl_803111FC:
	lis      r4, 0x305F3033@ha
	lis      r3, 0x00383339@ha
	addi     r19, r4, 0x305F3033@l
	addi     r18, r3, 0x00383339@l
	b        lbl_8031125C

lbl_80311210:
	lis      r4, 0x315F3033@ha
	lis      r3, 0x00383339@ha
	addi     r19, r4, 0x315F3033@l
	addi     r18, r3, 0x00383339@l
	b        lbl_8031125C

lbl_80311224:
	lis      r4, 0x325F3033@ha
	lis      r3, 0x00383339@ha
	addi     r19, r4, 0x325F3033@l
	addi     r18, r3, 0x00383339@l
	b        lbl_8031125C

lbl_80311238:
	lis      r4, 0x335F3033@ha
	lis      r3, 0x00383339@ha
	addi     r19, r4, 0x335F3033@l
	addi     r18, r3, 0x00383339@l
	b        lbl_8031125C

lbl_8031124C:
	lis      r4, 0x345F3033@ha
	lis      r3, 0x00383339@ha
	addi     r19, r4, 0x345F3033@l
	addi     r18, r3, 0x00383339@l

lbl_8031125C:
	lwz      r3, 0xac(r29)
	lis      r4, 0x61707469@ha
	addi     r6, r4, 0x61707469@l
	li       r5, 0x546d
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	stw      r19, 0x1c(r3)
	stw      r18, 0x18(r3)
	lwz      r3, 0xbc(r29)
	bl       setCallBackMessage__Q22og6ScreenFPQ29P2DScreen3Mgr
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3031@ha
	addi     r6, r4, 0x706B3031@l
	li       r5, 0x4e
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r18, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3032@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3032@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r19, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3033@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3033@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r20, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3034@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3034@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r21, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3035@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3035@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r22, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3036@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3036@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r23, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3037@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3037@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r24, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3038@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3038@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r25, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3039@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3039@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r26, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3130@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3130@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r27, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x706B3131@ha
	li       r5, 0x4e
	lwz      r12, 0(r3)
	addi     r6, r4, 0x706B3131@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	lwz      r4, 0xa8(r29)
	mr       r28, r3
	lbz      r0, 0x44(r4)
	cmplwi   r0, 0
	bne      lbl_80311440
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r18)
	stb      r0, 0xb0(r23)
	lwz      r3, 0xac(r29)
	bl       dispRed__Q32og6Screen10MapCounterFb

lbl_80311440:
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x45(r3)
	cmplwi   r0, 0
	bne      lbl_80311468
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r19)
	stb      r0, 0xb0(r24)
	lwz      r3, 0xac(r29)
	bl       dispYellow__Q32og6Screen10MapCounterFb

lbl_80311468:
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x46(r3)
	cmplwi   r0, 0
	bne      lbl_80311490
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r20)
	stb      r0, 0xb0(r25)
	lwz      r3, 0xac(r29)
	bl       dispBlue__Q32og6Screen10MapCounterFb

lbl_80311490:
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x47(r3)
	cmplwi   r0, 0
	bne      lbl_803114B8
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r21)
	stb      r0, 0xb0(r26)
	lwz      r3, 0xac(r29)
	bl       dispWhite__Q32og6Screen10MapCounterFb

lbl_803114B8:
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x48(r3)
	cmplwi   r0, 0
	bne      lbl_803114E0
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r22)
	stb      r0, 0xb0(r27)
	lwz      r3, 0xac(r29)
	bl       dispBlack__Q32og6Screen10MapCounterFb

lbl_803114E0:
	lwz      r3, 0xa8(r29)
	lbz      r0, 0x44(r3)
	cmplwi   r0, 0
	bne      lbl_80311534
	lbz      r0, 0x45(r3)
	cmplwi   r0, 0
	bne      lbl_80311534
	lbz      r0, 0x46(r3)
	cmplwi   r0, 0
	bne      lbl_80311534
	lbz      r0, 0x47(r3)
	cmplwi   r0, 0
	bne      lbl_80311534
	lbz      r0, 0x48(r3)
	cmplwi   r0, 0
	bne      lbl_80311534
	li       r0, 0
	li       r4, 0
	stb      r0, 0xb0(r28)
	lwz      r3, 0xac(r29)
	bl       dispFree__Q32og6Screen10MapCounterFb

lbl_80311534:
	lwz      r3, 0xac(r29)
	lis      r5, 0x636B6574@ha
	lis      r4, 0x004E726F@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x636B6574@l
	addi     r5, r4, 0x004E726F@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	lwz      r4, 0xa8(r29)
	lbz      r0, 0x47(r4)
	cmplwi   r0, 0
	bne      lbl_80311574
	lbz      r0, 0x48(r4)
	cmplwi   r0, 0
	beq      lbl_8031161C

lbl_80311574:
	li       r0, 1
	lis      r5, 0x636B5F31@ha
	stb      r0, 0xb0(r3)
	lis      r4, 0x004E726F@ha
	addi     r6, r5, 0x636B5F31@l
	lwz      r3, 0xac(r29)
	addi     r5, r4, 0x004E726F@l
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r19, r3
	lwz      r3, 0xac(r29)
	lis      r5, 0x636B5F32@ha
	lis      r4, 0x004E726F@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x636B5F32@l
	addi     r5, r4, 0x004E726F@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	li       r0, 0
	mr       r18, r3
	stb      r0, 0xb0(r19)
	lis      r4, 0x004F4741@ha
	lis      r6, 0x41555345@ha
	lis      r5, 0x534D5F50@ha
	stb      r0, 0xb0(r3)
	mr       r3, r31
	addi     r4, r4, 0x004F4741@l
	addi     r6, r6, 0x41555345@l
	addi     r5, r5, 0x534D5F50@l
	bl       getSubMember__Q32og6Screen14DispMemberBaseFUlUx
	lwz      r0, 0xc(r3)
	cmplwi   r0, 0x2710
	blt      lbl_80311610
	li       r0, 1
	stb      r0, 0xb0(r18)
	b        lbl_80311660

lbl_80311610:
	li       r0, 1
	stb      r0, 0xb0(r19)
	b        lbl_80311660

lbl_8031161C:
	li       r0, 0
	lis      r5, 0x72657475@ha
	stb      r0, 0xb0(r3)
	lis      r4, 0x4E746169@ha
	addi     r6, r5, 0x72657475@l
	lwz      r3, 0xac(r29)
	addi     r5, r4, 0x4E746169@l
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	lwz      r12, 0(r3)
	lfs      f1, lbl_8051D740@sda21(r2)
	lwz      r12, 0x14(r12)
	lfs      f2, lbl_8051D7D0@sda21(r2)
	mtctr    r12
	bctrl

lbl_80311660:
	lwz      r3, 0xac(r29)
	lis      r5, 0x796E5F31@ha
	lis      r4, 0x004E6F6E@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x796E5F31@l
	addi     r5, r4, 0x004E6F6E@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r20, r3
	lwz      r3, 0xac(r29)
	lis      r5, 0x796E5F32@ha
	lis      r4, 0x004E6F6E@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x796E5F32@l
	addi     r5, r4, 0x004E6F6E@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r19, r3
	lwz      r3, 0xac(r29)
	lis      r5, 0x796E5F33@ha
	lis      r4, 0x004E6F6E@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x796E5F33@l
	addi     r5, r4, 0x004E6F6E@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r18, r3
	lwz      r3, 0xac(r29)
	lis      r5, 0x796E5F34@ha
	lis      r4, 0x004E6F6E@ha
	lwz      r12, 0(r3)
	addi     r6, r5, 0x796E5F34@l
	addi     r5, r4, 0x004E6F6E@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	li       r0, 0
	stb      r0, 0xb0(r20)
	stb      r0, 0xb0(r19)
	stb      r0, 0xb0(r18)
	stb      r0, 0xb0(r3)
	lwz      r4, 0xa8(r29)
	lbz      r5, 0x44(r4)
	cmplwi   r5, 0
	beq      lbl_80311744
	lbz      r0, 0x45(r4)
	cmplwi   r0, 0
	beq      lbl_80311744
	lbz      r0, 0x46(r4)
	cmplwi   r0, 0
	beq      lbl_80311744
	li       r0, 1
	stb      r0, 0xb0(r3)
	b        lbl_80311794

lbl_80311744:
	cmplwi   r5, 0
	beq      lbl_80311764
	lbz      r0, 0x46(r4)
	cmplwi   r0, 0
	beq      lbl_80311764
	li       r0, 1
	stb      r0, 0xb0(r18)
	b        lbl_80311794

lbl_80311764:
	cmplwi   r5, 0
	beq      lbl_80311784
	lbz      r0, 0x45(r4)
	cmplwi   r0, 0
	beq      lbl_80311784
	li       r0, 1
	stb      r0, 0xb0(r19)
	b        lbl_80311794

lbl_80311784:
	cmplwi   r5, 0
	beq      lbl_80311794
	li       r0, 1
	stb      r0, 0xb0(r20)

lbl_80311794:
	lwz      r3, 0xac(r29)
	lis      r4, 0x61695F31@ha
	addi     r6, r4, 0x61695F31@l
	li       r5, 0x4e74
	lwz      r12, 0(r3)
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	mr       r18, r3
	lwz      r3, 0xac(r29)
	lis      r4, 0x61695F32@ha
	li       r5, 0x4e74
	lwz      r12, 0(r3)
	addi     r6, r4, 0x61695F32@l
	lwz      r12, 0x3c(r12)
	mtctr    r12
	bctrl
	li       r0, 0
	stb      r0, 0xb0(r18)
	stb      r0, 0xb0(r3)
	lwz      r4, 0xa8(r29)
	lbz      r0, 0x46(r4)
	cmplwi   r0, 0
	beq      lbl_803117FC
	li       r0, 1
	stb      r0, 0xb0(r18)

lbl_803117FC:
	lwz      r4, 0xa8(r29)
	lbz      r0, 0x45(r4)
	cmplwi   r0, 0
	beq      lbl_80311814
	li       r0, 1
	stb      r0, 0xb0(r3)

lbl_80311814:
	lwz      r5, 0xac(r29)
	mr       r3, r29
	mr       r4, r30
	bl
doCreateAfter__Q32og9newScreen12ObjSMenuBaseFP10JKRArchivePQ29P2DScreen3Mgr
lmw r18, 0x38(r1) lwz      r0, 0x74(r1) mtlr     r0 addi     r1, r1, 0x70
blr
	*/
}

/**
 * @note Address: 0x80311838
 * @note Size: 0x598
 */
void ObjSMenuMap::updateMap()
{
	mController = getOwner()->getGamePad();

	static f32 nv_frame = 0.0f;
	nv_frame += sys->mDeltaTime;

	if (nv_frame > 1.0f) {
		nv_frame = 0.0f;
	}

	f32 angle           = TAU * nv_frame;
	u8 olimarArrowAlpha = ((sinf(angle) + 1.0f) / 2 * 0.6f + 0.4f) * 255.0f;
	u8 louieArrowAlpha  = ((sinf(angle + PI) + 1.0f) / 2 * 0.6f + 0.4f) * 255.0f;

	calcCaveNameAlpha();

	for (int i = 0; i < mCaveLabelCount; i++) {
		mCaveLabelTextBoxes[i]->setAlpha(mZoomCaveTextAlpha);
	}

	f32 scale = mCurrentZoom;
	if (mDisp->mInCave)
		scale *= 2.0f;
	f32 mapX             = mMapPosition.x;
	f32 mapY             = mMapPosition.y;
	mMapRotationOrigin.x = -mapX;
	mMapRotationOrigin.y = -mapY;
	mMapTexPane->setBasePosition(J2DPOS_Center);
	mMapTexPane->updateScale(scale);
	mMapTexPane->rotate(mMapRotationOrigin.x, mMapRotationOrigin.y, J2DROTATE_Z, mMapAngle);
	mMapTexPane->move(mapX + msVal.mMapTexOffset.x, mapY + msVal.mMapTexOffset.y);

	f32 angleOffs = 360.0f;
	for (int i = 0; i < mMapIconNum; i++) {
		f32 scaleFactor = msVal.mMapIconScaleBase;
		(*mRadarPaneList[i])->updateScale(scaleFactor / scale);
		(*mRadarPaneList[i])->setAngle(angleOffs - mMapAngle);
	}

	if (mOlimarArrow && mOlimarObj) {
		// f32 factor = (msVal.mMapNaviArrowScaleMod / scale);
		f32 scaleFactor = msVal.mMapNaviArrowScaleMod * (msVal.mMapIconScaleBase / scale);
		f32 facedir     = mOlimarObj->getFaceDir();
		mOlimarArrow->setBasePosition(J2DPOS_Center);
		mOlimarArrow->updateScale(scaleFactor);
		facedir = (facedir * 360.0f) / TAU + 45.0f;
		mOlimarArrow->setAngle(facedir);
		mOlimarGlow->setBasePosition(J2DPOS_Center);
		mOlimarGlow->updateScale(scaleFactor);
		mOlimarGlow->setAngle(facedir);

		mOlimarArrow->setAlpha(olimarArrowAlpha);
		mOlimarGlow->setAlpha(olimarArrowAlpha);
	}

	if (mLouieArrow && mLouieObj) {
		f32 scaleFactor = msVal.mMapNaviArrowScaleMod * (msVal.mMapIconScaleBase / scale);
		f32 facedir     = mLouieObj->getFaceDir();
		mLouieArrow->setBasePosition(J2DPOS_Center);
		mLouieArrow->updateScale(scaleFactor);
		facedir = (facedir * 360.0f) / TAU + 45.0f;
		mLouieArrow->setAngle(facedir);
		mLouieGlow->setBasePosition(J2DPOS_Center);
		mLouieGlow->updateScale(scaleFactor);
		mLouieGlow->setAngle(facedir);

		mLouieArrow->setAlpha(louieArrowAlpha);
		mLouieGlow->setAlpha(louieArrowAlpha);
	}
}

/**
 * @note Address: 0x80311DD0
 * @note Size: 0xA8
 */
void ObjSMenuMap::commonUpdate()
{
	commonUpdateBase();
	setSMenuScale(msVal.mMapScreenScale.x, msVal.mMapScreenScale.y);
	mAnimGroup->update();
	updateMap();

	mMapCounter->setXY(mMovePos, 0.0f);

	mMapCounter->animation();
	mMapCounter->update();
	mIconScreen->update();
}

/**
 * @note Address: 0x80311E78
 * @note Size: 0x6C
 */
void ObjSMenuMap::doUpdateLAction()
{
	::Screen::SetSceneArg arg(SCENE_PAUSE_MENU_ITEMS, getDispMember());
	jump_L(arg);
}

/**
 * @note Address: 0x80311EE4
 * @note Size: 0xC4
 */
void ObjSMenuMap::doUpdateRAction()
{
	if (mDisp->mInCave) {
		::Screen::SetSceneArg arg(SCENE_PAUSE_MENU_DOUKUTU, getDispMember());
		jump_R(arg);
	} else {
		::Screen::SetSceneArg arg(SCENE_PAUSE_MENU, getDispMember());
		jump_R(arg);
	}
}

/**
 * @note Address: 0x80311FA8
 * @note Size: 0x10C
 */
bool ObjSMenuMap::doUpdate()
{
	::Screen::SceneBase* scene = getOwner();
	mController                = scene->getGamePad();
	transMap();

	bool isUp   = false;
	bool isDown = false;
	f32 cstick  = mController->getSubStickY();
	if (cstick > 0.4f) {
		isUp = true;
	}
	if (cstick < -0.4f) {
		isDown = true;
	}

	if (isUp) {
		mCurrentZoom += mCurrentZoom * 0.03f;
		if (mCurrentZoom > msVal.mMaxZoom)
			mCurrentZoom = msVal.mMaxZoom;
		ogSound->setZoomIn();
	} else if (isDown) {
		mCurrentZoom -= mCurrentZoom * 0.03f;
		if (mCurrentZoom < msVal.mMinZoom)
			mCurrentZoom = msVal.mMinZoom;
		ogSound->setZoomOut();
	}
	commonUpdate();
	return ObjSMenuBase::doUpdate();
}

/**
 * @note Address: 0x803120B4
 * @note Size: 0x138
 */
void ObjSMenuMap::doDraw(Graphics& gfx)
{
	J2DPerspGraph* graf = &gfx.mPerspGraph;
	drawMap(gfx);

	Graphics gfx2;
	mIconScreen->draw(gfx2, *graf);

	if (mCompassPic && mPane_Ncompas) {
		PSMTXCopy(mPane_Ncompas->mGlobalMtx, mCompassPic->mPositionMtx);
	}

	graf->setPort();
	mIconScreen2->draw(gfx, *graf);
	graf->setPort();
	drawYaji(gfx);
}

/**
 * @note Address: 0x803121EC
 * @note Size: 0x32C
 */
void ObjSMenuMap::drawMap(Graphics& gfx)
{
	J2DPerspGraph* graf = &gfx.mPerspGraph;
	if (mUpdateCaveTex) {
		mRadarMapTexture = Game::Cave::randMapMgr->getRadarMapTexture();
		mMapTexPane->changeTexture(mRadarMapTexture->mTexInfo, 0);
		mUpdateCaveTex          = false;
		mMapTextureDimensions.x = (int)mMapTexPane->getTexture(0)->mTexInfo->mSizeX;
		mMapTextureDimensions.y = (int)mMapTexPane->getTexture(0)->mTexInfo->mSizeY;
		mMapTexScale.x          = mMapBounds.x / mMapTextureDimensions.x;
		mMapTexScale.y          = mMapBounds.y / mMapTextureDimensions.y;
	}

	if (mPane_Ncompas) {
		mPane_Ncompas->setAngle(mMapAngle);
	}

	P2DScreen::Mgr_tuning* scrn = mMapCounter;
	if (scrn) {
		scrn->draw(gfx, *graf);
	}
	graf->setPort();

	Graphics* sysGfx = sys->mGfx;
	j3dSys.drawInit();
	sysGfx->initPrimDraw(nullptr);

	GXSetColorUpdate(GX_FALSE);
	GXSetAlphaUpdate(GX_FALSE);
	GXSetColorUpdate(GX_FALSE);

	Rectf rect;
	rect.p1.x = 0.0f;
	rect.p1.y = 0.0f;
	rect.p2.x = 640.0f;
	rect.p2.y = 480.0f;
	Color4 color(200, 10, 200, 155);
	drawRectZ(*sysGfx, rect, color, 0.999);
	JGeometry::TVec3f vec1 = mPane_map->getGlbVtx(0);
	JGeometry::TVec3f vec2 = mPane_map->getGlbVtx(1);
	JGeometry::TVec3f vec3 = mPane_map->getGlbVtx(2);
	JGeometry::TVec3f vec4 = mPane_map->getGlbVtx(3);
	Color4 color2(100, 0, 0, 155);
	drawVecZ(*sysGfx, *(Vec*)&vec1, *(Vec*)&vec2, *(Vec*)&vec3, *(Vec*)&vec4, color2, -0.999);
	GXSetColorUpdate(GX_TRUE);
	PSMTXCopy(mPane_map->mGlobalMtx, mRootPane->mPositionMtx);
	graf->setPort();
	GXSetZCompLoc(GX_TRUE);
	GXSetZMode(GX_TRUE, GX_LESS, GX_FALSE);
}

/**
 * @note Address: 0x80312518
 * @note Size: 0x1E0
 */
void ObjSMenuMap::drawRectZ(Graphics& gfx, Rectf& rect, Color4& color, f32 z)
{
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	Mtx44 mtx;
	Mtx mtx2;
	C_MTXOrtho(mtx, 0.0f, 480.0f, 0.0f, 640.0f, -1.0f, 1.0f);
	GXSetProjection(mtx, GX_ORTHOGRAPHIC);
	PSMTXIdentity(mtx2);
	GXLoadPosMtxImm(mtx2, 0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_POS_XYZ, GX_RGBA8, 0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

	GXPosition3f32(rect.p1.x, rect.p1.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(rect.p1.x, rect.p2.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(rect.p2.x, rect.p2.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(rect.p2.x, rect.p1.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXSetZMode(GX_TRUE, GX_LESS, GX_TRUE);
}

/**
 * @note Address: 0x803126F8
 * @note Size: 0x214
 */
void ObjSMenuMap::drawVecZ(Graphics& gfx, Vec& vec1, Vec& vec2, Vec& vec3, Vec& vec4, Color4& color, f32 z)
{
	u16 wid = System::getRenderModeWidth();
	u16 hei = System::getRenderModeHeight();
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	Mtx44 temp;
	Mtx temp2;
	C_MTXOrtho(temp, 0.0f, (f32)hei, 0.0f, (f32)wid, -1.0f, 1.0f);
	GXSetProjection(temp, GX_ORTHOGRAPHIC);
	PSMTXIdentity(temp2);
	GXLoadPosMtxImm(temp2, 0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_POS_XYZ, GX_RGBA8, 0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

	GXPosition3f32(vec1.x, vec1.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(vec2.x, vec2.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(vec4.x, vec4.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXPosition3f32(vec3.x, vec3.y, z);
	GXColor4u8(color.r, color.g, color.b, color.a);

	GXSetZMode(GX_TRUE, GX_LESS, GX_TRUE);
}

/**
 * @note Address: 0x8031290C
 * @note Size: 0xA4
 */
bool ObjSMenuMap::doStart(::Screen::StartSceneArg const* arg)
{
	mAnimGroup->setFrame(0.0f);
	mAnimGroup->setRepeat(true);
	mAnimGroup->setSpeed(1.0f);
	mAnimGroup->start();
	setYajiName('6051_00', '6052_00', '6050_00'); // "Items" "Menu" "Radar"
	stopYaji();
	start_LR(arg);
}

/**
 * @note Address: 0x803129B0
 * @note Size: 0x8
 */
bool ObjSMenuMap::doEnd(::Screen::EndSceneArg const*) { return true; }

/**
 * @note Address: 0x803129B8
 * @note Size: 0x20
 */
void ObjSMenuMap::doUpdateFinish() { ObjSMenuBase::doUpdateFinish(); }

/**
 * @note Address: 0x803129D8
 * @note Size: 0x4C
 */
bool ObjSMenuMap::doUpdateFadeout()
{
	commonUpdate();
	updateFadeOut();
}

/**
 * @note Address: 0x80312A24
 * @note Size: 0x14
 */
void ObjSMenuMap::in_L()
{
	mState = MENUSTATE_OpenL;
	mAngle = 15.0f;
}

/**
 * @note Address: 0x80312A38
 * @note Size: 0x14
 */
void ObjSMenuMap::in_R()
{
	mState = MENUSTATE_OpenR;
	mAngle = 15.0f;
}

/**
 * @note Address: 0x80312A4C
 * @note Size: 0xC
 */
void ObjSMenuMap::wait() { mState = MENUSTATE_Default; }

/**
 * @note Address: 0x80312A58
 * @note Size: 0x2C
 */
void ObjSMenuMap::out_L()
{
	mState = MENUSTATE_CloseL;
	ogSound->setSMenuLR();
}

/**
 * @note Address: 0x80312A84
 * @note Size: 0x2C
 */
void ObjSMenuMap::out_R()
{
	mState = MENUSTATE_CloseR;
	ogSound->setSMenuLR();
}

ObjSMenuMap::StaticValues ObjSMenuMap::msVal;

} // namespace newScreen

} // namespace og
