// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "lbsdevloggermacros.h"
#include "suplproxyprivacyrequestinfo.h"


//---------------------------------------------------------------------------
// CSuplProxyRequestInfoBase:
//---------------------------------------------------------------------------


/** 
Get the request ID.

@return The request ID associated with this request.
*/  
TInt CSuplProxyRequestInfoBase::GetRequestId() const
	{
	LBSLOG(ELogP1, "CSuplProxyRequestInfoBase::GetRequestId() Begin and End \n");
	return iRequestId;
	}


/** 
Set the request ID.

@param aRequestId The request ID to be associated with this request.
*/  
void CSuplProxyRequestInfoBase::SetRequestId(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyRequestInfoBase::SetRequestId() Begin and End\n");
	iRequestId = aRequestId;
	}


/** 
Get the request type.

@return The type of request.
*/  
CSuplProxyRequestInfoBase::TSuplProxyPrivacyRequestType CSuplProxyRequestInfoBase::Type() const
	{
	LBSLOG(ELogP1, "CSuplProxyRequestInfoBase::Type() Begin and End\n");
	return iRequestType;
	}


/** 
Constructor.

@param aRequestType The type of request.
*/  
CSuplProxyRequestInfoBase::CSuplProxyRequestInfoBase(CSuplProxyRequestInfoBase::TSuplProxyPrivacyRequestType aRequestType) :
	iRequestType(aRequestType)
	{
	LBSLOG(ELogP1, "CSuplProxyRequestInfoBase::CSuplProxyRequestInfoBase() Begin and End\n");
	}


//---------------------------------------------------------------------------
// CSuplProxyPrivacyRequestInfo:
//---------------------------------------------------------------------------


/** 
Static public constructor.

@return A new instance of this class.
*/  
CSuplProxyPrivacyRequestInfo* CSuplProxyPrivacyRequestInfo::NewL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::NewL() Begin\n");

	CSuplProxyPrivacyRequestInfo* self = new (ELeave) CSuplProxyPrivacyRequestInfo(aMessage);

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::NewL() End\n");

	return self;
	}


/** 
Destructor.
*/  
CSuplProxyPrivacyRequestInfo::~CSuplProxyPrivacyRequestInfo()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::~CSuplProxyPrivacyRequestInfo() Begin and End\n");
	}
		

/** 
Complete this request.

@param aStatus Status to complete with.
*/  
void CSuplProxyPrivacyRequestInfo::CompleteRequest(TInt aStatus)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::CompleteRequest() Begin\n");

    LBSLOG(ELogP7, "->A CSuplProxyPrivacyRequestInfo::CompleteRequest(aStatus)\n");
    LBSLOG2(ELogP7, "\t > TInt aStatus = %d\n", aStatus);
	iResponseMessage.Complete(aStatus);

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::CompleteRequest() End\n");

	}
		

/** 
Get the TLbsExternalRequestInfo2 associated with this request.

@param aRequestInfo Reference to the TLbsExternalRequestInfo2 to populate.
*/  
void CSuplProxyPrivacyRequestInfo::GetRequestInfo(TLbsExternalRequestInfo2& aRequestInfo) const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::GetRequestInfo() Begin\n");
	aRequestInfo = iRequestInfo;
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::GetRequestInfo() End\n");
	}


/** 
Set the TLbsExternalRequestInfo2 associated with this request.

@param aRequestInfo Reference to the TLbsExternalRequestInfo2 to set.
*/  
void CSuplProxyPrivacyRequestInfo::SetRequestInfo(const TLbsExternalRequestInfo2& aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetRequestInfo() Begin\n");
	iRequestInfo = aRequestInfo;
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetRequestInfo() End\n");
	}


/** 
Set the TLbsNetPosRequestPrivacy associated with this request.

@param aRequestPrivacy Reference to the TLbsNetPosRequestPrivacy to populate.
*/  
void CSuplProxyPrivacyRequestInfo::GetRequestPrivacy(TLbsNetPosRequestPrivacy& aRequestPrivacy) const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::GetRequestPrivacy() Begin\n");

	aRequestPrivacy = iRequestPrivacy;

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::GetRequestPrivacy() End\n");
	}


/** 
Set the TLbsNetPosRequestPrivacy associated with this request.

@param aRequestPrivacy Reference to the TLbsNetPosRequestPrivacy to set.
*/  
void CSuplProxyPrivacyRequestInfo::SetRequestPrivacy(const TLbsNetPosRequestPrivacy& aRequestPrivacy)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetRequestPrivacy() Begin\n");

	iRequestPrivacy = aRequestPrivacy;

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetRequestPrivacy() End\n");
	}


/** 
Determine if this request requires a response.

@return ETrue if a response if required, EFalse otherwise.
*/  
TBool CSuplProxyPrivacyRequestInfo::IsResponseRequired() const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::IsResponseRequired() Begin and End\n");
	return iResponseRequired;
	}


/** 
Set if this request requires a response.

@param aResponseRequired ETrue if a response is required, EFalse otherwise.
*/  
void CSuplProxyPrivacyRequestInfo::SetIsResponseRequired(TBool aResponseRequired)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetIsResponseRequired() Begin and End\n");
	iResponseRequired = aResponseRequired;
	}

/** 
Determine if the session should be completed by the Privacy Protocol Module, or by the client

@return ETrue If the Privacy Protocol Module should complete the session, EFalse otherwise
*/
TBool CSuplProxyPrivacyRequestInfo::IsSessionCompleteAutomatic() const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::IsSessionCompleteAutomatic() Begin and End\n");
	return iSessionCompleteAutomatic;	
	}

/** 
Set if the session should be completed by the Privacy Protocol Module, or by the client

@param aAutomatic If it's set to ETrue then the  Privacy Protocol Module should complete 
					the session, otherwise the client will
*/
void CSuplProxyPrivacyRequestInfo::SetIsSessionCompleteAutomatic(TBool aAutomatic)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetIsSessionCompleteAutomatic() Begin\n");

	iSessionCompleteAutomatic = aAutomatic;

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetIsSessionCompleteAutomatic() End\n");

	}

/** 
Determine if the response from LBS should be converted to an error code 
before sending to client or not.

@return ETrue If the Privacy Protocol Module should convert the response, EFalse otherwise
*/
TBool CSuplProxyPrivacyRequestInfo::ConvertResponseCode() const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::ConvertResponseCode() Begin and End\n");
	return iConvertResponseCode;	
	}

/** 
Set if the response from LBS should be converted to an error code by the 
Privacy Protocol Module, or not.

@param aConvert If it's set to ETrue then the Privacy Protocol Module will convert 
					the return code, otherwise it won't
*/
void CSuplProxyPrivacyRequestInfo::SetConvertResponseCode(TBool aConvert)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::SetConvertResponseCode() Begin and End\n");
	iConvertResponseCode = aConvert;	
	}

/**
Get the session associated with this request.

@return A pointer to the session.
*/
const CSession2* CSuplProxyPrivacyRequestInfo::Session() const
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::Session() Begin and End\n");
	if (iResponseMessage.IsNull())
		{
		return NULL;
		}
	else
		{
		return iResponseMessage.Session();
		}
	}

/** 
Constructor

@param aMessage The message associated with this response. This may be completed later if a response is required.
*/  
CSuplProxyPrivacyRequestInfo::CSuplProxyPrivacyRequestInfo(const RMessage2& aMessage) :
CSuplProxyRequestInfoBase(CSuplProxyRequestInfoBase::ESuplProxyPrivacyRequestTypeRequest),
	iResponseMessage(aMessage), iSessionCompleteAutomatic(ETrue), 
	iConvertResponseCode(ETrue)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestInfo::CSuplProxyPrivacyRequestInfo() Begin and End\n");
	}


//---------------------------------------------------------------------------
// CCancelSuplProxyPrivacyRequestInfo:
//---------------------------------------------------------------------------


/** 
Static public constructor.

@return A new instance of this class.
*/  
CCancelSuplProxyPrivacyRequestInfo* CCancelSuplProxyPrivacyRequestInfo::NewL()
	{
	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::NewL() Begin\n");

	CCancelSuplProxyPrivacyRequestInfo* self = new (ELeave) CCancelSuplProxyPrivacyRequestInfo();

	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::NewL() End\n");
	return self;
	}


/** 
Destructor.
*/  
CCancelSuplProxyPrivacyRequestInfo::~CCancelSuplProxyPrivacyRequestInfo()
	{
	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::~CCancelSuplProxyPrivacyRequestInfo() Begin and End\n");
	}


/** 
Get the reason why the request was cancelled.

@return A TInt indicating why the request was cancelled.
*/  
TInt CCancelSuplProxyPrivacyRequestInfo::CancelReason() const
	{
	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::CancelReason() Begin and End\n");
	return iCancelReason;
	}


/** 
Sets the reason why the request was cancelled.

@param aCancelReason A TInt indicating why the request was cancelled.
*/  
void CCancelSuplProxyPrivacyRequestInfo::SetCancelReason(TInt aCancelReason)
	{
	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::SetCancelReason() Begin\n");

	iCancelReason = aCancelReason;

	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::SetCancelReason() End\n");

	}


/** 
Constructor.
*/  
CCancelSuplProxyPrivacyRequestInfo::CCancelSuplProxyPrivacyRequestInfo() :
	CSuplProxyRequestInfoBase(CSuplProxyRequestInfoBase::ESuplProxyPrivacyRequestTypeCancel)
	{
	LBSLOG(ELogP1, "CCancelSuplProxyPrivacyRequestInfo::CCancelSuplProxyPrivacyRequestInfo() Begin and End\n");
	}


//---------------------------------------------------------------------------
// CSuplProxyPrivacyRequestBuffer:
//---------------------------------------------------------------------------


/** 
Static public constructor.
*/  
CSuplProxyPrivacyRequestBuffer* CSuplProxyPrivacyRequestBuffer::NewL()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::NewL() Begin\n");

	CSuplProxyPrivacyRequestBuffer* self = new (ELeave) CSuplProxyPrivacyRequestBuffer();

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::NewL() End\n");
	return self;
	}


/** 
Destructor.
*/  

CSuplProxyPrivacyRequestBuffer::~CSuplProxyPrivacyRequestBuffer()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::~CSuplProxyPrivacyRequestBuffer() Begin\n");

	iBuffer.ResetAndDestroy();
	iBuffer.Close();

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::~CSuplProxyPrivacyRequestBuffer() End\n");
	}


/** 
Adds a request to the buffer.

This class takes ownership of the request, and adds it to the end of its queue.

@param aRequestInfo The request to be added.
*/  
void CSuplProxyPrivacyRequestBuffer::AddRequestL(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::AddRequestL() Begin\n");

	iBuffer.AppendL(aRequestInfo);

	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::AddRequestL() End\n");
	}

/** 
Callback function for use with RPointerArray<CSuplProxyPrivacyRequestInfo> to find a request by ID.

@param aRequestId Pointer to the request ID to find.
@param requestInfo Reference to a CSuplProxyPrivacyRequestInfo to check against.

@return ETrue if requestInfo has the required ID specified by aRequestId.
*/  
TBool FindRequestById(const TInt* aRequestId, const CSuplProxyPrivacyRequestInfo& requestInfo)
	{
	return (requestInfo.GetRequestId() == *aRequestId);
	}

/** 
Find a request matching the specified ID in the buffer.

@return The found request, or NULL if no suitable request can be found.
*/  
CSuplProxyPrivacyRequestInfo* CSuplProxyPrivacyRequestBuffer::FindRequest(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() Begin\n");

	TInt pos = iBuffer.Find(aRequestId,FindRequestById);
	
	if (pos >= 0)
		{
		CSuplProxyPrivacyRequestInfo* requestInfo = iBuffer[pos];
		LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() End (FOUND REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() End (NOT FOUND)\n");

	return NULL;
	}

/** 
Callback function for use with RPointerArray<CSuplProxyPrivacyRequestInfo> to find a request by Session.

@param aSession Pointer to the session to find.
@param requestInfo Reference to a CSuplProxyPrivacyRequestInfo to check against.

@return ETrue if requestInfo has the required session specified by aSession.
*/  
TBool FindRequestBySession(const CSession2* aSession, const CSuplProxyPrivacyRequestInfo& requestInfo)
	{
	return (requestInfo.Session() == aSession);
	}

/** 
Find a request matching the specified ID in the buffer.

@return The found request, or NULL if no suitable request can be found.
*/  
CSuplProxyPrivacyRequestInfo* CSuplProxyPrivacyRequestBuffer::FindRequest(const CSession2* aSession)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() Begin\n");

	TInt pos = iBuffer.Find(*aSession,FindRequestBySession);
	
	if (pos >= 0)
		{
		CSuplProxyPrivacyRequestInfo* requestInfo = iBuffer[pos];
		LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() End (FOUND REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::FindRequest() End (NOT FOUND)\n");
	return NULL;
	}

/** 
Remove a request from the buffer.

This class relinquishes ownership of the request.

@param aRequestInfo Pointer to the request to remove.

@return The request from the buffer, if found, otherwise NULL.
*/  
CSuplProxyPrivacyRequestInfo* CSuplProxyPrivacyRequestBuffer::RemoveRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::RemoveRequest() Begin\n");
	
	TInt pos = iBuffer.Find(aRequestInfo);
	if (pos >= 0)
		{
		CSuplProxyPrivacyRequestInfo* requestInfo = iBuffer[pos];
		iBuffer.Remove(pos);
		LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::RemoveRequest() End (RETURNING REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::RemoveRequest() End (NOT FOUND)\n");
	return NULL;
	}

/** 
Gets the number of requests present in the buffer.

@return The number of requests.
*/  
TInt CSuplProxyPrivacyRequestBuffer::NumRequests() const
	{
	return iBuffer.Count();
	}

/**
Search through the buffer for requests with a matching Id and cancel them.

The cancel action depends on what type of request it is:
Verify: Convert it into a notify request but leave it in the buffer. Complete
        the client request with the given cancel reason.
Notify: Remove it from the buffer.

The cancel request should not be added to the buffer.

@param aRequestId ID of the cancel request to match.
@param aReason Reason code for the cancel.
*/
void CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestById(TInt aRequestId, TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestById() Begin\n");

	for (TInt pos = 0; pos < iBuffer.Count(); pos++)
		{
		CSuplProxyPrivacyRequestInfo* requestInfo = iBuffer[pos]; 
		if (requestInfo->GetRequestId() == aRequestId)
			{
			TLbsNetPosRequestPrivacy privacy;
			requestInfo->GetRequestPrivacy(privacy);
			
			switch (privacy.RequestAdvice())
				{
				case TLbsNetPosRequestPrivacy::ERequestAdviceVerify:
					{
					// Change the privacy advice to 'Notify'
					privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
					privacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); 
					requestInfo->SetRequestPrivacy(privacy);
				                         
					// Complete the outstanding client request
					__ASSERT_DEBUG(requestInfo->IsResponseRequired(), User::Invariant());
					requestInfo->CompleteRequest(aReason);
					requestInfo->SetIsResponseRequired(EFalse);
					break;
					}
				case TLbsNetPosRequestPrivacy::ERequestAdviceNotify:
					{
					// Remove this request from the buffer
					iBuffer.Remove(pos);
					delete requestInfo;
					
					// Need to decrement pos because we've just removed an entry from the buffer
					pos--;
					break;
					}
				default:
					{
					break;
					}
				}
			}
		}
	
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestById() End\n");
	}

/**
Cancels any outstanding asynchronous requests from a client session.

This function should only be used when a client session has disconnected,
in order to clean up any outstanding asynchronous requests.

@param aSession Session to clean up requests for.
*/
void CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession(const CSession2* aSession)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession() Begin\n");

	for (TInt pos = 0; pos < iBuffer.Count(); pos++)
		{
		CSuplProxyPrivacyRequestInfo* requestInfo = iBuffer[pos];
		if (requestInfo->IsResponseRequired()
			&& requestInfo->Session() == aSession)
			{
			// Outstanding asynchronous request for this client.
			// It should always be a privacy verify request.
			
			// Change the verify into a notify.
			TLbsNetPosRequestPrivacy privacy;
			requestInfo->GetRequestPrivacy(privacy);
			privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
			privacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); 
			requestInfo->SetRequestPrivacy(privacy);
			
			// Clean up the outstanding client request
			requestInfo->CompleteRequest(KErrCancel);
			requestInfo->SetIsResponseRequired(EFalse);
			}
		}
	
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession() End\n");
	}

/** 
Constructor.
*/  
CSuplProxyPrivacyRequestBuffer::CSuplProxyPrivacyRequestBuffer()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacyRequestBuffer::CSuplProxyPrivacyRequestBuffer() Begin and End\n");
	}


