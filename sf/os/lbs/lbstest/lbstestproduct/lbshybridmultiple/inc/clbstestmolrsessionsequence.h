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
// Description: Stores information for a MOLR test sequence
// 
//

#ifndef __CT_LBS_TEST_MOLR_SESSION_SEQUENCE_H__
#define __CT_LBS_TEST_MOLR_SESSION_SEQUENCE_H__

#include <lbs.h>
#include <lbscommon.h>

#include "clbstestsessionsequence.h"
#include <lbs/test/posserverwatch.h>

/** Class for representing a session sequence
*/
class CTestMolrSessionSequence : public CTestSessionSequence, public MPosServerObserver
{
public:
	static CTestMolrSessionSequence* NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
			TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	~CTestMolrSessionSequence();

	//Self Locate Functions
	void RequestLocationUpdate();
	
	//NG Message Functions
	void SendProcessLocationRequest();
	void SendProcessStatusUpdate();
	
	//NG Checker Functions
	void CheckSelfLocationRequest(TLbsNetSessionId* aSessionId);
    void CheckNetworkLocationRequest(TLbsNetSessionId* aSessionId);
	
protected:
	//from MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
	void SetupPsy();
		
protected:
	CTestMolrSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback);
	void ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
private:
	CPosServerWatcher* iPosWatcher;
	RPositionServer iPosServer;
	RPositioner iPos;
	TBool iSetupPsy;
	TUid iPsyUid;
};

#endif	//__CT_LBS_TEST_MOLR_SESSION_SEQUENCE_H__
