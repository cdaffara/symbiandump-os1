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

#ifndef SSMSWPPOLICYSESS_H
#define SSMSWPPOLICYSESS_H

#include <e32base.h>

class CSsmSwpPolicyServer;

/**
 The SsmSwpPolicy server session.
 
 @internalComponent
 @released
 */ 
NONSHARABLE_CLASS(CSsmSwpPolicySession) : public CSession2
	{
public:
	static CSsmSwpPolicySession* NewL(CSsmSwpPolicyServer& aSsmPolicyServer);
	~CSsmSwpPolicySession();
	// From CSession2
	void ServiceL(const RMessage2& aMessage);
	
private:
	CSsmSwpPolicySession(CSsmSwpPolicyServer& aSsmPolicyServer);
	void ConstructL();
	
private:
	CSsmSwpPolicyServer& iSsmSwpPolicySvr;
	TInt iSessionIndex;
	};

#endif //SSMSWPPOLICYSESS_H
