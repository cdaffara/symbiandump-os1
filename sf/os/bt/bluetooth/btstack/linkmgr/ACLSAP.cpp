// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// ACL.cpp
// Implementation of ACL Logical links
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <bluetooth/hci/aclpacketconsts.h>
#include <bluetooth/hci/hciconsts.h>

#include "ACLSAP.h"
#include "physicallinks.h"
#include "physicallinksmanager.h"
#include "Basebandmodel.h"
#include "linkmgr.h"
#include "AclDataQController.h"
#include "linkconsts.h"
#include "linkutil.h"

#include "hcifacade.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#pragma warning (disable: 4100) //unreference formal param

CACLLinkStateFactory* CACLLinkStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CACLLinkStateFactory* ret=new (ELeave) CACLLinkStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

void CACLLinkStateFactory::ConstructL()
	{
	LOG_FUNC
	iStates[EClosed]				=new (ELeave) TACLLinkStateClosed(*this);
	iStates[EListening]				=new (ELeave) TACLLinkStateListening(*this);
	iStates[EAccepting]				=new (ELeave) TACLLinkStateAccepting(*this);
	iStates[EWaitForLink]			=new (ELeave) TACLLinkStateWaitForLink(*this);
	iStates[EWaitForStart]			=new (ELeave) TACLLinkStateWaitForStart(*this);
	iStates[EWaitForStartError]		=new (ELeave) TACLLinkStateWaitForStartError(*this);
	iStates[EWaitForStartParked]	=new (ELeave) TACLLinkStateWaitForStartParked(*this);
	iStates[EOpen]					=new (ELeave) TACLLinkStateOpen(*this);
	iStates[EOpenParked]			=new (ELeave) TACLLinkStateOpenParked(*this);
	iStates[EClosing]				=new (ELeave) TACLLinkStateClosing(*this);
	iStates[EAcceptingClosing]		=new (ELeave) TACLLinkStateAcceptingClosing(*this);
	}

CACLLinkStateFactory::~CACLLinkStateFactory()
	{
	LOG_FUNC
	iStates.DeleteAll();
	}

TACLLinkState& CACLLinkStateFactory::GetState(CACLLinkStateFactory::TACLLinkStates aState)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EACLLinkMaxState, Panic(EACLLinkStateOutOfBounds));
	return *iStates[aState];
	}

TInt CACLLinkStateFactory::StateIndex(const TACLLinkState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < EACLLinkMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}

TACLLinkState::TACLLinkState(CACLLinkStateFactory& aFactory)
: iFactory(aFactory)
	{
	LOG_FUNC
	}
	
void TACLLinkState::PanicInState(TLinkPanic aPanic) const
	{
	LOG_FUNC
	Panic(aPanic, iFactory.StateIndex(this));
	}

void TACLLinkState::ChangeState(CACLLink& aContext, CACLLinkStateFactory::TACLLinkStates aState) const
	{
	LOG_FUNC
	aContext.iState->Exit(aContext);

#ifdef __FLOG_ACTIVE
	TACLLinkState* state=&iFactory.GetState(aState);
	LOG2(_L("ACLLink: State %S -> %S"), &aContext.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
	aContext.iState=&iFactory.GetState(aState);

	aContext.iState->Enter(aContext);
	}


void TACLLinkState::Enter(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TACLLinkState::Exit(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// do nothing
	}

void TACLLinkState::Deletion(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

void TACLLinkState::ActiveOpen(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

void TACLLinkState::DataReceived(CACLLink& /*aContext*/, TUint8 /*aFlag*/, const TDesC8& /*aData*/) const
	{
	LOG_FUNC
	__DEBUGGER(); // drop, but see why went wrong
	}

void TACLLinkState::Start(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent); //Panics since ESOCK and our state machine might be out of sync
	}

void TACLLinkState::ConnectComplete(CACLLink& /*aContext*/, const TBTConnect& /*aConnect*/) const
	{
	LOG_FUNC
	//Drop the event silently in UREL
 	#ifdef _DEBUG
	PanicInState(EBTACLSAPUnexpectedEvent);
	#endif
	}

TBool TACLLinkState::ConnectRequest(CACLLink& /*aContext*/,
									  const TBTConnect& /*aConnectDetails*/,
									  const CPhysicalLink& /*aPhysicalLink*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	return EFalse;
	}

void TACLLinkState::Disconnection(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

void TACLLinkState::Error(CACLLink& /*aContext*/, TInt /*aError*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

void TACLLinkState::Timeout(CACLLink& /*aContext*/, TBasebandTimeout /*aTimeout*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

void TACLLinkState::Shutdown(CACLLink& /*aContext*/, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	}

TInt TACLLinkState::PassiveOpen(CACLLink& /*aContext*/, TUint /*aQueSize*/) const
	{
	LOG_FUNC
	PanicInState(EBTACLSAPUnexpectedEvent);
	return KErrGeneral;
	}

TUint TACLLinkState::Write(CACLLink& /*aContext*/, const TDesC8& /*aData*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/) const
	{
	LOG_FUNC
	LOG(_L("CACLLink: Write in base state - Dropping data..."));
	return 0;
	}

TBool TACLLinkState::IsIdle() const
	{
	LOG_FUNC
	return EFalse;
	}

void TACLLinkState::Park(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// ignore
	}

void TACLLinkState::Unpark(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// ignore
	}

//----------------------------------------------------------------------------------

TACLLinkStateListening::TACLLinkStateListening(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Listening");
	}

void TACLLinkStateListening::Enter(CACLLink& aContext) const
	{
	LOG_FUNC
	TInt err = aContext.iLinksMan.AddListener(aContext, EACLLink);
	if (err)
		{
		__ASSERT_DEBUG(err!=KErrInUse, PanicInState(EBTACLAlreadyBoundToPhysicalLink));
		aContext.iSocket->Error(err, MSocketNotify::EErrorFatal);
		ChangeState(aContext, CACLLinkStateFactory::EClosed);
		}
	}


TBool TACLLinkStateListening::ConnectRequest(CACLLink& aContext,
											 const TBTConnect& /*aConnectDetails*/,
											 const CPhysicalLink& aPhysicalLink) const
	{
	LOG_FUNC										
	// ah! we have a connection
	/*	super - now BT1.1/1.2 says we don't need to do anything - 
	ACL is here for free: with same handle as PhysicalLink.
	We do however need to introduce ourselves to ACL Q
	*/
	LOG(_L("CACLLink: Physical Channel Up - Spawning..."));

	// clone the sap - we only support *one* child at present since
	// we dont expose ACL sockets to ESOCK - only to L2CAP who will 
	// only attempt to Start one child at a time
	TRAPD(err, SpawnL(aContext, const_cast<CPhysicalLink&>(aPhysicalLink)));

	return (err==KErrNone);
	}


void TACLLinkStateListening::SpawnL(CACLLink& aContext, CPhysicalLink& aPhysicalLink) const
/**
	Called when a PassiveSAP wants to create a (connected) child of itself
**/
	{
	LOG_FUNC
	//Listening SAP only keeps one pointer to accepting SAP so we 
	//can't have more than one outstanding accepting SAP
	if(aContext.iChild)
		{
		LOG(_L("TACLLinkStateListening::SpawnL: there is already an outstanding accepting SAP, can't spawn now"));
		User::Leave(KErrInUse);
		}

	CACLLink* child = CACLLink::NewLC(aContext.iLinksMan,
									  &aPhysicalLink,
									  aContext.iProtocol);
	
	User::LeaveIfError(child->BindLink(EACLLink, aPhysicalLink));
	CleanupStack::Pop(child);

	// tell child about parent
	child->ListeningSAP() = &aContext;

	// and parent about child
	aContext.iChild = child;

	// configure the SAP
	child->iRemoteDev = aPhysicalLink.BDAddr();
	child->iLocalPort = aContext.iLocalPort;

	// transition for child is Closed->WaitForStart
	ChangeState(*child, CACLLinkStateFactory::EAccepting);
	}

void TACLLinkStateListening::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG_FUNC
	ChangeState(aContext, CACLLinkStateFactory::EClosed);

	if(aContext.iChild)
		{
		//As soon as this method returns, this SAP could be deleted.
		//Notify the child.
		aContext.iChild->Shutdown(CServProviderBase::EImmediate);

		//If the child was in EAccepting state, the shutdown will move it to EClosing. It
		//will still be bound to PHY, and when the ConnectComplete arrives the PHY will
		//still route it to the child. If the parent has been deleted by then, the child
		//will cause a panic. The best thing to do is delete the child now.
		//N.B. It is important to Shutdown the child before deleting it, because if it
		//was in EAccepting state calling Shutdown stops its watchdog.
		aContext.DeleteChild(aContext.iChild);
		aContext.iChild=NULL; //Not sure if this is needed, but it'll do no harm.
		}

	if (aCloseType == CServProviderBase::ENormal)
		{
		aContext.Socket()->CanClose();
		}
	}

void TACLLinkStateListening::Deletion(CACLLink& aContext) const
	{
	LOG_FUNC
	delete aContext.iChild;
	}

void TACLLinkStateListening::Exit(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.iLinksMan.RemoveListener(aContext);
	}

void TACLLinkStateListening::Error(CACLLink& /*aContext*/, TInt __DEBUG_ONLY(aError)) const
	{
	LOG_FUNC
	// Don't propagate the error as listeners don't care if the hardware has gone down
	__ASSERT_DEBUG(aError==KErrHardwareNotAvailable, PanicInState(EBTACLSAPUnexpectedEvent));
	}

//----------------------------------------------------------------------------------

TACLLinkStateAccepting::TACLLinkStateAccepting(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Accepting");
	}

void TACLLinkStateAccepting::Enter(CACLLink& aContext) const
	{
	LOG_FUNC
	// start watchdog
	aContext.iAcceptWatchdog.Start();
	// If the link is accepting then it must be being remotely initiated.
	aContext.SetInitiationState(CACLLink::ERemotelyInitiated);
	if(aContext.iLinksMan.IsAcceptPairedOnlyMode())
		{
		// Start an access request for appropriate security - this will start as
		// the physical link is established.
		aContext.StartAccessRequest(*(aContext.ListeningSAP()));
		}
	}

void TACLLinkStateAccepting::Exit(CACLLink& aContext) const
 	{
	LOG_FUNC
 	//Ensure watchdog has been cancelled (safe to call Cancel multiple times)
 	aContext.iAcceptWatchdog.Cancel();
 	}

void TACLLinkStateAccepting::ConnectComplete(CACLLink& aContext, const TBTConnect& /*aConnect*/) const
	{
	LOG_FUNC
	// cancel watchdog
	aContext.iAcceptWatchdog.Cancel();

	// must wait until started
	ChangeState(aContext, CACLLinkStateFactory::EWaitForStart);
	// let the listening Socket know...
	__ASSERT_DEBUG(aContext.ListeningSAP()->Socket(), PanicInState(EBTACLSAPNullSocket));
	aContext.ListeningSAP()->Socket()->ConnectComplete(aContext);	// tell listening socket about its new SAP
	}

void TACLLinkStateAccepting::Timeout(CACLLink& aContext, TBasebandTimeout aTimeout) const
	{
	LOG_FUNC
	if (aTimeout == EAccept)
		{
		// the accept never worked - give up
		Error(aContext, KErrTimedOut);
		}
	}

void TACLLinkStateAccepting::Deletion(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TACLLinkStateAccepting::Disconnection(CACLLink& aContext) const
	{
	LOG_FUNC
	// This will be called if there is a hardware error - give up
	Error(aContext, KErrDisconnected);
	}

void TACLLinkStateAccepting::Error(CACLLink& aContext, TInt /*aError*/) const
	{
	LOG_FUNC
	// unbind from physical link
	aContext.UnbindLink(EACLLink);
	// cancel watchdog
	aContext.iAcceptWatchdog.Cancel();
	aContext.ListeningSAP()->DeleteChild(&aContext);
	}

void TACLLinkStateAccepting::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// we're not wanted - close
	ChangeState(aContext, CACLLinkStateFactory::EAcceptingClosing);
	}

//----------------------------------------------------------------------------------


TACLLinkStateWaitForStart::TACLLinkStateWaitForStart(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("WaitForStart");
	}

void TACLLinkStateWaitForStart::Enter(CACLLink& __DEBUG_ONLY(aContext)) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aContext.ListeningSAP(), PanicInState(EBTACLSAPParentlessChild));
	}

void TACLLinkStateWaitForStart::Start(CACLLink& aContext) const
	{
	LOG_FUNC
	// transfer ownership to higher entity
	aContext.ListeningSAP()->RemoveChild(&aContext);
	aContext.ListeningSAP() = NULL;
	TInt err = aContext.OpenLogicalLink();
	if (err == KErrNone)
		{
		ChangeState(aContext, CACLLinkStateFactory::EOpen);
		}
	else
		{
		Error(aContext, err);
		}
	}

void TACLLinkStateWaitForStart::Deletion(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TACLLinkStateWaitForStart::Error(CACLLink& aContext, TInt /*aError*/) const
	{
	LOG_FUNC
	ChangeState(aContext, CACLLinkStateFactory::EWaitForStartError);
	}

void TACLLinkStateWaitForStart::Park(CACLLink& aContext) const
	{
	LOG_FUNC
	// blimey! already gone into park - dont tell socket anything
	ChangeState(aContext, CACLLinkStateFactory::EWaitForStartParked);	
	}

void TACLLinkStateWaitForStart::Disconnection(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.ListeningSAP()->DeleteChild(&aContext);
	}

void TACLLinkStateWaitForStart::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// just go
	ChangeState(aContext, CACLLinkStateFactory::EAcceptingClosing);
	}

//----------------------------------------------------------------------------------

TACLLinkStateWaitForStartError::TACLLinkStateWaitForStartError(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Error");
	}

void TACLLinkStateWaitForStartError::Start(CACLLink& aContext) const
	{
	LOG_FUNC
	// summat went wrong - close and remove this acceptor
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->Disconnect();

	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TACLLinkStateWaitForStartError::Disconnection(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	//Release build should silently ignore the event
	__ASSERT_DEBUG(EFalse,PanicInState(EBTACLSAPUnexpectedEvent));	
	}

void TACLLinkStateWaitForStartError::Error(CACLLink& /*aContext*/, TInt /*aError*/) const
	{
	LOG_FUNC
	//Release build should silently ignore the event
	__ASSERT_DEBUG(EFalse,PanicInState(EBTACLSAPUnexpectedEvent));
	}	
	
void TACLLinkStateWaitForStartError::Deletion(CACLLink& aContext) const
/*
In WaitForStartError state we expect ESock to send a Start. But deletion could occur
if ESock deletes the parent SAP.
*/
	{
	LOG_FUNC
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}
	
//----------------------------------------------------------------------------------

TACLLinkStateWaitForStartParked::TACLLinkStateWaitForStartParked(CACLLinkStateFactory& aFactory)
: TACLLinkStateWaitForStart(aFactory)
	{
	LOG_FUNC
	STATENAME("WaitForStartParked");
	}

void TACLLinkStateWaitForStartParked::Start(CACLLink& aContext) const
	{
	LOG_FUNC
	// transfer ownership to higher entity
	aContext.ListeningSAP()->RemoveChild(&aContext);
	aContext.ListeningSAP() = NULL;
	ChangeState(aContext, CACLLinkStateFactory::EOpenParked);
	}

void TACLLinkStateWaitForStartParked::Unpark(CACLLink& aContext) const
	{
	LOG_FUNC
	// that's nice - un-parked!
	ChangeState(aContext, CACLLinkStateFactory::EWaitForStart);
	}

//----------------------------------------------------------------------------------

TACLLinkStateWaitForLink::TACLLinkStateWaitForLink(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("WaitForLink");
	}

void TACLLinkStateWaitForLink::ConnectComplete(CACLLink& aContext, const TBTConnect& aConnectDetails) const
	{
	LOG_FUNC
	/*	super - now BT1.1/1.2 says we don't need to do anything - 
	ACL is here for free: with same handle as PhysicalLink.
	We do however need to introduce ourselves to ACL Q
	*/
	LOG(_L("CACLLink: Physical Channel Up"))
	__ASSERT_DEBUG(aConnectDetails.iBdaddr == aContext.iPhysicalLink->BDAddr(), PanicInState(EBTACLSAPWrongPhysicalLink));
	aContext.iRemoteDev = aConnectDetails.iBdaddr;
	TInt err = aContext.OpenLogicalLink();
	if (err == KErrNone)
		{
		ChangeState(aContext, CACLLinkStateFactory::EOpen);
		aContext.Socket()->ConnectComplete();
		}
	else
		{
		Error(aContext, err);
		}
	}

void TACLLinkStateWaitForLink::Error(CACLLink& aContext, TInt aError) const
	{
	LOG_FUNC
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->Error(aError, MSocketNotify::EErrorConnect);
	}

void TACLLinkStateWaitForLink::Exit(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	}

void TACLLinkStateWaitForLink::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// just change to closing - we'll get notified of PhyUp, but that'll get swallowed
	// and the connection will timeout and die.
	ChangeState(aContext, CACLLinkStateFactory::EClosing);
	}

void TACLLinkStateWaitForLink::Disconnection(CACLLink& aContext) const
	{
	LOG_FUNC
	//Physical link is down
	Error(aContext, KErrCouldNotConnect);
	}
	
//----------------------------------------------------------------------------------

TACLLinkStateOpen::TACLLinkStateOpen(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Open");
	}

void TACLLinkStateOpen::Enter(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	}

void TACLLinkStateOpen::Disconnection(CACLLink& aContext) const
	{
	LOG_FUNC
	// an unexpected phy loss
	LOG(_L("CACLLink: Physical Channel Down"))

	// No more data will appear so close
	aContext.CloseLogicalLink();
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->Disconnect();
	}
	
void TACLLinkStateOpen::Error(CACLLink& aContext, TInt aError) const
	{
	LOG_FUNC
	// eg for weird times when we get another (PHY) connect complete with error!
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->Error(aError, MSocketNotify::EErrorAllOperations);
	}

TUint TACLLinkStateOpen::Write(CACLLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const
	{
	LOG_FUNC
	// Note: This method returns the number of data
	// fragments written (normally 1), NOT the number of bytes.
	LOG(_L("TACLLinkStateOpen::Write"));

	// we only allow 8 bit flags
	__ASSERT_DEBUG(aOptions < KMaxTUint8, PanicInState(EBTACLSAPBadFlagsOnWrite));

	TUint8 flags = static_cast<TUint8>(aOptions);

	// check not trying to do piconet broadcast - active bc is OK
	__ASSERT_DEBUG(!((flags >> KPacketPBBCFlagShift) & KPacketBCFlagMask),
		PanicInState(EBTACLSAPDoesNotSupportPiconetBroadcast));

	// check that L2CAP isn't trying to broadcast
	__ASSERT_DEBUG(!(((flags >> KPacketPBBCFlagShift) & KPacketBCFlagMask)
		&& aContext.iLocalPort==EACLPortL2CAP), PanicInState(EBTACLSAPDoesNotSupportBroadcastL2CAP));


	CACLDataQController& aclQctrl = aContext.iProtocol.ACLController();
	CACLDataItem* dataItem = aclQctrl.GetFreeItem(); // we don't own it - but might be NULL

	TUint retVal;

	if (!dataItem)
		{
		// no space on pool
		LOG(_L("\tno space in ACL data pool- blocking socket"))
		aContext.iSocketBlocked = ETrue;
		retVal = 0; // could not write anything - notify later
		}
	else
		{
		// space on pool...so format get the HCIFacade to format and queue the frame
		dataItem->SetElementHandle(aAddr);
		retVal = aContext.DoWrite(*dataItem, aclQctrl, aData, flags);
		}

	LOG1(_L("TACLLinkStateOpen::Write retVal = %d"), retVal);
	return retVal;
	}


void TACLLinkStateOpen::DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const
	{
	LOG_FUNC
	aContext.NotifyDataToSocket(aFlag, aData);
	}

void TACLLinkStateOpen::Park(CACLLink& aContext) const
	{
	LOG_FUNC
	ChangeState(aContext, CACLLinkStateFactory::EOpenParked);
	}

void TACLLinkStateOpen::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// an ACL link cannot be closed with the underlying PHY going.
	// to congrue with the spec we keep this object all the time the PHY
	// is around.
//	__ASSERT_DEBUG(aCloseType == CServProviderBase::ENormal, PanicInState(EBTACLSAPNotSupportedFeature));

	// Change state first in case the ACL is currently in Park mode.
	// This will allow the logic in the Exit of the Open Park state
	// to un-park the link before it is disconnected.
	ChangeState(aContext, CACLLinkStateFactory::EClosing); // we close when the PHY says
	}


void TACLLinkStateOpen::Exit(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	}

//----------------------------------------------------------------------------------

TACLLinkStateOpenParked::TACLLinkStateOpenParked(CACLLinkStateFactory& aFactory)
: TACLLinkStateOpen(aFactory)
	{
	LOG_FUNC
	STATENAME("OpenParked");
	}

void TACLLinkStateOpenParked::Enter(CACLLink& aContext) const
	{
	LOG_FUNC
	// signal ACL Data Q controller that we've parked up
	aContext.ParkLogicalLink();
	}

TUint TACLLinkStateOpenParked::Write(CACLLink& /*aContext*/, const TDesC8& /*aData*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/) const
	{
	LOG_FUNC
	// can't - parked; and don't support piconet broadcast yet...
	// for active broadcast support that in open state
	return 0;
	}

void TACLLinkStateOpenParked::Unpark(CACLLink& aContext) const
	{
	LOG_FUNC
	ChangeState(aContext, CACLLinkStateFactory::EOpen);

	// flow control on socket
	aContext.Socket()->CanSend();
	}

void TACLLinkStateOpenParked::DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const
	{
	LOG_FUNC
	// We handle data in this state to allow for the race condition between
	// the Mode Change [Park] event and data that was received at the HC just
	// prior to Park mode being entered.
	aContext.NotifyDataToSocket(aFlag, aData);
	}

void TACLLinkStateOpenParked::Exit(CACLLink& aContext) const
	{
	LOG_FUNC
	// signal ACL Data Q controller that we're un-parking
	aContext.UnparkLogicalLink();
	}

//----------------------------------------------------------------------------------

TACLLinkStateClosing::TACLLinkStateClosing(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Closing");
	}

void TACLLinkStateClosing::ActiveOpen(CACLLink& aContext) const
	{
	LOG_FUNC
	// erk - have been asked to Open as we're closing down
	// the link hasn't gone yet, so just say it's there!
	
	// Set as locally initiated (even if previously it was remotely initiated)
	aContext.SetInitiationState(CACLLink::ELocallyInitiated);
	ChangeState(aContext, CACLLinkStateFactory::EOpen);
	aContext.Socket()->ConnectComplete();
	}

void TACLLinkStateClosing::Shutdown(CACLLink& /*aContext*/, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// we are!  just ignore and continue
	}

void TACLLinkStateClosing::Disconnection(CACLLink& aContext) const
	{
	LOG_FUNC
	// an expected phy down!

	// No more data will appear, so close
	aContext.CloseLogicalLink();

	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->CanClose();
	}

void TACLLinkStateClosing::Enter(CACLLink& aContext) const
	{
	LOG_FUNC
	// Inform the physical link that this link has become idle.
	aContext.LinkStateIdle();
	}

void TACLLinkStateClosing::Deletion(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// allowed
	}

TBool TACLLinkStateClosing::IsIdle() const
	{
	LOG_FUNC
	return ETrue;
	}

void TACLLinkStateClosing::DataReceived(CACLLink& aContext, TUint8 aFlag, const TDesC8& aData) const
	{
	LOG_FUNC
	// got data for socket - that's fine: deliver it to decide
	aContext.NotifyDataToSocket(aFlag, aData);
	}
	
TUint TACLLinkStateClosing::Write(CACLLink& /*aContext*/, const TDesC8& /*aData*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/) const
	{
	LOG_FUNC
	//Drop, we are not interested in data anymore.
	return 0;
	}

void TACLLinkStateClosing::Error(CACLLink& aContext, TInt /*aError*/) const
	{
	LOG_FUNC
	// This method is reached by receiving a Disconnection Complete Event with Status field != EOk,
	// in this case we don't want to error the socket - so instead it is just closed
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->CanClose();
	}

//----------------------------------------------------------------------------------

TACLLinkStateAcceptingClosing::TACLLinkStateAcceptingClosing(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("AcceptingClosing");
	}

void TACLLinkStateAcceptingClosing::Shutdown(CACLLink& /*aContext*/, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG_FUNC
	// we are!  just ignore and continue
	}

void TACLLinkStateAcceptingClosing::Deletion(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TACLLinkStateAcceptingClosing::Error(CACLLink& aContext, TInt /*aError*/) const
	{
	LOG_FUNC
	// This method is reached by receiving a Disconnection Complete Event with Status field != EOk,
	// in this case we don't want to error the socket - so instead it is just closed
	aContext.ListeningSAP()->RemoveChild(&aContext);
	ChangeState(aContext, CACLLinkStateFactory::EClosed);
	aContext.Socket()->CanClose();
	}


//----------------------------------------------------------------------------------

TACLLinkStateClosed::TACLLinkStateClosed(CACLLinkStateFactory& aFactory)
: TACLLinkState(aFactory)
	{
	LOG_FUNC
	STATENAME("Closed");
	}

void TACLLinkStateClosed::Enter(CACLLink& aContext) const
	{
	LOG_FUNC
	aContext.ClearInitiationState(); // once closed, there is no direction
	aContext.ClearPhysicalLink();
	}

void TACLLinkStateClosed::Start(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// gulp this - don't need to transition
	}

void TACLLinkStateClosed::Shutdown(CACLLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG_FUNC
	if (aCloseType != CServProviderBase::EImmediate)
		{
		aContext.Socket()->CanClose();
		}
	}

void TACLLinkStateClosed::ActiveOpen(CACLLink& aContext) const
	{
	LOG_FUNC
	// create ACL link
	ChangeState(aContext, CACLLinkStateFactory::EWaitForLink);
	TRAPD(err, aContext.BindAndConnectPhysicalLinkL());

	if (err != KErrNone)
		{
		aContext.Error(err);
		}
	else
		{
		// Successful active open means that we've been locally initiated.
		aContext.SetInitiationState(CACLLink::ELocallyInitiated);
		}
	}

TInt TACLLinkStateClosed::PassiveOpen(CACLLink& aContext, TUint __DEBUG_ONLY(aQueSize)) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aQueSize == 1, PanicInState(EBTACLSAPUnsupportedQueSize));
	// only support a QueSize of 1 - we don't expose the ACL Links to ESOCK
	// for use by RSockets; furhter we enforce L2CAP to *start* us synchronously
	// upon ACL link up

	ChangeState(aContext, CACLLinkStateFactory::EListening);

	return KErrNone;
	}

void TACLLinkStateClosed::Deletion(CACLLink& /*aContext*/) const
	{
	LOG_FUNC
	// allowed deletion
	}

TBool TACLLinkStateClosed::IsIdle() const
	{
	LOG_FUNC
	return ETrue;
	}

//----------------------------------------------------------------------------------

CACLLink* CACLLink::NewLC(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aConnection, CLinkMgrProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CACLLink* s = new(ELeave) CACLLink(aLinksMan, aConnection, aProtocol);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}


CACLLink* CACLLink::NewL(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aConnection, CLinkMgrProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CACLLink* s = NewLC(aLinksMan, aConnection, aProtocol);
	CleanupStack::Pop(s);
	return s;
	}


CACLLink::CACLLink(CPhysicalLinksManager& aManager,
				   CPhysicalLink* aConnection,
				   CLinkMgrProtocol& aProtocol)
: 	CBTBasebandSAP(aManager, aConnection),
	iProtocol(aProtocol),
	iSocketBlocked(EFalse)
	{
	LOG_FUNC
	iState = &iProtocol.ACLStateFactory().GetState(CACLLinkStateFactory::EClosed);

	// Hard-coded security for ACL level security (not service level).
	// MITM desired is applied to ensure authentication takes place (if
	// since authentication is normally eq. MITM protection).  Pairing
	// is prevented where appropriate.
	// (See Paired Only Connections Mode for why this is used).
	iSecurity.SetAuthentication(EMitmDesired);
	iSecurity.SetEncryption(EFalse);
	iSecurity.SetAuthorisation(EFalse);
	}

CACLLink::~CACLLink()
	{
	LOG_FUNC
	// CloseLogicalLink() should have been called by now, but just in case...
	if ( iHandle != KInvalidConnectionHandle )
		{
		CloseLogicalLink();
		}
	
	// There could be an access request outstanding
	CancelAccessRequest();
	
	iState->Deletion(*this);
	
	UnbindLink(EACLLink);

	__ASSERT_DEBUG(!iChild, Panic(EAclSapChildStillPresentAtDestruction));
	}

void CACLLink::ConstructL()
	{
	LOG_FUNC
	CBTBasebandSAP::ConstructL();
	}

void CACLLink::ActiveOpen()
	{
	LOG_FUNC
	iState->ActiveOpen(*this);
	}

void CACLLink::BindAndConnectPhysicalLinkL()
	{
	LOG_FUNC
	if(!(Baseband().IsACLPossible()))
		{
		User::Leave(KErrInsufficientBasebandResources);
		}

	iPhysicalLink = iLinksMan.FindPhysicalLink(iRemoteDev);

	if (iPhysicalLink)
		{
		TBTBasebandLinkState::TLinkState linkState =  iPhysicalLink->LinkState().LinkState();
		if (linkState == TBTBasebandLinkState::ELinkDown)
			{
			// Incoming pending physical link connection. We must error here as we cannot 
			// know if the incoming connection will be successful or not.
			iPhysicalLink = NULL;
			User::Leave(KErrPendingPhysicalLink);
			}

		User::LeaveIfError(BindLink(EACLLink, *iPhysicalLink));

		if (linkState == TBTBasebandLinkState::ELinkUp)
			{
			// We have an existing, connected, physical link so we can complete this request
			TBTConnect connect;
			connect.iBdaddr = iPhysicalLink->BDAddr();
			ConnectComplete(connect);
			return;
			}
			
		// If we get here then we must have a pending active physical link connection. 
		// As we bind to it above then this connection request we be completed when
		// the physical link is completed.
		__ASSERT_ALWAYS(linkState == TBTBasebandLinkState::ELinkPending, Panic(EBTBasebandInvalidLinkState));
		}
	else
		{
		// New physical link connection request
		CPhysicalLink* physicalLink = &iLinksMan.NewPhysicalLinkL(iRemoteDev);
		// physicalLink is owned by the physical links manager.
		User::LeaveIfError(BindLink(EACLLink, *physicalLink));
		iPhysicalLink->Connect();
		}
	}

TInt CACLLink::SetLocalName(TSockAddr& aAddr)
	{
	LOG_FUNC
	// Copy this address into iLocal*
	// Overwrite their BTAddr if it's wrong
	// Must check that the endpoint required is free
	TACLSockAddr aclAddr = TACLSockAddr::Cast(aAddr);
	TACLPort port = aclAddr.Port();

	iLocalPort=port;
	return KErrNone;
	}

TInt CACLLink::SetRemName(TSockAddr& aAddr)
	{
	LOG_FUNC
	// Copy this over
	__ASSERT_DEBUG(!ListeningSAP(), Panic(EBTACLSAPListenerSettingName)); // not applicable to listeners

	TACLSockAddr bbAddr(aAddr);	// convert
	iRemoteDev = bbAddr.BTAddr();
	
	//try to get the details about the intended remote now
	return KErrNone;
	}
	
TInt CACLLink::PassiveOpen(TUint aQueSize)
	{
	LOG_FUNC
	return iState->PassiveOpen(*this, aQueSize);
	}


TInt CACLLink::GetOption(TUint aLevel,TUint aName,TDes8& aOption) const
	{
	LOG_FUNC
	TInt ret = KErrNone;

	if (aLevel != KSolBtLM && aLevel != KSolBtACL)
		ret = KErrNotSupported;
	else
		{
		if (aLevel == KSolBtACL)
			{
			ret = DoACLGetOption(aName, aOption);
			}
		else
			{
			if (iPhysicalLink)
				{
				// forward to PHY
				ret = iPhysicalLink->GetOption(aLevel, aName, aOption);
				}
			else
				{
				// ho-hum!
				ret = KErrNotReady;
				}
			}
		}
	return ret;
	}


TInt CACLLink::DoACLGetOption(TUint aName,TDes8& aOption) const
	{
	LOG_FUNC
	TInt ret = KErrNone;

	switch (aName)
		{
		case ELMOutboundACLSize:
			{
			if (aOption.Length() != sizeof(TInt))
				{
				ret = KErrArgument;
				}
			TInt val = iProtocol.ACLPacketMTU();
			aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
			break;
			}
		case ELMInboundACLSize:
			{
			if (aOption.Length() != sizeof(TInt))
				{
				ret = KErrArgument;
				}
			TInt val = KLinkMgrIncomingBufferSize;	// for now...
			aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
			break;
			}
		case KLMGetACLHandle:
			{
			if (aOption.Length() != sizeof(THCIConnHandle))
				{
				ret = KErrArgument;
				}
			aOption = TPtrC8(reinterpret_cast<const TUint8*>(&iHandle), sizeof(THCIConnHandle));
			break;
			}
		default:
			__DEBUGGER();
			ret = KErrNotSupported;
		}
	return ret;
	}


TUint CACLLink::DoWrite(CACLDataItem& aItem, CACLDataQController& aQctrl,
					   const TDesC8& aData, TUint8 aOptions)
/**
	aFrame has been obtained by someone else (the state)
	- we can go and complete the write
**/
	{
	LOG_FUNC
	LOG1(_L("CACLLink::DoWrite aItem = 0x%08x"), &aItem);

	__ASSERT_ALWAYS(aItem.Frame() != NULL, Panic(EBTACLSAPWriteDataItemWithNullFrame));

	// Fill the item with data...
	iLinksMan.HCIFacade().FormatACLData(*aItem.Frame(), Handle(), aOptions, aData);
	//.. and add it to the pool
	aQctrl.AddItem(aItem);
	TInt ret = 1; //one data 'item'
	
	LOG1(_L("CACLLink::DoWrite ret = %d"), ret);
	return ret;
	}

void CACLLink::Ioctl(TUint /*aLevel*/,TUint /*aName*/,TDes8* /*aOption*/)
	{
	LOG_FUNC
	// good stuff here!
	}

void CACLLink::CancelIoctl(TUint /*aLevel*/,TUint /*aName*/)
	{
	LOG_FUNC
	// good stuff here!
	}

TInt CACLLink::SAPSetOption(TUint aLevel,TUint aName,const TDesC8& aOption)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	switch(aLevel)
		{
		case KSolBtLM:
			rerr = KErrNotSupported;
			break;
			
		case KSolBtACL:
			rerr = DoACLSetOption(aName, aOption);
			break;
			
		default:
			rerr = KErrNotSupported;
			break;
		};

	return rerr;
	}

TInt CACLLink::DoACLSetOption(TUint aName, const TDesC8& /*aOption*/)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	
	switch(aName)
		{
		case KSolBtACLFlushOccured:
			rerr = ProcessFlush();
			break;
		
		default:
			rerr = KErrNotSupported;
			break;
		};
		
	return rerr;
	}

void CACLLink::RemName(TSockAddr& aAddr) const 
	{
	LOG_FUNC
	//Return the remote name
	// Copy iRemoteDev and iRemotePort into TSockAddr and return
	TACLSockAddr bbAddr(aAddr);
	bbAddr.SetBTAddr(iRemoteDev);
	aAddr=bbAddr;   // Convert back
	}

void CACLLink::LocalName(TSockAddr& aAddr) const 
	{
	LOG_FUNC
	// Copy iLocalPort into TSockAddr and return
	TACLSockAddr bbAddr(aAddr);
	bbAddr.SetBTAddr(iProtocol.LocalBTAddress());
	aAddr=bbAddr;
	}

void CACLLink::Start()
	{
	LOG_FUNC
	iState->Start(*this);
	}

void CACLLink::Timeout(TBasebandTimeout aTimeout)
	{
	LOG_FUNC
	iState->Timeout(*this, aTimeout);
	}

void CACLLink::AutoBind()
	{
	LOG_FUNC
	// at present no state bothers with this, so just process here
	// do nothing
	}

void CACLLink::Shutdown(TCloseType aOption)
	{
	LOG_FUNC
	iState->Shutdown(*this, aOption);
	}

void CACLLink::Shutdown(TCloseType aOption, const TDesC8& /*aDisconnectionData*/)
	{
	LOG_FUNC
	iState->Shutdown(*this, aOption);
	}

void CACLLink::NotifyDataToSocket(TUint8 aFlag, const TDesC8& aData)
	{
	LOG_FUNC
/*
	The design of the protocol specification for L2CAP
	means that both we and L2CAP need to know the flag parameter
	for now we just signal one datagram (*could* signal two - one for flag: but that's just as grubby)
*/

	RMBufChain aclData;

	CACLDataQController& aclQctrl = iProtocol.ACLController();
	THCIConnHandle connH = iHandle;
	TRAPD(err, aclData = aclQctrl.PopulateInboundBufferL(connH, aFlag, aData));
	if (err)
		{
		// We have run out of MBufs, there is nothing we can do here but 
		// 1) drop the received packet, or
		// 2) disconnect the link
		// We drop the packet to be multi-profile "friendly"
		LOG1(_L8("*** ERROR: Dropping ACL Data!!! (error = %d) ***"), err);
		return;
		}
	
	// slap onto the RMBufPacketQ
	iInboundBuffer.Append(aclData); // transfers
	
	iSocket->NewData(1);	// datagrams: could async notify - or get l2cap to drain async
	}


void CACLLink::GetData(TDes8& aDesc,TUint aOptions,TSockAddr* aAddr)
	{
	LOG_FUNC
// see note in call to NewData - we just give a single descriptor
// could get L2CAP to call down with options/addr: it could say whether
// it's expecting flags or not etc

// this overload is for descriptors - will be deprecated
// for now use a dummy mbufchain
	RMBufChain dummy;

	GetData(dummy, aDesc.Length(), aOptions, aAddr);

	aDesc.SetMax();
	dummy.CopyOut(aDesc);
	dummy.Free();	// release the actual mBuf - it's also gone from the inbound buffer collection
	}

TInt CACLLink::GetData(RMBufChain& aData, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	LOG_FUNC
	static_cast<void>(iInboundBuffer.Remove(aData));	// take off Q
	
	return 1; // datagram
	}

TUint CACLLink::Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr)
	{
	LOG_FUNC
	return iState->Write(*this, aDesc, aOptions, aAddr);
	}

void CACLLink::CloseLogicalLink()
	{
	LOG_FUNC
	// tell our Q controller about ACL death
	iProtocol.ACLController().ACLLogicalLinkDown(iHandle);
	iHandle = KInvalidConnectionHandle;
	}

void CACLLink::ClearPhysicalLink()
	{
	LOG_FUNC
	CBTBasebandSAP::UnbindLink(EACLLink);
	}

TInt CACLLink::OpenLogicalLink()
	{
	LOG_FUNC
	// spec says our ACL handle = that of PHY
	iHandle = iPhysicalLink->Handle();
	TInt err = iProtocol.ACLController().ACLLogicalLinkUp(iHandle, iPhysicalLink->IsParked());

	return err;
	}

void CACLLink::Error(TInt aErr)
	{
	LOG_FUNC
	iState->Error(*this, aErr);
	}

void CACLLink::Disconnection()
	{
	LOG_FUNC
	iState->Disconnection(*this);
	}

void CACLLink::ConnectComplete(const TBTConnect& aConnect)
	{
	LOG_FUNC
	iState->ConnectComplete(*this, aConnect);
	}

TBool CACLLink::ConnectRequest(const TBTConnect& aConnect, const CPhysicalLink& aPhysicalLink)
	{
	LOG_FUNC
	return iState->ConnectRequest(*this, aConnect, aPhysicalLink);
	}

void CACLLink::DataReceived(THCIConnHandle __DEBUG_ONLY(aConnH), TUint8 aFlag, const TDesC8& aData)
	{
	LOG_FUNC
	// even if this is broadcast, the data *FROM* the HC to the Host should have
	// the real connection handle of the physical link to the master (who sent the data!)
	__ASSERT_DEBUG(aConnH == Handle(), Panic(EBTACLSAPDataForWrongSAP));

	if (!CanHandleFlags(aFlag))
		{
		// we can't handle these flags - other ACL SAPs might though!
		return;
		}

	iState->DataReceived(*this, aFlag, aData);
	}

void CACLLink::PacketsSent(THCIConnHandle /*aHandle*/, TUint16 /*aNumPackets*/)
	{
	LOG_FUNC
	TryToSend();
	}
	
TBool CACLLink::CanHandleFlags(TUint8 aFlag) const
	{
	LOG_FUNC
	// map packet flags to port
	TUint8 pbFlags = static_cast<TUint8>((aFlag >> KPacketPBBCFlagShift) & KPacketPBFlagMask);
	if ((pbFlags == KFirstHLFragment || pbFlags == KContinuingHLFragment) && iLocalPort == EACLPortL2CAP)
		return ETrue;
	if (pbFlags == 0 && iLocalPort == EACLPortRaw)
		return ETrue;
	return EFalse;
	}

TInt CACLLink::ProcessFlush()
	{
	LOG_FUNC
	CACLDataQController& aclQCtrl = iProtocol.ACLController();
	return aclQCtrl.SetFlushInProgress(Handle());
	}	
	
TBool CACLLink::IsIdle() const
	{
	LOG_FUNC
	return iState->IsIdle();
	}

void CACLLink::PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& __DEBUG_ONLY(aPhysicalLink))
	{
	LOG_FUNC
	// ACL logical links only really interested in parking of PHY
	__ASSERT_DEBUG(&aPhysicalLink == iPhysicalLink, Panic(EBTACLSAPWrongPhysicalLink));

	// test for a park event
	if (aEvent.EventType() & ENotifyParkMode)
		{
		// gone into park, but check phy role: could do broadcast
		iState->Park(*this);
		}

	else if (aEvent.EventType() & ENotifyActiveMode)
		{
		//un-parked
		iState->Unpark(*this);
		}

	else 
		{
		// uninteresting event
		}
	}

void CACLLink::TryToSend()
	{
	LOG_FUNC
	// no need to tell the buffer management entity (acl pool) - already done
	// if socket blocked - unblock
	if (iSocketBlocked)
		{
		iSocketBlocked = EFalse;
		Socket()->CanSend();
		}
	}

void CACLLink::ParkLogicalLink()
	{
	LOG_FUNC
	iProtocol.ACLController().SetParked(Handle(), ETrue);
	}

void CACLLink::UnparkLogicalLink()
	{
	LOG_FUNC
	iProtocol.ACLController().SetParked(Handle(), EFalse);
	}

void CACLLink::SetInitiationState(TInitiationState aState)
	{
	__ASSERT_DEBUG(aState != EInvalidInitiationState, Panic(EBTACLSAPIndeterminateInitiator));
	iInitiationState = aState;
	}

void CACLLink::ClearInitiationState()
	{
	iInitiationState = EInvalidInitiationState;
	}

TBool CACLLink::IsLocallyInitiated() const
	{
	__ASSERT_DEBUG(iInitiationState != EInvalidInitiationState, Panic(EBTACLSAPIndeterminateInitiator));
	return iInitiationState == ELocallyInitiated;
	}

void CACLLink::AccessRequestComplete(TInt aResult)
	{
	// We don't expect the deferred result, if we get this then the state machine
	// for the logical link needs to be updated.
	__ASSERT_DEBUG(aResult != EBTSecManAccessDeferred, Panic(EBTACLSAPUnexpectedSecurityResult));
	if(aResult == EBTSecManAccessDenied) // convert to a symbian error
		{
		aResult = KErrAccessDenied;
		}
	if(aResult != KErrNone)
		{
		// Kill the physical link, this should result in the ACL link being
		// pulled down.
		if(iPhysicalLink)
			{
			iPhysicalLink->Terminate(ERemoteUserEndedConnection);
			}
		}
	// else success from the access requester so we're done.
	}

