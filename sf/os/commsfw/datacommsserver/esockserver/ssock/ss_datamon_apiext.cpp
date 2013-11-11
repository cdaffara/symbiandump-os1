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
*/

#include "ss_datamon_apiext.h"
#include <comms-infras/datamonmessagesecom.h>
#include <elements/responsemsg.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksdtmnp, "ESockSSocksdtmnp");
#endif

using namespace ESock;
using namespace Elements;

EXPORT_C void CDataMonitoringResponder::DataTransferred(CDataMonitoringResponder*& aThis, TUint32 aReceivedBytes, TUint32 aSentBytes)
	{
	if (aThis==NULL)
		return;

	aThis->DoDataTransferred(aReceivedBytes, aSentBytes);
	delete aThis;
	aThis = NULL;
	}
	
EXPORT_C void CDataMonitoringResponder::DataSentNotification(CDataMonitoringResponder*& aThis, TUint32 aSentBytes)
	{
	if (aThis==NULL)
		return;

	aThis->DoDataSentNotification(aSentBytes);
	delete aThis;
	aThis = NULL;
	}
	
EXPORT_C void CDataMonitoringResponder::DataReceivedNotification(CDataMonitoringResponder*& aThis, TUint32 aReceivedBytes)
	{
	if (aThis==NULL)
		return;

	aThis->DoDataReceivedNotification(aReceivedBytes);
	delete aThis;
	aThis = NULL;
	}

EXPORT_C void CDataMonitoringResponder::CancelRequest(CDataMonitoringResponder*& aThis)
	{
	if (aThis==NULL)
		return;

	aThis->DoCancelRequest();
	delete aThis;
	aThis = NULL;
	}

EXPORT_C void CDataMonitoringResponder::Error(CDataMonitoringResponder*& aThis, TInt aError)
	{
	if (aThis==NULL)
		return;

	aThis->DoComplete(aError);
	delete aThis;
	aThis = NULL;
	}

/**
	Completes the client's request for the amount of data transferred.
	
	@param aReceivedBytes The number of bytes received for communication to the client.	
	@param aSentBytes The number of bytes sent for communication to the client.
*/
void CDataMonitoringResponder::DoDataTransferred(TUint32 aReceivedBytes, TUint32 aSentBytes)
	{
	__ASSERT_DEBUG(iMsg == NULL, User::Panic(KSpecAssert_ESockSSocksdtmnp, 1));
	TRAP_IGNORE(iMsg = CDataTransferred::NewL());

	if(iMsg == NULL)
		{
		iResponseMsg.Complete(KErrNoMemory);
		return;
		}

	CDataTransferred& msg = static_cast<CDataTransferred&>(*iMsg);	
	
	msg.SetReceivedBytes(aReceivedBytes);
	msg.SetSentBytes(aSentBytes);

	iResponseMsg.Complete(msg);
	}
	
/**
	Completes the client's notification request for a data sent threshold.
	
	@param aSentBytes The number of bytes sent for communication to the client.
*/
void CDataMonitoringResponder::DoDataSentNotification(TUint32 aSentBytes)
	{	
	__ASSERT_DEBUG(iMsg == NULL, User::Panic(KSpecAssert_ESockSSocksdtmnp, 2));
	TRAP_IGNORE(iMsg = CDataSent::NewL());

	if(iMsg == NULL)
		{
		iResponseMsg.Complete(KErrNoMemory);
		return;
		}

	CDataSent& msg = static_cast<CDataSent&>(*iMsg);
	msg.SetSentBytes(aSentBytes);

	iResponseMsg.Complete(msg);
	}

/**
	Completes the client's notification request for a data received threshold.
	
	@param aReceivedBytes The number of bytes received for communication to the client.
*/
void CDataMonitoringResponder::DoDataReceivedNotification(TUint32 aReceivedBytes)
	{
	__ASSERT_DEBUG(iMsg == NULL, User::Panic(KSpecAssert_ESockSSocksdtmnp, 3));
	TRAP_IGNORE(iMsg = CDataReceived::NewL());

	if(iMsg == NULL)
		{
		iResponseMsg.Complete(KErrNoMemory);
		return;
		}
				
	CDataReceived& msg = static_cast<CDataReceived&>(*iMsg);
	msg.SetReceivedBytes(aReceivedBytes);

	iResponseMsg.Complete(msg);
	}

/**
	Cancels the outstanding response request.
*/
void CDataMonitoringResponder::DoCancelRequest()
	{
	iResponseMsg.Complete(KErrCancel);
	}

EXPORT_C CDataMonitoringResponder* CDataMonitoringResponder::NewL(Elements::RResponseMsg& aResponseMsg)
	{
	return new(ELeave) CDataMonitoringResponder(aResponseMsg);
	}

CDataMonitoringResponder::CDataMonitoringResponder(Elements::RResponseMsg& aResponseMsg)
	: CCommsApiExtIpcResponder(aResponseMsg)
	{
	}

CDataMonitoringResponder::~CDataMonitoringResponder()
	{
	delete iMsg;
	}


