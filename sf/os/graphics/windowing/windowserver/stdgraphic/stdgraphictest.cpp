// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "graphics/W32STDGRAPHICTEST.H"

EXPORT_C TWsGraphicFrameRate::TWsGraphicFrameRate():
	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this))
	{
	}

LOCAL_C const TUid KUidWsGraphicTestFrameRate = {0x10281AB2};

EXPORT_C CWsGraphicTestFrameRate* CWsGraphicTestFrameRate::NewL()
	{
	CWsGraphicTestFrameRate* self = new(ELeave) CWsGraphicTestFrameRate;
	CleanupStack::PushL(self);
	self->BaseConstructL(KUidWsGraphicTestFrameRate,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicTestFrameRate* CWsGraphicTestFrameRate::NewL(TUid aUid)
	{
	CWsGraphicTestFrameRate* self = new(ELeave) CWsGraphicTestFrameRate;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KUidWsGraphicTestFrameRate,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicTestFrameRate* CWsGraphicTestFrameRate::NewL(const TWsGraphicId& aReplace)
	{
	CWsGraphicTestFrameRate* self = new(ELeave) CWsGraphicTestFrameRate;
	CleanupStack::PushL(self);
	self->BaseConstructL(aReplace,KUidWsGraphicTestFrameRate,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicTestFrameRate::~CWsGraphicTestFrameRate()
	{
	}

CWsGraphicTestFrameRate::CWsGraphicTestFrameRate()
	{
	}

void CWsGraphicTestFrameRate::HandleMessage(const TDesC8& /*aData*/)
	{
	}

void CWsGraphicTestFrameRate::OnReplace()
	{
	}
