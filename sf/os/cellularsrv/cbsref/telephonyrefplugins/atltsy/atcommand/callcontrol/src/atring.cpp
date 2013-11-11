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
// Wait for first one incoming call

//system include

//user include
#include "atring.h"
#include "mslogger.h"

//const define
_LIT8(KLtsyIncomingExtCallIndication,"+CRING:*");
_LIT8(KLtsyIncomingCallIndication,"RING");

CATRing* CATRing::NewL(CGlobalPhonemanager& aGloblePhone,
			           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATRing::NewL()"));
	
	CATRing* self = CATRing::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


CATRing* CATRing::NewLC(CGlobalPhonemanager& aGloblePhone,
			            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATRing::NewLC()"));
	
	CATRing* self = new (ELeave) CATRing(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CATRing::~CATRing()
	{
	}

CATRing::CATRing(CGlobalPhonemanager& aGloblePhone,
		         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                 :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATRing::CATRing()"));
	
	iAtType = ELtsyAT_Call_RING;
	}

void CATRing::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATRing::ConstructL()"));
	
	CAtCommandBase::ConstructL(); //Not must
	RemoveAllExpectString();
	AddExpectStringL(KLtsyIncomingCallIndication);
	AddExpectStringL(KLtsyIncomingExtCallIndication);
	}

//End of file
