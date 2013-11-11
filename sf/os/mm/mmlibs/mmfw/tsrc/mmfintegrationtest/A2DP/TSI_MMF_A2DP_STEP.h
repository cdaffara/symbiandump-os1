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

#ifndef __TSI_MMF_A2DP_STEP_H__
#define __TSI_MMF_A2DP_STEP_H__


#include <e32base.h>
#include <badesca.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <utf.h>

#include <TestFramework.h>
#include <MdaAudioSamplePlayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioTonePlayer.h>
#include <MdaAudioOutputStream.h>
#include <mmfFormatImplementationUIDs.hrh>
#include <mmfControllerImplementationUIDs.hrh>
#include <mmfHwDeviceImplementationUids.hrh>
#include <mda\common\resource.h>
#include <mda\common\audio.h>
#include <mda\common\gsmaudio.h> 

#include <centralrepository.h>
#include <bt_sock.h>
#include "MmfRoutingPrioritySettings.h"

#if defined (__WINS__)
#define BT_COMM_PDD_NAME _L("ECDRV")
#define BT_COMM_LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define BT_COMM_PDD_NAME _L("EUART1")
#define BT_COMM_LDD_NAME _L("ECOMM")
#endif

const TUid KUidBTHeadsetRepository = {0x1020497A};
const TUint KTimeToRegisterLinkLost = 30000000; //30 seconds

#include "TSI_MMF_A2DP_SUITE.h"

//Test directory/files:
_LIT(KInputTestFileNamePCM16, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCM_08_16_m.raw");
_LIT(KInputTestFileNamePCM8, "C:\\TSU_MMF_DEVSOUND_SUITE\\Input\\PCMU8Source.raw");
_LIT(KAclntFileNamePCM16M, "C:\\AclntITestData\\PCM16BitMono.wav");


class CTestSuite;
class CTestSuiteA2dp;


/**
 *	Test asynch call back.
 */
class CAsyncTestStepNotifier : public CActive
	{
public:
	static CAsyncTestStepNotifier* NewL(RTestStepA2dp* aTestStep);
	~CAsyncTestStepNotifier();

	TRequestStatus& RequestStatus() 
		{
		return iStatus;
		}

	void HandleAsyncRequest();
	//CActive
	void RunL();
	void DoCancel();

private:
	CAsyncTestStepNotifier();
	void ConstructL(RTestStepA2dp* aTestStep);
private:
	RTestStepA2dp* iTestStep;
	};


/**
 * Class used by the test harness to stop the active scheduler 
 * after a timeout period
 */
class CStopActiveSchedulerTimer : public CTimer
	{
public:
	static CStopActiveSchedulerTimer* NewL();
	void StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval);
private:
	CStopActiveSchedulerTimer();
	void ConstructL();
	virtual void RunL();
	};


/**
 *
 * RTestStepA2dp - Test class that enables A2dp tests on both DevSound and Aclnt
 * It's a base class for all test steps.
 *
 * @lib "TSI_MMF_A2DP_SUITE.lib"
 *
 *
 */
class RTestStepA2dp :  public RTestStep, 
					   public MDevSoundObserver, 
					   public MMdaAudioPlayerCallback, 
					   public MMdaObjectStateChangeObserver, 
					   public MMdaAudioToneObserver,
					   public MMdaAudioOutputStreamCallback,
					   public MBluetoothPhysicalLinksNotifier
	{
public:
	RTestStepA2dp();
	~RTestStepA2dp();
	void SetTestSuite(const CTestSuiteA2dp* aTestSuite);
	void AsyncNotification(TInt aError);

protected:	
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

//MBluetoothPhysicalLinksNotifier
	virtual void HandleCreateConnectionCompleteL(TInt /*aErr*/){};
	virtual void HandleDisconnectCompleteL(TInt aErr);
	virtual void HandleDisconnectAllCompleteL(TInt /*aErr*/){};

//MDevSoundObserver
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* /*aBuffer*/){};
	void RecordError (TInt /*aError*/){};
	void ConvertError (TInt /*aError*/){};
	void DeviceMessage (TUid /*aMessageType*/, const TDesC8& /*aMsg*/){};
	
//MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt /*aError*/){};

//MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);


// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt /*aError*/){};

// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/);
	virtual void MaoscPlayComplete(TInt aError);
	
	TVerdict InitializeDevSound(TMMFState aMode, TBool aHeadset);
	TVerdict DevSoundPlayInit();
	TInt DevSoundPlayData();

// Utility Methods
	void ResetCallbacks();	
		
	enum TCallback 
		{
		EBuffToFill,
		EPlayError
		};
	
	
	
	TFixedArray<TInt, 2> iCallbackArray;

//pointer to suite which owns this test 
	const CTestSuiteA2dp* iA2dpTestSuite;

//Active Scheduler
	CActiveScheduler* iAS;
	CStopActiveSchedulerTimer* iTimer;
	
//Bluetooth Physical Link
	TBTDevAddr* iBTheadsetAddress;
	CBluetoothPhysicalLinks* iBtPhysicalLink;
	RSocketServ iSocketServer;
	TBool iBTLinkDisconnected;
	
	CMMFDevSound* iMMFDevSound;
	CAsyncTestStepNotifier* iAsyncRequestHandler;
	TInt iCallbackError;
	CMMFBuffer* iBuffer;
	TInt iExpectedValue;
	TTimeIntervalMicroSeconds iDuration;
	TInt iError;
	};


#endif// __TSI_MMF_A2DP_STEP_H__



