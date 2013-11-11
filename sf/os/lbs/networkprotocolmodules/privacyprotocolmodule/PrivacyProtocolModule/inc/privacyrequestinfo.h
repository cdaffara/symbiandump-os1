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

#ifndef __PRIVACYREQUESTINFO_H__
#define __PRIVACYREQUESTINFO_H__

#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacyrequestcommon.h>


/** 
CRequestInfoBase

Base class to represent requests that can be queued and handled by this protocol module
*/
class CRequestInfoBase : public CBase
	{
	public:

		TInt GetRequestId() const;
		void SetRequestId(TInt aRequestId);

		enum TPrivacyRequestType
		{
			EPrivacyRequestTypeUnknown = 0,
			EPrivacyRequestTypeRequest,
			EPrivacyRequestTypeCancel
		};
		
		TPrivacyRequestType Type() const;
		
	protected:

		CRequestInfoBase(TPrivacyRequestType aRequestType);
	
	private:

		CRequestInfoBase();

		TInt iRequestId;

		TPrivacyRequestType iRequestType;
		
	};


/** 
CPrivacyRequestInfo

Represents a privacy request
*/
class CPrivacyRequestInfo : public CRequestInfoBase
	{
	public:

		static CPrivacyRequestInfo* NewL(const RMessage2& aMessage);
		~CPrivacyRequestInfo();
		
		void CompleteRequest(TInt aStatus);
		
		void GetRequestInfo(TLbsExternalRequestInfo2& aRequestInfo) const;
		void SetRequestInfo(const TLbsExternalRequestInfo2& aRequestInfo);
		
		void GetRequestPrivacy(TLbsNetPosRequestPrivacy& aRequestPrivacy) const;
		void SetRequestPrivacy(const TLbsNetPosRequestPrivacy& aRequestPrivacy);
		
		TBool IsResponseRequired() const;
		void SetIsResponseRequired(TBool aResponseRequired);
		
		TBool IsSessionCompleteAutomatic() const;
		void SetIsSessionCompleteAutomatic(TBool aAutomatic);
		
		TBool ConvertResponseCode() const;
		void SetConvertResponseCode(TBool aConvert);
		
		const CSession2* Session() const;
		
	private:

		CPrivacyRequestInfo(const RMessage2& aMessage);

		RMessage2 iResponseMessage;
		TLbsExternalRequestInfo2 iRequestInfo;
		TLbsNetPosRequestPrivacy iRequestPrivacy;
		TBool iResponseRequired;
		TBool iSessionCompleteAutomatic;
		TBool iConvertResponseCode;
		
	};
	
	
/** 
CCancelPrivacyRequestInfo

Represents cancellation of a privacy request
*/
class CCancelPrivacyRequestInfo : public CRequestInfoBase
	{
	public:

		static CCancelPrivacyRequestInfo* NewL();
		~CCancelPrivacyRequestInfo();
		
		TInt CancelReason() const;
		void SetCancelReason(TInt aCancelReason);
		
	private:

		CCancelPrivacyRequestInfo();
		TInt iCancelReason;
		
	};


	
/** 
CPrivacyRequestBuffer

A buffer (or queue) of privacy requests to be handled by this protocol module
*/
class CPrivacyRequestBuffer : public CBase
	{
	public:
	
		static CPrivacyRequestBuffer* NewL();
		~CPrivacyRequestBuffer();
		
		void AddRequestL(CPrivacyRequestInfo* aRequestInfo);
		CPrivacyRequestInfo* FindRequest(TInt aRequestId);
		CPrivacyRequestInfo* FindRequest(const CSession2* aSession);
		CPrivacyRequestInfo* RemoveRequest(CPrivacyRequestInfo* aRequestInfo);

		TInt NumRequests() const;
		
		void CancelAnyOutstandingRequestById(TInt aRequestId, TInt aReason);
		void CancelAnyOutstandingRequestBySession(const CSession2* aSession);
		
	private:
	
		CPrivacyRequestBuffer();
		
		RPointerArray<CPrivacyRequestInfo> iBuffer;
	
	};
	

#endif // __PRIVACYREQUESTINFO_H__
