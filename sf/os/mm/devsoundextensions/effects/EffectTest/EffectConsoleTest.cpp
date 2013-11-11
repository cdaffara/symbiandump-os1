/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Effects console test implementation
*
*/



#include "EffectConsoleTest.h"

_LIT(KNewLine,"\n");

//////////////////////////////////////////////////////////////////////////////
//
// -----> CmyActiveScheduler (implementation)
//
//////////////////////////////////////////////////////////////////////////////
void CmyActiveScheduler::Error(TInt aError) const
	{
	_LIT(KMsgSchedErr,"CmyActiveScheduler-error");
	User::Panic(KMsgSchedErr,aError);
	}


//////////////////////////////////////////////////////////////////////////////
//
// -----> CActiveConsole (implementation)
//
//////////////////////////////////////////////////////////////////////////////
CActiveConsole::CActiveConsole( CConsoleBase* aConsole)
	: CActive(CActive::EPriorityStandard)
	  // Construct high-priority active object
	{
	iConsole = aConsole;
	}

void CActiveConsole::ConstructL()
	{
	  // Add to active scheduler
	CActiveScheduler::Add(this);
	}

CActiveConsole::~CActiveConsole()
	{
	// Make sure we're cancelled
	Cancel();
	}

void  CActiveConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void  CActiveConsole::RunL()
	{
	  // Handle completed request
	ProcessKeyPress(TChar(iConsole->KeyCode()));
	}

void CActiveConsole::RequestCharacter()
	{
	  // A request is issued to the CConsoleBase to accept a
	  // character from the keyboard.
	iConsole->Read(iStatus);
	SetActive();
	}


//////////////////////////////////////////////////////////////////////////////
//
// -----> CWriteKeyProcessor (implementation)
//
//////////////////////////////////////////////////////////////////////////////
CConsoleTest::CConsoleTest(CConsoleBase* aConsole)
	:	CActiveConsole(aConsole),
		iMenu(EMain)
	{
	iEqualizer = NULL;
	iEnvironmentalReverb = NULL;
	iStereoWidening = NULL;
	iInitStatus = -1;
	};

CConsoleTest::~CConsoleTest()
{
	RDebug::Print(_L("CConsoleTest::~CConsoleTest\n"));
	// Make sure we're cancelled
	Cancel();

	delete iPlayerAudioEqualizer;
	delete iPlayerEnvironmentalReverb;
	delete iPlayerStereoWidening;
	delete iRecorderAudioEqualizer;
	delete iRecorderEnvironmentalReverb;
	delete iRecorderStereoWidening;

	delete iCIUtility;

	delete iAudioPlayer;
	delete iAudioRecorder;
	delete iAudioConverter;

	iGoodData.Close();
	iBadData.Close();

	iFs.Close();

}

void CConsoleTest::ConstructL()
{
	User::LeaveIfError(iFs.Connect());

	iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(KAMRTestFile, *this);
	//iAudioRecorder = CMdaAudioRecorderUtility::NewL(*this);
	//iAudioConverter = CMdaAudioConvertUtility::NewL(*this);

	//iAudioPlayer->OpenFileL(KWAVTestFile);

	// Add to active scheduler
	CActiveScheduler::Add(this);
	InitializeTestData();
}

CConsoleTest* CConsoleTest::NewLC(CConsoleBase* aConsole)
{
	CConsoleTest* self=new (ELeave) CConsoleTest(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CConsoleTest* CConsoleTest::NewL(CConsoleBase* aConsole)
{
	CConsoleTest* self=NewLC(aConsole);
	CleanupStack::Pop();
	return self;
}



void CConsoleTest::ProcessKeyPress(TChar aChar)
{
//	iConsole->Printf(_L("You pressed: %d\n"), (TUint) aChar);

	  // if key is ESC
	  //   cancel any outstanding request
	  //   stop the scheduler
      TInt err(KErrNone);
#ifdef __WINS__
	if (aChar == EKeyEscape)
#else
	if (aChar == ' ')
#endif
	{
		Cancel();
		CActiveScheduler::Stop();
		return;
	}

	if (aChar == EKeyEnter)
	{
		iConsole->Printf(KNewLine);
		// Issue another request
		RequestCharacter();
		return;
	}

#ifdef __WINS__
	if (aChar == '?')
#else
	if (aChar == '7')
#endif
	{
		ShowMenu();
		// Issue another request
		RequestCharacter();
		return;
	}

	switch (iMenu)
	{
	case EMain:
		ProcessMain(aChar);
		break;

	case EPlay:
		TRAP(err,ProcessPlayL(aChar));
		break;

	case ERecord:
		//ProcessRecord(aChar);
		break;

	case EEqualizer:
		TRAP(err,ProcessEqualizerL(aChar));
		break;

	case EBandId:
		ProcessBandId(aChar);
		break;

	case EBandLevel:
		TRAP(err,ProcessBandLevelL(aChar));
		break;

	case EEnvironmentalReverb1:
		TRAP(err,ProcessEnvironmentalReverbL(aChar));
		break;

	case EEnvironmentalReverb2:
		break;

	case EStereoWideningLevel:
		TRAP(err,ProcessStereoWideningLevelL(aChar));
		break;

	case EStereoWidening:
		TRAP(err,ProcessStereoWideningL(aChar));
		break;


	default:
		iConsole->Printf(_L("ProcessKeyPress - Unknown function\n"));
		// Issue another request
		RequestCharacter();
		break;
	}
}


void CConsoleTest::ShowMenu()
{
	iConsole->ClearScreen();
	switch (iMenu)
	{
	case EMain:
		iConsole->Printf(_L("1: Player\n"));
		iConsole->Printf(_L("2: Recorder\n"));
		iConsole->Printf(_L("3: Converter\n"));
		iConsole->Printf(_L("9: Exit\n"));
		break;

	case EPlay:
	case ERecord:
	case EConvert:
		iConsole->Printf(_L("0: Main Menu\n"));
		iConsole->Printf(_L("1: Equalizer\n"));
		iConsole->Printf(_L("2: Env Reverb\n"));
		iConsole->Printf(_L("3: StereoWidening\n"));

		break;

	case EEqualizer:
		iConsole->Printf(_L("0: Main Menu\n"));
		iConsole->Printf(_L("1: Enable/Disable\n"));
		iConsole->Printf(_L("2: Toggle Enforce\n"));
		iConsole->Printf(_L("3: Set Band Level\n"));
		iConsole->Printf(_L("4: Show Equalizer info\n"));
		iConsole->Printf(_L("5: Show Band info\n"));
		iConsole->Printf(_L("6: Apply Settings\n"));
		iConsole->Printf(_L("9: Delete Equalizer\n"));
		break;

	case EBandLevel:
		iConsole->Printf(_L("0: Equalizer Menu\n"));
		iConsole->Printf(_L("1: Band Level Up\n"));
		iConsole->Printf(_L("2: Band Level Down\n"));
		iConsole->Printf(_L("3: Show Band info\n"));
		break;

	case EEnvironmentalReverb1:
		iConsole->Printf(_L("0: Main Menu\n"));
		iConsole->Printf(_L("1: Enable/Disable\n"));
		iConsole->Printf(_L("2: Toggle Enforce\n"));
		iConsole->Printf(_L("3: Test Case 1\n"));
		iConsole->Printf(_L("4: Test Case 2\n"));
		iConsole->Printf(_L("5: Test Case 3\n"));
		iConsole->Printf(_L("6: Undef\n"));
		iConsole->Printf(_L("7: Undef\n"));
		iConsole->Printf(_L("8: Apply Settings\n"));
		iConsole->Printf(_L("9: Del Env Reverb\n"));
		break;
	case EEnvironmentalReverb2:
		iConsole->Printf(_L("0: Main Menu\n"));
		iConsole->Printf(_L("1: \n"));
		iConsole->Printf(_L("2: \n"));
		iConsole->Printf(_L("3: \n"));
		iConsole->Printf(_L("4: \n"));
		iConsole->Printf(_L("5: \n"));
		iConsole->Printf(_L("6: \n"));
		iConsole->Printf(_L("7: More\n"));
		iConsole->Printf(_L("8: Apply Settings\n"));
		iConsole->Printf(_L("9: Del Env Reverb\n"));
		break;

	case EStereoWidening:
		iConsole->Printf(_L("0: Main Menu\n"));
		iConsole->Printf(_L("1: Enable/Disable\n"));
		iConsole->Printf(_L("2: Toggle Enforce\n"));
		iConsole->Printf(_L("3: Set Widening Level\n"));
		iConsole->Printf(_L("4: Apply Settings\n"));
		iConsole->Printf(_L("9: Delete Effect\n"));
		break;


	case EStereoWideningLevel:
		iConsole->Printf(_L("0: StereoWidening Menu\n"));
		iConsole->Printf(_L("1: Level Up\n"));
		iConsole->Printf(_L("2: Level Down\n"));
		iConsole->Printf(_L("3: Show Effect info\n"));
		iConsole->Printf(_L("9: Go Back\n"));
		break;



		default:
		// Do nothing
		break;
	}
	iConsole->Printf(KNewLine);
}



//////////////////////////////////////////////////////////////////////////////
//
// -----> MAIN
//
//////////////////////////////////////////////////////////////////////////////

void CConsoleTest::ProcessMain(TChar aChar)
{

	if ( !iInitStatus )
	{
		switch (aChar)
		{
		case '1':
			iMenu = iParentMenu = EPlay;
			ShowMenu();
			break;

		case '2':
			iMenu = iParentMenu = EPlay;
			ShowMenu();
			break;

		case '3':
			iMenu = iParentMenu = EPlay;
			ShowMenu();
			break;

		case '9':
			iConsole->Printf(_L("Stopping Scheduler...\n"));
			CActiveScheduler::Stop();
			iConsole->Printf(_L("Exiting...\n"));
			break;


		default:
			iConsole->Printf(_L("ProcessMain - Unknown command\n"));
			break;
		};
	}
	else
		iConsole->Printf(_L("Not Ready!\n"));



	// Issue another request
	RequestCharacter();
}


//////////////////////////////////////////////////////////////////////////////
//
// -----> Play menu
//
//////////////////////////////////////////////////////////////////////////////

void CConsoleTest::ProcessPlayL(TChar aChar)
{
	switch (aChar)
	{
	case '0':	// Main Menu
		iMenu = EMain;
		ShowMenu();
		break;

	case '1':	// Equalizer
		iMenu = EEqualizer;
		if (!iPlayerAudioEqualizer)
			{
			iPlayerAudioEqualizer = CAudioEqualizer::NewL(*iAudioPlayer);
			iPlayerAudioEqualizer->RegisterObserverL(*this);
			}
		iAudioEqualizer = iPlayerAudioEqualizer;
		ShowMenu();
		break;

	case '2':	// Environmental Reverb
		iMenu = EEnvironmentalReverb1;
		if (!iPlayerEnvironmentalReverb)
			{
			iPlayerEnvironmentalReverb = CEnvironmentalReverb::NewL(*iAudioPlayer);
			iPlayerEnvironmentalReverb->RegisterObserverL(*this);
			}
		iEnvironmentalReverb = iPlayerEnvironmentalReverb;
		ShowMenu();
		break;


	case '3':	// Stereo Widening
		iMenu = EStereoWidening;
		if (!iPlayerStereoWidening)
			{
			iPlayerStereoWidening = CStereoWidening::NewL(*iAudioPlayer,EFalse,5);
			iPlayerStereoWidening->RegisterObserverL(*this);
			}
		iStereoWidening = iPlayerStereoWidening;
		ShowMenu();
		break;



	default:
		iConsole->Printf(_L("Play Menu - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}



//////////////////////////////////////////////////////////////////////////////
//
// -----> Equalizer Menu
//
//////////////////////////////////////////////////////////////////////////////


void CConsoleTest::ProcessEqualizerL(TChar aChar)
{
	TInt32 min, max;

	switch (aChar)
	{
	case '0':	// Main Menu
		iMenu = EMain;
		ShowMenu();
		break;

	case '1':	// Enable / disable
		if ( iAudioEqualizer->IsEnabled() )
			iAudioEqualizer->DisableL();
		else
			iAudioEqualizer->EnableL();

		break;

	case '2':	// Enforce
		if ( iAudioEqualizer->IsEnforced() )
			iAudioEqualizer->EnforceL(EFalse);
		else
			iAudioEqualizer->EnforceL(ETrue);
		break;

	case '3':	// Set Band Level
		iMenu = EBandId;
		iConsole->Printf(_L("Enter Band ID: "));
		break;

	case '4':	// Print Equalizer information
		iConsole->Printf(_L("Enabled: %d\n"), iAudioEqualizer->IsEnabled());
		iConsole->Printf(_L("Enforced: %d\n"), iAudioEqualizer->IsEnforced());
		iConsole->Printf(_L("Number Of Bands: %d\n"), iAudioEqualizer->NumberOfBands());
		iAudioEqualizer->DbLevelLimits(min,max);
		iConsole->Printf(_L("Db Min %d, Max %d\n"), min, max );
		break;

	case '5':	// Print Band Info
		iMenu = EBandId;
		iConsole->Printf(_L("Enter Band ID: "));
		break;

	case '6':	// Apply Settings
		iAudioEqualizer->ApplyL();
		break;

	case '9':	// Delete volume object
		if (iParentMenu == EPlay)
			{
			delete iAudioEqualizer;
			iAudioEqualizer = NULL;
			iPlayerAudioEqualizer = NULL;
			}

		if (iParentMenu == ERecord)
			{
			delete iAudioEqualizer;
			iAudioEqualizer = NULL;
			iRecorderAudioEqualizer = NULL;
			}
		break;

	default:
		iConsole->Printf(_L("ProcessEqualizerL - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}


//////////////////////////////////////////////////////////////////////////////
//
// -----> Band Menu
//
//////////////////////////////////////////////////////////////////////////////


void CConsoleTest::ProcessBandId(TChar aChar)
{
	iBandId = aChar.GetNumericValue();
	iMenu = EBandLevel;
	ShowMenu();
	RequestCharacter();

}


void CConsoleTest::ProcessBandLevelL(TChar aChar)
{
	switch (aChar)
	{
	case '0':	// Main Menu
		iMenu = EEqualizer;
		ShowMenu();
		break;

	case '1':	// Level UP
		ShowMenu();
		iAudioEqualizer->SetBandLevelL(iBandId, iAudioEqualizer->BandLevel(iBandId) + 1);
		iConsole->Printf(_L("Band[%d] Level[%d]\n"), iBandId, iAudioEqualizer->BandLevel(iBandId) );
		//iAudioEqualizer->ApplyL();
		break;

	case '2':	// Level Down
		ShowMenu();
		iAudioEqualizer->SetBandLevelL(iBandId, iAudioEqualizer->BandLevel(iBandId) - 1);
		iConsole->Printf(_L("Band[%d] Level[%d]\n"), iBandId, iAudioEqualizer->BandLevel(iBandId) );
		//iAudioEqualizer->ApplyL();
		break;

	case '3':	// Print Band information
		iConsole->Printf(_L("Band Number: %d\n"), iBandId);
		iConsole->Printf(_L("BandLevel: %d\n"), iAudioEqualizer->BandLevel(iBandId));
		iConsole->Printf(_L("BandWidth: %d\n"), iAudioEqualizer->BandWidth(iBandId));
		iConsole->Printf(_L("Crossover Fequency %d\n"), iAudioEqualizer->CrossoverFrequency(iBandId));
		iConsole->Printf(_L("Center Fequency %d\n"), iAudioEqualizer->CenterFrequency(iBandId));
		break;

	default:
		iConsole->Printf(_L("ProcessEqualizerL - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}

//////////////////////////////////////////////////////////////////////////////
//
// -----> Stereo Widening Menu
//
//////////////////////////////////////////////////////////////////////////////


void CConsoleTest::ProcessStereoWideningL(TChar aChar)
{

	switch (aChar)
	{
	case '0':	// Main Menu
		iMenu = EMain;
		ShowMenu();
		break;

	case '1':	// Enable / disable
		if ( iStereoWidening->IsEnabled() )
			iStereoWidening->DisableL();
		else
			iStereoWidening->EnableL();

		break;

	case '2':	// Enforce
		if ( iStereoWidening->IsEnforced() )
			iStereoWidening->EnforceL(EFalse);
		else
			iStereoWidening->EnforceL(ETrue);
		break;

	case '3':	// Set Stereo Widening Level
		iMenu = EStereoWideningLevel;
		break;

	case '4':	// Apply Settings
		iStereoWidening->ApplyL();
		break;

	case '9':	// Delete volume object
		if (iParentMenu == EPlay)
			{
			delete iStereoWidening;
			iStereoWidening = NULL;
			iPlayerStereoWidening = NULL;
			}
		if (iParentMenu == ERecord)
			{
			delete iStereoWidening;
			iStereoWidening = NULL;
			iRecorderStereoWidening = NULL;
			}

			iMenu = EMain;
			ShowMenu();
			break;

	default:
		iConsole->Printf(_L("ProcessStereoWideningL - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}

void CConsoleTest::ProcessStereoWideningLevelL(TChar aChar)
{

	ShowMenu();
	switch (aChar)
	{

	case '1':	// Level UP
		ShowMenu();
		iStereoWidening->SetStereoWideningLevelL(iStereoWidening->StereoWideningLevel() + 1);
		iConsole->Printf(_L("Level[%d]\n"), iStereoWidening->StereoWideningLevel() );
		break;

	case '2':	// Level Down
		ShowMenu();
		iStereoWidening->SetStereoWideningLevelL(iStereoWidening->StereoWideningLevel() - 1);
		iConsole->Printf(_L("Level[%d]\n"), iStereoWidening->StereoWideningLevel() );
		break;

	case '3':	// Print Level information
		iConsole->Printf(_L("Level: %d\n"), iStereoWidening->StereoWideningLevel());
		iConsole->Printf(_L("Continuous Level %d\n"), iStereoWidening->IsContinuousLevelSupported());
		break;
	case '9':	// Main Menu
		iMenu = EStereoWidening;
		ShowMenu();
		break;

	default:
		iConsole->Printf(_L("ProcessStereoWideningL - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}

//////////////////////////////////////////////////////////////////////////////
//
// -----> Environmental reverb Menu
//
//////////////////////////////////////////////////////////////////////////////

void CConsoleTest::InitializeTestData()
{
	iGoodData.Append(1000); // Decay HF Ratio
	iGoodData.Append(1000); // Decay Time
	iGoodData.Append(1000); // Density
	iGoodData.Append(1000); // Diffusion
	iGoodData.Append(1000); // Reflections Delay
	iGoodData.Append(1000); // Reflections Level
	iGoodData.Append(1000); // Reverb Delay
	iGoodData.Append(1000); // Reverb Level
	iGoodData.Append(1000); // Room HF Level
	iGoodData.Append(1000); // Room Level

	iBadData.Append(50000); // Decay HF Ratio
	iBadData.Append(300000); // Decay Time
	iBadData.Append(10005); // Density
	iBadData.Append(10005); // Diffusion
	iBadData.Append(300000); // Reflections Delay
	iBadData.Append(2000); // Reflections Level
	iBadData.Append(300000); // Reverb Delay
	iBadData.Append(2000); // Reverb Level
	iBadData.Append(2000); // Room HF Level
	iBadData.Append(2000); // Room Level

}

void CConsoleTest::ProcessEnvironmentalReverbL(TChar aChar)
{

	switch (aChar)
	{
	case '0':	// Main Menu
		iMenu = EMain;
		ShowMenu();
		break;

	case '1':	// Enable / disable
		if ( iEnvironmentalReverb->IsEnabled() )
			iEnvironmentalReverb->DisableL();
		else
			iEnvironmentalReverb->EnableL();

		break;

	case '2':	// Enforce
		if ( iEnvironmentalReverb->IsEnforced() )
			iEnvironmentalReverb->EnforceL(EFalse);
		else
			iEnvironmentalReverb->EnforceL(ETrue);
		break;

	case '3':	// Test 1: Setters normal case
		TestCase1();
		break;

	case '4':	// Test 2: Setters abnormal case
		TestCase2();
		break;

	case '5':	// Test 3: Getters abnormal case
		TestCase3();
		break;

	case '8':	// Apply Settings
		iEnvironmentalReverb->ApplyL();
		break;

	case '9':	// Delete environmental reverb object
		if (iParentMenu == EPlay)
			{
			delete iEnvironmentalReverb;
			iEnvironmentalReverb = NULL;
			iPlayerEnvironmentalReverb = NULL;
			}

		if (iParentMenu == ERecord)
			{
			delete iEnvironmentalReverb;
			iEnvironmentalReverb = NULL;
			iRecorderEnvironmentalReverb = NULL;
			}
		break;

	default:
		iConsole->Printf(_L("ProcessEnvironmentalReverbL - Unknown command\n"));
		break;
	};

	// Issue another request
	RequestCharacter();
}

void CConsoleTest::TestCase1()
{
	TRAPD(err1, iEnvironmentalReverb->SetDecayHFRatioL(iGoodData[0]));
	iConsole->Printf(_L("Result 1.1 - %d\n"),err1);
	TRAPD(err2, iEnvironmentalReverb->SetDecayTimeL(iGoodData[1]));
	iConsole->Printf(_L("Result 1.2 - %d\n"),err2);
	TRAPD(err3, iEnvironmentalReverb->SetDensityL(iGoodData[2]));
	iConsole->Printf(_L("Result 1.3 - %d\n"),err3);
	TRAPD(err4, iEnvironmentalReverb->SetDiffusionL(iGoodData[3]));
	iConsole->Printf(_L("Result 1.4 - %d\n"),err4);
	TRAPD(err5, iEnvironmentalReverb->SetReflectionsDelayL(iGoodData[4]));
	iConsole->Printf(_L("Result 1.5 - %d\n"),err5);
	TRAPD(err6, iEnvironmentalReverb->SetReverbDelayL(iGoodData[6]));
	iConsole->Printf(_L("Result 1.6 - %d\n"),err6);
	TRAPD(err7, iEnvironmentalReverb->SetReverbLevelL(iGoodData[7]));
	iConsole->Printf(_L("Result 1.7 - %d\n"),err7);
	TRAPD(err8, iEnvironmentalReverb->SetRoomHFLevelL(iGoodData[8]));
	iConsole->Printf(_L("Result 1.8 - %d\n"),err8);
	TRAPD(err9, iEnvironmentalReverb->SetRoomLevelL(iGoodData[9]));
	iConsole->Printf(_L("Result 1.9 - %d\n"),err9);
	TRAPD(err10, iEnvironmentalReverb->SetReflectionsLevelL(iGoodData[5]));
	iConsole->Printf(_L("Result 1.10 - %d\n"),err10);
}

void CConsoleTest::TestCase2()
{
	TRAPD(err1, iEnvironmentalReverb->SetDecayHFRatioL(iBadData[0]));
	iConsole->Printf(_L("Result 1.1 - %d\n"),err1);
	TRAPD(err2, iEnvironmentalReverb->SetDecayTimeL(iBadData[1]));
	iConsole->Printf(_L("Result 1.2 - %d\n"),err2);
	TRAPD(err3, iEnvironmentalReverb->SetDensityL(iBadData[2]));
	iConsole->Printf(_L("Result 1.3 - %d\n"),err3);
	TRAPD(err4, iEnvironmentalReverb->SetDiffusionL(iBadData[3]));
	iConsole->Printf(_L("Result 1.4 - %d\n"),err4);
	TRAPD(err5, iEnvironmentalReverb->SetReflectionsDelayL(iBadData[4]));
	iConsole->Printf(_L("Result 1.5 - %d\n"),err5);
	TRAPD(err6, iEnvironmentalReverb->SetReverbDelayL(iBadData[6]));
	iConsole->Printf(_L("Result 1.6 - %d\n"),err6);
	TRAPD(err7, iEnvironmentalReverb->SetReverbLevelL(iBadData[7]));
	iConsole->Printf(_L("Result 1.7 - %d\n"),err7);
	TRAPD(err8, iEnvironmentalReverb->SetRoomHFLevelL(iBadData[8]));
	iConsole->Printf(_L("Result 1.8 - %d\n"),err8);
	TRAPD(err9, iEnvironmentalReverb->SetRoomLevelL(iBadData[9]));
	iConsole->Printf(_L("Result 1.9 - %d\n"),err9);
	TRAPD(err10, iEnvironmentalReverb->SetReflectionsLevelL(iBadData[5]));
	iConsole->Printf(_L("Result 1.10 - %d\n"),err10);
}

void CConsoleTest::TestCase3()
{
	if ( iEnvironmentalReverb->DecayHFRatio() == iGoodData[0] )
		iConsole->Printf(_L("Result 3.1 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.1 - 0\n"));

	if ( iEnvironmentalReverb->DecayTime() == iGoodData[1] )
		iConsole->Printf(_L("Result 3.2 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.2 - 0\n"));

	if ( iEnvironmentalReverb->Density() == iGoodData[2] )
		iConsole->Printf(_L("Result 3.3 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.3 - 0\n"));

	if ( iEnvironmentalReverb->Diffusion() == iGoodData[3] )
		iConsole->Printf(_L("Result 3.4 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.4 - 0\n"));

	if ( iEnvironmentalReverb->ReflectionsDelay() == iGoodData[4] )
		iConsole->Printf(_L("Result 3.5 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.5 - 0\n"));

	if ( iEnvironmentalReverb->ReflectionsLevel() == iGoodData[5] )
		iConsole->Printf(_L("Result 3.6 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.6 - 0\n"));

	if ( iEnvironmentalReverb->ReverbDelay() == iGoodData[6] )
		iConsole->Printf(_L("Result 3.7 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.7 - 0\n"));

	if ( iEnvironmentalReverb->ReverbLevel() == iGoodData[7] )
		iConsole->Printf(_L("Result 3.8 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.8 - 0\n"));

	if ( iEnvironmentalReverb->RoomHFLevel() == iGoodData[8] )
		iConsole->Printf(_L("Result 3.9 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.9 - 0\n"));

	if ( iEnvironmentalReverb->RoomLevel() == iGoodData[9] )
		iConsole->Printf(_L("Result 3.10 - 1\n"));
	else
		iConsole->Printf(_L("Result 3.10 - 0\n"));
}

/************************************************************************************************************/




void CConsoleTest::MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds& aDuration)
	{
	iConsole->Printf(_L("MapcInit: %d\n"), aStatus);
	iInitStatus = aStatus;
	TInt d = I64INT(aDuration.Int64());
	RDebug::Print(_L("CConsoleTest::MapcInitComplete :-> Status[%d] Duration[%d]"), aStatus, d);
	}

void CConsoleTest::MapcPlayComplete(TInt aErr)
	{
	iConsole->Printf(_L("MapcPlay: %d"), aErr);
	RDebug::Print(_L("CConsoleTest::MapcPlayComplete :-> Error[%d]"), aErr);
	}

void CConsoleTest::EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent )
	{
	RDebug::Print(_L("CConsoleTest::EffectChanged, Event = %d "), aEvent);

	if ( aAudioEffect->Uid() == KUidAudioEqualizerEffect )
		{
		if ( aEvent == KEnabled )
			{
			iConsole->Printf(_L("Equalizer state: %d"), ((CAudioEqualizer*)aAudioEffect)->IsEnabled());
			RDebug::Print(_L("CConsoleTest::EffectChanged :-> Enabled[%d]"), ((CAudioEqualizer*)aAudioEffect)->IsEnabled());
			}
		}
	else if ( aAudioEffect->Uid() == KUidStereoWideningEffect )
			{
			if ( aEvent == KEnabled )
				{
				iConsole->Printf(_L("StereoWidening state: %d"), ((CStereoWidening*)aAudioEffect)->IsEnabled());
				RDebug::Print(_L("CConsoleTest::EffectChanged :-> Enabled[%d]"), ((CStereoWidening*)aAudioEffect)->IsEnabled());
				}
			else
				{
				iConsole->Printf(_L("StereoWidening state: %d"), ((CStereoWidening*)aAudioEffect)->IsEnabled());
				RDebug::Print(_L("CConsoleTest::EffectChanged :-> Enabled[%d]"), ((CStereoWidening*)aAudioEffect)->IsEnabled());
				}
			}


	}

void CConsoleTest::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
	{
	RDebug::Print(_L("aPreviousState[%d], aCurrentState[%d], aErrorCode[%d]"), aPreviousState, aCurrentState, aErrorCode);
	iInitStatus = aErrorCode;
	iConsole->Printf(_L("Mosco: %d\n"), aErrorCode);
	}



//////////////////////////////////////////////////////////////////////////////
//
// Do the testing
//
//////////////////////////////////////////////////////////////////////////////
LOCAL_C void doTestL()
{

	CConsoleBase* console = Console::NewL(KTxtDBTest,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	CmyActiveScheduler* myScheduler = new (ELeave) CmyActiveScheduler;
	CleanupStack::PushL(myScheduler);
	CActiveScheduler::Install(myScheduler);

	// Create a CConsoleTest active object
	CConsoleTest* consoleTest = CConsoleTest::NewLC(console);
	consoleTest->ShowMenu();
	// Issue the first request
	consoleTest->RequestCharacter();

#ifdef __WINS__
	_LIT(KTitleMsg,"Ready!\nPress ESC to end.\n\n");
#else
	_LIT(KTitleMsg,"Press SPACE(0) to end.\n");
#endif
	console->Printf(KTitleMsg);
//	test.Title();

	// Main part of program is a wait loop
	// This function completes when the scheduler stops
	CActiveScheduler::Start();

	_LIT(KTxtPressAnyKey," [press any key]");
	console->Printf(KTxtPressAnyKey);
	console->Getch();					// get and ignore character

	// Remove from the cleanup stack and destroy:
	// 1. consoleTest
	// 2. myScheduler
	// 3. console
	CleanupStack::PopAndDestroy(3);
}

