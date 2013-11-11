/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: DRM Player
*
*/

#include "SimpleSoundPlayer.h"

const TInt CSimpleSoundPlayer::KPlayerActionPlay = 1;
const TInt CSimpleSoundPlayer::KPlayerActionPause = 2;
const TInt CSimpleSoundPlayer::KPlayerActionStopPlayFile = 3;
const TInt CSimpleSoundPlayer::KPlayerActionReportPosition = 4;
const TInt CSimpleSoundPlayer::KPlayerActionReportDuration = 5;
const TInt CSimpleSoundPlayer::KPlayerActionStopPlayDes = 6;
const TInt CSimpleSoundPlayer::KPlayerActionReportVolume = 7;
const TInt CSimpleSoundPlayer::KPlayerActionStopPlayUrl = 8;
const TInt CSimpleSoundPlayer::KPlayerActionStopPlayFileHandler = 9;
const TInt CSimpleSoundPlayer::KPlayerActionOpen = 10;
const TInt CSimpleSoundPlayer::KPlayerActionOpenFileHandler = 11;
const TInt CSimpleSoundPlayer::KplayerActionSetVolume = 12;
const TInt CSimpleSoundPlayer::KplayerActionGetBalance = 13;
const TInt CSimpleSoundPlayer::KplayerActionGetBitRate = 14;
const TInt CSimpleSoundPlayer::KplayerActionGetAudioLoadingProgress = 15;
const TInt CSimpleSoundPlayer::KplayerActionRegisterForAudioLoadingNotification = 16;
const TInt CSimpleSoundPlayer::KplayerActionControllerImplementationInformation = 17;
const TInt CSimpleSoundPlayer::KplayerActionMaxVolume = 18;
const TInt CSimpleSoundPlayer::KplayerActionSetBalance = 19;
const TInt CSimpleSoundPlayer::KplayerActionSetPriority = 20;
const TInt CSimpleSoundPlayer::KplayerActionSetRepeats = 21;
const TInt CSimpleSoundPlayer::KPlayerActionGetMetaDataEntry = 22;
const TInt CSimpleSoundPlayer::KPlayerActionGetNumberOfMetaDataEntries = 23;


CSimpleSoundPlayer::CSimpleSoundPlayer(CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate, TBool aFileType)
			: iConsole(aConsole) , iImmediate(aImmediate) , iLogger(aLogger) , iSoundOpen(EFalse),
				iFinalError(KErrNone), iFileType(aFileType)
	{
	iSupervisor =0 ;
	iEventTarget=0;
	iParameters =0;
	iPlayerId =0 ;
	iPlayVolume = false;
	iMetaInfo = false;
	iMetaInfoTest = false;
	iPlayWindow = false;
	iClearWindow = false;
	iSetBalance = false;
	iFsOpened = false;
	iDontPlayAudioClip = false;
	iIsLoopPlayEnabled = EFalse;
	}

void CSimpleSoundPlayer::ConstructUrlL(const TDesC &aUrl, TInt aIapId, const TDesC8 &aMime)
	{
	iMdaPlayer=CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer,
       							 TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));
	//KUseDefaultIap
	iMdaPlayer->OpenUrlL(aUrl, aIapId, aMime );
	iSoundOpen=ETrue;

	_LIT(KPlayerCreated, "SoundPlayerCreated");
	iConsole.Printf(100, KPlayerCreated, KPlayerCreated);
	}
CSimpleSoundPlayer* CSimpleSoundPlayer::NewUrlL(const TDesC &aUrl, TInt aIapId, const TDesC8 &aMime, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate)
	{
    CSimpleSoundPlayer* self = NewUrlLC(aUrl, aIapId, aMime, aConsole, aLogger, aImmediate);
	CleanupStack::Pop(self);
    return self;
	}
CSimpleSoundPlayer* CSimpleSoundPlayer::NewUrlLC(const TDesC &aUrl, TInt aIapId, const TDesC8 &aMime, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate)
	{
    CSimpleSoundPlayer* self = new (ELeave) CSimpleSoundPlayer(aConsole, aLogger, aImmediate);
    CleanupStack::PushL(self);
    self->ConstructUrlL(aUrl, aIapId, aMime);
    return self;
	}


void CSimpleSoundPlayer::ConstructL(const TDesC8 &aDescriptor)
	{
	// Create a file audio player utility instance
	//iMdaPlayer=CMdaAudioPlayerUtility::NewFilePlayerL(aFile, *this);	//Para usar creación inmediata

	if (iImmediate)
		{
		if (iFileType)
			{
			iMdaPlayer=CDrmPlayerUtility::NewDesPlayerReadOnlyL(aDescriptor,*this, KAudioPriorityRealOnePlayer,
			        		TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));    //For inmediate instantiation
			}
		else
			{
			iMdaPlayer=CDrmPlayerUtility::NewDesPlayerL(aDescriptor,*this, KAudioPriorityRealOnePlayer,
						        		TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));    //For inmediate instantiation
			}

	/*	iMdaPlayer=CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer,
       							 TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));		//Por Pasos
		iMdaPlayer->OpenDesL(aDescriptor);
		iSoundOpen=ETrue;	*/
		}
	else if (!iImmediate)
		{
		iMdaPlayer=CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer,
       							 TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));		//Por Pasos
		iMdaPlayer->OpenDesL(aDescriptor);
		iSoundOpen=ETrue;
		}
	_LIT(KPlayerCreated, "SoundPlayerCreated");
	iConsole.Printf(100, KPlayerCreated, KPlayerCreated);
	}
CSimpleSoundPlayer* CSimpleSoundPlayer::NewL(const TDesC8 &aDescriptor, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate, TBool aDesReadOnly)
	{
    CSimpleSoundPlayer* self = NewLC(aDescriptor, aConsole, aLogger, aImmediate, aDesReadOnly);
	CleanupStack::Pop(self);
    return self;
	}
CSimpleSoundPlayer* CSimpleSoundPlayer::NewLC(const TDesC8 &aDescriptor, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate, TBool aDesReadOnly)
	{
    CSimpleSoundPlayer* self = new (ELeave) CSimpleSoundPlayer(aConsole, aLogger, aImmediate, aDesReadOnly);
    CleanupStack::PushL(self);
    self->ConstructL(aDescriptor);
    return self;
	}

void CSimpleSoundPlayer::ConstructL(const TFileName &aFile)
	{
	// Create a file audio player utility instance
	//iMdaPlayer=CMdaAudioPlayerUtility::NewFilePlayerL(aFile, *this);	//For inmediate instantiation
    if (iImmediate)
        {
        iMdaPlayer=CDrmPlayerUtility::NewFilePlayerL(aFile,*this, KAudioPriorityRealOnePlayer,
        		TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));    //For inmediate instantiation
        }
    else if (!iImmediate)
    	{
    	iImmediate = ETrue;
		iMdaPlayer=CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer,
       							 TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));		//Instantiation by steps

		if (iFileType)
			{
			TMMFileSource FileName = aFile;
			iLogger.Log(_L("Opening file [%S] with FileType 1") , &FileName.Name());
//			TRAPD(err1,iMdaPlayer->OpenFileL(FileName));
			iMdaPlayer->OpenFileL(FileName);
			}
		else
			{
			iLogger.Log(_L("Opening file with FileType 0"));
//			TRAPD(err,iMdaPlayer->OpenFileL(aFile));
			iMdaPlayer->OpenFileL(aFile);
			}

		iSoundOpen=ETrue;
   		}

	_LIT(KPlayerCreated, "SoundPlayerCreated");
	iConsole.Printf(100, KPlayerCreated, KPlayerCreated);
	}

CSimpleSoundPlayer* CSimpleSoundPlayer::NewLC(const TFileName &aFile, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate, TBool aFileType)
	{
    CSimpleSoundPlayer* self = new (ELeave) CSimpleSoundPlayer(aConsole, aLogger, aImmediate, aFileType);
    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    return self;
	}

CSimpleSoundPlayer* CSimpleSoundPlayer::NewL(const TFileName &aFile, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate, TBool aFileType)
	{
    CSimpleSoundPlayer* self = NewLC(aFile, aConsole, aLogger, aImmediate, aFileType);
	CleanupStack::Pop(self);
    return self;
	}

void CSimpleSoundPlayer::ConstructL(const RFile &aFile)
	{
	// Create a file audio player utility instance
	//iMdaPlayer=CMdaAudioPlayerUtility::NewFilePlayerL(aFile, *this);	//For inmediate instantiation

	// iMdaPlayer=CDrmPlayerUtility::NewL(*this, KAudioPriorityRealOnePlayer, TMdaPriorityPreference(KAudioPrefRealOneLocalPlayback));		//Instantiation by steps
	iMdaPlayer=CDrmPlayerUtility::NewL(*this, EMdaPriorityNormal, TMdaPriorityPreference(EMdaPriorityPreferenceTimeAndQuality));	
	iMdaPlayer->OpenFileL(aFile);
	iSoundOpen=ETrue;

	_LIT(KPlayerCreated, "SoundPlayerCreated");
	iConsole.Printf(100, KPlayerCreated, KPlayerCreated);
	}

CSimpleSoundPlayer* CSimpleSoundPlayer::NewLC(const RFile &aFile, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate)
	{
    CSimpleSoundPlayer* self = new (ELeave) CSimpleSoundPlayer(aConsole, aLogger, aImmediate);
    CleanupStack::PushL(self);
    self->ConstructL(aFile);
    return self;
	}

CSimpleSoundPlayer* CSimpleSoundPlayer::NewL(const RFile &aFile, CTestModuleIf &aConsole, CStifLogger &aLogger, TBool aImmediate)
	{
    CSimpleSoundPlayer* self = NewLC(aFile, aConsole, aLogger, aImmediate);
	CleanupStack::Pop(self);
    return self;
	}

void CSimpleSoundPlayer::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iLogger.Log(_L("MapcInitComplete"));
	if (iFsOpened)
	    {
		iFs.Close();
		iFsOpened = false;
	    }
	if (iImmediate)
		{
		if (aError == KErrNone)
			{
			_LIT(KGonnaPlay, "MapcInitComplete, GoingToPlay");
			_LIT(KGoing, "Going to");
			_LIT(KDone, "Done Started Playing");
			iConsole.Printf(100, KGonnaPlay, KGoing);
			if (iPlayVolume)
			{
		//	TInt VolumeHigh;
			iLogger.Log(_L("SetVolume") );
			iMdaPlayer->SetVolume(iMdaPlayer->MaxVolume());
			iLogger.Log(_L("SetVolume(iMdaPlayer->MaxVolume())  max=(%d)"),  iMdaPlayer->MaxVolume() );
			}
			if (iMetaInfo)
			    {
				TInt NumMetaDatas = 0;
				TInt MetaDataError = iMdaPlayer->GetNumberOfMetaDataEntries(NumMetaDatas);
				if ( MetaDataError )
				    {	//Error getting meta data info
					Exit(MetaDataError);
				    }
//				else
				if (iMetaInfoTest)
				    {	//No error yet
					TInt currMetaDataIndex = 0;
					CMMFMetaDataEntry* currMetaData = NULL;
					for ( currMetaDataIndex=0 ; currMetaDataIndex < NumMetaDatas ; currMetaDataIndex++)
					    {
					    currMetaData = iMdaPlayer->GetMetaDataEntryL(currMetaDataIndex);
						iLogger.Log(_L("MetaData[%d]: Name: [%S], Value: [%S]") , currMetaDataIndex , &(currMetaData->Name()) , &(currMetaData->Value()) );
//						currMetaData = NULL;
					    }
					delete currMetaData;
					currMetaData = NULL;
				    }
			    }
			if (iPlayWindow)
			    {
				TInt err = iMdaPlayer->SetPlayWindow(iStartPosition, iEndPosition);
				iLogger.Log(_L("SetPlayWindow, start=[%d]"),iStartPosition.Int64());
				iLogger.Log(_L("SetPlayWindow, end=[%d]"), iEndPosition.Int64() );
				iLogger.Log(_L("SetPlayWindow, error = [%d]"),err);
				if (iClearWindow)
				    {
					iLogger.Log(_L("ClearPlayWindow"));
					iMdaPlayer->ClearPlayWindow();
				    }
			    }
			if (iSetBalance)
			    {
				SetBalance(iBalance);
			    }
            if (iDontPlayAudioClip)
                {
                iState = EReady;
                Exit(aError);
                }
            else
                {
                iState = EPlaying;
                iLastPlayTime.HomeTime();
                iLastStopTime.HomeTime();
                // do not manually set the position here. if play window is set then the position is set to 0 (iPosition)
                // instead the position must be set to the play window start position otherwise it crashes. 
                if (!iPlayWindow)
                    {
                    iLogger.Log(_L("Position before Play (%d)"), I64INT(iPosition.Int64())/1000000);
                    iMdaPlayer->SetPosition(iPosition);
                    }
                iMdaPlayer->Play();
                iLogger.Log(_L("Play() was called"));
                iConsole.Printf(100, KGonnaPlay, KDone);
                }
			}
		else
			{
			iLogger.Log(_L("InitError (%d)"), aError);
			iState = ENotReady;
			Exit(aError);
			}
		}
	else
		{
		iLogger.Log(_L("InitComplete, error(%d)") , aError);
		iState = aError ? ENotReady : EReady;
		if (aError)
			{
			Exit(aError);
			}
		}
	}

TInt CSimpleSoundPlayer::Pause()
	{
	iLogger.Log(_L("Requested Pause"));
	iState=EPaused;
	iMdaPlayer->Pause();
	return KErrNone;
	}

TInt CSimpleSoundPlayer::OpenL(const TFileName &aFileName , const TBool &aImmediate=ETrue)
	{
	TMMFileSource FileName = aFileName;
//	TMMFileSource* FileName = aFileName;
//	FileName->TMMFileSource((TDesC*)aFileName);//= (TMMSource*) aFileName;
	iImmediate = aImmediate;
//	Stop();
//	iLogger.Log(_L("Stop before opening file"));

	iLogger.Log(_L("Opening [%S]") , &aFileName);
	iMdaPlayer->OpenFileL(FileName);
	iSoundOpen=ETrue;
	iLogger.Log(_L("Opened"));
	return KErrNone;
	}

TInt CSimpleSoundPlayer::OpenFileHandlerL(const TFileName &aFileName , const TBool &aImmediate=ETrue)
	{
	iImmediate = aImmediate;
//	Stop();
//	iLogger.Log(_L("Stop before opening file"));

	iLogger.Log(_L("Opening [%S]") , &aFileName);

//	RFs aFs;
//	RFile aFileHandler;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	User::LeaveIfError(iFileHandler.Open( iFs, aFileName, EFileRead));
	iMdaPlayer->OpenFileL(iFileHandler);

	iFileHandler.Close();
	iFsOpened = ETrue;
//	aFs.Close();

	iSoundOpen=ETrue;
	iLogger.Log(_L("Opened"));
	return KErrNone;
	}

TInt CSimpleSoundPlayer::OpenUrlL(const TDesC &aUrl, TInt aIapId, const TDesC8 &aMime)
	{
	iLogger.Log(_L("Opening url [%S] with IapId [%s]") , &aUrl , aIapId );
	iLogger.Log(_L8("   and MIME Type [%S]") , &aMime);
	iMdaPlayer->OpenUrlL(aUrl, aIapId, aMime);
	return ETrue;
	}

TInt CSimpleSoundPlayer::OpenDesL(const TDesC8 &aSoundDes , const TBool &aImmediate)
	{
	iImmediate = aImmediate;
	Stop();
	iLogger.Log(_L("Stop before opening descriptor"));

	iLogger.Log(_L("Closed, opening second descriptor of length (%d)") , aSoundDes.Length() );

	iMdaPlayer->OpenDesL(aSoundDes);
	iSoundOpen=ETrue;
	iLogger.Log(_L("Opened"));
	////iAutoClose = ETrue;	///K
	return KErrNone;
	}

TInt CSimpleSoundPlayer::Play()
	{
	if(iState==EReady || iState==EPaused)
		{
		iState=EPlaying;
		iLastPlayTime.HomeTime();
		iLastStopTime.HomeTime();
		iMdaPlayer->Play();
		iLogger.Log(_L("Requested Play"));
		}
	else
		{
		iLogger.Log(_L("Requested Play, Not Ready to play"));
		iImmediate = true;
		}
	return KErrNone;
	}

TInt CSimpleSoundPlayer::Stop(const TBool &aAlsoClose)
	{
	iLogger.Log(_L("Requested Stop,,,,,"));
	iMdaPlayer->Stop();
	iLogger.Log(_L("Requested Stop,,,,, successful"));
	iLastStopTime.HomeTime();
	if (aAlsoClose)
		{
		iLogger.Log(_L("iMdaPlayer->Close()" ));
		iMdaPlayer->Close();
		iLogger.Log(_L("iMdaPlayer->Close() successful" ));
		iSoundOpen=EFalse;
		iState = ENotReady;
		}
	else
		{
		if (iState==EPlaying || iState==EPaused) {iState = EReady;}
		}
	return KErrNone;
	}

void CSimpleSoundPlayer::MdapcPlayComplete(TInt aError)
	{
	iLastStopTime.HomeTime();
	iLogger.Log(_L("MapcPlayComplete, error (%d)") , aError);

	_LIT(KGonnaPlay, "MapcPlayComplete, GoingToPlay");
	_LIT(KGoing, "Going to");

	iConsole.Printf(100, KGonnaPlay, KGoing);

	iState = aError ? ENotReady : EReady;
	iConsole.Printf(100, KGonnaPlay, KGoing);

	Exit(aError);
	if (iEventTarget && iParameters)
		{
		Stop();
		iEventTarget->ExecuteL(iParameters) ;
		}
	}

CSimpleSoundPlayer::~CSimpleSoundPlayer()
	{
	iLogger.Log(_L("Trying to stop player") );
	iMdaPlayer->Stop();
	iLogger.Log(_L("Trying to close player") );
	iMdaPlayer->Close();
	iLogger.Log(_L("  Player stopped") );
    delete iMdaPlayer;
    iMdaPlayer = NULL;

	delete iEventTarget;
   	delete iParameters;
    delete iSupervisor;
	}

TTimeIntervalMicroSeconds CSimpleSoundPlayer::GetDuration()
	{
	TTimeIntervalMicroSeconds duration = iMdaPlayer->Duration();
	iLogger.Log(_L("CMdaAudioPlayerUtility::Duration() returned [%d]") , duration.Int64() );
	return duration;
	}

TTimeIntervalMicroSeconds CSimpleSoundPlayer::GetPlayDuration() { return iLastStopTime.MicroSecondsFrom(iLastPlayTime); }

TTime CSimpleSoundPlayer::GetPlayStartTime() { return iLastPlayTime; }

TTime CSimpleSoundPlayer::GetPlayStopTime() { return iLastStopTime; }

TInt CSimpleSoundPlayer::GetBitRate(TUint &aBitRate)
	{
	TInt error = iMdaPlayer->GetBitRate(aBitRate);
	iLogger.Log(_L("Retrieving BitRate (%d), error(%d)") , aBitRate, error);
	return error;
	}

TInt CSimpleSoundPlayer::GetBalance(TInt &aBalance)
	{
	TInt error = iMdaPlayer->GetBalance(aBalance);
	iLogger.Log(_L("Retrieving Balance (%d), error(%d)") , aBalance, error);
	return error;
	}

TInt CSimpleSoundPlayer::SetBalance(TInt aBalance=KMMFBalanceCenter)
	{
	TInt error=KErrNone;
	TInt error2=KErrNone;
	TInt OldBalance=0;

	iLogger.Log(_L("Changing Balance "));

	error=iMdaPlayer->GetBalance(OldBalance);
	if (error)
		{
		iLogger.Log(_L("   Error getting old balance (%d)"), error) ;
		}
	else
		{
		iLogger.Log(_L("   Previous balance: %d"), OldBalance);
		}
	iLogger.Log(_L("   Setting balance to %d"), aBalance);
	error2 = iMdaPlayer->SetBalance(aBalance);
	if (error2)
		{
		iLogger.Log(_L("   Error setting new balance (%d)") , error);
		}
	else
		{
		error=iMdaPlayer->GetBalance(OldBalance);
		if (error)
			{
			iLogger.Log(_L("   Error getting new balance (%d)"), error) ;
			}
		else
			{
			iLogger.Log(_L("   Current balance: %d"), OldBalance);
			}
		}

	if (OldBalance!= aBalance)
	{
		Exit(-1502);
	}

	return error2;
	}

TInt CSimpleSoundPlayer::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
	{
	iLogger.Log(_L("Trying to set priority settings to (%d),(%d) ") , aPriority, aPref);
	TInt PriorityError = iMdaPlayer->SetPriority(aPriority, aPref);
	if (PriorityError)
		{
		iLogger.Log(_L("Error setting priority to (%d),(%d) ") , aPriority, aPref);
		}
	else
		{
		iLogger.Log(_L("Priority set to (%d),(%d)") , aPriority, aPref);
		}
	return PriorityError;
	}

void CSimpleSoundPlayer::SetPosition(const TTimeIntervalMicroSeconds &aPosition)
	{
	iMdaPlayer->SetPosition(aPosition);
	}


TInt CSimpleSoundPlayer::GetPosition(TTimeIntervalMicroSeconds &aPosition)
	{
	TTimeIntervalMicroSeconds position;
	TInt PositionError = iMdaPlayer->GetPosition(position);

	if (!PositionError)
		{
		iLogger.Log(_L("Current position [%d]") , position.Int64());
		if (iState != EPlaying) {iLogger.Log(_L("  But not playing"));}
		if ( ! iSoundOpen ) {iLogger.Log(_L("  But not open"));}
		aPosition=position;
		}
	else
		{
		Exit(PositionError);
		}
	return PositionError;
	}

void CSimpleSoundPlayer::Exit(TInt aExitCode)
	{
	iFinalError = aExitCode;
	if (iSupervisor)
		{
		iSupervisor->NotifyDestruction();
		}
	else
		{
		CActiveScheduler::Stop();
		}
	}

void CSimpleSoundPlayer::SetVolumeRamp(const TTimeIntervalMicroSeconds &aRampDuration)
	{
	iMdaPlayer->SetVolumeRamp(aRampDuration);
	}

void CSimpleSoundPlayer::SetVolume(TInt aNewVolume)
	{
	iMdaPlayer->SetVolume(aNewVolume);
	}

TInt CSimpleSoundPlayer::GetVolume(TInt &aVolume)
	{
	TInt VolumeError = iMdaPlayer->GetVolume(aVolume);
	if (VolumeError)
		{
		iLogger.Log(_L("Error (%d) getting volume") , VolumeError );
		}
	else
		{
		iLogger.Log(_L("Getting volume: (%d)") , aVolume);
		}
	return VolumeError;
	}

TInt CSimpleSoundPlayer::MaxVolume()
	{
	return iMdaPlayer->MaxVolume();
	}

void CSimpleSoundPlayer::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds &aTrailingSilence)
	{
	iMdaPlayer->SetRepeats(aRepeatNumberOfTimes , aTrailingSilence);
	if ((aRepeatNumberOfTimes == -2) || (aRepeatNumberOfTimes > 0))
	    iIsLoopPlayEnabled = ETrue;
	else
	    iIsLoopPlayEnabled = EFalse;
	}

void CSimpleSoundPlayer::RegisterForAudioLoadingNotification(MAudioLoadingObserver &aLoadingObserver)
	{
	iMdaPlayer->RegisterForAudioLoadingNotification(aLoadingObserver);
	}

void CSimpleSoundPlayer::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
	iMdaPlayer->GetAudioLoadingProgressL(aPercentageProgress);
	}

void CSimpleSoundPlayer::ControllerImplementationInformationL()
	{
	//int g=0,a;
	const CMMFControllerImplementationInformation& x = iMdaPlayer->ControllerImplementationInformationL();
	/*const CMMFControllerImplementationInformation* ControllerImplementationInformation;
	ControllerImplementationInformation = &(iMdaPlayer->ControllerImplementationInformationL());*/
	//g=a;
	}

TInt CSimpleSoundPlayer::SetPlayWindow(const TTimeIntervalMicroSeconds &aStart, const TTimeIntervalMicroSeconds &aEnd)
	{
	iLogger.Log(_L("iMdaPlayer->SetPlayWindow() Start[%d] End[%d]"), aStart.Int64(), aEnd.Int64());
	return iMdaPlayer->SetPlayWindow(aStart, aEnd);
	}

TInt CSimpleSoundPlayer::ClearPlayWindow()
	{
	iLogger.Log(_L("iMdaPlayer->ClearPlayWindow()"));
	return iMdaPlayer->ClearPlayWindow();
	}

void CSimpleSoundPlayer::SetSupervisor(TObjectCountDown *aCounter)
	{
	iSupervisor = aCounter;
	iSupervisor->NotifyCreation();
	}

void CSimpleSoundPlayer::SetPlayCompleteTarget(MEventTarget *aTarget, CParameters *aParameters)
	{
	iEventTarget = aTarget;
	iParameters = aParameters;
	}

TInt CSimpleSoundPlayer::GetNumberOfMetaDataEntries(TInt &aNumEntries)
	{
	TInt ErrorCode = iMdaPlayer->GetNumberOfMetaDataEntries(aNumEntries);
	if (ErrorCode)
		{
		iLogger.Log(_L("Error (%d) getting number of meta data entries") , ErrorCode);
		}
	else
		{
		iLogger.Log(_L("Clip has (%d) meta data entries") , aNumEntries);
		}
	return ErrorCode;
	}

CMMFMetaDataEntry *CSimpleSoundPlayer::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	iLogger.Log(_L("Obtaining meta data with index (%d)") , aMetaDataIndex);
	return iMdaPlayer->GetMetaDataEntryL(aMetaDataIndex);
	}

TInt CSimpleSoundPlayer::ExecuteL(CParameters *aParams)
	{
    TInt newVolume = 2000;
    TInt balance=0;

    TInt priority=EMdaPriorityNormal;
	TMdaPriorityPreference aPref=EMdaPriorityPreferenceTime;

    TInt repeatNumberOfTimes=3;
    TTimeIntervalMicroSeconds SilenceDuration = (TTimeIntervalMicroSeconds ((TInt64)500000));

	switch (aParams->iAction)
		{
		case KPlayerActionPause:
		    iLogger.Log(_L("CSimpleSoundPlayer::ExecuteL "));
			Pause();
			break;
		case KPlayerActionPlay:
			Play();
			break;
		case KPlayerActionStopPlayFile:
		//	OpenL( (static_cast<CFileNameParameters*>(aParams) )->GetFileName() , ETrue);
			Stop();
			break;
		case KPlayerActionReportPosition:
			{
			TTimeIntervalMicroSeconds tmpPosition=TTimeIntervalMicroSeconds(0);
			GetPosition(tmpPosition);
			return ETrue;
			}
			//break;
		case KPlayerActionReportDuration:
			GetDuration();
			return ETrue;
			//break;
        case KplayerActionMaxVolume:
            MaxVolume();
            return ETrue;
            //break;
        case KplayerActionSetVolume:
            SetVolume(newVolume);
            return ETrue;
            //break;
        case KplayerActionSetBalance:
            SetBalance(balance);
            return ETrue;
            //break;
        case KplayerActionGetBalance:
            GetBalance(iBalance);
            return ETrue;
            //break;
        case KplayerActionGetBitRate:
        	TUint bitRate;
            GetBitRate(bitRate);
            return ETrue;
            //break;
        case KplayerActionSetPriority:
        	SetPriority(priority, aPref);
            return ETrue;
            //break;
        case KplayerActionSetRepeats:
            SetRepeats(repeatNumberOfTimes, SilenceDuration);
            return ETrue;
            //break;
        case KPlayerActionGetMetaDataEntry:
            SetVolume(newVolume);
            return ETrue;
            //break;
        case KPlayerActionGetNumberOfMetaDataEntries:
            SetVolume(newVolume);
            return ETrue;
           // break;
        case KplayerActionGetAudioLoadingProgress:
            TInt percentageProgress;
            GetAudioLoadingProgressL(percentageProgress);
            return ETrue;
            //break;
        case KplayerActionRegisterForAudioLoadingNotification:
 //       	MAudioLoadingObserver &aLoadingObserver=0;
 //       	RegisterForAudioLoadingNotification(aLoadingObserver);
            return ETrue;
            //break;
        case KplayerActionControllerImplementationInformation:
            ControllerImplementationInformationL();
            return ETrue;
           // break;
		case KPlayerActionStopPlayDes:
			OpenDesL( (static_cast<CDes8Parameters *>(aParams) )->GetDes8() , ETrue);
			break;
		case KPlayerActionReportVolume:
			TInt tmpVolume;
			GetVolume(tmpVolume);
			return ETrue;
			//break;
		case KPlayerActionStopPlayUrl:
		   CUrlParameters *p;
			 p = static_cast<CUrlParameters *>(aParams);
			 OpenUrlL(p->GetUrl() , p->GetIapId() , p->GetMimeType() );
			 break;
		case KPlayerActionStopPlayFileHandler:
			Stop();
		//	OpenFileHandlerL((static_cast<CFileNameParameters*>(aParams) )->GetFileName() , ETrue);
		//	OpenFileHandlerL( (static_cast<CFileHandlerParameters*>(aParams) )->GetFileHandler() , ETrue);
			break;
		case KPlayerActionOpen:
			OpenL( (static_cast<CFileNameParameters*>(aParams) )->GetFileName() , ETrue);
			break;

		case KPlayerActionOpenFileHandler:
			OpenFileHandlerL((static_cast<CFileNameParameters*>(aParams) )->GetFileName() , ETrue);
			break;
		}
	if (iIsLoopPlayEnabled)
	    return ETrue;
	else
	    return EFalse;
	}
