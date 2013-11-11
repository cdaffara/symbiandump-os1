/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbstestprivacycontroller.h
*/
#ifndef __LBS_TEST_PRIVACY_CONTROLLER_H__
#define __LBS_TEST_PRIVACY_CONTROLLER_H__

#include <lbs/lbsprivacycontroller.h>
//#include "privfwproppipe.h"
#include <lbs/test/tlbstestrequestchannel.h>
//#include <lbs/test/lbsprivfwdefs.h>


class CTestPrivacyController : public CBase, public MLbsPrivacyObserver2, public MLbsTestChannelRequestObserver
	{
public:
	static CTestPrivacyController* NewL();
	~CTestPrivacyController();

	//From MLbsPrivacyObserver2
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);	
	void ProcessCloseDownNotification(TInt32 aReservedParam);
		
private:
	CTestPrivacyController();
	void ConstructL();

    static TInt TimerCallBack(TAny* aPtr);
    static TInt CompletionTimerCallBack(TAny* aPtr);
	void RespondNetworkLocationRequest();
	void ResetResponseTimer();

	void ConvertToTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo, const TLbsExternalRequestInfo2& aRequestInfo);
	
	//From MPrivFwReadPropPipeObserver
//	void OnReadyToReadL(CPrivFwReadPropPipe& aPipe);
	
	//From MLbsPrivFwChanelObserver
	//virtual void ProcessPrivFwChannelMessage(RLbsPrivFwChannel::TLbsPrivFwChannelId aChannelId, const TLbsPrivFwMsgBase& aMessage);

	//From MLbsTestChannelRequestObserver
	virtual void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);

private:
	//TInt	iCompleteReason;
	//TMessageType 	iPrivFwMsgType;
	typedef struct
		{
		TUint	iRequestId;
		TBool	iIsTimerCreated;
		CPeriodic*	iCompletionTimer;
		} TCompletionTimer;
		
	RArray<TCompletionTimer>	iCompletionTimerArray;
	RArray<TInt32>				iCompletionRequestIdArray;
		
	// the lbs privacy controller interface:
	CLbsPrivacyController* iLbsPrivacyController;

    CPeriodic*				iTimer;
    //CPeriodic*				iCompletionTimer;
    //TInt32					iCompletionRequestId;
    TResponseType			iCompletionResponse;
//	CPrivFwWritePropPipe* 		iResponseWriter;
//	CPrivFwNotifyReadPropPipe* 	iMsgReader;
	TBool						iChannelCreated;
	TBool						iRequestCancelled;

	CTestRequestChannel* 	iHandlerChannel;
	CTestRequestChannel* 	iProxyChannel;
	
	TUint iLastUsedRequestId;
	};



#endif // __LBS_TEST_PRIVACY_CONTROLLER_H__
