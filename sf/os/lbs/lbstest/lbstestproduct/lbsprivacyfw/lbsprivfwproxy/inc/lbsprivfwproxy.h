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
 @file lbsprivfwproxy.h
 @internalTechnology
 @test
*/
#ifndef __LBS_PRIVFW_PROXY_H__
#define __LBS_PRIVFW_PROXY_H__

#include <lbs/test/lbsprivfwdefs.h>

#include <lbs/lbsloccommon.h>

#include <lbs/epos_privacy.h>
#include <lbs/epos_cposrequestor.h>

#include <lbs/test/tlbstestrequestchannel.h>

// For Controller
#include <lbs/lbsprivacycontroller.h>

	
class TLbsNotifierTestData
	{
	public:
		// Getters

	
		// Setters
	
	private:
		TInt iDummy;
	};

	
class MPrivFwProxyObserver
	{
	public:
		virtual void NotifierDataAvailable(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData) = 0;
		virtual void NotifierRequestComplete(TInt aRequestId, TInt aCompletionCode) = 0;
    	virtual void NotiferCtrlCloseDown() = 0;
	};


class CPrivFwProxy : public CBase, public MLbsTestChannelRequestObserver
	{
public:
	IMPORT_C static CPrivFwProxy* NewL(TPrivacyHandlerType aPrivHandlerType, MPrivFwProxyObserver& aObserver);
	~CPrivFwProxy();

	IMPORT_C void SendNotifierResponse(TRequestType aRequestType, TInt aRequestId, TResponseType aResponse, TTimeIntervalMicroSeconds& aResponseDelay);
	IMPORT_C void SendNotifierTestData(TInt aRequestId, const TLbsNotifierTestData& aPrivacyTestData);
	
private:
	CPrivFwProxy(TPrivacyHandlerType aPrivHandlerType, MPrivFwProxyObserver& aObserver);
	void ConstructL();

 //	TInt ReadPipe(TInt aTimeout);
//	TInt GetArgsLC(TInt aDummy, ...);
	
//	TInt GetEPrivacyMsgQnNParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList);
//	TInt GetEPrivacyMsgNotifierParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList);
//	TInt GetEPrivacyMsgCtrlParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList);

	// Functions to convert QnN requestinfo elements to TLbsExternalRequestInfo elements
	TLbsExternalRequestInfo::TFormatIndicator ConvertTRequestorIdFormat(
			CPosRequestor::TRequestorIdFormat aRequestorIdFormat);
	TLbsExternalRequestInfo::TNetworkType ConvertTNetworkType(CPosRequestor::TNetworkType aNetworkType);
	TLbsExternalRequestInfo::TRequestType ConvertTRequestType(CPosRequestor::TRequestType aRequestType);
//	void ConvertToTestRequestInfo(const TLbsTestRequestInfo* aSrcRequestInfo, TLbsTestRequestInfo* aDestRequestInfo);


	//From MPrivFwReadPropPipeObserver
	//virtual void OnReadyToReadL(CPrivFwReadPropPipe& aPipe);
	
	//From MLbsPrivFwChanelObserver
	//virtual void ProcessPrivFwChannelMessage(RLbsPrivFwChannel::TLbsPrivFwChannelId aChannelId, const TLbsPrivFwMsgBase& aMessage);
	
	//From MLbsTestChannelRequestObserver
	virtual void ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage);
	
private:

private:
	/** Notifier access. */
//	CPrivFwWritePropPipe* iActionWriter;
//	CPrivFwNotifyReadPropPipe* 	iResponseReader;
	 	
	
//	TPrivFwMsgType 	iLastResponse;
	TPrivacyHandlerType iPrivHandlerType;
	TLbsTestChannelMsgBase::TLbsTestChannelMsgType iMsgType;
	TInt iRequestId;

	CTestRequestChannel* 	iHandlerChannel;
	CTestRequestChannel* 	iProxyChannel;
//	TBool					iChannelCreated;

	/** Observer to notify when data has been deliveried by the privacy notifier.
	*/
	MPrivFwProxyObserver& iObserver;
	};



#endif // __LBS_PRIVFW_PROXY_H__
