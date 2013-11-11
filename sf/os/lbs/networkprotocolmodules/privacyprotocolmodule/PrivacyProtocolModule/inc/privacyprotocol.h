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

#ifndef __PRIVACYPROTOCOL_H__
#define __PRIVACYPROTOCOL_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

class CCancelPrivacyRequestInfo;
class TLbsNetSessionId;
class CPrivacyRequestInfo;


/** 
Network privacy protocol mixin definition.
*/
class MPrivacyProtocol
	{
public:

	virtual void CancelVerifyLocationRequest(CCancelPrivacyRequestInfo* aCancelInfo) =0;
	virtual void GetNextSessionId(TLbsNetSessionId& aSessionId) =0;
	virtual void NotifyLocationRequest(CPrivacyRequestInfo* aRequestInfo) =0;
	virtual void NotifyVerificationTimeout(CPrivacyRequestInfo* aRequestInfo) =0;
	virtual void VerifyLocationRequest(CPrivacyRequestInfo* aRequestInfo) =0;
	virtual void PrivacyLocationRequest(CPrivacyRequestInfo* aRequestInfo) =0;
	virtual void CompletePrivacyRequest(TUint32 aRequestId, TInt aReason) =0;
	virtual void SessionDisconnected(const CSession2* aSession) =0;

	};

#endif // __PRIVACYPROTOCOL_H__