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
 @test
*/
#ifndef LBS_SUPLTEST_H
#define LBS_SUPLTEST_H


//-----------------------------------------------------------------------------------
#include <e32base.h>
#include <lbs/lbsassistancedatasourcemodule.h>
#include <es_sock.h>
#include <in_sock.h>
#include <lbs/test/suplmessage.h>
#include <lbs/test/suplstart.h>
#include <lbs/test/suplpos.h>
#include <lbs/test/suplposinit.h>
#include <lbs/test/rrcassistancedatadelivery.h>
#include "lbssuplsocketlistener.h"
//#include <suplresponse.h>
#include <lbs/test/suplend.h>



/**
This class defines the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.

@see CAssistanceDataSourceModule

The class also implements the internal MProtocolMgrObserver interface
which is used to receive calls from within the protocol module.
@see MProtocolMgrObserver
*/
NONSHARABLE_CLASS(CSuplProtocolSession)
 : public CAssistanceDataSourceBase, public CActive
	{

public:

    static CSuplProtocolSession* NewL(MLbsAssistanceDataSourceObserver& aObserver,RSocketServ& aSocketServer, const TInt16& aSessionNumber);
	~CSuplProtocolSession();
	
	// Implementation of interface CAsssistanceDataSourceBase
	void GetAssistanceData(RLbsAssistanceDataBuilderSet& aSet, const TLbsAssistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition);
	void CancelGetAssistanceData();
	
	// Methods from CActive
	void RunL();
	void DoCancel();
    TInt RunError(TInt aError);
	
	// Call back for timeouts
	static TInt OnTimerEventL(TAny* aPtr);

private:

 	/** SUPL session states
	*/
	enum TSuplSessionState
		{
		/**  */
		EIdle,
		/**  */
		EConnectingToRemoteServer,
		/**  */
		EConnectedAwaitingRequest,
		/** */
		EAssistanceDataRequested,
		/**  */
		ESendingSuplStart,
		/** */
		EAwaitingSuplResponse,
		/** */
		ESuplResponseAvailable,
		/** */
		ESendingSuplPosInit,
		/**  */
		EAwaitingSuplPos,
		/** */
		ESuplPosAvailable,
		/**  */
		ERrcPayloadDecodeComplete,
		/**  */
		EDisconnectingFromServer,
		/**  */
		ENetworkTimeout,
		/** */
		EProtocolError,
		/** */
		EEndingSession,
		/** */
		ESuplEndSent,
		};

	CSuplProtocolSession(MLbsAssistanceDataSourceObserver& aObserver, const TInt16& aSessionNumber);
	void ConstructL(RSocketServ& aSocketServerSession);

	// Utility methods for creating messages for sending
	void CreateSuplStartMessageL();
	void CreatePosInitMessageL();
	void CreateSuplEndMessageL();


    void BuildCellInfoL(CSuplCellInfoContent* aContent, TCellInformationChoice aCellInfoChoice);
	void BuildSetCapabilitiesL(CSuplSetCapabilitiesContent* aContent);
	void BuildCommonPartL(CSuplMessageContent* aSuplMsgContent);
	void BuildPositionInfoL(CSuplPositionContent* posContent);
	
	// Utility methods for decoding received messages
	void DecodeSuplMessageL(TSuplMessageType aSuplMessageType);
	TSuplMessageType DoDecodeMessageL();
	void DecodeRRCPayloadL();
    void TransitionAccordingToRequestAvailability();

	// Asynchronous services requested by state machine
	void ConnectToRemoteServer();
	void ReadSuplMessage();
	void SendSuplMessageL(TSuplMessageType aSuplMessageType);
	
	void PrepareSelfTransition();

	// Process a timeout
	void DoProcessTimeoutL();
	
private:

	// From the Network Simulator
	MLbsAssistanceDataSourceObserver* iObserver; // For callbacks into the NetSim
	RLbsAssistanceDataBuilderSet* iBuilderSet; // Not owned, belongs to the NetSim
	TInt16 iSessionNumber;
	
 	// Passed in GetAssistanceData
    TLbsAsistanceDataGroup iAssistanceDataFilter;
    TCoordinate iPosition;
    
    // Used for sending/receiving messages
    //
	RBuf8 iTxBuf;
	RBuf8 iRxBuf;
	
	RSocket iSocket;
	TInetAddr iAddress; // Remote server IP address

	CSuplSocketListener* iSuplSocketListener;
	
	// Used by the state machine
	//
	TBool iAssistanceDataRequestPending;
	TSuplSessionState iSessionState;
	CPeriodic* iTimer; 
	
	// Obtained from SUPL RESPONSE message
	TBuf8<4> iSlpSessionId;
	// Obtained from SUP POS message
	TUint iRrcOffset; // offset to the RRC payload in an encoded SUPLPOS message	
	};


/**
This class implements the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.
@see CAssistanceDataSourceModule


@see 
*/
NONSHARABLE_CLASS(CSuplProtocolModule)
 : public CAssistanceDataSourceModuleBase
	{
	
public:

	static CSuplProtocolModule* NewL(TAny* aParams);
	~CSuplProtocolModule();

    CAssistanceDataSourceBase* NewDataSourceL();

private:

	CSuplProtocolModule(TAny*);
	CSuplProtocolModule();
	void ConstructL();

private:

	/** Reference to this module's observer.
	This is used to make callbacks into the Network Simulator.
	*/
	MLbsAssistanceDataSourceObserver& iObserver;

	/** Socket Server Session.
	This session with the Socket Server will be used
	by the SUPL sessions to obtain their TCP/IP sockets.
	*/
	RSocketServ iSocketServerSession;

	/* To be part of the session ID of the next SUPL
	session that is created*/
	TInt16 iNextSessionNumber;
	};

#endif //LBS_SUPLTEST_H
