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
//

#include "TS_CMMFVideoTestCustomCommands.h"


EXPORT_C CMMFTestCustomCommandParser* CMMFTestCustomCommandParser::NewL(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor)
	{
	CMMFTestCustomCommandParser* self = CMMFTestCustomCommandParser::NewLC(aInterfaceId, aImplementor);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CMMFTestCustomCommandParser* CMMFTestCustomCommandParser::NewLC(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor)
	{
	CMMFTestCustomCommandParser* self = new (ELeave) CMMFTestCustomCommandParser(aInterfaceId, aImplementor);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMFTestCustomCommandParser::CMMFTestCustomCommandParser(TUid aInterfaceId, MMMFTestCustomCommandImplementor& aImplementor) 
	: CMMFCustomCommandParserBase(aInterfaceId),iImplementor(aImplementor)
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
	case EMMFTestCustomCommandSetFrameRate:
		DoSetFrameRate(aMessage);
		break;
	case EMMFTestCustomCommandGetFrameRate:
		DoGetFrameRate(aMessage);
		break;
	case EMMFTestCustomCommandGeneralSynchronous:
		iImplementor.MvtccCustomCommandSyncL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralSynchronousWithReturn:
		iImplementor.MvtccCustomCommandSyncWithReturnL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralAsynchronous:
		iImplementor.MvtccCustomCommandAsyncL(aMessage);
		break;
	case EMMFTestCustomCommandGeneralAsynchronousWithReturn:
		iImplementor.MvtccCustomCommandAsyncWithReturnL(aMessage);
		break;
	case EMMFTestCustomCommandRequestGeneralEvent:
		iImplementor.MvtccCustomCommandRequestGeneralEventL(aMessage);
		break;
	case EMMFTestCustomCommandSimulateReloading:
		iImplementor.MvtccCustomCommandSimulateReloadingL(aMessage);
		break;
	case EMMFTestCustomCommandCheckUrl:
		iImplementor.MvtccCustomCommandCheckUrlL(aMessage);
		break;
#ifdef SYMBIAN_BUILD_GCE
	case EMMFTestCustomCommandSimulateSurfaceCreated:
		iImplementor.MvtccCustomCommandSimulateSurfaceCreatedL(aMessage);
		break;
	case EMMFTestCustomCommandSimulateParaChanged:
		iImplementor.MvtccCustomCommandSimulateParaChangedL(aMessage);
		break;
	case EMMFTestCustomCommandSimulateRemoveSurface:
		iImplementor.MvtccCustomCommandSimulateRemoveSurfaceL(aMessage);
		break;
#endif // SYMBIAN_BUILD_GCE
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	case EMMFTestCustomCommandSetSubtitleDisplayCheck:
		iImplementor.MvtccCustomCommandSetSubtitleDisplayCheckL(aMessage);
		break;
	case EMMFTestCustomCommandGetSubtitleCallCount:
		iImplementor.MvtccCustomCommandGetSubtitleCallCount(aMessage);
		break;
	case EMMFTestCustomCommandSimulateCrpReadyEvent:
		iImplementor.MvtccCustomCommandSimulateCrpReadyEvent(aMessage);
		break;
	case EMMFTestCustomCommandSetLanguageNotAvail:
		iImplementor.MvtccCustomCommandSetLanguageNotAvail(aMessage);
		break;
#endif
	default:
		aMessage.Complete(KErrNotSupported);
		break;
		}
	}

void CMMFTestCustomCommandParser::DoSetFrameRate(TMMFMessage& aMessage)
	{
	TPckgBuf<TReal32> pckg;

	TRAPD(err, aMessage.ReadData1FromClientL(pckg));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}

	TReal32 frameRate = pckg();
	if (frameRate == 14)
		aMessage.Complete(KErrNone);
	else
		aMessage.Complete(KErrGeneral);
	}

void CMMFTestCustomCommandParser::DoGetFrameRate(TMMFMessage& aMessage)
	{
	TPckgBuf<TReal32> pckg = 14;

	TRAPD(err, aMessage.WriteDataToClientL(pckg));
	aMessage.Complete(err);
	}

	
/* RMMFTestCustomCommands.
	This is a test custom command object to show that the framework works correctly.

  */
  

EXPORT_C RMMFTestCustomCommands::RMMFTestCustomCommands(RMMFController& aController, TUid aInterfaceId) : RMMFCustomCommandsBase(aController, aInterfaceId)
	{
	}

// These are Dummy commands to prove the framework is working
EXPORT_C TInt RMMFTestCustomCommands::MvcSetFrameRateL(TReal32 aFramesPerSecond)
	{
	TInt functionNo = 1;  // Need to define Enums

	TPckgBuf<TReal32> pckg = aFramesPerSecond;
	return iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 pckg,
									 KNullDesC8);
	}

EXPORT_C TInt RMMFTestCustomCommands::MvcGetFrameRateL(TReal32& aFramesPerSecond)
	{
	TInt functionNo = 2; // Need to define Enums
	TPckgBuf<TReal32> pckg;

	TInt error = iController.CustomCommandSync(iDestinationPckg, 
									 functionNo, 
									 KNullDesC8,
									 KNullDesC8,
									 pckg);
	aFramesPerSecond = pckg();
	return error;
	}

