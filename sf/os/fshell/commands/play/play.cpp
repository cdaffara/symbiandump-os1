// play.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include <e32base.h>
#include <bacline.h>
#include <mmf/server/sounddevice.h>
#include "play.h"

_LIT(KNewLine, "\n");


//______________________________________________________________________________
//						~CCmdPlay
CCommandBase* CCmdPlay::NewLC()
	{
	CCmdPlay* self = new(ELeave)CCmdPlay();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CCmdPlay::ConstructL()
	{
	BaseConstructL();
	iPlayer = CMmfPlayer::NewL(Stdout());
	}
	
CCmdPlay::~CCmdPlay()
	{
	delete iPlayer;
	}
	
const TDesC& CCmdPlay::Name() const
	{
	_LIT(KName, "play");
	return KName;
	}
	
void CCmdPlay::DoRunL()
	{
	if (iMaxVolume)
		{
		// we use DevSound to get the max volume, as CMdaAudioRecorderUtility
		// always returns 0 until we have opened a file.
		CMMFDevSound* devSound = CMMFDevSound::NewL();
		TInt maxVol = devSound->MaxVolume();
		TBool cons = Stdout().AttachedToConsole();
		if (cons) Printf(_L("Maximum volume: "));
		Printf(_L("%d"), maxVol);
		if (cons) Printf(_L("\r\n"));
		delete devSound;
		}
	else
		{
		if (!iArguments.IsPresent(0)) 
			{
			PrintError(KErrArgument, _L("No filename specified"));
			DisplayHelp();
			Complete(KErrArgument);
			return;
			}
		iPlayer->GetReady(iFile, iVerbose);
		if (iOptions.IsPresent(&iPriority) || iOptions.IsPresent(&iPreference))
			{
			iPlayer->SetPriority(iPriority, iPreference);
			}
		if (iOptions.IsPresent(&iVolume))
			{
			iPlayer->SetVolume(iVolume);
			}
		CActiveScheduler::Start();
		User::LeaveIfError(iPlayer->Error());
		}
	}

void CCmdPlay::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptPriority, "priority");
	aOptions.AppendIntL(iPriority, KOptPriority);

	_LIT(KOptPreference, "preference");
	aOptions.AppendIntL(iPreference, KOptPreference);

	_LIT(KOptVolume, "volume");
	aOptions.AppendIntL(iVolume, KOptVolume);

	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptMaxVolume, "max-volume");
	aOptions.AppendBoolL(iMaxVolume, KOptMaxVolume);
	}
	
void CCmdPlay::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgFiles, "file_name");
	aArguments.AppendFileNameL(iFile, KArgFiles);
	}
			
CCmdPlay::CCmdPlay()
	{
	}

//______________________________________________________________________________
//						CMmfPlayer
CMmfPlayer* CMmfPlayer::NewL(RIoConsoleWriteHandle& aStdOut)
	{
	CMmfPlayer* self = new(ELeave)CMmfPlayer(aStdOut);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CMmfPlayer::ConstructL()
	{
	CActiveScheduler::Add(this);
	iUtil = CMdaAudioRecorderUtility::NewL(*this);
	}
	
CMmfPlayer::CMmfPlayer(RIoConsoleWriteHandle& aStdOut)
	: CActive(CActive::EPriorityStandard), iStdOut(aStdOut), iVolume(-1)
	{
	}

CMmfPlayer::~CMmfPlayer()
	{
	delete iUtil;
	}
	
TInt CMmfPlayer::GetMaxVolume()
	{
	return iUtil->MaxVolume();
	}
	
TInt CMmfPlayer::Error()
	{
	return iError;
	}

void CMmfPlayer::GetReady(const TDesC& aFileName, TBool aVerbose)
	{
	iFileName = &aFileName;
	iVerbose = aVerbose;
	iState = EInitial;
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrNone);
	SetActive();
	}
	
void CMmfPlayer::SetPriority(TInt aPriority, TInt aPreference)
	{
	iUtil->SetPriority(aPriority, (TMdaPriorityPreference)aPreference);
	}

void CMmfPlayer::SetVolume(TInt aVolume)
	{
	iVolume = aVolume;
	}

void CMmfPlayer::RunL()
	{
	ProcessL();
	}
	
TInt CMmfPlayer::RunError(TInt aError)
	{
	Error(aError);
	return KErrNone;
	}

void CMmfPlayer::DoCancel()
	{
	}

void CMmfPlayer::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt aCurrentState, TInt aErrorCode)
	{
	if (aErrorCode!=KErrNone)
		{
		Error(aErrorCode);
		}
	else
		{
		if (aCurrentState!=EPlaying)
			{
			TRAPD(err, ProcessL());
			if (err!=KErrNone) Error(err);
			}				
		}
	}
	
void CMmfPlayer::ProcessL()
	{
	switch (iState)
		{
	case EInitial:
		iState = EOpening;
		iUtil->OpenFileL(*iFileName);
		Message(_L("Opening clip %S"), &iFileName);
		break;
	case EOpening:
		iState = EPlaying;
		if (iVolume != -1)
			{
			iUtil->SetVolume(iVolume);
			}
		iUtil->PlayL();
		Message(_L("Playing"));
		break;
	case EPlaying:
		Message(_L("Playing complete"));
		Stop();
		break;
		}
	}
	
void CMmfPlayer::Error(TInt aError)
	{
	switch (iState)
		{
	case EInitial:
		Message(_L("Error %d"), aError);
		break;
	case EOpening:
		Message(_L("Error %d when opening clip"), aError);
		break;
	case EPlaying:
		Message(_L("Error %d when playing clip"), aError);
		break;
	default:
		Message(_L("Error %d (in unknown state)"), aError);
		break;
		}
	Stop();
	iError = aError;
	}
	
void CMmfPlayer::Message(TRefByValue<const TDesC> aFmt, ...)
	{
	if (iVerbose)
		{
		TDesC fmt(aFmt);
		RBuf16 newText;
		iMsgBuf.Zero();

		TTruncateOverflow overflow;
		VA_LIST list;
		VA_START(list, aFmt);
		iMsgBuf.AppendFormatList(aFmt, list, &overflow);
		
		iStdOut.Write(iMsgBuf);
		iStdOut.Write(KNewLine);
		}
	}
	
void CMmfPlayer::Stop()
	{
	CActiveScheduler::Stop();
	}

EXE_BOILER_PLATE(CCmdPlay);

