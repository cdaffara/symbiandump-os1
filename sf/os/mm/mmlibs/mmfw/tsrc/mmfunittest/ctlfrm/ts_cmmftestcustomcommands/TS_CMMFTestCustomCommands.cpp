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
// CMMFTestCustomCommandParser.h
// 
//

#include "TS_CMMFTestCustomCommands.h"

//const TUid KMmfTestControllerUidUID = {KMmfTestControllerUid};
const TUid KMmfTestCustomCommandsUid = {0x101f72B4};


EXPORT_C CMMFTestCustomCommandParser* CMMFTestCustomCommandParser::NewL(MMMFTestCustomCommandImplementor& aImplementor)
	{
	CMMFTestCustomCommandParser* self = CMMFTestCustomCommandParser::NewLC(aImplementor);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMMFTestCustomCommandParser* CMMFTestCustomCommandParser::NewLC(MMMFTestCustomCommandImplementor& aImplementor)
	{
	CMMFTestCustomCommandParser* self = new (ELeave) CMMFTestCustomCommandParser(aImplementor);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMFTestCustomCommandParser::CMMFTestCustomCommandParser(MMMFTestCustomCommandImplementor& aImplementor) : 
	CMMFCustomCommandParserBase(KMmfTestCustomCommandsUid), 
	iImplementor(aImplementor)
	{
	}

CMMFTestCustomCommandParser::~CMMFTestCustomCommandParser(void)
	{
	}

void CMMFTestCustomCommandParser::ConstructL (void)
	{
	}


EXPORT_C void CMMFTestCustomCommandParser::HandleRequest(TMMFMessage& aMessage)
	{
	if (aMessage.Destination().InterfaceId() == InterfaceId())
		{
		TRAPD(error, DoHandleRequestL(aMessage));
		if (error)
			aMessage.Complete(error);
		}
	else
		{
		aMessage.Complete(KErrNotSupported);
		}
	}
	
void CMMFTestCustomCommandParser::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case EMMFTestCustomCommandSetAudioQuality:
		DoSetAudioQuality(aMessage);
		break;
	case EMMFTestCustomCommandGetAudioQuality:
		DoGetAudioQuality(aMessage);
		break;
	case EMMFTestCustomCommandGeneralSynchronous:
		iImplementor.MtccCustomCommandSyncL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralSynchronousWithReturn:
		iImplementor.MtccCustomCommandSyncWithReturnL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralAsynchronous:
		iImplementor.MtccCustomCommandAsyncL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralAsynchronousWithReturn:
		iImplementor.MtccCustomCommandAsyncWithReturnL(aMessage);
		break;
	case EMMFTestCustomCommandRequestGeneralEvent:
		iImplementor.MtccCustomCommandRequestGeneralEventL(aMessage);
		break;
	case EMMFTestCustomCommandSimulateReloading:
		iImplementor.MtccCustomCommandSimulateReloadingL(aMessage);
		break;
	case EMMFTestCustomCommandCheckUrl:
		iImplementor.MtccCustomCommandCheckUrlL(aMessage);
		break;
	case EMMFTestCustomCommandCheckLongUrl:
		iImplementor.MtccCustomCommandCheckLongUrlL(aMessage);
		break;
	case EMMFTestCustomCommandPanic:
		aMessage.Complete(KErrCompletion); // Return right away
		User::Panic(_L("CustomCommandPanic"),1);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		break;
		}
	}



void CMMFTestCustomCommandParser::DoSetAudioQuality(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> pckg;
	TRAPD(err, aMessage.ReadData1FromClientL(pckg));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	TInt audioQuality = pckg();
	if (audioQuality == 14)
		aMessage.Complete(KErrNone);
	else
		aMessage.Complete(KErrGeneral);

	// Call Implementor
	iImplementor.MtccSetAudioQuality(audioQuality);
	}

void CMMFTestCustomCommandParser::DoGetAudioQuality(TMMFMessage& aMessage)
	{
	TInt audioQuality = 0;
	// Call Implementor
	iImplementor.MtccGetAudioQuality(audioQuality);

	TPckgBuf<TInt> pckg = 14;
	
	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	aMessage.Complete(err);
	}


/* RMMFTestCustomCommands.
	This is a test custom command object to show that the framework works correctly.
  */

EXPORT_C RMMFTestCustomCommands::RMMFTestCustomCommands(RMMFController& aController, TUid aInterfaceId) : RMMFCustomCommandsBase(aController, aInterfaceId)
	{
	_LIT(KFuncText, "");
	iMemFunctionText = KFuncText;
	}

// These are Dummy commands to prove the framework is working
EXPORT_C TInt RMMFTestCustomCommands::SetAudioQuality(TInt aAudioQualityLevel)
	{
	_LIT(KFuncText, "SetAudioQuality Called");
	iMemFunctionText = KFuncText;

	TInt functionNo = 1;  // Need to define Enums

	TPckgBuf<TInt> pckg = aAudioQualityLevel;
	return iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 pckg,
									 KNullDesC8);
	}

EXPORT_C TInt RMMFTestCustomCommands::GetAudioQuality(TInt& aAudioQualityLevel)
	{
	_LIT(KFuncText, "GetAudioQuality Called");
	iMemFunctionText = KFuncText;

	TInt functionNo = 2; // Need to define Enums
	TPckgBuf<TInt> pckg;

	TInt error = iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 KNullDesC8,
									 KNullDesC8,
									 pckg);
	aAudioQualityLevel = pckg();
	return error;
	}
