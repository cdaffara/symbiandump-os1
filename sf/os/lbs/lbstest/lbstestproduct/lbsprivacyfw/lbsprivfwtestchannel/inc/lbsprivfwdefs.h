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
 @file lbsprivfwdefs.h
 @internalTechnology
 @test
*/
#ifndef __LBS_PRIVFW_PROXY_DEFS_H__
#define __LBS_PRIVFW_PROXY_DEFS_H__

#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>

const TUint32 KSecond	= 1000 * 1000;


const TUint32 KDefaultNetworkPrivacyTimeoutInSeconds = 30;

const TUint32 KProxyToStubPipeKey = 0x07897655;
const TUint32 KProxyToStubFlowKey = 0x07897656;
const TUint32 KProxyToStubWriteTimeout = 60 * KSecond;

const TUint32 KStubToProxyPipeKey = 0x07897657;
const TUint32 KStubToProxyFlowKey = 0x07897658;
const TUint32 KStubToProxyWriteTimeout = 60 * KSecond;


enum TPosNetworkPrivacyRequestInfoType
	{
	ERequestInfoTypeGsm,
	ERequestInfoTypeSupl
	};

enum TPrivacyHandlerType
	{
	EPrivacyHandlerUnknown		= 0x00,
	EPrivacyHandleByNotifier	= 0x01,
	EPrivacyHandleByController	= 0x02,
	EPrivacyHandleByQnN	= 0x03
	};

enum TRequestType
	{
	ERequestUnknown		= 0x00,
	ERequestNotify	= 0x01,
	ERequestVerify = 0x02,
	ERequestVerifyCancel = 0x03,
	ERequestNotifyCancel = 0x04
	};

enum TResponseType
	{
	EResponseTypeUnknown = 0,
	EResponseTypeAccepted,
	EResponseTypeRejected,
	EResponseTypeTimedOut,
	// A few random unexpected responses
	EResponseTypeNoMemory,
	EResponseTypeNotfound,
	// A few notifier/controller control params to fully exercide the notifier side API
	// If appropriate, these will complete the notifier/controller with an Accept
	EResponseTypeCompleteAllRequests, // calls the notifier CompleteAllRequests method
	EResponseTypeIgnore			//Used for Privacy Controller
	};

	enum TMessageType
	{
	EMessageUnknown  = 0,
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
	//	request = request info sent to the notifier
	//	response = expected response the notifier should return
	};	

#endif // __LBS_PRIVFW_PROXY_DEFS_H__
