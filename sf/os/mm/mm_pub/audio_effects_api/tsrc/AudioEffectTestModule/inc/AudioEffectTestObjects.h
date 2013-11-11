/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: AudioEffectTestModule
*
*/



#ifndef AUDIO_EFFECT_TEST_OBJECTS
#define AUDIO_EFFECT_TEST_OBJECTS

#include "AudioEffectTestModule.h"

#include <AudioEffectBase.h>
#include <MAudioEffectObserver.h>
#include <MAudioEqualizerObserver.h>
#include <MBassBoostObserver.h>
#include <MEnvironmentalReverbObserver.h>
#include <MDistanceAttenuationObserver.h>
#include <MListenerDopplerObserver.h>
#include <MSourceDopplerObserver.h>
#include <MListenerLocationObserver.h>
#include <MSourceLocationObserver.h>
#include <MLoudnessObserver.h>
#include <MListenerOrientationObserver.h>
#include <MSourceOrientationObserver.h>
#include <MStereoWideningObserver.h>
#include <MRoomLevelObserver.h>


// CLASS DECLARATION
class CEnvironmentalReverbUtility;

/*==================================================================================
//  CTestEffectBase
//
//  This class is the base class for the test effects classes.
====================================================================================*/
class CTestAudioEffect: public MAudioEffectObserver
{
public:
	CTestAudioEffect(CStifLogger * aLogger);
	~CTestAudioEffect();

	// public methods
	virtual TInt CreateEffect() = 0;
	virtual TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer) = 0;
	virtual TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder) = 0;
	virtual TInt CreateEffect(CMdaAudioConvertUtility* aConverter) = 0;
	virtual TInt CreateEffect(CMdaAudioToneUtility* aTone) = 0;
	virtual TInt CreateEffect(CMMFDevSound* aDevSound) = 0;
	virtual TInt CreateEffect(CCustomCommandUtility* aCustomCommand) = 0;
	virtual TInt CreateEffect(MCustomInterface* aCustomInterface) = 0;
	virtual TInt CreateEffect(CMdaAudioInputStream* aInputStream) = 0;
	virtual TInt CreateEffect(CMdaAudioOutputStream* aOutputStream) = 0;
	virtual TInt CreateEffect(CMidiClientUtility* aMidiClient) = 0;
	virtual TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer) = 0;
	virtual TInt CreateEffect(CVideoPlayerUtility* aUtility) = 0;
	virtual TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

protected:
	// Callbacks
	virtual void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	void PrintError(TInt err);
	void DisplayCommon();

	CAudioEffect* iAudioEffect;
	CStifLogger*  iLogger;

	TBool iRegister;
	TBool iChange;

private:
    virtual void DeleteAudioEffect()=0;

    TInt RegisterEffect(CStifItemParser *aItem, TTestResult & aResult);
    TInt UnRegisterEffect(CStifItemParser *aItem, TTestResult & aResult);
    TInt ApplySettings(CStifItemParser *aItem, TTestResult & aResult);
    TInt Enable(CStifItemParser *aItem, TTestResult & aResult);
    TInt Disable(CStifItemParser *aItem, TTestResult & aResult);
    TInt Enforce(CStifItemParser *aItem, TTestResult & aResult);
};


/*==================================================================================
//  CTestAudioEqualizer
//
//  This class test the Audio Equalizer
====================================================================================*/
class CTestAudioEqualizer: public CTestAudioEffect,
                           public MAudioEqualizerObserver
{
public:
	// Destructor
	~CTestAudioEqualizer();

	// Static
	static CTestAudioEqualizer *NewLC (CStifLogger * aLogger);
	static CTestAudioEqualizer *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

    // virtual
    void DeleteAudioEffect();

	// member function
	TInt BandIdLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	void ConstructL();
	CTestAudioEqualizer(CStifLogger * aLogger);

	// member data
	TInt iBandId;
};


/*==================================================================================
//  CTestBassBoost
//
//  This class test the Audio Equalizer
====================================================================================*/
class CTestBassBoost: public CTestAudioEffect,
                      public MBassBoostObserver
{
public:
	// Destructor
	~CTestBassBoost();

	// Static
	static CTestBassBoost *NewLC (CStifLogger * aLogger);
	static CTestBassBoost *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();

	// Callbacks
	virtual void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// member functions
	void ConstructL();
	CTestBassBoost(CStifLogger * aLogger);

	TInt BassBoostLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	// member data
};


/*==================================================================================
//  CTestDistanceAttenuation
//
//  This class test the Distance Attenuation
====================================================================================*/
class CTestDistanceAttenuation: public CTestAudioEffect,
                                public MDistanceAttenuationObserver
{
public:
	// Destructor
	~CTestDistanceAttenuation();

	// Static
	static CTestDistanceAttenuation *NewLC (CStifLogger * aLogger);
	static CTestDistanceAttenuation *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();

	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// Member Function
	void ConstructL();
	CTestDistanceAttenuation(CStifLogger * aLogger);

    TInt DistanceAttenuation(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);
};


/*==================================================================================
//  CTestDoppler
//
//  This class test the Listener and Source Doppler
====================================================================================*/
class CTestDoppler: public CTestAudioEffect,
                    public MDopplerObserver
{
public:
	// Destructor
	~CTestDoppler();

	// Static
	static CTestDoppler *NewListenerLC (CStifLogger * aLogger);
	static CTestDoppler *NewListenerL (CStifLogger * aLogger);
	static CTestDoppler *NewSourceLC (CStifLogger * aLogger);
	static CTestDoppler *NewSourceL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    enum TDopplerType
    {
        EListener,
        ESource
    };

    // virtual
    void DeleteAudioEffect();

	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// member function
	void ConstructL();
	CTestDoppler(CStifLogger * aLogger);

	TInt CartesianVelocity(CStifItemParser *aItem, TTestResult & aResult);
	TInt SphericalVelocity(CStifItemParser *aItem, TTestResult & aResult);
	TInt Factor(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	// 	member data
	TDopplerType iType;
};


/*==================================================================================
//  CTestEnvironmentalReverb
//
//  This class test the Environmental Reverb
====================================================================================*/
class CTestEnvironmentalReverb: public CTestAudioEffect,
                                public MEnvironmentalReverbObserver
{
public:
	// Destructor
	~CTestEnvironmentalReverb();

	// Static
	static CTestEnvironmentalReverb *NewLC (CStifLogger * aLogger);
	static CTestEnvironmentalReverb *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// member function
	void ConstructL();
	CTestEnvironmentalReverb(CStifLogger * aLogger);

	TInt DecayHFRatio(CStifItemParser *aItem, TTestResult & aResult);
	TInt DecayTime(CStifItemParser *aItem, TTestResult & aResult);
	TInt Density(CStifItemParser *aItem, TTestResult & aResult);
	TInt Diffusion(CStifItemParser *aItem, TTestResult & aResult);
	TInt ReflectionsDelay(CStifItemParser *aItem, TTestResult & aResult);
	TInt ReflectionsLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt ReverbDelay(CStifItemParser *aItem, TTestResult & aResult);
	TInt ReverbLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt RoomHFLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt RoomLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);
};


/*==================================================================================
//  CTestLocation
//
//  This class test the Listener and Source Location
====================================================================================*/
class CTestLocation: public CTestAudioEffect,
                     public MLocationObserver
{
public:
	// Destructor
	~CTestLocation();

	// Static
	static CTestLocation *NewListenerLC (CStifLogger * aLogger);
	static CTestLocation *NewListenerL (CStifLogger * aLogger);
	static CTestLocation *NewSourceLC (CStifLogger * aLogger);
	static CTestLocation *NewSourceL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    enum TLocationType
    {
        EListener,
        ESource
    };

    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// member function
	void ConstructL();
	CTestLocation(CStifLogger * aLogger);

	TInt LocationCartesian(CStifItemParser *aItem, TTestResult & aResult);
	TInt LocationSpherical(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	// member data
	TLocationType iType;
};


/*==================================================================================
//  CTestOrientation
//
//  This class test the Listener and Source Orientation
====================================================================================*/
class CTestOrientation: public CTestAudioEffect,
                        public MOrientationObserver
{
public:
	// Destructor
	~CTestOrientation();

	// Static
	static CTestOrientation *NewListenerLC (CStifLogger * aLogger);
	static CTestOrientation *NewListenerL (CStifLogger * aLogger);
	static CTestOrientation *NewSourceLC (CStifLogger * aLogger);
	static CTestOrientation *NewSourceL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    enum TOrientationType
    {
        EListener,
        ESource
    };

    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );

	// member function
	void ConstructL();
	CTestOrientation(CStifLogger * aLogger);

	TInt Orientation(CStifItemParser *aItem, TTestResult & aResult);
	TInt OrientationVectors(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	// member data
	TOrientationType iType;
};


/*==================================================================================
//  CTestLoudness
//
//  This class test the Loudness
====================================================================================*/
class CTestLoudness: public CTestAudioEffect,
                     public MLoudnessObserver
{
public:
	// Destructor
	~CTestLoudness();

	// Static
	static CTestLoudness *NewLC (CStifLogger * aLogger);
	static CTestLoudness *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );
	// member function
	void ConstructL();
	CTestLoudness(CStifLogger * aLogger);

	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);
};


/*==================================================================================
//  CTestStereoWidening
//
//  This class test the Stereo Widening
====================================================================================*/
class CTestStereoWidening: public CTestAudioEffect,
                           public MStereoWideningObserver
{
public:
	// Destructor
	~CTestStereoWidening();

	// Static
	static CTestStereoWidening *NewLC (CStifLogger * aLogger);
	static CTestStereoWidening *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );
	void StereoWideningEvent( TDesC8& aDataPckg );
	// member function
	void ConstructL();
	CTestStereoWidening(CStifLogger * aLogger);

	TInt StereoWideningLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);
};


/*==================================================================================
//  CTestRoomLevel
//
//  This class test the Room Level Effect
====================================================================================*/
class CTestRoomLevel: public CTestAudioEffect,
                      public MRoomLevelObserver,
                      public MMidiClientUtilityObserver
{
public:
	// Destructor
	~CTestRoomLevel();

	// Static
	static CTestRoomLevel *NewLC (CStifLogger * aLogger);
	static CTestRoomLevel *NewL (CStifLogger * aLogger);

	// virtual
	TInt CreateEffect();
	TInt CreateEffect(CMdaAudioPlayerUtility* aPlayer);
	TInt CreateEffect(CMdaAudioRecorderUtility* aRecorder);
	TInt CreateEffect(CMdaAudioConvertUtility* aConverter);
	TInt CreateEffect(CMdaAudioToneUtility* aTone);
	TInt CreateEffect(CMMFDevSound* aDevSound);
	TInt CreateEffect(CCustomCommandUtility* aCustomCommand);
	TInt CreateEffect(MCustomInterface* aCustomInterface);
	TInt CreateEffect(CMdaAudioInputStream* aInputStream);
	TInt CreateEffect(CMdaAudioOutputStream* aOutputStream);
	TInt CreateEffect(CMidiClientUtility* aMidiClient);
	TInt CreateEffect(CDrmPlayerUtility* aDrmPlayer);
	TInt CreateEffect(CVideoPlayerUtility* aUtility);
	TInt ProcessTag(CStifItemParser *aItem, TTestResult & aResult);

    // MidiClientUtilityObserver
    virtual void MmcuoStateChanged(TMidiState aOldState,TMidiState aNewState,const TTimeIntervalMicroSeconds& aTime,TInt aError);
   	virtual void MmcuoTempoChanged(TInt aMicroBeatsPerMinute);
    virtual void MmcuoVolumeChanged(TInt aChannel,TReal32 aVolumeInDecibels);
    virtual void MmcuoMuteChanged(TInt aChannel,TBool aMuted);
    virtual void MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& aMicroSeconds,TInt64 aMicroBeats);
    virtual void MmcuoMetaDataEntryFound(const TInt aMetaDataEntryId,const TTimeIntervalMicroSeconds& aPosition);
    virtual void MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& aMessage);
    virtual void MmcuoPolyphonyChanged(TInt aNewPolyphony);
    virtual void MmcuoInstrumentChanged(TInt aChannel,TInt aBankId,TInt aInstrumentId);

private:
    // virtual
    void DeleteAudioEffect();
	// Callbacks
	void EffectChanged( const CAudioEffect* aAudioEffect, TUint8 aEvent );
	// member function
	void ConstructL();
	CTestRoomLevel(CStifLogger * aLogger);

	TInt RoomLevel(CStifItemParser *aItem, TTestResult & aResult);
	TInt DisplayData(CStifItemParser *aItem, TTestResult & aResult);

	// data
	CEnvironmentalReverbUtility* iEnvironmentalReverbUtility_B;

};

#endif  //AUDIO_EFFECT_TEST_OBJECTS