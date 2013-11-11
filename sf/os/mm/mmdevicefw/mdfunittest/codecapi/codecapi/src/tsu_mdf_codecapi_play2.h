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
// Header file: More play related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_play2.h
*/

#ifndef TSU_MDF_CODECAPI_PLAY2_H
#define TSU_MDF_CODECAPI_PLAY2_H

#include "tsu_mdf_codecapi_play.h"

class CMdfTimerPlayStop;	

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0101-HP
 * Play an audio file and stop it after N seconds.
 *
 * @class RTestStepCodecApiPlayPartial 
 *
 */
class RTestStepCodecApiPlayPartial : public RTestStepCodecApiPlay
	{
public:
	RTestStepCodecApiPlayPartial();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0145-HP
 * Play an audio file, stop and delete codec after N seconds.
 *
 * @class RTestStepCodecApiPlayStopDelete 
 *
 */
class RTestStepCodecApiPlayStopDelete : public RTestStepCodecApiPlay
	{
public:
	RTestStepCodecApiPlayStopDelete();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0125-HP
 * Play an audio file, stop and start playing again after N seconds.
 *
 * @class RTestStepCodecApiPlayStopRestart 
 *
 */
class RTestStepCodecApiPlayStopRestart : public RTestStepCodecApiPlay
	{
public:
	RTestStepCodecApiPlayStopRestart();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Active player for RTestStepCodecApiPlayPartial and derived test classes.
 *
 * @class CPlayAudioFilePartial 
 *
 */
class CPlayAudioFilePartial : public CPlayAudioFile
	{
public:
	static CPlayAudioFilePartial* NewL(RTestStepCodecApi* aParent);
	~CPlayAudioFilePartial();
	virtual void RunL();
protected:
	CPlayAudioFilePartial(RTestStepCodecApi* aParent);
private:
	void ConstructL();	

protected:
	CMdfTimerPlayStop* iPlayTimer;
	};

/**
 * 
 * Active player for RTestStepCodecApiPlayStopDelete and derived test classes.
 *
 * @class CPlayAudioFileStopDelete 
 *
 */
class CPlayAudioFileStopDelete : public CPlayAudioFilePartial
	{
public:
	static CPlayAudioFileStopDelete* NewL(RTestStepCodecApi* aParent);
	~CPlayAudioFileStopDelete();
private:
	CPlayAudioFileStopDelete(RTestStepCodecApi* aParent);
	void ConstructL();	
	virtual void Finished();
	};

/**
 * 
 * Active player for RTestStepCodecApiPlayStopRestart and derived test classes.
 *
 * @class CPlayAudioFileStopRestart 
 *
 */
class CPlayAudioFileStopRestart : public CPlayAudioFile
	{
public:
	static CPlayAudioFileStopRestart* NewL(RTestStepCodecApi* aParent);
	~CPlayAudioFileStopRestart();
	virtual void RunL();
private:
	CPlayAudioFileStopRestart(RTestStepCodecApi* aParent);
	void ConstructL();	

protected:
	CMdfTimerPlayStop* iPlayTimer;
	TBool	iRestarted;
	};

/**
 * 
 * Timer for those test classes requiring timed stop
 *
 * @class CMdfTimerPlayStop 
 *
 */
class CMdfTimerPlayStop : public CTimer
	{
public:
	static CMdfTimerPlayStop* NewL(CPlayAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime);	
	void Start();	
protected:
	TInt RunError(TInt aError);	
	void RunL();
private:
	CMdfTimerPlayStop(CPlayAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime);
	CPlayAudioFile* iParent;
	CMMFHwDevice* iHwDevice;
	TTimeIntervalMicroSeconds32 iWaitTime;
	};
	
#endif // TSU_MDF_CODECAPI_PLAY2_H


