// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Wait for 1st incoming call

#ifndef ATWAITFORCALLHANDLER_H_
#define ATWAITFORCALLHANDLER_H_

//system include

//user include
#include "atcommandcontrolbase.h"


//class forward
class CATRing;
class CATClcc;

class CATWaitForCallHandler : public CATCommandControlBase
	{
public:
	static CATWaitForCallHandler* NewL(CGlobalPhonemanager& aGloblePhone,
			                           CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	static CATWaitForCallHandler* NewLC(CGlobalPhonemanager& aGloblePhone,
			                            CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	virtual ~CATWaitForCallHandler();
	
	void StartToWaitForCall();
	
	//From MAtCommandExecuteObserver
	void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus);
	
protected:
	
	CATWaitForCallHandler(CGlobalPhonemanager& aGloblePhone,
			              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	void ConstructL();
	
	TInt ProcessIncomingCallInfomation();	

private:
	
	CATRing* iAtRing;
	
	CATClcc* iAtClcc;

	};

#endif /*ATWAITFORCALLHANDLER_H_*/
