// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @test
*/


#ifndef LBS_PRIVFW_TESTCHANNEL_MSGTYPES_H
#define LBS_PRIVFW_TESTCHANNEL_MSGTYPES_H

#include <lbscommon.h>
#include <lbssatellite.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourcegpsbase.h>

#include <lbs/epos_cposrequestor.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/epos_privacy.h>
#include <lbs/test/tlbsprivfwtestrequestinfo.h>
#include <lbs/test/lbsprivfwdefs.h>

//
// Message classes between:
//    - the Network Request Handler and Network Gateway
//    - the Network Request Handler and AGPS Manager
//

const TInt KMaximumMessageSizeBytes = 1576;


/** Base class for messages sent between channels.

This class defines the types of messages and reserves a buffer to store
information, but it can't be used itself. Instead derived classes will
implement each of the types of messages and supply accessor functions.

See the prototype of a location request message below.
*/
class TLbsTestChannelMsgBase
	{
public:
	/** List of all possible message types.
	
	Each type of message listed here must be implemented as a
	separate class that inherits from TLbsTestChannelMsgBase.
	*/
	enum TLbsTestChannelMsgType
		{
		EMessageUnknown  = 0,
		EPrivacyRequest,
		EMessageNotifyRequest,
		EMessageVerifyRequest,
		EMessageVerifyCancelRequest,
		EMessageNotifyResponse,
		EMessageVerifyResponse,
		EPrivacyMsgParamInfo,
		EPrivacyMsgCancel,
		EPrivacyMsgSessionComplete,
		EPrivacyMsgTimeoutExpired,
		EMessageCtrlCloseDown,  //To inform controller closedown
		EMessageNull
		};

public:
	IMPORT_C TLbsTestChannelMsgBase();
	IMPORT_C TLbsTestChannelMsgBase(const TLbsTestChannelMsgBase& aSrc);
	
	IMPORT_C TLbsTestChannelMsgType Type() const;
	IMPORT_C TUint RequestId() const;
	IMPORT_C TInt32 Size() const;
	
	
	IMPORT_C TLbsTestChannelMsgBase& operator = (const TLbsTestChannelMsgBase& aSrc);

protected:
	// Derived classes set their type by using these constructors.
	TLbsTestChannelMsgBase(TLbsTestChannelMsgType aType,
						   TInt aSizeInBytes);	
	TLbsTestChannelMsgBase(TLbsTestChannelMsgType aType, 
						   TInt aSizeInBytes,
						   TUint aRequestId);

private:
	// All messages have a type, size and session identifier
	TLbsTestChannelMsgType iType;
	TInt32 	iSize;
	TUint 	iRequestId;
	
	};
	
	
	
//Message classes between proxy and handler

NONSHARABLE_CLASS(TLbsPrivFwNotifyRequestMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwNotifyRequestMsg(TUint& aRequestId);
	
	TLbsTestRequestInfo		iTestRequestInfo;	
										 
	
private:
	TLbsPrivFwNotifyRequestMsg();
	};
	

NONSHARABLE_CLASS(TLbsPrivFwVerifyRequestMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwVerifyRequestMsg(TUint& aRequestId);
	
	TLbsTestRequestInfo		iTestRequestInfo;										 
	
private:
	TLbsPrivFwVerifyRequestMsg();
	};
	

NONSHARABLE_CLASS(TLbsPrivFwVerifyResponseMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwVerifyResponseMsg(TUint& aRequestId);
	
	TResponseType	iResponse;
	TTimeIntervalMicroSeconds	iResponseDelay;
	
private:
	TLbsPrivFwVerifyResponseMsg();
	};


NONSHARABLE_CLASS(TLbsPrivFwNotifyResponseMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwNotifyResponseMsg(TUint& aRequestId);
	
	TResponseType	iResponse;
	TTimeIntervalMicroSeconds	iResponseDelay;

private:
	TLbsPrivFwNotifyResponseMsg();
	};


NONSHARABLE_CLASS(TLbsPrivFwSessionCompleteMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwSessionCompleteMsg(TUint& aRequestId);
	
	TInt	iCompleteReason;
private:
	TLbsPrivFwSessionCompleteMsg();
	};


NONSHARABLE_CLASS(TLbsPrivFwCtrlCloseDownMsg) : public TLbsTestChannelMsgBase
	{
public:
	IMPORT_C TLbsPrivFwCtrlCloseDownMsg();
	
	};

#endif // LBS_PRIVFW_TESTCHANNEL_MSGTYPES_H
