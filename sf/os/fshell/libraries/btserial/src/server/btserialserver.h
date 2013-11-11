// btserialserver.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __BTSERIALSERVER_H__
#define __BTSERIALSERVER_H__

#include <e32base.h>
#include "btserialclientserver.h"

class CBtSerialSession;

class CBtSerialServer : public CServer2
	{
public:
	static CServer2* NewLC();
	void DropSession();
private:
	CBtSerialServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	CBtSerialSession* iSession;
	};

#endif //__BTSERIALSERVER_H__
