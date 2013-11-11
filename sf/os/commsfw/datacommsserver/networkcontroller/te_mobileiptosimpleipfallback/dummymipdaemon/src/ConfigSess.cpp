// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a Session of a Symbian OS server for the test 
// configuration daemon.
// 
//

/**
 @file
 @internalComponent
*/


#include "ConfigSess.h"
#include "ConfigServer.h"
#include "Config_Std.h"
#include <comms-infras/rconfigdaemonmess.h>


CConfigSession::CConfigSession():iProgressNotificationWasSent(EFalse)
/**
 * C++ Constructor.
 *
 * @internalComponent
 */
{	
}

CConfigSession::~CConfigSession()
/**
 * Destructor, closes the server.
 *
 * @internalComponent
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::~CConfigSession Start"));
	
	ConfigServer()->Close(this);

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::~CConfigSession End"));
	}

void CConfigSession::ServiceL(
	const RMessage2& aMessage)
/**
 * Called when a message is received from the interface.
 *
 * @internalComponent
 *
 * @param aMessageMessage received from the interface.
 * @leave Leaves from any of the called methods.
 */
	{
	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL Start"));

	// These must be defined here, otherwise ARM compiler fails.
	TDaemonProgress progressInfo;
	progressInfo.iStage = KConfigDaemonStartingRegistration + 13;;
	progressInfo.iError = KErrNone;		
	TPckg<TDaemonProgress> progressPkg(progressInfo);
	
	switch (aMessage.Function())
		{
	case EConfigDaemonConfigure:
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonConfigure requested. Returning KErrCouldNotConnect[-34]"));
		CompleteClientMessage(aMessage, KErrCouldNotConnect);
		break;
		
    case EConfigDaemonDeregister:
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonDeregister requested. Returning KErrNone[0]"));
		CompleteClientMessage(aMessage, KErrNone);
    	break;
    
    case EConfigDaemonProgress:
    	// This progress notification is sent to the client to verify that the DummyMipDaemon has loaded and run.
    	// The notification code itself is irrelevant.
    	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonProgress requested. Sending progress[8313, 0] "));
		if(!iProgressNotificationWasSent)
			{
			iProgressNotificationWasSent = ETrue;
			aMessage.WriteL(0, progressPkg);
		    aMessage.Complete(KErrNone);
			}
		
	   	break;
	   	
	case EConfigDaemonIoctl: 
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonIoctl requested. Returning KErrNotSupported[-5]"));
		CompleteClientMessage(aMessage, KErrNotSupported);
		break;
    case EConfigDaemonCancel: 
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonCancel requested. Returning KErrNone[0]"));
    	CompleteClientMessage(aMessage, KErrNone);
        break;
    case EConfigDaemonCancelMask: 
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: EConfigDaemonCancelMask requested. Returning KErrNone[0]"));
		CompleteClientMessage(aMessage, KErrNone);
        break;
	default:
		__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL: ERROR: Unsupported operation requested. Leaving with KErrNotSupported[-5]"));

		User::Leave(KErrNotSupported);
		break;
		}

	__FLOG_STATIC(KConfigLoggingTag1(), KConfigLoggingTag2(), _L("CConfigSession::ServiceL End"));
	}

/**
 * Completes client message
 *
 * @internalComponent
 *
 * @param aMessageMessage to be completed.
 * @param aErrorCode error code used to complete the message.
 * @post message is completed.
 */
void CConfigSession::CompleteClientMessage(
	const RMessage2& aMessage,
	const TInt aErrorCode)
	{
	if (!aMessage.IsNull())
		{
		aMessage.Complete(aErrorCode);	
		}	
	}

