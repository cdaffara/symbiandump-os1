// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <remconinterfaceselector.h>
#include <remcon/remconinterfacebase.h>
#include <remconerrorobserver.h>
#include <remcon/remconinterfacefeatures.h>
#include <remcon/remconifdetails.h>
#include <s32mem.h>
#include "bulkreceiver.h"
#include "receiver.h"
#include "remconclient.h"
#include "remconbulkclient.h"
#include "utils.h"


const TInt KMaxSharedThreadHeapSize = 0x0400;


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_IF_SEL);
#endif

#ifdef _DEBUG
PANICCATEGORY("ifsel");
#endif

#define RCIS_VERBOSE_PANIC(code) \
	{ \
	LOG1(_L("Panicking with RemConIfSel / %d"), code); \
	User::Panic(KRemConIfSelPanicCat, code); \
	}

#define RCIS_VERBOSE_ASSERT(cond, code) \
	{ \
	if ( !(cond) ) \
		{ \
		RCIS_VERBOSE_PANIC(code); \
		} \
	}

void CloseDeleteAndNull(TAny* aPtr)
	{
	RRemCon** sessionPtrPtr = static_cast<RRemCon**>(aPtr);
	RRemCon* session = *sessionPtrPtr;
	session->Close();
	delete session;
	*sessionPtrPtr = NULL;
	}

void CleanupCloseDeleteAndNullPushL(RRemCon** aSession)
	{
	TCleanupItem item(CloseDeleteAndNull, aSession);
	CleanupStack::PushL(item);	
	}

EXPORT_C CRemConInterfaceSelector* CRemConInterfaceSelector::NewL()
	{
	CONNECT_LOGGER
	LOG_STATIC_FUNC

	CRemConInterfaceSelector* ifSel = new(ELeave) CRemConInterfaceSelector;
	CleanupStack::PushL(ifSel);
	ifSel->ConstructL();
	CleanupStack::Pop(ifSel);
	return ifSel;
	}

void CRemConInterfaceSelector::ConstructL()
	{
	iSharedThreadHeap = UserHeap::ChunkHeap(NULL, 0, KMaxSharedThreadHeapSize);
	if(!iSharedThreadHeap)
		{
		LEAVEL(KErrNoMemory);
		}
	iBulkCleanupCall = new(ELeave) RSpecificThreadCallBack;
	TCallBack bulkCleanupCb(StaticBulkCleanup, this);
	LEAVEIFERRORL(iBulkCleanupCall->Create(bulkCleanupCb, CActive::EPriorityStandard));
	
	// allocate in the shared objects heap.
	RHeap* currentHeap = User::SwitchHeap(iSharedThreadHeap);
	CleanupSwitchHeapPushL(*currentHeap);
		{
		iInterfaces = CRemConInterfaceDetailsArray::NewL();
		}
	CleanupStack::PopAndDestroy(currentHeap);
	
	RNestableLock* lock = new (ELeave) RNestableLock;
    CleanupStack::PushL(lock);
	LEAVEIFERRORL(lock->CreateLocal());
	CleanupStack::Pop(lock);
	iLock = lock;
	}

CRemConInterfaceSelector::CRemConInterfaceSelector()
	{
	LOG_FUNC
	iTargetReceiver = NULL;
	}

EXPORT_C CRemConInterfaceSelector::~CRemConInterfaceSelector()
	{
	LOG_FUNC
	
	// The easy one... (i.e. non-bulk interfaces)
	for(TInt ix = 0; ix < iInterfaces->Array().Count(); ++ix)
		{
		CRemConInterfaceDetails* const details = iInterfaces->Array()[ix];
		ASSERT_DEBUG(details);
		if(!details->IsBulk())
			{
			CRemConInterfaceBase* interface = details->Interface();
			details->Interface() = NULL;
			delete interface;
			}
		}
	
	// The tricky one...
	// we have to use the thread specific cleanup because they have to 
	// cancel some objects...
	TInt err = iBulkCleanupCall->CallBack();
	if(err == KErrDied)
		{
		// If the other thread is dead then we cannot cleanly cleanup
		// but if the heap is shared then we should be ok.
		if(iBulkHeap == &User::Heap())
			{
			BulkCleanup();
			}
		}
	
	// Finally tidy-up shared thread objects.
	RHeap* currentHeap = User::SwitchHeap(iSharedThreadHeap);
		{
		delete iInterfaces;
		}
	User::SwitchHeap(currentHeap);

	
	delete iTargetReceiver;
	delete iControllerReceiver;
	// delete iBulkReceiver; // This is done in the bulk thread cleanup.

	if(iControllerSession)
		{
		iControllerSession->Close();
		delete iControllerSession;
		}

	if(iTargetSession)
		{
		iTargetSession->Close();
		delete iTargetSession;
		}

	// iBulkSession.Close(); // This is done in the bulk thread cleanup.

	iBulkCleanupCall->Close();
	delete iBulkCleanupCall;
	
	iSharedThreadHeap->Close();
	
	iBulkThread.Close();

	iLock->Wait();
	iLock->Close();
	delete iLock;
	
	CLOSE_LOGGER
	}

TInt CRemConInterfaceSelector::StaticBulkCleanup(TAny* aSelf)
	{
	LOG_STATIC_FUNC
	reinterpret_cast<CRemConInterfaceSelector*>(aSelf)->BulkCleanup();
	return KErrNone;
	}

void CRemConInterfaceSelector::BulkCleanup()
	{
	LOG_FUNC
	CBulkReceiver* recv = iBulkReceiver;
	iBulkReceiver = NULL;
	delete recv;
	if(RThread().Id() == iBulkThread.Id() && iBulkSession)
		{
		iBulkSession->Close();
		delete iBulkSession;
		iBulkSession = NULL;
		}
	for(TInt ix = 0; ix < iInterfaces->Array().Count(); ++ix)
		{
		CRemConInterfaceDetails* const details = iInterfaces->Array()[ix];
		ASSERT_DEBUG(details);
		if(details->IsBulk())
			{
			CRemConInterfaceBase* interface = details->Interface();
			details->Interface() = NULL;
			delete interface;
			}
		}
	}

void CRemConInterfaceSelector::EstablishBulkThreadBindingL()
	{
	LOG_FUNC
	if(iBulkHeap)
		{
		// Already bound
		RCIS_VERBOSE_ASSERT(RThread().Id() == iBulkThread.Id(), ERemConIfSelMultipleBulkInterfaceThreads);
		}
	else
		{
		// Create Binding.
		LEAVEIFERRORL(iBulkThread.Duplicate(RThread()));
		CleanupClosePushL(iBulkThread);
		LEAVEIFERRORL(iBulkCleanupCall->Start());
		iBulkReceiver = CBulkReceiver::NewL(*this);	
		iBulkHeap = &User::Heap();
		CleanupStack::Pop(&iBulkThread);
		}
	
	}

void CRemConInterfaceSelector::RegisterInterfaceCommonL(CRemConInterfaceBase& aInterface, const TDesC8& aFeatures)
	{
	LOG_FUNC
	
	const TBool bulkIf = aInterface.Bulk();
	if(bulkIf)
		{
		EstablishBulkThreadBindingL();
		}

	// Check an instance of the same interface (same interface UID and type) 
	// has not already been added.
	const TUint count = iInterfaces->Array().Count();
	for(TUint ii=0; ii<count; ++ii)
		{
		CRemConInterfaceBase* const iface = iInterfaces->Array()[ii]->Interface();
		RCIS_VERBOSE_ASSERT(iface, ERemConIfSelInternalError);
		RCIS_VERBOSE_ASSERT( (iface->InterfaceUid() != aInterface.InterfaceUid()) || (iface->Type() != aInterface.Type()), 
			ERemConIfSelInterfaceOfThatTypeAlreadyRegistered);
		}

	// Registration of interfaces should occur before we try to use any of 
	// them.
	RCIS_VERBOSE_ASSERT(!TargetOpened() && !ControllerOpened(), ERemConIfSelTardyInterfaceRegistration);
	
	iLock->Wait();	// critical session
	RHeap* currentHeap = User::SwitchHeap(iSharedThreadHeap);
	CleanupSwitchHeapPushL(*currentHeap);
		{
		CRemConInterfaceDetails* details = CRemConInterfaceDetails::NewLC(aInterface.InterfaceUid(), aInterface.Type(), bulkIf, &aInterface, aFeatures);
		iInterfaces->AppendL(details);
		CleanupStack::Pop(details);
		}
	CleanupStack::PopAndDestroy(currentHeap);
	iLock->Signal(); // end of critical session
	
	// The interface has been appended OK, so adjust iMaxDataLength.
	const TUint interfaceMaxLength = aInterface.MaxLength();
	if(!bulkIf && interfaceMaxLength > iControlMaxDataLength)
		{
		iControlMaxDataLength = interfaceMaxLength;
		}
	else if(bulkIf && interfaceMaxLength > iBulkMaxDataLength)
		{
		iBulkMaxDataLength = interfaceMaxLength;
		}
	}

EXPORT_C void CRemConInterfaceSelector::RegisterInterfaceL(CRemConInterfaceBase& aInterface)
	{
	LOG_FUNC
	
	RegisterInterfaceCommonL(aInterface, KNullDesC8);
	}

void CRemConInterfaceSelector::RegisterInterfaceL(CRemConInterfaceBase& aInterface, RRemConInterfaceFeatures& aRemConInterfaceFeatures)
	{
	LOG_FUNC

	RegisterInterfaceCommonL(aInterface, aRemConInterfaceFeatures.SupportedOperations());
	}

EXPORT_C void CRemConInterfaceSelector::RegisterErrorObserver(MRemConErrorObserver* aObserver)
	{
	LOG_FUNC
	LOG1(_L("\taObserver = 0x%08x"), aObserver);
	
	iErrorObserver = aObserver;
	}

EXPORT_C void CRemConInterfaceSelector::OpenControllerL()
	{
	LOG_FUNC

	// An attempt to open a controller session when one is already open 
	// is an exceptional condition.
	if (ControllerOpened())
		{
		LEAVEL(KErrInUse);
		}

	// NB We don't enforce that there are some interfaces registered here 
	// because the client might be connecting just to watch connection 
	// statuses.

	iControllerSession = new(ELeave)RRemConController();
	LEAVEIFERRORL(iControllerSession->Connect());
	CleanupCloseDeleteAndNullPushL(reinterpret_cast<RRemCon**>(&iControllerSession));
	
	RegisterInterestedApisL(ERemConClientTypeController);

	// Now there's a session to receive on, start the receiver.
	RCIS_VERBOSE_ASSERT(!iControllerReceiver, ERemConIfSelInternalError);
	iControllerReceiver = CReceiver::NewL(*iControllerSession, *this, iControlMaxDataLength, ERemConClientTypeController);
	CleanupStack::Pop(&iControllerSession);
	}

void CRemConInterfaceSelector::RegisterInterestedApisL(TRemConClientType aType)
	{
	LOG_FUNC

	TBool target = CRemConInterfaceBase::Target(aType);
	
	// First get the size of the data we are going to pass to the server.
	RCountSizeWriteStream counter;
	iInterfaces->ExternalizeL(counter, aType);
	TInt size = counter.Size();
	counter.Close();
	
	// Create a suitable size buffer.
	RBuf8 ipcBuf;
	ipcBuf.CreateL(size);
	ipcBuf.CleanupClosePushL();
	
	// Encode the interface details into the buffer.
	RDesWriteStream ipcStream(ipcBuf);
	iInterfaces->ExternalizeL(ipcStream, aType);
	ipcStream.CommitL();
	ipcStream.Close();
	
	// Inform the correct session about the interfaces
	if(!target)
		{
		LEAVEIFERRORL(iControllerSession->RegisterInterestedAPIs(ipcBuf));
		}
	else
		{	
		LEAVEIFERRORL(iTargetSession->RegisterInterestedAPIs(ipcBuf));
		}
	
	CleanupStack::PopAndDestroy(&ipcBuf);
	}

void CRemConInterfaceSelector::OpenTargetCommonL()
	{
	LOG_FUNC

	// NB We don't enforce that there are some interfaces registered here 
	// because the client might be connecting just to watch connection 
	// statuses.
	CleanupCloseDeleteAndNullPushL(reinterpret_cast<RRemCon**>(&iTargetSession));

	RegisterInterestedApisL(ERemConClientTypeTarget);
	
	// Now we are finished with the features, so we can release some memory.
	RHeap* currentHeap = User::SwitchHeap(iSharedThreadHeap);
		{
		for(TInt ix = 0; ix < iInterfaces->Array().Count(); ++ix)
			{
			CRemConInterfaceDetails* const details = iInterfaces->Array()[ix];
			ASSERT_DEBUG(details);
			details->DeleteFeatures();
			}
		}
	User::SwitchHeap(currentHeap);
	
	// Now there's a session to receive on, start the receiver.
	RCIS_VERBOSE_ASSERT(!iTargetReceiver, ERemConIfSelInternalError);
	iTargetReceiver = CReceiver::NewL(*iTargetSession, *this, iControlMaxDataLength, ERemConClientTypeTarget);
	CleanupDeleteAndNullPushL(iTargetReceiver);

	if(iBulkReceiver)
		{
		// We delegate the call to the thread the receiver is running
		// in - waiting until it has completed (with success or error).
		iBulkReceiver->WaitUntilConnectedL();
		}

	CleanupStack::Pop(2, &iTargetSession); // iTargetReceiver, iTargetSession
	}

EXPORT_C void CRemConInterfaceSelector::OpenTargetL()
	{
	LOG_FUNC

	// An attempt to open a target session when one is already open 
	// is an exceptional condition.
	if (TargetOpened())
		{
		LEAVEL(KErrInUse);
		}
	
	iTargetSession = new(ELeave)RRemConTarget();
	TInt err = iTargetSession->Connect();
	if(err == KErrNone)
		{
		OpenTargetCommonL();
		}
	else
		{
		delete iTargetSession;
		iTargetSession = NULL;
		LEAVEL(err);
		}
	}

/**
This should be run from the thread in which the bulk interfaces are to run.
*/
void CRemConInterfaceSelector::BulkSessionConnectL()
	{
	LOG_FUNC

	RRemConBulk* bulkSession = new(ELeave)RRemConBulk;
	CleanupStack::PushL(bulkSession);
	LEAVEIFERRORL(bulkSession->Connect());
	CleanupClosePushL(*bulkSession);
	RCIS_VERBOSE_ASSERT(iBulkReceiver, ERemConIfSelInternalError);
	iBulkReceiver->InitialiseL(*bulkSession, iBulkMaxDataLength);
	CleanupStack::Pop(2, bulkSession);
	iBulkSession = bulkSession;
	}

/**
Opens a target session to RemCon.

If any bulk interfaces have been registered on this interface selector the
the thread in which the first bulk interface was created must be ready to run and
not blocked waiting for the completion of this function.  Failure to do so will lead
to deadlock.

@param aPlayerType The type of player
@param aPlayerSubType The sub-type of the player
@param aPlayerName  The name of the player
@leave KErrInUse If a target session is already open.
*/
EXPORT_C void CRemConInterfaceSelector::OpenTargetL(TPlayerType aPlayerType, TPlayerSubType aPlayerSubType, const TDesC8& aPlayerName)
	{
	LOG_FUNC

	// An attempt to open a target session when one is already open 
	// is an exceptional condition.
	if (TargetOpened())
		{
		LEAVEL(KErrInUse);
		}

	iTargetSession = new(ELeave)RRemConTarget();
	TInt err = iTargetSession->Connect(aPlayerType,aPlayerSubType,aPlayerName);
	if(err == KErrNone)
		{
		OpenTargetCommonL();
		}
	else
		{
		delete iTargetSession;
		iTargetSession = NULL;
		LEAVEL(err);
		}
	}

EXPORT_C TBool CRemConInterfaceSelector::ControllerOpened() const
	{
	ASSERT_DEBUG((iControllerSession && iControllerSession->Handle()) || !iControllerSession);
	return iControllerSession ? ETrue : EFalse;
	}

EXPORT_C TBool CRemConInterfaceSelector::TargetOpened() const
	{
	ASSERT_DEBUG((iTargetSession && iTargetSession->Handle()) || !iTargetSession);
	return iTargetSession ? ETrue : EFalse;
	}

TBool CRemConInterfaceSelector::BulkOpened() const
	{
	ASSERT_DEBUG((iBulkSession && iBulkSession->Handle()) || !iBulkSession);
	return iBulkSession ? ETrue : EFalse;
	}

EXPORT_C void CRemConInterfaceSelector::GoConnectionOrientedL(const TRemConAddress& aConnection)
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	LEAVEIFERRORL(iControllerSession->GoConnectionOriented(aConnection));
	
	// Store the address.  This means that if the server dies we know all
	// we need to to return the existing sessions to usability without 
	// troubling the app.
	iAddress = aConnection;
	}

EXPORT_C void CRemConInterfaceSelector::GoConnectionlessL()
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	LEAVEIFERRORL(iControllerSession->GoConnectionless());
	
	// Unset any stored address, so we know we are connectionless.
	iAddress.BearerUid() = KNullUid;
	}

EXPORT_C void CRemConInterfaceSelector::ConnectBearer(TRequestStatus& aStat)
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	iControllerSession->ConnectBearer(aStat);
	}

EXPORT_C TInt CRemConInterfaceSelector::ConnectBearerCancel()
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	//Ignore Return code because
	// a) It'll mostly be other than KErrNone because the server has terminated, in which
	//    case the original async request will have completed with the error anyway!
	// b) It's meaningless to the client whatever the return code is.
	(void)iControllerSession->ConnectBearerCancel();
	
	return KErrNone;
	}

EXPORT_C void CRemConInterfaceSelector::DisconnectBearer(TRequestStatus& aStat)
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	iControllerSession->DisconnectBearer(aStat);
	}

EXPORT_C TInt CRemConInterfaceSelector::DisconnectBearerCancel()
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);

	//See CRemConInterfaceSelector::ConnectBearerCancel() for comment
	(void)iControllerSession->DisconnectBearerCancel();

	return KErrNone;
	}

EXPORT_C void CRemConInterfaceSelector::Send(TRequestStatus& aStatus, 
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 TUint& aNumRemotes,
											 TRemConMessageType aMsgType,
											 const TDesC8& aData)
	{
	LOG_FUNC

	Send(aStatus, aInterfaceUid, aOperationId, aNumRemotes, aMsgType, ERemConMessageDefault, aData);
	}

EXPORT_C void CRemConInterfaceSelector::Send(TRequestStatus& aStatus, 
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 TUint& aNumRemotes,
											 TRemConMessageType aMsgType,
											 TRemConMessageSubType aSubType,
											 const TDesC8& aData)
	{
	LOG_FUNC
	
	switch ( aMsgType )
		{
	case ERemConCommand:
	case ERemConNotifyCommand:
		RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);
		iControllerSession->Send(aStatus, aInterfaceUid, aOperationId, aNumRemotes, aSubType, aData);
		break;
		
	case ERemConResponse:
		RCIS_VERBOSE_ASSERT(TargetOpened(), ERemConIfSelNoTargetSession);
		iTargetSession->Send(aStatus, aInterfaceUid, aOperationId, aNumRemotes, aSubType, aData);
		break;
		
	default:
		RCIS_VERBOSE_PANIC(ERemConIfSelBadMessageType);
		break;
		}
	}

/**
Sends a notify command to the remote device.

@see CRemConInterfaceSelector::Send()
*/
EXPORT_C void CRemConInterfaceSelector::SendNotify(TRequestStatus& aStatus, 
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 TRemConMessageType aMsgType,
											 TRemConMessageSubType aSubType,
											 const TDesC8& aData)
	{
	LOG_FUNC
	if (aMsgType == ERemConNotifyCommand)
		{
		RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);
		iControllerSession->SendNotify(aStatus, aInterfaceUid, aOperationId, aSubType, aData);
		}
	else
		{
		RCIS_VERBOSE_PANIC(ERemConIfSelBadMessageType);
		}
	}

EXPORT_C TInt CRemConInterfaceSelector::SendUnreliable(
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 TRemConMessageType aMsgType,
											 const TDesC8& aData)
	{
	LOG_FUNC
	
	return SendUnreliable(aInterfaceUid, aOperationId, aMsgType, ERemConMessageDefault, aData);
	}

EXPORT_C TInt CRemConInterfaceSelector::SendUnreliable(
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 TRemConMessageType aMsgType,
											 TRemConMessageSubType aSubType,
											 const TDesC8& aData)
	{
	LOG_FUNC
	
	TInt ret = KErrNone;
	
	switch ( aMsgType )
		{
		case ERemConCommand:
		case ERemConNotifyCommand:
		RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);
		ret = iControllerSession->SendUnreliable(aInterfaceUid, aOperationId, aSubType, aData);
		break;
		
		case ERemConResponse:
		RCIS_VERBOSE_ASSERT(TargetOpened(), ERemConIfSelNoTargetSession);
		ret = iTargetSession->SendUnreliable(aInterfaceUid, aOperationId, aSubType, aData);
		break;
		
		default:
		RCIS_VERBOSE_PANIC(ERemConIfSelBadMessageType);
		break;
		}
	return ret;
	}

EXPORT_C TInt CRemConInterfaceSelector::SendCancel(TRemConMessageType aMsgType)
	{
	LOG_FUNC

	switch ( aMsgType )
		{
	case ERemConCommand:
	case ERemConNotifyCommand:
		RCIS_VERBOSE_ASSERT(ControllerOpened(), ERemConIfSelNoControllerSession);
		//See CRemConInterfaceSelector::ConnectBearerCancel() for comment
		(void)iControllerSession->SendCancel();
		break;

	case ERemConResponse:
		RCIS_VERBOSE_ASSERT(TargetOpened(), ERemConIfSelNoTargetSession);
		//See CRemConInterfaceSelector::ConnectBearerCancel() for comment
		(void)iTargetSession->SendCancel();
		break;

	default:
		RCIS_VERBOSE_PANIC(ERemConIfSelBadMessageType);
		break;
		}

	return KErrNone;
	}

EXPORT_C void CRemConInterfaceSelector::SendBulk(TRequestStatus& aStatus, 
											 TUid aInterfaceUid, 
											 TUint aOperationId, 
											 const TDesC8& aData)
	{
	LOG_FUNC
	
	// Panic as Target Session, because bulkness is transparent to client
	RCIS_VERBOSE_ASSERT(BulkOpened(), ERemConIfSelNoTargetSession);
	iBulkSession->Send(aStatus, aInterfaceUid, aOperationId, aData);
	}

EXPORT_C TInt CRemConInterfaceSelector::SendBulkUnreliable(
											TUid aInterfaceUid,
											TUint aOperationId,
											const TDesC8& aData)
	{
	LOG_FUNC
	
	// Panic as Target Session, because bulkness is transparent to client
	RCIS_VERBOSE_ASSERT(BulkOpened(), ERemConIfSelNoTargetSession);
	return iBulkSession->SendUnreliable(aInterfaceUid, aOperationId, aData);
	}

EXPORT_C TInt CRemConInterfaceSelector::SendBulkCancel()
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(BulkOpened(), ERemConIfSelNoTargetSession);
	//See CRemConInterfaceSelector::ConnectBearerCancel() for comment
	(void)iBulkSession->SendCancel();

	return KErrNone;
	}

void CRemConInterfaceSelector::ReceiveComplete(TUid aInterfaceUid, 
		TUint aOperationId, 
		TRemConMessageSubType aMsgSubType, 
		const TRemConAddress& aRemoteAddress,
		const TDesC8& aData, 
		TRemConClientType aType)
	{
	LOG_FUNC
	LOG1(_L("\taInterfaceUid = 0x%08x"), aInterfaceUid);
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taRemoteAddress.BearerUid = 0x%08x"), aRemoteAddress.BearerUid());
	
	const TUint count = iInterfaces->Array().Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConInterfaceDetails* const details = iInterfaces->Array()[ii];
		ASSERT_DEBUG(details);
		CRemConInterfaceBase* const iface = details->Interface();
		RCIS_VERBOSE_ASSERT(iface, ERemConIfSelInternalError);

		if (	iface->InterfaceUid() == aInterfaceUid 
			&&	iface->Type() == aType )
			{
			ASSERT_DEBUG(!details->IsBulk());
			MRemConInterfaceIf3* interfaceIf3 = reinterpret_cast<MRemConInterfaceIf3*>(iface->GetInterfaceIf(TUid::Uid(KRemConInterfaceIf3)));
			if (interfaceIf3)
				{
				interfaceIf3->MrcibNewMessage(aOperationId, aData, aMsgSubType, aRemoteAddress);
				break;
				}
			MRemConInterfaceIf2* interfaceIf2 = reinterpret_cast<MRemConInterfaceIf2*>(iface->GetInterfaceIf(TUid::Uid(KRemConInterfaceIf2)));
			if (interfaceIf2)
				{
				interfaceIf2->MrcibNewMessage(aOperationId, aData, aMsgSubType);
				break;
				}
			MRemConInterfaceIf* interfaceIf = reinterpret_cast<MRemConInterfaceIf*>(iface->GetInterfaceIf(TUid::Uid(KRemConInterfaceIf1)));
			RCIS_VERBOSE_ASSERT(interfaceIf, ERemConIfSelNoInterfaceImplementation);
			interfaceIf->MrcibNewMessage(aOperationId, aData);
			break;
			}
		}
	}

void CRemConInterfaceSelector::BulkReceiveComplete(TUid aInterfaceUid, TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taInterfaceUid = 0x%08x"), aInterfaceUid);
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);

	const TUint count = iInterfaces->Array().Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CRemConInterfaceDetails* const details = iInterfaces->Array()[ii];
		ASSERT_DEBUG(details);
		CRemConInterfaceBase* const iface = details->Interface();
		RCIS_VERBOSE_ASSERT(iface, ERemConIfSelInternalError);

		if(details->IsBulk() && iface->InterfaceUid() == aInterfaceUid)
			{
			MRemConInterfaceIf* interfaceIf = reinterpret_cast<MRemConInterfaceIf*>(iface->GetInterfaceIf(TUid::Uid(KRemConInterfaceIf1)));
			RCIS_VERBOSE_ASSERT(interfaceIf, ERemConIfSelNoInterfaceImplementation);
			interfaceIf->MrcibNewMessage(aOperationId, aData);
			}
		}
	}

void CRemConInterfaceSelector::Error(TInt aError)
	{
	LOG_FUNC
	LOG1(_L("\taError = %d"), aError);
	LOG1(_L("\tiErrorObserver = 0x%08x"), iErrorObserver);
	
	if(aError == KErrServerTerminated)
		{
		// Initially try and deal with server death in a way that is
		// transparent to the app.
		TInt err = TryToReconnect();
		LOG1(_L("\tTryToReconnect error = %d"), err);
	
		// If we fail inform any registered app.  Unregistered apps
		// just take the risk that they may not realise the server has
		// died if it errors on a receive.
		if(err && iErrorObserver)
			{
			iErrorObserver->MrceoError(KErrServerTerminated);
			}
		}
	}

void CRemConInterfaceSelector::BulkError(TInt aError)
	{
	LOG_FUNC
	LOG1(_L("\taError = %d"), aError);
	LOG1(_L("\tiErrorObserver = 0x%08x"), iErrorObserver);
	
	if(aError == KErrServerTerminated)
		{
		// Initially try and deal with server death in a way that is
		// transparent to the app.
		TInt err = TryToReconnectBulk();
		LOG1(_L("\tTryToReconnectBulk error = %d"), err);
	
		// If we fail inform any registered app.  Unregistered apps
		// just take the risk that they may not realise the server has
		// died if it errors on a receive.
		if(err && iErrorObserver)
			{
			iErrorObserver->MrceoError(KErrServerTerminated);
			}
		}
	}

EXPORT_C TInt CRemConInterfaceSelector::GetConnections(TSglQue<TRemConAddress>& aConnections)
	{
	LOG_FUNC

	// It doesn't matter which session we use for this.
	RRemCon* sess = ControllerOpened() ? reinterpret_cast<RRemCon*>(iControllerSession) : reinterpret_cast<RRemCon*>(iTargetSession);
	AssertSession(sess, ERemConIfSelNoSession);

	return sess->GetConnections(aConnections);
	}

EXPORT_C void CRemConInterfaceSelector::NotifyConnectionsChange(TRequestStatus& aStatus)
	{
	LOG_FUNC

	// It doesn't matter which session we use for this.
	iNotificationSession = ControllerOpened() ? reinterpret_cast<RRemCon*>(iControllerSession) : reinterpret_cast<RRemCon*>(iTargetSession);
	AssertSession(iNotificationSession, ERemConIfSelNoSession);

	iNotificationSession->NotifyConnectionsChange(aStatus);
	}

EXPORT_C TInt CRemConInterfaceSelector::NotifyConnectionsChangeCancel()
	{
	LOG_FUNC

	// Get the session we used for posting the original notification. It won't 
	// have gone away in the meantime as that only happens when 'this' is 
	// destroyed, but the client may call this without ever having called the 
	// original notification.
	AssertSession(iNotificationSession, ERemConIfSelNoSession);

	//See CRemConInterfaceSelector::ConnectBearerCancel() for comment
	(void)iNotificationSession->NotifyConnectionsChangeCancel();

	return KErrNone;
	}

void CRemConInterfaceSelector::AssertSession(RRemCon* aSess, TInt aPanicCode) const
	{
	LOG_FUNC

	RCIS_VERBOSE_ASSERT(aSess && aSess->Handle(), aPanicCode);
	}

/** 
Resurrects sessions in case of server death.

For the target and controller session we try to open a new
session.  If this succeeds we can then set the old session
to point to the new one after closing our old defunct 
session.

On success we are left with happy functioning handles.
On failure we are left with defunct handles, which are still 
valid but will complete all requests with KErrServerTerminated.

This means that we won't cause an application to get a bad handle
panic if it hasn't done anything wrong.
*/
TInt CRemConInterfaceSelector::TryToReconnect()
	{
	LOG_FUNC
	
	// Always want to stop receiving.  The receiver itself should stop issuing
	// further receives on completion, but we only want to have the error function
	// called once, so Cancel() receivers in case we had both.
	if(TargetOpened())
		{
		RCIS_VERBOSE_ASSERT(iTargetReceiver, ERemConIfSelInternalError);
		iTargetReceiver->Cancel();
		}

	if(ControllerOpened())
		{
		RCIS_VERBOSE_ASSERT(iControllerReceiver, ERemConIfSelInternalError);
		iControllerReceiver->Cancel();
		}
		
	// Now try and create new sessions.  We do all the failable work first
	// so we aren't left in a half alive situation.		
	TInt err = KErrNone;
	RRemConTarget newTarget;
	RRemConController newController;
	
	if(TargetOpened())
		{
		// See if we can kick RemCon back into life
		err = newTarget.Connect();
		}
	
	if(!err && ControllerOpened())
		{		
		// See if we can kick RemCon back into life
		err = newController.Connect();
		
		if(!err && !iAddress.IsNull())
			{	
			// If an address is set the session was connection oriented, 
			// resurrect that now.	
			err = newController.GoConnectionOriented(iAddress);
			}
		}
	
	if(!err)
		{
		// RemCon lives!  Set our sessions to be the nice new ones.
		if(TargetOpened())
			{
			iTargetSession->Close();
			iTargetSession->SetHandle(newTarget.Handle());
			iTargetReceiver->Receive();
			}

		if(ControllerOpened())
			{
			iControllerSession->Close();
			iControllerSession->SetHandle(newController.Handle());
			iControllerReceiver->Receive();
			}
		}
	else
		{
		// We may not have successfully opened these, but it's safe to 
		// close them anyway.
		newTarget.Close();
		newController.Close();
		}
	
	return err;
	}



/** 
Resurrects bulk sessions in case of server death.

For the bulk session we try to open a new
session.  If this succeeds we can then set the old session
to point to the new one after closing our old defunct 
session.

On success we are left with happy functioning handles.
On failure we are left with defunct handles, which are still 
valid but will complete all requests with KErrServerTerminated.

This means that we won't cause an application to get a bad handle
panic if it hasn't done anything wrong.
*/
TInt CRemConInterfaceSelector::TryToReconnectBulk()
	{
	LOG_FUNC
	
	// Always want to stop receiving.  The receiver itself should stop issuing
	// further receives on completion, but we only want to have the error function
	// called once, so Cancel() receivers in case we had both.
	if(BulkOpened())
		{
		RCIS_VERBOSE_ASSERT(iBulkReceiver, ERemConIfSelInternalError);
		iBulkReceiver->Cancel();
		}
		
	// Now try and create new sessions.  We do all the failable work first
	// so we aren't left in a half alive situation.		
	TInt err = KErrNone;
	RRemConBulk newBulk;
	
	if(BulkOpened())
		{
		// See if we can kick RemCon back into life
		err = newBulk.Connect();
		}
	
	if(err == KErrNone)
		{
		// RemCon lives!  Set our session to be the nice new one.
		if(BulkOpened())
			{
			iBulkSession->Close();
			iBulkSession->SetHandle(newBulk.Handle());
			iBulkReceiver->Receive();
			}
		}
	else
		{
		// We may not have successfully opened this, but it's safe to 
		// close it anyway.
		newBulk.Close();
		}
	
	return err;
	}

