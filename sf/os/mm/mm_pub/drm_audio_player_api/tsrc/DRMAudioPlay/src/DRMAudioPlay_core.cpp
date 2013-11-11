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

// INCLUDE FILES
#include <StifTestInterface.h>
#include "DRMAudioPlay.h"
#include <caf.h>
#include <caf/supplier.h>
#include <caf/importfile.h>
#include <BAUTILS.H>

using namespace ContentAccess;

	// Constants
	const TInt CDRMAudioPlay::MAX_TESTCASE_ID = 1046;
	const TInt CDRMAudioPlay::KDefaultRepeats = 5;
	const TInt CDRMAudioPlay::KDefaultRepeatForever = -2;
	const TInt CDRMAudioPlay::KDefaultPauseDelay = 1000000;
	const TInt CDRMAudioPlay::KDefaultPlayDelay = 3000000;
	const TInt CDRMAudioPlay::KDefaultStopOpenPlayDelay = 1000000;
	const TInt CDRMAudioPlay::KDefaultReportDelay = 500000;
	
	// Taken from Oma2Agent.h
	_LIT8(KOma1XmlRoContentType, "application/vnd.oma.drm.rights+xml");

	//Cases
	const TInt CDRMAudioPlay::KCaseImplementationInformation                        = 1001;
	const TInt CDRMAudioPlay::KCasePlayFile                                         = 1002;
	const TInt CDRMAudioPlay::KCasePlayFileStopOpen                                 = 1003;
    const TInt CDRMAudioPlay::KCasePlayDes                                          = 1004;
    const TInt CDRMAudioPlay::KCasePlayDesStopOpen                                  = 1005;
    const TInt CDRMAudioPlay::KCasePlayUrl                                          = 1006;
    const TInt CDRMAudioPlay::KCasePlayUrlStopOpen                                  = 1007;
	const TInt CDRMAudioPlay::KCasePlayFileWithPause                                = 1008;
	const TInt CDRMAudioPlay::KCasePlayFileWithPosition                             = 1009;
	const TInt CDRMAudioPlay::KCasePlayFileReportPosition                           = 1010;
	const TInt CDRMAudioPlay::KCasePlayFileReportDuration                           = 1011;
    const TInt CDRMAudioPlay::KCasePlayFilesWithVolumes                             = 1012;
    const TInt CDRMAudioPlay::KCasePlayFileWithRamp                                 = 1013;
    const TInt CDRMAudioPlay::KCasePlayFileMetaInfo                                 = 1014;
    const TInt CDRMAudioPlay::KCasePlayFileWindow                                   = 1015;
    const TInt CDRMAudioPlay::KCasePlayFileClearWindow                              = 1016;
    const TInt CDRMAudioPlay::KCasePlayFileWithRepeats                              = 1017;
    const TInt CDRMAudioPlay::KCasePlayFileWithBalance                              = 1018;
    const TInt CDRMAudioPlay::KCasePlayFilesWithPriorities                          = 1019;
    const TInt CDRMAudioPlay::KCasePlayHandler                                      = 1020;
    const TInt CDRMAudioPlay::KCasePlayHandlerStopOpen                              = 1021;
    const TInt CDRMAudioPlay::KCasePlayHandlerWithPause                             = 1022;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithRamp                          = 1023;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithSetPosition                   = 1024;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithGetPosition                   = 1025;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithDuration                      = 1026;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithWindow                        = 1027;
    const TInt CDRMAudioPlay::KCasePlayFileHandlerWithClearWindow                   = 1028;
    const TInt CDRMAudioPlay::KCasePlayFileSetVolume                                = 1029;
    const TInt CDRMAudioPlay::KCasePlayFileGetBalance                               = 1030;
    const TInt CDRMAudioPlay::KCasePlayFileGetBitRate                               = 1031;
    const TInt CDRMAudioPlay::KCasePlayFileGetAudioLoadingProgress                  = 1032;
    const TInt CDRMAudioPlay::KCasePlayFileRegisterForAudioLoadingNotification      = 1033;
    const TInt CDRMAudioPlay::KCasePlayFileImplementationInformation                = 1034;
    const TInt CDRMAudioPlay::KCasePlayFileMaxVolume                                = 1035;
    const TInt CDRMAudioPlay::KCasePlayFileSetBalance                               = 1036;
    const TInt CDRMAudioPlay::KCasePlayFileSetPriority                              = 1037;
    const TInt CDRMAudioPlay::KCasePlayFileSetRepeats                               = 1038;
	const TInt CDRMAudioPlay::KCasePlayFilesGetMetaDataEntry                        = 1039;
	const TInt CDRMAudioPlay::KCasePlayFilesGetNumberOfMetaDataEntries              = 1040;
    const TInt CDRMAudioPlay::KCasePlayRepeatDRMFile                                = 1041;
    const TInt CDRMAudioPlay::KCasePlayPauseRepeatDRMFile                           = 1042;
    const TInt CDRMAudioPlay::KCasePlayRepeatDRMFileForever                         = 1043;
    const TInt CDRMAudioPlay::KCasePlayRepeatDRMFileWithPlayWindow                  = 1044;
    const TInt CDRMAudioPlay::KCasePlayPauseRepeatDRMFileWithPlayWindow             = 1045;
    const TInt CDRMAudioPlay::KCasePlayRepeatForeverDRMFileWithPlayWindow           = 1046;

// ** FilePlay *******************************************************
TInt CDRMAudioPlay::TestCasePlayFileWithPauseL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds32 PauseDelay=KDefaultPauseDelay;
	TTimeIntervalMicroSeconds32 PlayDelay=KDefaultPlayDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************************************
		TBool UsingDefault;
		PauseDelay = GetTimeIntervalL(section, KTagPauseDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving pause delay, using default"));}

		PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}

		if (PauseDelay > PlayDelay)
			{
			//Pause delay longer than play delay. Switching
			TTimeIntervalMicroSeconds32 TempDelay = PlayDelay;
			PlayDelay = PauseDelay;
			PauseDelay = TempDelay;
			}
		iLogger->Log(_L("Setting Pause and Play delays to [%d] and [%d]"), PauseDelay.Int(), PlayDelay.Int());

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}

		//*********************************************

		CParameters *pauseParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPause );
		CleanupStack::PushL(pauseParams);

		CParameters *playParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPlay );
		CleanupStack::PushL(playParams);

		CMediaEvent *mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, PlayDelay, sndPlayer, playParams);
		CMediaEvent *mEventPause = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PauseDelay, PauseDelay, sndPlayer, pauseParams, CActive::EPriorityStandard+1);

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime, EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		TTime ExpectedEnd = StartTime + sndPlayer->GetDuration() + PlayDelay - PauseDelay;
		TTime Range1 = ExpectedEnd - ErrorRange;
		TTime Range2 = ExpectedEnd + ErrorRange;

		TTimeIntervalMicroSeconds ExpectedDuration = ExpectedEnd.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Perceived duration (%d)") , PerceivedDuration.Int64() );

		iLogger->Log(_L("Expected duration  (%d)"), ExpectedDuration.Int64() );
	//	iLogger->Log(_L("Difference = (%d)") , ExpectedEnd.MicroSecondsFrom(EndTime) );

		if (mEventPlay->GetCount()<=0 || mEventPause->GetCount()<=0)
			{
			iLogger->Log( KTestCaseResultFailSkippedEvents() );
			aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
			aResult.iResult = KErrSkippedEvents;
			}
		else if (EndTime < Range1 || Range2 < EndTime)
			{	//Durations too different
			iLogger->Log(_L("The clips duration + silence duration (%d) is too different from the actual duration (%d)") , ExpectedDuration.Int64() , PerceivedDuration.Int64() );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else if (sndPlayer->iFinalError)
			{
			aResult.iResult = sndPlayer->iFinalError;
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(mEventPause);
		CleanupStack::PopAndDestroy(mEventPlay);
		CleanupStack::Pop(playParams);
		CleanupStack::Pop(pauseParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayHandlerWithPauseL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds32 PauseDelay = KDefaultPauseDelay;
	TTimeIntervalMicroSeconds32 PlayDelay = KDefaultPlayDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;

		iLogger->Log(_L("Creating simple player with file handler [%S]"), &FileNamePtr);
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));

		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);

		iFile.Close();
	//	iFs.Close();

		CleanupStack::PushL(sndPlayer);

		//*********************************************
		TBool UsingDefault;
		PauseDelay = GetTimeIntervalL(section, KTagPauseDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving pause delay, using default"));}

		PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}

		if (PauseDelay > PlayDelay)
			{
			//Pause delay longer than play delay. Switching
			TTimeIntervalMicroSeconds32 TempDelay = PlayDelay;
			PlayDelay = PauseDelay;
			PauseDelay = TempDelay;
			}
		iLogger->Log(_L("Setting Pause and Play delays to [%d] and [%d]"), PauseDelay.Int(), PlayDelay.Int());

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}

		//*********************************************

		CParameters *pauseParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPause );
		CleanupStack::PushL(pauseParams);

		CParameters *playParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPlay );
		CleanupStack::PushL(playParams);

		CMediaEvent *mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, PlayDelay, sndPlayer, playParams);
		CMediaEvent *mEventPause = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PauseDelay, PauseDelay, sndPlayer, pauseParams, CActive::EPriorityStandard+1);

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime, EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();

		iFs.Close();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		TTime ExpectedEnd = StartTime + sndPlayer->GetDuration() + PlayDelay - PauseDelay;
		TTime Range1 = ExpectedEnd - ErrorRange;
		TTime Range2 = ExpectedEnd + ErrorRange;

		TTimeIntervalMicroSeconds ExpectedDuration = ExpectedEnd.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Perceived duration (%d)") , PerceivedDuration.Int64() );

		iLogger->Log(_L("Expected duration  (%d)"), ExpectedDuration.Int64() );

	//	iLogger->Log(_L("Difference = (%d)") , ExpectedEnd.MicroSecondsFrom(EndTime) );

		if (mEventPlay->GetCount()<=0 || mEventPause->GetCount()<=0)
			{
			iLogger->Log( KTestCaseResultFailSkippedEvents() );
			aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
			aResult.iResult = KErrSkippedEvents;
			}
		else if (EndTime < Range1 || Range2 < EndTime)
			{	//Durations too different
			iLogger->Log(_L("The clips duration + silence duration (%d) is too different from the actual duration (%d)") , ExpectedDuration.Int64() , PerceivedDuration.Int64() );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else if (sndPlayer->iFinalError)
			{
			aResult.iResult = sndPlayer->iFinalError;
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(mEventPause);
		CleanupStack::PopAndDestroy(mEventPlay);
		CleanupStack::Pop(playParams);
		CleanupStack::Pop(pauseParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayFileL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
    CStifItemParser *item;
    TInt parseError = KErrNone;
    TBool Immediate = 0;
	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        item = section->GetItemLineL(KTagImmediate);
        if (item)
            {
            CleanupStack::PushL(item);
            parseError = item->GetInt(KTagImmediate, Immediate);
            CleanupStack::PopAndDestroy(item);
            }
        item = NULL;
        TBool FileType = EFalse;
        item = section->GetItemLineL(KTagFileType);
        if (item)
        	{
        	CleanupStack::PushL(item);
        	parseError = item->GetInt(KTagFileType, FileType);
        	CleanupStack::PopAndDestroy(item);
        	}

        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger, Immediate, FileType);

		CleanupStack::PushL(sndPlayer);
		iLogger->Log(_L("Starting scheduler"));

		CActiveScheduler::Start();


		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError ;

		TBool DontCheckExpectedDuration;
		TTimeIntervalMicroSeconds ErrorRange=GetTimeIntervalL(section, KTagErrorRange, DontCheckExpectedDuration);
		if (!DontCheckExpectedDuration && !aResult.iResult)
			{
			TBool DontUseExpectedDuration;
			TTimeIntervalMicroSeconds ExpectedDuration=GetTimeIntervalL(section, KTagExpectedDuration, DontUseExpectedDuration);
			if (!IsDurationInRange(0, (DontUseExpectedDuration ? sndPlayer->GetDuration() : ExpectedDuration), TTimeIntervalMicroSeconds(0), ErrorRange, sndPlayer->GetPlayStartTime(), sndPlayer->GetPlayStopTime() ) )
				{
				iLogger->Log(_L("Duration too different"));
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}

		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayHandlerL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;

	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);

		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));
	//	TRAP(err, iAudioPlayer->OpenFileL( iFile ));

	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);

		iFile.Close();
	//	iFs.Close();

		CleanupStack::PushL(sndPlayer);
		iLogger->Log(_L("Starting scheduler"));

		CActiveScheduler::Start();

		iFs.Close();
		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError ;

		TBool DontCheckExpectedDuration;
		TTimeIntervalMicroSeconds ErrorRange=GetTimeIntervalL(section, KTagErrorRange, DontCheckExpectedDuration);
		if (!DontCheckExpectedDuration && !aResult.iResult)
			{
			TBool DontUseExpectedDuration;
			TTimeIntervalMicroSeconds ExpectedDuration=GetTimeIntervalL(section, KTagExpectedDuration, DontUseExpectedDuration);
			if (!IsDurationInRange(0, (DontUseExpectedDuration ? sndPlayer->GetDuration() : ExpectedDuration), TTimeIntervalMicroSeconds(0), ErrorRange, sndPlayer->GetPlayStartTime(), sndPlayer->GetPlayStopTime() ) )
				{
				iLogger->Log(_L("Duration too different"));
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}

		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileWithRepeatsL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TInt Repeats = KDefaultRepeats;
	TTimeIntervalMicroSeconds SilenceDuration = KDefaultSilenceDuration;
	TInt ParseError=KErrNone;
	CStifItemParser *item;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		TBool DontCheckExpectedDuration;
		TTimeIntervalMicroSeconds ErrorRange=GetTimeIntervalL(section, KTagErrorRange, DontCheckExpectedDuration);

		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//Setup repeat count
		ParseError=KErrNotFound;
		item = section->GetItemLineL(KTagRepeatTimes);
		if (item)
			{
			CleanupStack::PushL(item);
			ParseError = item->GetInt(KTagRepeatTimes, Repeats);
			CleanupStack::PopAndDestroy(item);
			}
		iLogger->Log(_L("Setting up repeat count to %d"), Repeats);
		if (ParseError)
			{
			iLogger->Log(_L("Using default value of %d"), KDefaultRepeats); Repeats=KDefaultRepeats;
			}

		TBool UsingDefault;
		SilenceDuration = GetTimeIntervalL(section, KTagSilenceDuration, UsingDefault, KDefaultSilenceDuration);
		// * ****************************************************
		iLogger->Log(_L("Setting interval to %d"), SilenceDuration.Int64());

		if (UsingDefault) { iLogger->Log(_L("Using default of %d"), KDefaultSilenceDuration.Int64()) ; }
		sndPlayer->SetRepeats(Repeats, SilenceDuration);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;

		if (!DontCheckExpectedDuration && !aResult.iResult)
			{
			TBool DontUseExpectedDuration;
			TTimeIntervalMicroSeconds ExpectedDuration=GetTimeIntervalL(section, KTagExpectedDuration, DontUseExpectedDuration);
			if (!IsDurationInRange(Repeats, ( DontUseExpectedDuration ? sndPlayer->GetDuration() : ExpectedDuration), SilenceDuration, ErrorRange, sndPlayer->GetPlayStartTime(), sndPlayer->GetPlayStopTime() ) )
				{
				iLogger->Log(_L("Duration too different"));
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}

		CleanupStack::PopAndDestroy(sndPlayer);
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		return KErrExecuted;	//aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileWithPositionL(CStifSectionParser *section, TTestResult & aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds Position = KDefaultPosition;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		TBool UsingDefault;
		Position = GetTimeIntervalL(section, KTagPosition, UsingDefault, KDefaultPosition);

		iLogger->Log(_L("Seting position to: %d"), Position.Int64());
		if (UsingDefault) {iLogger->Log(_L("Using default position duration of: (%d)") , KDefaultPosition.Int64());}
		sndPlayer->iPosition = Position;

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}
		// * **************************************************************

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime,EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();



		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		iLogger->Log(_L("Perceived duration: %d") , PerceivedDuration.Int64() );


		TTimeIntervalMicroSeconds ExpectedDuration = TTimeIntervalMicroSeconds(I64INT(sndPlayer->GetDuration().Int64()) - I64INT(Position.Int64()));
		iLogger->Log(_L("ExpectedDuration: %d") ,ExpectedDuration.Int64() );

		if (  Abs(ExpectedDuration.Int64() - PerceivedDuration.Int64()) > ErrorRange.Int64() )
			{	//Durations too different
			iLogger->Log(_L("The clips duration is too different from the actual duration + position") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else
			{
			aResult.iResult = sndPlayer->iFinalError;
			if (aResult.iResult)
				{
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy(KTestCaseResultFail());
				}
			else
				{
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy(KTestCaseResultSuccess());
				}
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithSetPositionL(CStifSectionParser *section, TTestResult & aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds Position = KDefaultPosition;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));
	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		TBool UsingDefault;
		Position = GetTimeIntervalL(section, KTagPosition, UsingDefault, KDefaultPosition);

		iLogger->Log(_L("Seting position to: %d"), Position.Int64());
		if (UsingDefault) {iLogger->Log(_L("Using default position duration of: (%d)") , KDefaultPosition.Int64());}
		sndPlayer->iPosition = Position;

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}
		// * **************************************************************

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime,EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();

		iFs.Close();

		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		iLogger->Log(_L("Perceived duration: %d") , PerceivedDuration.Int64() );


		TTimeIntervalMicroSeconds ExpectedDuration = TTimeIntervalMicroSeconds(I64INT(sndPlayer->GetDuration().Int64()) - I64INT(Position.Int64()));
		iLogger->Log(_L("ExpectedDuration: %d") ,ExpectedDuration.Int64() );

		if (  Abs(ExpectedDuration.Int64() - PerceivedDuration.Int64()) > ErrorRange.Int64() )
			{	//Durations too different
			iLogger->Log(_L("The clips duration is too different from the actual duration + position") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else
			{
			aResult.iResult = sndPlayer->iFinalError;
			if (aResult.iResult)
				{
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy(KTestCaseResultFail());
				}
			else
				{
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy(KTestCaseResultSuccess());
				}
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileStopOpenL(CStifSectionParser *section , TTestResult &aResult)
	{
		TPtrC FileNamePtr;
		TTimeIntervalMicroSeconds32 StopDelay=KDefaultPauseDelay;
		TTimeIntervalMicroSeconds32 PlayDelay=KDefaultPlayDelay;

		if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
			TFileName FileName = FileNamePtr;
			iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
			CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
			CleanupStack::PushL(sndPlayer);


			CParameters *playParams;
			if ( section->GetNextLine(KTagSoundFile, FileNamePtr, ENoTag) )
			{
				playParams = CFileNameParameters::NewLC(CSimpleSoundPlayer::KPlayerActionOpen, FileName );
				iLogger->Log(_L("Second filename not found, using the same file twice"));
			}
			else
			{
				playParams = CFileNameParameters::NewLC(CSimpleSoundPlayer::KPlayerActionOpen, FileNamePtr );
				iLogger->Log(_L("Second filename set to [%S]") , &FileNamePtr );
			}

			//*********************************************
			TBool UsingDefault;
			StopDelay = GetTimeIntervalL(section, KTagStopDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
			if (UsingDefault)
			{
				iLogger->Log(_L("Error retrieving pause delay, using default"));
			}

			PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
			if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}

			if (StopDelay > PlayDelay)
			{
				//Pause delay longer than play delay. Switching
				TTimeIntervalMicroSeconds32 TempDelay = PlayDelay;
				PlayDelay = StopDelay;
				StopDelay = TempDelay;
			}

			TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
			if (UsingDefault) {iLogger->Log(_L("Using default error range"));}

			//*********************************************

			CParameters *stopParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionStopPlayFile );
			CleanupStack::PushL(stopParams);

			CMediaEvent *mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, PlayDelay, sndPlayer, playParams);
			CMediaEvent *mEventStop = CMediaEvent::NewLC(TestModuleIf() , *iLogger, StopDelay, StopDelay, sndPlayer, stopParams, CActive::EPriorityStandard+1);

			iLogger->Log(_L("Starting scheduler"));
			TTime StartTime, EndTime;
			StartTime.HomeTime();
			CActiveScheduler::Start();
			EndTime.HomeTime();

			iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
			aResult.iResult = sndPlayer->iFinalError;
			iLogger->Log(_L("Playcount: (%d)") , mEventStop->GetCount() );
			if (mEventStop->GetCount() <= 0)
			{
				aResult.iResult = KErrSkippedEvents;
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy(KTestCaseResultFailSkippedEvents());
			}
			else if (sndPlayer->iFinalError)
				{
					iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
					aResult.iResultDes.Copy(KTestCaseResultFail());
					aResult.iResult = sndPlayer->iFinalError;
				}
				else
				{
					iLogger->Log(_L("Test was successful"));
					aResult.iResultDes.Copy(KTestCaseResultSuccess());
					aResult.iResult = sndPlayer->iFinalError;
				}

			CleanupStack::PopAndDestroy(mEventStop);
			CleanupStack::PopAndDestroy(mEventPlay);
			CleanupStack::Pop(stopParams);
			CleanupStack::Pop(playParams);
			CleanupStack::PopAndDestroy(sndPlayer);
			return KErrExecuted;

		}
		else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayHandlerStopOpenL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds32 StopDelay=KDefaultStopOpenPlayDelay;
	TTimeIntervalMicroSeconds32 PlayDelay=KDefaultPlayDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
	{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));

		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);

		iFile.Close();
		CleanupStack::PushL(sndPlayer);


		CParameters *playParams;
		if ( section->GetNextLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
			playParams = CFileNameParameters::NewLC(CSimpleSoundPlayer::KPlayerActionOpenFileHandler, FileName );
			iLogger->Log(_L("Second filename not found, using the same file twice"));
		}
		else
		{
			playParams = CFileNameParameters::NewLC(CSimpleSoundPlayer::KPlayerActionOpenFileHandler, FileNamePtr );
			iLogger->Log(_L("Second filename set to [%S]") , &FileNamePtr );
		}

		//*********************************************
		TBool UsingDefault;
		StopDelay = GetTimeIntervalL(section, KTagStopDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving pause delay, using default"));}

		PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}

		if (StopDelay > PlayDelay)
		{
			//Pause delay longer than play delay. Switching
			TTimeIntervalMicroSeconds32 TempDelay = PlayDelay;
			PlayDelay = StopDelay;
			StopDelay = TempDelay;
		}

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}

		//*********************************************

		CParameters *stopParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionStopPlayFileHandler );
		CleanupStack::PushL(stopParams);

		CMediaEvent *mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, PlayDelay, sndPlayer, playParams);
		CMediaEvent *mEventStop = CMediaEvent::NewLC(TestModuleIf() , *iLogger, StopDelay, StopDelay, sndPlayer, stopParams, CActive::EPriorityStandard+1);

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime, EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();

		iFs.Close();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;
		iLogger->Log(_L("Playcount: (%d)") , mEventStop->GetCount() );
		if (mEventStop->GetCount() <= 0)
		{
			aResult.iResult = KErrSkippedEvents;
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFailSkippedEvents());
		}
		else if (sndPlayer->iFinalError)
			{
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy(KTestCaseResultFail());
				aResult.iResult = sndPlayer->iFinalError;
			}
			else
			{
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy(KTestCaseResultSuccess());
				aResult.iResult = sndPlayer->iFinalError;
			}

		CleanupStack::PopAndDestroy(mEventStop);
		CleanupStack::PopAndDestroy(mEventPlay);
		CleanupStack::Pop(stopParams);
		CleanupStack::Pop(playParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

	}
	else
	{
	aResult.iResultDes.Copy(KConfigInvalid());
	aResult.iResult = KErrNotExecuted;
	return KErrConfigInvalid;
	}
}


TInt CDRMAudioPlay::TestCasePlayFileWithBalanceL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TInt Balance = KMMFBalanceCenter;
	TInt ParseError=KErrNone;
	CStifItemParser *item;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
	//	TInt BalanceError=KErrNone;
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************
		ParseError=KErrNotFound;
		item = section->GetItemLineL(KTagBalance);
		if (item)
			{
			CleanupStack::PushL(item);
			ParseError = item->GetInt(KTagBalance, Balance);
			CleanupStack::PopAndDestroy(item);
			}
		if (ParseError)
			{
			iLogger->Log(_L("Value for Balance not found, using default (%d)") , KMMFBalanceCenter);
			Balance = KMMFBalanceCenter;
			}
		else
			{
			iLogger->Log(_L("Setting Balance to: %d"), Balance);
			sndPlayer->iBalance = Balance;
			}

		sndPlayer->iSetBalance = true;

//		iLogger->Log(_L("Setting Balance to: %d"), Balance);
//		BalanceError = sndPlayer->SetBalance(Balance);
	//	if (BalanceError)
		//	{
		//	iLogger->Log(_L("Error setting balance"));
		//	aResult.iResult = BalanceError;
		//	}
	//	else
		//	{
			iLogger->Log(_L("Starting scheduler"));
			CActiveScheduler::Start();

			iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
			aResult.iResult = sndPlayer->iFinalError;
		//	}

		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayDesL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
    CStifItemParser *item;
    TInt parseError = KErrNone;
    TBool Immediate = 0;
    TBool DesReadOnly = 0;
	if ( !section->GetLine(KTagSoundDes, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;

		HBufC8 *SoundFile = GetDescriptor8LC(FileName, section);

        item = section->GetItemLineL(KTagImmediate);
        if (item)
            {
            CleanupStack::PushL(item);
            parseError = item->GetInt(KTagImmediate, Immediate);
            CleanupStack::PopAndDestroy(item);
            }
        item = NULL;
        item = section->GetItemLineL(KTagDesReadOnly);
        if (item)
        	{
        	CleanupStack::PushL(item);
        	parseError = item->GetInt(KTagDesReadOnly, DesReadOnly);
        	CleanupStack::PopAndDestroy(item);
        	}

		iLogger->Log(_L("Creating simple player with descriptor from [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( *SoundFile, TestModuleIf() , *iLogger, Immediate, DesReadOnly);

		CleanupStack::PushL(sndPlayer);

		iLogger->Log(_L("Starting scheduler"));

		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError ;

		TBool DontCheckExpectedDuration;
		TTimeIntervalMicroSeconds ErrorRange=GetTimeIntervalL(section, KTagErrorRange, DontCheckExpectedDuration);

		if (!DontCheckExpectedDuration && !aResult.iResult)
			{
			TBool DontUseExpectedDuration;
			TTimeIntervalMicroSeconds ExpectedDuration=GetTimeIntervalL(section, KTagExpectedDuration, DontUseExpectedDuration);
			if (!IsDurationInRange(0, (DontUseExpectedDuration
						? sndPlayer->GetDuration()
						: ExpectedDuration
						), TTimeIntervalMicroSeconds(0), ErrorRange, sndPlayer->GetPlayStartTime(), sndPlayer->GetPlayStopTime() ) )
				{
				iLogger->Log(_L("Duration too different"));
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}


		CleanupStack::PopAndDestroy(sndPlayer);

		CleanupStack::PopAndDestroy(SoundFile);

		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;

		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayFileReportPositionL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		TBool UsingDefaultReportDelay;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************************************
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
		//*********************************************

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithGetPositionL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		TBool UsingDefaultReportDelay;
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		//*********************************************
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
		//*********************************************

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iFs.Close();

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		return aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileReportDurationL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool ExpectedDurationNotSet = EFalse, UsingDefaultReportDelay;;
		TTimeIntervalMicroSeconds ExpectedDuration;

		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************************************
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
		//*********************************************
		ExpectedDuration = GetTimeIntervalL( section, KTagExpectedDuration, ExpectedDurationNotSet) ;
		//*********************************************

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportDuration);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

		aResult.iResult = sndPlayer->iFinalError;

		if (mEventReportPosition->GetCount() <= 0)
			{
			aResult.iResult = KErrSkippedEvents;
			}

		if (!ExpectedDurationNotSet)
			{
			iLogger->Log(_L("Expected duration: (%d)") , ExpectedDuration.Int64() );

			if ( ExpectedDuration!=sndPlayer->GetDuration() )
				{
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}

		switch (aResult.iResult)
			{
			case KErrNone:
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy( KTestCaseResultSuccess() );
				break;
			case KErrExpectedValueDifferent:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
				break;
			case KErrSkippedEvents:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
				break;
			default:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFail() );
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);

		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrNone;	//aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayFileMaxVolumeL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionMaxVolume);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventMaxVolume = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventMaxVolume->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }


        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventMaxVolume);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayFileSetVolumeL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionSetVolume);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventSetVolume = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventSetVolume->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventSetVolume);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }




TInt CDRMAudioPlay::TestCasePlayFileGetBalanceL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionGetBalance);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetBalance = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetBalance->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetBalance);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }


TInt CDRMAudioPlay::TestCasePlayFileSetBalanceL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionSetBalance);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventSetBalance = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventSetBalance->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventSetBalance);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }


TInt CDRMAudioPlay::TestCasePlayFileSetPriorityL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionSetPriority);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetBalance = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetBalance->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetBalance);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }



TInt CDRMAudioPlay::TestCasePlayFileSetRepeatsL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        sndPlayer->ControllerImplementationInformationL();
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        //CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionSetRepeats);
        //CleanupStack::PushL(reportParams);
        //CMediaEvent *mEventSetRepeats = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        /*if (mEventSetRepeats->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }*/

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        //CleanupStack::PopAndDestroy(mEventSetRepeats);
        //CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }


TInt CDRMAudioPlay::TestCasePlayFilesGetMetaDataEntryL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionGetBalance);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetBalance = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetBalance->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetBalance);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }



TInt CDRMAudioPlay::TestCasePlayFilesGetNumberOfMetaDataEntriesL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionGetBalance);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetBalance = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetBalance->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetBalance);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }



TInt CDRMAudioPlay::TestCasePlayFileGetBitRateL(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionGetBitRate);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetBitRate = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetBitRate->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetBitRate);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }



TInt CDRMAudioPlay::TestCasePlayFilesImplementationInformation(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionControllerImplementationInformation);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventImplementationInformation = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventImplementationInformation->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventImplementationInformation);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayFilesRegisterForAudioLoadingNotification(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionRegisterForAudioLoadingNotification);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventRegisterForAudioLoadingNotification = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventRegisterForAudioLoadingNotification->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventRegisterForAudioLoadingNotification);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayFilesGetAudioLoadingProgress(CStifSectionParser *section , TTestResult &aResult)
    {
    TPtrC FileNamePtr;
    TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

    if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
        {
        TBool UsingDefaultReportDelay;;
        TFileName FileName = FileNamePtr;
        iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);

        //###############################################
        ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
        iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
        //###############################################

        CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KplayerActionGetAudioLoadingProgress);
        CleanupStack::PushL(reportParams);
        CMediaEvent *mEventGetAudioLoadingProgress = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();

        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

        aResult.iResult = sndPlayer->iFinalError;

        if (mEventGetAudioLoadingProgress->GetCount() <= 0)
            {
            aResult.iResult = KErrSkippedEvents;
            }

        switch (aResult.iResult)
            {
            case KErrNone:
                iLogger->Log(_L("Test was successful"));
                aResult.iResultDes.Copy( KTestCaseResultSuccess() );
                break;
            case KErrExpectedValueDifferent:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
                break;
            case KErrSkippedEvents:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
                break;
            default:
                iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
                aResult.iResultDes.Copy( KTestCaseResultFail() );
            }

        CleanupStack::PopAndDestroy(mEventGetAudioLoadingProgress);
        CleanupStack::Pop(reportParams);

        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrNone;    //aResult.iResult;

        }
    else
        {
        //The file name of the clip to play is missing
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }


//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithDurationL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool ExpectedDurationNotSet = EFalse, UsingDefaultReportDelay;;
		TTimeIntervalMicroSeconds ExpectedDuration;

		TFileName FileName = FileNamePtr;
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		//*********************************************
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());
		//*********************************************
		ExpectedDuration = GetTimeIntervalL( section, KTagExpectedDuration, ExpectedDurationNotSet) ;
		//*********************************************

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportDuration);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();
		iFs.Close();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

		aResult.iResult = sndPlayer->iFinalError;

		if (mEventReportPosition->GetCount() <= 0)
			{
			aResult.iResult = KErrSkippedEvents;
			}

		if (!ExpectedDurationNotSet)
			{
			iLogger->Log(_L("Expected duration: (%d)") , ExpectedDuration.Int64() );

			if ( ExpectedDuration!=sndPlayer->GetDuration() )
				{
				aResult.iResult = KErrExpectedValueDifferent;
				}
			}

		switch (aResult.iResult)
			{
			case KErrNone:
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy( KTestCaseResultSuccess() );
				break;
			case KErrExpectedValueDifferent:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailExpectedValueDifferent() );
				break;
			case KErrSkippedEvents:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
				break;
			default:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFail() );
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);

		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrNone;	//aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileWindowL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds StartPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds EndPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
	TBool UsingDefault;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool UsingStartDefault, UsingEndDefault, UsingDefaultReportDelay;
		TFileName FileName = FileNamePtr;
	//	TInt WindowError;

		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************
		StartPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
		EndPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}


		sndPlayer->iStartPosition = StartPosition;
		sndPlayer->iEndPosition = EndPosition;
		sndPlayer->iPlayWindow = true;

		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime,EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();

		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		iLogger->Log(_L("Perceived duration: %d") , PerceivedDuration.Int64() );


		TTimeIntervalMicroSeconds ExpectedDuration = TTimeIntervalMicroSeconds(I64INT(EndPosition.Int64()) - I64INT(StartPosition.Int64()));
		iLogger->Log(_L("ExpectedDuration: %d") ,ExpectedDuration.Int64() );

		if (  Abs(ExpectedDuration.Int64() - PerceivedDuration.Int64()) > ErrorRange.Int64() )
			{	//Durations too different
			iLogger->Log(_L("The clips duration is too different from the actual duration + position") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else if (aResult.iResult = sndPlayer->iFinalError , aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithWindowL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds StartPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds EndPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
	TBool UsingDefault;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool UsingStartDefault, UsingEndDefault, UsingDefaultReportDelay;
		TFileName FileName = FileNamePtr;
	//	TInt WindowError;
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		//*********************
		StartPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
		EndPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));

		TTimeIntervalMicroSeconds ErrorRange = GetTimeIntervalL(section, KTagErrorRange, UsingDefault, KDefaultErrorRange);
		if (UsingDefault) {iLogger->Log(_L("Using default error range"));}


		sndPlayer->iStartPosition = StartPosition;
		sndPlayer->iEndPosition = EndPosition;
		sndPlayer->iPlayWindow = true;

		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		TTime StartTime,EndTime;
		StartTime.HomeTime();
		CActiveScheduler::Start();
		EndTime.HomeTime();
		iFs.Close();
		TTimeIntervalMicroSeconds PerceivedDuration = EndTime.MicroSecondsFrom( StartTime );

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		iLogger->Log(_L("Perceived duration: %d") , PerceivedDuration.Int64() );


		TTimeIntervalMicroSeconds ExpectedDuration = TTimeIntervalMicroSeconds(I64INT(EndPosition.Int64()) - I64INT(StartPosition.Int64()));
		iLogger->Log(_L("ExpectedDuration: %d") ,ExpectedDuration.Int64() );

		if (  Abs(ExpectedDuration.Int64() - PerceivedDuration.Int64()) > ErrorRange.Int64() )
			{	//Durations too different
			iLogger->Log(_L("The clips duration is too different from the actual duration + position") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			}
		else if (aResult.iResult = sndPlayer->iFinalError , aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileWindowClearL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds StartPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds EndPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool UsingStartDefault, UsingEndDefault, UsingDefaultReportDelay;
		TFileName FileName = FileNamePtr;

		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************
		StartPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
		EndPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));

		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		sndPlayer->iStartPosition = StartPosition;
		sndPlayer->iEndPosition = EndPosition;
		sndPlayer->iPlayWindow = true;
		sndPlayer->iClearWindow = true;


		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		TBool PositionReset=ETrue;
		TTimeIntervalMicroSeconds position;
		TInt PosError = sndPlayer->GetPosition(position);
		if ( !PosError && (position!=TTimeIntervalMicroSeconds(0) ) )
			{
			iLogger->Log(_L("Position not reset to zero, position is (%d)") , position.Int64() );
			PositionReset=EFalse;
			}
		if (PosError)
			{
			iLogger->Log(_L("Error (%d) obtaining clip position"), PosError );
			}


		if (!PositionReset)
			{
			iLogger->Log(_L("Error clearing PlayWindow, position not reset to zero") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFail() );
			}
		else if (aResult.iResult = sndPlayer->iFinalError , aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithClearWindowL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds StartPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds EndPosition = KDefaultPosition;
	TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TBool UsingStartDefault, UsingEndDefault, UsingDefaultReportDelay;
		TFileName FileName = FileNamePtr;
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));

		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
	//	CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		//*********************
		StartPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
		EndPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));

		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultReportDelay, (TTimeIntervalMicroSeconds32)KDefaultReportDelay);
		iLogger->Log(_L("Setting delays to report position to [%d]"), ReportDelay.Int());

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportPosition);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		sndPlayer->iStartPosition = StartPosition;
		sndPlayer->iEndPosition = EndPosition;
		sndPlayer->iPlayWindow = true;
		sndPlayer->iClearWindow = true;


		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iFs.Close();
		TBool PositionReset=ETrue;
		TTimeIntervalMicroSeconds position;
		TInt PosError = sndPlayer->GetPosition(position);
		if ( !PosError && (position!=TTimeIntervalMicroSeconds(0) ) )
			{
			iLogger->Log(_L("Position not reset to zero, position is (%d)") , position.Int64() );
			PositionReset=EFalse;
			}
		if (PosError)
			{
			iLogger->Log(_L("Error (%d) obtaining clip position"), PosError );
			}


		if (!PositionReset)
			{
			iLogger->Log(_L("Error clearing PlayWindow, position not reset to zero") );
			aResult.iResult = KErrExpectedValueDifferent;
			aResult.iResultDes.Copy(KTestCaseResultFail() );
			}
		else if (aResult.iResult = sndPlayer->iFinalError , aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayDesStopOpenL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;

	if ( !section->GetLine(KTagSoundDes, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with descriptor from [%S]"), &FileName);
		//*********************************************
		//Open descriptor
		HBufC8 *SoundFile = GetDescriptor8LC(FileName, section);
		HBufC8 *SoundFile2;

		//*********************************************
		//FileName of second file to play
		CParameters *stopParams ;
		if ( section->GetNextLine(KTagSoundDes, FileNamePtr, ENoTag) )
			{	//Using same filename
			SoundFile2 = SoundFile->AllocLC();
			stopParams = CDes8Parameters::NewLC(CSimpleSoundPlayer::KPlayerActionStopPlayDes, *SoundFile2 );
			iLogger->Log(_L("Second descriptor filename not found, using the same file twice"));
			}
		else
			{	//Found second filename
			FileName=FileNamePtr;
			SoundFile2 = GetDescriptor8LC(FileName, section);
			stopParams = CDes8Parameters::NewLC(CSimpleSoundPlayer::KPlayerActionStopPlayDes, *SoundFile2 );
			iLogger->Log(_L("Second descriptor from [%S]") , &FileName );
			}

		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewLC( *SoundFile, TestModuleIf() , *iLogger);

		TBool UsingDefault;
		TTimeIntervalMicroSeconds32 StopDelay = GetTimeIntervalL(section, KTagDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultStopOpenPlayDelay);
		if (UsingDefault) {iLogger->Log(_L("Error retrieving stop delay, using default"));}
		iLogger->Log(_L("Setting delays to stop and open descriptor to [%d]"), StopDelay.Int());
		//*********************************************

		CMediaEvent *mEventStopPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, StopDelay, StopDelay, sndPlayer, stopParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );

		//]TBool DontCheckExpectedDuration;
		//]TTimeIntervalMicroSeconds ErrorRange=GetTimeIntervalL(section, KTagErrorRange, DontCheckExpectedDuration);
		//]TBool DontUseExpectedDuration;
		//]TTimeIntervalMicroSeconds ExpectedDuration=GetTimeIntervalL(section, KTagExpectedDuration, DontUseExpectedDuration);

		if (mEventStopPlay->GetCount() <= 0)
			{
			aResult.iResult = KErrSkippedEvents;
			aResult.iResultDes.Copy(KTestCaseResultFailSkippedEvents);
			iLogger->Log(KTestCaseResultFailSkippedEvents() );
			}
		else if ( sndPlayer->iFinalError)
			{
			aResult.iResult = sndPlayer->iFinalError;
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail() );
			}
		else
			{
			//]aResult.iResult = KErrNone;
			//]if (!DontCheckExpectedDuration && !DontUseExpectedDuration)
			//]	{
			//]	if (!IsDurationInRange(0, ExpectedDuration, TTimeIntervalMicroSeconds(0), ErrorRange, sndPlayer->GetPlayStartTime(), sndPlayer->GetPlayStopTime() ) )
			//]		{
			//]		iLogger->Log(_L("Duration too different"));
			//]		aResult.iResult = KErrExpectedValueDifferent;
			//]		}
			//]	}
			//]if (aResult.iResult)
			//]	{
			//]	aResult.iResultDes.Copy(KTestCaseResultFailExpectedValueDifferent());
			//]	}
			//]else
			//]	{
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy(KTestCaseResultSuccess());
			//]	}
			}

		CleanupStack::PopAndDestroy(mEventStopPlay);
		CleanupStack::PopAndDestroy(sndPlayer);
		CleanupStack::Pop(stopParams);
		CleanupStack::PopAndDestroy(SoundFile2);
		CleanupStack::PopAndDestroy(SoundFile);
		return KErrExecuted;
		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFileWithVolumeRampL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	TTimeIntervalMicroSeconds RampDuration = KDefaultRampDuration;
	TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CleanupStack::PushL(sndPlayer);

		//*********************
		TBool UsingReportDefault, UsingRampDefault;
		RampDuration = GetTimeIntervalL(section, KTagRampDuration, UsingRampDefault, KDefaultRampDuration);
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingReportDefault, (TTimeIntervalMicroSeconds32)KDefaultReportDelay );

		// ******************************************
		iLogger->Log(_L("Seting up volume ramp with duration: %d"), RampDuration.Int64());
		if (UsingRampDefault) {iLogger->Log(_L("Using default Ramp duration of: %d") , KDefaultRampDuration.Int64());}

		sndPlayer->SetVolumeRamp(RampDuration);

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportVolume);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;

		if (mEventReportPosition->GetCount() <= 0)
			{
			aResult.iResult = KErrSkippedEvents;
			}

		switch (aResult.iResult)
			{
			case KErrNone:
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy( KTestCaseResultSuccess() );
				break;
			case KErrSkippedEvents:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
				break;
			default:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFail() );
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

//FH
TInt CDRMAudioPlay::TestCasePlayHandlerWithRampL(CStifSectionParser *section , TTestResult &aResult)
	{

	TPtrC FileNamePtr;
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();

	TTimeIntervalMicroSeconds RampDuration = KDefaultRampDuration;
	TTimeIntervalMicroSeconds32 ReportDelay=KDefaultReportDelay;

	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		iLogger->Log(_L("Creating simple player with file [%S]"), &FileNamePtr);
		User::LeaveIfError(iFile.Open( iFs, FileName, EFileRead));

		//CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( FileName, TestModuleIf() , *iLogger);
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
		iFile.Close();
		CleanupStack::PushL(sndPlayer);

		//*********************
		TBool UsingReportDefault, UsingRampDefault;
		RampDuration = GetTimeIntervalL(section, KTagRampDuration, UsingRampDefault, KDefaultRampDuration);
		ReportDelay = GetTimeIntervalL(section, KTagDelay, UsingReportDefault, (TTimeIntervalMicroSeconds32)KDefaultReportDelay );

		// ******************************************
		iLogger->Log(_L("Seting up volume ramp with duration: %d"), RampDuration.Int64());
		if (UsingRampDefault) {iLogger->Log(_L("Using default Ramp duration of: %d") , KDefaultRampDuration.Int64());}

		sndPlayer->SetVolumeRamp(RampDuration);

		CParameters *reportParams = new(ELeave)CParameters(CSimpleSoundPlayer::KPlayerActionReportVolume);
		CleanupStack::PushL(reportParams);
		CMediaEvent *mEventReportPosition = CMediaEvent::NewLC(TestModuleIf() , *iLogger, ReportDelay, ReportDelay, sndPlayer, reportParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();
		iFs.Close();
		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;

		if (mEventReportPosition->GetCount() <= 0)
			{
			aResult.iResult = KErrSkippedEvents;
			}

		switch (aResult.iResult)
			{
			case KErrNone:
				iLogger->Log(_L("Test was successful"));
				aResult.iResultDes.Copy( KTestCaseResultSuccess() );
				break;
			case KErrSkippedEvents:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFailSkippedEvents() );
				break;
			default:
				iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
				aResult.iResultDes.Copy( KTestCaseResultFail() );
			}

		CleanupStack::PopAndDestroy(mEventReportPosition);
		CleanupStack::Pop(reportParams);
		CleanupStack::PopAndDestroy(sndPlayer);
		return aResult.iResult;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}


TInt CDRMAudioPlay::TestCasePlayFilesWithPrioritiesL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TInt PriorityLow = EMdaPriorityNormal+1;
		TInt PriorityHigh = EMdaPriorityNormal+20;

		TBool UsingDefaultPriority;
		PriorityHigh = GetIntL(section, KTagPriority, UsingDefaultPriority, PriorityHigh);
		PriorityLow = GetIntL(section, KTagPriority, UsingDefaultPriority, PriorityLow, ETrue);

		if (PriorityHigh < PriorityLow)
			{
			TInt tempPriority = PriorityHigh;
			PriorityHigh = PriorityLow;
			PriorityLow = tempPriority;
			}

		TBool UsingDefaultPriorityPreference;
		TInt tmpPrefHigh = GetIntL(section, KTagPriorityPreference, UsingDefaultPriorityPreference, -1);
		TInt tmpPrefLow = GetIntL(section, KTagPriorityPreference, UsingDefaultPriorityPreference, tmpPrefHigh, ETrue);

		TMdaPriorityPreference prefHigh;
		switch (tmpPrefHigh)
			{
			case 0:
				prefHigh = EMdaPriorityPreferenceNone; break;
			case 1:
				prefHigh = EMdaPriorityPreferenceTime; break;
			case 2:
				prefHigh = EMdaPriorityPreferenceQuality; break;
			case 3:
			default:
				prefHigh = EMdaPriorityPreferenceTimeAndQuality; break;
			}
		TMdaPriorityPreference prefLow;
		switch (tmpPrefLow)
			{
			case 0:
				prefLow = EMdaPriorityPreferenceNone; break;
			case 1:
				prefLow = EMdaPriorityPreferenceTime; break;
			case 2:
				prefLow = EMdaPriorityPreferenceQuality; break;
			case 3:
			default:
				prefLow = EMdaPriorityPreferenceTimeAndQuality; break;
			}

		TFileName FileName = FileNamePtr;
		TObjectCountDown counter;

		iLogger->Log(_L("Creating playerHigh with file [%S]"), &FileName);
		CSimpleSoundPlayer *sndPlayerHigh = CSimpleSoundPlayer::NewLC( FileName, TestModuleIf() , *iLogger);

		if ( section->GetNextLine(KTagSoundFile, FileNamePtr, ENoTag) )
			{
			iLogger->Log(_L("Second filename not found, using the same file twice"));
			}
		else
			{
			FileName=FileNamePtr;
			iLogger->Log(_L("Second filename set to [%S]") , &FileName );
			}

		// ***************
		iLogger->Log(_L("Creating playerLow with file [%S]"), &FileName);
		CSimpleSoundPlayer *sndPlayerLow = CSimpleSoundPlayer::NewLC( FileName, TestModuleIf() , *iLogger);

		iLogger->Log(_L("Setting playerLow priority to (%d)"), PriorityLow);
		sndPlayerLow->SetPriority( PriorityLow, prefLow);

		iLogger->Log(_L("Setting playerHigh priority to (%d)"), PriorityHigh);
		sndPlayerHigh->SetPriority( PriorityHigh, prefHigh);

		sndPlayerLow->SetSupervisor(&counter);
		sndPlayerHigh->SetSupervisor(&counter);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from playerHigh, errorcode: (%d)"), sndPlayerHigh->iFinalError );
		iLogger->Log(_L("Returned from playerLow, errorcode: (%d)"), sndPlayerLow->iFinalError );

		aResult.iResult = sndPlayerHigh->iFinalError ;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayerLow);
		CleanupStack::PopAndDestroy(sndPlayerHigh);
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayFilesWithVolumesL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC FileNamePtr;
	if ( !section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) )
		{
		TFileName FileName = FileNamePtr;
		TObjectCountDown counter;

		CSimpleSoundPlayer *sndPlayerHigh = CSimpleSoundPlayer::NewLC( FileName, TestModuleIf() , *iLogger);
	//	TInt VolumeHigh;
	//	sndPlayerHigh->SetVolume(VolumeHigh=sndPlayerHigh->MaxVolume());
	//	iLogger->Log(_L("Creating player 1 (playerHigh) with file [%S] and volume (%d), max=(%d)"), &FileName, VolumeHigh, sndPlayerHigh->MaxVolume() );
		iLogger->Log(_L("Creating player 1 (playerHigh) with file [%S] "), &FileName);

		sndPlayerHigh->iPlayVolume = true;

		if ( section->GetNextLine(KTagSoundFile, FileNamePtr, ENoTag) )
			{
			iLogger->Log(_L("Second filename not found, using the same file twice"));
			}
		else
			{
			FileName=FileNamePtr;
			iLogger->Log(_L("Second filename set to [%S]") , &FileName );
			}

		// ***************
		CSimpleSoundPlayer *sndPlayerLow = CSimpleSoundPlayer::NewLC( FileName , TestModuleIf() , *iLogger , EFalse);
	//	TInt VolumeLow;
	//	sndPlayerLow->SetVolume(VolumeLow=sndPlayerLow->MaxVolume()/2);
	//	iLogger->Log(_L("Creating player 2 (playerLow) with file [%S] and volume (%d), max=(%d)"), &FileName, VolumeLow, sndPlayerLow->MaxVolume() );
		iLogger->Log(_L("Creating player 2 (playerHigh) with file [%S] "), &FileName);
		sndPlayerLow->iPlayVolume = true;

		CParameters *mPlayParameters = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPlay );
		CleanupStack::PushL(mPlayParameters);

		sndPlayerHigh->SetPlayCompleteTarget(sndPlayerLow, mPlayParameters);

		sndPlayerLow->SetSupervisor(&counter);
		sndPlayerHigh->SetSupervisor(&counter);

		iLogger->Log(_L("Starting scheduler"));


		CActiveScheduler::Start();


		iLogger->Log(_L("Returned from playerHigh, errorcode: (%d)"), sndPlayerHigh->iFinalError );
		iLogger->Log(_L("Returned from playerLow, errorcode: (%d)"), sndPlayerLow->iFinalError );

		aResult.iResult = sndPlayerHigh->iFinalError ;

		if (!sndPlayerHigh->iFinalError && !sndPlayerLow->iFinalError)
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			aResult.iResult = KErrNone;
			}
		else
			{
			if (sndPlayerHigh->iFinalError)
				{
				aResult.iResult = sndPlayerHigh->iFinalError;
				}
			if (sndPlayerLow->iFinalError)
				{
				aResult.iResult = sndPlayerLow->iFinalError;
				}

			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		CleanupStack::PopAndDestroy(mPlayParameters);
		CleanupStack::PopAndDestroy(sndPlayerLow);
		CleanupStack::PopAndDestroy(sndPlayerHigh);
		return KErrExecuted;
		}
	else
		{
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayUrlL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC UrlPtr;
	if ( !section->GetLine(KTagSoundUrl, UrlPtr, ENoTag) )
		{
		CStifItemParser *item;
		HBufC *Url = UrlPtr.AllocLC();
		TInt IapId = KUseDefaultIap;
		TPtrC MimePtr;
		HBufC *Mime;
		HBufC8 *Mime8;
		if ( ! section->GetLine(KTagMime, MimePtr, ENoTag) )
			{
			Mime = MimePtr.AllocLC();
			}
		else
			{
			Mime = KNullDesC().AllocLC();
			}

		if ( item=section->GetItemLineL(KTagIapId) , item)
			{
			CleanupStack::PushL(item);
			TInt newVal;
			if ( ! item->GetInt(KTagIapId, newVal) )
				{
				IapId = newVal;
				}
			CleanupStack::PopAndDestroy(item);
			}

		Mime8 = HBufC8::NewLC( Mime->Length() );
		TPtr8 Mime8Ptr = Mime8->Des();

		for (TInt x=0 ; x < Mime->Length() ; x++)
			{
			//TUint8 tmp8 = (TUint8)( (*Mime)[x]);
			//TBufC8<10> tmp( &tmp8 );
			TUint8 tmp8Array[2] = {0, 0};
			//tmp8Array[1]=0;
			tmp8Array[0]=(TUint8)( (*Mime)[x] );
			TBufC8<10> tmp( tmp8Array );
			Mime8Ptr += tmp.Left(1);
			}

		iLogger->Log(_L("Creating simple player with url [%S], IapId [%d] "), Url, IapId);
		iLogger->Log(_L8("   and MIME Type [%S]L(%d)") , Mime8, Mime8->Length() );
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewUrlLC( *Url, IapId, *Mime8, TestModuleIf() , *iLogger);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError ;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}
		CleanupStack::PopAndDestroy(sndPlayer);
		CleanupStack::PopAndDestroy(Mime8);
		CleanupStack::PopAndDestroy(Mime);
		CleanupStack::PopAndDestroy(Url);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

TInt CDRMAudioPlay::TestCasePlayUrlStopOpenL(CStifSectionParser *section , TTestResult &aResult)
	{
	TPtrC UrlPtr;
	TTimeIntervalMicroSeconds32 StopDelay=KDefaultStopOpenPlayDelay;

	if ( !section->GetLine(KTagSoundUrl, UrlPtr, ENoTag) )
		{
		CStifItemParser *item;
		TInt IapId = KUseDefaultIap;
		TInt IapIdB = KUseDefaultIap;

		TPtrC MimePtr;
		HBufC *Mime;
		HBufC8 *Mime8;

		HBufC *MimeB;
		HBufC8 *Mime8B;

		HBufC *Url = UrlPtr.AllocLC();
		if ( ! section->GetLine(KTagMime, MimePtr, ENoTag) )
			{
			Mime = MimePtr.AllocLC();
			}
		else
			{
			Mime = KNullDesC().AllocLC();
			}

		Mime8 = HBufC8::NewLC( Mime->Length() );
		TPtr8 Mime8Ptr = Mime8->Des();
		TInt x;
		for (x=0 ; x < Mime->Length() ; x++)
			{
			TUint8 tmp8[2];
			tmp8[0] = (TUint8)( (*Mime)[x]);
			tmp8[1] = '\0';
			TBufC8<10> tmp( tmp8 );
			Mime8Ptr += tmp.Left(1);
			}

		if ( item=section->GetItemLineL(KTagIapId) , item)
			{
			CleanupStack::PushL(item);
			TInt newVal;
			if ( ! item->GetInt(KTagIapId, newVal) )
				{
				IapId = newVal;
				}
			CleanupStack::PopAndDestroy(item);
			}

		iLogger->Log(_L("Creating simple player with url [%S], IapId [%d] "), Url, IapId);
		iLogger->Log(_L8("   and MIME Type [%S]L(%d)") , Mime8, Mime8->Length() );
		CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewUrlLC( *Url, IapId, *Mime8, TestModuleIf() , *iLogger);

		//*********************************************
		//Url of second file to play
		HBufC *UrlB;
		if ( ! section->GetLine(KTagMime, MimePtr, ENoTag) )
			{
			MimeB = MimePtr.AllocLC();
			}
		else
			{
			MimeB = KNullDesC().AllocLC();
			}

		Mime8B = HBufC8::NewLC( MimeB->Length() );
		TPtr8 Mime8PtrB = Mime8B->Des();
		for (x=0 ; x < MimeB->Length() ; x++)
			{
			TUint8 tmp8B = (TUint8)( (*MimeB)[x]);
			TBufC8<10> tmpB( &tmp8B );
			Mime8PtrB += tmpB.Left(1);
			}

		if ( item=section->GetItemLineL(KTagIapId) , item)
		{
			CleanupStack::PushL(item);
			TInt newValB;
			if ( ! item->GetInt(KTagIapId, newValB) )
				{
				IapIdB = newValB;
				}
			CleanupStack::PopAndDestroy(item);
		}

		CUrlParameters *stopParams ;
		if ( section->GetNextLine(KTagSoundUrl, UrlPtr, ENoTag) )
			{
			iLogger->Log(_L("Second url not found, using the same file twice"));
			UrlB = Url->AllocLC();
			}
		else
			{
			iLogger->Log(_L("Second url set to [%S]") , &UrlPtr );
			UrlB = UrlPtr.AllocLC();
			}

		stopParams = new(ELeave) CUrlParameters (CSimpleSoundPlayer::KPlayerActionStopPlayUrl, *UrlB, IapIdB, *Mime8B);
		CleanupStack::PushL(stopParams);

		TBool UsingDefaultDelay=EFalse;
		StopDelay = GetTimeIntervalL(section, KTagDelay, UsingDefaultDelay, (TTimeIntervalMicroSeconds32)KDefaultStopOpenPlayDelay);

		iLogger->Log(_L("Setting delays to stop and open url to [%d]"), StopDelay.Int());
		//*********************************************

		CMediaEvent *mEventStopPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, StopDelay, StopDelay, sndPlayer, stopParams);

		iLogger->Log(_L("Starting scheduler"));
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(mEventStopPlay);
		CleanupStack::Pop(stopParams);

		iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
		aResult.iResult = sndPlayer->iFinalError;
		if (aResult.iResult)
			{
			iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
			aResult.iResultDes.Copy(KTestCaseResultFail());
			}
		else
			{
			iLogger->Log(_L("Test was successful"));
			aResult.iResultDes.Copy(KTestCaseResultSuccess());
			}

		CleanupStack::PopAndDestroy(UrlB);
		CleanupStack::PopAndDestroy(Mime8B);
		CleanupStack::PopAndDestroy(MimeB);
		CleanupStack::PopAndDestroy(sndPlayer);

		CleanupStack::PopAndDestroy(Mime8);
		CleanupStack::PopAndDestroy(Mime);

		CleanupStack::PopAndDestroy(Url);
		return KErrExecuted;

		}
	else
		{
		//The file name of the clip to play is missing
		aResult.iResultDes.Copy(KConfigInvalid());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
		}
	}

// ** Module *****************************************************************
HBufC8 *CDRMAudioPlay::GetDescriptor8LC(const TFileName &aFileName, CStifSectionParser *section)
	{
	TInt FileError, SoundSize=0;
	RFile file;
	RFs FsSession;
	HBufC8 *SoundFile;


	TInt MimeUsed = 0;
	CStifItemParser *item;
	item = section->GetItemLineL(KTagMime);
	if (item)
	{
		CleanupStack::PushL(item);
		item->GetInt(KTagMime, MimeUsed) ;
		CleanupStack::PopAndDestroy(item);
	}

	iLogger->Log(_L("Connecting to File Server Session") );
	FileError = FsSession.Connect();
	if (FileError)
		{
		iLogger->Log(_L("File error FsSession.Connect() (%d)") , FileError);
		User::LeaveIfError(FileError);
		}

	iLogger->Log(_L("Opening file to get size") );
	FileError = file.Open(FsSession, aFileName, EFileStream);
	if (FileError)
		{
		iLogger->Log(_L("File error file.Open() (%d)") , FileError);
		User::LeaveIfError(FileError);
		}
	FileError = file.Size(SoundSize);
	iLogger->Log(_L("File Size (%d)") , SoundSize);
	if (FileError)
		{
		iLogger->Log(_L("Error getting size") );
		User::LeaveIfError(FileError);
		}

//	TPtr8 helper;

	if (MimeUsed == 1)
	{
		_LIT(MP3MIME,"zzaudio/mpeg");
		SoundFile = HBufC8::NewLC(SoundSize+12);

		TPtr8 helper(SoundFile->Des());
		helper.Append (MP3MIME);
		helper.AppendFill (0x00, 1);
		TPtr8 helper1(&helper[12], SoundSize);
		User::LeaveIfError(file.Read(helper1, SoundSize));
		helper.SetLength (SoundSize+12);




	}
	else
	{
		SoundFile = HBufC8::NewLC(SoundSize);

	//	TPtr8 helper = SoundFile->Des();
		TPtr8 helper(SoundFile->Des());
	//	FileError = file.Read( helper );
		User::LeaveIfError(file.Read(helper, SoundSize));


	}

	iLogger->Log(_L("Descriptor length [%d]") , SoundFile->Size() );
	file.Close();
	FsSession.Close();
	return SoundFile;
	}

TTimeIntervalMicroSeconds CDRMAudioPlay::GetTimeIntervalL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefaults, const TTimeIntervalMicroSeconds &aDefaultInterval)
	{
	TInt TempInterval1=0 , TempInterval2=0;
	TInt64 tmp=0;
	TInt ParseError=KErrNotFound;
	TTimeIntervalMicroSeconds ReturnValue = aDefaultInterval;
	aUsingDefaults = ETrue;

	CStifItemParser *item = aSection->GetItemLineL(aTag);
	if (item)
		{
		CleanupStack::PushL(item);
		ParseError = item->GetInt(aTag, TempInterval1) ;
		if (!ParseError)
			{
			//First delay found.
			aUsingDefaults = EFalse;
			tmp = TempInterval1*1000000;	//convert from seconds to microseconds

			if ( ! item->GetNextInt(TempInterval2) )
				{
				tmp += TempInterval2*1000;	//add miliseconds
				if ( ! item->GetNextInt(TempInterval2) )
					{
					tmp += TempInterval2;
					}
				}
			ReturnValue = tmp;
			}
		CleanupStack::PopAndDestroy(item);
		}
	if (ParseError) {iLogger->Log(_L("Error retrieving interval, [%S] not found"), &aTag);}
	iLogger->Log(_L("Interval retrieved: (%d)") , ReturnValue.Int64());
	return ReturnValue;
	}

	/*TTimeIntervalMicroSeconds CDRMAudioPlay::GetDelayL(CStifSectionParser *aSection, const TDesC &aTag)
{return 0;}*/

TTimeIntervalMicroSeconds32 CDRMAudioPlay::GetTimeIntervalL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefault, const TTimeIntervalMicroSeconds32 &aDefaultDelay)
	{
	TInt ParseError=KErrNotFound;
	TInt TempDelay=0;
	TTimeIntervalMicroSeconds32 ReturnValue=aDefaultDelay;
	CStifItemParser *item = aSection->GetItemLineL(aTag);
	if (item)
		{
		CleanupStack::PushL(item);
		ParseError = item->GetInt(aTag, TempDelay) ;
		if (ParseError)
			{	//Delay not found, using defaults
			aUsingDefault=ETrue;
			}
		else
			{	//Delay found.
			aUsingDefault=EFalse;
			ReturnValue=TempDelay;
			}
		CleanupStack::PopAndDestroy(item);
		}
		if (ParseError) {iLogger->Log(_L("Error retrieving report delay, [%S] not found") , &aTag);}
	return ReturnValue;
	}

TInt CDRMAudioPlay::RunTestCaseFunctionL(TInt id , CStifSectionParser *section , TTestResult &aResult)
	{
	switch (id)
		{
		case KCasePlayFile:
			return TestCasePlayFileL(section, aResult);
		case KCasePlayFileWithRamp:
			return TestCasePlayFileWithVolumeRampL(section, aResult);
		case KCasePlayFileWithRepeats:
			return TestCasePlayFileWithRepeatsL(section, aResult);
		case KCasePlayFileWithPause:
			return TestCasePlayFileWithPauseL(section, aResult);
		case KCasePlayFileWithPosition:
			return TestCasePlayFileWithPositionL(section, aResult);
		case KCasePlayFileWithBalance:
			return TestCasePlayFileWithBalanceL(section, aResult);
		case KCasePlayFileStopOpen:
			return TestCasePlayFileStopOpenL(section, aResult);
		case KCasePlayFileReportDuration:
			return TestCasePlayFileReportDurationL(section, aResult);
        case KCasePlayFileMaxVolume:
            return TestCasePlayFileMaxVolumeL(section, aResult);
        case KCasePlayFileSetVolume:
            return TestCasePlayFileSetVolumeL(section, aResult);
        case KCasePlayFileSetBalance:
            return TestCasePlayFileSetBalanceL(section, aResult);
        case KCasePlayFileGetBalance:
            return TestCasePlayFileGetBalanceL(section, aResult);
        case KCasePlayFileGetBitRate:
            return TestCasePlayFileGetBitRateL(section, aResult);
        case KCasePlayFileSetPriority:
            return TestCasePlayFileSetPriorityL(section, aResult);
        case KCasePlayFileSetRepeats:
        	return TestCasePlayFileSetRepeatsL(section, aResult);
        case KCasePlayFileReportPosition:
			return TestCasePlayFileReportPositionL(section, aResult);
		case KCasePlayFileWindow:
			return TestCasePlayFileWindowL(section, aResult);
		case KCasePlayFileClearWindow:
			return TestCasePlayFileWindowClearL(section, aResult);
		case KCasePlayFilesWithPriorities:
			return TestCasePlayFilesWithPrioritiesL(section, aResult);
		case KCasePlayFilesWithVolumes:
			return TestCasePlayFilesWithVolumesL(section, aResult);
        case KCasePlayFilesGetMetaDataEntry:
            return TestCasePlayFilesGetMetaDataEntryL(section, aResult);
        case KCasePlayFilesGetNumberOfMetaDataEntries:
            return TestCasePlayFilesGetNumberOfMetaDataEntriesL(section, aResult);
		case KCasePlayDes:
			return TestCasePlayDesL(section, aResult);
		case KCasePlayDesStopOpen:
			return TestCasePlayDesStopOpenL(section, aResult);
		case KCasePlayUrl:
			return TestCasePlayUrlL(section, aResult);
		case KCasePlayUrlStopOpen:
			return TestCasePlayUrlStopOpenL(section, aResult);
		case KCasePlayFileMetaInfo:
			return TestCasePlayFileMetaInfoL(section, aResult);
		case KCaseImplementationInformation:
			return TestCasePlayFileFormatsL(section, aResult);
		case KCasePlayHandler:
			return TestCasePlayHandlerL(section, aResult);
		case KCasePlayHandlerStopOpen:
			return TestCasePlayHandlerStopOpenL(section, aResult);
		case KCasePlayHandlerWithPause:
			return TestCasePlayHandlerWithPauseL(section, aResult);
		case KCasePlayFileHandlerWithRamp:
			return TestCasePlayHandlerWithRampL(section, aResult);
		case KCasePlayFileHandlerWithSetPosition:
			return TestCasePlayHandlerWithSetPositionL(section, aResult);
		case KCasePlayFileHandlerWithGetPosition:
			return TestCasePlayHandlerWithGetPositionL(section, aResult);
		case KCasePlayFileHandlerWithDuration:
			return TestCasePlayHandlerWithDurationL(section, aResult);
		case KCasePlayFileHandlerWithWindow:
			return TestCasePlayHandlerWithWindowL(section, aResult);
		case KCasePlayFileHandlerWithClearWindow:
			return TestCasePlayHandlerWithClearWindowL(section, aResult);
		case KCasePlayFileGetAudioLoadingProgress:
			return TestCasePlayFilesGetAudioLoadingProgress(section, aResult);
		case KCasePlayFileRegisterForAudioLoadingNotification:
			return TestCasePlayFilesRegisterForAudioLoadingNotification(section, aResult);
		case KCasePlayFileImplementationInformation:
			return TestCasePlayFilesImplementationInformation(section, aResult);
		case KCasePlayRepeatDRMFile:
            return TestCasePlayRepeatDRMFile(section, aResult);
        case KCasePlayPauseRepeatDRMFile:
            return TestCasePlayPauseRepeatDRMFile(section, aResult);
        case KCasePlayRepeatDRMFileForever:
            return TestCasePlayRepeatDRMFileForever(section, aResult);
        case KCasePlayRepeatDRMFileWithPlayWindow:
            return TestCasePlayRepeatDRMFileWithPlayWindow(section, aResult);
        case KCasePlayPauseRepeatDRMFileWithPlayWindow:
            return TestCasePlayPauseRepeatDRMFileWithPlayWindow(section, aResult);
        case KCasePlayRepeatForeverDRMFileWithPlayWindow:
            return TestCasePlayRepeatForeverDRMFileWithPlayWindow(section, aResult);
		}
	aResult.iResultDes.Copy(KConfigInvalid());
	aResult.iResult = KErrNotExecuted;
	return KErrConfigInvalid;
	}

TBool CDRMAudioPlay::IsDurationInRange
	(	TInt aRepeatTimes, TTimeIntervalMicroSeconds aToneDuration, TTimeIntervalMicroSeconds aRepeatSilence,
		TTimeIntervalMicroSeconds aErrorRange , TTime aStartTime , TTime aEndTime )
	{
	TInt64 tmp=aToneDuration.Int64();
	TInt64 tmp2=aRepeatSilence.Int64();

	tmp*=(aRepeatTimes+1);  tmp2*=(aRepeatTimes);
	TTimeIntervalMicroSeconds ExpectedDuration = tmp+tmp2;

	TTime EndTimeRange1 = aStartTime;
	EndTimeRange1+= ExpectedDuration;
	TTime EndTimeRange2 = EndTimeRange1;
	EndTimeRange1-=aErrorRange;
	EndTimeRange2+=aErrorRange;

	iLogger->Log(_L("Actual play duration (%d)") , (aEndTime.MicroSecondsFrom(aStartTime)).Int64() );
	iLogger->Log(_L("Expected duration (%d)") , ExpectedDuration.Int64());
	iLogger->Log(_L("Error range (%d)") , aErrorRange.Int64());
	return (EndTimeRange1<aEndTime && aEndTime<EndTimeRange2) ;
	}

void CDRMAudioPlay::TestCasePreCheck(CStifSectionParser *section, TFileName& aFileName)
    {
    TPtrC FileNamePtr;
    TPtrC RightsPtr;
    RFs rightsFs;
    RFile rightsFile;
    RFs contentFs;
    RFile contentFile;
    if (( ! section->GetLine(KTagSoundFile, FileNamePtr, ENoTag) ) && ( ! section->GetLine(KTagRightsFile, RightsPtr, ENoTag) ))
        {
        User::LeaveIfError(rightsFs.Connect());
        rightsFs.ShareProtected();
        TFileName rightsFileName = RightsPtr;
        iLogger->Log(_L("Rights file location [%S] "), &rightsFileName);
        if (! BaflUtils::FileExists(rightsFs, rightsFileName))
            {
            iLogger->Log(_L("Rights file [%S] doesn't exits !!! "), &rightsFileName);
            User::Leave(KErrNotFound);
            }
#ifdef __WINSCW__
        // copy the DRM rights file to the destination folder (\private\DRM_AGENT_SID\Import) only for WINSCW
        if (! BaflUtils::PathExists(rightsFs, KTagOMADRMRightsDir))
            {
            TRAPD(err, rightsFs.MkDirAll(KTagOMADRMRightsDir));
            if ((err != KErrNone) && (err != KErrAlreadyExists))
                {
                iLogger->Log(_L("Unable to create the path [%S] Err[%d]"), &KTagOMADRMRightsDir, err);
                User::Leave(err);
                }
            }
        iLogger->Log(_L("Copying Rights file [%S] to [%S] "),&rightsFileName, &KTagOMADRMRightsDir);
        BaflUtils::CopyFile(rightsFs, rightsFileName, KTagOMADRMRightsDir);
#endif        
        rightsFs.Close();
        User::LeaveIfError(rightsFs.Connect());
        rightsFs.ShareProtected();
        User::LeaveIfError(rightsFile.Open( rightsFs, rightsFileName, EFileRead));
        HBufC8* rContent = HBufC8::NewL(2048);      // incase the drm rights file size is more than 2kb then this size must be altered accordingly
        CleanupStack::PushL(rContent);
        TPtr8 rightsContentPtr = rContent->Des();
        rightsFile.Read(rightsContentPtr);
        ProcessRightsL(rightsContentPtr);
        CleanupStack::PopAndDestroy(rContent);  // HBufC8*
        rightsFile.Close();
        rightsFs.Close();

        // copy the DRM content file to the destination folder (\private\DRM_AGENT_SID\DRM_AGENT_NAME)
        User::LeaveIfError(contentFs.Connect());
        contentFs.ShareProtected();
        TFileName contentFileName = FileNamePtr;
        iLogger->Log(_L("Audio file location [%S] "), &contentFileName);
        if (! BaflUtils::FileExists(contentFs, contentFileName))
            {
            iLogger->Log(_L("Content file [%S] doesn't exits !!! "), &contentFileName);
            User::Leave(KErrNotFound);
            }
        if (! BaflUtils::PathExists(contentFs, KTagOMADRMContentDir))
            {
            TRAPD(err, contentFs.MkDirAll(KTagOMADRMContentDir));
            if ((err != KErrNone) && (err != KErrAlreadyExists))
                {
                iLogger->Log(_L("Unable to create the path [%S] Err[%d]"), &KTagOMADRMContentDir, err);
                User::Leave(err);
                }
            }
        iLogger->Log(_L("Copying Content file [%S] to [%S] "),&contentFileName, &KTagOMADRMContentDir);
        BaflUtils::CopyFile(contentFs, contentFileName, KTagOMADRMContentDir);
        contentFs.Close();
        User::LeaveIfError(contentFs.Connect());
        contentFs.ShareProtected();
        User::LeaveIfError(contentFile.Open( contentFs, contentFileName, EFileRead));
        TFileName contentFileNameWithExt;
        contentFile.Name(contentFileNameWithExt);
        aFileName.Append(KTagOMADRMContentDir);
        aFileName.Append(contentFileNameWithExt);
        iLogger->Log(_L("contentFileNameWithExt [%S]"), &contentFileNameWithExt);
        contentFile.Close();
        contentFs.Close();
        iDRMPreCheckPassed = ETrue;
        }
    else
        {
        iDRMPreCheckPassed = EFalse;
        }
    }

_LIT(KDirectoryPath, "C:\\private\\12345678\\");
void CDRMAudioPlay::ProcessRightsL(const TDesC8& aRightsXMLContent)
	{
    iLogger->Log(_L("ProcessRightsL <==="));
	TInt err = KErrNone;
	TFileName fileName;
	TPath outputFileName;
	CSupplier *mySupplier = CSupplier::NewL();
	CleanupStack::PushL(mySupplier);
	iLogger->Log(_L("ProcessRightsL: Supplier created ... "));
	CMetaDataArray* metaDataArray = CMetaDataArray::NewL();
	iLogger->Log(_L("ProcessRightsL: MetaDataArray created ... "));
	CleanupStack::PushL(metaDataArray);
	metaDataArray->AddL(_L("Content Type"),_L("application/vnd.oma.drm.rights+xml")); 
	// Create the import object
	CImportFile* import = mySupplier->ImportFileL(KOma1XmlRoContentType, *metaDataArray);
	iLogger->Log(_L("ProcessRightsL: ImportFile created ... "));
	CleanupStack::PushL(import);
	err = import->WriteData(aRightsXMLContent);
	iLogger->Log(_L("ProcessRightsL: Wrote the rightsfile to ImportData err[%d] "), err);
	while(err == KErrCANewFileHandleRequired)
		{
        iLogger->Log(_L("ProcessRightsL:  KErrCANewFileHandleRequired"));
        RFs  newfs; 
		RFile newFile;
		User::LeaveIfError(newfs.Connect());
		import->GetSuggestedOutputFileName(fileName);
		outputFileName.Copy(KDirectoryPath);
		outputFileName.Append(fileName);
		newFile.Open(newfs, outputFileName, EFileWrite);
		err = import->ContinueWithNewOutputFile(newFile, outputFileName);
		newFile.Close(); // agent makes a copy so we don't need to keep our file handle
		newfs.Close();
		}
	iLogger->Log(_L("ProcessRightsL:  Error after the Writing ImportFile [%d]"), err);
	// tell CAF (and hence the agent) it's now got the entire file
	if (err == KErrNone)
		{
		err = import->WriteDataComplete();
		iLogger->Log(_L("ProcessRightsL:  Error after the ImportFile WriteDataComplete [%d]"), err);
		if (err == KErrNone)
			{
			// Look at the output of the import operation
			for(TInt i = 0; i < import->OutputFileCountL(); i++)
				{
				// for this example only add content output files
				// (absorb any rights in the original attachment 'silently')
				if(import->OutputFileL(i).OutputType() == EContent)
					{
					// Add file to list of attachments for this message
                    iLogger->Log(_L("FileName [%S]"),(import->OutputFileL(i).FileName().Ptr()));
					}
				}
			}
		}
		// Error handling
		if (err != KErrNone)
			{
			if (err == KErrNotReady)
				{
                iLogger->Log(_L("Agent not ready for import"));
				}
			else if (err == KErrCACorruptContent)
				{
                iLogger->Log(_L("Content data is corrupt"));
				}
			else
				{
                iLogger->Log(_L("Unexpected error: %d"), err);
				}
			}
			// Finished
	CleanupStack::PopAndDestroy(3);         // mySupplier, metaDataArray, import
	iLogger->Log(_L("ProcessRightsL ===>"));
	}

TInt CDRMAudioPlay::TestCasePlayRepeatDRMFile(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TTimeIntervalMicroSeconds repeatSilenceTimer = KDefaultSilenceDuration;
    TInt repeatCount = 0;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
    if (iDRMPreCheckPassed)
        {
        TBool UsingRepeatCountDefault, UsingRepeatSilencerDefault;
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, KDefaultSilenceDuration);
        iLogger->Log(_L("Repeat count [%d] Silence timer [%d]"), repeatCount, I64LOW(repeatSilenceTimer.Int64()));
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        sndPlayer->iDontPlayAudioClip = ETrue; // don't start the play back after initialize complete
        CActiveScheduler::Start();
        sndPlayer->SetRepeats(repeatCount, TTimeIntervalMicroSeconds(repeatSilenceTimer));
        sndPlayer->SetVolumeRamp(100000);
        sndPlayer->Play(); // Start the playback
        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start();
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        sndPlayer->SetRepeats(0,0);
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        iFile.Close();
        iFs.Close();
        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        iFs.Close();
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayPauseRepeatDRMFile(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TInt repeatCount = 0;
    TTimeIntervalMicroSeconds repeatSilenceTimer = KDefaultSilenceDuration;
    TTimeIntervalMicroSeconds32 PauseDelay=KDefaultPauseDelay;
    TTimeIntervalMicroSeconds32 PlayDelay=KDefaultPlayDelay;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
    if (iDRMPreCheckPassed)
        {
        TBool UsingRepeatCountDefault, UsingRepeatSilencerDefault, UsingDefault;
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, KDefaultSilenceDuration);
        iLogger->Log(_L("Repeat count [%d] Silence timer [%d]"), repeatCount, I64LOW(repeatSilenceTimer.Int64()));
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        PauseDelay = GetTimeIntervalL(section, KTagPauseDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
        if (UsingDefault) {iLogger->Log(_L("Error retrieving pause delay, using default"));}
        PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
        if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}
        iLogger->Log(_L("Setting Pause and Play delays to [%d] and [%d]"), PauseDelay.Int(), PlayDelay.Int());
        sndPlayer->SetRepeats(repeatCount, TTimeIntervalMicroSeconds(repeatSilenceTimer));
        sndPlayer->SetVolumeRamp(100000);
        sndPlayer->iDontPlayAudioClip = ETrue; // don't start the play back after initialize complete
        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start(); // initialize the player and wait for the intialize complete event
        TTimeIntervalMicroSeconds dur = sndPlayer->GetDuration();
        sndPlayer->Play(); // Start the loop play in the play window
        /*********************************************/
        /* For generating the Pause event during playback */
        /*********************************************/
        TTimeIntervalMicroSeconds32 interval = TTimeIntervalMicroSeconds32(dur.Int64() + repeatSilenceTimer.Int64() + (PlayDelay.Int() - PauseDelay.Int()));
        CParameters *pauseParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPause );
        CleanupStack::PushL(pauseParams);
        CParameters *playParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPlay );
        CleanupStack::PushL(playParams);
        CMediaEvent* mEventPause = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PauseDelay, interval, sndPlayer, pauseParams);
        CMediaEvent* mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, interval, sndPlayer, playParams);
        CActiveScheduler::Start(); // start the playback in the play window and loop play
        sndPlayer->SetRepeats(0,0);
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        iFile.Close();
        iFs.Close();
        CleanupStack::PopAndDestroy(mEventPlay);
        CleanupStack::PopAndDestroy(mEventPause);
        CleanupStack::Pop(playParams);
        CleanupStack::Pop(pauseParams);
        CleanupStack::PopAndDestroy(sndPlayer);
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayRepeatDRMFileForever(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TInt repeatCount = 0;
    TTimeIntervalMicroSeconds repeatSilenceTimer = 0;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
   if (iDRMPreCheckPassed)
        {
        TBool UsingRepeatCountDefault, UsingRepeatSilencerDefault;
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        if (repeatCount != KDefaultRepeatForever)
            repeatCount = KDefaultRepeatForever;
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, (TTimeIntervalMicroSeconds)KDefaultSilenceDuration);
        sndPlayer->SetRepeats(repeatCount, repeatSilenceTimer);
        sndPlayer->SetVolumeRamp(100000);
        CActiveScheduler::Start();
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        sndPlayer->SetRepeats(0,0);
        CleanupStack::PopAndDestroy(sndPlayer);
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        iFile.Close();
        iFs.Close();
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayRepeatDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds startPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds endPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TInt repeatCount = 0;
    TTimeIntervalMicroSeconds repeatSilenceTimer = 0;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
    if (iDRMPreCheckPassed)
        {
        TBool UsingStartDefault, UsingEndDefault, UsingRepeatCountDefault, UsingRepeatSilencerDefault;
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, (TTimeIntervalMicroSeconds)KDefaultSilenceDuration);
        startPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
        endPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));
        // sndPlayer->iDontPlayAudioClip = ETrue; // don't start the play back after initialize complete
        iLogger->Log(_L("Starting scheduler"));
        sndPlayer->SetRepeats(repeatCount, repeatSilenceTimer);
        sndPlayer->SetVolumeRamp(100000);
        sndPlayer->iPlayWindow = ETrue;
        sndPlayer->iStartPosition = startPosition;
        sndPlayer->iEndPosition = endPosition;
        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start(); // initialize the player and wait for the intialize complete event
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        sndPlayer->SetRepeats(0,0);
        sndPlayer->iPlayWindow = EFalse;
        sndPlayer->ClearPlayWindow();
        CleanupStack::PopAndDestroy(sndPlayer);
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        iFile.Close();
        iFs.Close();
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayPauseRepeatDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds startPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds endPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TTimeIntervalMicroSeconds32 PauseDelay=KDefaultPauseDelay;
    TTimeIntervalMicroSeconds32 PlayDelay=KDefaultPlayDelay;
    TInt repeatCount = 0;
    TTimeIntervalMicroSeconds repeatSilenceTimer = 0;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
    if (iDRMPreCheckPassed)
        {
        TBool UsingStartDefault, UsingEndDefault, UsingRepeatCountDefault, UsingRepeatSilencerDefault, UsingDefault;
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, (TTimeIntervalMicroSeconds)KDefaultSilenceDuration);
        startPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
        endPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));
        PauseDelay = GetTimeIntervalL(section, KTagPauseDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPauseDelay);
        if (UsingDefault) {iLogger->Log(_L("Error retrieving pause delay, using default"));}
        PlayDelay = GetTimeIntervalL(section, KTagPlayDelay, UsingDefault, (TTimeIntervalMicroSeconds32)KDefaultPlayDelay);
        if (UsingDefault) {iLogger->Log(_L("Error retrieving play delay, using default"));}
        iLogger->Log(_L("Setting Pause and Play delays to [%d] and [%d]"), PauseDelay.Int(), PlayDelay.Int());
        sndPlayer->iDontPlayAudioClip = ETrue; // don't start the play back after initialize complete
        sndPlayer->SetRepeats(repeatCount, repeatSilenceTimer);
        sndPlayer->SetVolumeRamp(100000);
        sndPlayer->iPlayWindow = ETrue;
        sndPlayer->iStartPosition = startPosition;
        sndPlayer->iEndPosition = endPosition;
        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start(); // initialize the player and wait for the intialize complete event
        TTimeIntervalMicroSeconds dur = sndPlayer->GetDuration();
        TTimeIntervalMicroSeconds32 interval = TTimeIntervalMicroSeconds32((endPosition.Int64() - startPosition.Int64()) + repeatSilenceTimer.Int64() + (PlayDelay.Int() - PauseDelay.Int()));
        sndPlayer->Play(); // Start the loop play in the play window
        // sndPlayer->SetPlayWindow(startPosition, endPosition);
        /*********************************************/
        /* For generating the Pause event during playback */
        /*********************************************/
        CParameters *pauseParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPause );
        CleanupStack::PushL(pauseParams);
        CParameters *playParams = new (ELeave) CParameters( CSimpleSoundPlayer::KPlayerActionPlay );
        CleanupStack::PushL(playParams);
        CMediaEvent* mEventPause = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PauseDelay, interval, sndPlayer, pauseParams);
        CMediaEvent* mEventPlay = CMediaEvent::NewLC(TestModuleIf() , *iLogger, PlayDelay, interval, sndPlayer, playParams);
        CActiveScheduler::Start(); // start the playback in the play window and loop play
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        sndPlayer->SetRepeats(0,0);
        sndPlayer->iPlayWindow = EFalse;
        sndPlayer->ClearPlayWindow();
        CleanupStack::PopAndDestroy(mEventPlay);
        CleanupStack::PopAndDestroy(mEventPause);
        CleanupStack::Pop(playParams);
        CleanupStack::Pop(pauseParams);
        CleanupStack::PopAndDestroy(sndPlayer);
        iFile.Close();
        iFs.Close();
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }

TInt CDRMAudioPlay::TestCasePlayRepeatForeverDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult)
    {
    TTimeIntervalMicroSeconds startPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds endPosition = KDefaultPosition;
    TTimeIntervalMicroSeconds32 ReportDelay = KDefaultReportDelay;
    TInt repeatCount = 0;
    TTimeIntervalMicroSeconds repeatSilenceTimer = 0;
    TFileName fileName;
    iDRMPreCheckPassed = EFalse;
    TestCasePreCheck(section, fileName);
    if (iDRMPreCheckPassed)
        {
        TBool UsingStartDefault, UsingEndDefault, UsingRepeatCountDefault, UsingRepeatSilencerDefault;
        User::LeaveIfError(iFs.Connect());
        iFs.ShareProtected();
        User::LeaveIfError(iFile.Open( iFs, fileName, EFileRead));
        iLogger->Log(_L("Creating simple player with file [%S]"), &fileName);
        CSimpleSoundPlayer *sndPlayer = CSimpleSoundPlayer::NewL( iFile, TestModuleIf() , *iLogger);
        CleanupStack::PushL(sndPlayer);
        repeatCount = GetIntL(section, KTagRepeatTimes, UsingRepeatCountDefault, KDefaultRepeats);
        if (repeatCount != KDefaultRepeatForever)
            repeatCount = KDefaultRepeatForever;
        repeatSilenceTimer = GetTimeIntervalL(section, KTagSilenceDuration, UsingRepeatSilencerDefault, (TTimeIntervalMicroSeconds)KDefaultSilenceDuration);
        startPosition = GetTimeIntervalL(section, KTagWindowStart, UsingStartDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()-1E6));
        endPosition = GetTimeIntervalL(section, KTagWindowEnd, UsingEndDefault, (TTimeIntervalMicroSeconds)(KDefaultPosition.Int64()+1E6));
        sndPlayer->SetRepeats(repeatCount, repeatSilenceTimer);
        sndPlayer->SetVolumeRamp(100000);
        sndPlayer->iPlayWindow = ETrue;
        sndPlayer->iStartPosition = startPosition;
        sndPlayer->iEndPosition = endPosition;
        sndPlayer->iDontPlayAudioClip = ETrue; // don't start the play back after initialize complete
        iLogger->Log(_L("Starting scheduler"));
        CActiveScheduler::Start(); // initialize the player and wait for the intialize complete event
        sndPlayer->Play(); // Start the loop play in the play window
        CActiveScheduler::Start();
        iLogger->Log(_L("Returned from player, errorcode: %d"), sndPlayer->iFinalError );
        aResult.iResult = sndPlayer->iFinalError;
        sndPlayer->SetRepeats(0,0);
        sndPlayer->iPlayWindow = EFalse;
        sndPlayer->ClearPlayWindow();
        CleanupStack::PopAndDestroy(sndPlayer);
        if ((aResult.iResult != KErrNone) && (aResult.iResult != KErrCANoRights))
            {
            iLogger->Log(_L("Test failed, error code %d"), aResult.iResult);
            aResult.iResultDes.Copy(KTestCaseResultFail());
            }
        else
            {
            iLogger->Log(_L("Test was successful"));
            if (aResult.iResult == KErrCANoRights)
                aResult.iResult = KErrNone;
            aResult.iResultDes.Copy(KTestCaseResultSuccess());
            }
        iFile.Close();
        iFs.Close();
        return KErrExecuted;
        }
    else
        {
        aResult.iResultDes.Copy(KConfigInvalid());
        aResult.iResult = KErrNotExecuted;
        return KErrConfigInvalid;
        }
    }
