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
//



// INCLUDE FILES
#include <s32mem.h>
#include <e32property.h>
#include <lbs/epos_privacy.h>
#include <lbs/epos_cposcontactrequestor.h>
#include <lbs/epos_cposservicerequestor.h>
#include <lbs/epos_rposrequestorstack.h>
#include <lbs/epos_privacynotifier.hrh>
#include "EPos_CPosDialogCtrl.h"
#include <lbs/lbslocclasstypes.h>

// CONSTANTS
#if defined(NRH_UNIT_TEST)
// For the NRH unit test, use a different Q&N notifier implementation UID.
// This is so that we can have both the unit test Q&N notifier and
// the integration test Q&N notifier (with the real implementation UID)
// in the same ROM.
const TUid KNotifierUid = { 0x10283744 };
#else
const TUid KNotifierUid = { KPosPrivacyNotifierImplUid };
#endif // NRH_UNIT_TEST

// P&S Key so NG can check the status of notifications
const TInt EPrivacyNumberOfRequest = 0x1028720F;

// The output descriptor for Notifier calls. Since we want to pass
// just a dummy null descriptor there is no need to create a HBufC
// each time.
TPtr8 nullPtr(reinterpret_cast<TUint8*>(NULL), 0);


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosDialogCtrl::CPosDialogCtrl() : CActive(EPriorityNormal),
    iRequestQueue(1) // Set granularity to 1
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CPosDialogCtrl::ConstructL()
    {
    TInt err = iNotifier.Connect();
    User::LeaveIfError(err);
    
    // Define the property
    RProperty::Define(KUidSystemCategory, EPrivacyNumberOfRequest, RProperty::EInt);
    UpdateProperty();
    }

// Two-phased constructor.
CPosDialogCtrl* CPosDialogCtrl::InstanceL()
    {
    CPosDialogCtrl* self = new (ELeave) CPosDialogCtrl;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosDialogCtrl::~CPosDialogCtrl()
    {
    Cancel();
    TInt count = iRequestQueue.Count();
    for (TInt i = 0; i < count; i++)
        {
        DoCompleteRequest(0, KErrServerTerminated);
        }

    iNotifier.Close();
    iRequestQueue.Close();
    RProperty::Delete(KUidSystemCategory, EPrivacyNumberOfRequest);
    }

// ---------------------------------------------------------
// CPosDialogCtrl::NotifyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::NotifyL(
	TLbsExternalRequestInfo& aRequestInfo,
    const TPosRequestData& aRequestData)
    {
    TRequest request;
    request.iType = TPosQNInputData::ENotification;
    request.iRequestInfo = &aRequestInfo;
    request.iStatus = NULL;
    request.iRequestSource = aRequestData.iRequestSource;
    request.iTimeoutStrategy = aRequestData.iTimeoutStrategy;
    request.iRequestDecision = aRequestData.iRequestDecision;
    request.iCancelReason = aRequestData.iCancelReason;
    request.iNotificationReason = aRequestData.iNotificationReason;

    ScheduleRequestL(request);
    }

// ---------------------------------------------------------
// CPosDialogCtrl::VerifyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::VerifyL(
    TRequestStatus& aStatus,
	TLbsExternalRequestInfo& aRequestInfo,
    const TPosRequestData& aRequestData)
    {
    aStatus = KRequestPending;

    TRequest request;
    request.iType = TPosQNInputData::EQuery;
    request.iRequestInfo = &aRequestInfo;
    request.iStatus = &aStatus;
    request.iRequestSource = aRequestData.iRequestSource;
    request.iTimeoutStrategy = aRequestData.iTimeoutStrategy;
    request.iRequestDecision = aRequestData.iRequestDecision;
    request.iCancelReason = aRequestData.iCancelReason;
    request.iNotificationReason = aRequestData.iNotificationReason;

    GivePriorityToVerificationL(request);
    }

// ---------------------------------------------------------
// CPosDialogCtrl::CancelLaunch
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::CancelLaunch(TRequestStatus& aStatus)
    {
    TInt count = iRequestQueue.Count();

    if (count > 0 && iRequestQueue[0].iStatus == &aStatus)
        {
        Cancel();
        DoCompleteRequest(0, KErrCancel);
        NextRequest();
        }
    else
        {
        for (TInt i = 1; i < count; ++i)
            {
            if (iRequestQueue[i].iStatus == &aStatus)
                {
                DoCompleteRequest(i, KErrCancel);
                return;
                }
            }
        }
    }

// ---------------------------------------------------------
// CPosDialogCtrl::CancelLaunch
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::CancelLaunch(
    TRequestStatus& aStatus,
    TInt& aErrorCode,
    TInt& aResponse,
    TBool& aRequestQueued,
    TPosVerifyCancelReason aCancelReason)
    {
    TInt count = iRequestQueue.Count();

    if (count > 0 && iRequestQueue[0].iStatus == &aStatus)
        {
        TPtr8 nullPtr(reinterpret_cast<TUint8*>(NULL), 0);
        TPosQNInputData data;
        data.iId = iRequestQueue[0].iId;
        data.iCancelReason = aCancelReason;

        TPckg<TPosQNInputData> dataPtr(data);

        aErrorCode = iNotifier.UpdateNotifier(KNotifierUid, dataPtr, nullPtr);
        if( KErrNone==aErrorCode  /* && iRequestQueue[0].iTimeoutStrategy == EPosDecisionAccepted  */)  
	        {
	        // If there is an error, notifications will be triggered through PrivManager
	        TRAPD(err,CreateNotificationAndEnqueueL());
	        err = err; // To avoid compiler warning
	        }
        aResponse = iStatus.Int();
        aRequestQueued = EFalse;
        }
    else
        {
        aResponse = KErrNone;
        aErrorCode = KErrNone;
        aRequestQueued = ETrue;
        }
    CancelLaunch(aStatus);
    }

// ---------------------------------------------------------
// CPosDialogCtrl::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::RunL()
    {
    if (iRequestQueue.Count() > 0)
        {
        DoCompleteRequest(0, iStatus.Int());
        NextRequest();
        }
    }

// ---------------------------------------------------------
// CPosDialogCtrl::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::DoCancel()
    {
    if (iStatus == KRequestPending)
        {
        iNotifier.CancelNotifier(KNotifierUid);
        }
    }

// ---------------------------------------------------------
// CPosDialogCtrl::DialogActive
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CPosDialogCtrl::DialogActive() const
    {
    return iRequestQueue.Count() > 0;
    }

// ---------------------------------------------------------
// CPosDialogCtrl::NextRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::NextRequest()
    {
    while (iRequestQueue.Count() > 0)
        {
        TInt err = StartNotifierRequest();
        if (err)
            {
            DoCompleteRequest(0, err);
            }
        else
            {
            return;
            }
        }
    }

// ---------------------------------------------------------
// CPosDialogCtrl::DoCompleteRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::DoCompleteRequest(
    TInt aIndex,
    TInt aCompletionCode)
    {
    TRequest request = iRequestQueue[aIndex];
    if (request.iType == TPosQNInputData::ENotification)
        {
        delete request.iRequestInfo;
        }
    else
        {
        TRequestStatus* status = request.iStatus;
        User::RequestComplete(status, aCompletionCode);
        }
    iRequestQueue.Remove(aIndex);
    iRequestQueue.Compress();
    UpdateProperty();
    }

// ---------------------------------------------------------
// CPosDialogCtrl::ScheduleRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::ScheduleRequestL(TRequest& aRequest)
    {
    // Start a new request.    
    aRequest.iId = ++iRequestId;
    User::LeaveIfError(iRequestQueue.Append(aRequest));
    UpdateProperty();

    if (iRequestQueue.Count() == 1)
        {
        TInt err = StartNotifierRequest();

        if (err != KErrNone)
            {
            DoCompleteRequest(0, err);
            }
        }
    }

// ---------------------------------------------------------
// CPosDialogCtrl::StartNotifierRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosDialogCtrl::StartNotifierRequest()
    {
    TRequest request = iRequestQueue[0];
    
    // Convert the TLbsExternalRequestInfo into a CPosRequestorStack
    RPosRequestorStack reqStack;
    TRAPD(err, ExtractRequestorsL(*request.iRequestInfo, reqStack));
    if (err != KErrNone)
    	{
    	return err;
    	}
    TInt dataSize = CalcSize(reqStack);

    HBufC8* reqStackBuf = HBufC8::New(dataSize);
    if (!reqStackBuf)
        {
        reqStack.ResetAndDestroy();
        return KErrNoMemory;
        }

    TPtr8 ptr = reqStackBuf->Des();
    TRAP(err, CreateSerializedStackL(ptr, reqStack));
    if (err == KErrNone)
        {
        TPosQNInputData data;
        data.iId = request.iId;
        data.iType = request.iType;
        data.iDataSize = dataSize;
        data.iDataPtr = NULL;
        data.iRequestSource = request.iRequestSource;
        data.iTimeoutStrategy = request.iTimeoutStrategy;
        data.iRequestDecision = request.iRequestDecision;
        data.iNotificationReason = request.iNotificationReason;
        data.iCancelReason = request.iCancelReason;

        TPckg<TPosQNInputData> dataPtr(data);

        HBufC8* buffer = HBufC8::New(KPosQNInputDataClassSize +
            reqStackBuf->Size());
        if (!buffer)
            {
            delete reqStackBuf;
            return KErrNoMemory;
            }

        buffer->Des().Append(dataPtr);
        buffer->Des().Append(*reqStackBuf);

        delete reqStackBuf;
        reqStackBuf = NULL;

        iNotifier.StartNotifierAndGetResponse(
            iStatus, KNotifierUid, nullPtr, nullPtr);
        SetActive();

        err = iNotifier.UpdateNotifier(KNotifierUid, *buffer, nullPtr);
        if (err != KErrNone)
            {
            Cancel();
            }
        delete buffer;
        }

    delete reqStackBuf;
    reqStack.ResetAndDestroy();
    return err;
    }

// ---------------------------------------------------------
// CPosDialogCtrl::CreateSerializedStackL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::CreateSerializedStackL(
    TDes8& aBuffer,
    const RPosRequestorStack& aRequestStack)
    {
    RDesWriteStream writeStream(aBuffer);
    CleanupClosePushL(writeStream);
    aRequestStack.ExternalizeL(writeStream);
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(1, &writeStream);
    }

// ---------------------------------------------------------
// CPosDialogCtrl::CalcSize
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosDialogCtrl::CalcSize(
    const RPosRequestorStack& aRequestStack)
    {
    TInt size = sizeof(TInt);	// One TInt for number of requestors.
    const TInt numRequestors = aRequestStack.Count();
    for (TInt i = 0; i < numRequestors; i++)
    	{
    	const CPosRequestor* requestor = aRequestStack[i];
    	
    	// Add on the size of the requestor class, the 
    	// descriptor it contains, and an extra TInt which
    	// is used to store the length of the descriptor.
    	size += sizeof(CPosRequestor);
    	size += sizeof(TInt);
    	size += requestor->RequestorIdString().Size();
    	}

    return size;
    }

/** Convert a TLbsExternalRequestInfo into a RPosRequestorStack.
 */
void CPosDialogCtrl::ExtractRequestorsL(
 		const TLbsExternalRequestInfo& aRequestInfo,
     	RPosRequestorStack& aRequestors)
 	{	
 	// Convert the Client name into a CPosRequestor::ERequestorService requestor.
 	switch (aRequestInfo.ClassType())
 		{
 		case EExternalRequestInfoClass:
 			{
 			TBuf<KLbsMaxClientNameSize> buf;
 			aRequestInfo.GetClientName(buf);
 			AddRequestorL(CPosRequestor::ERequestorService,
 						  aRequestInfo.ClientNameFormat(),
 						  aRequestInfo.RequestType(),
 						  aRequestInfo.NetworkType(),
 						  buf,
 						  aRequestors
 						  );
 			break;
 			}
 		case (EExternalRequestInfoClass 
 			  | EExternalRequestInfoClass2):
 		  	{
   			const TLbsExternalRequestInfo2& info = static_cast<const TLbsExternalRequestInfo2&>(aRequestInfo);
   			TBuf<KLbsMaxClientNameSize2> buf;
 			info.GetClientName(buf);
 			AddRequestorL(CPosRequestor::ERequestorService,
 						  aRequestInfo.ClientNameFormat(),
 						  aRequestInfo.RequestType(),
 						  aRequestInfo.NetworkType(),
 						  buf,
 						  aRequestors
 						  );		
 		  	break;
 		  	}
 		default:
 			{
 			// Unrecognised TLbsExternalRequestInfo class type, so ignore it.
 			break;
 			}
 		}	
 	
 	// Convert the Requestor name into a CPosRequestor::ERequestorContact requestor.
 	switch (aRequestInfo.ClassType())
 		{
 		case EExternalRequestInfoClass:
 			{
 			TBuf<KLbsMaxRequesterIdSize> buf;
 			aRequestInfo.GetRequesterId(buf);
 			AddRequestorL(CPosRequestor::ERequestorContact,
 						  aRequestInfo.RequesterIdFormat(),
 						  aRequestInfo.RequestType(),
 						  aRequestInfo.NetworkType(),
 						  buf,
 						  aRequestors
 						  );
 			break;
 			}
 		case (EExternalRequestInfoClass 
 			  | EExternalRequestInfoClass2):
 		  	{
   			const TLbsExternalRequestInfo2& info = static_cast<const TLbsExternalRequestInfo2&>(aRequestInfo);
   			TBuf<KLbsMaxRequesterIdSize2> buf;
 			info.GetRequesterId(buf);
 			AddRequestorL(CPosRequestor::ERequestorContact,
 						  aRequestInfo.RequesterIdFormat(),
 						  aRequestInfo.RequestType(),
 						  aRequestInfo.NetworkType(),
 						  buf,
 						  aRequestors
 						  );
 		  	break;
 		  	}
 		default:
 			{
 			// Unrecognised TLbsExternalRequestInfo class type, so ignore it.
 			break;
 			}
 		}
 	}

 /**
 */	
void CPosDialogCtrl::AddRequestorL(
 		CPosRequestor::TRequestorType aRequestorType,
 		TLbsExternalRequestInfo::TFormatIndicator aIdType,
 	    CPosRequestor::TRequestType aRequestType,
 	    CPosRequestor::TNetworkType aNetworkType,
 		const TDesC& aId,
 		RPosRequestorStack& aRequestors)
 	{
 	CPosRequestor::TRequestorIdFormat format = CPosRequestor::EIdFormatUnknown;
 	switch (aIdType)
 		{
 		case TLbsExternalRequestInfo::EFormatLogicalName :
 			format = CPosRequestor::EIdFormatGenericName;
 			break;
 		case TLbsExternalRequestInfo::EFormatMSISDN :
 			format = CPosRequestor::EIdFormatPhoneNumber;
 			break;
 		case TLbsExternalRequestInfo::EFormatEmailAddress :
 			format = CPosRequestor::EIdFormatEmail;
 			break;
 		case TLbsExternalRequestInfo::EFormatURL :
 			format = CPosRequestor::EIdFormatUrl;
 			break;
 		case TLbsExternalRequestInfo::EFormatSIPURL :
 			format = CPosRequestor::EIdFormatSIPUrl;
 			break;
 		case TLbsExternalRequestInfo::EFormatIMSPublicIdentity :
 			format = CPosRequestor::EIdFormatIMSPublicIdentity;
 			break;
 		case TLbsExternalRequestInfo::EFormatMIN:
 			format = CPosRequestor::EIdFormatMIN;
 			break;
 		case TLbsExternalRequestInfo::EFormatMDN:
 			format = CPosRequestor::EIdFormatMDN;
 			break;
 		}

 	// Only add the requestor to the stack if it is a recognised format.
 	if (format != CPosRequestor::EIdFormatUnknown)
 		{
 	    CPosRequestor* requestor = NULL;
 	    if (aRequestorType == CPosRequestor::ERequestorContact)
 	        {
 	        requestor = CPosContactRequestor::NewLC(format, aId);
 	        }
 	    else
 	        {
 	        requestor = CPosServiceRequestor::NewLC(format, aId);
 	        }
 	        
 	    requestor->SetRequestType(aRequestType);
 	    requestor->SetNetworkType(aNetworkType);
     	User::LeaveIfError(aRequestors.Append(requestor));
     	CleanupStack::Pop(requestor);
 		}	
 	}
 
 
// ---------------------------------------------------------
// CPosDialogCtrl::GivePriorityToVerificationL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::GivePriorityToVerificationL(TRequest& aRequest)
    {
    TBool notificationDeferred = EFalse;
    
    if( iRequestQueue.Count() == 0 )
	    {
	    ScheduleRequestL(aRequest);
	    return;
	    }
	if ( iRequestQueue[0].iType == TPosQNInputData::ENotification )
		{
		notificationDeferred=ETrue; // To indicate that notification is deferred,dont remove it from queue.
		}
		
	EnqueueRequestL(aRequest);
	
	if(notificationDeferred)
		{
		Cancel();	
	    DeferNotificationL();
		}
    }
// ---------------------------------------------------------
// CPosDialogCtrl::EnqueueRequestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::EnqueueRequestL(TRequest& aRequest)
    {
    aRequest.iId = ++iRequestId;
    
    for( TInt i = 1; i < iRequestQueue.Count(); i++ )
		{
		if ( iRequestQueue[i].iType == TPosQNInputData::ENotification )
			{
			User::LeaveIfError(iRequestQueue.Insert( aRequest,i ));
			UpdateProperty();
			return;
			}
		}
	
    User::LeaveIfError(iRequestQueue.Append(aRequest));
    UpdateProperty();
    }

// ---------------------------------------------------------
// CPosDialogCtrl::DeferNotificationL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::DeferNotificationL()
	{
	if( KErrCancel == iStatus.Int() )
		{
		//Notification request has been cancelled..Defer it by adding 
		// after verification requests,remove it from first place & schedule next request.
		TRequest request = iRequestQueue[0];
		EnqueueRequestL(request);
		}
	iRequestQueue.Remove(0);
    iRequestQueue.Compress();
    UpdateProperty();
	NextRequest();	
	}

// ---------------------------------------------------------
// CPosDialogCtrl::CreateNotificationAndEnqueueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosDialogCtrl::CreateNotificationAndEnqueueL()
	{   
    TRequest verifyRequest = iRequestQueue[0];   
    TRequest notifyRequest;
    TLbsExternalRequestInfo* requestinfocopy = NULL;
    
    if (verifyRequest.iRequestInfo->ClassType() == EExternalRequestInfoClass) 
        {        
        requestinfocopy = new (ELeave) TLbsExternalRequestInfo;
        CleanupStack::PushL(requestinfocopy);
        Mem::Copy(requestinfocopy, verifyRequest.iRequestInfo, sizeof(TLbsExternalRequestInfo));
        }
    else 
        {
        __ASSERT_DEBUG(verifyRequest.iRequestInfo->ClassType() == EExternalRequestInfoClass | EExternalRequestInfoClass2, User::Invariant());
        requestinfocopy = new (ELeave) TLbsExternalRequestInfo2;
        CleanupStack::PushL(requestinfocopy);
        Mem::Copy(requestinfocopy, verifyRequest.iRequestInfo, sizeof(TLbsExternalRequestInfo2));        
        }

    notifyRequest.iRequestInfo = requestinfocopy;    
    notifyRequest.iType = TPosQNInputData::ENotification;
    notifyRequest.iStatus = NULL;
    notifyRequest.iRequestSource = EPosRequestSourceNetwork;
    
    notifyRequest.iTimeoutStrategy = EPosDecisionNotAvailable;
    notifyRequest.iRequestDecision = verifyRequest.iTimeoutStrategy;
    
    notifyRequest.iCancelReason = EPosCancelReasonTimeout; 
    notifyRequest.iNotificationReason = EPosVerificationTimeout;
    
    EnqueueRequestL(notifyRequest);        
    CleanupStack::Pop(requestinfocopy);
    
	}

void CPosDialogCtrl::UpdateProperty()
    {
    RProperty::Set(KUidSystemCategory, EPrivacyNumberOfRequest, iRequestQueue.Count());
    }


//  End of File
