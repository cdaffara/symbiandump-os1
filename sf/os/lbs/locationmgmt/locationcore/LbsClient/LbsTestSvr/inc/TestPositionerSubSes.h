// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTPOSITIONERSUBSES_H__
#define __TESTPOSITIONERSUBSES_H__

#include <e32base.h>
#include <lbscommon.h>
#include <lbsipc.h>
#include "TestServer.h"
#include "TestSession.h"

class CEposPositioner : public CObject, public MRequestTimerUser
	{
public:
	static CEposPositioner* NewL();
	~CEposPositioner();
	void ServiceL(const RMessage2& aMessage);
private:
	// Services available
	void SetRequestorL(const RMessage2& aMessage);
	void SetMultipleRequestorsL(const RMessage2& aMessage);
	void SetUpdateOptionsL(const RMessage2& aMessage);
	void GetUpdateOptionsL(const RMessage2& aMessage);
	void GetLastKnownPosition(const RMessage2& aMessage);
	void GetLastKnownPositionCompleteL(const RMessage2& aMessage);
	void GetLastKnownPositionCancel(const RMessage2& aMessage);
	void NotifyPositionUpdate(const RMessage2& aMessage);
	void NotifyPositionUpdateCompleteL(const RMessage2& aMessage);
	void NotifyPositionUpdateCancel(const RMessage2& aMessage);
private:
	// Privacy checking
	TBool AreRequestorsAcceptedL();
private:
	CEposPositioner();
	void ConstructL();
private:
	// Callbacks
	virtual void TimerCompleted(const RMessage2& aMessage);
private:
	TPositionUpdateOptions iUpdateOptions;
	RRequestorStack iRequestors;
private:
	CRequestTimer* iGetLastKnownPositionTimer;
	CRequestTimer* iNotifyPositionUpdateTimer;
	};

#endif // __TESTPOSITIONERSUBSES_H__
