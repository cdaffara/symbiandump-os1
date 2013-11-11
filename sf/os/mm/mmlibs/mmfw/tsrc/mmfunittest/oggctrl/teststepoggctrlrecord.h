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

#ifndef TESTSTEPOGGCTRLRECORD_H
#define TESTSTEPOGGCTRLRECORD_H

#include "tsu_mmf_oggctrl_teststep.h"

/**
 *
 * This class tests recording an ogg file for 2 secs using the OggRecordController
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecord : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecord(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the gain on OggRecordController in Stopped, Primed and Playing 
 * states and checks if the gain set is same as the gain got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGain : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGain(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the RecordBalance on OggRecordController in Stopped, Primed and Playing 
 * states and checks if the RecordBalance set is same as the RecordBalance got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecordBalance : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecordBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This test Primes the OggRecordController and sets a limit on the recorded file size
 * and checks for the KErrEof from the OggRecordController
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlSetMaxFileSize : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlSetMaxFileSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the PlayBalance on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the PlayBalance set is same as the PlayBalance got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecordTimeAvailable : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecordTimeAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests cropping an existing file and checks for the duration
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecordCrop : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecordCrop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName1, const TDesC& aKeyName2, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	TVerdict DoTestStepPostambleL();
	TBool SetupWriteFileConfigL();
private:
	TBuf<25> iKeyName2; //specific to this test step
	};

/**
 *
 *This test Adds new metadata to the Ogg file and checks if the meta data is added to the clip
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetMetaData : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSetMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
/**
 *
 * This tests setting the Number Of Channels on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the Number Of Channels set is same as the Number Of Channels got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetSinkChannels : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSetSinkChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This tests setting the samplerate on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the samplerate set is same as the samplerate got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetSinkSampleRate : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSetSinkSampleRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This tests setting the bitrate on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the bitrate set is same as the bitrate got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetSinkBitRate : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSetSinkBitRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This tests setting the SinkDataType on OggRecordController in Stopped, Primed and Recording 
 * states and checks if the SinkDataType set is same as the SinkDataType got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetSinkDataType : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSetSinkDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
/**
 *
 * This tries to get SupportedSinkChannels from OggRecordController in Stopped
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSupportedSinkNoChannels : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSupportedSinkNoChannels(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This tries to get SupportedSinkSampleRates from OggRecordController in Stopped
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSupportedSinkSampleRates : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSupportedSinkSampleRates(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This tries to get SupportedSinkBitRates from OggRecordController in Stopped
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSupportedSinkBitRates : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlGetSupportedSinkBitRates(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This test Primes the OggRecordController, Records an Ogg file for 2 secs and stops the controller
 * records again and checks the duration
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlAppend : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlAppend(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
	
	
/**
 *
 * This class tests recording on a readonly ogg file. Expected results is to receive KErrAccessDenied 
 * and have the controller stopped.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlFailsWithAccessDenied : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlFailsWithAccessDenied(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPostambleL(void);
	TBool SetupWriteFileConfigL();
	virtual TVerdict  DoTestStepL();
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);	
	};	

/**
 *
 * DEF116323
 * This class tests recording an ogg file with sample rate 12KHz for 2 secs using the OggRecordController
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecord12 : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecord12(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
	
/**
 *
 * DEF116323
 * This class tests recording an ogg file with sample rate 24KHz for 2 secs using the OggRecordController
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlRecord24 : public RTestStepOggCtrlRecordBase
	{
public:
	RTestStepOggCtrlRecord24(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
#endif //(TESTSTEPOGGCTRLRECORD_H)
