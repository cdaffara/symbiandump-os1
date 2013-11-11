// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RFCOMMMUXCHANNEL_H
#define RFCOMMMUXCHANNEL_H

#include <e32base.h>
#include "debug.h"
#include "rfcommframe.h"
#include "rfcommmuxer.h"
#include "rfcommutil.h"

class CMuxChannelState;
class TMuxChannelState;

/**
   Factory for the Mux channel states

   The states are flyweight classes
**/
NONSHARABLE_CLASS(CMuxChannelStateFactory) : public CBase
	{
public:
	static CMuxChannelStateFactory* NewL();
	~CMuxChannelStateFactory();
	enum TChannelState
		{
		EClosed,
		EWaitForLink,
		EError,
		ELinkUp,
		EWaitForSABMResp,
		EOpen,
		EClosing,
	// *** keep next one last ***
		ERfcommChannelMaxState,
		};
	
	TMuxChannelState* GetState(TChannelState aState);
	TInt StateIndex(const TMuxChannelState* aState) const;
private:
	void ConstructL();
	TFixedArray<TMuxChannelState*, ERfcommChannelMaxState> iStates;
	};

/**
   Manages the mux channel (DLCI 0) and the L2CAP link

   Before an RFCOMM session can be used, a L2CAP link to the remote
   host must be established and the multiplexer control channel must
   be brought up.  When the RFCOMM session is no longer required, or
   when the remote end wishes to close things down, the control
   channel and the L2CAP link must be terminated.

   As both these operations have multiple steps and interesting
   failure modes, a state machine is implemented to control this.
**/
NONSHARABLE_CLASS(CRfcommMuxChannel) : public CBase
	{
friend class TMuxChannelState;
friend class TMuxChannelStateClosed;
friend class TMuxChannelStateWaitForLink;
friend class TMuxChannelStateError;
friend class TMuxChannelStateConnected;
friend class TMuxChannelStateLinkUp;
friend class TMuxChannelStateWaitForSABMResp;
friend class TMuxChannelStateOpen;
friend class TMuxChannelStateClosing;

public:
	CRfcommMuxChannel(CMuxChannelStateFactory& aFactory, CRfcommMuxer& aMux,
		CServProviderBase& aSAP, CMuxChannelStateFactory::TChannelState aInitialState);
	~CRfcommMuxChannel();

	// Requests from the muxer
	void SetAddress(TBTDevAddr& aAddr);
	void Open();
	void Close();
	TBool IsOpen();
	TBool IsErrored();
	TInt MaxDataSize();

	// Rfcomm events
	void UA();
	void DISC();
	void DM();
	void PN(TBool aCommand, TRfcommPortParams& aParams);
	void SABM();
	void FrameTimeout(CRfcommFrame* aFrm);

	// L2CAP events (passed on from the mux)
	void Disconnect();
	void CanClose();
	void ConnectComplete();
	void Error(TInt aError,TUint aOperationMask);
	TBool CanAttachSAP();

private:
	TInt TransmitSABM();
	TInt TransmitUA();
	TInt TransmitDM();
	TInt TransmitPN(TBool aCommand, const TRfcommPortParams& aParams);
	void QueIdleTimer(TInt aDelay);
	void DequeIdleTimer();
	static TInt IdleTimerCallback(TAny*);
	
	CServProviderBase& iSAP;
	CRfcommMuxer& iMux;
	TMuxChannelState* iState;
	TBool iClosePending;
	TBool iOpenPending;
	TDeltaTimerEntry iIdleTimer;
	TBool iIdleTimerQueued;
	TInt iMaxDataSize;
	};


/**
   Base class for mux channel states.

   This implements a basic set of behaviours for all states that can
   then be overridden.  All the states are flyweight classes.
**/
NONSHARABLE_CLASS(TMuxChannelState)
	{
public:
	TMuxChannelState(CMuxChannelStateFactory& aFactory);

	virtual void Enter(CRfcommMuxChannel& aContext, TBool aDisconnectingIdleTimer = ETrue);
	virtual void Open(CRfcommMuxChannel& aContext);
	virtual void Close(CRfcommMuxChannel& aContext);
	virtual TBool IsOpen(CRfcommMuxChannel& aContext);
	virtual TBool IsErrored(CRfcommMuxChannel& aContext);
	virtual void UA(CRfcommMuxChannel& aContext);
	virtual void DISC(CRfcommMuxChannel& aContext);
	virtual void DM(CRfcommMuxChannel& aContext);
	virtual void PN(CRfcommMuxChannel& aContext,
					TBool aCommand, TRfcommPortParams&
					aParams);
	virtual void SABM(CRfcommMuxChannel& aContext);
	virtual void FrameTimeout(CRfcommMuxChannel& aContext,
							  CRfcommFrame* aFrm);
	virtual void Disconnect(CRfcommMuxChannel& aContext);
	virtual void CanClose(CRfcommMuxChannel& aContext);
	virtual void ConnectComplete(CRfcommMuxChannel& aContext);
	virtual void Error(CRfcommMuxChannel& aContext, TInt aError,
					   TUint aOperationMask);
	virtual void IdleTimeout(CRfcommMuxChannel& aContext);
	virtual TBool CanAttachSAP();

protected:
	// Sets the state, but doesn't enter it.
	void SetState(CRfcommMuxChannel& aContext, CMuxChannelStateFactory::TChannelState aState);
	void PanicInState(TRFCOMMPanic aPanic) const;
	void DebugPanicInState(TRFCOMMPanic aPanic) const;
	
protected:
	CMuxChannelStateFactory& iFactory;
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif
	};

/**
   Closed state.

   This is the state we start in when this end is to initiate the L2CAP link.

   On an Open() we do the active connect.

**/
NONSHARABLE_CLASS(TMuxChannelStateClosed) : public TMuxChannelState
	{
public:
	TMuxChannelStateClosed(CMuxChannelStateFactory& aFactory);
	
	void Enter(CRfcommMuxChannel& aContext, TBool aDisconnectingIdleTimer = ETrue);
	void Open(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	};

/**
   Waiting for the L2CAP link to come up.
**/
NONSHARABLE_CLASS(TMuxChannelStateWaitForLink) : public TMuxChannelState
	{
public:
	TMuxChannelStateWaitForLink(CMuxChannelStateFactory& aFactory);
	
	void ConnectComplete(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	void Error(CRfcommMuxChannel& aContext, TInt aError,
			   TUint aOperationMask);
	};

/**
   The Error state

   This represents a fatal error on the L2CAP link (SAP) such that it
   would be pointless to attempt to reconnect the channel with the
   same SAP.

   Entry to this state should be terminal to our associated muxer.
**/
NONSHARABLE_CLASS(TMuxChannelStateError) : public TMuxChannelState
	{
public:
	TMuxChannelStateError(CMuxChannelStateFactory& aFactory);
	
	void Open(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	TBool CanAttachSAP();
	TBool IsErrored(CRfcommMuxChannel& aContext);
	};

/**
   Base class for all the connected states.

   This implements the connected superstate, with the superstate
   behaviours.  It also includes some default behaviours for its
   substates.

   The connected state is when the L2CAP link is up.
**/
NONSHARABLE_CLASS(TMuxChannelStateConnected) : public TMuxChannelState
	{
public:
	TMuxChannelStateConnected(CMuxChannelStateFactory& aFactory);

	void FrameTimeoutHelper(CRfcommMuxChannel& aContext);

	// These functions are over-ridden from TMuxChannelState 
	void Disconnect(CRfcommMuxChannel& aContext);

	// ...and these will also be over-ridden in child states
	virtual void SABM(CRfcommMuxChannel& aContext);
	virtual void FrameTimeout(CRfcommMuxChannel& aContext,
							  CRfcommFrame* aFrm);
	virtual void Error(CRfcommMuxChannel& aContext, TInt aError,
					   TUint aOperationMask);
	};

/**
   The L2CAP link is up
   
   This class is entered when the link comes up, or when the mux
   channel is disconnected without closing the l2cap link.  From here,
   a timeout will close the link.

   This is the initial state for the mux channel if the muxer was
   created due to an incoming connection.

   From here, the SABM/UA exchange occurs to bring up the mux channel,
   and the DISC/UA to return to here.  Eventually a timeout, link
   disconnection or Close() will cause the link to be dropped.
**/
NONSHARABLE_CLASS(TMuxChannelStateLinkUp) : public TMuxChannelStateConnected
	{
public:
	TMuxChannelStateLinkUp(CMuxChannelStateFactory& aFactory);
	
	// Over-ridden from parent class:
	void Enter(CRfcommMuxChannel& aContext, TBool aDisconnectingIdleTimer = ETrue);
	void Open(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	void SABM(CRfcommMuxChannel& aContext);
	void DISC(CRfcommMuxChannel& aContext);
	void IdleTimeout(CRfcommMuxChannel& aContext);
	void FrameTimeout(CRfcommMuxChannel& aContext, CRfcommFrame* aFrm);
	};

/**
   Waiting for the mux channel to be connected
**/
NONSHARABLE_CLASS(TMuxChannelStateWaitForSABMResp) : public TMuxChannelStateConnected
	{
public:
	TMuxChannelStateWaitForSABMResp(CMuxChannelStateFactory& aFactory);
	
	void UA(CRfcommMuxChannel& aContext);
	void DM(CRfcommMuxChannel& aContext);
	void SABM(CRfcommMuxChannel& aContext);
	void DISC(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	};

/**
   The mux channel is now fully open

   At this point the muxer is signalled that it can start to use the
   channel.
   
**/
NONSHARABLE_CLASS(TMuxChannelStateOpen) : public TMuxChannelStateConnected
	{
public:
	TMuxChannelStateOpen(CMuxChannelStateFactory& aFactory);
	
	void Enter(CRfcommMuxChannel& aContext, TBool aDisconnectingIdleTimer = ETrue);
	TBool IsOpen(CRfcommMuxChannel& aContext);
	void Close(CRfcommMuxChannel& aContext);
	void SABM(CRfcommMuxChannel& aContext);
	void DISC(CRfcommMuxChannel& aContext);
	};

/**
   Closing down the L2CAP Link

   The whole mux channel is no longer required, so the link is being
   closed.
**/
NONSHARABLE_CLASS(TMuxChannelStateClosing) : public TMuxChannelStateConnected
	{
public:
	TMuxChannelStateClosing(CMuxChannelStateFactory& aFactory);
	// Over-ridden from parent:
	void FrameTimeout(CRfcommMuxChannel& aContext,
							  CRfcommFrame* aFrm);
	void CanClose(CRfcommMuxChannel& aContext);
	TBool CanAttachSAP();
	};

#ifdef __FLOGGING__
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif

#include "rfcommmuxchannel.inl"
#endif
