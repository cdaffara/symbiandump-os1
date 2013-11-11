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
#include "privacyrequestinfo.h"


//---------------------------------------------------------------------------
// CRequestInfoBase:
//---------------------------------------------------------------------------


/** 
Get the request ID.

@return The request ID associated with this request.
*/  
TInt CRequestInfoBase::GetRequestId() const
	{
	LBSLOG(ELogP1, "CRequestInfoBase::GetRequestId()\n");
	return iRequestId;
	}


/** 
Set the request ID.

@param aRequestId The request ID to be associated with this request.
*/  
void CRequestInfoBase::SetRequestId(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CRequestInfoBase::SetRequestId()\n");
	iRequestId = aRequestId;
	}


/** 
Get the request type.

@return The type of request.
*/  
CRequestInfoBase::TPrivacyRequestType CRequestInfoBase::Type() const
	{
	LBSLOG(ELogP1, "CRequestInfoBase::Type()\n");
	return iRequestType;
	}


/** 
Constructor.

@param aRequestType The type of request.
*/  
CRequestInfoBase::CRequestInfoBase(CRequestInfoBase::TPrivacyRequestType aRequestType) :
	iRequestType(aRequestType)
	{
	LBSLOG(ELogP1, "CRequestInfoBase::CRequestInfoBase()\n");
	}


//---------------------------------------------------------------------------
// CPrivacyRequestInfo:
//---------------------------------------------------------------------------


/** 
Static public constructor.

@return A new instance of this class.
*/  
CPrivacyRequestInfo* CPrivacyRequestInfo::NewL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::NewL() Begin\n");
	CPrivacyRequestInfo* self = new (ELeave) CPrivacyRequestInfo(aMessage);
	LBSLOG(ELogP1, "CPrivacyRequestInfo::NewL() End\n");
	return self;
	}


/** 
Destructor.
*/  
CPrivacyRequestInfo::~CPrivacyRequestInfo()
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::~CPrivacyRequestInfo()\n");
	}
		

/** 
Complete this request.

@param aStatus Status to complete with.
*/  
void CPrivacyRequestInfo::CompleteRequest(TInt aStatus)
	{
    LBSLOG(ELogP7, "->A CPrivacyRequestInfo::CompleteRequest(aStatus)\n");
    LBSLOG2(ELogP7, "\t > TInt aStatus = %d\n", aStatus);
	iResponseMessage.Complete(aStatus);
	}
		

/** 
Get the TLbsExternalRequestInfo2 associated with this request.

@param aRequestInfo Reference to the TLbsExternalRequestInfo2 to populate.
*/  
void CPrivacyRequestInfo::GetRequestInfo(TLbsExternalRequestInfo2& aRequestInfo) const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::GetRequestInfo() Begin\n");
	aRequestInfo = iRequestInfo;
	LBSLOG(ELogP1, "CPrivacyRequestInfo::GetRequestInfo() End\n");
	}


/** 
Set the TLbsExternalRequestInfo2 associated with this request.

@param aRequestInfo Reference to the TLbsExternalRequestInfo2 to set.
*/  
void CPrivacyRequestInfo::SetRequestInfo(const TLbsExternalRequestInfo2& aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetRequestInfo() Begin\n");
	iRequestInfo = aRequestInfo;
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetRequestInfo() End\n");
	}


/** 
Set the TLbsNetPosRequestPrivacy associated with this request.

@param aRequestPrivacy Reference to the TLbsNetPosRequestPrivacy to populate.
*/  
void CPrivacyRequestInfo::GetRequestPrivacy(TLbsNetPosRequestPrivacy& aRequestPrivacy) const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::GetRequestPrivacy() Begin\n");
	aRequestPrivacy = iRequestPrivacy;
	LBSLOG(ELogP1, "CPrivacyRequestInfo::GetRequestPrivacy() End\n");
	}


/** 
Set the TLbsNetPosRequestPrivacy associated with this request.

@param aRequestPrivacy Reference to the TLbsNetPosRequestPrivacy to set.
*/  
void CPrivacyRequestInfo::SetRequestPrivacy(const TLbsNetPosRequestPrivacy& aRequestPrivacy)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetRequestPrivacy() Begin\n");
	iRequestPrivacy = aRequestPrivacy;
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetRequestPrivacy() End\n");
	}


/** 
Determine if this request requires a response.

@return ETrue if a response if required, EFalse otherwise.
*/  
TBool CPrivacyRequestInfo::IsResponseRequired() const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::IsResponseRequired()\n");
	return iResponseRequired;
	}


/** 
Set if this request requires a response.

@param aResponseRequired ETrue if a response is required, EFalse otherwise.
*/  
void CPrivacyRequestInfo::SetIsResponseRequired(TBool aResponseRequired)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetIsResponseRequired()\n");
	iResponseRequired = aResponseRequired;
	}

/** 
Determine if the session should be completed by the Privacy Protocol Module, or by the client

@return ETrue If the Privacy Protocol Module should complete the session, EFalse otherwise
*/
TBool CPrivacyRequestInfo::IsSessionCompleteAutomatic() const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::IsSessionCompleteAutomatic()\n");
	return iSessionCompleteAutomatic;	
	}

/** 
Set if the session should be completed by the Privacy Protocol Module, or by the client

@param aAutomatic If it's set to ETrue then the  Privacy Protocol Module should complete 
					the session, otherwise the client will
*/
void CPrivacyRequestInfo::SetIsSessionCompleteAutomatic(TBool aAutomatic)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetIsSessionCompleteAutomatic()\n");
	iSessionCompleteAutomatic = aAutomatic;	
	}

/** 
Determine if the response from LBS should be converted to an error code 
before sending to client or not.

@return ETrue If the Privacy Protocol Module should convert the response, EFalse otherwise
*/
TBool CPrivacyRequestInfo::ConvertResponseCode() const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::ConvertResponseCode()\n");
	return iConvertResponseCode;	
	}

/** 
Set if the response from LBS should be converted to an error code by the 
Privacy Protocol Module, or not.

@param aConvert If it's set to ETrue then the Privacy Protocol Module will convert 
					the return code, otherwise it won't
*/
void CPrivacyRequestInfo::SetConvertResponseCode(TBool aConvert)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::SetConvertResponseCode()\n");
	iConvertResponseCode = aConvert;	
	}

/**
Get the session associated with this request.

@return A pointer to the session.
*/
const CSession2* CPrivacyRequestInfo::Session() const
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::Session()\n");
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
CPrivacyRequestInfo::CPrivacyRequestInfo(const RMessage2& aMessage) :
	CRequestInfoBase(CRequestInfoBase::EPrivacyRequestTypeRequest),
	iResponseMessage(aMessage), iSessionCompleteAutomatic(ETrue), 
	iConvertResponseCode(ETrue)
	{
	LBSLOG(ELogP1, "CPrivacyRequestInfo::CPrivacyRequestInfo()\n");
	}


//---------------------------------------------------------------------------
// CCancelPrivacyRequestInfo:
//---------------------------------------------------------------------------


/** 
Static public constructor.

@return A new instance of this class.
*/  
CCancelPrivacyRequestInfo* CCancelPrivacyRequestInfo::NewL()
	{
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::NewL() Begin\n");
	CCancelPrivacyRequestInfo* self = new (ELeave) CCancelPrivacyRequestInfo();
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::NewL() End\n");
	return self;
	}


/** 
Destructor.
*/  
CCancelPrivacyRequestInfo::~CCancelPrivacyRequestInfo()
	{
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::~CCancelPrivacyRequestInfo()\n");
	}


/** 
Get the reason why the request was cancelled.

@return A TInt indicating why the request was cancelled.
*/  
TInt CCancelPrivacyRequestInfo::CancelReason() const
	{
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::CancelReason()\n");
	return iCancelReason;
	}


/** 
Sets the reason why the request was cancelled.

@param aCancelReason A TInt indicating why the request was cancelled.
*/  
void CCancelPrivacyRequestInfo::SetCancelReason(TInt aCancelReason)
	{
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::SetCancelReason()\n");
	iCancelReason = aCancelReason;
	}


/** 
Constructor.
*/  
CCancelPrivacyRequestInfo::CCancelPrivacyRequestInfo() :
	CRequestInfoBase(CRequestInfoBase::EPrivacyRequestTypeCancel)
	{
	LBSLOG(ELogP1, "CCancelPrivacyRequestInfo::CCancelPrivacyRequestInfo()\n");
	}


//---------------------------------------------------------------------------
// CPrivacyRequestBuffer:
//---------------------------------------------------------------------------


/** 
Static public constructor.
*/  
CPrivacyRequestBuffer* CPrivacyRequestBuffer::NewL()
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::NewL() Begin\n");
	CPrivacyRequestBuffer* self = new (ELeave) CPrivacyRequestBuffer();
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::NewL() End\n");
	return self;
	}


/** 
Destructor.
*/  

CPrivacyRequestBuffer::~CPrivacyRequestBuffer()
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::~CPrivacyRequestBuffer() Begin\n");
	iBuffer.ResetAndDestroy();
	iBuffer.Close();
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::~CPrivacyRequestBuffer() End\n");
	}


/** 
Adds a request to the buffer.

This class takes ownership of the request, and adds it to the end of its queue.

@param aRequestInfo The request to be added.
*/  
void CPrivacyRequestBuffer::AddRequestL(CPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::AddRequestL() Begin\n");
	iBuffer.AppendL(aRequestInfo);
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::AddRequestL() End\n");
	}

/** 
Callback function for use with RPointerArray<CPrivacyRequestInfo> to find a request by ID.

@param aRequestId Pointer to the request ID to find.
@param requestInfo Reference to a CPrivacyRequestInfo to check against.

@return ETrue if requestInfo has the required ID specified by aRequestId.
*/  
TBool FindRequestById(const TInt* aRequestId, const CPrivacyRequestInfo& requestInfo)
	{
	return (requestInfo.GetRequestId() == *aRequestId);
	}

/** 
Find a request matching the specified ID in the buffer.

@return The found request, or NULL if no suitable request can be found.
*/  
CPrivacyRequestInfo* CPrivacyRequestBuffer::FindRequest(TInt aRequestId)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() Begin\n");
	TInt pos = iBuffer.Find(aRequestId,FindRequestById);
	
	if (pos >= 0)
		{
		CPrivacyRequestInfo* requestInfo = iBuffer[pos];
		LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() End (FOUND REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() End (NOT FOUND)\n");
	return NULL;
	}

/** 
Callback function for use with RPointerArray<CPrivacyRequestInfo> to find a request by Session.

@param aSession Pointer to the session to find.
@param requestInfo Reference to a CPrivacyRequestInfo to check against.

@return ETrue if requestInfo has the required session specified by aSession.
*/  
TBool FindRequestBySession(const CSession2* aSession, const CPrivacyRequestInfo& requestInfo)
	{
	return (requestInfo.Session() == aSession);
	}

/** 
Find a request matching the specified ID in the buffer.

@return The found request, or NULL if no suitable request can be found.
*/  
CPrivacyRequestInfo* CPrivacyRequestBuffer::FindRequest(const CSession2* aSession)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() Begin\n");
	TInt pos = iBuffer.Find(*aSession,FindRequestBySession);
	
	if (pos >= 0)
		{
		CPrivacyRequestInfo* requestInfo = iBuffer[pos];
		LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() End (FOUND REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::FindRequest() End (NOT FOUND)\n");
	return NULL;
	}

/** 
Remove a request from the buffer.

This class relinquishes ownership of the request.

@param aRequestInfo Pointer to the request to remove.

@return The request from the buffer, if found, otherwise NULL.
*/  
CPrivacyRequestInfo* CPrivacyRequestBuffer::RemoveRequest(CPrivacyRequestInfo* aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::RemoveRequest() Begin\n");
	
	TInt pos = iBuffer.Find(aRequestInfo);
	if (pos >= 0)
		{
		CPrivacyRequestInfo* requestInfo = iBuffer[pos];
		iBuffer.Remove(pos);
		LBSLOG(ELogP1, "CPrivacyRequestBuffer::RemoveRequest() End (RETURNING REQUEST)\n");
		return requestInfo;
		}
	
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::RemoveRequest() End (NOT FOUND)\n");
	return NULL;
	}

/** 
Gets the number of requests present in the buffer.

@return The number of requests.
*/  
TInt CPrivacyRequestBuffer::NumRequests() const
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
void CPrivacyRequestBuffer::CancelAnyOutstandingRequestById(TInt aRequestId, TInt aReason)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::CancelAnyOutstandingRequestById() Begin\n");

	for (TInt pos = 0; pos < iBuffer.Count(); pos++)
		{
		CPrivacyRequestInfo* requestInfo = iBuffer[pos]; 
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
	
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::CancelAnyOutstandingRequestById() End\n");
	}

/**
Cancels any outstanding asynchronous requests from a client session.

This function should only be used when a client session has disconnected,
in order to clean up any outstanding asynchronous requests.

@param aSession Session to clean up requests for.
*/
void CPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession(const CSession2* aSession)
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession() Begin\n");

	for (TInt pos = 0; pos < iBuffer.Count(); pos++)
		{
		CPrivacyRequestInfo* requestInfo = iBuffer[pos];
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
	
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::CancelAnyOutstandingRequestBySession() End\n");
	}

/** 
Constructor.
*/  
CPrivacyRequestBuffer::CPrivacyRequestBuffer()
	{
	LBSLOG(ELogP1, "CPrivacyRequestBuffer::CPrivacyRequestBuffer()\n");
	}


