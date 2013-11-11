//mmrcclient.cpp

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

#include "mmrcclient.h"
#include "mmrcclientimplementation.h"
#include "audiocontext.h"
#include <a3f/a3f_trace_utils.h>

/**
 * 
 */
EXPORT_C RMMRCClient::RMMRCClient():
		iImplementation(NULL)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> RMMRCClient::RMMRCClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 * 
 */
EXPORT_C RMMRCClient::~RMMRCClient()
	{
	DP_CONTEXT(----> RMMRCClient::~RMMRCClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	delete iImplementation;
	iImplementation = NULL;
	DP_OUT();
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::Open(MMultimediaResourceControlObserver& aCallback)
	{
	DP_CONTEXT(----> RMMRCClient::Open *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	if(iImplementation)
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!RMMRCClient::Open -  iImplementation already created"));
		#endif
		DP0_RET(KErrCompletion, "error = %d");
		}
		
	TRAP( err, iImplementation = CMMRCClientImplementation::NewL(aCallback) );
	if(err)
		{
		Close();
		}
	DP0_RET(err, "error = %d");
	}

/**
 * 
 */
EXPORT_C void RMMRCClient::Close()
	{
	DP_CONTEXT(----> RMMRCClient::Close *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	#ifdef _DEBUG	
	if(	NULL == iImplementation )
		{
		RDebug::Print(_L("!!!!RMMRCClient::Close -  iImplementation NULL"));
		}
	#endif
	
	if(iImplementation)
		{
		iImplementation->Close();
		delete iImplementation;
		iImplementation = NULL;
		}
	
	DP_OUT();
	}

/**
 * 
 */
EXPORT_C TUint64 RMMRCClient::LogOn(TProcessId aProcessId)
	{
	DP_CONTEXT(----> RMMRCClient::LogOn *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	#ifdef _DEBUG	
	if(	NULL == iImplementation )
		{
		RDebug::Print(_L("!!!!RMMRCClient::LogOn -  iImplementation NULL"));
		}
	#endif

	TUint64 contextId = 0;
	if(iImplementation)
		{
		contextId = iImplementation->LogOn(aProcessId);
		}
	
	DP0_RET(contextId, "contextId = %d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::SendResourceRequest(MLogicalChain* aLogicalChainLastCommited, MLogicalChain* aLogicalChainRequested, CAudioContext* aContext)
	{
	DP_CONTEXT(----> RMMRCClient::SendResourceRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(	NULL == iImplementation )
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!RMMRCClient::SendResourceRequest -  iImplementation NULL"));
		#endif
		DP0_RET(KErrCompletion, "error = %d");
		}
	
	TInt error = iImplementation->SendResourceRequest(aLogicalChainLastCommited, aLogicalChainRequested, aContext);

	DP0_RET(error, "error = %d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::SendResourceConfirmation()
	{
	DP_CONTEXT(----> RMMRCClient::SendResourceConfirmation *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNotSupported;
	DP0_RET(err, "%d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::SendResourceUpdateResult()
	{
	DP_CONTEXT(----> RMMRCClient::SendResourceUpdateResult *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNotSupported;
	DP0_RET(err, "%d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::SendResourceErrorNotification()
	{
	DP_CONTEXT(----> RMMRCClient::SendResourceErrorNotification *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_IN();
	TInt err = KErrNotSupported;
	DP0_RET(err, "%d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(RMMRCClient::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
		if(	NULL == iImplementation )
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!RMMRCClient::RegisterAsClient -  iImplementation NULL"));
		#endif
		DP0_RET(KErrCompletion, "error = %d");
		}
	
	TInt err = iImplementation->RegisterAsClient(aEventType, aNotificationRegistrationData);
	
	DP0_RET(err, "%d");
	}

/**
 * 
 */
EXPORT_C TInt RMMRCClient::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(RMMRCClient::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
		if(	NULL == iImplementation )
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!RMMRCClient::CancelRegisterAsClient -  iImplementation NULL"));
		#endif
		DP0_RET(KErrCompletion, "error = %d");
		}
	
	TInt err = iImplementation->CancelRegisterAsClient(aEventType);
	
	DP0_RET(err, "%d");
	}
	
/**
 * 
 */
EXPORT_C TInt RMMRCClient::WillResumePlay()
	{
	DP_CONTEXT(RMMRCClient::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
		if(	NULL == iImplementation )
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!RMMRCClient::WillResumePlay -  iImplementation NULL"));
		#endif
		DP0_RET(KErrCompletion, "error = %d");
		}
	
	TInt err = iImplementation->WillResumePlay();
	
	DP0_RET(err, "%d");
	}
	
EXPORT_C void RMMRCClient::ResetMessages()
	{
	DP_CONTEXT(RMMRCClient::ResetMessages *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iImplementation->ResetMessages();
	
	DP_OUT();
	}
//EOF
