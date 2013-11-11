// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef MDASOUNDADAPTERBODY_H
#define MDASOUNDADAPTERBODY_H

#include "mdasoundadapter.h"
#include <d32soundsc.h>
#include <e32base.h>
#include <e32std.h>

/** 
Panic category and codes for the mdasoundadapter
*/
_LIT(KSoundAdapterPanicCategory, "mdasoundadapter");
enum TSoundAdapterPanicCodes
	{
	EDeviceNotOpened,
	EPanicPartialBufferConverterNotSupported,
	EBadState,
	ENoClientPlayRequest,
	EFifoEmpty,
	EFifoFull
	};
	
//Structure used to map samples per second to the corresponding enums in RSoundSc
struct TSampleRateEnumTable
  	{
	TInt iRate;
	TSoundRate iRateEnum;
	TUint iRateConstant;
	};

//Table that maps given samples per second to the corresponding enums in RSoundSc
const TSampleRateEnumTable KRateEnumLookup[] =
								 {
									{48000,ESoundRate48000Hz,KSoundRate48000Hz},
			                   		{44100,ESoundRate44100Hz,KSoundRate44100Hz},
				                  	{32000,ESoundRate32000Hz,KSoundRate32000Hz},
									{24000,ESoundRate24000Hz,KSoundRate24000Hz},
				                  	{22050,ESoundRate22050Hz,KSoundRate22050Hz},
				                  	{16000,ESoundRate16000Hz,KSoundRate16000Hz},
				                  	{12000,ESoundRate12000Hz,KSoundRate12000Hz},
				                  	{11025,ESoundRate11025Hz,KSoundRate11025Hz},
				                  	{8000, ESoundRate8000Hz, KSoundRate8000Hz}
                   				 };
//Structure used to map linear value of the volume to the decibel value.
struct TLinearToDbTable
	{
	TInt iLiniearValue;
	TInt iDBValue;
	};


//Table that maps given linear value of volume to the corresponding decibel value.
const TLinearToDbTable KLinerToDbConstantLookup[] =
						{
							{0,0},
							{1,158},
							{2,170},
							{3,177},
							{4,182},
							{5,186},
							{6,189},
							{7,192},
							{8,194},
							{9,196},
							{10,198},
							{11,200},
							{12,201},
							{13,203},
							{14,204},
							{15,205},
							{16,206},
							{17,207},
							{18,208},
							{19,209},
							{20,210},
							{21,211},
							{22,212},
							{23,213},
							{24,213},
							{25,214},
							{26,215},
							{27,215},
							{28,216},
							{29,217},
							{30,217},
							{31,218},
							{32,218},
							{33,219},
							{34,219},
							{35,220},
							{36,220},
							{37,221},
							{38,221},
							{39,222},
							{40,222},
							{41,223},
							{42,223},
							{43,224},
							{44,224},
							{45,224},
							{46,225},
							{47,225},
							{48,225},
							{49,226},
							{50,226},
							{51,226},
							{52,227},
							{53,227},
							{54,227},
							{55,228},
							{56,228},
							{57,228},
							{58,229},
							{59,229},
							{60,229},
							{61,230},
							{62,230},
							{63,230},
							{64,230},
							{65,231},
							{66,231},
							{67,231},
							{68,231},
							{69,232},
							{70,232},
							{71,232},
							{72,232},
							{73,233},
							{74,233},
							{75,233},
							{76,233},
							{77,234},
							{78,234},
							{79,234},
							{80,234},
							{81,235},
							{82,235},
							{83,235},
							{84,235},
							{85,235},
							{86,236},
							{87,236},
							{88,236},
							{89,236},
							{90,236},
							{91,237},
							{92,237},
							{93,237},
							{94,237},
							{95,237},
							{96,237},
							{97,238},
							{98,238},
							{99,238},
							{100,238},
							{101,238},
							{102,239},
							{103,239},
							{104,239},
							{105,239},
							{106,239},
							{107,239},
							{108,240},
							{109,240},
							{110,240},
							{111,240},
							{112,240},
							{113,240},
							{114,240},
							{115,241},
							{116,241},
							{117,241},
							{118,241},
							{119,241},
							{120,241},
							{121,241},
							{122,242},
							{123,242},
							{124,242},
							{125,242},
							{126,242},
							{127,242},
							{128,242},
							{129,243},
							{130,243},
							{131,243},
							{132,243},
							{133,243},
							{134,243},
							{135,243},
							{136,244},
							{137,244},
							{138,244},
							{139,244},
							{140,244},
							{141,244},
							{142,244},
							{143,244},
							{144,245},
							{145,245},
							{146,245},
							{147,245},
							{148,245},
							{149,245},
							{150,245},
							{151,245},
							{152,245},
							{153,246},
							{154,246},
							{155,246},
							{156,246},
							{157,246},
							{158,246},
							{159,246},
							{160,246},
							{161,246},
							{162,247},
							{163,247},
							{164,247},
							{165,247},
							{166,247},
							{167,247},
							{168,247},
							{169,247},
							{170,247},
							{171,247},
							{172,248},
							{173,248},
							{174,248},
							{175,248},
							{176,248},
							{177,248},
							{178,248},
							{179,248},
							{180,248},
							{181,248},
							{182,249},
							{183,249},
							{184,249},
							{185,249},
							{186,249},
							{187,249},
							{188,249},
							{189,249},
							{190,249},
							{191,249},
							{192,250},
							{193,250},
							{194,250},
							{195,250},
							{196,250},
							{197,250},
							{198,250},
							{199,250},
							{200,250},
							{201,250},
							{202,250},
							{203,250},
							{204,251},
							{205,251},
							{206,251},
							{207,251},
							{208,251},
							{209,251},
							{210,251},
							{211,251},
							{212,251},
							{213,251},
							{214,251},
							{215,251},
							{216,252},
							{217,252},
							{218,252},
							{219,252},
							{220,252},
							{221,252},
							{222,252},
							{223,252},
							{224,252},
							{225,252},
							{226,252},
							{227,252},
							{228,252},
							{229,253},
							{230,253},
							{231,253},
							{232,253},
							{233,253},
							{234,253},
							{235,253},
							{236,253},
							{237,253},
							{238,253},
							{239,253},
							{240,253},
							{241,253},
							{242,254},
							{243,254},
							{244,254},
							{245,254},
							{246,254},
							{247,254},
							{248,254},
							{249,254},
							{250,254},
							{251,254},
							{252,254},
							{253,254},
							{254,254},
							{255,254}
						};

// Total Number of sample rates
const TUint KNumSampleRates = 9;
// Number of shared chunk buffers used for playing
// Each buffer is permanently mapped, via an index number, to a particular buffer in the chunk
// The esoundsc.ldd can only handle a max of 8 pending play requests, therefore no point in having
// more than 8 play buffers...
const TUint KPlaySharedChunkBuffers = 8;
// Size of RSoundSc play buffers
const TUint KPlaySharedChunkBufferSize = 4096;

//Number of shared chunk buffers used for recording
const TUint KRecordMaxSharedChunkBuffers = 8;
// Size of RSoundSc record buffers
const TUint KRecordSharedChunkBufferSize = 4096;

//Shared chunk driver does not support max. buffer size. 16K is given in order to simulate the old driver behavior.
const TUint KMaxBufferSize = 0x4000;

class TPlaySharedChunkBufConfig : public TSharedChunkBufConfigBase
	{
public:
	TInt iBufferOffsetList[KPlaySharedChunkBuffers];
	};

class TRecordSharedChunkBufConfig : public TSharedChunkBufConfigBase
	{
public:
	TInt iBufferOffsetList[KRecordMaxSharedChunkBuffers];
	};
	
class CChannelAndSampleRateConverter; // forward dec

GLDEF_C void Panic(TSoundAdapterPanicCodes aPanicCode);//forward declaration

// RFifo class which manages a fifo of up to COUNT items of type T
template<typename T, TUint32 COUNT> class RFifo
	{
public:
	RFifo()
		: iWriteIndex(0), iReadIndex(0)
		{}
	TBool IsEmpty() const
		{
		return iWriteIndex == iReadIndex;
		}
	TBool IsFull() const
		{
		// Full if writing one more item would make iWriteIndex equal to iReadIndex
		TUint32 next = NextIndex(iWriteIndex);
		return next == iReadIndex;
		}
	/// Push item into FIFO. Does not take ownership. Will PANIC with EFifoFull if full.
	void Push(const T &aItem)
		{
		if(IsFull())
			{
			Panic(EFifoFull);
			}
		iFifo[iWriteIndex] = aItem;
		iWriteIndex = NextIndex(iWriteIndex);
		}
    /// Pop item from FIFO. Will PANIC with EFifoEmpty if empty 
	T Pop()
		{
		if(IsEmpty())
			{
			Panic(EFifoEmpty);
			}
		TUint32 tmp = iReadIndex;
		iReadIndex = NextIndex(iReadIndex);
		return iFifo[tmp];
		}

    /// Peek first item from FIFO. Will PANIC with EFifoEmpty if empty 
	T Peek()
		{
		if(IsEmpty())
			{
			Panic(EFifoEmpty);
			}
		return iFifo[iReadIndex];
		}
	TUint Length() const
		{
		TUint len;
		if(iWriteIndex >= iReadIndex)
			{
			len = iWriteIndex - iReadIndex;
			}
		else
			{
			len =  COUNT+1 - (iReadIndex - iWriteIndex);
			}
		return len;
		}
private:
	TUint32 NextIndex(TUint32 aIndex) const
		{
		++aIndex;
		aIndex %= (COUNT+1);
		return aIndex;
		}
	T iFifo[COUNT+1];
	TUint32 iWriteIndex;
	TUint32 iReadIndex;
	};



//Body class for the adapter
NONSHARABLE_CLASS( RMdaDevSound::CBody ): public CBase
	{
public:
	//This class handles the play/record completions from the new sound driver
	NONSHARABLE_CLASS( CPlayer ) : public CActive
		{
	public:
		explicit CPlayer(TInt aPriority, RMdaDevSound::CBody& aParent, TInt aIndex);
		~CPlayer();
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();
		void PlayData(TUint aChunkOffset, TInt aLength);

		TUint GetPlayerIndex() const;

	private:		
		RMdaDevSound::CBody& iParent;
		const TUint iIndex; // index of this object in parent
		
		TInt iBufferOffset;
		TInt iBufferLength;
		};

	
	NONSHARABLE_CLASS( CRecorder ) : public CActive
		{
	public:
		explicit CRecorder(TInt aPriority, RMdaDevSound::CBody& aParent);
		~CRecorder();
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();
		void RecordData(TInt& aLength);

	private:		
		RMdaDevSound::CBody& iParent;

		TInt iBufferOffset;
		TInt iBufferLength;
		};
	
	enum TStateEnum
		{
		ENotReady,
		EStopped,
		ERecording,
		ERecordingPausedInHw,
		ERecordingPausedInSw,
		EPlaying,
		EPlayingPausedInHw, // ie. Play request pending on h/w and paused
		EPlayingPausedInSw, // ie. Driver not playing or paused
		EPlayingUnderrun
		};

	NONSHARABLE_CLASS( TState )
		{
		public:
			TState(TStateEnum aState) : iState(aState) {}
			const TText8 *Name() const;
			TState &operator=(TStateEnum aNewState);
			operator TStateEnum() const { return iState; }
		private:
			TStateEnum iState;
		};
		
	class TFormatData
		{
	public:
		inline TFormatData():
			iSampleRate(8000), iRequestedChannels(1) // default
			{
			}
	public:
		CChannelAndSampleRateConverter* iConverter;
		TInt iSampleRate;
		TInt iActualRate;
		TInt iRequestedChannels;
		TInt iActualChannels;			
		};
		
public:
	~CBody();
	static CBody* NewL();
	TInt Open(TInt aUnit=KNullUnit);
	TVersion VersionRequired() const;
	TInt IsMdaSound();
	void PlayFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported);
	void GetPlayFormat(TCurrentSoundFormatBuf& aFormat);
	TInt SetPlayFormat(const TCurrentSoundFormatBuf& aFormat);
	TInt PlayVolume();
	void SetPlayVolume(TInt aVolume);
	void SetVolume(TInt aLogarithmicVolume);
	void CancelPlayData();
	void RecordFormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported);
	void GetRecordFormat(TCurrentSoundFormatBuf& aFormat);
	TInt SetRecordFormat(const TCurrentSoundFormatBuf& aFormat);
	TInt RecordLevel();
	void SetRecordLevel(TInt aLevel);
	void CancelRecordData();
	void FlushRecordBuffer();
	TInt BytesPlayed();
	void ResetBytesPlayed();
	void PausePlayBuffer();
	void ResumePlaying();
	void PauseRecordBuffer();
	void ResumeRecording();
	TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTimePlayed);
	void Close();
	TInt Handle();
	void PlayData(TRequestStatus& aStatus,const TDesC8& aData);
	void RecordData(TRequestStatus& aStatus,TDes8& aData);
	void NotifyRecordError(TRequestStatus& aStatus);
	void NotifyPlayError(TRequestStatus& aStatus);
	void CancelNotifyPlayError();
	void CancelNotifyRecordError();
	void FlushPlayBuffer();
	//internal methods added to reduce the code
	void FormatsSupported(TSoundFormatsSupportedBuf& aFormatsSupported, RSoundSc& aDevice);
	void GetFormat(TCurrentSoundFormatBuf& aFormat, RSoundSc& aDevice, const TFormatData &aFormatData);
	TInt SetFormat(const TCurrentSoundFormatBuf& aFormat, RSoundSc& aDevice, TFormatData &aFormatData);
	
	//for players
	void SoundDeviceError(TInt aError);
	RSoundSc& PlaySoundDevice();
	RSoundSc& RecordSoundDevice();
	const TState &State() const;
	void BufferFilled(TInt aError);

	// Called whenever a player becomes inactive.
	// This includes driver request ok, driver request failed, CPlayer:::RunError invoked.
	void PlayRequestHasCompleted(CPlayer *aPlayer, TInt aStatus, TBool aDueToCancelCommand);

private:
	CBody();
	void ConstructL();
	
	TInt NegotiateFormat(const TCurrentSoundFormatBuf& aFormat, RSoundSc& aDevice, TFormatData &aFormatData);

	void StartPlayersAndUpdateState();
	void StartRecordRequest();

	const char *StateName() const;

	TBool InRecordMode() const;
	TBool InPlayMode() const;

	TUint32 CurrentTimeInMsec() const;
	TUint64 BytesPlayed64();

private:
	RSoundSc iPlaySoundDevice;
	RChunk iPlayChunk;//handle to the shared chunk
	RSoundSc iRecordSoundDevice;
	RChunk iRecordChunk;//handle to the shared chunk
	TState iState;

	//Playing Properties
	TPlaySharedChunkBufConfig iPlayBufferConfig;
	TInt iDeviceBufferLength;
	
	//Stores the status of CDataPathPlayer
	TRequestStatus* iClientPlayStatus;
	TPtrC8 iClientPlayData;
	//Stores the status of CSoundDevPlayErrorReceiver
	TRequestStatus* iClientPlayErrorStatus;
	RBuf8 iConvertedPlayData;
	RBuf8 iSavedTrailingData;

	CPlayer* iPlayers[KPlaySharedChunkBuffers];
	RFifo<CPlayer *, KPlaySharedChunkBuffers> iFreePlayers;
	RFifo<TUint32, KPlaySharedChunkBuffers> iActivePlayRequestSizes;
	
	TInt iRequestMinSize;
	TUint iRequestMinMask;
	
	//Recording Properties
	TRecordSharedChunkBufConfig iRecordBufferConfig;
	TInt iBufferOffset;
	TInt iBufferLength;

	//Stores the status of CDataPathRecorder
	TRequestStatus* iClientRecordStatus;
	//Stores the status of CSoundDevRecordErrorReceiver
	TRequestStatus* iClientRecordErrorStatus;
	TDes8* iClientRecordData;//stores the data pointer from datapath recorder
	RBuf8 iBufferedRecordData; // Used if RSoundSc returns more data than current client request requires.

	CRecorder* iRecorder; // We only need one recorder. The driver will buffer data for us.

	TBool iUnderFlowReportedSinceLastPlayOrRecordRequest;
	
	TUint64 iBytesPlayed;
	TUint32 iNTickPeriodInUsec;
	TUint32 iStartTime; // Time when previous driver PlayData completed (or first was issued) in msec
	TUint32 iPauseTime; // Time when pause started in msec
	TUint64 iPausedBytesPlayed;

	TFormatData iPlayFormatData;
	TFormatData iRecordFormatData;
	};
#endif
