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

#if !defined(SYMBIAN_NETWORKING_ESOCKTEST_BLOCKER_H_)
#define SYMBIAN_NETWORKING_ESOCKTEST_BLOCKER_H_

#include <e32base.h>
#include <cfmsgs.h>
#include <cfshared.h>
#include <cfutil.h>
using namespace CommsFW;

_LIT_SECURE_ID(TBlockerSID, 0x1020E7CF);
_LIT8(KBlockerCPMName, "blocker");

NONSHARABLE_CLASS(CBlockerChannelHandler) : public CCFModuleChannelHandler
	{
public:
	enum { EBlockingStateKey = 1 };
	enum {	EStateDontBlock = 0,	// default is not blocking
			EStateBlock = 1,		// test code wants block
			EStateBlocking = 2,		// blocker confirms it is now blocking
			EStateRelease = 3		// test code wants block released
		};

	static CBlockerChannelHandler* NewL(RCFChannel::TMsgQueues aRxQueues, RCFChannel::TMsgQueues aTxQueues);
	void MaybeBlockStartup();

protected:
	virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
private:
	CBlockerChannelHandler();
	};



#endif


