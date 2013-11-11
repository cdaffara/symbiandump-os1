// pointer.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdPointer : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPointer();
private:
	CCmdPointer();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TPoint iStart;
	};

EXE_BOILER_PLATE(CCmdPointer)

CCommandBase* CCmdPointer::NewLC()
	{
	CCmdPointer* self = new(ELeave) CCmdPointer();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPointer::~CCmdPointer()
	{
	}

CCmdPointer::CCmdPointer()
	{
	}

const TDesC& CCmdPointer::Name() const
	{
	_LIT(KName, "pointer");	
	return KName;
	}

void CCmdPointer::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgXPos, "x-pos");
	aArguments.AppendIntL(iStart.iX, KArgXPos);

	_LIT(KArgYPos, "y-pos");
	aArguments.AppendIntL(iStart.iY, KArgYPos);
	}

void CCmdPointer::DoRunL()
	{
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EButton1Down, iStart.iX, iStart.iY);
	LeaveIfErr(UserSvr::AddEvent(rawEvent), _L("Failed to add TRawEvent::EButton1Down event at %d, %d"), iStart.iX, iStart.iY);
	rawEvent.Set(TRawEvent::EButton1Up, iStart.iX, iStart.iY);
	LeaveIfErr(UserSvr::AddEvent(rawEvent), _L("Failed to add TRawEvent::EButton1Up event at %d, %d"), iStart.iX, iStart.iY);
	}
