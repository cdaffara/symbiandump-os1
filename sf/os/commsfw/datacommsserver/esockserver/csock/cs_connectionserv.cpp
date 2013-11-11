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
 @publishedPartner
 @released since 399
*/

#include <ecom/ecom.h>
#include <es_sock.h>
#include <comms-infras/es_connectionserv.h>
#include "cs_connectionservimpl.h"
#include <comms-infras/es_connectionservermessages.h>
#include <connpref.h>
#include <comms-infras/ss_log.h>
#include <rsshared.h>
#include <es_ver.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockcscnct, "ESockCSockcscnct");
#endif


using namespace ConnectionServ;


_LIT(KConnectionServerName, "!ConnectionServer");

EXPORT_C RConnectionServ::RConnectionServ() :
	iConnectionServImpl(0)
/**
 Empty ctor
 */
	{
	LOG( ESockLog::Printf(_L8("RConnectionServ %08x:\tRConnectionServ() tid %d"), this, (TUint)RThread().Id()); );
	}


EXPORT_C TInt RConnectionServ::Connect(TUint aTierId)
/** Opens a session to the connection server.

RHandleBase::Close() should be called once the session is no longer required.
All resources which are opened using the session will be automatically closed
when the session terminates.

@return KErrNone if successful, otherwise another of the system-wide error
codes. */
	{
	LOG( ESockLog::Printf(_L8("RConnectionServ %08x:\tConnect() tid %d tier %d"), this, (TUint)RThread().Id(), aTierId ));
#ifndef __ESOCK_SUPPRESS_ESOCK_HANDLE_OVERWRITE_PANICS
	__ASSERT_DEBUG(Handle() == 0, User::Panic(KSpecAssert_ESockCSockcscnct, 1));
	__ASSERT_DEBUG(iConnectionServImpl == 0, User::Panic(KSpecAssert_ESockCSockcscnct, 2));
#endif

	if(Handle())
		{
		return KErrInUse;
		}

	TVersion ver(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
	TInt result = CreateSession(KConnectionServerName,ver,8,EIpcSession_Sharable);

	if (result==KErrNotFound)
		{
		result=StartC32Full();
		if (result==KErrNone || result==KErrAlreadyExists)
			{
			const TUint numberOfConcurrentRequests = 8;

			// If this connection attempt also fails then most likely the connection server is
			// not enabled in the current configuration.
			result=CreateSession(KConnectionServerName,ver,numberOfConcurrentRequests,EIpcSession_Sharable);
			}
		}

	if(result != KErrNone)
		{
		this->Close();
		return result;
		}

	__ASSERT_DEBUG(Handle(), User::Panic(KSpecAssert_ESockCSockcscnct, 3));

    result = SendReceive(ECMAttachToTierManager, TIpcArgs(aTierId));
	
	if(result != KErrNone)
		{
		this->Close();
		return result;
		}

	if( iConnectionServImpl == 0 )
		{
		TRAP(result, iConnectionServImpl = CConnectionServImpl::NewL(*this));
		if (result != KErrNone)
    		{
    		this->Close();
    		}
		}

	return result;
	}


EXPORT_C void RConnectionServ::Close()
/**
 Closes the comms manager sub-session

 Delete owned object and drop to base class
 */
	{
	LOG( ESockLog::Printf(_L8("RConnectionServ%08x:\tClose() tid %d"), this, (TUint)RThread().Id()); );
	delete iConnectionServImpl; // this causes any requests to be cancelled
	iConnectionServImpl = 0;
	RSessionBase::Close();
	
	// The API uses ECom types.. This is the last time we get to attempt a cleanup.
	// This call does no harm if the types are still in use (e.g. stored by client code).
	REComSession::FinalClose();
	}


// Asynchronous multiple-IPC functions. Proxy them through to the active objects which handle them, via the impl object.

EXPORT_C void RConnectionServ::AccessPointStatusQuery(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iConnectionServImpl != NULL, User::Panic(KConnectionServerName, KErrBadHandle));
	iConnectionServImpl->AccessPointStatusQuery(aQuery,aResult,aStatus);
	}

EXPORT_C void RConnectionServ::CancelAccessPointStatusQuery()
	{
	__ASSERT_ALWAYS(iConnectionServImpl != NULL, User::Panic(KConnectionServerName, KErrBadHandle));
	iConnectionServImpl->CancelAccessPointStatusQuery();
	}

EXPORT_C void RConnectionServ::AccessPointNotification(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver)
	{
	__ASSERT_ALWAYS(iConnectionServImpl != NULL, User::Panic(KConnectionServerName, KErrBadHandle));
	iConnectionServImpl->AccessPointNotification(aQuery,aObserver);
	}

EXPORT_C void RConnectionServ::CancelAccessPointNotification()
	{
	__ASSERT_ALWAYS(iConnectionServImpl != NULL, User::Panic(KConnectionServerName, KErrBadHandle));
	iConnectionServImpl->CancelAccessPointNotification();
	}


