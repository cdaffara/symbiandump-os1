// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_A2DPBLUETOOTH_PLAY_H__
#define __TSU_MMF_A2DPBLUETOOTH_PLAY_H__


#include "TSU_MMF_A2DPBLUETOOTH_Step.h"


_LIT(Ksbc_test_07,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.sbc");
_LIT(Ksbc_test_21,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_21.sbc");
_LIT(Ksbc_test_25,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_25.sbc");
_LIT(Kpcm16_test_04,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_04.wav");//44.1KHz , stereo
_LIT(Kpcm16_test_08,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_08.wav");//16KHz , stereo
_LIT(Kpcm16_test_07,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_07.wav");//16KHz, Mono
_LIT(Kpcm16_test_18,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_18.wav");//48KHz , stereo
_LIT(Kpcm16_test_21,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\sbc_test_21.wav");//44.1KHz, Mono
_LIT(Kmp3_test32kbs,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\mono32Khz.mp3");//32Khz 32kbs-1
_LIT(Kmp3Stereo_test32kbs,"\\mm\\mmf\\testfiles\\a2dpbluetooth\\stereo32Khz.mp3"); //32Khz 32kbs-1

const TUint K16000HzMonoSBCFrameSize = 36;
const TUint KSbc_TestFile07FrameSize = 36;
const TUint KSbc_TestFile21FrameSize = 46;
const TUint KSbc_TestFile25FrameSize = 70;
const TUint KPcm16_TestFile04FrameSize = 250;
const TUint KPcm16_TestFile07FrameSize = 96;
const TUint KPcm16_TestFile08FrameSize = 192;
const TUint KPcm16_TestFile18FrameSize = 512;
const TUint KWavHeaderAudioDataOffset = 0x2C;	
const TUint KDefaultNumberOfSBCFrames = 12;
const TUint KLargeBufferTestSize = 4096;
const TUint KMp3BufferTestSize = 432;
const TUint KNumberOfBuffersToComplete = 5;


class CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
protected:
	CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer(TFileName aFilename, TUint aBufferSize, TUint aSampleRate,  TUint aChannels, TMMFStereoSupport aStereoMode);
	virtual TVerdict DoTestStepL(void );
private:
	TFileName iFileName;
	TUint iBufferSize;
	TUint iSampleRate;
	TUint iChannels;
	TMMFStereoSupport iStereoSupport;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;	
	};


class CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
protected:
	CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers(TFileName aFilename, TUint aBufferSize, TUint aSampleRate,  TUint aChannels, TMMFStereoSupport aStereoMode);
	virtual TVerdict DoTestStepL(void );
	virtual void AsyncNotification(TInt aError);
private:
	TFileName iFileName;
	TUint iBufferSize;
	TUint iSampleRate;
	TUint iChannels;
	TMMFStereoSupport iStereoSupport;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TBool iStreaming;
	TUint iFilePos;
	TRequestStatus* iStatus;	
	};

class CTestStep_MMF_A2DPBLUETOOTH_U_0301 : public CTestStep_MMF_A2DPBLUETOOTH
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0301() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0301(){} ;
	virtual TVerdict DoTestStepL();
private:
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0302 : public CTestStep_MMF_A2DPBLUETOOTH
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0302() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0302(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TUint iFrameSize;
	TBool iStreaming;
	TUint iFilePos;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;


class CTestStep_MMF_A2DPBLUETOOTH_U_0303 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0303();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0303(){} ;
	};
	
class CTestStep_MMF_A2DPBLUETOOTH_U_0304 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0304();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0304(){} ;
	};
	

class CTestStep_MMF_A2DPBLUETOOTH_U_0305 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0305();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0305(){} ;
	};

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0306 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0306();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0306(){} ;
	};

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0307 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0307();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0307(){} ;
	};


class CTestStep_MMF_A2DPBLUETOOTH_U_0308 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0308();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0308(){} ;
	};
	
		
class CTestStep_MMF_A2DPBLUETOOTH_U_0309 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Buffer
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0309();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0309(){} ;
	};


class CTestStep_MMF_A2DPBLUETOOTH_U_0310 : public CTestStep_MMF_A2DPBLUETOOTH_Play_Multiple_Buffers
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0310();
	~CTestStep_MMF_A2DPBLUETOOTH_U_0310(){} ;
	};
	
		
class CTestStep_MMF_A2DPBLUETOOTH_U_0311 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0311() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0311(){} ;
	virtual TVerdict DoTestStepL();
private:
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	} ;
	
	
class CTestStep_MMF_A2DPBLUETOOTH_U_0312 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0312() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0312(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iBufferCount;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;
	
	
class CTestStep_MMF_A2DPBLUETOOTH_U_0313 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0313() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0313(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iBufferCount;
	TUint iBytesSent;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0314 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0314() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0314(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TBool iPaused;
	TUint iBufferCount;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0315 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0315() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0315(){} ;
	virtual TVerdict DoTestStepL();
private:
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0316 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0316() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0316(){} ;
	virtual TVerdict DoTestStepL();
private:
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0320 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0320() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0320(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iFilePos;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0330 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0330() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0330(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iBufferCount;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0331 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0331() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0331(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iBufferCount;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;

	
class CTestStep_MMF_A2DPBLUETOOTH_U_0350 : public CTestStep_MMF_A2DPBLUETOOTH_Initialized
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_U_0350() ;
	~CTestStep_MMF_A2DPBLUETOOTH_U_0350(){} ;
	virtual TVerdict DoTestStepL();
	virtual void AsyncNotification(TInt aError);
private:
	TBool iStreaming;
	TUint iFilePos;
	RFs iFileSession;
	RFile iFile;
	HBufC8* iBuffer;
	TRequestStatus* iStatus;
	} ;	

#endif /* __TSU_MMF_A2DPBLUETOOTH_PLAY_H__ */
