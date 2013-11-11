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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "wsbufferdrawer.h"
#include <s32mem.h>

CWsBufferGraphic::CWsBufferGraphic()
	{
	}

EXPORT_C CWsBufferGraphic::~CWsBufferGraphic()
	{
	}

EXPORT_C CWsBufferGraphic* CWsBufferGraphic::NewL()
	{
	CWsBufferGraphic* self = new(ELeave) CWsBufferGraphic;
	CleanupStack::PushL(self);
 	self->BaseConstructL(KBufferDrawerImplId,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsBufferGraphic* CWsBufferGraphic::NewL(TUid aUid)
	{
	CWsBufferGraphic* self = new(ELeave) CWsBufferGraphic;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KBufferDrawerImplId,KNullDesC8());
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsBufferGraphic* CWsBufferGraphic::NewL(const TWsGraphicId& aReplace)
	{
	CWsBufferGraphic* self = new(ELeave) CWsBufferGraphic;
	CleanupStack::PushL(self);
	self->BaseConstructL(aReplace,KBufferDrawerImplId,KNullDesC8());
 	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TInt CWsBufferGraphic::UpdateWhiteLinePos(TInt aWhiteLinePos)
	{
	// Send the message to server side
	TBuf8<1> cmd;
	cmd.Append(aWhiteLinePos);
	
	SendMessage(cmd);
	return Flush();
	}

EXPORT_C void CWsBufferGraphic::HandleMessage(const TDesC8& /*aData*/)
	{
	}

EXPORT_C void CWsBufferGraphic::OnReplace()
	{
	}

EXPORT_C TInt CWsBufferGraphic::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

EXPORT_C TInt CWsBufferGraphic::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

EXPORT_C TInt CWsBufferGraphic::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

EXPORT_C TInt CWsBufferGraphic::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}  		 

