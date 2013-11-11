// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __UNIT_MMF_AUD_CLNT_TESTSTEP_H__)
#define __UNIT_MMF_AUD_CLNT_TESTSTEP_H__

// EPOC includes
#include <e32base.h>
#include <testframework.h>
#include "mmfclientutility.h"
#include <mdaaudiosampleplayer.h>
/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityIdle) { CActiveScheduler::Add(this); }
	void InitialiseActiveListener() { iStatus = KRequestPending; SetActive(); }
	// From CActive
	virtual void RunL() { CActiveScheduler::Stop(); }
	virtual void DoCancel() {};
	};

/**
 * Utility class for asynchronous timed callbacks
 */
class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(TCallBack aCallBack, TPriority aPriority = EPriorityStandard);
private:
	virtual void RunL();
private:
	CCallBackTimer(TCallBack aCallBack, TPriority aPriority);
private:
	TCallBack iCallBack;
	};



class CTestSuiteUnitMMFAudClient;
class CMMFMdaAudioPlayerUtility;
class CMMFMdaAudioToneUtility;
class CMMFMdaAudioOutputStream;
class CMMFMdaAudioInputStream;
class CMMFMdaAudioRecorderUtility;
class CMMFMdaAudioConvertUtility;
class CMdaAudioRecorderUtility;
class TMMFMessageDestination;
class TMMFPrioritySettings;
/**
 *
 * CTestStepUnitMMFAudClient, which is the base class for all 
 * the UnitMMFAudClient suite test steps.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepUnitMMFAudClient : public CTestStep
	{
public:
	CTestStepUnitMMFAudClient();
	~CTestStepUnitMMFAudClient();

	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	
	// accessors
	void SetUnitMMFAudClientSuite(CTestSuiteUnitMMFAudClient* aSuite);
	inline CTestSuiteUnitMMFAudClient* UnitMMFAudClientSuite() const;

	TInt GetAudPlayerState(CMMFMdaAudioPlayerUtility*, TInt& /*aState*/);
	TInt GetAudPlayerSourceHandle(CMMFMdaAudioPlayerUtility*, TMMFMessageDestination* /*aSrcHandle*/);
	TInt GetAudPlayerPriority(CMMFMdaAudioPlayerUtility*, TMMFPrioritySettings&);

	TInt GetAudToneType(CMMFMdaAudioToneUtility*, TInt& /*aType*/);
	TInt GetAudToneTypeSafe(CMMFMdaAudioToneUtility*, TInt& /*aType*/);
	TInt GetAudTonePriority(CMMFMdaAudioToneUtility*, TMMFPrioritySettings& /*aPrior*/);

	TInt GetAudOutStreamStopped(CMMFMdaAudioOutputStream*, TBool& /*aStopped*/);
	TInt GetAudOutStreamPlaying(CMMFMdaAudioOutputStream*, TBool& /*aStopped*/);
	TInt GetAudOutStreamPosition(CMMFMdaAudioOutputStream*, TTimeIntervalMicroSeconds& /*aPosition*/);

	TInt GetAudInStreamStopped(CMMFMdaAudioInputStream*, TBool& /*aStopped*/);
	TInt GetAudInStreamRecording(CMMFMdaAudioInputStream*, TBool& /*aStopped*/);
	TInt GetAudInStreamPosition(CMMFMdaAudioInputStream*, TTimeIntervalMicroSeconds& /*aPosition*/);

	TInt GetAudRecorderPrioritySettings(CMMFMdaAudioRecorderUtility*, TMMFPrioritySettings&);
	TInt GetAudRecorderRecTimeAvail(CMMFMdaAudioRecorderUtility*, TTimeIntervalMicroSeconds&);
	TInt SetAudRecorderParent(CMMFMdaAudioRecorderUtility* aUtility, CMdaAudioRecorderUtility* aParent);
	
	void SetTestMediaIdL(CMMFMdaAudioPlayerUtility* aPlayer);
	void SetTestMediaIdL(CMMFMdaAudioRecorderUtility* aRecorder);
	void SetTestMediaIdL(CMMFMdaAudioConvertUtility* aConverter);
	
	void SetTestDummyMediaIdL(CMMFMdaAudioPlayerUtility* aPlayer);  //to select dummy controller
	void SetTestDummyMediaIdL(CMMFMdaAudioRecorderUtility* aRecorder);
	
	TInt OpenFileWithTestControllerL(CMMFMdaAudioPlayerUtility*, TDesC& aFileName);
	const TDesC& DefaultPath() const;
		
private:
	// pointer to suite which owns this test 
	CTestSuiteUnitMMFAudClient* iUnitMMFAudClientSuite;
	CActiveScheduler*           iScheduler ;

protected:
	TPtrC iFileName;
	TPtrC* iRes;

	RFs iFs;
	RFile iFile;
	};

//Test for DEF113412:CMMFFindAndOpenController::SendError enters infinite loop on 
class CTsuClientUtilityControllerLoading : public RAsyncTestStep, public MMdaAudioPlayerCallback
	{
public:
static	CTsuClientUtilityControllerLoading* NewL(const TDesC& aStepName, const TDesC& aFileName);
	//MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
private:
	CTsuClientUtilityControllerLoading(const TDesC& aStepName, const TDesC& aFileName);	
	RFs iFs;
	TFileName iFileName;
	CMdaAudioPlayerUtility* iUtility;
	};

#endif //__UNIT_MMF_AUD_CLNT_TESTSTEP_H__
