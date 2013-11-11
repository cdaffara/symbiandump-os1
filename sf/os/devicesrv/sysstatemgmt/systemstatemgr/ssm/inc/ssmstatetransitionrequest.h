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

#ifndef __SSMSTATETRANSITIONREQUEST_H_
#define __SSMSTATETRANSITIONREQUEST_H_

#include <e32base.h>
#include <ssm/ssmstatetransition.h>

/**
Encapsulates a TSsmStateTransition object and the requesting client RMessage2
@internalComponent
@released
*/		
class CSsmStateTransitionRequest : public CBase
	{
public:
	CSsmStateTransitionRequest(const TSsmStateTransition& aStateTransition);
	CSsmStateTransitionRequest(const TSsmStateTransition& aStateTransition, const RMessage2& aMessage);
	~CSsmStateTransitionRequest();
	void Complete(TInt aCompletionCode);
	const TSsmStateTransition& StateTransition() const;
	void SetStateTransition(const TSsmStateTransition& aStateTransition);
	TSsmState State() const;
	TInt Reason() const;
	TBool OriginatesFrom(CSession2* aSession) const;
	
private:
	TSsmStateTransition iStateTransition;
	RMessage2 iMessage;
	};

#endif
