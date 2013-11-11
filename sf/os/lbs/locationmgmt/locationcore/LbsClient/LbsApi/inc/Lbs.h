// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBS_H__
#define __LBS_H__

#include <lbspositioninfo.h>
#include <lbsareainfo.h>
#include <lbsipc.h>
#include <lbscommon.h>
#include <lbsrequestor.h>
#include <lbscriteria.h>

// Forward declarations
class CPositioningPtrHolder;
class CServerPositioningPtrHolder;
class RPositionServer : public RSessionBase

/**
This is generally the first interface class used by all client applications.
It is used to make the primary connection to the location server. After the
primary connection has been established, its handle is passed as a parameter
of the Open methods of RPositioner to create a "sub-session".

The RPositionServer class can also be used to discover what position
technology "modules" are available. However, this is only required if a
client application actually needs to use a particular module. It is also
used for requests to clear(empty) the last known position store.

Asynchronous requests can be cancelled using the method CancelRequest() with
a value from the enumeration _TReqestId corresponding to the particular
operation being cancelled.

@publishedAll
@released
 */
	{
public:
	/** Type for values defined in _TRequestId
	@publishedPartner
	@prototype*/
	typedef TInt TRequestId;

 	/** Identification numbers used for cancelling requests.
 	Note that the TRequestId enum may be extended in the future by adding
 	more enumerated values. To maintain compatibility any unrecognized values 
 	must be handled as EReqUnknown.
 	@publishedPartner
	@prototype*/
	enum _TRequestId
		{
		/** Unknown/undefined Id */
		EReqUnknown						= 0,
		/** The id of the RPositionServer::NotifyModuleStatusEvent request */
		EReqNotifyModuleStatusEvent     = 1,
		/** The id of the RPositionServer::EmptyLastKnownPositionStore request */
		EReqEmptyLastKnownPositionStore = 2
		};
		
public:
	IMPORT_C RPositionServer();

	IMPORT_C TInt Connect();
	IMPORT_C void Close();

	IMPORT_C TInt CancelRequest(TRequestId aRequestId);
	IMPORT_C TVersion Version() const;

	IMPORT_C TInt GetDefaultModuleId(TPositionModuleId& aModuleId) const;
	IMPORT_C TInt GetNumModules(TUint& aNumModules) const;
	IMPORT_C TInt GetModuleInfoByIndex(TInt aModuleIndex,
	                                   TPositionModuleInfoBase& aModuleInfo) const;
	IMPORT_C TInt GetModuleInfoById(TPositionModuleId aModuleId,
	                                TPositionModuleInfoBase& aModuleInfo) const;

	IMPORT_C TInt GetModuleStatus(TPositionModuleStatusBase& aPosModuleStatus,
	                              TPositionModuleId aModuleId) const;

	IMPORT_C void NotifyModuleStatusEvent(TPositionModuleStatusEventBase& aStatusEvent,
	                                      TRequestStatus& aStatus,
	                                      const TPositionModuleId aModuleId = KPositionNullModuleId) const;
	                                      
	IMPORT_C void EmptyLastKnownPositionStore(TRequestStatus& aStatus);

private:
	void ConstructL();
	void Destruct();
	static TInt StartServer();

private:
	/** A pointer to a container that holds pointer descriptors, needed to point
	to the clients request data during asynchronous requests */
	CServerPositioningPtrHolder* iPtrHolder;
	
	/** Variable for future expansion. */
	TAny* iReserved;
	};


class RPositionerSubSessionBase : public RSubSessionBase
/**
Abstract base class for all positioning sub-sessions, including RPositioner.

Asynchronous requests issued from derived classess are cancelled using the
method CancelRequest() with a value from the enumeration _TReqestId corresponding
to the particular operation being cancelled.

@see RPositioner
@publishedAll
@released
 */
	{
	
public:

	/** Type for values defined in _TRequestId
	@publishedPartner
	@prototype*/
	typedef TInt TRequestId;

	/** Identification numbers used for cancelling requests.
 	Note that the TRequestId enum may be extended in the future by adding
 	more enumerated values. To maintain compatibility any unrecognized values 
 	must be handled as EReqUnknown.
 	@publishedPartner
	@prototype*/
	enum _TRequestId
		{
		/** Unknown/undefined Id */
		EReqUnknown					 = 0,
		/** The id of the GetLastKnownPosition request */
		EReqGetLastKnownPosition     = 1,
		/** The id of the GetLastKnownPositionArea request */
		EReqGetLastKnownPositionArea = 2,
		/** The id of the ReqNotifyPositionUpdate request */
		EReqNotifyPositionUpdate     = 3
		};
public:
	IMPORT_C TInt CancelRequest(TRequestId aRequestId);

	IMPORT_C TInt CompleteRequest(TInt aRequestId);

protected:
	IMPORT_C RPositionerSubSessionBase();
	
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);

private:
	/** Unused variable for future expansion. */
	TAny* iReserved;
	};


class RPositioner : public RPositionerSubSessionBase
/**
This class is used to create a sub-session with the server for the
purpose of obtaining the current position. In addition to actually
obtaining position information, this class also provides mechanisms
for obtaining the last known position, the last known position with 
area information, the general status of the positioning module,
changing how often it wishes to receive position updates, as well as 
identifying itself to the location framework. 

Before using the class, a primary connection must have already been
established with the server.

@see RPositionServer
@publishedAll
@released
 */
	{

public:
	IMPORT_C RPositioner();

	IMPORT_C TInt Open(RPositionServer& aPosServer);
	IMPORT_C TInt Open(RPositionServer& aPosServer,
	                   TPositionModuleId aModuleId);
	IMPORT_C TInt Open(RPositionServer& aPosServer,
	                   const TPositionCriteriaBase& aCriteria);
	IMPORT_C void Close();

	IMPORT_C TInt SetRequestor(CRequestor::TRequestorType aType,
	                           CRequestor::TRequestorFormat aFormat,
	                           const TDesC& aData);
	IMPORT_C TInt SetRequestor(const RRequestorStack& aRequestorStack);

	IMPORT_C TInt SetUpdateOptions(const TPositionUpdateOptionsBase& aPosOption);
	IMPORT_C TInt GetUpdateOptions(TPositionUpdateOptionsBase& aPosOption) const;

	IMPORT_C void GetLastKnownPosition(TPositionInfoBase& aPosInfo,
	                                   TRequestStatus& aStatus) const;
	                                   
	IMPORT_C void GetLastKnownPositionArea(TPositionInfoBase& aPosInfo,
					      				 TPositionAreaInfoBase& aAreaInfo,
	                                   	 TRequestStatus& aStatus) const;

	IMPORT_C void NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
	                                   TRequestStatus& aStatus) const;

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
    IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);

public:
	TInt OpenImpl(RPositionServer& aPosServer, TPositionModuleId aModuleId, const TPositionCriteriaBase& aCriteria, TBool aOpenedUsingModuleId);
private:
	/** A pointer to a container that holds pointer descriptors, needed to point
	to the clients request data during asynchronous requests */
	CPositioningPtrHolder* iPtrHolder;
	/** Unused variable for future expansion. */
	TAny* iReserved;
	};


#include <lbs.inl>

#endif // __LBS_H__
