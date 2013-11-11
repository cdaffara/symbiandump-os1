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

#ifndef CMOCKSYSERVER_H
#define CMOCKSYSERVER_H

#include <e32base.h>

class CMockSYEngine;

class CMockSYServer : public CServer2
	{
public:
	IMPORT_C static CServer2* NewL(const TDesC& aName, TInt aPriority, CMockSYEngine& aEngine);

private:
	CMockSYServer(TInt aPriority, CMockSYEngine& aEngine);
	void ConstructL(const TDesC& aName);

private:
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

private:
	CMockSYEngine& iEngine;
	};
	
#endif
