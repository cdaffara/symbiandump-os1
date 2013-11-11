// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <s32mem.h>
#include <lbs.h>
#include <lbserrors.h>
#include <lbsareainfo.h>

#include "lbslocservermessageenums.h"
#include "LbsPtrHolder.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "LbsPositionerTraces.h"
#endif


#ifdef _DEBUG
    #include <e32debug.h>
#endif
//-----------------------------------------------------------------------------
// RPositionServer
//-----------------------------------------------------------------------------

/**
RPositionServer::RPositionServer()
C++ default constructor can NOT contain any code, that might leave.
*/
EXPORT_C RPositionServer::RPositionServer()
  : RSessionBase(), 
    iPtrHolder(NULL),
    iReserved(NULL)
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_RPOSITIONSERVER_ENTRY, this );
	OstTraceFunctionExit1( RPOSITIONSERVER_RPOSITIONSERVER_EXIT, this );
	}

/**
RPositionServer::ConstructL
Symbian 2nd phase constructor can leave.
*/
void RPositionServer::ConstructL()
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_CONSTRUCTL_ENTRY, this );
	__ASSERT_ALWAYS(iPtrHolder == NULL, User::Panic(KPosClientFault, EPositionServerHandleNotClosed));

	iPtrHolder = CServerPositioningPtrHolder::NewL(1, 1);
	OstTraceFunctionExit1( RPOSITIONSERVER_CONSTRUCTL_EXIT, this );
	}

/**
RPositionServer::Destruct
Destruction method for RPositionServer. Just deletes the ptr-holder.
*/
void RPositionServer::Destruct()
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_DESTRUCT_ENTRY, this );
	delete iPtrHolder;
	iPtrHolder = NULL;
	OstTraceFunctionExit1( RPOSITIONSERVER_DESTRUCT_EXIT, this );
	}

/**
Creates a session with the positioning server. If there are no other
sessions when this is called, the method will start the server.
This method must be called before an attempt to obtain module 
information or on opening a sub-session.

@return a Symbian OS error code.
@panic "Lbs Client Fault" 5 If this method is called when a connection is already present. 
*/
EXPORT_C TInt RPositionServer::Connect()
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_CONNECT_ENTRY, this );
	TRAPD(ret, ConstructL());

	if (ret == KErrNone)
		{
		const TInt KNumAttempts = 200;
		TInt retry = KNumAttempts;
		for (;;) // FOREVER
			{
			ret = CreateSession(KPositionServerName, Version(), KLbsDefaultMessageSlots);

			if (ret != KErrNotFound && ret != KErrServerTerminated)
				{
				break;
				}

			if (--retry == 0)
				{
				break;
				}

			ret = StartServer();

			if (ret != KErrAlreadyExists && ret != KErrNone)
				{
				break;
				}
			}
		}

	if (ret != KErrNone)
		{
		Destruct();
		}

	OstTraceFunctionExitExt( RPOSITIONSERVER_CONNECT_EXIT, this, ret );
	return ret;
	}

/**
Closes a session with the positioning server.Before the connection to 
Location Server is closed, the client application must ensure that any 
outstanding notification requests have been cancelled. In particular, 
applications must issue all appropriate cancel requests and then wait 
for a confirmation that the notifications have been terminated. A 
failure to do so results in a client side panic.

@panic "Lbs Client Fault" 16 If the outstanding notification requests 
are not cancelled before calling this method.
*/
EXPORT_C void RPositionServer::Close()
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_CLOSE_ENTRY, this );
	RSessionBase::Close();
	Destruct();
	OstTraceFunctionExit1( RPOSITIONSERVER_CLOSE_EXIT, this );
	}

/**
Cancels a previously issued asynchronous request. The TRequestStatus of the
original request will be set to KErrCancel if the cancellation request was
successful, or any other Symbian OS error code if CancelRequest was called
too late.

@param aRequestId is a number that specifies the request to be cancelled.
It should be set to EPositionServerNotifyModuleStatusEvent to cancel a 
call to RPositionServer::NotifyModuleStatusEvent()
@return a symbian OS error code.
@return KErrNotFound if there is no outstanding request for the 
specified request ID.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
*/
EXPORT_C TInt RPositionServer::CancelRequest(TRequestId aRequestId)
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_CANCELREQUEST_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

	if (aRequestId != KServerNotifyModuleStatusEventSymbian &&
		aRequestId != KServerNotifyModuleStatusEventVariant &&
		aRequestId != EReqNotifyModuleStatusEvent && 
		aRequestId != EReqEmptyLastKnownPositionStore
		)
		{
        OstTraceFunctionExitExt( RPOSITIONSERVER_CANCELREQUEST_EXIT, this, KErrNotSupported );
        return KErrNotSupported;
        }
	
	TIpcArgs args;
	
	switch (aRequestId)
		{
		case KServerNotifyModuleStatusEventSymbian:
		case KServerNotifyModuleStatusEventVariant:
			args.Set(0, aRequestId);
			break;
		case EReqNotifyModuleStatusEvent:
			args.Set(0, ELbsNotifyModuleStatusEvent);
			break;
		case EReqEmptyLastKnownPositionStore:
			args.Set(0, ELbsEmptyLastKnownPositionStore);
			break;
		}
    
    
	return SendReceive(ELbsServerCancelAsyncRequest, args);
	}


/**
Obtains the current version number of the location server.

@return the version of the client API.
*/
EXPORT_C TVersion RPositionServer::Version() const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_VERSION_ENTRY, this );
	return TVersion(
		KPosMajorVersionNumber,
		KPosMinorVersionNumber,
		KPosBuildVersionNumber);
	}

/**
This method obtains the identifier of the systems' default positioning
module. This module will be used if no criteria is specified when an
RPositioner sub-session is created.

@see RPositioner::Open()
@param[out] aModuleId will be set, upon successful completion, to the ID of the
module that the positioning server uses by default.
@return a Symbian OS error code.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
are not cancelled before calling close.

@pre before calling this method RPositionServer::Connect() should be called.
*/
EXPORT_C TInt RPositionServer::GetDefaultModuleId(TPositionModuleId& aModuleId) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_GETDEFAULTMODULEID_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

    TPckg<TPositionModuleId> moduleId(aModuleId);
	return SendReceive(ELbsGetDefaultModuleId, TIpcArgs(&moduleId));
	}

/**
This method obtains the current number of available positioning modules.

@param[out] aNumModules will be set, upon successful completion, to the number of
positioning modules (PSYs) that are currently available.
@return a Symbian OS error code.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  

@pre before calling this method RPositionServer::Connect() should be called.
*/
EXPORT_C TInt RPositionServer::GetNumModules(TUint& aNumModules) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_GETNUMMODULES_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

    TPckg<TUint> numModules(aNumModules);
	return SendReceive(ELbsGetNumModules, TIpcArgs(&numModules));
	}

/**
Return details about a specified module.
 
@param aModuleIndex a logical index (0...GetNumModules()-1) of a 
positioning module.
@param[out] aModuleInfo contains, on successful completion, the information
relating to the specified positioning module.
@return a Symbian OS error code.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
 
@pre before calling this method RPositionServer::Connect() should be called.
*/
EXPORT_C TInt RPositionServer::GetModuleInfoByIndex(
	TInt aModuleIndex,
	TPositionModuleInfoBase& aModuleInfo) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_GETMODULEINFOBYINDEX_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

	TPtr8 ptr(reinterpret_cast<TUint8*>(&aModuleInfo),
		aModuleInfo.PositionClassSize(), aModuleInfo.PositionClassSize());

    TIpcArgs args;
    args.Set(0, aModuleIndex);
    args.Set(1, &ptr);
	return SendReceive(ELbsGetModuleInfoByIndex, args);
	}

/**
Return details about a specified module.
 
@param aModuleId the unique identifier (UID) of a positioning module
@param[out] aModuleInfo contains, on successful completion, the information
relating to the specified positioning module.
@return a symbian OS error code.
@return KErrNotFound is returned if the specified moduleId is not valid.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
 
@pre before calling this method RPositionServer::Connect() should be called.
*/
EXPORT_C TInt RPositionServer::GetModuleInfoById(TPositionModuleId aModuleId,
                                                 TPositionModuleInfoBase& aModuleInfo) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_GETMODULEINFOBYID_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

    TPckg<TPositionModuleId> moduleId(aModuleId);

	TPtr8 ptr(reinterpret_cast<TUint8*>(&aModuleInfo),
		aModuleInfo.PositionClassSize(), aModuleInfo.PositionClassSize());

	return SendReceive(ELbsGetModuleInfoById, TIpcArgs(&moduleId, &ptr));
	}

/**
Obtains information about the specified positioning module.
 
@param[out] aPosModuleStatus contains, on successful completion, the status of
the specified positioning module
@param aModuleId the unique identifier (UID) of a positioning module
@return a symbian OS error code.
@return KErrNotFound is returned if the specified moduleId is not valid.
*/
EXPORT_C TInt RPositionServer::GetModuleStatus(TPositionModuleStatusBase& aPosModuleStatus,
                                               TPositionModuleId aModuleId) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_GETMODULESTATUS_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

    TPckg<TPositionModuleId> moduleId(aModuleId);

	TPtr8 ptr(reinterpret_cast<TUint8*>(&aPosModuleStatus),
		aPosModuleStatus.PositionClassSize(), aPosModuleStatus.PositionClassSize());
		
	return SendReceive(ELbsGetModuleStatus, TIpcArgs(&moduleId, &ptr));
	}

/**
This method reports status changes of either a single module or all
positioning modules. It is an asynchronous request and the client should
use it appropriately using active objects.
 
After a notification has been received, the client application must re-issue
the status change request if it wishes to obtain further updates.

To cancel this request use RPositionServer::CancelRequest() with 
EPositionServerNotifyModuleStatusEvent as parameter

@param[out] aStatusEvent contains, on successful completion, information regarding
the status event and the new status of of the specified positioning module.
@param[out] aStatus contains the result code after the asynchronous call completes.
It returns
- KErrNotFound if the moduleId specified does not exist
- KErrArgument is the requested event mask is zero
- KErrCancel if the notification is cancelled.
@param aModuleId the unique identifier (UID) of a positioning module. If 
the parameter is zero, then a status update is provided for all positioning 
modules.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
@panic "Lbs Client Fault" 15 If there is already is a pending notification for
module status event.
*/
EXPORT_C void RPositionServer::NotifyModuleStatusEvent(TPositionModuleStatusEventBase& aStatusEvent,
                                                       TRequestStatus& aStatus,
                                                       const TPositionModuleId aModuleId) const
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_NOTIFYMODULESTATUSEVENT_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Panic(KPosClientFault, EPositionNullPointerHolder));

	aStatusEvent.Reset();
	iPtrHolder->Ptr(0).Set(reinterpret_cast<TUint8*>(&aStatusEvent),
		aStatusEvent.PositionClassSize(), aStatusEvent.PositionClassSize());
	iPtrHolder->iPositionModuleId = aModuleId;
	iPtrHolder->PtrC(0).Set(reinterpret_cast<TUint8*>(&iPtrHolder->iPositionModuleId),
		sizeof(TPositionModuleId));

	SendReceive(ELbsNotifyModuleStatusEvent, 
        TIpcArgs(&iPtrHolder->Ptr(0), &iPtrHolder->PtrC(0)), aStatus);
	OstTraceFunctionExit1( RPOSITIONSERVER_NOTIFYMODULESTATUSEVENT_EXIT, this );
	}

/**
Deletes all the cached positions accessible via the RPositioner::GetLastKnownPosition 
and the RPositioner:: GetLastKnownPositionArea methods.
 
@param aStatus [Out] Returns the result code after the asynchronous call completes.
KErrNone if successful; any other system wide error code otherwise.
 
@see RPositioner::GetLastKnownPosition
@see RPositioner::GetLastKnownPositionArea
 
@prototype
@capability WriteUserData
*/
EXPORT_C void RPositionServer::EmptyLastKnownPositionStore(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry1( RPOSITIONSERVER_EMPTYLASTKNOWNPOSITIONSTORE_ENTRY, this );
	__ASSERT_ALWAYS(Handle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Panic(KPosClientFault, EPositionNullPointerHolder));
	
	return SendReceive(ELbsEmptyLastKnownPositionStore, TIpcArgs(),aStatus);
	
	}

//-----------------------------------------------------------------------------
// RPositionerSubSessionBase
//-----------------------------------------------------------------------------

/**
Constructor for RPositionerSubSessionBase
*/
EXPORT_C RPositionerSubSessionBase::RPositionerSubSessionBase()
  : RSubSessionBase(),
    iReserved(NULL)
    {
	OstTraceFunctionEntry1( RPOSITIONERSUBSESSIONBASE_RPOSITIONERSUBSESSIONBASE_ENTRY, this );
	OstTraceFunctionExit1( RPOSITIONERSUBSESSIONBASE_RPOSITIONERSUBSESSIONBASE_EXIT, this );
	}


/**
Cancels a previously issued asynchronous request. The TRequestStatus of the
original request will be set to KErrCancel if the cancellation request was
successful, or any other Symbian OS error code if CancelRequest was called
too late.
 
@param aRequestId is a number that specifies the request to be cancelled.
It should be set to 
- EPositionerNotifyPositionUpdate to cancel a call to
RPositioner::NotifyPositionUpdate().
- EPositionerGetLastKnownPosition to cancel a call to
RPositioner::GetLastKnownPosition()
@return KErrNotFound if there is no outstanding request for the 
specified request ID.
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
*/
EXPORT_C TInt RPositionerSubSessionBase::CancelRequest(TRequestId aRequestId)
	{
	OstTraceFunctionEntry1( RPOSITIONERSUBSESSIONBASE_CANCELREQUEST_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

	if (aRequestId != KGetLastKnownPositionSymbian &&
		aRequestId != KGetLastKnownPositionVariant &&
		aRequestId != KNotifyPositionUpdateSymbian &&
		aRequestId != KNotifyPositionUpdateVariant && 
		aRequestId != EReqGetLastKnownPosition &&
		aRequestId != EReqNotifyPositionUpdate &&
		aRequestId != EReqGetLastKnownPositionArea)
        {
        OstTraceFunctionExitExt( RPOSITIONERSUBSESSIONBASE_CANCELREQUEST_EXIT, this, KErrNotSupported );
        return KErrNotSupported;
        }

    TIpcArgs args;
 	
	switch (aRequestId)
		{
		case KGetLastKnownPositionSymbian:
		case KGetLastKnownPositionVariant:
		case KNotifyPositionUpdateSymbian:
		case KNotifyPositionUpdateVariant:
			args.Set(0, aRequestId);
			break;
		case EReqGetLastKnownPosition:
			args.Set(0, ELbsGetLastKnownPosition);
			break;
		case EReqNotifyPositionUpdate:
			args.Set(0, ELbsPosNotifyPositionUpdate);
			break;		
		case EReqGetLastKnownPositionArea:
			args.Set(0, ELbsGetLastKnownPositionArea);
			break;
		}
			
	return SendReceive(ELbsPositionerCancelAsyncRequest, args);
	}

/**
*/
EXPORT_C TInt RPositionerSubSessionBase::CompleteRequest(TInt /*aRequestId*/)
	{
	OstTraceFunctionEntry1( RPOSITIONERSUBSESSIONBASE_COMPLETEREQUEST_ENTRY, this );
	OstTraceFunctionExitExt( RPOSITIONERSUBSESSIONBASE_COMPLETEREQUEST_EXIT, this, KErrNotSupported );
	return KErrNotSupported;
	}

/**
This method is used to allow polymorphic extensions to the API without breaking
BC. See documentation for explanation.

@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data
@param aPtr2 a pointer to any data.
*/
EXPORT_C TAny* RPositionerSubSessionBase::ExtendedInterface(TInt /*aFunctionNumber*/,
                                                            TAny* /*aPtr1*/,
                                                            TAny* /*aPtr2*/)
	{
	OstTraceFunctionEntry1( RPOSITIONERSUBSESSIONBASE_EXTENDEDINTERFACE_ENTRY, this );
	OstTraceFunctionExitExt( RPOSITIONERSUBSESSIONBASE_EXTENDEDINTERFACE_EXIT, this, ( TUint )( NULL ) );
	return NULL;
	}

//-----------------------------------------------------------------------------
// RPositioner
//-----------------------------------------------------------------------------

/**
Constructor for RPositioner
*/
EXPORT_C RPositioner::RPositioner()
  : RPositionerSubSessionBase(), 
    iPtrHolder(NULL),
    iReserved(NULL)
	{
	OstTraceFunctionEntry1( RPOSITIONER_RPOSITIONER_ENTRY, this );
	OstTraceFunctionExit1( RPOSITIONER_RPOSITIONER_EXIT, this );
	}

/**
symbian 2nd phase constructor
*/
EXPORT_C void RPositioner::ConstructL()
	{
	OstTraceFunctionEntry1( RPOSITIONER_CONSTRUCTL_ENTRY, this );
	__ASSERT_ALWAYS(iPtrHolder == NULL, User::Panic(KPosClientFault, EPositionServerHandleNotClosed));
	
	// Pointers to as many messages/requests as can be supported concurrently (currently 4)
	iPtrHolder = CPositioningPtrHolder::NewL(4, 0);
	OstTraceFunctionExit1( RPOSITIONER_CONSTRUCTL_EXIT, this );
	}

/**
destructs the data inside this class
*/
EXPORT_C void RPositioner::Destruct()
	{
	OstTraceFunctionEntry1( RPOSITIONER_DESTRUCT_ENTRY, this );
	delete iPtrHolder;
	iPtrHolder = NULL;
	OstTraceFunctionExit1( RPOSITIONER_DESTRUCT_EXIT, this );
	}

/**
Creates a sub-session with the positioning server. The server uses the 
positioning module with the highest priority by default. 
If the highest priority positioning module is not available or if it 
returns an error for a position request then the 
positioning module with the next highest priority is used.

@param[in] aPosServer is a connected session with the positioning server.
@return a Symbian OS error code.
@panic "Lbs Client Fault" 5 If open is called more than one time on the
same RPositioner instance. 
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  

@pre a connection with the server should already have been created
by calling RPositionServer::Connect().
*/
EXPORT_C TInt RPositioner::Open(RPositionServer& aPosServer)
	{
	OstTraceFunctionEntry1( RPOSITIONER_OPEN_ENTRY, this );
	__ASSERT_ALWAYS(aPosServer.Handle() != 0,
		User::Panic(KPosClientFault, EPositionServerBadHandle));

	TRAPD(ret, ConstructL());
	if (ret == KErrNone)
		{
		ret = CreateSubSession(aPosServer, ELbsPositionerOpen, TIpcArgs());
		}
	if (ret != KErrNone)
		{
		Destruct();
		}
	OstTraceFunctionExitExt( RPOSITIONER_OPEN_EXIT, this, ret );
	return ret;
	}

/**
Creates a sub-session with the positioning server. The client specifies
the module ID of the positioning module to be used for obtaining position 
information.
 
@param[in] aPosServer is a connected session with the positioning server.
@param aModuleId is the module ID for this sub-session to use to
obtain location information. The module ID of different positioning modules can
be obtained by calling RPositionServer::GetNumModules() and 
RPositionServer::GetModuleInfoByIndex()
 
@return a symbian OS error code.
@return KErrNotFound if the module ID is not valid.
panic "Lbs Client Fault" 5 If open is called more than one time on the
same RPositioner instance. 
@panic "Lbs Client Fault" 6 If no connection has been established with 
Location Server ( by calling RPositionServer::Connect() ).  
 
@pre a connection with the server should already have been created
by calling RPositionServer::Connect().
*/
EXPORT_C TInt RPositioner::Open(RPositionServer& aPosServer,
                                TPositionModuleId aModuleId)
	{
	OstTraceFunctionEntry1( DUP1_RPOSITIONER_OPEN_ENTRY, this );
	__ASSERT_ALWAYS(aPosServer.Handle() != 0,
		User::Panic(KPosClientFault, EPositionServerBadHandle));

	TRAPD(ret, ConstructL());
	if (ret == KErrNone)
		{
        TPckg<TPositionModuleId> moduleId(aModuleId);
		ret = CreateSubSession(aPosServer, ELbsPositionerOpenModuleId, TIpcArgs(&moduleId));
		}
	if (ret != KErrNone)
		{
		Destruct();
		}
	OstTraceFunctionExitExt( DUP1_RPOSITIONER_OPEN_EXIT, this, ret );
	return ret;
	}

/**
Creates a sub-session with the positioning server. The client specifies
the criteria for choosing the positioning module. The server chooses the 
positioning module that satisfies the criteria parameter.

@param[in] aPosServer is a connected session with the positioning server.
@param[in] aCriteria is the criteria that the server must use to choose an
appropriate PSY for this sub-session.
@return a Symbian OS error code.
@note This function is not supported and it returns KErrNotFound.
*/
EXPORT_C TInt RPositioner::Open(RPositionServer& aPosServer,
                                const TPositionCriteriaBase& aCriteria)
	{
	OstTraceFunctionEntry1( DUP2_RPOSITIONER_OPEN_ENTRY, this );
	__ASSERT_ALWAYS(aPosServer.Handle() != 0,
		User::Panic(KPosClientFault, EPositionServerBadHandle));

	TRAPD(ret, ConstructL());
	if (ret == KErrNone)
		{
		TPtrC8 ptr(reinterpret_cast<const TUint8*>(&aCriteria),
				   aCriteria.PositionClassSize());
		ret = CreateSubSession(aPosServer, ELbsPositionerOpenCriteria, TIpcArgs(&ptr));
		}
	if (ret != KErrNone)
		{
		Destruct();
		}
	OstTraceFunctionExitExt( DUP2_RPOSITIONER_OPEN_EXIT, this, ret );
	return ret;
	}

/**
Closes a sub-session with the positioning server.
Before a sub-session is closed, the client application must ensure 
that all outstanding notification requests have been cancelled. In 
particular, the application must issue all the appropriate Cancel 
requests and then wait for a confirmation that the notifications 
have been terminated. A failure to do so results in a panic.

@panic "Lbs Client Fault" 16 if the outstanding position request is not
cancelled before calling this method.
*/
EXPORT_C void RPositioner::Close()
	{
	OstTraceFunctionEntry1( RPOSITIONER_CLOSE_ENTRY, this );
	CloseSubSession(ELbsPositionerClose);
	Destruct();
	OstTraceFunctionExit1( RPOSITIONER_CLOSE_EXIT, this );
	}

/**
Set the requestor for this sub-session. 
This method is used when there is only one requestor involved in the
positioning request.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat identifies the format of the requestor.
@param[in] aData identifies the requestor string. The requestor string can 
be a telephone number, a URL etc.
@return KErrNone
@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  

@pre RPositioner::Open() should have been called prior to this operation.
*/
EXPORT_C TInt RPositioner::SetRequestor(CRequestor::TRequestorType /*aType*/,
                                        CRequestor::TRequestorFormat /*aFormat*/,
                                        const TDesC& /*aData*/)
	{
	OstTraceFunctionEntry1( RPOSITIONER_SETREQUESTOR_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	
	OstTraceFunctionExitExt( RPOSITIONER_SETREQUESTOR_EXIT, this, KErrNone );
	return KErrNone;
	}

/**
Sets the requestors for this sub-session. 
This method is used when a chain of requestors is involved in the
positioning request.

@param[in] aRequestorStack is a collection of CRequestor objects.

@return KErrNone.

@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  

@pre RPositioner::Open() should have been called prior to this operation.
*/
EXPORT_C TInt RPositioner::SetRequestor(const RRequestorStack& /*aRequestorStack*/)
	{
	OstTraceFunctionEntry1( DUP1_RPOSITIONER_SETREQUESTOR_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	
	OstTraceFunctionExitExt( DUP1_RPOSITIONER_SETREQUESTOR_EXIT, this, KErrNone );
	return KErrNone;
	}

/**
This method returns cached position information if it is available. This
method can be an efficient mechanism - in terms of speed, cost and power
consumption - of obtaining the devices' recent position.

@param[out] aPosInfo will be set, upon successful completion, to the most recently
determined location data.
@param[out] aStatus returns the result code after the asynchronous call completes.
The parameter can contain the following values,
- KErrNone on successful completion.
- KErrUnknown if no cached position information is available. 
- KErrArgument if the parameter aPosInfo is of a non-supported type. The 
only parameter type that is guaranteed to be supported is TPositionInfo.
- KErrAccessDenied if no requestor information has been specified or if
privacy check fails.
- KErrPositionBufferOverflow if there is insufficient space to return 
the required information back to the client. This situation can occur when 
using HPositionGenericInfo if the application has not allocated a large enough buffer.

@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  

@note To cancel this request use RPositioner::CancelRequest() with 
EPositionerGetLastKnownPosition as parameter.

@note The GetLastKnownPosition does not use any of the options specified
using SetUpdateOptions() (namely update interval, maximum age, timeout and 
partial updates).

@pre The application should have called RPositioner::SetRequestor() before
calling this method.
*/
EXPORT_C void RPositioner::GetLastKnownPosition(TPositionInfoBase& aPosInfo,
                                                TRequestStatus& aStatus) const
	{
	OstTraceFunctionEntry1( RPOSITIONER_GETLASTKNOWNPOSITION_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Panic(KPosClientFault, EPositionNullPointerHolder));

	iPtrHolder->Ptr(0).Set(
		reinterpret_cast<TUint8*>(&aPosInfo),
		aPosInfo.PositionClassSize(),
		aPosInfo.PositionClassSize());

	SendReceive(ELbsGetLastKnownPosition, 
				TIpcArgs(&iPtrHolder->Ptr(0), aPosInfo.PositionClassType(), aPosInfo.PositionClassSize()), 
				aStatus);
	OstTraceFunctionExit1( RPOSITIONER_GETLASTKNOWNPOSITION_EXIT, this );
	}

/**
This method returns the area specific latest cached position information if it 
is available.

Unlike GetLastKnownPosition(), GetLastKnownPositionArea() does not just return the
latest cached position. It analyses the current area and finds the best match
(for more information about match criteria see TPositionAreaInfo and derived classes)
in an internal database of known positions instead.
If complete match is not available, the best matching position is returned.
If complete match is not available, and there are multiple cached positions with 
the same, highest match level, the latest position is returned.

Many different techniques may be applied to retrieve area information quickly
and power/cost effectively. The most common one uses information from a mobile 
network to determine the country, or the mobile cell the phone is connected to.
Other techniques may involve scanning available WLAN networks.

The user of the API can request the LBS subsystem to provide the basic or the 
extended area information. If the type of the aAreaInfo parameter is 
TPositionAreaInfo, then the LBS subsystem will return the basic information only.
Passing a parameter of the TPositionAreaExtendedInfo type will instruct the
LBS subsystem to return detailed information.

Please note that when calculating area the LBS subsystem assumes the worst 
case scenario (e.g. mobile cells are big - 10s of km radius). In the centres 
of big cities the cell sizes are normally much smaller, taking the area from a 
City to a District or even Street level. A mapping application having that specialised
knowledge about the nature and the topography of the location can therefore adjust the
area accordingly before presenting the final results to a user.

Currently, the only supported source of area information is the mobile network.

@see TPositionAreaInfo
@see TPositionAreaExtendedInfo

@param aPosInfo [InOut] Upon successful, asynchronous completion, the parameter
will be set to the best matching known location.
Please note that the embedded accuracy information is historical information 
about the quality of the position when it was captured and not the current 
accuracy. Use the aAreaInfo parameter to estimate the current accuracy of 
the position.

@param aAreaInfo [Out] Upon successful, asynchronous completion, the parameter
will contain information about area of the position returned in the aPosInfo
parameter to the currently known area info.

@param aStatus [Out] Returns the result code after the asynchronous call completes.
KErrNone if successful; KErrNotFound if a matching entry has not been found; 
KErrNotSupported if the functionality is not supported; any other system wide 
error code otherwise.

The call is supported by the lbs.lib only.

@publishedAll
@prototype
@capability Location
*/
EXPORT_C void RPositioner::GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
					      				 TPositionAreaInfoBase& aAreaInfo,
	                                   	 TRequestStatus& aStatus) const
	{
	 OstTraceFunctionEntry1( RPOSITIONER_GETLASTKNOWNPOSITIONAREA_ENTRY, this );
	 
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Panic(KPosClientFault, EPositionNullPointerHolder));
	
	iPtrHolder->Ptr(2).Set(
			reinterpret_cast<TUint8*>(&aPosInfo),
			aPosInfo.PositionClassSize(),
			aPosInfo.PositionClassSize());
	iPtrHolder->Ptr(3).Set(
			reinterpret_cast<TUint8*>(&aAreaInfo),
			aAreaInfo.PositionClassSize(),
			aAreaInfo.PositionClassSize());
	
	SendReceive(ELbsGetLastKnownPositionArea, 
				TIpcArgs(&iPtrHolder->Ptr(2),&iPtrHolder->Ptr(3)), 
				aStatus);
	OstTraceFunctionExit1( RPOSITIONER_GETLASTKNOWNPOSITIONAREA_EXIT, this );
	}
	
/**
This is an asynchronous method for obtaining position updates. It is possible
to pass any class that is derived from TPositionInfoBase. However, the
standard data retrieval class is TPositionInfo. The standard means of
retrieving extended information is to use HPositionGenericInfo.

@param[out] aPosInfo will hold, on successful completion, information on the
devices' current position.
@param[out] aStatus returns the result code after the asynchronous call completes.
On completion, the parameter can contain the following values,
- KErrNone on successful completion.
- KPositionQualityLoss if the positioning module is unable to return 
any position information.
- KPositionPartialUpdate if position information has been retrieved but 
it is incomplete.
- KErrNotFound is returned if the currently used module is invalid. A 
previously correct module may become invalid if the positioning 
module has been uninstalled or disabled by user.
- KErrTimedOut if the requested location information could not be 
retrieved within the maximum period as specified in the current update 
options.
- KErrArgument if the positioning module is unable to support the type 
of the class passed in aPosInfo. All positioning modules are required 
to support both TPositionInfo and HPositionGenericInfo.
- KErrAccessDenied if no requestor information has been specified or if
privacy check fails.
- KErrPositionBufferOverflow if there is insufficient space to return 
the required information back to the client. This situation can occur when 
using HPositionGenericInfo if the application has not allocated a large 
enough buffer.
- KErrCancel if the request was successfully cancelled.
@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  
@panic "Lbs Client Fault" 15 If there is already an outstanding request
pending for position information.

@note To cancel this request use RPositioner::CancelRequest() with 
EPositionerNotifyPositionUpdate as parameter.

@pre The application should have called RPositioner::SetRequestor() before
calling this method.
*/
EXPORT_C void RPositioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
                                                TRequestStatus& aStatus) const
	{
	OstTraceFunctionEntry1( RPOSITIONER_NOTIFYPOSITIONUPDATE_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));
	__ASSERT_ALWAYS(iPtrHolder != NULL, User::Panic(KPosClientFault, EPositionNullPointerHolder));

	iPtrHolder->Ptr(1).Set(
		reinterpret_cast<TUint8*>(&aPosInfo),
		aPosInfo.PositionClassSize(),
		aPosInfo.PositionClassSize());

	SendReceive(ELbsPosNotifyPositionUpdate, TIpcArgs(&iPtrHolder->Ptr(1)), aStatus);

	OstTraceFunctionExit1( RPOSITIONER_NOTIFYPOSITIONUPDATE_EXIT, this );
	}

/**
This method can be used to modify the current options set for this sub-session.
It enables the client to request an interval time for receiving
position updates.

@param[in] aPosOption contains the clients requested options for receiving
location updates.

@return a Symbian OS error code.
@return KErrArgument if the specified options conflict – for example, 
if the timeout period is less than the specified update interval.
@return KErrNotSupported if the positioning module is unable to support 
the required options – for example, if the update interval period is 
too short.

@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  

@pre RPositioner::Open() should have been called prior to this operation.
*/
EXPORT_C TInt RPositioner::SetUpdateOptions(const TPositionUpdateOptionsBase& aPosOption)
	{
	OstTraceFunctionEntry1( RPOSITIONER_SETUPDATEOPTIONS_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

	TPtrC8 ptr(
		reinterpret_cast<const TUint8*>(&aPosOption),
		aPosOption.PositionClassSize());

	return SendReceive(ELbsSetUpdateOptions, TIpcArgs(&ptr));
	}

/**
This method retrieves the current options set for this sub-session.
These options are related to receiving the position update from the server.

@param[out] aPosOption contains, upon successful completion, the set of update
options for NotifyPositionUpdate() that are currently in use.

@return a Symbian OS error code.

@panic "Lbs Client Fault" 6 If no sub session has been created with 
this session ( by calling RPositioner::Open() ).  
*/
EXPORT_C TInt RPositioner::GetUpdateOptions(TPositionUpdateOptionsBase& aPosOption) const
	{
	OstTraceFunctionEntry1( RPOSITIONER_GETUPDATEOPTIONS_ENTRY, this );
	__ASSERT_ALWAYS(SubSessionHandle(), User::Panic(KPosClientFault, EPositionServerBadHandle));

	TPtr8 ptr(
		reinterpret_cast<TUint8*>(&aPosOption),
		aPosOption.PositionClassSize(),
		aPosOption.PositionClassSize());

	return SendReceive(ELbsGetUpdateOptions, TIpcArgs(&ptr));
	}

/**
This method is used to allow polymorphic extensions to the API without breaking
BC. See documentation for explanation.

@publishedAll
@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data
@param aPtr2 a pointer to any data.
 */
EXPORT_C TAny* RPositioner::ExtendedInterface(TInt aFunctionNumber,
                                              TAny* aPtr1,
                                              TAny* aPtr2)
	{
	OstTraceFunctionEntry1( RPOSITIONER_EXTENDEDINTERFACE_ENTRY, this );
	return RPositionerSubSessionBase::ExtendedInterface(aFunctionNumber, aPtr1, aPtr2);
	}
