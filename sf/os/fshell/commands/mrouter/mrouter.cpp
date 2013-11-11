// mrouter.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <mrouterclient30.h>

using namespace IoUtils;

class CCmdMrouter : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMrouter();
private:
	CCmdMrouter();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
private:
	};


CCommandBase* CCmdMrouter::NewLC()
	{
	CCmdMrouter* self = new(ELeave) CCmdMrouter();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMrouter::~CCmdMrouter()
	{
	}

CCmdMrouter::CCmdMrouter()
	{
	}

const TDesC& CCmdMrouter::Name() const
	{
	_LIT(KName, "mrouter");	
	return KName;
	}

#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }
	
const TDesC* StringifyConnectionStatus(TmRouterProgressStatus aStatus)
	{
	switch (aStatus)
		{
		CASE_RETURN_LIT(EmRouterConnecting);
		CASE_RETURN_LIT(EmRouterConnected);
		CASE_RETURN_LIT(EmRouterDisconnected);
		CASE_RETURN_LIT(EmRouterDisconnecting);
		DEFAULT_RETURN_LIT("Unknown connection status");
		}
	}

void CCmdMrouter::DoRunL()
	{
	RmRouterClient30 mRouter;
	mRouter.OpenL();
	CleanupClosePushL(mRouter);
	TmRouterConnectionStatus connectionStatus;
	mRouter.ConnectionStatus(connectionStatus);
	TName connectionName;
	mRouter.GetConnectionNameL(connectionName);
	CleanupStack::PopAndDestroy(&mRouter);

	Printf(_L("Connection status: %S\r\n"), StringifyConnectionStatus(connectionStatus.iStatus));
	Printf(_L("Connection bearer:  0x%08x\r\n"), connectionStatus.iCurrentBearer.iUid);
	Printf(_L("Connection name:   \"%S\"\r\n"), &connectionName);
	}


EXE_BOILER_PLATE(CCmdMrouter)

