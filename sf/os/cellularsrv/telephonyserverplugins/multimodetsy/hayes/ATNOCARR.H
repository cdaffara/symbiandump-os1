// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef __ATNOCARR_H__
#define __ATNOCARR_H__

#include "ATBASE.H"

class CATWaitForNoCarrier : public CATBase
/**
@internalComponent
*/
	{
public:
	static CATWaitForNoCarrier* NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	~CATWaitForNoCarrier();
	void StartWait();
	void StopWait();
protected:
	CATWaitForNoCarrier(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void EventSignal(TEventSource aSource);
private:
	CCommChatString* iNoCarrierExpectString;
	CCommChatString* iCallMonitoringEventString;
	};

#endif
