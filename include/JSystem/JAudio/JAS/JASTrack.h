#ifndef _JSYSTEM_JAS_JASTRACK_H
#define _JSYSTEM_JAS_JASTRACK_H

#include "JSystem/JAudio/JAS/JASBank.h"
#include "JSystem/JAudio/JAS/JASOscillator.h"
#include "types.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JAudio/JAS/JASSeqCtrl.h"
#include "JSystem/JAudio/JAS/JASChannel.h"
#include "JSystem/JAudio/JAS/JASRegisterParam.h"

struct JASTrack;

enum JASOuterParamFlag {
	OUTERPARAM_Volume = 0x1,
	OUTERPARAM_Pitch  = 0x2,
	OUTERPARAM_Fxmix  = 0x4,
	OUTERPARAM_Pan    = 0x8,
	OUTERPARAM_Dolby  = 0x10,
	OUTERPARAM_Tempo  = 0x40,
};

/**
 * @size = 0xC
 */
struct JASVibrate {
	JASVibrate();

	void init();
	void incCounter();
	f32 getValue() const;

	f32 _00;    // _00
	f32 mDepth; // _04 - depth?
	f32 mPitch; // _08 - pitch?
};

/**
 * @size = 0x2C
 */
struct JASIntrMgr {
	void init();
	void request(u32);
	void setIntr(u32, void*);
	void resetInter(u32);
	void* checkIntr();
	void timerProcess();

	u8 _00;       // _00
	u8 _01;       // _01
	u8 _02;       // _02
	u8 _03;       // _03
	u32 _04;      // _04
	u32 _08;      // _08
	void* _0C[8]; // _0C
};

/**
 * @size = 0x40
 */
struct JASTrackPort {
	void init();
	u16 readImport(int);
	u16 readExport(int);
	void writeImport(int, u16);
	void writeExport(int, u16);

	u8 _00[0x10];  // _00
	u8 _10[0x10];  // _10
	u16 _20[0x10]; // _20
};

/**
 * @size = 0x2C
 */
struct JASOuterParam {

	JASOuterParam();

	void initExtBuffer();
	void setParam(u8, f32);
	void setOuterSwitch(u16 newValue);
	bool checkOuterSwitch(u16);
	void setOuterUpdate(u16 newValue);
	u16 getOuterUpdate();
	s16 getIntFirFilter(u8 index);
	void setFirFilter(s16*);
	void onSwitch(u16);
	// weak, but forced to exist via recursion in JASTrack::updateTempo
	f32 getTempo() const { return mTempo; }

	// unused/inlined:
	u16 getSwitch(); // maybe getOuterSwitch?
	void setIntFirFilter(s16 newValue, u8 index);

	u16 mOuterSwitch;  // _00 - outerSwitch?
	u16 mOuterUpdate;  // _02 - outerUpdate?
	f32 mVolume;       // _04
	f32 mPitch;        // _08
	f32 mFxmix;        // _0C
	f32 mDolby;        // _10
	f32 mPan;          // _14
	f32 mTempo;        // _18
	s16 mFirFilter[8]; // _1C - firFilter?
};

/**
 * @size = 0x358
 */
struct JASTrack : JSUList<JASChannel> {
	typedef JASChannel* (*NoteOnCallback)(JASTrack*, u8, u8, u8, u8, u16);
	typedef u16 (*SeqCallback)(JASTrack*, u16);

	struct MoveParam_ {
		MoveParam_()
		    : _00(0.0f)
		    , mGoalValue(0.0f)
		    , _08(0.0f)
		    , _0C(0.0f)
		{
		}

		f32 _00;        // _00
		f32 mGoalValue; // _04
		f32 _08;        // _08
		f32 _0C;        // _0C
	};

	struct AInnerParam_ {
		AInnerParam_()
		    : _00()
		    , _10()
		    , _20()
		    , _30()
		    , _40()
		    , _50()
		    , _60()
		    , _70()
		    , _80()
		    , _90()
		    , _A0()
		    , _B0()
		    , _C0()
		    , _100()
		    , _110()
		{
		}

		MoveParam_ _00;
		MoveParam_ _10;
		MoveParam_ _20;
		MoveParam_ _30;
		MoveParam_ _40;
		MoveParam_ _50;
		MoveParam_ _60;
		MoveParam_ _70;
		MoveParam_ _80;
		MoveParam_ _90;
		MoveParam_ _A0;
		MoveParam_ _B0;
		MoveParam_ _C0[4];
		MoveParam_ _100;
		MoveParam_ _110;
	};

	union TimedParam_ {
		TimedParam_() { }

		AInnerParam_ mInnerParam;
		MoveParam_ mMoveParams[0x12];
	};

	enum ParamType {
		JASParam_Volume = 0,
		JASParam_Pitch  = 1,
		JASParam_Null   = 2,
		JASParam_Pan    = 3,
	};

	JASTrack();

	void init();
	void initTimed();
	s8 mainProc();
	void setInterrupt(u16);
	bool tryInterrupt();
	void assignExtBuffer(JASOuterParam*);
	void connectBus(int, int);
	int noteOn(u8, s32, s32, s32, u32);
	void overwriteOsc(JASChannel*);
	bool noteOff(u8, u16);
	int gateOn(u8, s32, s32, s32);
	BOOL checkNoteStop(s32);
	void oscSetupFull(u8, u32, u32);
	void oscSetupSimpleEnv(u8, u32);
	void oscSetupSimple(u8);
	void updateTimedParam();
	void updateTrackAll();
	void updateTrack(u32);
	void updateTempo();
	void updateSeq(u32, bool);
	s32 seqTimeToDspTime(s32, u8);
	void setParam(int type, f32 value, int);
	bool setSeqData(u8*, s32);
	bool startSeq();
	bool stopSeq();
	void stopSeqMain();
	void close();
	bool start(void*, u32);
	JASTrack* openChild(u8, u8);
	u32 exchangeRegisterValue(u8);
	u32 readReg32(u8);
	u32 readReg16(u8);
	void writeRegDirect(u8, u16);
	void writeRegParam(u8);
	u16 readSelfPort(int portNumber);
	void writeSelfPort(int portNumber, u16 value);
	bool writePortAppDirect(u32, u16);
	bool readPortAppDirect(u32, u16*);
	bool writePortApp(u32, u16);
	void readPortApp(u32, u16*);
	void pause(bool, bool);
	int getTranspose() const;
	void setTempo(u16);
	void setTimebase(u16);
	f32 panCalc(f32, f32, f32, u8);
	void setNoteMask(u8);
	void muteTrack(bool);

	static s32 rootCallback(void*);
	static void channelUpdateCallback(u32, JASChannel*, JASDsp::TChannel*, void*);
	static void newMemPool(int);
	static void registerSeqCallback(SeqCallback);

	// unused/inlined:
	~JASTrack();
	int inherit();
	void setBankNumber(u8);
	void setPanSwitchExt(u8, int);
	void setPanSwitchParent(u8, int);
	void setPanSwitchJcs(u8, int);
	void getBank() const;
	void getProgramNumber() const;
	void getVolume() const;
	void getPitch() const;
	void getPan() const;
	void getFxmix() const;
	void getDolby() const;
	void updateOscParam(int, f32);
	void muteChildTracks(u16);
	void loadTbl(u32, u32, u32);
	JASTrack* routeTrack(u32 count);
	void routeTrack(u32) const;
	void checkExportApp(u32) const;
	void checkImportApp(u32) const;
	int getFreeMemCount();
	int getChannelCount() const;
	int getReleaseChannelCount() const;

	inline JASSeqCtrl* getCtrl() { return &mSeqCtrl; }
	inline JASOuterParam* getExtBuffer() const { return mExtBuffer; }

	// JSUPtrList _00;        // _00
	JASSeqCtrl mSeqCtrl;               // _0C
	JASTrackPort mTrackPort;           // _54
	JASIntrMgr mIntrMgr;               // _94
	JASChannel* mChannels[8];          // _C0
	u32 _E0;                           // _E0
	u8 _E4;                            // _E4
	u8 _E5;                            // _E5
	u8 _E6;                            // _E6
	u8 _E7;                            // _E7 - might be padding
	JASVibrate mVibrate;               // _E8
	JASChannelUpdater mChannelUpdater; // _F4
	NoteOnCallback _144;               // _144
	TimedParam_ mTimedParam;           // _148
	JASRegisterParam mRegisterParam;   // _268
	u8 _298[0x10];                     // _298 - unknown
	JASOscillator::Data mOscData[2];   // _2A8
	u32 _2D8[2];                       // _2D8
	s16 _2E0[12];                      // _2E0
	JASTrack* mParentTrack;            // _2F8
	JASTrack* mChildList[16];          // _2FC
	JASOuterParam* mExtBuffer;         // _33C
	f32 _340;                          // _340
	f32 mCurrentTempo;                 // _344, actual calculated tempo for playback
	u32 _348;                          // _348
	u32 _34C;                          // _34C - unknown
	u16 _350;                          // _350
	u16 mTempo;                        // _352, direct value read from bms
	u16 mTimeBase;                     // _354
	s8 mTranspose;                     // _356
	u8 _357;                           // _357
	u8 mPauseStatus;                   // _358
	u8 mVolumeMode;                    // _359
	u8 mNoteMask;                      // _35A
	u8 _35B;                           // _35B
	u8 _35C;                           // _35C
	u8 _35D;                           // _35D
	u8 _35E;                           // _35E
	u8 _35F;                           // _35F
	u8 _360;                           // _360
	u8 _361;                           // _361
	bool mIsPaused;                    // _362
	bool mIsMuted;                     // _363
	u8 mTimeRelate;                    // _364
	u8 _365;                           // _365
	u8 _366;                           // _366

	static struct JASSeqParser* sParser;

	// these might be JSUList<JASChannel>, for whatever difference that may or may not make
	static JASTrack* sFreeList;
	static JASTrack* sFreeListEnd;

	static SeqCallback sCallBackFunc;
};

#endif
