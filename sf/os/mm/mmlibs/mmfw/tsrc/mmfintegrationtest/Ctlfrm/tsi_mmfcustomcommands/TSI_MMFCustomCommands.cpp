// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSI_MMFCustomCommands.h
// 
//

#include "TSI_MMFCustomCommands.h"



EXPORT_C CTSIMmfCustomCommandParser* CTSIMmfCustomCommandParser::NewL(TUid aInterfaceId)
	{
	CTSIMmfCustomCommandParser* self = CTSIMmfCustomCommandParser::NewLC(aInterfaceId);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CTSIMmfCustomCommandParser* CTSIMmfCustomCommandParser::NewLC(TUid aInterfaceId)
	{
	CTSIMmfCustomCommandParser* self = new (ELeave) CTSIMmfCustomCommandParser(aInterfaceId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTSIMmfCustomCommandParser::CTSIMmfCustomCommandParser(TUid aInterfaceId)
 : CMMFCustomCommandParserBase(aInterfaceId)
	{
	}

CTSIMmfCustomCommandParser::~CTSIMmfCustomCommandParser(void)
	{
	}

void CTSIMmfCustomCommandParser::ConstructL (void)
	{
	}

EXPORT_C void CTSIMmfCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	TBool complete = ETrue;
	switch (aMessage.Function())
		{
	case 1:
		complete = DoSetAudioQuality(aMessage);
		break;
	case 2:
		complete = DoGetAudioQuality(aMessage);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		return;
		}
	if (complete)
		aMessage.Complete(KErrNone);

	}


TBool CTSIMmfCustomCommandParser::DoSetAudioQuality(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNone);
	return ETrue;
	}

TBool CTSIMmfCustomCommandParser::DoGetAudioQuality(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrNone);
	return ETrue;
	}


// RTSIMmfCustomCommands.
// This is a test custom command object to show that the framework works correctly.
EXPORT_C RTSIMmfCustomCommands* RTSIMmfCustomCommands::NewL(RMMFController& aController, TUid aInterfaceId)
	{
	RTSIMmfCustomCommands* self = RTSIMmfCustomCommands::NewLC(aController, aInterfaceId);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C RTSIMmfCustomCommands* RTSIMmfCustomCommands::NewLC(RMMFController& aController, TUid aInterfaceId)
	{
	RTSIMmfCustomCommands* self = new (ELeave) RTSIMmfCustomCommands(aController, aInterfaceId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void RTSIMmfCustomCommands::ConstructL(void)
	{
	}

RTSIMmfCustomCommands::RTSIMmfCustomCommands(RMMFController& aController, TUid aInterfaceId) : RMMFCustomCommandsBase(aController, aInterfaceId)
	{
	}

RTSIMmfCustomCommands::~RTSIMmfCustomCommands()
	{
	}

// These are Dummy commands to prove the framework is working
EXPORT_C TBool RTSIMmfCustomCommands::SetAudioQuality(const TDesC8& aAudioQualityLevel)
	{
	TInt functionNo = EDummyFNumSetAudioQuality;

	return iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 aAudioQualityLevel,
									 KNullDesC8);
	}

EXPORT_C TInt RTSIMmfCustomCommands::GetAudioQuality(TDes8& aAudioQualityLevel)
	{
	TInt functionNo = EDummyFNumGetAudioQuality;

	return iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 KNullDesC8,
									 KNullDesC8,
									 aAudioQualityLevel);
	}
	















