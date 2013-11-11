// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_A2DPBLUETOOTH_ASYNC_H__
#define __TSU_MMF_A2DPBLUETOOTH_ASYNC_H__

#include <E32base.h>

class CTestStep_MMF_A2DPBLUETOOTH; //forward ref

/**
 * This is a utility class used by CTestStep_MMF_A2DPBLUETOOTH test steps
 * in order to allow test steps to test the methods of
 * the CA2DPHeadsetAudioInterface that take a TRequestStatus without
 * the test step itself having to derive from CActive.
 * Test steps use the TRequestStatus from CAsyncTestStepNotifier for the
 * API under test.  The AsyncNotification method on the test step
 * is called back when the TRequestStatus completes.  A mixin class
 * has not been used for the AsyncNotification since this class is just 
 * used internally.
 * @lib "TSU_MMF_A2DPBLUETOOTH.lib"
 *
 */
class CAsyncTestStepNotifier : public CActive
	{
public:
	static CAsyncTestStepNotifier* NewL(CTestStep_MMF_A2DPBLUETOOTH* aTestStep);
	~CAsyncTestStepNotifier();
	TRequestStatus& RequestStatus() 
		{
		return iStatus;
		}
	void HandleAsyncRequest();
	void SetActiveIfNotActive();
	//CActive
	void RunL();
	void DoCancel();
private:
	CAsyncTestStepNotifier();
	void ConstructL(CTestStep_MMF_A2DPBLUETOOTH* aTestStep);
private:
	CTestStep_MMF_A2DPBLUETOOTH* iTestStep;
	};


#endif /* __TSU_MMF_A2DPBLUETOOTH_ASYNC_H__ */
