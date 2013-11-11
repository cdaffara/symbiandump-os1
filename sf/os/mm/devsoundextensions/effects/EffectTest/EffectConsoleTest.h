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
* Description:  Effects console test
*
*/




#ifndef EFFECTSCONSOLETEST
#define EFFECTSCONSOLETEST

//  INCLUDES
#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <mdaaudiosampleplayer.h>
#include <MdaAudioSampleEditor.h>

#include <AudioEffectBase.h>
#include <MAudioEffectObserver.h>
#include <AudioEqualizerBase.h>
#include <MAudioEqualizerObserver.h>
#include <EnvironmentalReverbBase.h>
#include <MEnvironmentalReverbObserver.h>
#include <StereoWideningBase.h>
#include <MStereoWideningObserver.h>

#include <customInterfaceUtility.h>

_LIT(KAMRTestFile,"c:\\test.amr");
_LIT(KWAVTestFile,"c:\\test.wav");


//////////////////////////////////////////////////////////////////////////////
//
// -----> CmyActiveScheduler (definition)
//
//////////////////////////////////////////////////////////////////////////////
class CmyActiveScheduler : public CActiveScheduler
	{
public:
	void Error (TInt aError) const;
	};


//////////////////////////////////////////////////////////////////////////////
//
// -----> CActiveConsole (definition)
//
// An abstract class which provides the facility to issue key requests.
//
//////////////////////////////////////////////////////////////////////////////
class CActiveConsole : public CActive
	{
public:
	  // Construction
	CActiveConsole(CConsoleBase* aConsole);
	void ConstructL();
	static CActiveConsole* NewLC(CConsoleBase* aConsole) ;
	~CActiveConsole();

	  // Issue request
	void RequestCharacter();

	  // Cancel request.
	  // Defined as pure virtual by CActive;
	  // implementation provided by this class.
	void DoCancel();

	  // Service completed request.
	  // Defined as pure virtual by CActive;
	  // implementation provided by this class,
	void RunL();

	  // Called from RunL() - an implementation must be provided
	  // by derived classes to handle the completed request
	virtual void ProcessKeyPress(TChar aChar) = 0;

protected:
	  // Data members defined by this class
	CConsoleBase* iConsole; // A console for reading from
	};

//////////////////////////////////////////////////////////////////////////////
//
// -----> CConsoleTest (definition)
//
// This class is derived from CActiveConsole.
// Request handling: accepts input from the keyboard and outputs the
// test result through test harness.
//
//////////////////////////////////////////////////////////////////////////////
class CConsoleTest : public CActiveConsole,
				     public MMdaAudioPlayerCallback,
				     public MMdaObjectStateChangeObserver,
 					 public MAudioEffectObserver
					 //public MAudioEqualizerObserver
{
public:

	enum TMenu
	{
		EMain,
		EPlay,
		ERecord,
		EConvert,
		EEqualizer,
		EBandId,
		EBandLevel,
		EEnvironmentalReverb1,
		EEnvironmentalReverb2,
		EStereoWidening,
		EStereoWideningLevel

	};

	// Destruction
	~CConsoleTest();

public:
	  // Static constuction
	static CConsoleTest *NewLC (CConsoleBase* aConsole);
	static CConsoleTest *NewL(CConsoleBase* aConsole);

	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorcCode);
	virtual void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

public:
	  // Service request
	void ProcessKeyPress(TChar aChar);
	void ShowMenu();

private:

	void ConstructL();
	CConsoleTest(CConsoleBase* aConsole);
	void ProcessMain(TChar aChar);
	void ProcessPlayL(TChar aChar);
	void ProcessEqualizerL(TChar aChar);
	void ProcessBandId(TChar aChar);
	void ProcessBandLevelL(TChar aChar);
	void InitializeTestData();
	void ProcessEnvironmentalReverbL(TChar aChar);
	void TestCase1();
	void TestCase2();
	void TestCase3();
	void ProcessStereoWideningL(TChar aChar);
	void ProcessStereoWideningLevelL(TChar aChar);




private:

	TInt iInitStatus;

	TMenu iMenu;
	TMenu iParentMenu;
	TUint32 iBandId;
	CCustomInterfaceUtility* iCIUtility;

	CMdaAudioPlayerUtility* iAudioPlayer;
	CAudioEqualizer* iPlayerAudioEqualizer;
	CEnvironmentalReverb* iPlayerEnvironmentalReverb;
	CStereoWidening* iPlayerStereoWidening;

	CMdaAudioRecorderUtility* iAudioRecorder;
	CAudioEqualizer* iRecorderAudioEqualizer;
	CEnvironmentalReverb* iRecorderEnvironmentalReverb;
	CStereoWidening* iRecorderStereoWidening;

	CMdaAudioConvertUtility* iAudioConverter;

	CAudioEqualizer* iEqualizer;
	CEnvironmentalReverb* iEnvironmentalReverb;
	CStereoWidening* iStereoWidening;


	CAudioEqualizer* iAudioEqualizer;

	TBuf<100> iSampleFilePath;

	RFs  iFs;

	RArray<TInt> iGoodData;
	RArray<TInt> iBadData;

	};

//////////////////////////////////////////////////////////////////////////////
//
// Main
//
//////////////////////////////////////////////////////////////////////////////

_LIT(KTxtDBTest, "Effects Console Test");

LOCAL_C void doTestL();

GLDEF_C TInt E32Main() // main function called by E32
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error, doTestL());
	__ASSERT_ALWAYS(!error,User::Panic(KTxtDBTest, error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
}

#endif
