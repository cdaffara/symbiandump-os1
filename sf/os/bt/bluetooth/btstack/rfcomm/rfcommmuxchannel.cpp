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
// Runs the mux control channel for the Rfcomm muxer
// 
//

#include <bluetooth/logger.h>
#include "rfcommmuxchannel.h"
#include "rfcommconsts.h"
#include "rfcommutil.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

CRfcommMuxChannel::CRfcommMuxChannel(CMuxChannelStateFactory& aFact, CRfcommMuxer& aMux,
									 CServProviderBase& aSAP, CMuxChannelStateFactory::TChannelState aInitialState)
	: iSAP(aSAP),
	  iMux(aMux)
	{
	iState=aFact.GetState(aInitialState);
 	iState->Enter(*this, EFalse);
   	TCallBack cb(IdleTimerCallback, this);
   	iIdleTimer.Set(cb);
	}

CRfcommMuxChannel::~CRfcommMuxChannel()
	{
	DequeIdleTimer();
	}

void CRfcommMuxChannel::SetAddress(TBTDevAddr& aAddr)
	{
	TL2CAPSockAddr remote;
	iSAP.AutoBind();
	remote.SetBTAddr(aAddr);
	remote.SetPort(KRFCOMMPSM);  // 0x03 normally...
	if(iSAP.SetRemName(remote)!=KErrNone)
		{
		Panic(ERfcommErrorSettingAddress);
		}
	}

void CRfcommMuxChannel::QueIdleTimer(TInt aDelay)
	{
	if(!iIdleTimerQueued)
		{
		BTSocketTimer::Queue(aDelay, iIdleTimer);
		iIdleTimerQueued=ETrue;
		}
	}

void CRfcommMuxChannel::DequeIdleTimer()
	{
	if(iIdleTimerQueued)
		{
		BTSocketTimer::Remove(iIdleTimer);
		iIdleTimerQueued=EFalse;
		}
	}

TInt CRfcommMuxChannel::IdleTimerCallback(TAny* aChannel)
	{
	CRfcommMuxChannel* channel=static_cast<CRfcommMuxChannel*>(aChannel);
	channel->iIdleTimerQueued=EFalse;
	LOG(_L("RFCOMM: Mux channel idle timer callback"));
	channel->iState->IdleTimeout(*channel);
	return EFalse;
	}

/** 
 This lets the protocol know whether this mux is able to be attached to.
 Sometimes it will be irrevocably committed to going down and so is not
 valid to be attached to.
 @return TBool Whether it's ok to attach a SAP
 */
TBool CRfcommMuxChannel::CanAttachSAP()
	{
	// Let the state use its context to work out if we're going down
	return iState->CanAttachSAP();
	}

/*
  State factory
*/

CMuxChannelStateFactory* CMuxChannelStateFactory::NewL()
	{
	CMuxChannelStateFactory* ret=new (ELeave) CMuxChannelStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

void CMuxChannelStateFactory::ConstructL()
	{
	iStates[EClosed]=new (ELeave) TMuxChannelStateClosed(*this);
	iStates[EWaitForLink]=new (ELeave) TMuxChannelStateWaitForLink(*this);
	iStates[EError]= new (ELeave) TMuxChannelStateError(*this);
	iStates[ELinkUp]=new (ELeave) TMuxChannelStateLinkUp(*this);
	iStates[EWaitForSABMResp]=new (ELeave) TMuxChannelStateWaitForSABMResp(*this);
	iStates[EOpen]=new (ELeave) TMuxChannelStateOpen(*this);
	iStates[EClosing]=new (ELeave) TMuxChannelStateClosing(*this);
	}

CMuxChannelStateFactory::~CMuxChannelStateFactory()
	{
	iStates.DeleteAll();
	}

TMuxChannelState* CMuxChannelStateFactory::GetState(TChannelState aState)
	{
	__ASSERT_DEBUG(aState != ERfcommChannelMaxState, Panic(ERfCommMuxerStateOutOfBounds));
	return iStates[aState];
	}
	
TInt CMuxChannelStateFactory::StateIndex(const TMuxChannelState* aState) const
	{
	TInt state;
	for (state = 0; state < ERfcommChannelMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}


/*
  The base state class
*/

void TMuxChannelState::SetState(CRfcommMuxChannel& aContext, CMuxChannelStateFactory::TChannelState aState)
	{
#ifdef __FLOG_ACTIVE
	TMuxChannelState* state=iFactory.GetState(aState);
	LOG2(_L("RFCOMM: MuxChannel : State %S -> %S"),
				  &aContext.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
#ifdef _DEBUG
	TMuxChannelState* st=iFactory.GetState(aState);
	__ASSERT_DEBUG(st!=aContext.iState, PanicInState(ERfcommMuxChannelStateChangeToSelf));
#endif
	aContext.iState=iFactory.GetState(aState);
	}

TMuxChannelState::TMuxChannelState(CMuxChannelStateFactory& aFactory)
	: iFactory(aFactory)
	{
	}

void TMuxChannelState::PanicInState(TRFCOMMPanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}

#ifdef _DEBUG
void TMuxChannelState::DebugPanicInState(TRFCOMMPanic aPanic) const
#else
void TMuxChannelState::DebugPanicInState(TRFCOMMPanic /*aPanic*/) const
#endif
	{
	#ifdef _DEBUG
	PanicInState(aPanic);
	#endif
	}

void TMuxChannelState::Enter(CRfcommMuxChannel& aContext, TBool /*aDisconnectingIdleTimer*/)
	/**
	   Called when entering a state
	**/
	{
	aContext.DequeIdleTimer(); //ENTERED NEW STATE => NOT IDLE
	}

void TMuxChannelState::Open(CRfcommMuxChannel& aContext)
	/**
	   A request to bring up the mux channel
	**/
	{
	aContext.iOpenPending=ETrue;
	aContext.iClosePending=EFalse;
	}

void TMuxChannelState::Close(CRfcommMuxChannel& aContext)
	/**
	   Close the mux channel down
	**/
	{
	aContext.iClosePending=ETrue;
	aContext.iOpenPending=EFalse;
	}

TBool TMuxChannelState::IsOpen(CRfcommMuxChannel& /*aContext*/)
	{
	return EFalse;
	}
	
TBool TMuxChannelState::IsErrored(CRfcommMuxChannel& /*aContext*/)
	{
	return EFalse;
	}	

void TMuxChannelState::UA(CRfcommMuxChannel& /*aContext*/)
	/**
	   A UA frame for DLCI zero has been received
	**/
	{
	// Do nothing
	}

void TMuxChannelState::DISC(CRfcommMuxChannel& /*aContext*/)
	/**
	   A DISC frame for DLCI zero has been received
	**/
	{
	// Do nothing
	}

void TMuxChannelState::DM(CRfcommMuxChannel& /*aContext*/)
	/**
	   A DM frame for DLCI zero has been received
	**/
	{
	// Do nothing
	}

void TMuxChannelState::PN(CRfcommMuxChannel& aContext,
						  TBool aCommand, TRfcommPortParams& /*aParams*/)
	/**
	   A PN command or response for DLCI zero has been received
	**/
	{
	if(aCommand)
		{
	//	We're at liberty to ignore this, since PNs are not allowed 
	//	on DLCI 0. However, we can send back a PN response with a 
	//	MTU of 127 (the default, which we can guarantee).
	//
	//	NB. Default TRfcommPortParams constructor sets default MTU (127)
	//
		TRfcommPortParams muxParams;
		muxParams.iInitialCredit=0;	//DS	improved by making 0 default?
		aContext.TransmitPN(EFalse, muxParams);
		}
	else
		{
		//	Should never happen
		LOG(_L("RFCOMM:  Received a PN response on Mux channel!"));
		}

	}

void TMuxChannelState::SABM(CRfcommMuxChannel& /*aContext*/)
	/**
		A SABM has been received for DLCI 0
	**/
	{
	//do nothing
	}

void TMuxChannelState::FrameTimeout(CRfcommMuxChannel& /*aContext*/,
									CRfcommFrame* /*aFrm*/)
	/**
	   A ctrl or mux frame has timed out before a response was received.

	   Generally this is not a good thing.
	**/
	{
	DebugPanicInState(ERfcommChannelIdleTimeout);
	}

void TMuxChannelState::Disconnect(CRfcommMuxChannel& /*aContext*/)
	/**
	   A disconnect indication has come in from L2CAP
	**/
	{
	// Do nothing
	}

void TMuxChannelState::CanClose(CRfcommMuxChannel& /*aContext*/)
	/**
	   L2CAP is happy for us to delete the sap
	**/
	{
	// Do nothing
	}

void TMuxChannelState::ConnectComplete(CRfcommMuxChannel& /*aContext*/)
	/**
	   The L2CAP connect has completed
	**/
	{
	// Do nothing
	}

void TMuxChannelState::Error(CRfcommMuxChannel& /*aContext*/, TInt /*aError*/,
							 TUint /*anOperationMask*/)
	/**
	   An error on the L2CAP sap has occurred
	**/
	{
	// Do nothing
	}

void TMuxChannelState::IdleTimeout(CRfcommMuxChannel& /*aContext*/)
	/**
	   The idle timer has gone off.
	**/
	{
	DebugPanicInState(ERfcommChannelIdleTimeout);
	}

TBool TMuxChannelState::CanAttachSAP()
	{
	return ETrue;
	}

/*
  Closed
*/

TMuxChannelStateClosed::TMuxChannelStateClosed(CMuxChannelStateFactory& aFactory)
	: TMuxChannelState(aFactory)
	{
	STATENAME("Closed");
	}

void TMuxChannelStateClosed::Enter(CRfcommMuxChannel& aContext, TBool /*aDisconnectingIdleTimer*/)
	/**
	   Reset everything
	**/
	{
	aContext.iOpenPending=EFalse;
	aContext.iClosePending=EFalse;
	aContext.DequeIdleTimer();
	}

void TMuxChannelStateClosed::Open(CRfcommMuxChannel& aContext)
	/**
	   Open up the L2CAP link
	**/
	{
	// Set opening\closing flags
	TMuxChannelState::Open(aContext);
	// Make sure the PSM is in place, rather than an old CID
	TL2CAPSockAddr remote;
	aContext.iSAP.RemName(remote);
	remote.SetPort(KRFCOMMPSM);
	
	TBTServiceSecurity ssec;
	ssec.SetAuthentication(EMitmNotRequired);
	ssec.SetAuthorisation(EFalse);
	ssec.SetEncryption(EFalse);
	ssec.SetDenied(EFalse);
	
	remote.SetSecurity(ssec);

	aContext.iSAP.SetRemName(remote);
	SetState(aContext,CMuxChannelStateFactory::EWaitForLink);
	aContext.iSAP.ActiveOpen();
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateClosed::Close(CRfcommMuxChannel& aContext)
	{
	//	No need to do anything - call back synchronously
	aContext.iMux.MuxChannelClosed();
	}

/*
  WaitForLink - waiting for the link to come up
*/

TMuxChannelStateWaitForLink::TMuxChannelStateWaitForLink(CMuxChannelStateFactory& aFactory)
	: TMuxChannelState(aFactory)
	{
	STATENAME("WaitForLink");
	}

void TMuxChannelStateWaitForLink::ConnectComplete(CRfcommMuxChannel& aContext)
	/**
	   The link has come up

	   Move to the start state in the connected superstate (LinkUp)
	**/
	{
	SetState(aContext,CMuxChannelStateFactory::ELinkUp);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateWaitForLink::Close(CRfcommMuxChannel& aContext)
	/**
	   The mux is no longer wanted.

	   Cancel the pending opening so we'll just timeout and die in
	   state LinkUp, or the link won't come up so we'll drop to
	   closed.
	 **/
	{
	aContext.iOpenPending=EFalse;
	}

void TMuxChannelStateWaitForLink::Error(CRfcommMuxChannel& aContext, TInt aError,
										TUint aOperationMask)
	/**
	   The connection failed to come up
	**/
	{
	if(aOperationMask | MSocketNotify::EErrorFatal)
		{
		// Things are very bad, so drop to the error state
		SetState(aContext,CMuxChannelStateFactory::EError);
		aContext.iMux.MuxChannelError(ETrue, aError);
		}
	else if(aOperationMask | MSocketNotify::EErrorConnect)
		{
		// This connect failed, so we'd better go to the Closed state
		SetState(aContext,CMuxChannelStateFactory::EClosed);
		aContext.iMux.MuxChannelError(EFalse, aError);
		}
	else
		{
		// We have an error that probably should be ignored
		LOG(_L("RFCOMM:  **** Muxchannel wait for link, ignoring error ****"));
		}
	aContext.iState->Enter(aContext);
	}

/*
  Error
*/

TMuxChannelStateError::TMuxChannelStateError(CMuxChannelStateFactory& aFactory)
	: TMuxChannelState(aFactory)
	{
	STATENAME("Error");
	}

void TMuxChannelStateError::Open(CRfcommMuxChannel& /*aContext*/)
	{
	PanicInState(ERfcommChannelError);
	}

void TMuxChannelStateError::Close(CRfcommMuxChannel& aContext)
	{
	// tell the muxer now since there'll be no transition from this state
	aContext.iMux.MuxChannelClosed();
	}

TBool TMuxChannelStateError::CanAttachSAP()
	{
	return EFalse;
	}

TBool TMuxChannelStateError::IsErrored(CRfcommMuxChannel& /*aContext*/)
	{
	return ETrue;
	}

/*
  Connected.  Super state for several states
*/


TMuxChannelStateConnected::TMuxChannelStateConnected(CMuxChannelStateFactory& aFactory)
	: TMuxChannelState(aFactory)
	{
	STATENAME("Connected");
	}

void TMuxChannelStateConnected::SABM(CRfcommMuxChannel& aContext)
	/**
		A SABM has been received for DLCI 0
		We are not in one of the connected states that expects a SABM
		Attempt to tell the other side that we are not in a fit state.
	**/
	{
	LOG(_L("RFCOMM: sending NACK on unexpected SABM"));
	aContext.TransmitDM();
	}

void TMuxChannelStateConnected::FrameTimeout(CRfcommMuxChannel& aContext, CRfcommFrame* /*aFrm*/)
	/**
	   A frame has failed to elicit a response

	   This is bad for the mux channel, so we go to LinkUp.  The muxer
	   will take care of the frame..
	**/
	{
	// Do the basics...
	FrameTimeoutHelper(aContext);

	//... and then go to ELinkUp
	SetState(aContext,CMuxChannelStateFactory::ELinkUp);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateConnected::FrameTimeoutHelper(CRfcommMuxChannel& aContext)
/**
	Helper function for code re-use in FrameTimeout methods
**/
	{
	aContext.iOpenPending=EFalse;
	aContext.iClosePending=EFalse;
	aContext.iMux.MuxChannelError(EFalse, KErrRfcommFrameResponseTimeout);
	}

void  TMuxChannelStateConnected::Disconnect(CRfcommMuxChannel& aContext)
	/**
	   The other end has disconnected the L2CAP link
	**/
	{
	aContext.DequeIdleTimer();
	SetState(aContext,CMuxChannelStateFactory::EClosed);
	aContext.iState->Enter(aContext);
	aContext.iMux.MuxChannelDown();
	aContext.iMux.MuxChannelClosed();
	}

void TMuxChannelStateConnected::Error(CRfcommMuxChannel& aContext, TInt aError,
									  TUint aOperationMask)
	/**
	   Summ'ts up at t'mill!

	   Check the op mask - if it's only Ioctl then we ignore it, else
	   something bad is wrong.
	   
	   Clear the link timer as well in case we are in LinkUp.
	**/
	{
	if(aOperationMask != MSocketNotify::EErrorIoctl)
		{
		aContext.DequeIdleTimer();
		SetState(aContext,CMuxChannelStateFactory::EError);
		aContext.iMux.MuxChannelError(ETrue, aError);
		aContext.iState->Enter(aContext);
		}
	}

/*
  Now the meaty ones...

  LinkUp - the l2cap link is up, and maybe we need to bring up the mux channel...
*/

TMuxChannelStateLinkUp::TMuxChannelStateLinkUp(CMuxChannelStateFactory& aFactory)
	: TMuxChannelStateConnected(aFactory)
	{
	STATENAME("LinkUp");
	}

void  TMuxChannelStateLinkUp::Enter(CRfcommMuxChannel& aContext, TBool aDisconnectingIdleTimer)
	/**
	   Entered the state

       Start to bring up the link if a Open is pending, or bring it
	   down if a Close is pending, or just remain link up and kick
	   off a timer if neither is true.
 	   
 	   Find out the L2CAP MTU as this is now important.
 	**/
   	{
   	__ASSERT_DEBUG(!(aContext.iOpenPending && aContext.iClosePending), 
   		PanicInState(ERfcommMuxChannelOpeningAndClosing));
   	
   	aContext.DequeIdleTimer(); //ENTERED NEW STATE => NOT IDLE

	TPckgBuf<TInt> buf;
	
	// Find out what the max data size is.
	aContext.iSAP.GetOption(KSolBtL2CAP, KL2CAPInboundMTU, buf);
	TInt t = buf();
	aContext.iSAP.GetOption(KSolBtL2CAP, KL2CAPOutboundMTUForBestPerformance, buf);
	// Max size is the lower of incoming and outgoing
	// L2CAP MTUs since it's symmetrical
	aContext.iMaxDataSize=Min(buf(), t);
 
	if(aContext.iOpenPending)
		{
		aContext.iOpenPending=EFalse;
		TInt err=aContext.TransmitSABM();
		if(err != KErrNone)
			{
			SetState(aContext,CMuxChannelStateFactory::EError);
			aContext.iMux.MuxChannelError(ETrue, err);
			}
		else
			{
			aContext.iState=
				iFactory.GetState(CMuxChannelStateFactory::EWaitForSABMResp);
			}
		aContext.iState->Enter(aContext);
		}
	else if(aContext.iClosePending)
		{
		SetState(aContext,CMuxChannelStateFactory::EClosing);
		aContext.iSAP.Shutdown(CServProviderBase::ENormal);
		aContext.iState->Enter(aContext);
		}
	else
		{
		if(aDisconnectingIdleTimer)
			{
			// Timeout (nominally 1 sec) to prevent a DoS attack from an out-of-sequence PN
			aContext.QueIdleTimer(KRfcommMuxDisconnectingChannelTimeout);
			}
		else
			{
			// We start the idle timer (nominally 10 secs). This is a countdown
			// which will be cancelled when we receive a SABM from the remote.
			// Otherwise the connection is closed to avoid sapping our battery.
			aContext.QueIdleTimer(KRfcommMuxConnectingChannelTimeout);
			}
		}
	}

void TMuxChannelStateLinkUp::Close(CRfcommMuxChannel& aContext)
	/**
	   We've been explicitly asked to close down.
	   
	   We should now bring down the link.  We can signal that the mux
	   channel is closed immediately.
	**/
	{
	aContext.iMux.MuxChannelDown();
	SetState(aContext,CMuxChannelStateFactory::EClosing);
	aContext.iSAP.Shutdown(CServProviderBase::ENormal);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateLinkUp::Open(CRfcommMuxChannel& aContext)
	/**
	   Time to bring up that channel
	**/
	{
	aContext.DequeIdleTimer();
	aContext.iOpenPending=EFalse;
	TInt err=aContext.TransmitSABM();
	if(err != KErrNone)
		{
		SetState(aContext,CMuxChannelStateFactory::EError);
		aContext.iMux.MuxChannelError(ETrue, err);
		}
	else
		{
		SetState(aContext,CMuxChannelStateFactory::EWaitForSABMResp);
		}
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateLinkUp::SABM(CRfcommMuxChannel& aContext)
	/**
		We've received a SABM on DLCI 0.

		Respond with a UA, and move to the Open state.
	**/
	{
	TInt err=aContext.TransmitUA();
	if(err!=KErrNone)
		{
		// We're unable to respond, so we need to error stuff
		SetState(aContext,CMuxChannelStateFactory::EError);
		aContext.iMux.MuxChannelError(ETrue, err);
		}
	else
		{
		aContext.DequeIdleTimer();
		SetState(aContext,CMuxChannelStateFactory::EOpen);
		}
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateLinkUp::DISC(CRfcommMuxChannel& /*aContext*/)
 	/**
 	   We've received a DISC on DLCI 0.
 	**/
 	{
 	// Do nothing
 	}

void TMuxChannelStateLinkUp::IdleTimeout(CRfcommMuxChannel& aContext)
	/**
	   We've been idle long enough.  Bring it down...
	**/
	{
	LOG(_L("RFCOMM: Shutting down mux channel link"));
	SetState(aContext,CMuxChannelStateFactory::EClosing);
	aContext.iSAP.Shutdown(CServProviderBase::ENormal);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateLinkUp::FrameTimeout(CRfcommMuxChannel& aContext, CRfcommFrame* /*aFrm*/)
/**
	Needs to be over-ridden as parent 'Connected' state does too much!

	FIXME - Yeah, I know it's horrible but short of re-writing most of the 
	mux channel	states, what else can we do?
**/
	{
	// Do the basics...
	FrameTimeoutHelper(aContext);

	//... and don't go to ELinkUp 'cos we're already there.
	}

/*
  Wait for UA to the SABM
*/

TMuxChannelStateWaitForSABMResp::TMuxChannelStateWaitForSABMResp(CMuxChannelStateFactory& aFactory)
	: TMuxChannelStateConnected(aFactory)
	{
	STATENAME("WaitForSABMResp");
	}

void TMuxChannelStateWaitForSABMResp::UA(CRfcommMuxChannel& aContext)
	/**
	   The mux channel is up, we're done
	**/
	{
	SetState(aContext,CMuxChannelStateFactory::EOpen);
	aContext.iState->Enter(aContext);
	}
	   
void TMuxChannelStateWaitForSABMResp::DM(CRfcommMuxChannel& aContext)
	/**
	   Our SABM has failed.
	   
	   It would be a good idea to rip up this muxer now, since the other end
	   is very unhappy.
	**/
	{
	SetState(aContext,CMuxChannelStateFactory::EError);
	aContext.iMux.MuxChannelError(ETrue, KErrCouldNotConnect);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateWaitForSABMResp::SABM(CRfcommMuxChannel& aContext)
	/**
	   Our SABMs have passed in the post.

	   It's not clear whether the sender of the SABM gets to be
	   initiator or not, we assume that it's the creator of the L2CAP
	   channel which gets that honour.  Since others may assume
	   otherwise, the safest thing to do here is to rip up this mux
	   and wait for it to go round again.
	**/
	{
	SetState(aContext,CMuxChannelStateFactory::EError);
	aContext.iMux.MuxChannelError(ETrue, KErrCouldNotConnect);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateWaitForSABMResp::Close(CRfcommMuxChannel& aContext)
	{
	//	We need to give up...pretend we never sent the SABM
	aContext.iClosePending = ETrue;
	aContext.iOpenPending = EFalse;
	SetState(aContext,CMuxChannelStateFactory::ELinkUp);
	aContext.iState->Enter(aContext);
	}

void TMuxChannelStateWaitForSABMResp::DISC(CRfcommMuxChannel& aContext)
 	/**
 	   DISC received. 	   
 	**/
 	{
 	SetState(aContext,CMuxChannelStateFactory::EError);
 	aContext.iMux.MuxChannelError(ETrue, KErrCouldNotConnect);
 	aContext.iState->Enter(aContext);
 	}

/*
  Open - all systems are go.
*/

TMuxChannelStateOpen::TMuxChannelStateOpen(CMuxChannelStateFactory& aFactory)
	: TMuxChannelStateConnected(aFactory)
	{
	STATENAME("Open");
	}

void TMuxChannelStateOpen::Enter(CRfcommMuxChannel& aContext, TBool /*aDisconnectingIdleTimer*/)
	/**
	   Entering the Open state.

	   Let the mux know, unless we're supposed to be closing.
	**/
	{
	aContext.iOpenPending=EFalse; // Since we're open now!
	if(!aContext.iClosePending)
		{
		aContext.iMux.MuxChannelUp();
		}
	else
		{
		// We're going down again!
		Close(aContext);
		}
	}

void TMuxChannelStateOpen::Close(CRfcommMuxChannel& aContext)
	/**
	   Time's up.  Go on down.
	**/
	{
	aContext.iClosePending=ETrue;
	SetState(aContext,CMuxChannelStateFactory::ELinkUp);
	aContext.iState->Enter(aContext);
	}

/**
A SABM has been received for DLCI 0.
We're already in Open state- the remote may have sent another SABM because we 
replied to the first SABM later than they expected.
All we can reasonably do is reply with a UA. Just dropping the SABM might 
result in a timeout on the other end. 
We need to override the SABM method to avoid the base class implementation's 
panic.
*/
void TMuxChannelStateOpen::SABM(CRfcommMuxChannel& aContext)
	{
	static_cast<void>(aContext.TransmitUA());
	// We don't care about the error here.
	// If the UA succeeded, we're already in the right state so no further 
	// action is required.
	// If the UA failed, it doesn't matter as we're already in the right state 
	// and the connection already exists from the original SABM. If the remote 
	// times out the SABM it sent and consequently wants to pull down the 
	// connection, that's their business. Anyway, there's nothing we can do 
	// about it.
	}

void TMuxChannelStateOpen::DISC(CRfcommMuxChannel& aContext)
	/**
	   Remote end wants us to shut down
	**/
	{
	aContext.TransmitUA();  // May fail
	SetState(aContext,CMuxChannelStateFactory::ELinkUp);
	aContext.iMux.CloseSAPs();
	aContext.iState->Enter(aContext);
	}

TBool TMuxChannelStateOpen::IsOpen(CRfcommMuxChannel& /*aContext*/)
	{
	return ETrue;
	}




/*
  Closing down the channel
*/

TMuxChannelStateClosing::TMuxChannelStateClosing(CMuxChannelStateFactory& aFactory)
	: TMuxChannelStateConnected(aFactory)
	{
	STATENAME("Closing");
	}

void TMuxChannelStateClosing::FrameTimeout(CRfcommMuxChannel& aContext, CRfcommFrame* /*aFrm*/)
/**
	Needs to be over-ridden as parent 'Connected' state does too much!

	FIXME - Yeah, I know it's horrible but short of re-writing most of the mux channel
	states, what else can we do?
**/
	{
	// Do the basics...
	FrameTimeoutHelper(aContext);

	//... and don't go to ELinkUp ('cos we're taking the link down!)
	}

void TMuxChannelStateClosing::CanClose(CRfcommMuxChannel& aContext)
	/**
	   The socket is closed down now - we can move back to closed.
	   
	   We tear up the muxer at this point, since the creator of the l2cap
	   link is the one who acts as initiator for RFCOMM dlci assignment.
	   This is configured on mux creation, so we can't go back from here to
	   link up without changing it.
	**/
	{
	SetState(aContext,CMuxChannelStateFactory::EClosed);
	aContext.iState->Enter(aContext);
	aContext.iMux.MuxChannelClosed();
	}

TBool TMuxChannelStateClosing::CanAttachSAP()
	{
	return EFalse;
	}

