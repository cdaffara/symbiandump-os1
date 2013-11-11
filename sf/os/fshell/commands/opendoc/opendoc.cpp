// opendoc.cpp
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

#include <apgcli.h>
#include <apasvst.h>
#include "opendoc.h"


CCommandBase* CCmdOpenDoc::NewLC()
	{
	CCmdOpenDoc* self = new(ELeave) CCmdOpenDoc();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdOpenDoc::~CCmdOpenDoc()
	{
	}

CCmdOpenDoc::CCmdOpenDoc()
	{
	}

const TDesC& CCmdOpenDoc::Name() const
	{
	_LIT(KName, "opendoc");
	return KName;
	}

void CCmdOpenDoc::DoRunL()
	{
	RApaLsSession lsSession;
	LeaveIfErr(lsSession.Connect(), _L("Couldn't connect to AppArc"));
	CleanupClosePushL(lsSession);
	TThreadId threadId;
	LeaveIfErr(lsSession.StartDocument(iFileName, threadId), _L("Couldn't open document"));
	CleanupStack::PopAndDestroy(&lsSession);
	}

void CCmdOpenDoc::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFileName, "file_name");
	aArguments.AppendFileNameL(iFileName, KArgFileName);
	}


#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdOpenDoc)
#endif

