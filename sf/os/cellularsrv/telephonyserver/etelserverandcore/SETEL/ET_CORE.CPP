// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ET_CORETraces.h"
#endif

#include "ET_SSTD.H"
#include "ETBUFFER.H"
#include <f32file.h>
#include "etsvr_slots.h"
#include "et_phone_util.h"

//
//
// CFaxBase
//
//
EXPORT_C CFaxBase::CFaxBase()
//
//	C'Tor
//
	{
	__DECLARE_NAME(_S("CFaxBase"));
	}

EXPORT_C CFaxBase::~CFaxBase()
//
//	D'Tor
//
	{}

EXPORT_C CTelObject* CFaxBase::OpenNewObjectByNameL(const TDesC&)
//
// Always return NULL not allow CFaxBase Tsy object to create new object
//
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C CTelObject* CFaxBase::OpenNewObjectL(TDes&)
//
// Always return NULL not allow CFaxBase Tsy object to create new object
//
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C void CFaxBase::OpenPostProcessing(CTelSession*,const TInt)
//
// Perform post-processing after object has been added to session's CObjectIx.
//
	{}

EXPORT_C void CFaxBase::CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle)
	{
	CreateDummySession(aSession,aSubSessionHandle);
	FlushReqs(aSession,aSubSessionHandle);
	}

EXPORT_C TInt CFaxBase::CancelService(const TInt aIpc,const TTsyReqHandle /*aTsyReqHandle*/)
//
// Cancel Service for Fax
//
	{
	__ASSERT_ALWAYS((aIpc==EEtelFaxRead) ||
					(aIpc==EEtelFaxWrite) ||
					(aIpc==EEtelFaxWaitForEndOfPage)
					,Fault(EEtelFaultInvalidIpcForCancel));

	switch (aIpc)
		{
	// this is a special case for Fax session - 
	// count already increment before calling susession
	// for those reqs below - server do expect Tsy to call ReqCompleted()
	// so the count should be decrement again !
	case EEtelFaxRead:
	case EEtelFaxWrite:
	case EEtelFaxWaitForEndOfPage:
		return KErrNone;	
		// There is no individual cancel associated with these requests
	default:
		return KErrGeneral;
		}
	}


EXPORT_C TInt CFaxBase::Service(const RMessage2& /*aMessage*/,CReqEntry* aReqEntry)
//
// The ServiceL functionality for the fax
//
	{
	__ASSERT_ALWAYS(aReqEntry!=NULL,Fault(EEtelFaultCallTsyServiceWithoutReqPackage));
	aReqEntry->iPlacedRequest=ETrue;
	TTsyReqHandle tsyReqHandle=aReqEntry->iTsyReqHandle;
	TDes8* des1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);

	switch (aReqEntry->iFunction)
		{
	case EEtelFaxRead:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return Read(tsyReqHandle,des1);
	case EEtelFaxWrite:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return Write(tsyReqHandle,des1);
	case EEtelFaxWaitForEndOfPage:
		return WaitForEndOfPage(tsyReqHandle);
	case EEtelFaxTerminateFaxSession:
		return TerminateFaxSession(tsyReqHandle);
	default:
		return KErrNotSupported;
		}
	}


EXPORT_C void CFaxBase::Init()
	{}

EXPORT_C CTelObject::TReqMode CFaxBase::ReqModeL(const TInt aIpc)
//
// Fax Request Inquiry Functions
//
    {
	CTelObject::TReqMode mode=0;
	switch (aIpc)
		{
	case EEtelFaxTerminateFaxSession:
		mode=KReqModeMultipleCompletionEnabled;	// if two clients call this, only want the first to
											// be acted upon
		break;
	case EEtelFaxRead:
	case EEtelFaxWrite:
	case EEtelFaxWaitForEndOfPage:
		mode=KReqModeFlowControlObeyed;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return mode;
	}

EXPORT_C TInt CFaxBase::NumberOfSlotsL(const TInt /*aIpc*/)
	{
	User::Leave(KErrNotSupported);	// there are no repost immediately functions for fax
	return KSlotNumbersDefault;
	}

//
//
// CCallBase
//
//
EXPORT_C CCallBase::CCallBase()
//
//	C'Tor
//
	:iOwnershipStatus(EOwnedUnowned)
	,iOwnerSession(NULL)
	,iOwnerSubSessionHandle(0)
	,iLoanDataPort(EFalse)
	{
	__DECLARE_NAME(_S("CCallBase"));
	}

EXPORT_C CCallBase::~CCallBase()
//
//	D'Tor
//	Attual closing of the owner is done in CSubSessionBase destructor
//	This is for all object inherit fron CSubSessionBase
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CCALLBASE_DTOR_1, "~CCallBase");
	}

EXPORT_C TInt CCallBase::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
//
// Cancel Service for Call
//
	{
	__ASSERT_ALWAYS((aIpc==EEtelCallNotifyHookChange) ||
					(aIpc==EEtelCallNotifyStatusChange) ||
					(aIpc==EEtelCallNotifyDurationChange) ||
					(aIpc==EEtelCallCapsChangeNotification) ||
					(aIpc==EEtelCallAcquireOwnership) ||
					(aIpc==EEtelCallDial) ||
					(aIpc==EEtelCallAnswer) ||
					(aIpc==EEtelCallHangUp) ||
					(aIpc==EEtelCallConnect)||
					(aIpc==EEtelCallLoanDataPort)
					,Fault(EEtelFaultInvalidIpcForCancel));
	switch (aIpc)
		{
	case EEtelCallCapsChangeNotification:
		return NotifyCapsChangeCancel(aTsyReqHandle);
	case EEtelCallNotifyHookChange:
		return NotifyHookChangeCancel(aTsyReqHandle);
	case EEtelCallNotifyStatusChange:
		return NotifyStatusChangeCancel(aTsyReqHandle);
	case EEtelCallNotifyDurationChange:
		return NotifyDurationChangeCancel(aTsyReqHandle);
	case EEtelCallAcquireOwnership:
		return AcquireOwnershipCancel(aTsyReqHandle);
	case EEtelCallDial:
		return DialCancel(aTsyReqHandle);
	case EEtelCallConnect:
		return ConnectCancel(aTsyReqHandle);
	case EEtelCallAnswer:
		return AnswerIncomingCallCancel(aTsyReqHandle);
	case EEtelCallHangUp:
		return HangUpCancel(aTsyReqHandle);
	case EEtelCallLoanDataPort:
		return LoanDataPortCancel(aTsyReqHandle);
	default:
		return KErrGeneral; // should never reached here
		}
	}

EXPORT_C void CCallBase::Init()
	{}

EXPORT_C CCallBase::TCallOwnership CCallBase::CheckOwnership(const TTsyReqHandle aTsyReqHandle) const
//
// Tsy upcall function - return the call ownership status
//
	{
	CReqEntry* reqEntry=PhoneOwner()->FindByTsyHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(reqEntry!=NULL,Fault(EEtelFaultNotRecognisedTsyHandle));
	RCall::TOwnershipStatus owner=CheckOwnershipBySession(reqEntry->iSession,reqEntry->iMessage.Int3());

	if (owner==RCall::EOwnershipUnowned)
		return EOwnedUnowned;

	if (owner==RCall::EOwnershipOwnedByThisClient)
		return EOwnedTrue;

	if (owner==RCall::EOwnershipThisIsPriorityClient)
		return EOwnedPriorityClient; 

	return EOwnedFalse;
	}

EXPORT_C RCall::TOwnershipStatus CCallBase::CheckOwnershipBySession(CTelSession* aSession,const TInt /*aSubSessionHandle*/) const
//
// Return true if the owner
//
	{
	if (iOwnershipStatus==EOwnedUnowned)
		return RCall::EOwnershipUnowned;

	if(iOwnershipStatus==EOwnedTrue && iOwnerSession==aSession /*&& iOwnerSubSessionHandle==aSubSessionHandle*/)	// ownership is session based!
		return RCall::EOwnershipOwnedByThisClient;

// Ann changed
	if (iOwnershipStatus==EOwnedTrue && aSession->TelServer()->IsPriorityClient(aSession))
		return RCall::EOwnershipThisIsPriorityClient;
// end

	return RCall::EOwnershipOwnedByAnotherClient;
	}

EXPORT_C TInt CCallBase::SetUnowned()
//
// Return Owner ship status
//
	{
	iOwnershipStatus=EOwnedUnowned;
	iOwnerSession=NULL; // set to invalid value
	iOwnerSubSessionHandle=0; 

	return KErrNone;
	}

EXPORT_C TInt CCallBase::SetOwnership(const TTsyReqHandle aTsyReqHandle)
//
// If owner ship if not own
//
	{
	CReqEntry* reqEntry=PhoneOwner()->FindByTsyHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(reqEntry!=NULL,Fault(EEtelFaultNotRecognisedTsyHandle));

	iOwnershipStatus=EOwnedTrue;
	iOwnerSession=reqEntry->iSession;
	iOwnerSubSessionHandle=reqEntry->iMessage.Int3();
	return KErrNone;
	}

EXPORT_C TBool CCallBase::CheckPriorityClient(const TTsyReqHandle aTsyReqHandle) const
	{
	CReqEntry* reqEntry=PhoneOwner()->FindByTsyHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(reqEntry!=NULL,Fault(EEtelFaultNotRecognisedTsyHandle));

	return reqEntry->iSession->TelServer()->IsPriorityClient(reqEntry->iSession);
	}		

EXPORT_C CCallBase* CCallBase::ResolveSubSessionHandle(const TTsyReqHandle aTsyReqHandle,
													   const TInt aSubSessionHandle)
//
//	expects the subsession handle of a call, which must have been opened by the same client session
//	which opened this call and called the current request (identified by aTsyReqHandle)
//
	{
	CReqEntry* reqEntry=PhoneOwner()->FindByTsyHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(reqEntry!=NULL,Fault(EEtelFaultNotRecognisedTsyHandle));
	return REINTERPRET_CAST(CCallBase*,reqEntry->iSession->CObjectFromHandle(aSubSessionHandle));
	}

EXPORT_C void CCallBase::OpenPostProcessing(CTelSession* ,const TInt)
//
// Perform post-processing after object has been added to session's CObjectIx.
//
	{}

EXPORT_C void CCallBase::CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle)
//
// Perform close post-processing before object being close
// if is owner then set to EUnOwned
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CCALLBASE_CLOSESUBSESSIONPREPROCESSING_1, "CCallBase::CloseSubSessionPreProcessing() - about to create dummy session");
	CreateDummySession(aSession,aSubSessionHandle);
	RCall::TOwnershipStatus owner=CheckOwnershipBySession(aSession,aSubSessionHandle);
	if (owner==RCall::EOwnershipOwnedByThisClient)
		{
		CreateDummySession(aSession,aSubSessionHandle,ETrue);
		if (iLoanDataPort)  // data port being on loan by this owner
			RecoverDataPortAndRelinquishOwnership(); // inform tsy about this
		else
			RelinquishOwnership();
		}
	FlushReqs(aSession,aSubSessionHandle);	
	}

EXPORT_C void CCallBase::RelinquishOwnershipCompleted(const TInt aError)
//
// Server called Tsy with RelinquishOwnership()
// TSY should do a Up call with this function
//
	{
	__ASSERT_ALWAYS(aError==KErrNone,Fault(EEtelFaultCanNotRelinquishOwnership));
	CheckAndDestroyDummySubSession();
	}

EXPORT_C void CCallBase::RecoverDataPortAndRelinquishOwnershipCompleted(const TInt aError)
//
// Server called Tsy with RecoverDataPortAndRelinquishOwnership()
// TSY should do a Up call with this function
//
	{
	__ASSERT_ALWAYS(aError==KErrNone,Fault(EEtelFaultCanNotRelinquishOwnership));
	iLoanDataPort=EFalse;
	RelinquishOwnershipCompleted(aError);
	}

EXPORT_C TInt CCallBase::Service(const RMessage2& aMessage,CReqEntry* aReqEntry)
//
// The ServiceL functionality for the call
//
	{
	__ASSERT_ALWAYS(aReqEntry!=NULL,Fault(EEtelFaultCallTsyServiceWithoutReqPackage));
	aReqEntry->iPlacedRequest=ETrue;
	TTsyReqHandle tsyReqHandle=aReqEntry->iTsyReqHandle;
	TDes8* des1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);
	TUint8* ptr1=Ptr1(des1);
	switch (aReqEntry->iFunction)
		{

	// Asynchronous + Cancel
	case EEtelCallCapsChangeNotification:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyCapsChange(tsyReqHandle,REINTERPRET_CAST(RCall::TCaps*,ptr1));
	case EEtelCallCapsChangeNotificationCancel:
		return NotifyCapsChangeCancel(tsyReqHandle);
	case EEtelCallNotifyHookChange:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyHookChange(tsyReqHandle,REINTERPRET_CAST(RCall::THookStatus*,ptr1));
	case EEtelCallNotifyHookChangeCancel:
		return NotifyHookChangeCancel(tsyReqHandle);

	case EEtelCallNotifyStatusChange:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyStatusChange(tsyReqHandle,REINTERPRET_CAST(RCall::TStatus*,ptr1));
	case EEtelCallNotifyStatusChangeCancel:
		return  NotifyStatusChangeCancel(tsyReqHandle);
	case EEtelCallNotifyDurationChange:
		return NotifyDurationChange(tsyReqHandle,REINTERPRET_CAST(TTimeIntervalSeconds*,ptr1));
	case EEtelCallNotifyDurationChangeCancel:
		return NotifyDurationChangeCancel(tsyReqHandle);
	case EEtelCallDial:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		TDes* des2 = NULL;
		des2 = BufferDes2u(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		__ASSERT_ALWAYS(des2!=NULL,Fault(EEtelFaultDes2DoesNotExist));
		return Dial(tsyReqHandle,des1,des2);
		}
	case EEtelCallDialCancel:
		return DialCancel(tsyReqHandle);

	case EEtelCallConnect:
		return Connect(tsyReqHandle,des1);
	case EEtelCallConnectCancel:
		return ConnectCancel(tsyReqHandle);

	case EEtelCallAnswer:
		return AnswerIncomingCall(tsyReqHandle,des1);
	case EEtelCallAnswerCancel:
		return AnswerIncomingCallCancel(tsyReqHandle);

	case EEtelCallHangUp:
		return HangUp(tsyReqHandle);
	case EEtelCallHangUpCancel:
		return HangUpCancel(tsyReqHandle);

	case EEtelCallAcquireOwnership:
		{
		if (RCall::EOwnershipOwnedByThisClient==
			CheckOwnershipBySession(aReqEntry->iSession,aMessage.Int3()))
			return KErrEtelAlreadyCallOwner;
		else
			return AcquireOwnership(tsyReqHandle);
		}
	case EEtelCallAcquireOwnershipCancel:
		return AcquireOwnershipCancel(tsyReqHandle);

	// Synchronous
	case EEtelCallGetInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetInfo(tsyReqHandle,REINTERPRET_CAST(RCall::TCallInfo*,ptr1));
	case EEtelCallGetStatus:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetStatus(tsyReqHandle,REINTERPRET_CAST(RCall::TStatus*,ptr1));
	case EEtelCallGetCaps:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCaps(tsyReqHandle,REINTERPRET_CAST(RCall::TCaps*,ptr1));
	case EEtelCallLoanDataPort:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		TInt ret=LoanDataPort(tsyReqHandle,REINTERPRET_CAST(RCall::TCommPort*,ptr1));
		if (ret==KErrNone)
			iLoanDataPort=ETrue;
		return ret;
		}
	case EEtelCallLoanDataPortCancel:
		return LoanDataPortCancel(tsyReqHandle);

	case EEtelCallRecoverDataPort:
		{
		TInt ret=RecoverDataPort(tsyReqHandle);
		if (ret==KErrNone)
			iLoanDataPort=EFalse;
		return ret;
		}

	case EEtelCallTransferOwnership:
		return TransferOwnership(tsyReqHandle);
	case EEtelCallGetBearerServiceInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetBearerServiceInfo(tsyReqHandle,REINTERPRET_CAST(RCall::TBearerService*,ptr1));
		
	case EEtelCallGetOwnershipStatus:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));

		RCall::TOwnershipStatus* owner=REINTERPRET_CAST(RCall::TOwnershipStatus*,ptr1);
		*owner=CheckOwnershipBySession(aReqEntry->iSession,aMessage.Int3());
		ReqCompleted(tsyReqHandle,KErrNone);
		return KErrNone;
		}

	case EEtelCallGetCallParams:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCallParams(tsyReqHandle,des1);
		}
	case EEtelCallGetCallDuration:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCallDuration(tsyReqHandle,REINTERPRET_CAST(TTimeIntervalSeconds*,ptr1));
		}
	case EEtelCallGetFaxSettings:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetFaxSettings(tsyReqHandle,REINTERPRET_CAST(RCall::TFaxSessionSettings*,ptr1));
		}

	case EEtelCallSetFaxSettings:
		{
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return SetFaxSettings(tsyReqHandle,REINTERPRET_CAST(RCall::TFaxSessionSettings*,ptr1));
		}
	case EEtelCallReferenceCount:
 		{
 		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
 
 		TInt* count=REINTERPRET_CAST(TInt*,ptr1);
 		*count=AccessCount();
 		ReqCompleted(tsyReqHandle,KErrNone);
 		return KErrNone;
 		}
 	case EEtelAdoptFaxSharedHeaderFile:
		{
		CFaxSharedFileHandles* fshare = NULL;
		TRAPD(err, fshare = CFaxSharedFileHandles::NewL(aMessage));
		if(err != KErrNone)
			{
			return err;	
			}
		//Pass the CFaxSharedFileHandles object to the TSY.  If call is successful then TSY assumes owenership of the object else we delete it.	
		TInt ret = SetFaxSharedHeaderFile(tsyReqHandle, fshare);
		if(ret!= KErrNone)
			{
			delete fshare;	
			}			
		return ret;	
		}
	default:
		return ServiceExtFunc(aMessage,aReqEntry);
		}
	}

EXPORT_C RHandleBase* CCallBase::GlobalKernelObjectHandle()
	{
	if (iChunk.Handle() == KNullHandle) return NULL;
	return &iChunk;	
	}

	
EXPORT_C RFax::TProgress* CCallBase::CreateFaxProgressChunk()
//
//	Even if already created, return pointer to it
//
	{
	if (iChunk.Handle() == KNullHandle)
 		{
			TInt r = iChunk.CreateGlobal(KNullDesC,sizeof (RFax::TProgress), sizeof (RFax::TProgress),EOwnerProcess); 
			if (r == KErrNone)
				{
				RFax::TProgress* progress = new(iChunk.Base()) RFax::TProgress;
				progress->iLastUpdateTime = 0;
				progress->iAnswerback.Zero ();
				progress->iPhase = ENotYetStarted;
				progress->iSpeed = 9600;
				progress->iResolution = EFaxNormal;
				progress->iCompression = EModifiedHuffman;
				progress->iECM = 0;
 				progress->iPage = 0;
				progress->iLines = 0;
				return progress;
				}	
		if (r != KErrNone)
			return NULL;
		
		}
	return (RFax::TProgress*)iChunk.Base();	
 	}

/**
Base implementation for virtual API declared in MCallBaseTSY.  This base implementation always returns KErrNotSupported and 
it is therefore necessary for TSYs who support this API to override this in their CCallBase derived class.

This function passes through a pointer to a heap-based CFaxSharedFileHandles object.  This object is instantiated by Etel but 
a successful return of this function will pass responsibility for deletion of aFaxSharedFileHandles from Etel server to the TSY.  
If an error code is returned from this function (e.g. KErrNotSupported) Etel maintains ownership of the aFaxSharedFileHandles and 
is responsible for its deletion.

TSY overrides of this function must correctly handle the cleanup of the aFaxSharedFileHandles object if they take ownership of it 
which is implied when returning from this function with KErrNone.

@publishedPartner
@param aTsyReqHandle Handle to request.
@param aFaxSharedFileHandles CFaxSharedFileHandles instance to be stored in TSY.
@released Released v9.0 to allow a fax client to pass a file handle to the Fax server.
*/
EXPORT_C TInt CCallBase::SetFaxSharedHeaderFile(const TTsyReqHandle /*aTsyReqHandle*/, CFaxSharedFileHandles* /*aFaxSharedFileHandles*/)
	{
	return KErrNotSupported;	
	}

EXPORT_C void CCallBase::DeleteFaxProgressChunk()
	{
	iChunk.Close();
	}

EXPORT_C CTelObject::TReqMode CCallBase::ReqModeL(const TInt aIpc)
//
//	Basic Request Mode for Call
//
	{
	CTelObject::TReqMode mode=0;
	switch (aIpc)
		{
	case EEtelCallNotifyHookChange:
	case EEtelCallNotifyStatusChange:
	case EEtelCallNotifyDurationChange:
	case EEtelCallCapsChangeNotification:
		mode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	case EEtelCallGetInfo:
	case EEtelCallRecoverDataPort:
	case EEtelCallGetStatus:
	case EEtelCallGetCaps:
	case EEtelCallTransferOwnership:
	case EEtelCallAcquireOwnership:
	case EEtelCallGetFaxSettings:
	case EEtelCallGetCallParams:
	case EEtelCallGetCallDuration:
	case EEtelCallAnswer:	// no longer obeys flow control since it may be placed long before
							// a call arrives
		break;
	case EEtelCallGetOwnershipStatus:
	case EEtelCallGetBearerServiceInfo:	
	case EEtelCallSetFaxSettings:
	case EEtelAdoptFaxSharedHeaderFile:
	case EEtelCallReferenceCount:
		break;
	case EEtelCallLoanDataPort:
	case EEtelCallDial:
	case EEtelCallConnect:
	case EEtelCallHangUp:
	
		mode=KReqModeFlowControlObeyed;
		break;

	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return mode;
	}

EXPORT_C TInt CCallBase::NumberOfSlotsL(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EEtelCallNotifyHookChange:
	case EEtelCallNotifyDurationChange:
		break;
	case EEtelCallNotifyStatusChange:
		return KSlotNumbersCallStatusChange;
	case EEtelCallCapsChangeNotification:
		return KSlotNumbersCallCapsChange;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return KSlotNumbersDefault;
	}

//
//
// CLineBase
//
//
EXPORT_C CLineBase::CLineBase()
//
//	C'Tor
//
	{
	__DECLARE_NAME(_S("CLineBase"));
	}

EXPORT_C CLineBase::~CLineBase()
//
//	D'Tor
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CLINEBASE_DTOR_1, "~CLineBase");
	}

EXPORT_C TInt CLineBase::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
//
// Cancel Service for Line
//
	{
	__ASSERT_ALWAYS((	aIpc==EEtelLineNotifyIncomingCall	||
						aIpc==EEtelLineNotifyHookChange		||
						aIpc==EEtelLineNotifyStatusChange	||
						aIpc==EEtelLineNotifyCallAdded		||
						aIpc==EETelLineCapsChangeNotification)
						,Fault(EEtelFaultInvalidIpcForCancel));
	switch (aIpc)
		{
	case EEtelLineNotifyIncomingCall:
		return NotifyIncomingCallCancel(aTsyReqHandle);
	case EEtelLineNotifyHookChange:
		return NotifyHookChangeCancel(aTsyReqHandle);
	case EEtelLineNotifyStatusChange:
		return NotifyStatusChangeCancel(aTsyReqHandle);
	case EEtelLineNotifyCallAdded:
		return NotifyCallAddedCancel(aTsyReqHandle);
	case EETelLineCapsChangeNotification:
		return NotifyCapsChangeCancel(aTsyReqHandle);
	default:
		return KErrGeneral; // should never reaches here
		}
	}

EXPORT_C TInt CLineBase::Service(const RMessage2& aMessage, CReqEntry* aReqEntry)
//
// The ServiceL Functionality for the Line
//
	{
	__ASSERT_ALWAYS(aReqEntry!=NULL,Fault(EEtelFaultCallTsyServiceWithoutReqPackage));
	aReqEntry->iPlacedRequest=ETrue;
	TTsyReqHandle tsyReqHandle=aReqEntry->iTsyReqHandle;
	TDes8* des1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);
	TUint8* ptr1=Ptr1(des1);

	switch (aReqEntry->iFunction)
		{
	case EETelLineCapsChangeNotification:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyCapsChange(tsyReqHandle,REINTERPRET_CAST(RLine::TCaps*,ptr1));
	case EETelLineCapsChangeNotificationCancel:
		return NotifyCapsChangeCancel(tsyReqHandle);
	case EEtelLineNotifyIncomingCall:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyIncomingCall(tsyReqHandle,REINTERPRET_CAST(TName*,ptr1));
	case EEtelLineNotifyIncomingCallCancel:
		return NotifyIncomingCallCancel(tsyReqHandle);

	case EEtelLineNotifyHookChange:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyHookChange(tsyReqHandle,REINTERPRET_CAST(RCall::THookStatus*,ptr1));
	case EEtelLineNotifyHookChangeCancel:
		return NotifyHookChangeCancel(tsyReqHandle);
	case EEtelLineNotifyStatusChange:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyStatusChange(tsyReqHandle,REINTERPRET_CAST(RCall::TStatus*,ptr1));
	case EEtelLineNotifyStatusChangeCancel:
		return NotifyStatusChangeCancel(tsyReqHandle);
	case EEtelLineNotifyCallAdded:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyCallAdded(tsyReqHandle,REINTERPRET_CAST(TName*,ptr1));
	case EEtelLineNotifyCallAddedCancel:
		return NotifyCallAddedCancel(tsyReqHandle);

	case EEtelLineGetHookStatus:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetHookStatus(tsyReqHandle,REINTERPRET_CAST(RCall::THookStatus*,ptr1));

	case EEtelLineGetInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetInfo(tsyReqHandle,REINTERPRET_CAST(RLine::TLineInfo*,ptr1));
	case EEtelLineGetCaps:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCaps(tsyReqHandle,REINTERPRET_CAST(RLine::TCaps*,ptr1));
	case EEtelLineGetStatus:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetStatus(tsyReqHandle,REINTERPRET_CAST(RCall::TStatus*,ptr1));
	case EEtelLineEnumerateCall:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return EnumerateCall(tsyReqHandle,REINTERPRET_CAST(TInt*,ptr1));
	case EEtelLineGetCallInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCallInfo(tsyReqHandle,REINTERPRET_CAST(TCallInfoIndex*,ptr1));
	default:
		return ServiceExtFunc(aMessage,aReqEntry);
		}
	}

EXPORT_C void CLineBase::Init()
//
// not used for Line
//
	{}

EXPORT_C void CLineBase::OpenPostProcessing(CTelSession*,const TInt)
//
// Perform post-processing after object has been added to session's CObjectIx.
//
	{}

EXPORT_C void CLineBase::CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle)
//
// 
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CLINEBASE_CLOSESUBSESSIONPREPROCESSING_1, "CLineBase::CloseSubSessionPreProcessing() - about to create dummy session");
	CreateDummySession(aSession,aSubSessionHandle);
	FlushReqs(aSession,aSubSessionHandle);
	}

EXPORT_C CTelObject::TReqMode CLineBase::ReqModeL(const TInt aIpc)
//
// Mode Request Inquiry Functions
//
	{
	CTelObject::TReqMode mode=0;
	switch (aIpc)
		{
	case EEtelLineGetCaps:
	case EEtelLineGetStatus:
	case EEtelLineGetHookStatus:
	case EEtelLineEnumerateCall:
	case EEtelLineGetCallInfo:
	case EEtelLineGetInfo:
	case EEtelLineNotifyIncomingCall:	// 3/12/98 Removed Repost ability.
		mode=KReqModeMultipleCompletionEnabled;
		break;
	case EEtelLineNotifyHookChange:
	case EEtelLineNotifyStatusChange:
	case EEtelLineNotifyCallAdded:
	case EETelLineCapsChangeNotification:
		mode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return mode;
	}

EXPORT_C TInt CLineBase::NumberOfSlotsL(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EEtelLineNotifyHookChange:
	case EETelLineCapsChangeNotification:
		break;
	case EEtelLineNotifyStatusChange:
		return KSlotNumbersLineStatusChange;
	case EEtelLineNotifyCallAdded:
		return KSlotNumbersLineCallAddedChange;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return KSlotNumbersDefault;
	}

//
//
// CPhoneBase
//
//
EXPORT_C CPhoneBase::CPhoneBase()
//
// CPhoneBase constructor
//
	{
	__DECLARE_NAME(_S("CPhoneBase"));
	iReqWaitList.SetOffset(_FOFF(CReqEntry,iLink));
	iReqActiveList.SetOffset(_FOFF(CReqEntry,iLink));
	iTsyReqHandleCnt=TSY_HANDLE_INIT_VALUE;
	}

EXPORT_C CPhoneBase::~CPhoneBase()
//
//	CPhone destructor.
//	Attual closing of the owner is done in CSubSessionBase destructor
//	This is for all object inherit fron CSubSesissionBase
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CPHONEBASE_DTOR_1, "~CPhoneBase");
	}

//
// Request List Manipulation Functions
//
CReqEntry* CPhoneBase::ActivateNextWaitingReq()
//
// Use:		during completion to Activate the next waiting request
// Action:	Find next Waiting Req, move to active, and return.
//
	{
	if(iReqWaitList.IsEmpty())
		return NULL;

	CReqEntry* reqEntry=iReqWaitList.First();
	if(reqEntry==NULL)
		return NULL;
	reqEntry->iLink.Deque();
	iReqActiveList.AddLast(*reqEntry);
	if (reqEntry->iReqMode & KReqModeMultipleCompletionEnabled)
		{
		CReqEntry* otherClientsEntry;
		TDblQueIter<CReqEntry> iter(iReqWaitList);
		while(otherClientsEntry=iter++, otherClientsEntry!=NULL)
			{
			if(otherClientsEntry->iTsyReqHandle==reqEntry->iTsyReqHandle)
				{
				otherClientsEntry->iLink.Deque();
				iReqActiveList.AddLast(*otherClientsEntry);
				}
			}
		}
	return reqEntry;
	}

CReqEntry* CPhoneBase::FindByTsyHandleAndPlacedRequest(const TTsyReqHandle aTsyReqHandle)
//
// Use: to search active req list for a single req, remove and then return it. Can be called repeatedly for Notification
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry=iter++, reqEntry!=NULL)
		{
		if(reqEntry->iTsyReqHandle==aTsyReqHandle && reqEntry->iPlacedRequest)
			return reqEntry;
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindByTsyHandle(const TTsyReqHandle aTsyReqHandle)
//
// Use: to search active req list for a single req, remove and then return it. Can be called repeatedly for Notification
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry=iter++, reqEntry!=NULL)
		{
		if(reqEntry->iTsyReqHandle==aTsyReqHandle)
			return reqEntry;
		}
	return NULL;
	}

void CPhoneBase::UpdateBuffer(CReqEntry* aUpdatedReqEntry,CReqEntry* aReqEntry)
//
//	Is passed both the TSY-updated request entry (which may be multi-buffered or not)
//	and another request entry which is to have the data copied into it (which again may or may
//	not be multi-buffered) copies the latest data into the target request entry. 
//
	{
	__ASSERT_ALWAYS(aUpdatedReqEntry->iFunction==aReqEntry->iFunction,Fault(EEtelFaultUpdatingBufferOfDifferentIpc));
	TInt msgType = aReqEntry->iMessage.Int1();
	if (msgType==EIsaNarrowAndUnicodeDoubleDesTobeRead)
		{
		TDes8* targetDes1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes16* targetDes2 = BufferDes2u(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes8* sourceDes1 = BufferDes1(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes16* sourceDes2 = BufferDes2u(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		if (sourceDes1!=targetDes1)
			{
			targetDes1->Copy(*sourceDes1);
			if (targetDes2)
				targetDes2->Copy(*sourceDes2);
			aReqEntry->iBuffer->IncWrite();
			}
		return;
		}
	if (aUpdatedReqEntry->iSession->IsUnicodeReq(msgType))
		{
		TDes16* targetDes1 = BufferDes1u(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes16* targetDes2 = BufferDes2u(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes16* sourceDes1 = BufferDes1u(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes16* sourceDes2 = BufferDes2u(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		if (sourceDes1!=targetDes1)
			{
			targetDes1->Copy(*sourceDes1);
			if (targetDes2)
				targetDes2->Copy(*sourceDes2);
			aReqEntry->iBuffer->IncWrite();
			}
		}
	else		// this is narrow
		{
		TDes8* targetDes1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes8* targetDes2 = BufferDes2(aReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes8* sourceDes1 = BufferDes1(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		TDes8* sourceDes2 = BufferDes2(aUpdatedReqEntry->iBuffer,CBuffer::ESlotWrite);
		if (sourceDes1!=targetDes1)
			{
			targetDes1->Copy(*sourceDes1);
			if (targetDes2)
				targetDes2->Copy(*sourceDes2);
			aReqEntry->iBuffer->IncWrite();
			}
		}
	}

CReqEntry* CPhoneBase::FindClientInWaiting(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc)
//
// Used to search waiting list for a req that's to be cancelled.
// If non-Null is returned it will have been removed by this function and can be completed by the caller.
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqWaitList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if((reqEntry->iSession==aSession)&&(reqEntry->iMessage.Int3()==aSubSessionHandle)&&(reqEntry->iFunction==aIpc))
			return reqEntry;
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindClientInActive(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc)
//
// Used to search the active req list for a req that's to be cancelled.
// The function will return any req entry found, by the caller does not know to pass the cancel down until a search has been done just on the aIpc
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if((reqEntry->iSession==aSession)&&(reqEntry->iMessage.Int3()==aSubSessionHandle)&&(reqEntry->iFunction==aIpc))
			return reqEntry;
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindByIpcAndTelObject(const TInt aIpc, const CTelObject* aTelObject, const TUint aBufSize)
//
//	Finds an active request by both IPC and by the TelObject it was placed on
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if(reqEntry->iFunction==aIpc && reqEntry->iTelObject==aTelObject
			&& reqEntry->iBuffer->Size() == aBufSize)	
			return reqEntry;
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindByIpcAndTelObjectInWaiting(const TInt aIpc, const CTelObject* aTelObject, const TUint aBufSize)
//
//	Finds a waiting request by both IPC and by the TelObject it was placed on
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqWaitList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if(reqEntry->iFunction==aIpc && reqEntry->iTelObject==aTelObject
			&& reqEntry->iBuffer->Size() == aBufSize)	
			return reqEntry;
		}
	return NULL;
	}				 

void CPhoneBase::AddReqToActive(CReqEntry* aReqEntry)
//
//	Add reqEntry to the 'active list'
//
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBASE_ADDREQTOACTIVE_1, "CPhoneBase::AddReqToActive with TsyHandle of %d", aReqEntry->iTsyReqHandle);
	iReqActiveList.AddLast(*aReqEntry);
	}

void CPhoneBase::AddReqToWaiting(CReqEntry* aReqEntry)
//
//	Add entry to the 'wait list'
//
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBASE_ADDREQTOWAITING_1, "CPhoneBase::AddReqToWaiting with TsyHandle of %d", aReqEntry->iTsyReqHandle);
	iReqWaitList.AddLast(*aReqEntry);
	}

TTsyReqHandle CPhoneBase::TsyReqHandle()
//
// Return the request handle  if wrap round avoid assigned to TSY_HANDLE_INIT_VALUE
//
	{
	iTsyReqHandleCnt++ ;
	if (iTsyReqHandleCnt==TSY_HANDLE_INIT_VALUE)
		iTsyReqHandleCnt=(TSY_HANDLE_INIT_VALUE+1);
	return iTsyReqHandleCnt;
	}

CReqEntry* CPhoneBase::NewReqL(const RMessage2& aMessage, CTelSession* aSession,CBuffer* aBuffer,
							   const CTelObject* aTelObject,TInt aFunction)
//
// Create new req entry
//
	{
	return CReqEntry::NewL(TsyReqHandle(),aMessage,aSession,aBuffer,aTelObject,aFunction,aSession->EmergencyClientHeap(aMessage.Int1()));
	}

CReqEntry* CPhoneBase::FindSameClientEntry(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc)
//
// Seaching for duplicate request and panic client is return not NULL
//
	{
	CReqEntry* reqEntry=FindClientInWaiting(aSession,aSubSessionHandle,aIpc);

	if (reqEntry!=NULL)
		return reqEntry;
	else // not found in waiting
		reqEntry=FindClientInActive(aSession,aSubSessionHandle,aIpc);
	return reqEntry;

	}

EXPORT_C void CPhoneBase::OpenPostProcessing(CTelSession*,const TInt)
//
// Perform post-processing after object has been added to session's CObjectIx.
//
	{}

EXPORT_C void CPhoneBase::CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle)
//
//  Find if any active request if so create a dummy session
//
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CPHONEBASE_CLOSESUBSESSIONPREPROCESSING_1, "CPhoneBase::CloseSubSessionPreProcessing");
	CreateDummySession(aSession,aSubSessionHandle);
	FlushReqs(aSession,aSubSessionHandle);
	}

EXPORT_C TInt CPhoneBase::Service(const RMessage2& aMessage, CReqEntry* aReqEntry)
//
// The ServiceL functionality for the phone
//
	{
	__ASSERT_ALWAYS(aReqEntry!=NULL,Fault(EEtelFaultCallTsyServiceWithoutReqPackage));
	aReqEntry->iPlacedRequest=ETrue;
	TTsyReqHandle tsyReqHandle=aReqEntry->iTsyReqHandle;
	TDes8* des1 = BufferDes1(aReqEntry->iBuffer,CBuffer::ESlotWrite);
	TUint8* ptr1=Ptr1(des1);
	switch (aReqEntry->iFunction)
		{
	case EETelPhoneCapsChangeNotification:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyCapsChange(tsyReqHandle,REINTERPRET_CAST(RPhone::TCaps*,ptr1));
	case EETelPhoneCapsChangeNotificationCancel:
		return NotifyCapsChangeCancel(tsyReqHandle);
	case EEtelPhoneInitialise:
		return ControlledInitialisation(tsyReqHandle);
	case EEtelPhoneInitialiseCancel:
		return ControlledInitialisationCancel(tsyReqHandle);
	case EEtelPhoneNotifyModemDetected:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return NotifyModemDetected(tsyReqHandle,REINTERPRET_CAST(RPhone::TModemDetection*,ptr1) );
	case EEtelPhoneNotifyModemDetectedCancel:
		return NotifyModemDetectedCancel(tsyReqHandle);
	case EEtelPhoneGetInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetInfo(tsyReqHandle,REINTERPRET_CAST(RPhone::TPhoneInfo*,ptr1));
	case EEtelPhoneGetCaps:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetCaps(tsyReqHandle,REINTERPRET_CAST(RPhone::TCaps*,ptr1));
	case EEtelPhoneGetStatus:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetStatus(tsyReqHandle,REINTERPRET_CAST(RPhone::TStatus*,ptr1));
	case EEtelPhoneEnumerateLines:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return EnumerateLines(tsyReqHandle,REINTERPRET_CAST(TInt*,ptr1));
	case EEtelPhoneGetLineInfo:
		__ASSERT_ALWAYS(des1!=NULL,Fault(EETelFaultRequestWithoutBuffer));
		return GetLineInfo(tsyReqHandle,REINTERPRET_CAST(TLineInfoIndex*,ptr1));
	default:
		return ServiceExtFunc(aMessage,aReqEntry);
		}
	}

EXPORT_C TInt CPhoneBase::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
//
// Cancel Outstanding service for the phone
//
	{
	switch (aIpc)
		{
	case EEtelPhoneNotifyModemDetected:
		return NotifyModemDetectedCancel(aTsyReqHandle);
	case EETelPhoneCapsChangeNotification:
		return NotifyCapsChangeCancel(aTsyReqHandle);
	case EEtelPhoneInitialise:
		return ControlledInitialisationCancel(aTsyReqHandle);
	default:
		return KErrGeneral;
		}
	}

CReqEntry* CPhoneBase::FindClientReqInWaitList(CTelSession* aSession,const TInt aSubSessionHandle)
//
// Find first entry already logged by this client - in wait list
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqWaitList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if(aSession==reqEntry->iSession && aSubSessionHandle==reqEntry->iMessage.Int3())
			return reqEntry;
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindThisReqByAnotherClient(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc,const TUint aBufSize,const CTelObject* aThisTelObject)
//
// Find this IPC in active list, placed by a different client
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry=iter++,reqEntry!=NULL)
		{
		if((aSession!=reqEntry->iSession || aSubSessionHandle!=reqEntry->iMessage.Int3()) 
			&& reqEntry->iFunction==aIpc && reqEntry->iTelObject==aThisTelObject
			&& reqEntry->iBuffer->Size()==aBufSize)
			{
			return reqEntry;
			}
		}
	return NULL;
	}

CReqEntry* CPhoneBase::FindNonCancelledClientReq(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc)
//
// Find this IPC in active list, placed by this client but not the one which placed request on TSY
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry=iter++,reqEntry!=NULL)
		{
		if(aSession==reqEntry->iSession && aSubSessionHandle==reqEntry->iMessage.Int3() && reqEntry->iFunction==aIpc && reqEntry->iCancelFnCalled==EFalse)
			{
			return reqEntry;
			}
		}
	return NULL;
	}
  
void CPhoneBase::CheckAndCompleteAllActive(CReqEntry* aUpdatedReqEntry,const TReqMode aReqMode,const TInt aIpc,const TInt aError)
//
//	Cycles through active list, checking for any requests with the same IPC value and if 
//	registered, copying the data from the buffer that the TSY filled in to this buffer. All those
//	with the same IPC are completed up to the client (if the client is ready).
//
	{
	CReqEntry* reqEntry;
	TDblQueIter<CReqEntry> iter(iReqActiveList);
	while(reqEntry = iter++, reqEntry!=NULL)
		{
		if(reqEntry->iFunction==aIpc && reqEntry!=aUpdatedReqEntry 
			&& reqEntry->iTelObject==aUpdatedReqEntry->iTelObject 
			&& reqEntry->iBuffer->Size()==aUpdatedReqEntry->iBuffer->Size())	
			{						   
			UpdateBuffer(aUpdatedReqEntry,reqEntry);
			TInt error = ResolveError(reqEntry->iSession,aError);		// set error as either low or high byte
			if (aReqMode & KReqModeRePostImmediately)
				UpdateAndCompleteIfNecessary(reqEntry,error);
			else
				WriteBackAndCompleteReq(reqEntry,error);
			}
		}
	}
//
// CPhoneBase performs Flow Control
//
TInt CPhoneBase::FlowControl() const
//
// Return the value of iFlowControlCnt
//
 	{
	return iFlowControlCnt;
	}

void CPhoneBase::FlowControlInc()
//
// Increase iFlowControlCnt
//
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBASE_FLOWCONTROLINC_1, "Incrementing Flow Control from %d to %d", iFlowControlCnt, iFlowControlCnt+1);
	iFlowControlCnt++;
	}

void CPhoneBase::FlowControlDec()
//
// Decrease iFlowControlCnt
//
	{
	__ASSERT_ALWAYS((iFlowControlCnt>0),Fault(EEtelFaultNegativeFlowcontrolCount));
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBASE_FLOWCONTROLDEC_1, "Decrementing Flow Control from %d to %d", iFlowControlCnt, iFlowControlCnt-1);
	iFlowControlCnt--;
	}

EXPORT_C TBool CPhoneBase::CheckEmergencyClient(const TTsyReqHandle aTsyReqHandle) const
	{
	CReqEntry* reqEntry=PhoneOwner()->FindByTsyHandle(aTsyReqHandle);
	__ASSERT_ALWAYS(reqEntry!=NULL,Fault(EEtelFaultNotRecognisedTsyHandle));

	return reqEntry->iSession->TelServer()->IsEmergencyClient(reqEntry->iSession);
	}


EXPORT_C CTelObject::TReqMode CPhoneBase::ReqModeL(const TInt aIpc)
//
// Mode Request Inquiry Functions
//
	{
	CTelObject::TReqMode mode=0;
	switch (aIpc)
		{
	case EEtelPhoneInitialise:	//test
		mode=KReqModeFlowControlObeyed | KReqModeMultipleCompletionEnabled;
		break;
	case EEtelPhoneGetCaps:
	case EEtelPhoneGetStatus:
	case EEtelPhoneEnumerateLines:
	case EEtelPhoneGetLineInfo:
	case EEtelPhoneGetInfo:
		mode=KReqModeMultipleCompletionEnabled;
		break;	
	case EETelPhoneSetEmergencyClient:
		mode=KReqModeFlowControlObeyed;
		break;
	case EEtelPhoneNotifyModemDetected:
	case EETelPhoneCapsChangeNotification:
		mode=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return mode;
	}

EXPORT_C TInt CPhoneBase::NumberOfSlotsL(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EEtelPhoneNotifyModemDetected:
	case EETelPhoneCapsChangeNotification:
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	return KSlotNumbersDefault;
	}

TDblQue<CReqEntry>& CPhoneBase::ReqActiveList()
	{
	return iReqActiveList;
	}


