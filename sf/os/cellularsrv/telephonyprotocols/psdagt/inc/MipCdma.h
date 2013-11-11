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
* This file contains the definition of the
* handling MIP handovers in a CDMA2000 network.
* 
*
*/



/**
 @file MIPCDMA.H
 @internalComponent
*/

#ifndef __MIPCDMA_H__
#define __MIPCDMA_H__
#include <e32base.h>
#include <etel.h>
#include <etelmm.h>
#include <comms-infras/dbaccess.h>

class CPSDAgent;
class CMIPCdmaEtelZoneChangeRequest;

class CMipCdmaHandoverHandler : public CActive
/**
This class is the main class that handles MIP handover in a 
CDMA2000 network. It is written as an active object.
It is triggered by one of two things
1) a PPPLinkDown event or
2) an ETEL notification that we have changed Zone

When either of these two events come in it triggers a timer.
If the timer runs out before the other  event takes place a normal
Reconnect dialog box is shown. But, if the other event takes place 
before the timer runs out, we will restart the connection without 
user intervention.
Lifetime of the object is as long as its owner.

@internalComponent
*/
	{
public:
	static CMipCdmaHandoverHandler* NewLC(CPSDAgent& aAgent, CCommsDbAccess& aDb);
	static CMipCdmaHandoverHandler* NewL(CPSDAgent& aAgent, CCommsDbAccess& aDb);
	~CMipCdmaHandoverHandler();
	
	//Trigger Functions
	TBool PPPLinkDownEvent();
	void ETELChangedZoneEvent();

	//Starting and stopping to listen
	TInt StartListening();
	void StopListening();

private:
	CMipCdmaHandoverHandler(CPSDAgent& aAgent, CCommsDbAccess& aDb);
	void ConstructL();
	
	// From CActive
	virtual void DoCancel();
	virtual void RunL();
	
	void StartTimer();
	void RestartPPP();
private:
	RTimer iTimer;											///< Timer object 
	TBool iPPPLinkDownEvent;								///< Boolean describing if we have got a PPPLinkDownEvent 
	TBool iETELChangedZoneEvent;							///< Boolean describing if we have got an ETEL Zone Change Event
	CPSDAgent& iAgent;										///< Reference to our owner(parent)
	CMIPCdmaEtelZoneChangeRequest* iMipCdmaEtelZoneChanger;	///< Handle to our subclass that is responsible for receiving ETEL Zone change notifications
	CCommsDbAccess& iDb;
	};

class CMIPCdmaEtelZoneChangeRequest : public CActive
/**
This class is a subclass owned by the CMipCdmaHandoverHandler.
It is responsible to receive ETEL Zone change events.
The class is written as an active object.

When a notification comes in this class will notify its owner.
Lifetime of the object is as long as its owner.
The object can be started and stopped from the owner.

@internalComponent
*/
	{
public:
	static CMIPCdmaEtelZoneChangeRequest* NewLC(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb);
	static CMIPCdmaEtelZoneChangeRequest* NewL(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb);
	~CMIPCdmaEtelZoneChangeRequest();
	
	//Trigger Function
	void StartRequest();

	//Starting and stopping to listen to notifications
	TInt StartListening();
	void StopListening();

private:
	CMIPCdmaEtelZoneChangeRequest(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb);
	void ConstructL();
	
	// From CActive
	virtual void DoCancel();
	virtual void RunL();

	void InitPhoneL();
private:
	TBool iFirstTime;							///< Boolean stating if it is the first time the object is run.
	CMipCdmaHandoverHandler& iHandoverHandler;	///< Handle to our owner/parent that we report to
	CPSDAgent& iAgent;							///< Handle to the agent class
	CCommsDbAccess& iDb;						///< Handle to CommDB
	TBool iStarted;								///< Boolean describing if we are started (i.e. the link is up)
	RTelServer iTelServer;						///< Handle to a phone server
	RMobilePhone iMmPhone;						///< Handle to a phone object
	HBufC* iTsyName;							///< Handle to the TSY name
	RMobilePhone::TMobilePhoneNetworkInfoV1 iAsyncCurrentNetwork;		///< Object storing network information
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iPhoneNetworkInfoPckg;	///< Package object for the TMobilePhoneNetworkInfoV1 object
	RMobilePhone::TMobilePhoneLocationAreaV1 iAsyncLocArea;				///< Object storing location information
	RMobilePhone::TMobilePhoneNetworkIdentity iCurrentCdmaSID;			///< Object storing SID
	RMobilePhone::TMobilePhoneNetworkIdentity iCurrentNetworkId;		///< Object storing NID
	};
#endif
