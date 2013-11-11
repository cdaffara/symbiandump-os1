// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef  TSU_MMF_AVICTRLRECORD_H
#define TSU_MMF_AVICTRLRECORD_H

#include "tsu_mmf_avictrl_teststep.h"
const TInt KBalanceMultiplyFactor = 30;

/**
This class tests recording an .avi file for 2 secs using the AviRecordController
*/  
class RTestStepAviRecordCtrlRecord : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
	
/**
This class tests setting the gain on AviRecordController in Stopped, Primed and Playing 
states and checks if the gain set is same as the gain got.
*/  
class RTestStepAviRecordCtrlGain : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGain(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
	
/**
This class tests setting the RecordBalance on AviRecordController in Stopped, Primed and Playing 
states and checks if the RecordBalance set is same as the RecordBalance got.
*/   
class RTestStepAviRecordCtrlRecordBalance : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlRecordBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This test Primes the AviRecordController and sets a limit on the recorded file size
 and checks for the KErrEof from the AviRecordController
*/    
class RTestStepAviRecordCtrlSetMaxFileSize : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetMaxFileSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
/**
This class checks for the available record time using .avi record controller.
*/  
class RTestStepAviRecordCtrlRecordTimeAvailable : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlRecordTimeAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This tests setting the frame rate on AviRecordController in Stopped, Primed and Recording
states and checks if the framerate set is same as the frame rate got. 
*/  
class RTestStepAviRecordCtrlSetGetFrameRate : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetGetFrameRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This tests setting the bitrate on AviRecordController in Stopped, Primed and Recording 
 states and checks if the bitrate set is same as the bitrate got.
*/  
 class RTestStepAviRecirdCtrlSetGetFrameSize : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecirdCtrlSetGetFrameSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This tests setting the SinkAudioDataType on AviRecordController in Stopped, Primed and Recording 
states and checks if the SinkAudioDataType set is same as the SinkAudioDataType got.
*/   
class RTestStepAviRecordCtrlGetSetAudioDataType : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSetAudioDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This tests setting the SinkVideoDataType on AviRecordController in Stopped, Primed and Recording 
 states and checks if the SinkVideoDataType set is same as the SinkVideoDataType got.
*/ 
 class RTestStepAviRecordCtrlGetSetVideoDataType : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSetVideoDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This test Adds new metadata to the Avi file and checks if the meta data is added to the clip
*/  
class RTestStepAviRecordCtrlGetSetMetaData : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSetMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

 /**
This test Primes the AviRecordController, Records an .Avi file for 2 secs and stop the controller
 recording again and checks the duration
*/ 
 class RTestStepAviRecordCtrlDuration : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This test Primes the AviRecordController and does the -ve testing for gain and balance when audio is not supported
*/ 
 class RTestStepAviRecordNotAudioEnabled : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordNotAudioEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
 
/**
This test tests for setting and getting the recording position and see if it returns KErrNotSupported
*/ 
class RTestStepAviRecordCtrlPosition : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This test checks for Pause functionality while recording and checks if it returns KErrNotSupported
*/  
class RTestStepAviRecordCtrlPause : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
DEF088332: MVS UI Application does not update the timer during video recording
This tests the Duration functionality of the AviPlayController.
*/  
class RTestStepAviRecordCtrlDurationNewClip : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlDurationNewClip(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting and setting the video pixel aspect ratio for recording.
*/  
class RTestStepAviRecordCtrlSetGetPixelAspectRatio : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetGetPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting the list of suported video pixel aspect ratios for recording.
*/  
class RTestStepAviRecordCtrlGetSupportedPixelAspectRatios : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSupportedPixelAspectRatios(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting and setting the audio sample rate for recording.
*/  
class RTestStepAviRecordCtrlSetGetAudioSampleRate : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetGetAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting the list of supported audio sample rates for recording.
*/  
class RTestStepAviRecordCtrlGetSupportedAudioSampleRate : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSupportedAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting and setting the number of audio channels for recording.
*/  
class RTestStepAviRecordCtrlSetGetAudioChannels : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetGetAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests getting the list of supported numbers of audio channels for recording.
*/  
class RTestStepAviRecordCtrlGetSupportedAudioChannels : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlGetSupportedAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting an invalid video pixel aspect ratio for recording.
*/  
class RTestStepAviRecordCtrlSetInvalidPixelAspectRatio : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetInvalidPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting an invalid audio sample rate for recording.
*/  
class RTestStepAviRecordCtrlSetInvalidAudioSampleRate : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetInvalidAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting an invalid number of audio channels for recording.
*/  
class RTestStepAviRecordCtrlSetInvalidAudioChannels : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlSetInvalidAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting the video pixel aspect ratio for recording while recording (invalid state).
*/  
class RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlInvalidStateSetPixelAspectRatio(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting the audio sample rate for recording while recording (invalid state).
*/  
class RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlInvalidStateSetAudioSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

/**
CR0958: MMF Video Recorder Utility API improvements
This tests setting the number of audio channels for recording while recording (invalid state).
*/  
class RTestStepAviRecordCtrlInvalidStateSetAudioChannels : public RTestStepAviCtrlRecordBase
	{
public:
	RTestStepAviRecordCtrlInvalidStateSetAudioChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
protected:
	};

#endif //(TSU_MMF_AVICTRLRECORD.H)
