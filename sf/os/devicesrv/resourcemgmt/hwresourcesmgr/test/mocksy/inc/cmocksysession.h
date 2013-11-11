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

#ifndef CMOCKSYSESSION_H
#define CMOCKSYSESSION_H

#include <e32base.h>

#include "cmocksyengine.h"

enum TMockSYPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage,TMockSYPanic aPanic);

class CMockSYSession : public CSession2, public MMockSYEngineListener
	{
public:
	static CSession2* NewL(CMockSYEngine& aEngine);
	~CMockSYSession();

public: // from MockSYEngineListener
	virtual void Notify(TNotificationType aNotification);
		
protected:
	CMockSYSession(CMockSYEngine& aEngine);
	void ConstructL();

public:
	void ServiceL(const RMessage2& aMessage);
	
private:
	RMessagePtr2 iNotifyDoneMsg;
	CMockSYEngine& iEngine;
	};
	
#endif
