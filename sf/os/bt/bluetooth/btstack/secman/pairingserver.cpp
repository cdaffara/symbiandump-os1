// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <e32base.h>
#include <bttypes.h>
#include <bluetooth/pairing.h>
#include <bluetooth/pairingservershared.h>

#include "pairingserversecuritypolicy.h"
#include "oobdata.h"
#include "pairingserver.h"

#include <bluetooth/logger.h>
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAIRING_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("pairingsrv");
#endif

// These come after the logging to ensure that debug.h knows we're using the
// bluetooth logger.
#include "physicallinksmanager.h"
#include "ProxySAP.h"

//
// CPairingServer
//

CPairingServer* CPairingServer::NewL(COobDataManager& aOobDataManager, CSimplePairingResultList& aSimplePairingResultList, CAuthenticationResultList& aAuthenticationResultList)
	{
	LOG_STATIC_FUNC

	CPairingServer* self = new(ELeave) CPairingServer(aOobDataManager, aSimplePairingResultList, aAuthenticationResultList);
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance
	// of the same server running, so do it before ConstructL.
	self->StartL(KPairingServerName);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPairingServer::~CPairingServer()
	{
	LOG_FUNC
	delete iContainerIndex;
	}

CPairingServer::CPairingServer(COobDataManager& aOobDataManager, CSimplePairingResultList& aSimplePairingResultList, CAuthenticationResultList& aAuthenticationResultList)
	: CPolicyServer(CActive::EPriorityStandard, KPairingServerPolicy)
	, iOobDataManager(aOobDataManager)
	, iSimplePairingResultList(aSimplePairingResultList)
	, iAuthenticationResultList(aAuthenticationResultList)
	{
	LOG_FUNC
	}

void CPairingServer::ConstructL()
	{
	LOG_FUNC
	iContainerIndex = CObjectConIx::NewL();
	}

// Callback to LinksMgrProtocol to add a pairing session
void CPairingServer::AddSession()
	{
	LOG_FUNC
	if(iSessionCount++ == 0)
		{
		// While we have clients we need to make sure that the protocol remains alive.
		iLinkMgrProtocol->LocalOpen();
		}
	}

// Callback to LinksMgrProtocol to remove a pairing session
void CPairingServer::DropSession()
	{
	LOG_FUNC
	if(--iSessionCount == 0)
		{
		// There are no longer any clients
		iLinkMgrProtocol->LocalClose();
		}
	}

void CPairingServer::SetPhysicalLinksManager(CPhysicalLinksManager& aLinksMan)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iLinksMan, PANIC(KPairingServerFaultCat, EPairingServerLinkManagerAlreadyProvided));
	iLinksMan = &aLinksMan;
	}

void CPairingServer::ClearPhysicalLinkMgr()
	{
	LOG_FUNC
	iLinksMan = NULL;
	}

// Save a reference to the LinksMgrProtocol
void CPairingServer::SetLinksMgrProtocol(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_FUNC
	iLinkMgrProtocol = &aLinkMgrProtocol;
	}

// Clear the reference to the LinksMgrProtocol
void CPairingServer::ClearLinksMgrProtocol()
	{
	LOG_FUNC
	iLinkMgrProtocol = NULL;
	}

CSession2* CPairingServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	LOG_FUNC
	LOG3(_L("aVersion = (%d, %d, %d)"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild)

	// Version number check...
	TVersion v(KPairingServerMajorVersionNumber,
			   KPairingServerMinorVersionNumber,
			   KPairingServerBuildVersionNumber);
	if(!User::QueryVersionSupported(v, aVersion))
		{
		User::LeaveIfError(KErrNotSupported);
		}

	// Ensure that the reference to the LinksMgrProtocol has been receieved
	if(!iLinkMgrProtocol)
		{
		User::Leave(KErrNotReady);
		}
	// Cast 'this' to allow use in Constructor 
	CPairingServer* ncThis = const_cast<CPairingServer*>(this);
	CPairingSession* sess = CPairingSession::NewL(*ncThis);
	return sess;
	}

CObjectCon* CPairingServer::NewContainerL()
	{
	LOG_FUNC
	return iContainerIndex->CreateL();
	}

void CPairingServer::DeleteContainer(CObjectCon* aContainer)
	{
	LOG_FUNC
	iContainerIndex->Remove(aContainer);
	}

CPhysicalLinksManager& CPairingServer::LinksManL() const
	{
	LOG_FUNC
	if(!iLinksMan)
		{
		LEAVEL(KErrNotReady);
		}
	return *iLinksMan;
	}

COobDataManager& CPairingServer::OobDataManager() const
	{
	LOG_FUNC
	return iOobDataManager;
	}

CSimplePairingResultList& CPairingServer::SimplePairingResultList() const
	{
	LOG_FUNC
	return iSimplePairingResultList;
	}

CAuthenticationResultList& CPairingServer::AuthenticationResultList() const
	{
	LOG_FUNC
	return iAuthenticationResultList;
	}

//
// CPairingSession
//

CPairingSession* CPairingSession::NewL(CPairingServer& aPairingServer)
	{
	LOG_STATIC_FUNC
	CPairingSession* self = new(ELeave) CPairingSession(aPairingServer);
	aPairingServer.AddSession();
	return self;
	}

CPairingSession::CPairingSession(CPairingServer& aPairingServer)
	: iPairingServer(aPairingServer)
	{
	LOG_FUNC
	}

CPairingServer& CPairingSession::Server() const
	{
	LOG_FUNC
	return *static_cast<CPairingServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

CPairingSession::~CPairingSession()
	{
	LOG_FUNC
	iPairingServer.DropSession();
	delete iSubSessions; // Must be deleted before container to ensure ref counting is correct
	Server().DeleteContainer(iContainer);
	}

void CPairingSession::CreateL()
	{
	LOG_FUNC
	iSubSessions = CObjectIx::NewL();
	iContainer = Server().NewContainerL();
	__TEST_INVARIANT;
	}

#ifdef _DEBUG
void CPairingSession::__DbgTestInvariant() const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iContainer->Count() == iSubSessions->ActiveCount(),
			PANIC(KPairingServerFaultCat, EPairingServerSessionContainerAndIndexMismatch));
	}
#endif // _DEBUG

void CPairingSession::ServiceL(const RMessage2& aMessage)
	{
	LOG_FUNC
	LOG1(_L("aMessage.Function() = &d"), aMessage.Function());
//	__TEST_INVARIANT; // Not needed as all major functional exits are checked
	TBool handled = DispatchSessMessageL(aMessage);
	if(!handled)
		{
		// if not handled, it must be a subsession request.
		DispatchSubSessMessageL(aMessage);
		}
	__TEST_INVARIANT;
	}

void CPairingSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	LOG_FUNC
	LOG1(_L("aError = %d"), aError);
	if(aError == KErrBadDescriptor)
		{
		// If a KErrBadDescriptor is received then the request is bad so punish.
		aMessage.Panic(KPairingServerPanicCat, EPairingServerBadDescriptor);
		}
	else
		{
		aMessage.Complete(aError);
		}
	__TEST_INVARIANT;
	}

TBool CPairingSession::DispatchSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	TBool handled = ETrue;
	switch(aMessage.Function())
		{
	case EPairingServerCreateOobDataSubSession:
		NewOobDataSubSessionL(aMessage);
		break;

	case EPairingServerCreateDedicatedBondingSubSession:
		NewDedicatedBondingSubSessionL(aMessage);
		break;

	case EPairingServerCreateSimplePairingResultSubSession:
		NewSimplePairingResultSubSessionL(aMessage);
		break;

	case EPairingServerCreateAuthenticationResultSubSession:
		NewAuthenticationResultSubSessionL(aMessage);
		break;

	case EPairingServerCloseSubSession:
		DeleteSubSession(aMessage);
		break;

	default:
		handled = EFalse;
		break;
		}
	return handled;
	}

void CPairingSession::DispatchSubSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	CPairingSubSession* ss = SubSessionFromHandle(aMessage.Int3());
	if(!ss)
		{
		aMessage.Panic(KPairingServerPanicCat, EPairingServerBadSubSessionHandle);
		}
	else
		{
		ss->DispatchSubSessMessageL(aMessage);
		}
	}

CPairingSubSession* CPairingSession::SubSessionFromHandle(TInt aHandle)
	{
	LOG_FUNC
	LOG1(_L("aHandle = %d"), aHandle);
	return static_cast<CPairingSubSession*>(iSubSessions->At(aHandle));
	}

void CPairingSession::NewOobDataSubSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC
	COobDataSession* oob = COobDataSession::NewLC(*this, Server().OobDataManager());
	SubSessionConstructL(aMessage, oob); // handles oob on CleanupStack
	}

void CPairingSession::NewDedicatedBondingSubSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC
	CDedicatedBondingSession* ded = CDedicatedBondingSession::NewLC(*this, Server().LinksManL());
	SubSessionConstructL(aMessage, ded); // handles ded on CleanupStack
	}

void CPairingSession::NewSimplePairingResultSubSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC
	CSimplePairingResultSession* sim = CSimplePairingResultSession::NewLC(*this, Server().SimplePairingResultList());
	SubSessionConstructL(aMessage, sim);
	}


void CPairingSession::NewAuthenticationResultSubSessionL(const RMessage2& aMessage)
	{
	LOG_FUNC
	CAuthenticationResultSession* aut = CAuthenticationResultSession::NewLC(*this, Server().AuthenticationResultList());
	SubSessionConstructL(aMessage, aut);
	}

/**
This function handles the subsession handle binding to the subsession that has been
created.
*/
void CPairingSession::SubSessionConstructL(const RMessage2& aMessage, CPairingSubSession* aSubSession)
	{
	LOG_FUNC
	// The subsession is on the cleanup stack when this function is called.
	TInt subSessionHandleLength = aMessage.GetDesMaxLengthL(3);
	if(subSessionHandleLength != sizeof(TInt))
		{
		aMessage.Panic(KPairingServerPanicCat, EPairingServerBadDescriptor);
		CleanupStack::PopAndDestroy(aSubSession);
		}
	else
		{
		iContainer->AddL(aSubSession);
		TInt handle = iSubSessions->AddL(aSubSession);
		CleanupStack::Pop(aSubSession); // Now lifetime is controlled by the binding CObjectIx
		TPckg<TInt> pckgHandle(handle);
		TRAPD(err, aMessage.WriteL(3, pckgHandle));
		if(err != KErrNone)
			{
			iSubSessions->Remove(handle);
			LEAVEL(err);
			}
		aMessage.Complete(KErrNone);
		}
	}

void CPairingSession::DeleteSubSession(const RMessage2& aMessage)
	{
	LOG_FUNC
	TInt handle = aMessage.Int3();
	CPairingSubSession* ss = SubSessionFromHandle(handle);
	if(!ss)
		{
		aMessage.Panic(KPairingServerPanicCat, EPairingServerBadSubSessionHandle);
		}
	else
		{
		iSubSessions->Remove(handle);
		aMessage.Complete(KErrNone);
		}
	}


//
// CPairingSubSession
//

CPairingSubSession::CPairingSubSession(CPairingSession& aSession)
	: iSession(aSession)
	{
	LOG_FUNC
	}

CPairingSubSession::~CPairingSubSession()
	{
	LOG_FUNC
	}

void CPairingSubSession::DispatchSubSessMessageL(const RMessage2& /*aMessage*/)
	{
	LOG_FUNC
	LEAVEL(KErrNotSupported);
	}

void CPairingSubSession::ConstructL()
	{
	LOG_FUNC
	}

CPairingSession& CPairingSubSession::Session() const
	{
	LOG_FUNC
	return iSession;
	}


//
// COobDataSession
//

COobDataSession* COobDataSession::NewLC(CPairingSession& aSession, COobDataManager& aOobDataManager)
	{
	LOG_STATIC_FUNC
	COobDataSession* self = new(ELeave) COobDataSession(aSession, aOobDataManager);
	//As its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	return self;
	}

COobDataSession::COobDataSession(CPairingSession& aSession, COobDataManager& aOobDataManager)
	: CPairingSubSession(aSession)
	, iOobDataManager(aOobDataManager)
	{
	LOG_FUNC
	}

COobDataSession::~COobDataSession()
	{
	LOG_FUNC
	TryCancelReadLocalOobData();
	}

COobDataManager& COobDataSession::OobDataManager() const
	{
	LOG_FUNC
	return iOobDataManager;
	}

void COobDataSession::DispatchSubSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	switch(aMessage.Function())
		{
	case EPairingServerOobDataRefreshLocal:
		iOobDataManager.RefreshLocalOobData();
		aMessage.Complete(KErrNone);
		break;
	case EPairingServerOobDataReadLocal:
		ReadLocalOobDataL(aMessage);
		break;
	case EPairingServerOobDataCancelReadLocal:
		CancelReadLocalOobData(aMessage);
		break;
	case EPairingServerOobDataProvideRemoteParsed:
		ProvideParsedRemoteOobDataL(aMessage);
		break;
	case EPairingServerOobDataProvideRemoteRaw:
		ProvideRawRemoteOobDataL(aMessage);
		break;
	case EPairingServerOobDataClearRemote:
		ClearRemoteOobDataL(aMessage);
		break;
	default:
		CPairingSubSession::DispatchSubSessMessageL(aMessage);
		break;
		}
	}

void COobDataSession::ProvideParsedRemoteOobDataL(const RMessage2& aMessage)
	{
	LOG_FUNC

	TPckgBuf<TBTDevAddr> addrBuf;
	if(aMessage.GetDesLengthL(0) != sizeof(TBTDevAddr))
		{
		LEAVEL(KErrBadDescriptor);
		}
	aMessage.ReadL(0, addrBuf);

	TBluetoothSimplePairingHash hash;
	if(aMessage.GetDesLengthL(1) != KBluetoothSimplePairingHashSize)
		{
		LEAVEL(KErrBadDescriptor);
		}
	aMessage.ReadL(1, hash);

	TBluetoothSimplePairingRandomizer randomizer;
	TInt len = aMessage.GetDesLength(2);
	if (len < KErrNone) // The randomizer wasn't available (which is fine...)
		{
		randomizer.FillZ(KBluetoothSimplePairingRandomizerSize);
		}
	else
		{
		if(len != KBluetoothSimplePairingRandomizerSize)
			{
			LEAVEL(KErrBadDescriptor);
			}
		aMessage.ReadL(2, randomizer);
		}

	TOobData oobData(addrBuf(), hash, randomizer);
	OobDataManager().ProvideRemoteOobDataL(oobData);
	aMessage.Complete(KErrNone);
	}

void COobDataSession::ProvideRawRemoteOobDataL(const RMessage2& aMessage)
	{
	LOG_FUNC
	//Do a quick check to ensure there is at least enough data to get the BT address out.
	if(aMessage.GetDesLengthL(0) < KOOBOptionalDataOffset)
		User::Leave(KErrUnderflow);

	RBuf8 rawBuf;
	rawBuf.CreateL(aMessage.GetDesMaxLengthL(0));
	rawBuf.CleanupClosePushL();
	//Read the optional oob data to rawbuf so it can be parsed to the data block parser
	aMessage.ReadL(0, rawBuf, KOOBOptionalDataOffset);
	rawBuf.SetMax();

	//We need to get the address out manually, EIR can't parse the address an no EIR device
	//address tag is defined. We are currently ignoring the length and anything other than
	//the pairing hash & randomizer.
	TBuf8<KBTDevAddrSize> addrBuf;
	aMessage.ReadL(0, addrBuf, KOOBBtAddrOffset);
	TBTDevAddr addr(addrBuf);

	//Any remaining data is optional and will be in EIR format so we can use the
	//EIR object to parse the data properly.
	TExtendedInquiryResponseDataCodec dataBlockParser(rawBuf);

	TPtrC8 hashBuf;
	LEAVEIFERRORL(dataBlockParser.GetData(EEirOobSimplePairingHash, hashBuf));
	if(hashBuf.Length() != KBluetoothSimplePairingHashSize)
		{
		LEAVEL(KErrCorrupt);
		}
	TBluetoothSimplePairingHash hash(hashBuf);

	TPtrC8 randomizerBuf;
	TBluetoothSimplePairingRandomizer randomizer;
	TInt err = dataBlockParser.GetData(EEirOobSimplePairingRandomizerR, randomizerBuf);
	if(err < KErrNone) // The randomizer wasn't available (which is fine...)
		{
		randomizer.FillZ(KBluetoothSimplePairingRandomizerSize);
		}
	else
		{
		if(randomizerBuf.Length() != KBluetoothSimplePairingRandomizerSize)
			{
			LEAVEL(KErrCorrupt);
			}
		randomizer.Copy(randomizerBuf);
		}

	// Mark this line as intentional as coverity raises a false positive error that hash
	// has not been initalised even though it is set to hashBuf a few lines above.
	// coverity[uninit_use_in_call]
	TOobData oobData(addr, hash, randomizer);
	OobDataManager().ProvideRemoteOobDataL(oobData);

	CleanupStack::PopAndDestroy(&rawBuf); // finished with the backing buffer

	aMessage.Complete(KErrNone);
	}

void COobDataSession::ClearRemoteOobDataL(const RMessage2& aMessage)
	{
	LOG_FUNC
	TPckgBuf<TBTDevAddr> addrBuf;
	if(aMessage.GetDesLengthL(0) != sizeof(TBTDevAddr))
		{
		LEAVEL(KErrBadDescriptor);
		}
	aMessage.ReadL(0, addrBuf);

	OobDataManager().ClearRemoteOobData(addrBuf());
	aMessage.Complete(KErrNone);
	}

void COobDataSession::ReadLocalOobDataL(const RMessage2& aMessage)
	{
	LOG_FUNC
	if(iReadLocalOobDataMsg.Handle())
		{
		aMessage.Panic(KPairingServerPanicCat, EPairingServerReadLocalOobDataOutstanding);
		return;
		}
	iReadLocalOobDataMsg = aMessage; // Remember which request to complete.
	OobDataManager().ReadLocalOobData(*this);
	}

void COobDataSession::CancelReadLocalOobData(const RMessage2& aMessage)
	{
	LOG_FUNC
	TryCancelReadLocalOobData();
	aMessage.Complete(KErrNone);
	}

void COobDataSession::TryCancelReadLocalOobData()
	{
	LOG_FUNC
	if(iReadLocalOobDataMsg.Handle())
		{
		OobDataManager().CancelReadLocalOobData(*this);
		}
	}

void COobDataSession::XoldoLocalOobDataRetrieved(TInt aResult, const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iReadLocalOobDataMsg.Handle(),
			PANIC(KPairingServerFaultCat, EPairingServerNoMessageForObserverCompletion));

	TInt err = aResult;
	if(err == KErrNone)
		{
		TRAP(err, LocalOobDataRetrievedL(aHash, aRandomizer));
		}
	if(err == KErrBadDescriptor)
		{
		iReadLocalOobDataMsg.Panic(KPairingServerPanicCat, EPairingServerBadDescriptor);
		}
	else
		{
		iReadLocalOobDataMsg.Complete(err);
		}
	}

void COobDataSession::LocalOobDataRetrievedL(const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer)
	{
	LOG_FUNC
	iReadLocalOobDataMsg.WriteL(0, aHash);
	iReadLocalOobDataMsg.WriteL(1, aRandomizer);
	}


//
// CDedicatedBondingSession
//

CDedicatedBondingSession* CDedicatedBondingSession::NewLC(CPairingSession& aSession, CPhysicalLinksManager& aPhysicalLinksManager)
	{
	LOG_STATIC_FUNC
	CDedicatedBondingSession* self = new(ELeave) CDedicatedBondingSession(aSession, aPhysicalLinksManager);
	//As its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	return self;
	}

CDedicatedBondingSession::CDedicatedBondingSession(CPairingSession& aSession, CPhysicalLinksManager& aPhysicalLinksManager)
	: CPairingSubSession(aSession)
	, iPhysicalLinksManager(aPhysicalLinksManager)
	{
	LOG_FUNC
	}

void CDedicatedBondingSession::ConstructL()
	{
	LOG_FUNC
	CPairingSubSession::ConstructL();
	TCallBack cb(CDedicatedBondingSession::StaticShutdown, this);
	iAsyncShutdown = new(ELeave) CAsyncCallBack(cb, CActive::EPriorityStandard);
	}

CDedicatedBondingSession::~CDedicatedBondingSession()
	{
	LOG_FUNC
	iPhysicalLinksManager.SecMan().CancelRequest(*this);
	delete iProxySap; // cannot do an immediate shutdown as the semantics for that kill the phy
	if(!iStartBondingMsg.IsNull())
		{
		iStartBondingMsg.Complete(KErrCancel);
		}
	delete iAsyncShutdown;
	}

void CDedicatedBondingSession::DispatchSubSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	switch(aMessage.Function())
		{
	case EPairingServerStartDedicatedBond:
		StartBondingL(aMessage);
		break;
		
	default:
		CPairingSubSession::DispatchSubSessMessageL(aMessage);
		break;
		}
	}

void CDedicatedBondingSession::Complete(TInt aError)
	{
	LOG_FUNC
	if(!iStartBondingMsg.IsNull())
		{
		__ASSERT_DEBUG(!ShuttingDown(), PANIC(KPairingServerFaultCat, EPairingServerBadShutdownState));
		iState = EShutdownRequested;
		iAsyncShutdown->CallBack();
		iPhysicalLinksManager.SecMan().CancelRequest(*this); // we don't want the result anymore (if it's still pending).
		iStartBondingMsg.Complete(aError); 
		}
	else
		{
		__ASSERT_DEBUG(ShuttingDown(), PANIC(KPairingServerFaultCat, EPairingServerBadShutdownState));
		}
	}

TInt CDedicatedBondingSession::StaticShutdown(TAny* aDedBond)
	{
	LOG_STATIC_FUNC
	reinterpret_cast<CDedicatedBondingSession*>(aDedBond)->Shutdown();
	return KErrNone;
	}

void CDedicatedBondingSession::Shutdown()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iState == EShutdownRequested, PANIC(KPairingServerFaultCat, EPairingServerBadShutdownState));
	__ASSERT_DEBUG(iStartBondingMsg.IsNull(), PANIC(KPairingServerFaultCat, EPairingServerMessageShouldBeNull));
	iState = EShutdownPending;
	iProxySap->Shutdown(CServProviderBase::ENormal);
	}
	
TBool CDedicatedBondingSession::ShuttingDown() const
	{
	return iState == EShutdownRequested || iState == EShutdownPending;
	}

void CDedicatedBondingSession::StartBondingL(const RMessage2& aMessage)
	{
	LOG_FUNC

	if(iState != EMintCondition)
		{
		aMessage.Panic(KPairingServerPanicCat, EPairingServerDedicatedBondAlreadyInProgress);
		return;
		}
	__ASSERT_DEBUG(iStartBondingMsg.IsNull(), PANIC(KPairingServerFaultCat, EPairingServerMessageShouldBeNull));
	
	// Now we've been initiated, no turning back for this object.
	iState = EInitiated;
	
	TPckgBuf<TBTDevAddr> addrBuf;
	TInt addrLen = aMessage.GetDesLengthL(0);
	if(addrLen != sizeof(TBTDevAddr))
		{
		// If the length is incorrect then the address has been packaged incorrectly for the
		// IPC operation.
		LEAVEL(KErrBadDescriptor);
		}
	aMessage.ReadL(0, addrBuf);

	TBTSockAddr addr;
	addr.SetBTAddr(addrBuf());
	iProxySap = CBTProxySAP::NewL(iPhysicalLinksManager, NULL);
	
	iStartBondingMsg = aMessage;
	// Now we've entered the realm of not leaving with an error, since the connection
	// process has started.  Errors from now on must be via the Error() function call.
	
	iState = EInitialConnectionPending;
	iProxySap->SetNotify(this);
	iProxySap->SetRemName(addr);
	iProxySap->ActiveOpen();
	TRAPD(err, DoAccessRequestL());
	if(err != KErrNone)
		{
		Error(err);
		}
	}

void CDedicatedBondingSession::DoAccessRequestL()
	{
	LOG_FUNC
	TBTServiceSecurity security;
	security.SetAuthentication(EMitmDesired);
	security.SetUid(KBluetoothDedicatedBondingUid);

	iPhysicalLinksManager.SecMan().AccessRequestL(security, NULL, iProxySap->RemoteAddress(), EDedicatedBonding, EFalse, *this);
	}

void CDedicatedBondingSession::AccessRequestComplete(TInt aResult)
	{
	LOG_FUNC
	TInt err = aResult;
	TBool completed = EFalse;
	TBTSockAddr addr;
	switch (iState)
		{
	case EZombie:
		// Ready for a retry.
		if(aResult == EBTSecManAccessDeferred)
			{
			iState = EFinalConnectionPending;
			addr.SetBTAddr(iProxySap->RemoteAddress());
			iProxySap->SetRemName(addr); // triggers finding a link again.
			iProxySap->ActiveOpen();
			TRAPD(err, DoAccessRequestL());
			if(err != KErrNone)
				{
				Error(err);
				}
			break;
			}
		// else not deferred so complete now....
		// fall-through...
	case EInitialConnection:
		ASSERT_DEBUG(aResult != EBTSecManAccessDeferred); // Should have been disconnected if we receive
														  // this - I don't expect this to happen.
		// fall-through...
	case EFinalConnection:
		completed = ETrue; // in the final connection any complete is errored.
		if(aResult == EBTSecManAccessDenied)
			{
			err = KErrAccessDenied;
			}
		break;
		
	case EInitialConnectionPending:
		// fall-through deliberate
	case EFinalConnectionPending:
		// Access request shouldn't successfully complete if the connection is still pending
		__ASSERT_DEBUG(aResult != EBTSecManAccessGranted,  PANIC(KPairingServerFaultCat, EPairingServerUnexpectedAccessCallback));
		// We should get the MSocketNotify::Error callback, so don't do anything else
		break;
		
	default:
		LOG1(_L("Unexpected Access Request Complete in state %d"), iState);
		__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedAccessCallback));
		break;
		}

	if (completed)
		{
		Complete(err);
		}
	}

void CDedicatedBondingSession::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	// We aren't using a raw conduit
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::CanSend()
	{
	LOG_FUNC
	// We aren't using a raw conduit
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::ConnectComplete()
	{
	LOG_FUNC
	switch(iState)
		{
	case EInitialConnectionPending:
		iState = EInitialConnection;
		break;
	case EFinalConnectionPending:
		iState = EFinalConnection;
		break;
		
	case EInitialConnection:
		// fall-through deliberate
	case EFinalConnection:
		// Apparently multiple connect completes are allowed by CSocket
		break;
		
	case EShutdownRequested:
		// fall-through deliberate
	case EShutdownPending:
		// If an error occurred just after the connection request then we
		// might receive a connection complete before the async shutdown request
		// has been executed or completed.
		break;
		
	default:
		LOG1(_L("Unexpected Connect Complete in state %d"), iState);
		__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
		break;
		}
	}

void CDedicatedBondingSession::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	ConnectComplete(); // Connection data isn't useful for us.
	}

void CDedicatedBondingSession::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	LOG_FUNC
	// ProxySAPs aren't used for passive connections.
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::ConnectComplete(CServProviderBase& /*aSSP*/, const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	// ProxySAPs aren't used for passive connections.
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::CanClose(TDelete aDelete)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iState == EShutdownPending, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	__ASSERT_DEBUG(iStartBondingMsg.IsNull(), PANIC(KPairingServerFaultCat, EPairingServerMessageShouldBeNull));
	iState = EShutdown;
	if (aDelete == EDelete)
		{
		delete iProxySap; iProxySap = NULL;
		}
	}

void CDedicatedBondingSession::CanClose(const TDesC8& /*aDisconnectData*/, TDelete aDelete)
	{
	LOG_FUNC
	CanClose(aDelete);
	}

void CDedicatedBondingSession::Error(TInt aError, TUint /*anOperationMask*/)
	{
	LOG_FUNC
	Complete(aError);
	}

void CDedicatedBondingSession::Disconnect()
	{
	LOG_FUNC
	switch (iState)
		{
	case EInitialConnection:
		// enter the zombie state and wait for the access requester to complete.
		iState = EZombie;
		break;
		
	case EFinalConnection:
		Error(KErrDisconnected);
		break;
		
	case EShutdownRequested:
		// fall-through deliberate
	case EShutdownPending:
		// Already closing down.
		break;
		
	default:
		LOG1(_L("Unexpected Disconnect in state %d"), iState);
		__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
		break;
		}
	}

void CDedicatedBondingSession::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	Disconnect();
	}

void CDedicatedBondingSession::IoctlComplete(TDesC8* /*aBuf*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::NoBearer(const TDesC8& /*aConnectionParams*/)
	{
	LOG_FUNC
	// No idea what this is for and proxy sap never calls it... so as far as I'm concerned it's
	// unexpected.
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}

void CDedicatedBondingSession::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	LOG_FUNC
	// No idea what this is for and proxy sap never calls it... so as far as I'm concerned it's
	// unexpected.
	__ASSERT_DEBUG(EFalse, PANIC(KPairingServerFaultCat, EPairingServerUnexpectedSocketCallback));
	}


//
// CSimplePairingResultSession
//

CSimplePairingResultSession* CSimplePairingResultSession::NewLC(CPairingSession& aSession, CSimplePairingResultList& aResultList)
	{
	LOG_STATIC_FUNC
	CSimplePairingResultSession* self = new(ELeave) CSimplePairingResultSession(aSession, aResultList);
	//As its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	return self;
	}

CSimplePairingResultSession::CSimplePairingResultSession(CPairingSession& aSession, CSimplePairingResultList& aResultList)
	: CPairingSubSession(aSession)
	, iResultList(aResultList)
	{
	LOG_FUNC
	}

void CSimplePairingResultSession::ConstructL()
	{
	LOG_FUNC
	CPairingSubSession::ConstructL();
	iResultList.RegisterObserverL(*this);
	}

CSimplePairingResultSession::~CSimplePairingResultSession()
	{
	LOG_FUNC
	iResultList.ReleaseObserver(*this);
	if(!iResultMsg.IsNull())
		{
		iResultMsg.Complete(KErrCancel);
		}
	}

void CSimplePairingResultSession::DispatchSubSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	switch(aMessage.Function())
		{
	case EPairingServerSimplePairingResult:
		SimplePairingResultL(aMessage);
		break;
	case EPairingServerCancelSimplePairingResult:
		CancelSimplePairingResult(aMessage);
		break;
	default:
		CPairingSubSession::DispatchSubSessMessageL(aMessage);
		break;
		}
	}

void CSimplePairingResultSession::SimplePairingResultL(const RMessage2& aMessage)
	{
	LOG_FUNC

	TPckgBuf<TBTDevAddr> addrBuf;
	if(aMessage.GetDesMaxLengthL(0) != sizeof(TBTDevAddr))
		{
		LEAVEL(KErrBadDescriptor);
		}

	if(!iResultMsg.IsNull())
		{
		LEAVEL(KErrInUse);
		}

	iResultMsg = aMessage;
	iResultList.ReturnResult();
	}

void CSimplePairingResultSession::CancelSimplePairingResult(const RMessage2& aMessage)
	{
	LOG_FUNC
	if(!iResultMsg.IsNull())
		{
		iResultList.CancelReturn();
		iResultMsg.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}

TInt CSimplePairingResultSession::MbsroResult(const TBTDevAddr& aDevAddr, TInt aResult)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iResultMsg.Handle(),
			PANIC(KPairingServerFaultCat, EPairingServerNoMessageForObserverCompletion));

	TRAPD(err, ReturnResultL(aDevAddr));
	if(err == KErrBadDescriptor)
		{
		iResultMsg.Panic(KPairingServerPanicCat, EPairingServerBadDescriptor);
		}
	else if(err != KErrNone)
		{
		iResultMsg.Complete(err);
		}
	else
		{
		iResultMsg.Complete(aResult);
		}
	return err;
	}

void CSimplePairingResultSession::ReturnResultL(const TBTDevAddr& aDevAddr)
	{
	LOG_FUNC
	TPckg<TBTDevAddr> pckg(aDevAddr);
	iResultMsg.WriteL(0, pckg);
	}


//
// CAuthenticationResultSession
//

CAuthenticationResultSession* CAuthenticationResultSession::NewLC(CPairingSession& aSession, CAuthenticationResultList& aResultList)
	{
	LOG_STATIC_FUNC
	CAuthenticationResultSession* self = new(ELeave) CAuthenticationResultSession(aSession, aResultList);
	//As its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);
	self->ConstructL();
	return self;
	}

CAuthenticationResultSession::CAuthenticationResultSession(CPairingSession& aSession, CAuthenticationResultList& aResultList)
	: CPairingSubSession(aSession)
	, iResultList(aResultList)
	{
	LOG_FUNC
	}

void CAuthenticationResultSession::ConstructL()
	{
	LOG_FUNC
	CPairingSubSession::ConstructL();
	iResultList.RegisterObserverL(*this);
	}

CAuthenticationResultSession::~CAuthenticationResultSession()
	{
	LOG_FUNC
	iResultList.ReleaseObserver(*this);
	if(!iResultMsg.IsNull())
		{
		iResultMsg.Complete(KErrCancel);
		}
	}

void CAuthenticationResultSession::DispatchSubSessMessageL(const RMessage2& aMessage)
	{
	LOG_FUNC
	switch(aMessage.Function())
		{
	case EPairingServerAuthenticationResult:
		AuthenticationResultL(aMessage);
		break;
	case EPairingServerCancelAuthenticationResult:
		CancelAuthenticationResult(aMessage);
		break;
	default:
		CPairingSubSession::DispatchSubSessMessageL(aMessage);
		break;
		}
	}

void CAuthenticationResultSession::AuthenticationResultL(const RMessage2& aMessage)
	{
	LOG_FUNC

	TPckgBuf<TBTDevAddr> addrBuf;
	if(aMessage.GetDesMaxLengthL(0) != sizeof(TBTDevAddr))
		{
		LEAVEL(KErrBadDescriptor);
		}

	if(!iResultMsg.IsNull())
		{
		LEAVEL(KErrInUse);
		}

	iResultMsg = aMessage;
	iResultList.ReturnResult();
	}

void CAuthenticationResultSession::CancelAuthenticationResult(const RMessage2& aMessage)
	{
	LOG_FUNC
	if(!iResultMsg.IsNull())
		{
		iResultList.CancelReturn();
		iResultMsg.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}

TInt CAuthenticationResultSession::MbsroResult(const TBTDevAddr& aDevAddr, TInt aResult)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iResultMsg.Handle(),
			PANIC(KPairingServerFaultCat, EPairingServerNoMessageForObserverCompletion));

	TRAPD(err, ReturnResultL(aDevAddr));
	if(err == KErrBadDescriptor)
		{
		iResultMsg.Panic(KPairingServerPanicCat, EPairingServerBadDescriptor);
		}
	else if(err != KErrNone)
		{
		iResultMsg.Complete(err);
		}
	else
		{
		iResultMsg.Complete(aResult);
		}
	return err;
	}

void CAuthenticationResultSession::ReturnResultL(const TBTDevAddr& aDevAddr)
	{
	LOG_FUNC
	TPckg<TBTDevAddr> pckg(aDevAddr);
	iResultMsg.WriteL(0, pckg);
	}


