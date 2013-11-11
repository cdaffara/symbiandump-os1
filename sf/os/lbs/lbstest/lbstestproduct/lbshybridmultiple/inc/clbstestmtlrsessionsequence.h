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
// Description: Stores information for a MTLR test sequence
// 
//

#ifndef __CT_LBS_TEST_MTLR_SESSION_SEQUENCE_H__
#define __CT_LBS_TEST_MTLR_SESSION_SEQUENCE_H__

#include "clbstestsessionsequence.h"

/** Class for representing a session sequence
*/
class CTestMtlrSessionSequence : public CTestSessionSequence
{
public:
	static CTestMtlrSessionSequence* NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
			TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	~CTestMtlrSessionSequence();
	
	void CheckRespondPrivacyRequest(CLbsNetworkProtocolBase::TLbsPrivacyResponse aPrivacyResponse);
	
protected:
	//NG Messages
	void SendProcessPrivacyRequest();
	void SendProcessLocationRequest();
	void SendProcessStatusUpdate();
	
protected:
	CTestMtlrSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback);
	void ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
private:
	//Indicates whether the sequence is an emergency sequence
	TBool iEmergency;
	//Contains the service type of the request (either MTLR or NI)
	MLbsNetworkProtocolObserver::TLbsNetProtocolService iService;
};

#endif	//__CT_LBS_TEST_MTLR_SESSION_SEQUENCE_H__
