/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header for asyncronous request wrapping active object
* 
*
*/



/**
 @file ASYNWRAP.H
 @internalComponent
*/



#ifndef __ASYNWRAP_H__
#define __ASYNWRAP_H__

#include <e32base.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>

/**
Contains information about ETel Request Error
*/
struct TETelRequestError
	{
	TInt iError;			///<  ETel Request Error number
	TInt iFailedRequest;	///< ETel Failed Request Error number
	};

/**
Contains the information returned from ETEL
*/
struct TETelAsyncRequestData
	{
	TETelRequestError iETelAsynError;	///< ETel Request Error 
	RPacketService::TMSClass iMSClass;	///< MS Class configuration of packet-switched connection to a packet network

	/** Packet Network Registration/Availability Status 
	of packet-switched connection to a packet network
	*/
	RPacketService::TRegistrationStatus iRegistrationStatus;
	};

class MAsyncRequestsObserver
/**
Class to register the asyncronous requests

@internalComponent
*/
	{
public:
	/**
	The MS Class and Network Registration status have been retrieved
	Store these values and work out which bearers are available

	@param aRequestData, contains the information returned from ETEL
	*/
	virtual void ETelAsyncRequestsComplete(TETelAsyncRequestData aRequestData) = 0;
	};

NONSHARABLE_CLASS(CASyncEtelRequestWrapper) : public CActive
/**
Class to make asyncronous requests synchronous whilst not blocking the 
thread and thus allowing other active objects to run as normal

@internalComponent
*/
	{
public:
	static CASyncEtelRequestWrapper* NewL(MAsyncRequestsObserver* aObserver);
	~CASyncEtelRequestWrapper();
	void StartAsyncRequests(RTelServer& aTelServer, RMobilePhone& aMmPhone);
private:
	CASyncEtelRequestWrapper();
	void ConstructL();

	// From CActive 
	virtual void DoCancel();
	virtual void RunL();
private:
	enum TETelAsyncRequestState
		{
		ENoRequests,
		EGetMSClass,
		EGetNwRegStatus
		};
private:
	MAsyncRequestsObserver* iObserver;
	RPacketService iPacket;
	TETelAsyncRequestData iAsyncRequestData;
	RPacketService::TMSClass iMaxMSClass;
	TETelAsyncRequestState iAsyncRequestState;
	TInt iReqToCancel;
	};
#endif

