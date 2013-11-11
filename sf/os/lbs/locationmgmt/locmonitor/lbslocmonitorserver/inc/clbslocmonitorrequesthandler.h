/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CLbsLocMonitorRequestHandler_H
#define CLbsLocMonitorRequestHandler_H

#include "lbstimer.h"
#include "tserverstartparams.h"
#include "messageutils.h"
#include "LbsInternalInterface.h"
#include "LbsLogger.h"
#include "mlbslocmonitorareainfofinderobserver.h"
#include "clbslocmonitorposlistener.h"
#include "clbslocmonitorutils.h"
#include "rlbslocmonitordb.h"
#include "lbslocmonitorserverdata.h"

/**
This class accepts requests for retrieving positions from a database that had been
previosly stored in association with current area information (e.g. global cell id).
The requests are of two types: get the last know position and get the last known
position with location area information.

The requests originate in subsessions (clients) of the Location Monitor and are kept
in a queue provided by this class.

All of the queued requests of the same type are completed when a position is
retrieved from the database.

This class is an active object because the requests it sends to the database may be
asynchronous.

The class derives from MLbsLocMonitorAreaInfoFinderObserver so that it can be notified
of current area information (e.g, global cell id) from any of the Location Monitor's
area information "finder" objects (currently only one such object exits to collect
global cell information).

@internalComponent
@released
*/
class CLbsLocMonitorRequestHandler : public CActive,
									public MLbsLocMonitorAreaInfoFinderObserver,
									public MLbsLocMonitorPosListenerObserver,
									public MLbsCallbackTimerObserver
	{

	enum TAreaInfoType
		{
		EGlobalCellIdType = 0,
		ELastAreaInfoType,	
		};
		
	enum EDbOperationType
		{
		ELocMonDbOperationNone,
		ELocMonDbSavePos,
		ELocMonDbSaveLastPos,
		ELocMonDbGetPosArea,
		ELocMonDbGetLast,
		};

public:
	
	static CLbsLocMonitorRequestHandler* NewL();
	~CLbsLocMonitorRequestHandler();
	
	//
	// Methods called from server and server subsessions to save
	// or retrieve positions from the database.
	//

	void PositionRequestL(const RMessage2& aMessage);

	void CancelPositionRequestL(const RMessage2& aMessage);
	
	void StoreLastKnownPosition(const TPosition& aPosition);

	/*
	Called when a subsession is closed (its outstanding requests
	have to be completed/destroyed)
	*/
	void ClientTerminated(const CLbsLocMonitorUtils::sessionAndSubSessionId& aSubsessionId);
	
	/*
	This method is called by sessions of the location monitor to delete
	the database of positions.
	*/
	TInt DeleteDatabaseL();
		
	/*
	Cancel a previous request to delete the database.
	*/
	void CancelDeleteDatabaseL(const RMessage2& aMessage);
	
	//
	// Methods derived from MLbsLocMonitorAreaInfoFinderObserver
	//
	
	/*
	Method called to inform this class of an updated of area information
	(e.g, new current global cell id)
	*/
	void AreaInfoUpdate(const TLbsLocMonitorAreaInfoBase& aAreaInfo);
	
	//
	// Methods derived from MLbsLocMonitorPosListenerObserver
	//
	
	/* Method called to request that a position is
	stored in the database. This method is called
	as a consequence of a new position being observed
	on an internal position bus.
	*/
	void PositionAvailableL(const TPosition& aPosition);
	
private:

	CLbsLocMonitorRequestHandler();
	void ConstructL();
	
	void ProcessNextRequest();
	void CompleteClientRequest(TInt aIndex, TInt aCompletionCode);
	TInt CompleteSameTypeMessages(EDbOperationType aDbReadOperation, TInt aError);
	void ClearRequestVariables();
	TInt DbReadOperationToOpCode(EDbOperationType aDbOperationType);
	void HandleDbAccessError(TInt aError);

	// From MLbsCallbackTimerObserver
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

	// Derived from CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:

	//Interface to positions DB
	RLbsLocMonitorDb iDb; 
	
	// Type of async DB operation in progress
	EDbOperationType iOperationInProgress; 

	//Requests from different clients.
	RArray<RMessage2> iRequestQueue; 

	// An array with current area info from different sources (one entry per source).
	// Currently only one element in the array as only cell-information is used.
	RPointerArray<TLbsLocMonitorAreaInfoBase>  iCurrentAreaInfo;
	
	TBool iAreaInfoAvailable;
	
	// A queue of positions waiting to be written to the database. 
	RArray<TPosition> iPositionsQueue;

	// Temporary store for the LKP communicated by the LS
	TPosition iLastKnownPosition;
	// Whether a last known position is available for storing.
	TBool iLastKnownPositionAvailable;

	// The following variables are data members so that no memory from the heap
	// has to be allocated for them (keeping a static memory footprint)
	TPosition iPosition; // Position read from the DB
	TPositionAreaExtendedInfo iAreaMatchInfo; // Area Information read from the DB
	TPositionInfo iPositionInfo; // Used to write position info back to client
	TPosAreaReqParams iRequestParams; // Used to find out the type of client's area info
	
	CLbsCallbackTimer* iDbOperationTimer;	
	};
	
#endif //CLbsLocMonitorRequestHandler_H
