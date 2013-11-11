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
*/

#include "ss_mobility_apiext.h"
#include <comms-infras/mobilitymessagesecom.h>
#include <elements/responsemsg.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/ss_mobility_apiext_internal.h>
#endif

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksmblty, "ESockSSocksmblty");
#endif

using namespace ESock;
using namespace Elements;

CCommsMobilitySrvResp::~CCommsMobilitySrvResp()
	{
	}

CCommsMobilitySrvResp::CCommsMobilitySrvResp()
:	CCommsApiExtResponder()
	{
	}

EXPORT_C void CCommsMobilitySrvResp::PreferredCarrierAvailable(CCommsMobilitySrvResp*& aThis, TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless)
	{
	if (aThis == NULL)
	    return;

	aThis->DoPreferredCarrierAvailable(aOldAP, aNewAP, aIsUpgrade, aIsSeamless);

	delete aThis;
	aThis = NULL;
	}

EXPORT_C void CCommsMobilitySrvResp::NewCarrierActive(CCommsMobilitySrvResp*& aThis, TAccessPointInfo aNewAP, TBool aIsSeamless)
	{
	if (aThis == NULL)
	    return;

	aThis->DoNewCarrierActive(aNewAP, aIsSeamless);

	delete aThis;
	aThis = NULL;
	}

EXPORT_C void CCommsMobilitySrvResp::Error(CCommsMobilitySrvResp*& aThis, TInt aError)
	{
	CCommsApiExtResponder* responder = aThis;
    CCommsApiExtResponder::Complete(responder, aError);
	aThis = NULL;
	}

EXPORT_C CCommsMobilitySrvIpcResponder* CCommsMobilitySrvIpcResponder::NewL(Elements::RResponseMsg& aResponseMsg)
	{
	return new (ELeave) CCommsMobilitySrvIpcResponder(aResponseMsg);
	}

CCommsMobilitySrvIpcResponder::CCommsMobilitySrvIpcResponder(Elements::RResponseMsg& aResponseMsg)
:	CCommsMobilitySrvResp(),
	iResponseMsg(aResponseMsg)
	{
	}

CCommsMobilitySrvIpcResponder::~CCommsMobilitySrvIpcResponder()
	{
	delete iMsg;
	}

EXPORT_C void CCommsMobilitySrvIpcResponder::DoPreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless)
	{
	__ASSERT_DEBUG(iMsg == NULL, User::Panic(KSpecAssert_ESockSSocksmblty, 1));
	TRAP_IGNORE(iMsg = CPreferredCarrierAvailable::NewL()); //we could make it in place, no TRAP

    if (iMsg == NULL)
    	{
    	iResponseMsg.Complete(KErrNoMemory);
    	return;
    	}

	CPreferredCarrierAvailable& msg = reinterpret_cast<CPreferredCarrierAvailable&>(*iMsg);
	
	msg.SetOldAPInfo(aOldAP);
	msg.SetNewAPInfo(aNewAP);
	msg.SetIsUpgrade(aIsUpgrade);
	msg.SetIsSeamless(aIsSeamless);

	iResponseMsg.Complete(msg);
	}

EXPORT_C void CCommsMobilitySrvIpcResponder::DoNewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless)
	{
	__ASSERT_DEBUG(iMsg == NULL, User::Panic(KSpecAssert_ESockSSocksmblty, 2));
	TRAP_IGNORE(iMsg = CNewCarrierActive::NewL()); //we could make it in place, no TRAP

    if (iMsg == NULL)
    	{
    	iResponseMsg.Complete(KErrNoMemory);
    	return;
    	}

	CNewCarrierActive& msg = reinterpret_cast<CNewCarrierActive&>(*iMsg);

	msg.SetNewAPInfo(aNewAP);
	msg.SetIsSeamless(aIsSeamless);

	iResponseMsg.Complete(msg);
	}

void CCommsMobilitySrvIpcResponder::DoComplete(TInt aError)
	{
	iResponseMsg.Complete(aError);
	}

