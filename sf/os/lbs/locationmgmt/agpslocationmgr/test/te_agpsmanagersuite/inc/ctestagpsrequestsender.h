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
// ctestrequestsenderbase.h
// Base class for sending requests to the AGPS Manager using
// the same channels that the Location Server or the
// Network Request Handler would
// 
//

#ifndef __CTESTREQUESTSENDERBASE_H__
#define __CTESTREQUESTSENDERBASE_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "lbsprocessuiddefs.h"
#include "lbsinternalinterface.h"

/** Class that sends requests to the
AGPS Manager through the Position Update
Request bus (as LS and NRH do)
*/
class CTestAgpsRequestSender : public CBase
	{
	
public:
	//enum TTestQualityLevel
	//{
	//	ETQualityLevelLow, // Horiz Acc = Vertical Acc = 100 m; MaxFixTime 5 secs
	//	ETQualityLevelMedium, 
	//	ETQualityLevelHigh // Horiz Acc = Vertical Acc = 1 m; MaxFixTime 5 secs
	//};
	
	enum TTestValidPosMethod
	{
		ETPosMethodAut,   // Autonomous
		ETPosMethodTb, 	  // Terminal Based
		ETPosMethodTa, 	  // Terminal Assisted
		ETPosMethodTaAut, // Terminal Assisted and Autonomous
		ETPosMethodTbTa   // Terminal Assisted and TerminalBased
	};

public:

	static CTestAgpsRequestSender* NewL(const TUint aChannelUidValue);
	virtual	~CTestAgpsRequestSender();

	// Use these methods when only the positioning methods in
	// the request are important for the test
	void RequestPositionUpdate(const TTestValidPosMethod& aPosMethod);
	void RequestPositionUpdate(const TLbsNetPosRequestMethodInt& aPosMethods);

	// Use this call when only the level or horizontal/vertical
	// accuracy are important for the test
	// RequestLocationUpdate(const TTestQualityLevel& aQualityLevel)
	
	// Use this call when only the target time and MaxFixTime of
	// the Location Request are relevant to the test
	// RequestLocationUpdate(const TTime& aTargetTime, const TTime& aMaxFixTime)

	// The test can use this call to specify all the parameters in a location update request
	void RequestPositionUpdate(const TLbsLocRequestQualityInt& aQuality,const TLbsNetPosRequestMethodInt& aPosMethods, const TTime& aTargetTime);

	void CancelPositionUpdate();
	
	void AdviseStatus(const TLbsPositionUpdateRequestBase::TPowerAdvice& aPowerAdvice);

private:

	void ConstructL(TUint aChannelUidValue);
	CTestAgpsRequestSender();

	void SendRequest(const TLbsPositionUpdateRequestBase& aRequest);

private:

	RLbsPositionUpdateRequests iPosUpdateReq;
	};

#endif //__CTESTREQUESTSENDERBASE_H__