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

#ifndef __SUPLPROXYPRIVACYREQUESTINFO_H__
#define __SUPLPROXYPRIVACYREQUESTINFO_H__

#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacyrequestcommon.h>


/** 
CSuplProxyRequestInfoBase

Base class to represent requests that can be queued and handled by this protocol module
*/
class CSuplProxyRequestInfoBase : public CBase
	{
	public:

		TInt GetRequestId() const;
		void SetRequestId(TInt aRequestId);

		enum TSuplProxyPrivacyRequestType
		{
			ESuplProxyPrivacyRequestTypeUnknown = 0,
			ESuplProxyPrivacyRequestTypeRequest,
			ESuplProxyPrivacyRequestTypeCancel
		};
		
		TSuplProxyPrivacyRequestType Type() const;
		
	protected:

		CSuplProxyRequestInfoBase(TSuplProxyPrivacyRequestType aRequestType);
	
	private:

		CSuplProxyRequestInfoBase();

		TInt iRequestId;

		TSuplProxyPrivacyRequestType iRequestType;
		
	};


/** 
CSuplProxyPrivacyRequestInfo

Represents a privacy request
*/
class CSuplProxyPrivacyRequestInfo : public CSuplProxyRequestInfoBase
	{
	public:

		static CSuplProxyPrivacyRequestInfo* NewL(const RMessage2& aMessage);
		~CSuplProxyPrivacyRequestInfo();
		
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

		CSuplProxyPrivacyRequestInfo(const RMessage2& aMessage);

		RMessage2 iResponseMessage;
		TLbsExternalRequestInfo2 iRequestInfo;
		TLbsNetPosRequestPrivacy iRequestPrivacy;
		TBool iResponseRequired;
		TBool iSessionCompleteAutomatic;
		TBool iConvertResponseCode;
		
	};
	
	
/** 
CCancelSuplProxyRequestInfo

Represents cancellation of a supl proxy privacy request
*/
class CCancelSuplProxyPrivacyRequestInfo : public CSuplProxyRequestInfoBase
	{
	public:

		static CCancelSuplProxyPrivacyRequestInfo* NewL();
		~CCancelSuplProxyPrivacyRequestInfo();
		
		TInt CancelReason() const;
		void SetCancelReason(TInt aCancelReason);
		
	private:

		CCancelSuplProxyPrivacyRequestInfo();
		TInt iCancelReason;
		
	};


	
/** 
CSuplProxyRequestBuffer

A buffer (or queue) of requests to be handled by this supl proxy protocol module
*/
class CSuplProxyPrivacyRequestBuffer : public CBase
	{
	public:
	
		static CSuplProxyPrivacyRequestBuffer* NewL();
		~CSuplProxyPrivacyRequestBuffer();
		
		void AddRequestL(CSuplProxyPrivacyRequestInfo* aRequestInfo);
		CSuplProxyPrivacyRequestInfo* FindRequest(TInt aRequestId);
		CSuplProxyPrivacyRequestInfo* FindRequest(const CSession2* aSession);
		CSuplProxyPrivacyRequestInfo* RemoveRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo);

		TInt NumRequests() const;
		
		void CancelAnyOutstandingRequestById(TInt aRequestId, TInt aReason);
		void CancelAnyOutstandingRequestBySession(const CSession2* aSession);
		
	private:
	
	    CSuplProxyPrivacyRequestBuffer();
		
		RPointerArray<CSuplProxyPrivacyRequestInfo> iBuffer;
	
	};
	

#endif // __SUPLPROXYPRIVACYREQUESTINFO_H__
