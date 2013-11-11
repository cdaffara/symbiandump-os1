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

#ifndef TSU_MMF_DEVSOUND_CIU_STEP_H
#define TSU_MMF_DEVSOUND_CIU_STEP_H

#include <e32base.h>
#include <testframework.h>

#include <mmf/server/sounddevice.h>
#include "devsoundciutestdevices.hrh"


/**
 * CActiveListener provides the asynchronous operation of an active object
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
 * CTestDevSoundCiuStep, which is the base class for all 
 * the UnitMMFAudClient suite test steps.
 *
 *
 *
 */
class CTestSuite;
class CTestSuiteCustomInterfaceUnificationUnit;

class CTestStepCustominterfaceUnificationUnit : public CTestStep,
												public MDevSoundObserver
	{
public:
	CTestStepCustominterfaceUnificationUnit();
	CTestStepCustominterfaceUnificationUnit(const TDesC& aTestName);
	~CTestStepCustominterfaceUnificationUnit();

	void SetSuite(CTestSuiteCustomInterfaceUnificationUnit* aSuite);
	CTestSuiteCustomInterfaceUnificationUnit* TestSuite() const;
	void WriteFileL(const TDesC &aName, TInt aData);
	void ReadFileL(const TDesC &aName, TInt& aData);

protected:
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);

	// Callback methods from MDevSoundObserver mixin
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);

	typedef enum TCallback 
		{
		EInitComplete,
		EToneFinished,
		EBuffToFill,
		EPlayError,
		EBuffToEmpty,
		ERecError,
		EConvertMsg,
		EDeviceMsg
		} TCallback;

	// Test methods (called by test cases)
	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestInitialize(TFourCC& aDataType, TMMFState aMode);
	TVerdict TestInitialize(TUid aHwDeviceUid, TMMFState aMode);
	
	TAny* InitializeAndInstantiateInterfaceL(const TUid aHwDeviceUid, const TUid aInterfaceUid);
	TAny* InitializeAndInstantiateInterfaceL(TFourCC& aDataType, const TUid aInterfaceUid);

	// Utility Methods
	void ResetCallbacks();

	CMMFDevSound* iMMFDevSound;
private:
	TFixedArray<TInt, 7> iCallbackArray;
	TInt iCallbackError;
	TInt iExpectedValue;
	// pointer to suite which owns this test 
	CTestSuiteCustomInterfaceUnificationUnit* iTestSuite;
	CActiveScheduler*	iScheduler ;
	CActiveListener*	iActiveListener;

protected:
	TFileName	iSourceFileName;
	TFileName*	iRes;
	RFs		iFs;
	RFile	iFile;
	CMMFBuffer* iBuffer;  // for callback usage
	};

#endif 
