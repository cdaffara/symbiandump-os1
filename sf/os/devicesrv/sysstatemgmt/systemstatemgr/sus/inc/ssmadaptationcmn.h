// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMADAPTATIONCMN_H__
#define __SSMADAPTATIONCMN_H__

#include <e32cmn.h>
#include "susadaptionclisrv.h"

class CAdaptationMessage : public CBase
	{
public:
	CAdaptationMessage(const RMessage2 &aMessage);
	void Complete(TInt aReason);
	CSession2 *Session() const;
	void WriteL(TInt aParam, const TDesC8 &aDes);
	TInt Function() const;
	TInt Int0() const;
	void ReadL(TInt aParamNumber,TPtr8 aParam);
	TInt GetDesLength(TInt aParam);
	void SetMessage(const RMessage2& aMessage);
private:
	RMessage2 iMessage;
public :
	TSusAdaptionServerRequests iRequestType;	
	};

class RSsmAdaptationRequestQueue
	{
public:
	void Close();
	TBool IsEmpty();
	void Dequeue(CAdaptationMessage *&aCurrentMessage);
	void RemoveFromQueueAndComplete(const RMessage2 &aMessage);
	TInt Queue(CAdaptationMessage *aPendingRequest);
	void NotifyAndRemoveAll();
	TInt Count();	
private:
	RPointerArray<CAdaptationMessage > iQueue;
		
	};


#endif	__SSMADAPTATIONCMN_H__
