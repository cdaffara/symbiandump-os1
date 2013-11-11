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
* Description:  Class definition header
*
*/



#ifndef EFFECT_PRESET_TEST_CORE
#define EFFECT_PRESET_TEST_CORE

#include "EffectPresetTestConstants.h"
#include "EffectPresetsTest.h"


//  INCLUDES
#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>

#include <mdaaudiosampleplayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioTonePlayer.h>
#include <SoundDevice.h>
#include <MdaAudioInputStream.h>
#include <MdaAudioOutputStream.h>
#include <McustomInterface.h>
#include <McustomCommand.h>

#include <AudioEffectBase.h>
#include <mdaaudiosampleplayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioTonePlayer.h>

#include <AudioEqualizerBase.h>
#include <AudioEqualizerUtility.h>

#include <EnvironmentalReverbBase.h>
#include <EnvironmentalReverbUtility.h>

#include <StereoWideningBase.h>
#include <StereoWideningUtility.h>

class CTestAudioEqualizerUtility;
class CTestEnvironmentalReverbUtility;
class CTestStereoWideningUtility;


/*==================================================================================
//  CTestEffectPresetBase
//
//  This class is the base class for the test effects classes.
====================================================================================*/
class CTestAudioEffectPreset: public CBase
{
public:
	CTestAudioEffectPreset(CStifLogger * aLogger);
	~CTestAudioEffectPreset();

	// public methods
	virtual TInt AssignObject(CMdaAudioPlayerUtility* aPlayer) = 0;
	virtual TInt AssignObject(CMdaAudioRecorderUtility* aRecorder) = 0;
	virtual TInt AssignObject(CMdaAudioConvertUtility* aConverter) = 0;
	virtual TInt AssignObject(CMdaAudioToneUtility* aTone) = 0;
	virtual TInt AssignObject(CMMFDevSound* aDevSound) = 0;
	virtual TInt AssignObject(CCustomCommandUtility* aCustomCommand) = 0;
	virtual TInt AssignObject(MCustomInterface* aCustomInterface) = 0;
	virtual TInt AssignObject(CMdaAudioInputStream* aInputStream) = 0;
	virtual TInt AssignObject(CMdaAudioOutputStream* aOutputStream) = 0;
	virtual TInt AssignObject(CMidiClientUtility* aMidiClientUtility) = 0;		
	virtual TInt AssignObject(CDrmPlayerUtility* aDrmPlayer) = 0; 			
	virtual TInt AssignObject(CVideoPlayerUtility* aUtility) = 0; 			
	virtual TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);
//	virtual void SetMenu(TMenu aMenu)=0;
//	virtual TMenu ProcessKeyPress(TChar aChar)=0;
	void CreateNextPresetName(TInt aNextPresetNumber);
    virtual void PrintPresetNamesL() = 0;
  //	TInt AssignEffect(TObject aObject);


protected:
	// Callbacks
	
	void PrintError(TInt err);

//	TMenu iMenu;
//	CConsoleBase* iConsole;
	CStifLogger*  iLogger;
//	CBase* iAudioEffectPreset;
	CAudioEqualizer*	iAudioEqualizer;
	CEnvironmentalReverb* iEnvironmentalReverb;
	CStereoWidening* iStereoWidening;
	
	CAudioEqualizerUtility* iAudioEqualizerUtility; 
	CEnvironmentalReverbUtility* iEnvironmentalReverbUtility;
	CStereoWideningUtility* iStereoWideningUtility;
	
	TUint32 iBandID;
	TBuf16<32> iPresetName;
	TInt iPresetIndex;
	TInt iPresetCount;
	TBool iState;
	TInt iError;

private:
    virtual void DeleteAudioEffect()=0;
};


/*==================================================================================
//  CTestAudioEqualizerUtility
//
//  This class test the Audio Equalizer Utility
====================================================================================*/
class CTestAudioEqualizerUtility: public CTestAudioEffectPreset
{
public:
	// Destructor
	~CTestAudioEqualizerUtility();

	// Static
	static CTestAudioEqualizerUtility *NewLC (CStifLogger * aLogger);
	static CTestAudioEqualizerUtility *NewL (CStifLogger * aLogger);

	// virtual
	TInt AssignObject(CMdaAudioPlayerUtility* aPlayer);
	TInt AssignObject(CMdaAudioRecorderUtility* aRecorder);
	TInt AssignObject(CMdaAudioConvertUtility* aConverter);
	TInt AssignObject(CMdaAudioToneUtility* aTone);
	TInt AssignObject(CMMFDevSound* aDevSound);
	TInt AssignObject(CCustomCommandUtility* aCustomCommand);
	TInt AssignObject(MCustomInterface* aCustomInterface);
	TInt AssignObject(CMdaAudioInputStream* aInputStream);
	TInt AssignObject(CMdaAudioOutputStream* aOutputStream);
	TInt AssignObject(CMidiClientUtility* aMidiClientUtility);		
	TInt AssignObject(CDrmPlayerUtility* aDrmPlayer);			
	TInt AssignObject(CVideoPlayerUtility* aUtility);			
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();
    
	void ConstructL();
	CTestAudioEqualizerUtility(CStifLogger * aLogger);
	TBool AssignEffect();

	TInt CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ApplyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ModifyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ResetPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt DeletePreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt GetPreset(CStifItemParser *aItem, TTestResult & aResult);	
	TUint32 NumberOfPreDefinePresets(TTestResult & aResult);	
	TInt DisableEffect(CStifItemParser *aItem, TTestResult & aResult);
	TInt BassBoostTest(CStifItemParser *aItem, TTestResult & aResult);
	
    void PrintPresetNamesL();

	TUint32 iBandId;

};



/*==================================================================================
//  CTestEnvironmentalReverbUtility
//
//  This class test the Environmental Reverb Utility
====================================================================================*/
class CTestEnvironmentalReverbUtility: public CTestAudioEffectPreset
{
public:
	// Destructor
	~CTestEnvironmentalReverbUtility();

	// Static
	static CTestEnvironmentalReverbUtility *NewLC (CStifLogger * aLogger);
	static CTestEnvironmentalReverbUtility *NewL (CStifLogger * aLogger);

	// virtual
	TInt AssignObject(CMdaAudioPlayerUtility* aPlayer);
	TInt AssignObject(CMdaAudioRecorderUtility* aRecorder);
	TInt AssignObject(CMdaAudioConvertUtility* aConverter);
	TInt AssignObject(CMdaAudioToneUtility* aTone);
	TInt AssignObject(CMMFDevSound* aDevSound);
	TInt AssignObject(CCustomCommandUtility* aCustomCommand);
	TInt AssignObject(MCustomInterface* aCustomInterface);
	TInt AssignObject(CMdaAudioInputStream* aInputStream);
	TInt AssignObject(CMdaAudioOutputStream* aOutputStream);
	TInt AssignObject(CMidiClientUtility* aMidiClientUtility);				
	TInt AssignObject(CDrmPlayerUtility* aDrmPlayer);					
	TInt AssignObject(CVideoPlayerUtility* aUtility);				
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks

	void ConstructL();
	CTestEnvironmentalReverbUtility(CStifLogger * aLogger);
	TBool AssignEffect();

	TInt CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt GetPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt NumberOfPreDefinedPresets(TTestResult & aResult);
	TInt ApplyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ModifyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ResetPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt DeletePreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisableEffect(CStifItemParser *aItem, TTestResult & aResult);

    void PrintPresetNamesL();
};


/*==================================================================================
//  CTestStereoWideningUtility
//
//  This class test the Stereo Widening Utility
====================================================================================*/
class CTestStereoWideningUtility: public CTestAudioEffectPreset
{
public:
	// Destructor
	~CTestStereoWideningUtility();

	// Static
	static CTestStereoWideningUtility *NewLC (CStifLogger * aLogger);
	static CTestStereoWideningUtility *NewL (CStifLogger * aLogger);

	// virtual
	TInt AssignObject(CMdaAudioPlayerUtility* aPlayer);
	TInt AssignObject(CMdaAudioRecorderUtility* aRecorder);
	TInt AssignObject(CMdaAudioConvertUtility* aConverter);
	TInt AssignObject(CMdaAudioToneUtility* aTone);
	TInt AssignObject(CMMFDevSound* aDevSound);
	TInt AssignObject(CCustomCommandUtility* aCustomCommand);
	TInt AssignObject(MCustomInterface* aCustomInterface);
	TInt AssignObject(CMdaAudioInputStream* aInputStream);
	TInt AssignObject(CMdaAudioOutputStream* aOutputStream);
	TInt AssignObject(CMidiClientUtility* aMidiClientUtility);		
	TInt AssignObject(CDrmPlayerUtility* aDrmPlayer);			
	TInt AssignObject(CVideoPlayerUtility* aUtility);				
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);


private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks

	void ConstructL();
	CTestStereoWideningUtility(CStifLogger * aLogger);
	TBool AssignEffect();

	TInt CreateNewPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt GetPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt NumberOfPreDefinedPresets(TTestResult & aResult);
	TInt ApplyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ModifyPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt ResetPreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt DeletePreset(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisableEffect(CStifItemParser *aItem, TTestResult & aResult);

    void PrintPresetNamesL();
};

#endif  //EFFECT_PRESET_TEST_CORE