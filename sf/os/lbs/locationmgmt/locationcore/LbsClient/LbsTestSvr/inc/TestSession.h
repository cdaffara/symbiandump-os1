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

#ifndef __TESTSESSION_H__
#define __TESTSESSION_H__

#include <e32base.h>
#include <lbscommon.h>
#include <lbsipc.h>
#include "TestServer.h"
#include "RequestTimer.h"

class CEposServSession : public CSession2, public MRequestTimerUser
	{
public:
	CEposServSession();
	~CEposServSession();
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void CreateL();
private:
	// Helpers
	void DispatchL(const RMessage2& aMessage);
	CEposServServer& Server();
	TInt SetDummyPositionModuleInfo(TInt aNum, TPositionModuleInfo& aPositionModuleInfo);
private:
	// Services available 
	void NewPositionerL(const RMessage2& aMessage);
	void NewPositionerFromModuleIdL(const RMessage2& aMessage);
	void NewPositionerFromCriteriaL(const RMessage2& aMessage);
	void ClosePositionerL(const RMessage2& aMessage);
	void GetDefaultModuleIdL(const RMessage2& aMessage);
	void GetNumModulesL(const RMessage2& aMessage);
	void GetModuleInfoByIndexL(const RMessage2& aMessage);
	void GetModuleInfoByIdL(const RMessage2& aMessage);
	void GetModuleStatusL(const RMessage2& aMessage);
	void NotifyModuleStatusEvent(const RMessage2& aMessage);
	void NotifyModuleStatusEventCancel(const RMessage2& aMessage);
	void NotifyModuleStatusEventCompleteL(const RMessage2& aMessage);
private:
	// Callbacks
	virtual void TimerCompleted(const RMessage2& aMessage);
private:
	CObjectIx* iObjectIx;
	CObjectConIx* iContainerIx;
	CObjectCon* iObjectCon;
private:
	CRequestTimer* iNotifyModuleStatusEventTimer;
	};

inline CEposServServer& CEposServSession::Server()
	{return *static_cast<CEposServServer*>(const_cast<CServer2*>(CSession2::Server()));}

#endif  // __TESTSESSION_H__
