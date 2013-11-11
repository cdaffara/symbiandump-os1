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

#ifndef __SUSUTILSESSION_H__
#define __SUSUTILSESSION_H__

#include <e32base.h>

class CSusUtilServer;

/**
 CSusUtilSession maintains session with the client
 @internalComponent
 @released
 */
class CSusUtilSession : public CSession2
	{
public:
	static CSusUtilSession* NewL();
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);
	~CSusUtilSession();

private:
	CSusUtilSession();
	CSusUtilServer* UtilServer();
	};

#endif
