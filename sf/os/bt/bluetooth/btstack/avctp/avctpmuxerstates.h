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
// Defines classes to implement each state a Avctp Muxer can
// be in.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVCTPTRANSPORTSTATES_H
#define AVCTPTRANSPORTSTATES_H

#include <e32base.h>
#include <bluetoothav.h>

// when rationalising logging, will not need this to get FLOG_ACTIVE MACRO in this header
#include <comms-infras/commsdebugutility.h>


#include "avctputils.h"

// Forward definitions
class TAvctpMuxerState;
class RMBufChain;

/**
The state factory for AVCTP muxer states

  @internalComponent
*/
NONSHARABLE_CLASS(CAvctpMuxerStateFactory) : public CBase
	{
friend class CAvctpProtocol; // So only the protocol can new up a state factory

public:
	/** Index of the Muxer states */
	enum TAvctpMuxerStates
		{
		/** No link, prior to establishing the link*/
		EClosed = 0,
		/** Waiting for the lower protocol link */
		ELinkPending = 1,
		/** The lower protocol link has been established */
		EOpen = 2,
		/** The second lower protocol link is opening */
		ESecondLinkPending = 3,
		/** Both lower protocol links have been established */
		EFullyOpen = 4,
		/** Provides convenient way of numbering the states */
		EMaxStates = 5,
		};
	
	TAvctpMuxerState& GetState(const TAvctpMuxerStates aState) const;
	TInt StateIndex(const TAvctpMuxerState* aState) const;

	~CAvctpMuxerStateFactory();

private:
	static CAvctpMuxerStateFactory* NewL();
	CAvctpMuxerStateFactory();

private:
	/** The lightweight Muxer states */
	TFixedArray<TAvctpMuxerState*, EMaxStates> iStates;
	};

// Forward Declarations
class CAvctpTransport;
class TBTDevAddr;
class MSubConnectionControlClient;

/**
The base class for all the state classes in the AVCTP muxer state
pattern.  These classes act as the state classes in the pattern,
with the Muxer being the context. This class provides the default 
implementation for the states.

All states are flyweight classes managed by a State factory, so
this class also has a pointer to the global factory singleton.

There are four categories of events represented as pure virtual
functions.  These are:

1.	"Artificial" state events
2.	Events coming in from the muxer's control clients.
3.	Events coming in from the CAvctpSaps.
4.	Events coming from below, via the L2CAP SAP.

We don't panic on any events coming from below since these could be the
result of defective remote behaviour and we don't want to give them a 
means of bringing down our device. 

Events coming from above we do panic if they are incorrect since any
errors that reach this point are the result of programming errors.
This is because CAvctpSap, CAvctpSapLinksMgr shield the muxer from
direct interaction with other components / clients.

For debugging purposes, a state name is implemented in derived classes 
to allow state changes to be logged.

This class defines basic default behaviour for all the events that
a state responds to.

Derived classes implement the events that they care about.

@internalComponent
*/
NONSHARABLE_CLASS(TAvctpMuxerState)
	{
protected:
	TAvctpMuxerState(CAvctpMuxerStateFactory& aFactory);
public:
	
	// "Artificial" state events
	virtual void Enter(CAvctpTransport& aTransport) const;
	virtual void Exit(CAvctpTransport& aTransport) const;

	virtual TBool IsIdle(CAvctpTransport& aTransport) const;

	virtual void IdleTimerExpired(CAvctpTransport& aTransport) const;

	virtual TInt Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 aClientId) const;
	virtual TInt StartIncoming(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, CServProviderBase* aL2CAPConSAP) const;
	virtual TInt AddSecondChannel(CAvctpTransport& aTransport, CServProviderBase& aSAP) const;
	virtual void RemoveSecondChannel(CAvctpTransport& aTransport) const;
	virtual TInt SecondChannelRemoved(CAvctpTransport& aTransport) const;
	virtual TInt CancelConnectRequest(CAvctpTransport& aTransport, TUint16 aClientId) const;
	
	// Events coming in on the user plane.
	virtual void Shutdown(CAvctpTransport& aTransport, TInt aError) const;
	
	// Events coming from below, via the L2CAP SAP.  These are
	// essentially state transitions caused by the remote end.
	virtual void Disconnect(CAvctpTransport& aTransport) const;
	virtual void ConnectComplete(CAvctpTransport& aTransport) const;
	virtual void Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const;
	virtual TInt NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const;
	virtual void CanSend(CAvctpTransport& aTransport, TInt aChannel) const;
	
#ifdef __FLOG_ACTIVE
	TBuf<48> iName;
#endif //__FLOG_ACTIVE

protected:
	// State change utility function
	void ChangeState(CAvctpTransport& aTransport, CAvctpMuxerStateFactory::TAvctpMuxerStates aNewState) const;
	void PanicInState(SymbianAvctp::TPanic aPanic) const;
	void ShutDownSecondarySap(CAvctpTransport& aTransport, TBool aNotifyClient) const;
private:
    // Forbid copying
	TAvctpMuxerState(const TAvctpMuxerState&);
	const TAvctpMuxerState& operator=(const TAvctpMuxerState&) const;

private:
	/** The state factory, used to change states */
	CAvctpMuxerStateFactory& iFactory;
	};

/**
The closed state.

This is the state that newly created saps start in, and the state
they end in after a communication has closed down or a connection
failed.

Basically this is a quiescent state from which the Muxer can go to
different futures though currently it commits suicide :] Note that
though the delete attempts to be asynchronous, it's not guaranteed
so it's best not to do anything with the muxer after changing state
to closed.

  @internalComponent
*/
NONSHARABLE_CLASS(TAvctpStateClosed) : public TAvctpMuxerState
	{
public:
	TAvctpStateClosed(CAvctpMuxerStateFactory& aFactory);
	
	// "Artificial" state events
	virtual void Enter(CAvctpTransport& aTransport) const;
	virtual void Exit(CAvctpTransport& aTransport) const;
	virtual void IdleTimerExpired(CAvctpTransport& aTransport) const;
	
	virtual TInt Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 aClientId) const;
	// Events coming in on the control plane.
	virtual TInt StartIncoming(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, CServProviderBase* aL2CAPConSAP) const;
	
	};

  
/**
The link pending state.

Waiting for the L2CAP link to be created.

  @internalComponent
*/
NONSHARABLE_CLASS(TAvctpStateLinkPending) : public TAvctpMuxerState
 	{
public:
	TAvctpStateLinkPending(CAvctpMuxerStateFactory& aFactory);	

	// "Artificial" state events
	virtual void Enter(CAvctpTransport& aTransport) const;

	virtual TBool IsIdle(CAvctpTransport& aTransport) const;
	
	// Events coming in on the control plane.
	virtual TInt CancelConnectRequest(CAvctpTransport& aTransport, TUint16 aClientId) const;
	
	// Events coming from below, via the L2CAP SAP.  These are
	// essentially state transitions caused by the remote end.
	virtual void ConnectComplete(CAvctpTransport& aTransport) const;
	virtual void Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const;
	};


/**
The open state.

We have an open L2CAP link with the remote device

  @internalComponent
*/
NONSHARABLE_CLASS(TAvctpStateOpen) : public TAvctpMuxerState
	{
public:
	TAvctpStateOpen(CAvctpMuxerStateFactory& aFactory);

	// "Artificial" state events
	virtual void Enter(CAvctpTransport& aTransport) const;
	virtual void Exit(CAvctpTransport& aTransport) const;

	virtual TInt Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 aClientId) const;
	virtual TInt AddSecondChannel(CAvctpTransport& aTransport, CServProviderBase& aSAP) const;
	
	// Events coming in on the user plane.
	virtual void Shutdown(CAvctpTransport& aTransport, TInt aError) const;

	// Events coming from below, via the L2CAP SAP.  These are
	// essentially state transitions caused by the remote end.
	virtual void Disconnect(CAvctpTransport& aTransport) const;
	virtual void Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const;
	virtual TInt NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const;
	virtual void CanSend(CAvctpTransport& aTransport, TInt aChannel) const;
	};


/**
The Second channel pending state.

  @internalComponent
*/
NONSHARABLE_CLASS(TAvctpStateSecondChannelPending) : public TAvctpStateOpen
	{
public:
	TAvctpStateSecondChannelPending(CAvctpMuxerStateFactory& aFactory);
	
	virtual void Enter(CAvctpTransport& aTransport) const;
	virtual void Exit(CAvctpTransport& aTransport) const;
	virtual TInt Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 aClientId) const;

	// Events coming in on the user plane.
	virtual void Shutdown(CAvctpTransport& aTransport, TInt aError) const;
	
	// Events coming from below, via the L2CAP SAP.  These are
	// essentially state transitions caused by the remote end.
	virtual void Disconnect(CAvctpTransport& aTransport) const;
	virtual void Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const;
	virtual TInt NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const;
	virtual void CanSend(CAvctpTransport& aTransport, TInt aChannel) const;
	virtual void ConnectComplete(CAvctpTransport& aTransport) const;
	virtual void RemoveSecondChannel(CAvctpTransport& aTransport) const;
	};

	
/**
The fully open state.

We have an open L2CAP link with the remote device

  @internalComponent
*/
NONSHARABLE_CLASS(TAvctpStateFullyOpen) : public TAvctpStateOpen
	{
public:
	TAvctpStateFullyOpen(CAvctpMuxerStateFactory& aFactory);

	// "Artificial" state events
	virtual void Enter(CAvctpTransport& aTransport) const;
	virtual void Exit(CAvctpTransport& aTransport) const;
	
	virtual TInt Start(CAvctpTransport& aTransport, const TBTDevAddr& aAddr, TUint16 aClientId) const;
	virtual void RemoveSecondChannel(CAvctpTransport& aTransport) const;
	virtual TInt SecondChannelRemoved(CAvctpTransport& aTransport) const;
	
	// Events coming in on the user plane.
	virtual void Shutdown(CAvctpTransport& aTransport, TInt aError) const;

	// Events coming from below, via the L2CAP SAP.  These are
	// essentially state transitions caused by the remote end.
	virtual void Disconnect(CAvctpTransport& aTransport) const;
	virtual void Error(CAvctpTransport& aTransport, TInt aError,TUint aOperationMask, TInt aChannel) const;
	virtual TInt NewData(CAvctpTransport& aTransport, TUint aMtu, TInt aChannel) const;
	virtual void CanSend(CAvctpTransport& aTransport, TInt aChannel) const;
	};
	
	
#ifdef __FLOG_ACTIVE
#define STATENAME(x)  iName=_L(x)
#else
#define STATENAME(x)
#endif

#endif // AVCTPTRANSPORTSTATES_H 

