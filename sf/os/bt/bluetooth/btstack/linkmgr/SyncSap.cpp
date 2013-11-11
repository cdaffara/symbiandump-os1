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
// Implements the Synchronous SAP class
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <bluetooth/hci/hciconsts.h>
#include "SCOSAP.h"
#include "physicallinksmanager.h"
#include "physicallinks.h"
#include "hcifacade.h"
#include "linkutil.h"
#include "linkmuxer.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif


CBTSynchronousLink::CBTSynchronousLink(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink, TLinkType aLinkType)
: CBTBasebandSAP(aLinksMan, aPhysicalLink), iLinkType(aLinkType)
	{
	LOG2(_L("Creating sync SAP 0x%08x, type %d"), this, aLinkType);
	iState = &aLinksMan.LinkManagerProtocol().
			SyncStateFactory().GetState(CSyncLinkStateFactory::EClosed);
	}

void CBTSynchronousLink::ConstructL()
	{
	CBTBasebandSAP::ConstructL();
	}

CBTSynchronousLink::~CBTSynchronousLink()
	{
	LOG2(_L("Destroying sync SAP 0x%08x, iPhysicalLink 0x%08x"), this, iPhysicalLink);
	
	/*
	The ASSERT below has been commented due to an issue with ESock.

	During a normal shutdown, a CBTSynchronousLink will always unbind from the physical link before
	calling CanClose(). However automated tests have shown that there is no guarantee that during a 
	normal shutdown ESock will wait until the SAP has called CanClose() before deleting it. If
	the socket is closed whilst ESock is waiting for the CanClose() it will immediately delete the
	SAP (and complete the Shutdown request with KErrCancel). In this case the SAP will still be
	bound to a physical link.
	
	If the ESock issue is rectified, the ASSERT should be reinstated.
	
	
	ASSERT(!iPhysicalLink);
	*/
	
	iState->Deletion(*this);
	
	ReleaseAndUnbind();
	}

void CBTSynchronousLink::ReleaseAndUnbind()
/**
	NB If the physical link is unbound, and the SCO packet type is
	already ENoSCOLink, this function will do nothing!
*/
	{
	LOG1(_L("CBTSyncLink (0x%08x): Release and unbind"), this);

	if(iPhysicalLink)
		{
		iPhysicalLink->UndoOverridePark();
		}
	UnbindLink(iLinkType); //sets iPhysicalLink to NULL
	}

void CBTSynchronousLink::PhysicalLinkChange(const TBTBasebandEventNotification& IF_FLOGGING(aEvent), CPhysicalLink& __DEBUG_ONLY(aPhysicalLink))
	{
	__ASSERT_DEBUG(&aPhysicalLink == iPhysicalLink, Panic(EBTSCOSAPWrongPhysicalLink));

	// spec says should never have got parked with sco link
	//__ASSERT_DEBUG(!(aEvent.EventType() & ENotifyParkMode), Panic(EBTSCOSAPExistsWhenParking));
	//
	// Above ASSERT too strong as races are occuring. Log, but 'ignore'.

	#ifdef __FLOG_ACTIVE
	if (!(aEvent.EventType() & ENotifyParkMode))
		{
		LOG(_L("Sync SAP: Park request received with an active eSCO link. Likely to be a race."));
		}
	#endif
	}

void CBTSynchronousLink::AutoBind()
	/**
	   Pick an appropriate local port.
	**/
	{
	}

TInt CBTSynchronousLink::SetLocalName(TSockAddr& /*aAddr*/)
/**
	Override KErrNotSupported in base class.
*/
	{
	return KErrNone;
	}

void CBTSynchronousLink::Start()
	{
	iState->Start(*this);
	}

void CBTSynchronousLink::Timeout(TBasebandTimeout aTimeout)
	{
	iState->Timeout(*this, aTimeout);
	}

TInt CBTSynchronousLink::SetRemName(TSockAddr& aAddr)
	{
	TBTSockAddr bbAddr(aAddr);	// convert
	iRemoteDev=bbAddr.BTAddr();

	return KErrNone;	// to avoid race conditions we check for ACL later
	}

void CBTSynchronousLink::ActiveOpen()
/**
	Attach SCO link - the packet type should have been set by this stage
**/
	{
	iState->ActiveOpen(*this);
	}

void CBTSynchronousLink::DoActiveOpenL()
	{
	// proceed with creating the SCO link, but first check that phy exists
	CPhysicalLink* foundPhysicalLink = iLinksMan.FindPhysicalLink(iRemoteDev);

	if (!foundPhysicalLink)
		{
		// cannot do this until baseband is up...SCO Transports aren't allowed to instantiate
		// PHYs
		User::Leave(KErrNotReady);
		}
	else
		{
		if (foundPhysicalLink->HasSyncLink())
			{
			User::Leave(KErrInUse);
			}
		
		// go and actually make the link
		if(!IsLinkProbablyPossible())
			{
			User::Leave(KErrOverflow);
			}
		foundPhysicalLink->OverridePark();
		BindLink(iLinkType, *foundPhysicalLink);
		
		//Initiate a SCO connection with user specified packet types. all types will be allowed
		//if SetOption has not been called.
		TInt err = MakeConnection(foundPhysicalLink);
		if (err)
			{
			UnbindLink(iLinkType);
			User::Leave(err);
			}
		}
	}

TInt CBTSynchronousLink::PassiveOpen(TUint aQueSize)
/**
	Record ourselves as willing to accept a SCO link (possibly of the CoD we want)
**/
	{
	LOG1(_L("CBTSyncLink (0x%08x): PassiveOpen"), this);
	return iState->PassiveOpen(*this, aQueSize);
	}

TUint CBTSynchronousLink::Write(const TDesC8& aData, TUint aOptions, TSockAddr* aAddr)
	{
	return iState->Write(*this, aData, aOptions, aAddr);
	}

void CBTSynchronousLink::PacketsSent(THCIConnHandle /*aHandle*/, TUint16 /*aNumPackets*/)
	{
	// ignore - no need to unblock socket
	}

TBool CBTSynchronousLink::ConnectRequest(const TBTConnect& aSCOLink, const CPhysicalLink& aPhysicalLink)
	{
	return iState->ConnectRequest(*this, aSCOLink, aPhysicalLink);
	}

void CBTSynchronousLink::GetData(TDes8& aDesc,TUint /*aOptions*/,TSockAddr* /*aAddr*/)
	{
	// return the packet - we only have one - if other stuff has got overwritten
	// so be it - we're going for synchronicity...
	aDesc = InboundFrame();
	}

void CBTSynchronousLink::ConnectComplete(const TBTConnect& aSCOLink)
	{
	TBTSyncConnectOpts SCODefaultOpts(KSCODefaultTransmissionInterval, KSCODefaultRetransmissionWindow,
		KSCODefaultRxPacketLength, KSCODefaultTxPacketLength, KSCODefaultAirMode);

	SyncConnectComplete(aSCOLink, SCODefaultOpts);
	}

void CBTSynchronousLink::SyncConnectComplete(const TBTConnect& aSCOLink, const TBTSyncConnectOpts& aSyncOpts)
	{
	iState->ConnectComplete(*this, aSCOLink, aSyncOpts);
	}

void CBTSynchronousLink::Disconnection()
	{
	LOG2(_L("CBTSyncLink (0x%08x): disconnection on handle %d"), this, iHandle);
	iState->Disconnection(*this);
	}

void CBTSynchronousLink::ParentClosing()
	{
	iState->ParentClosing(*this);
	}

void CBTSynchronousLink::DataReceived(THCIConnHandle __DEBUG_ONLY(aConnH), TUint8 /*aIgnore*/, const TDesC8& aData)
	{
	__ASSERT_DEBUG(aConnH == Handle(), Panic(EBTSCOSAPWrongSCOSAP));
	iState->DataReceived(*this, aData);
	}

void CBTSynchronousLink::Error(TInt aErr)
	{
	iState->Error(*this, aErr);
	}

void CBTSynchronousLink::Shutdown(TCloseType aClose)
/**
	Detach SCO
**/
	{
	LOG2(_L("CBTSyncLink (0x%08x): shutdown no data on handle %d"), this, iHandle);
	iState->Shutdown(*this, aClose);
	}

void CBTSynchronousLink::Shutdown(TCloseType aClose, const TDesC8& /*aDisconnectionData*/)
/**
	Detach SCO - ESock seems to send us through this way 
	whatever the client request has been. The overload above probably does
	not need implementing.
**/
	{
	LOG2(_L("CBTSyncLink (0x%08x): shutdown on handle %d"), this, iHandle);
	iState->Shutdown(*this, aClose);
	}

TBool CBTSynchronousLink::IsIdle() const
	{
	return iState->IsIdle();
	}

TBool CBTSynchronousLink::IsOpen() const
	{
	return iState->IsOpen();
	}

TInt CBTSynchronousLink::CommonShutdown()
	{
	// not common to all baseband SAPs - only for closing SCO links at moment
	// since PhysicalLink object looks after the PHY, and ACL goes up/down with that
	TRAPD(err, iLinksMan.HCIFacade().DisconnectL(Handle(), static_cast<THCIErrorCode>(KActiveDisconnectReason)));
	return err;
	}

TBool CBTSynchronousLink::ConnectPending() const
	{
	return iState->IsConnectPending();
	}
		


//----------------------------------------------------------------------------------
// STATE FACTORY
//----------------------------------------------------------------------------------

CSyncLinkStateFactory* CSyncLinkStateFactory::NewL()
	{
	CSyncLinkStateFactory* ret=new (ELeave) CSyncLinkStateFactory();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop();
	return ret;
	}

void CSyncLinkStateFactory::ConstructL()
	{
	iStates[EClosed]				=new (ELeave) TSCOLinkStateClosed(*this);
	iStates[EWaitForSCO]			=new (ELeave) TSCOLinkStateWaitForSCO(*this);
	iStates[EListening]				=new (ELeave) TSCOLinkStateListening(*this);
	iStates[EAccepting]				=new (ELeave) TSCOLinkStateAccepting(*this);
	iStates[EWaitForStart]			=new (ELeave) TSCOLinkStateWaitForStart(*this);
	iStates[EWaitForStartError]		=new (ELeave) TSCOLinkStateWaitForStartError(*this);
	iStates[EOpen]					=new (ELeave) TSCOLinkStateOpen(*this);
	iStates[EClosing]				=new (ELeave) TSCOLinkStateClosing(*this);
	}

CSyncLinkStateFactory::~CSyncLinkStateFactory()
	{
	iStates.DeleteAll();
	}

TSyncLinkState& CSyncLinkStateFactory::GetState(CSyncLinkStateFactory::TSyncLinkStates aState)
	{
	__ASSERT_DEBUG(iStates[aState],  Panic(ELinkMgrBadSCOState));
	return *iStates[aState];
	}

TInt CSyncLinkStateFactory::StateIndex(const TSyncLinkState* aState) const
	{
	TInt state;
	for (state = 0; state < ESyncLinkMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}




//----------------------------------------------------------------------------------
// STATES
//----------------------------------------------------------------------------------

TSyncLinkState::TSyncLinkState(CSyncLinkStateFactory& aFactory)
: iFactory(aFactory)
	{
	}

void TSyncLinkState::PanicInState(TLinkPanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}

void TSyncLinkState::ChangeState(CBTSynchronousLink& aContext, CSyncLinkStateFactory::TSyncLinkStates aState) const
	{
	aContext.iState->Exit(aContext);

#ifdef __FLOG_ACTIVE
	TSyncLinkState* state=&iFactory.GetState(aState);
	LOG2(_L("SCOLink: State %S -> %S"), &aContext.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
	aContext.iState=&iFactory.GetState(aState);

	aContext.iState->Enter(aContext);
	}

void TSyncLinkState::Enter(CBTSynchronousLink& /*aContext*/) const
	{
	// do nothing
	}

void TSyncLinkState::Deletion(CBTSynchronousLink& /*aContext*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

void TSyncLinkState::Exit(CBTSynchronousLink& /*aContext*/) const
	{
	// do nothing
	}

void TSyncLinkState::Timeout(CBTSynchronousLink& /*aContext*/, TBasebandTimeout /*aTimeout*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

void TSyncLinkState::Shutdown(CBTSynchronousLink& /*aContext*/, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

TInt TSyncLinkState::PassiveOpen(CBTSynchronousLink& /*aContext*/, TUint /*aQueSize*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	return KErrNotSupported;
	}

void TSyncLinkState::ActiveOpen(CBTSynchronousLink& /*aContext*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

TBool TSyncLinkState::ConnectRequest(CBTSynchronousLink& /*aContext*/, const TBTConnect& /*aSCOLink*/, const CPhysicalLink& /*aPhysicalLink*/) const
	{
#ifdef _DEBUG
	PanicInState(EBTSCOSAPUnexpectedEvent); //listener code could be suspect
#endif
	return EFalse;
	}

void TSyncLinkState::Error(CBTSynchronousLink& /*aContext*/, TInt __DEBUG_ONLY(aErr)) const
	{
	// Don't propogate the error as listeners don't care if the hardware has gone down
	__ASSERT_DEBUG(aErr==KErrHardwareNotAvailable||aErr==EHostSecurityRejection, PanicInState(EBTSCOSAPUnexpectedEvent));
	}

void TSyncLinkState::ConnectComplete(CBTSynchronousLink& /*aContext*/, const TBTConnect& /*aConnect*/, const TBTSyncConnectOpts& /*aSyncOpts*/) const
	{
	// eat unexpected HW event
	}

void TSyncLinkState::Disconnection(CBTSynchronousLink& /*aContext*/) const
	{
	// eat unexpected HW event
	}

void TSyncLinkState::DataReceived(CBTSynchronousLink& /*aContext*/, const TDesC8& /*aData*/) const
	{
	// dump data
	}

void TSyncLinkState::PacketTypeChange(CBTSynchronousLink& /*aContext*/, THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, TUint16 /*aNewPacket*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

void TSyncLinkState::Start(CBTSynchronousLink& /*aContext*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	}

TUint TSyncLinkState::Write(CBTSynchronousLink& /*aContext*/, const TDesC8& /*aData*/, TUint /*aOptions*/, TSockAddr* /*aAddr*/) const
	{
	PanicInState(EBTSCOSAPUnexpectedEvent);
	return 0;
	}

TBool TSyncLinkState::IsIdle() const
	{
	return EFalse;
	}

TBool TSyncLinkState::IsOpen() const
	{
	return EFalse;
	}

void TSyncLinkState::ParentClosing(CBTSynchronousLink& /*aContext*/) const
	{
	// Swallow the event.
	}

TBool TSyncLinkState::IsConnectPending() const
	{
	return EFalse;
	}

//----------------------------------------------------------------------------------

TSCOLinkStateClosed::TSCOLinkStateClosed(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateClosed");
	}

void TSCOLinkStateClosed::Enter(CBTSynchronousLink& aContext) const
	{
	aContext.ReleaseAndUnbind();
	}

void TSCOLinkStateClosed::Start(CBTSynchronousLink& /*aContext*/) const
	{
	//gulp
	}

void TSCOLinkStateClosed::ActiveOpen(CBTSynchronousLink& aContext) const
	{
	TRAPD(err, aContext.DoActiveOpenL());
	if (err)
		{
		aContext.Socket()->Error(err, MSocketNotify::EErrorConnect);
		}
	else
		{
		ChangeState(aContext, CSyncLinkStateFactory::EWaitForSCO);
		}
	}

TBool TSCOLinkStateClosed::IsIdle() const
	{
	return ETrue;
	}


TInt TSCOLinkStateClosed::PassiveOpen(CBTSynchronousLink& aContext, TUint /*aQueSize*/) const
	{
	// need to attach to the PHY

	TInt err = aContext.iLinksMan.AddListener(aContext, aContext.iLinkType);
	
	if (err == KErrNone)
		{
		ChangeState(aContext, CSyncLinkStateFactory::EListening);
		}
	
	return err;
	}

void TSCOLinkStateClosed::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG(_L("CBTSyncLink: closed state shutdown, immediate"));
	if (aCloseType == CServProviderBase::ENormal)
		{
		aContext.Socket()->CanClose();	// tell socket it's fine to go
		}
	}


void TSCOLinkStateClosed::Deletion(CBTSynchronousLink& /*aContext*/) const
	{
	// allowed
	}

void TSCOLinkStateClosed::Disconnection(CBTSynchronousLink& /*aContext*/) const
	{
	//discard 
	}
	
//----------------------------------------------------------------------------------

TSCOLinkStateWaitForSCO::TSCOLinkStateWaitForSCO(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateWaitForSCO");
	}


void TSCOLinkStateWaitForSCO::ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& aSyncOpts) const
	{
	aContext.iHandle = aSCOInfo.iConnH;
	aContext.iRemoteDev = aSCOInfo.iBdaddr;

	aContext.SetExtOptions(aSyncOpts);

	aContext.Socket()->ConnectComplete();

	ChangeState(aContext, CSyncLinkStateFactory::EOpen);
	}

void TSCOLinkStateWaitForSCO::Disconnection(CBTSynchronousLink& aContext) const
	{
	// The SCO is not currently connected, therefore this 
	// must be a PHY disconnection event.  A ConnectionComplete event
	// should not be received now so enter the closed state. 
	aContext.Socket()->Error(ENoConnection, MSocketNotify::EErrorConnect);
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	}


void TSCOLinkStateWaitForSCO::Error(CBTSynchronousLink& aContext, TInt aErr) const
	{
	// SCO failed
	aContext.Socket()->Error(aErr, MSocketNotify::EErrorConnect);
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	}


void TSCOLinkStateWaitForSCO::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	// just change to closing - we'll get notified of PhyUp, but that'll get swallowed
	// and the connection will timeout and die.
	if (aCloseType == CServProviderBase::ENormal)
		{
		LOG(_L("CBTSyncLink: wait for SCO state shutdown, move to closing"));
		ChangeState(aContext, CSyncLinkStateFactory::EClosing);
		}
	else
		{
		LOG(_L("CBTSyncLink: wait for SCO state shutdown, die immediately"));
		aContext.ReleaseAndUnbind();
		ChangeState(aContext, CSyncLinkStateFactory::EClosed);
		}
	}

TBool TSCOLinkStateWaitForSCO::IsConnectPending() const
	{
	return ETrue;
	}




//----------------------------------------------------------------------------------

TSCOLinkStateWaitForStart::TSCOLinkStateWaitForStart(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateWaitForStart");
	}


void TSCOLinkStateWaitForStart::Start(CBTSynchronousLink& aContext) const
/**
	Called as a result of a socket "ConnectComplete"
*/
	{
	aContext.ListeningSAP()->RemoveChild(&aContext);
	aContext.ListeningSAP() = NULL;
	ChangeState(aContext, CSyncLinkStateFactory::EOpen);
	}

void TSCOLinkStateWaitForStart::Error(CBTSynchronousLink& aContext, TInt /*aError*/) const
	{
	aContext.ReleaseAndUnbind();
	// no socket to notify of error until Start - transition state
	ChangeState(aContext, CSyncLinkStateFactory::EWaitForStartError);
	}

void TSCOLinkStateWaitForStart::Disconnection(CBTSynchronousLink& aContext) const
	{
	// the open - but unaccepted - link has disappeared
	Error(aContext, KErrDisconnected);
	}

void TSCOLinkStateWaitForStart::Deletion(CBTSynchronousLink& aContext) const
	{
	// allowed deletion - should be listening SAP that is deleting us from
	// an unwanted connection
	aContext.ReleaseAndUnbind();
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TSCOLinkStateWaitForStart::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType /*aCloseType*/) const
	{
	LOG(_L("CBTSyncLink: wait for start state shutdown, tell peer and die"));
	TInt err = aContext.CommonShutdown(); // tell peer
	if (err)
		{
		Error(aContext, KErrCouldNotDisconnect);
		}

	aContext.ReleaseAndUnbind();

	// just go
	aContext.ListeningSAP()->DeleteChild(&aContext);
	}


//----------------------------------------------------------------------------------

TSCOLinkStateWaitForStartError::TSCOLinkStateWaitForStartError(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateWaitForStartError");
	}

void TSCOLinkStateWaitForStartError::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG(_L("CBTSyncLink: wait for start error state shutdown, immediate"));
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	if (aCloseType == CServProviderBase::ENormal)
		{
		aContext.Socket()->CanClose();
		}
	}

void TSCOLinkStateWaitForStartError::Start(CBTSynchronousLink& aContext) const
	{
	// summat went wrong - close and remove this acceptor
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	aContext.Socket()->Disconnect();

	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TSCOLinkStateWaitForStartError::Deletion(CBTSynchronousLink& aContext) const
 	{
 	// allowed deletion - should be listening SAP that is deleting us from
 	// an unwanted connection
 	aContext.ReleaseAndUnbind();
 	aContext.ListeningSAP()->RemoveChild(&aContext);
 	}

//----------------------------------------------------------------------------------

TSCOLinkStateListening::TSCOLinkStateListening(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateListening");
	}

void TSCOLinkStateListening::Deletion(CBTSynchronousLink& aContext) const
	{
	delete aContext.iChild;
	}

void TSCOLinkStateListening::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG(_L("CBTSyncLink: listening state shutdown, immediate"));
	if (aContext.iChild)
		{
		aContext.iChild->ParentClosing();
		}
	
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	if (aCloseType == CServProviderBase::ENormal)
		{
		aContext.Socket()->CanClose();
		}
	}

TBool TSCOLinkStateListening::ConnectRequest(CBTSynchronousLink& aContext, const TBTConnect& /*aSCOLink*/, const CPhysicalLink& aPhysicalLink) const
	{
	// only support one 'link' (HCI-wise!) at moment
	TRAPD(err, SpawnL(aContext, const_cast<CPhysicalLink&>(aPhysicalLink)));
	return (err==KErrNone);
	// don't transition state yet - connection may fail.
	};


void TSCOLinkStateListening::SpawnL(CBTSynchronousLink& aContext, CPhysicalLink& aPhysicalLink) const
	{
	CBTSynchronousLink* child;
	switch (aContext.iLinkType)
		{
		case ESCOLink:
			child = CSCOLink::NewLC(aContext.iLinksMan, NULL);
			break;
		default:
			child = CeSCOLink::NewLC(aContext.iLinksMan, NULL);
			break;
		}

	// configure the SAP and prepare PHY for SCO link
	User::LeaveIfError(child->BindLink(child->iLinkType, aPhysicalLink));

	// tell child about parent
	child->ListeningSAP() = &aContext;

	// and tell parent about child
	aContext.iChild = child;
	CleanupStack::Pop(child);

	aPhysicalLink.OverridePark();

	child->iRemoteDev = aPhysicalLink.BDAddr();

	// transition for child is Closed->Accepting
	ChangeState(*child, CSyncLinkStateFactory::EAccepting);
	}

void TSCOLinkStateListening::Exit(CBTSynchronousLink& aContext) const
	{
	aContext.iLinksMan.RemoveListener(aContext);
	}


//----------------------------------------------------------------------------------

TSCOLinkStateAccepting::TSCOLinkStateAccepting(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateAccepting");
	}

void TSCOLinkStateAccepting::Enter(CBTSynchronousLink& aContext) const
	{
	// start watchdog
	aContext.iAcceptWatchdog.Start();
	}

void TSCOLinkStateAccepting::Exit(CBTSynchronousLink& aContext) const
	{
	// Ensure watchdog is cancelled (calling Cancel twice is OK)
	aContext.iAcceptWatchdog.Cancel();
	}
	
void TSCOLinkStateAccepting::ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& aSyncOpts) const
	{
	// cancel watchdog
	aContext.iAcceptWatchdog.Cancel();

	aContext.iHandle = aSCOInfo.iConnH;
	aContext.iRemoteDev = aSCOInfo.iBdaddr;
	aContext.SetExtOptions(aSyncOpts);

	if (aContext.iClosePending)
		{
		//aContext.Shutdown(CServProviderBase::EImmediate);//this just resets iClosePending to ETrue
		TInt err = aContext.CommonShutdown();

		if (err)
			{
			aContext.Socket()->Error(KErrCouldNotDisconnect, MSocketNotify::EErrorClose);
			ChangeState(aContext, CSyncLinkStateFactory::EOpen);
			}
		else
			{
			// wait for disconnection complete before anything else
			ChangeState(aContext, CSyncLinkStateFactory::EClosing);
			}
		}
	else
		{
		// let the listening Socket know...
		__ASSERT_DEBUG(aContext.iParent, PanicInState(EBTSCOSAPParentlessChild));
		__ASSERT_DEBUG(aContext.iParent->Socket(), PanicInState(EBTSCOSAPNullSocket));
		ChangeState(aContext, CSyncLinkStateFactory::EWaitForStart);
		aContext.iParent->Socket()->ConnectComplete(aContext);	// tell listening socket about its new SAP
		}
	}

void TSCOLinkStateAccepting::Error(CBTSynchronousLink& aContext, TInt /*aError*/) const
	{
	// cancel watchdog
	aContext.iAcceptWatchdog.Cancel();

	aContext.ReleaseAndUnbind();

	// aContext will be deleted
	aContext.ListeningSAP()->DeleteChild(&aContext);
	}

void TSCOLinkStateAccepting::Deletion(CBTSynchronousLink& aContext) const
	{
	// cancel watchdog
	aContext.iAcceptWatchdog.Cancel();

	// allowed deletion
	aContext.ReleaseAndUnbind();
	aContext.ListeningSAP()->RemoveChild(&aContext);
	}

void TSCOLinkStateAccepting::Timeout(CBTSynchronousLink& aContext, TBasebandTimeout aTimeout) const
	{
	if (aTimeout == EAccept)
		{
		// the accept never worked - give up
		Error(aContext, KErrTimedOut);
		}
	}

void TSCOLinkStateAccepting::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType IF_FLOGGING(aCloseType)) const
	{
	LOG1(_L("CBTSyncLink: accepting state shutdown, just take a note. Type: %d"), aCloseType);
	aContext.iClosePending = ETrue;
	}

void TSCOLinkStateAccepting::Disconnection(CBTSynchronousLink& aContext) const
	{
	LOG(_L("CBTSyncLink: accepting state and PHY has disconnected."));
	Error(aContext, KErrDisconnected);
	}

void TSCOLinkStateAccepting::ParentClosing(CBTSynchronousLink& aContext) const
	{
	aContext.iClosePending = ETrue;
	}


//----------------------------------------------------------------------------------

TSCOLinkStateOpen::TSCOLinkStateOpen(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateOpen");
	}

void TSCOLinkStateOpen::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	TInt err = aContext.CommonShutdown();

	if (err)
		{
		aContext.Socket()->Error(KErrCouldNotDisconnect, MSocketNotify::EErrorClose);
		}
	else
		{
		if (aCloseType == CServProviderBase::ENormal)
			{
			LOG(_L("CBTSyncLink: open state shutdown, wait for link down"));

			// wait for disconnection complete before anything else
			ChangeState(aContext, CSyncLinkStateFactory::EClosing);
			}
		else
			{
			LOG(_L("CBTSyncLink: open state shutdown, immediate"));

			aContext.ReleaseAndUnbind();
			aContext.iLinksMan.Baseband().UpdateModelForDisconnection(aContext.iHandle, aContext.LinkType());
			ChangeState(aContext, CSyncLinkStateFactory::EClosed);
			}
		}
	}

TUint TSCOLinkStateOpen::Write(CBTSynchronousLink& aContext, const TDesC8& aData, TUint aOptions, TSockAddr* aAddr) const
	{
	TUint retVal = aContext.DoWrite(aData, aOptions, aAddr);
	return retVal;
	}

void TSCOLinkStateOpen::Disconnection(CBTSynchronousLink& aContext) const
	{
	// we were open, go to closed	
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	aContext.Socket()->Disconnect();
	}

void TSCOLinkStateOpen::Error(CBTSynchronousLink& aContext, TInt /*aErr*/) const
	{
	// the link has gone - go through disconnect path
	aContext.Disconnection();
	}

void TSCOLinkStateOpen::PacketTypeChange(CBTSynchronousLink& __DEBUG_ONLY(aContext), THCIErrorCode aErr, THCIConnHandle __DEBUG_ONLY(aConnH), TUint16 /*aNewPacketMask*/) const
	{
	__ASSERT_DEBUG(aConnH == aContext.Handle(), PanicInState(EBTSCOSAPWrongSCOSAP));

	if (aErr == KErrNone)
		{
		// in 1.1 this won't happen - for 1.2 eSCO do something here
		}
	else
		{
		// dump
		}
	}

void TSCOLinkStateOpen::DataReceived(CBTSynchronousLink& aContext, const TDesC8& aData) const
	{
	__ASSERT_DEBUG(aData.Size() <= KInboundSCODataSize, Panic(ESCOInboundPacketTooLarge));
	aContext.InboundFrame() = aData;

	// signal (stream) socket of number of bytes we have
	aContext.Socket()->NewData(aData.Length());
	}

TBool TSCOLinkStateOpen::IsOpen() const
	{
	return ETrue;
	}

void TSCOLinkStateOpen::Exit(CBTSynchronousLink& /*aContext*/) const
	{
	}


//----------------------------------------------------------------------------------

TSCOLinkStateClosing::TSCOLinkStateClosing(CSyncLinkStateFactory& aFactory)
: TSyncLinkState(aFactory)
	{
	STATENAME("TSCOLinkStateClosing");
	}

void TSCOLinkStateClosing::Shutdown(CBTSynchronousLink& aContext, CServProviderBase::TCloseType aCloseType) const
	{
	LOG(_L("CBTSyncLink: closing state shutdown, immediate"));

	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	if ((aCloseType == CServProviderBase::ENormal) && (aContext.iSocket))
		{
		aContext.Socket()->CanClose();
		}
	}

void TSCOLinkStateClosing::ActiveOpen(CBTSynchronousLink& aContext) const
	{
	// erk - have been asked to Open as we're closing down
	// the link hasn't gone yet, so just say it's there!
	ChangeState(aContext, CSyncLinkStateFactory::EOpen);
	aContext.Socket()->ConnectComplete();
	}


void TSCOLinkStateClosing::Error(CBTSynchronousLink& aContext, TInt /*aError*/) const
	{
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	}

void TSCOLinkStateClosing::Disconnection(CBTSynchronousLink& aContext) const
	{
	LOG(_L("CBTSyncLink: disconnection notification in closing state"));

	// as expected
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);
	if (aContext.iSocket)
		{
		aContext.Socket()->CanClose();
		}
	// Otherwise just swallow the event...
	}

TBool TSCOLinkStateClosing::IsIdle() const
	{
	return ETrue;
	}

void TSCOLinkStateClosing::ConnectComplete(CBTSynchronousLink& aContext, const TBTConnect& aSCOInfo, const TBTSyncConnectOpts& /*aSyncOpts*/) const
	{
	LOG(_L("CBTSyncLink: connection notification in closing state"));

	// This connection is no longer required, so close the socket
	ChangeState(aContext, CSyncLinkStateFactory::EClosed);

		
	//The call path to this ConnectComplete will have included CPhysicalLink::ConnectionComplete.
	//CPhysicalLink::ConnectionComplete updates the baseband model to show a new SCO link is in place
	//But if the SCO link is in state TSCOLinkStateClosing, setup of the new link won't occur. So the model 
	//should be updated again. If it isn't updated, it won't be possible to add a new SCO link, because the 
	//model will indicate that one already exists.
	aContext.iLinksMan.Baseband().UpdateModelForDisconnection(aSCOInfo.iConnH, aSCOInfo.iLinkType);	
	
	//Even though the SAP is now EClosed, the baseband might have a SCO link to the remote device.
	//The only way to be sure is to send a Disconnect. The transition to EClosed at the will have 
	//unbound this SAP from the PHY, so if the remote device responds to the Disconnect, the response
	//will be thrown away.
	TRAP_IGNORE(aContext.iLinksMan.HCIFacade().DisconnectL(aSCOInfo.iConnH, ERemoteUserEndedConnection));

	if (aContext.iSocket)
		{
		aContext.Socket()->CanClose();
		}
	// socket could be deleted now so be careful
	}

void TSCOLinkStateClosing::Deletion(CBTSynchronousLink& /*aContext*/) const
 	{
 	// allowed
 	}

//-----------------------------------------------------------------------------------

