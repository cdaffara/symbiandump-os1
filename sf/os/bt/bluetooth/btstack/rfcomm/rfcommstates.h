// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMMSTATES_H
#define RFCOMMSTATES_H

#include <bt_sock.h>
#include "rfcommsap.h"
#include "rfcommutil.h"
#include "rfcommtypes.h"

// Defines the states for the state pattern used by the rfcomm saps

class CRfcommState;
class CRfcommSAP;

NONSHARABLE_CLASS(CRfcommStateFactory) : public CBase
	{
friend class CRfcommProtocol;
public:
	enum TRfcommStates
		{
		EError,
		EClosed,
		EWaitForMux,
		EWaitForPNResp,
		EWaitForUA,
		EOpen,
		EDisconnect,
		EListening,
		EWaitForSABM,
		EWaitForOutgoingSecurityCheck,
		EWaitForIncomingSecurityCheck,
		EWaitForStart,
		ECloseOnStart,
		EDisconnecting,
	// *** keep next one last ***
		ERfcommMaxState,
		};
	
	TRfcommState* GetState(const TRfcommStates aState);
	TInt StateIndex(const TRfcommState* aState) const;
	
	static CRfcommStateFactory* NewL();
	~CRfcommStateFactory();
private:
	CRfcommStateFactory();
	void ConstructL();

	TFixedArray<TRfcommState*, ERfcommMaxState> iStates;
	};

/**
   The base class for all the Rfcomm SAP states.
   
   This is interface that all state objects participating in the state
   pattern have to adhere to.  It provides a mirror of the interface
   of a SAP, since SAPs pass all actions through to the state object
   to provide stateful behaviour.

   Since States are implemented as flyweight objects, there can be no
   SAP specific state information maintained in the state.  All the
   information specific to the SAP is stored by the SAP, which acts as
   a context for the state.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommState)
	{
public:
	TRfcommState(CRfcommStateFactory& aFactory);
	// State
	virtual void Enter(CRfcommSAP& aSAP)=0;  // Entry to a state
	virtual void Exit(CRfcommSAP& aSAP)=0;	//	Exiting a state
	// From the socket
	virtual void ActiveOpen(CRfcommSAP& aSAP)=0;
	virtual TInt PassiveOpen(CRfcommSAP& aSAP, TUint aQueSize)=0;
	virtual void Shutdown(CRfcommSAP& aSAP)=0;
	virtual void FastShutdown(CRfcommSAP& aSAP)=0;
	virtual TInt Send(CRfcommSAP& aSAP, const TDesC8& aData)=0;
	virtual void Read(CRfcommSAP& aSAP, TDesC8& aData)=0;
	virtual void Ioctl(CRfcommSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption)=0;
	virtual void CancelIoctl(CRfcommSAP& aSAP, TUint aLevel, TUint aName)=0;
	virtual void Start(CRfcommSAP& aSAP)=0;
	virtual TInt SetOption(CRfcommSAP& aSAP, TUint aLevel, 
						   TUint aName, const TDesC8 &aOption) =0;

	// From the muxer
	virtual void MuxUp(CRfcommSAP& aSAP)=0;
	virtual void LinkDown(CRfcommSAP& aSAP)=0;
	virtual void IoctlComplete(CRfcommSAP& aSAP, TInt aErr, TUint aLevel,TUint aName, TDesC8* aBuf) =0;

	// From the security requester
	virtual void AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult)=0;

	// frame types
	virtual void SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI)=0;	
	virtual void UA(CRfcommSAP& aSAP)=0;
	virtual void DISC(CRfcommSAP& aSAP)=0;
	virtual void DM(CRfcommSAP& aSAP)=0;
	// mux channel commands
	// Remote parameter negotiation
	virtual void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI)=0;
	virtual void RPNRsp(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction)=0;
	// Parameter negotiation
	virtual void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI)=0;  
	virtual void PNResp(CRfcommSAP& aSAP, TRfcommPortParams& aParams)=0;
	virtual void MSC(CRfcommSAP& aSAP, TUint8 aSignals)=0;// Modem status command
 	virtual void RLS(CRfcommSAP& aSAP, TUint8 aStatus)=0; // Remote line status
	// data	
	virtual void NewData(CRfcommSAP& aSAP, const TDesC8& aData)=0;
	virtual void NotifyNewDataCallback(CRfcommSAP& aSAP)=0;
	virtual void CanSend(CRfcommSAP& aSAP)=0;
	virtual void Error(CRfcommSAP& aSAP, TInt aErr,
					   CRfcommSAP::TErrorTypes aType)=0;
//TRY_CBFC
	virtual TUint8 FreeCredit(CRfcommSAP& aSAP)=0;
	virtual TInt ProxyForRemoteCredit(const CRfcommSAP& aSAP) const =0;
	virtual void SetProxyForRemoteCredit(CRfcommSAP& aSAP, TInt aCredit)=0;
	virtual void ProxyForRemoteCreditDecrement(CRfcommSAP& aSAP)=0;
	virtual void ProxyForRemoteCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit)=0;
	virtual TInt LocalCredit(const CRfcommSAP& aSAP) const =0;
	virtual void SetInitialLocalCredit(CRfcommSAP& aSAP, TInt aCredit)=0;
	virtual void LocalCreditDecrement(CRfcommSAP& aSAP)=0;
	virtual void LocalCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit)=0;
	// Notification for cloned SAPs waiting for connection.
	virtual void ParentClosed(CRfcommSAP& aSAP)=0;
	
	// Notifications from sent frames waiting for a response.
	virtual TBool HandleFrameResponseTimeout(CRfcommSAP& aSAP)=0;

protected:
	// Utility Error function
	TInt SignalError(CRfcommSAP& aSAP, TInt aErr, MSocketNotify::TOperationBitmasks aType);

	// State change utility function
	void ChangeState(CRfcommSAP& aSAP, CRfcommStateFactory::TRfcommStates aNewState) const;
	
	// State panicking functions
	void PanicInState(TRFCOMMPanic aPanic) const;
	void DebugPanicInState(TRFCOMMPanic aPanic) const;

protected:
	// Data
	CRfcommStateFactory& iFactory;  // Get states
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
private:
    // Forbid copying
	TRfcommState(const TRfcommState&);
	const TRfcommState& operator=(const TRfcommState&);
	};

#ifdef __FLOG_ACTIVE
#define STATENAME(x) iName=_L(x)
#else
#define STATENAME(x)
#endif

/**
   A state class that provides default behaviours.
   
   This state provides some default behaviours so that other states
   can simply derive from it and thus pick up a set of default
   behaviours.  This reduces the coding burden for other states.

   For events originating within the Socket, the default behaviour is
   to panic as derived classes should be implementing the correct
   functionality.  For events coming in from the Muxer, we don't
   assume that the derived class will be expecting this event (since
   it has essentially come from the remote end).  Thus the default
   behaviour is to place the SAP into an error state, as this is the
   fail-safe state when the remote end sends us an unexpected event.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateDefault) : public TRfcommState
 	{
public:
	TRfcommStateDefault(CRfcommStateFactory& aFactory);

	// Normal state changes
	virtual void Enter(CRfcommSAP& aSAP);
	virtual void Exit(CRfcommSAP& aSAP);

	// State changes initiated by the SAP. Default is to Panic.
	virtual void ActiveOpen(CRfcommSAP&  aSAP);
	virtual TInt PassiveOpen(CRfcommSAP&  aSAP, TUint aQueSize);
	virtual void Shutdown(CRfcommSAP&  aSAP);
	virtual void FastShutdown(CRfcommSAP&  aSAP);
	virtual TInt Send(CRfcommSAP& aSAP, const TDesC8& aData);
	virtual void Read(CRfcommSAP& aSAP, TDesC8& aData);
	virtual void Ioctl(CRfcommSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption);
	virtual void CancelIoctl(CRfcommSAP& aSAP, TUint aLevel, TUint aName);
	virtual void Start(CRfcommSAP& aSAP);
	virtual TInt SetOption(CRfcommSAP& aSAP, TUint level, TUint name, const TDesC8& aOption);

	// From the signaller, go to error state
	virtual void MuxUp(CRfcommSAP& aSAP);
	virtual void LinkDown(CRfcommSAP& aSAP);
	virtual void SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI);	//	SABM for listening SAP
	virtual void DISC(CRfcommSAP& aSAP);
	virtual void UA(CRfcommSAP& aSAP);
	virtual void DM(CRfcommSAP& aSAP);
	virtual void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI);
	virtual void RPNRsp(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction);
	virtual void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI);

	virtual void PNResp(CRfcommSAP& aSAP, TRfcommPortParams& aParams);
	virtual void MSC(CRfcommSAP& aSAP, TUint8 aSignals);
	virtual void RLS(CRfcommSAP& aSAP, TUint8 aStatus);
	virtual void NewData(CRfcommSAP& aSAP, const TDesC8& aData);
	virtual void NotifyNewDataCallback(CRfcommSAP& aSAP);
	virtual void CanSend(CRfcommSAP& aSAP);
	virtual void IoctlComplete(CRfcommSAP& aSAP, TInt aErr, TUint aLevel,TUint aName, TDesC8* aBuf);
	virtual void AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult);
	virtual void Error(CRfcommSAP& aSAP, TInt aErr, CRfcommSAP::TErrorTypes aType);

	// Notification for cloned SAPs waiting for connection.
	virtual void ParentClosed(CRfcommSAP& aSAP);

	// Notifications from sent frames waiting for a response.
	virtual TBool HandleFrameResponseTimeout(CRfcommSAP& aSAP);
//TRY_CBFC
	virtual TUint8 FreeCredit(CRfcommSAP& aSAP);
	virtual TInt ProxyForRemoteCredit(const CRfcommSAP& aSAP) const;
	virtual void SetProxyForRemoteCredit(CRfcommSAP& aSAP, TInt aCredit);
	virtual void ProxyForRemoteCreditDecrement(CRfcommSAP& aSAP);
	virtual void ProxyForRemoteCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit);
	virtual TInt LocalCredit(const CRfcommSAP& aSAP) const;
	virtual void SetInitialLocalCredit(CRfcommSAP& aSAP, TInt aCredit);
	virtual void LocalCreditDecrement(CRfcommSAP& aSAP);
	virtual void LocalCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit);

protected:
	
	};

/**
   The error state.

   All operations are errors, and cause error signals to be sent to
   the SAP.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateError) : public TRfcommStateDefault
	{
public:
	TRfcommStateError(CRfcommStateFactory& aFactory);
	virtual void Enter(CRfcommSAP& aSAP);
	// State changes initiated by the SAP
	virtual void ActiveOpen(CRfcommSAP& aSAP);
	virtual TInt PassiveOpen(CRfcommSAP& aSAP, TUint /*aQueSize*/);
	virtual void Shutdown(CRfcommSAP& aSAP);
	virtual void FastShutdown(CRfcommSAP& aSAP);
	virtual TInt Send(CRfcommSAP& aSAP, const TDesC8&);
	virtual void Read(CRfcommSAP& aSAP, TDesC8&);
	virtual void Ioctl(CRfcommSAP& aSAP, TUint, TUint, TDes8*);
	virtual void CancelIoctl(CRfcommSAP& aSAP, TUint, TUint);
	virtual void Start(CRfcommSAP& aSAP);
	};

//
// The interesting states start here
//

/**
   Closed - the initial state.

	This state is the initial state of the SAP, as well as the end
	state when a connection is disconnected.  A SAP in this state
	cannot be assumed to have valid remote or local addresses in
	place.

	The invariant on this state is that it will not have a muxer.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateClosed) : public TRfcommStateDefault
	{
public:
	TRfcommStateClosed(CRfcommStateFactory& aFactory);
	virtual void Enter(CRfcommSAP& aSAP);
	// State changes initiated by the SAP
	void ActiveOpen(CRfcommSAP& aSAP);
	TInt PassiveOpen(CRfcommSAP& aSAP, TUint aQueSize);
	void Shutdown(CRfcommSAP&  aSAP);	
	void Error(CRfcommSAP& aSAP, TInt aErr,
					   CRfcommSAP::TErrorTypes aType);
	void Start(CRfcommSAP& aSAP);
	TInt SetOption(CRfcommSAP& aSAP, TUint aLevel, TUint aName, const TDesC8 &aOption);
	};

/**
   A superstate class that provides default behaviours for connecting states.

   Connecting states are the states between closed and open when the
   local side is doing an active open.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateConnecting) : public TRfcommStateDefault
 	{
public:
	TRfcommStateConnecting(CRfcommStateFactory& aFactory);

	// State changes initiated by the SAP. Default is to Panic.
	virtual void Shutdown(CRfcommSAP&  aSAP);
	virtual void FastShutdown(CRfcommSAP&  aSAP);

	virtual void DM(CRfcommSAP& aSAP);

	virtual void LinkDown(CRfcommSAP& aSAP);
protected:
	//	Used to issue appropriate notifications to SAP when
	//	connection fails (CanClose or SignalError)
	virtual void FailureWhileConnecting(CRfcommSAP& aSAP, TInt aErrorCode);
	};

/**
   Waiting for the muxer.
   
   When the protocol is asked to find the muxer for a SAP, it will
   either find an existing one or bring up a link, create a muxer and
   then use the new one.  Either way, eventually the MuxUp event will
   be generated.
   Once the Mux is up, we then need to send a SABM command.
   Note that a LinkDown event can occur in this state.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateWaitForMux) : public TRfcommStateConnecting
	{
public:
	TRfcommStateWaitForMux(CRfcommStateFactory& aFactory);
	void Enter(CRfcommSAP& aSAP);
	void Shutdown(CRfcommSAP& aSAP);
	void SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI);	
	void UA(CRfcommSAP& aSAP);
	void DISC(CRfcommSAP& aSAP);
	void DM(CRfcommSAP& aSAP);
	// mux channel commands
	// Remote parameter negotiation
	void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI);
	void RPNRsp(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction);
	// Parameter negotiation
	void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI);  
	void PNResp(CRfcommSAP& aSAP, TRfcommPortParams& aParams);
	void MSC(CRfcommSAP& aSAP, TUint8 aSignals);// Modem status command
 	void RLS(CRfcommSAP& aSAP, TUint8 aStatus); // Remote line status
	// data	
	void NewData(CRfcommSAP& aSAP, const TDesC8& aData);

	// State change from the Mux
	void MuxUp(CRfcommSAP& aSAP);
	};

/**
	Waiting for Parameter Negotiation response.

   On successful response, we contine with the connection. On unsuccessful
   response, we signal a socket error and return to the Closed state.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateWaitForPNResp) : public TRfcommStateConnecting
	{
public:
	TRfcommStateWaitForPNResp(CRfcommStateFactory& aFactory);
	void Enter(CRfcommSAP& aSAP);
	// State change from the Mux
	void PNResp(CRfcommSAP& aSAP, TRfcommPortParams& aParams);
	};


/**
   Wait For UA state (subclass of Connecting state).

   State in which we are waiting for a reponse to our SABM command.

   On successful connect we send an MSC (modem status command) and
   enter the open state.

   Private
**/
NONSHARABLE_CLASS(TRfcommStateWaitForUA) : public TRfcommStateConnecting
	{
public:
	TRfcommStateWaitForUA(CRfcommStateFactory& aFactory);
	// mux events
	void UA(CRfcommSAP& aSAP);

	// Notifications from sent frames waiting for a response.
	TBool HandleFrameResponseTimeout(CRfcommSAP& aSAP);
	};


/**
   Waiting for the outgoing security check to complete.

   This is the state while we wait for the SecMan to give a go/no-go
   on this connection.
**/
NONSHARABLE_CLASS(TRfcommStateOutgoingSecurityCheck) : public TRfcommStateConnecting
	{
public:
	TRfcommStateOutgoingSecurityCheck(CRfcommStateFactory& aFactory);
	// mux events
	void Enter(CRfcommSAP& aSAP);

	void AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult);
	void Exit(CRfcommSAP& aSAP);
	};


/**
	Listening State

	State that the SAP enters following a PassiveOpen.
   
    Private
**/

NONSHARABLE_CLASS(TRfcommStateListening) : public TRfcommStateDefault
	{
public:
	TRfcommStateListening(CRfcommStateFactory& aFactory);
	void Enter(CRfcommSAP& aSAP);
	void Exit(CRfcommSAP& aSAP);
	void SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI);
	void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI);
	void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI);
	void Shutdown(CRfcommSAP& aSAP);
	void FastShutdown(CRfcommSAP& aSAP);
	TInt SetOption(CRfcommSAP& aSAP, TUint aLevel, TUint aName, const TDesC8 &aOption);
private:
	CRfcommSAP* AttemptToClone(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI);
	};

/**
   Base state for cloned saps before they are taken over by ESOCK.

   This base state handles a bunch of the error handling for the
   derived states.
**/

NONSHARABLE_CLASS(TRfcommStateCloned) : public TRfcommStateDefault
	{
public:
	TRfcommStateCloned(CRfcommStateFactory& aFactory);
	void LinkDown(CRfcommSAP& aSAP);
	void Error(CRfcommSAP& aSAP, TInt aErr,
			   CRfcommSAP::TErrorTypes aType);
	void DISC(CRfcommSAP& aSAP);
	void DM(CRfcommSAP& aSAP);
	};	

/**
	The Wait For SABM state.

	The state that cloned SAPs start off in before receiving
	a PN or SABM.
   
    Private
**/
NONSHARABLE_CLASS(TRfcommStateWaitForSABM) : public TRfcommStateCloned
	{
public:
	TRfcommStateWaitForSABM(CRfcommStateFactory& aFactory);
	void SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI);
	void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, 
			 CRfcommMuxer& aMux, TUint8 aDLCI);
	void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI);
	void MuxUp(CRfcommSAP& aSAP);
	};

/**
   Waiting for the incoming security check to complete.

   This is the state while we wait for the SecMan to give a go/no-go
   on this connection.  We will respond to SABM with a DM if the
   answer is no.
**/
NONSHARABLE_CLASS(TRfcommStateIncomingSecurityCheck) : public TRfcommStateCloned
	{
public:
	TRfcommStateIncomingSecurityCheck(CRfcommStateFactory& aFactory);
	void Enter(CRfcommSAP& aSAP);
	void AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult);
	void Exit(CRfcommSAP& aSAP);
	};

/**
	The Wait For Start state.

	The state that cloned SAPs arrive in after receiving a SABM
   
    Private
**/
NONSHARABLE_CLASS(TRfcommStateWaitForStart) : public TRfcommStateDefault
	{
public:
	TRfcommStateWaitForStart(CRfcommStateFactory& aFactory);
	void Enter(CRfcommSAP& aSAP);
	void Start(CRfcommSAP& aSAP);
	void DISC(CRfcommSAP& aSAP);
	void Error(CRfcommSAP& aSAP, TInt aErr,
				CRfcommSAP::TErrorTypes aType);
	void Exit(CRfcommSAP& aSAP);
	};

/**
	The Close On Start state

	State arrived at when a disconnect is sent to a SAP which is
	waiting for a Start. A SAP in this state will disconnect 
	immediately that a Start is called on it.
   
    Private
**/
NONSHARABLE_CLASS(TRfcommStateCloseOnStart) : public TRfcommStateDefault
	{
public:
	TRfcommStateCloseOnStart(CRfcommStateFactory& aFactory);
	void Start(CRfcommSAP& aSAP);
	void Error(CRfcommSAP& aSAP, TInt aErr,
				CRfcommSAP::TErrorTypes aType);
	};

/**
   The open state.

   The channel is connected and the data can now flow.
   
   Private
**/

NONSHARABLE_CLASS(TRfcommStateOpen) : public TRfcommStateDefault
	{
public:
	TRfcommStateOpen(CRfcommStateFactory& aFactory);

	void Enter(CRfcommSAP& aSAP);
	void PerformDataAwareStateChange(CRfcommSAP& aSAP);
	// From ESOCK
	void Exit(CRfcommSAP& aSAP);
	void Shutdown(CRfcommSAP& aSAP);
	void FastShutdown(CRfcommSAP& aSAP);
	TInt Send(CRfcommSAP& aSAP, const TDesC8& aData);
	void Read(CRfcommSAP& aSAP, TDesC8& aData);
	void Ioctl(CRfcommSAP& aSAP,TUint aLevel,TUint aName,TDes8* aOption);

	// From mux
	void NewData(CRfcommSAP& aSAP, const TDesC8& aData);
	void NotifyNewDataCallback(CRfcommSAP& aSAP);
	void CanSend(CRfcommSAP& aSAP);
	void IoctlComplete(CRfcommSAP& aSAP, TInt aErr, TUint aLevel,TUint aName, TDesC8* aBuf);
	void MSC(CRfcommSAP& aSAP, TUint8 aSignals);
	void RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI);
	void RPNRsp(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction);
	void PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI);
	void RLS(CRfcommSAP& aSAP, TUint8 /*aStatus*/);
	void DISC(CRfcommSAP& aSAP);
	void DM(CRfcommSAP& aSAP);
	void LinkDown(CRfcommSAP& aSAP);
	TInt SetOption(CRfcommSAP &aSAP, TUint aLevel, TUint aName, const TDesC8& aOption);
//TRY_CBFC
	TUint8 FreeCredit(CRfcommSAP& aSAP);

	virtual TInt ProxyForRemoteCredit(const CRfcommSAP& aSAP) const;
#ifndef __GCC32__
	using TRfcommStateDefault::ProxyForRemoteCredit; //unhide other overloads;
#endif
	virtual TInt LocalCredit(const CRfcommSAP& aSAP) const;
#ifndef __GCC32__
	using TRfcommStateDefault::LocalCredit; //unhide other overloads;
#endif
	};

/**
   The disconnect state.

   This is for a SAP on its way to being shutdown.
   
   Private
**/


NONSHARABLE_CLASS(TRfcommStateDisconnect) : public TRfcommStateDefault
	{
public:
	TRfcommStateDisconnect(CRfcommStateFactory& aFactory);

	void Enter(CRfcommSAP& aSAP);
	void DISC(CRfcommSAP& aSAP);
	void UA(CRfcommSAP& aSAP);
	void DM(CRfcommSAP& aSAP);
	void LinkDown(CRfcommSAP& aSAP);
private:
	//	Code common to UA, DM and DISC
	void DisconnectComplete(CRfcommSAP& aSAP);
	};

/**
   Disconnecting state.
   
   Here the remote end has shut down the DLCI, but we have still got
   data to send up to the socket.  We wait for the data to drain
   before calling Disconnect().  If the client tries to write, we
   immediately signal Disconnect() as we are no longer able to write.
   After Disconnect() we move to state closed.
   
   In this state we are not associated with a mux, as we no longer
   want events from the link.
   
   Private
**/
NONSHARABLE_CLASS(TRfcommStateDisconnecting) : public TRfcommStateDefault
	{
public:
	TRfcommStateDisconnecting(CRfcommStateFactory& aFactory);

	void Enter(CRfcommSAP& aSAP);
	void Shutdown(CRfcommSAP& aSAP);
	void FastShutdown(CRfcommSAP& aSAP);
	TInt Send(CRfcommSAP& aSAP, const TDesC8& aData);
	void Read(CRfcommSAP& aSAP, TDesC8& aData);
	void Ioctl(CRfcommSAP& aSAP,TUint aLevel,TUint aName,TDes8* aOption);
	void NotifyNewDataCallback(CRfcommSAP& aSAP);
	
private:
	};


#include "rfcommstates.inl"
#endif

