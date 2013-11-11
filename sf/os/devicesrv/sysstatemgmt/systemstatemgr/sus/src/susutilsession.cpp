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
//

#include <ssm/ssmsuscli.h>

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
#include <ssm/ssmpatchableconstants.h>
#include <e32uid.h>
#include <e32property.h>
#endif	// SYMBIAN_SSM_GRACEFUL_SHUTDOWN

#include "susutilserver.h"
#include "susutilsession.h"
#include "susutilclisrv.h"
#include "ssmdebug.h"

CSusUtilSession::~CSusUtilSession()
	{
	}

CSusUtilSession::CSusUtilSession()
	{
	}

CSusUtilSession* CSusUtilSession::NewL()
	{	
	CSusUtilSession* session = new(ELeave) CSusUtilSession();
	return session;
	}

void CSusUtilSession::ServiceL(const RMessage2& aMessage)
	{
    const TBool allowed = (aMessage.SecureId() == UtilServer()->ClientSecureId());
    SSMLOGLEAVEIFFALSE( allowed, KErrPermissionDenied);

	switch (aMessage.Function())
		{
		case ERequestLoadSup:
			{
			DEBUGPRINT1(_L ("CSusUtilSession received: ERequestLoadSup "));
			
			if (sizeof(TSsmSupInfo) != aMessage.GetDesLengthL(0))
				{
				//Leave with KErrArgument if client passes other than TSsmSupInfo
				DEBUGPRINT2(_L ("size of TSsmSupInfo - %d "),sizeof(TSsmSupInfo));
				User::Leave(KErrArgument);
				}
			
			TSsmSupInfo pluginInfo;
			TPckg<TSsmSupInfo> pckg(pluginInfo);
			aMessage.ReadL(0, pckg);
			
			UtilServer()->LoadUtilityPluginL(pluginInfo);
			aMessage.Complete(KErrNone);
			break;
			}
		case ERequestLoadSupCancel:
			{
			DEBUGPRINT1(_L ("CSusUtilSession received: ERequestLoadSupCancel "));
		 	//Current design will never have any pending async operations
			aMessage.Complete(KErrNone);
			break;	
			}
		case ERequestUnLoadSup:
			{
			DEBUGPRINT1(_L ("CSusUtilSession received: ERequestUnLoadSup "));

			if (sizeof(TSsmSupInfo) != aMessage.GetDesLengthL(0))
				{
				//Leave with KErrArgument if client passes other than TSsmSupInfo
				User::Leave(KErrArgument);
				}

			TSsmSupInfo pluginInfo;
			TPckg<TSsmSupInfo> pckg(pluginInfo);
			aMessage.ReadL(0, pckg);

		 	const TInt index = UtilServer()->UnLoadUtilityPluginL(pluginInfo);
		 	aMessage.Complete( (index>KErrNotFound) ? KErrNone : KErrNotFound );
			break;	
			}
#ifdef _DEBUG
		case (EEndOfSusUtilOpCodes):
			{
			DEBUGPRINT1(_L ("CSusUtilSession received: EEndOfSusUtilOpCodes, shutting down "));
			aMessage.Complete (KErrNone);
			CActiveScheduler::Stop();
			break;
			}
#endif //_DEBUG
		//Only for testing purpose
		case EDebugMarkHeap:
#ifdef _DEBUG
			DEBUGPRINT1(_L ("CSusUtilSession received EDebugMarkHeap(start Heap mark)"));
			__UHEAP_MARK;
#endif
			aMessage.Complete(KErrNone);
			break;
		//Only for testing purpose
		case EDebugMarkHeapEnd:
#ifdef _DEBUG
			DEBUGPRINT1(_L ("CSusUtilSession received EDebugMarkHeapEnd(end Heap mark)"));
			__UHEAP_MARKEND;
#endif
			aMessage.Complete(KErrNone);
			break;
		//Only for testing purpose
		case EDebugCompressPluginArray:
#ifdef _DEBUG
			UtilServer()->CompressPluginArray();
#endif
			aMessage.Complete(KErrNone);
			break;
		default:
			//Should never come here if policy-server configuraion is correct in susutilserver.cpp
			DEBUGPRINT2(_L("CSusUtilSession received unsupported request: %d "), aMessage.Function());
			aMessage.Complete (KErrNotSupported);
			break;
		}
	}

void CSusUtilSession::ServiceError(const RMessage2 &aMessage, TInt aError)
	{
	DEBUGPRINT2(_L ("CSusUtilSession: ServiceError %d"), aError);
	aMessage.Complete (aError);
	}

/**
 Returns a pointer to the server object
 */
CSusUtilServer* CSusUtilSession::UtilServer()
	{
	return static_cast<CSusUtilServer*> (const_cast<CServer2*> (Server()));
	}
