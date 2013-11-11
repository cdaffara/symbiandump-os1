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

#ifndef __SSMSESSION_H__
#define __SSMSESSION_H__

#include <e32base.h>

class CSsmServer;

/**
 CSsmSession maintains session with the client
 @internalComponent
 @released
 */
class CSsmSession : public CSession2
	{
public:
	static CSsmSession* NewL();
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);
	~CSsmSession();

private:
	CSsmSession();
	CSsmServer* SsmServer() const;
	};

#endif
