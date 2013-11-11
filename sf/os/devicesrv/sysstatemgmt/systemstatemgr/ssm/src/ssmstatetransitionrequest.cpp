// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmstatetransitionrequest.h"
#include "ssmserverpanic.h"

CSsmStateTransitionRequest::CSsmStateTransitionRequest(const TSsmStateTransition& aStateTransition)
	: iStateTransition(aStateTransition)
	{
	}

CSsmStateTransitionRequest::CSsmStateTransitionRequest(const TSsmStateTransition& aStateTransition, const RMessage2& aMessage)
	: iStateTransition(aStateTransition), iMessage(aMessage)
	{
	}
CSsmStateTransitionRequest::~CSsmStateTransitionRequest()
	{
	__ASSERT_DEBUG(KNullHandle == iMessage.Handle(), PanicNow(KPanicSysStateMgr, ESsmTransitionRequestError1));
	}

const TSsmStateTransition& CSsmStateTransitionRequest::StateTransition() const
	{
	return iStateTransition;
	}

void CSsmStateTransitionRequest::SetStateTransition(const TSsmStateTransition& aStateTransition)
	{
	iStateTransition = aStateTransition;
	}

void CSsmStateTransitionRequest::Complete(TInt aCompletionCode)
	{
	// Guard against multiple calls to complete
	if( KNullHandle != iMessage.Handle() )
		{
		iMessage.Complete(aCompletionCode);
		}
	}
TSsmState CSsmStateTransitionRequest::State() const
	{
	return iStateTransition.State();
	}

TInt CSsmStateTransitionRequest::Reason() const
	{
	return iStateTransition.Reason();
	}

TBool CSsmStateTransitionRequest::OriginatesFrom(CSession2* aSession) const
	{
	return (iMessage.Session() == aSession);
	}
