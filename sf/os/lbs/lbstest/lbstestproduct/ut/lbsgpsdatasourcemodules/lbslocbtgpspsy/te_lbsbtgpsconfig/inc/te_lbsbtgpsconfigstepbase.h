// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// lbsbtgpsconfig unit test: Base test step 
//

#ifndef TE_LBS_BTGPSCONFIGSTEPBASE_H
#define TE_LBS_BTGPSCONFIGSTEPBASE_H

#include "te_lbsbtgpsconfigserver.h"

#include <lbs/lbsbtgpsconfig.h>


class CLbsBtGpsConfigStepBase : public CTestStep, public CActive, public MLbsBtGpsConfigObserver
	{
public:
	CLbsBtGpsConfigStepBase();
	~CLbsBtGpsConfigStepBase();
	
	void SetTestServer(CLbsBtGpsConfigTestServer* aServer);
	
	// From CTestStep:
 	TVerdict doTestStepPreambleL(void);
	TVerdict doTestStepPostambleL(void);
	TVerdict doTestStepL(void);

	// From MLbsBtGpsConfigObserver:
	void OnDeviceListUpdate();
	
protected:

	// From CActive:
	void DoCancel();
	void RunL();

protected:
	
	virtual TBool RunTestStage(TInt aStage) =0;
	
protected:

	// Test stage
	TInt iStage;
	
	// Timer
	RTimer iTimer;
	
	CLbsBtGpsConfigTestServer* iServer;
	
	RArray<TLbsBtGpsEntryKey> iKeys;
	
	TInt iUpdatesRecieved;
	
	};


#endif //TE_LBS_BTGPSCONFIGSTEPBASE_H
