// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef HCISESSION_H
#define HCISESSION_H

#include <e32base.h>

class CHCIServer;
class CHCIServerRequestManager;

NONSHARABLE_CLASS(CHCISession) : public CSession2
	{
public:
	static CHCISession* NewL(CHCIServer& aServer);
	~CHCISession();

private:
	CHCISession(CHCIServer& aServer);
	void ConstructL();
		
private:
	// CSession2 virtuals
	virtual void ServiceL(const RMessage2& aMessage);

private:
	CHCIServer& iServer; // unowned
	CHCIServerRequestManager* iService; // unowned
	};

#endif // HCISESSION_H
