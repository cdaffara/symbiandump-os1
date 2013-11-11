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
// Description: Stores information for a X3P test sequence
// 
//

#ifndef __CT_LBS_TEST_X3P_SESSION_SEQUENCE_H__
#define __CT_LBS_TEST_X3P_SESSION_SEQUENCE_H__

#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsx3p.h>

#include "clbstestsessionsequence.h"

/* Call back function which the X3P test sequence should implement to receive 
 * notifications of when a message is received.
 */
class MTestX3PPositionListener
	{
public:
	virtual void NotifyX3PEvent(TInt aId) = 0;
	};

/** When making the X3P request, it requires two TRequestStatus objects that it completes when
 *   a valid position is available.  This class is used to represent those TRequestStatus (one
 *   instance of the class for each TRequestStatus).  It notifies the X3P session sequence when
 *   the TRequestStatus completes (via the callback) allowing the test to know it has received
 *   an X3P Position.
 */
class CTestX3PPositionListener : public CActive
	{
public:
	static CTestX3PPositionListener* NewL(MTestX3PPositionListener& aObserver, TInt aCallBackId);
	~CTestX3PPositionListener();
	
	//From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	//Returns the TRequestStatus
	TRequestStatus& GetStatus();
	
protected:
	CTestX3PPositionListener(MTestX3PPositionListener& aObserver, TInt aCallBackId);
	
private:
	//Id of this class (EX3PRefPositionUpdate or EX3PPositionUpdate)
	TInt iCallBackId;
	MTestX3PPositionListener& iObserver;
	};

/** Class for representing a session sequence
*/
class CTestX3PSessionSequence : public CTestSessionSequence, public MTestX3PPositionListener
{
public:
	static CTestX3PSessionSequence* NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
			TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	~CTestX3PSessionSequence();

	void TransmitPosition();
	
	// NG Message Handlers
	void SendProcessStatusUpdate();
	void SendProcessLocationRequest();
	
	//NG Message Checkers
	void CheckRequestTransmitLocation(TLbsNetSessionId* aSessionId, TInt aPriority, const TDesC16& aThirdParty);
	
	//from MTestX3PPositionListener
	void NotifyX3PEvent(TInt aId);
		
protected:
	CTestX3PSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback);
	void ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
	void SetupTransmit();
	
private:
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iTransmitter;
	TBool iSetup;
	CTestX3PPositionListener* iRefPosListener;
	CTestX3PPositionListener* iPosListener;
	TPositionInfo iRefPosInfo;
	TPositionInfo iTransPosInfo;
};

#endif	//__CT_LBS_TEST_X3P_SESSION_SEQUENCE_H__

