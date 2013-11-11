// BtSerialTest.cpp
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

#include <fshell/ioutils.h>
#include <fshell/btserialclient.h>

using namespace IoUtils;

class CCmdBtSerialTest : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdBtSerialTest();
private:
	CCmdBtSerialTest();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RBtSerialSession iSession;
	};


CCommandBase* CCmdBtSerialTest::NewLC()
	{
	CCmdBtSerialTest* self = new(ELeave) CCmdBtSerialTest();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdBtSerialTest::~CCmdBtSerialTest()
	{
	}

CCmdBtSerialTest::CCmdBtSerialTest()
	{
	}

const TDesC& CCmdBtSerialTest::Name() const
	{
	_LIT(KName, "BtSerialTest");	

	return KName;
	}

const TDesC& CCmdBtSerialTest::Description() const
	{
	_LIT(KDescription, "Test for BT serial incoming connection");
	return KDescription;
	}

void CCmdBtSerialTest::DoRunL()
	{
	TInt err = iSession.Connect();
	if (err!=KErrNone)
		{
		Printf(_L("Failed to connect %d\n"), err);
		iSession.Close();
		return;
		}
	TRequestStatus req;
	iSession.WaitForConnection(req);
	Printf(_L("Waiting for connection...\n"));
	User::WaitForRequest(req);
	Printf(_L("Connected %d\n"), req.Int());
	
	TBuf8<0x100> buf;
	iSession.Read(buf, req);
	Printf(_L("Waiting for data...\n"));
	User::WaitForRequest(req);
	if (req != KErrNone)
		{
		Printf(_L("Read error %d\n"), req.Int());
		}
	else
		{
		iSession.Write(buf, req);
		Printf(_L("Echoing data...\n"));
		User::WaitForRequest(req);
		Printf(_L("Write returns %d"), req.Int());
		}
	
	iSession.Close();	
	}

void CCmdBtSerialTest::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CCmdBtSerialTest::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	}


EXE_BOILER_PLATE(CCmdBtSerialTest)

