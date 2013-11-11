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
* Description:  AudioroutingControl test module
*
*/



#ifndef CAudioRoutingTestClass_H
#define CAudioRoutingTestClass_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <customInterfaceUtility.h>
#include <CustomCommandUtility.h>
#include <SoundDevice.h>
#include <AudioInput.h>
#include <AudioOutput.h>
#include <mdaaudiosampleplayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioOutputStream.h>
#include <MdaAudioInputStream.h>
#include <MdaAudioTonePlayer.h>
#include <MidiClientUtility.h>
#include <MAudioOutputObserver.h>
#include <DrmAudioSamplePlayer.h>
#include <videoplayer.h>
#include <VideoRecorder.h>
#include "TimeoutController.h"

#include <COEDEF.H>


// CONSTANTS
const TUint screenWidth  = 0;
const TUint screenHeight = 0;

const TInt KErrBadTestParameter= -1000;         // Error on configuration file

const TInt KErrEventPending =-2000;             // Timeout and not callback
const TInt KErrCallbackErrorCode = -2001;       // Callback return error code
const TInt KErrUnexpectedValue =-2002;          // Unexpected value at setting
const TInt KErrTimeoutController = -2007;		// Unexpected notification

const TInt KShortTimeout = 1000;
const TInt KMediumTimeout = 4000;
const TInt KLongTimeout = 100000;

//for record with input stream a pcm bit buffer
const TInt KStreamBufferSize 	= 320;
const TInt KStreamBufferCount 	= 2;
const TInt KMaxFileSize 	= 500000;
/************************************************/

_LIT( KMsgBadTestParameters, "[Error] No valid test case parameters");

// General
_LIT( KTagG711, "G711");
_LIT( KTagOutput32Bit, "Output32Bit");
_LIT( KTagDownMixToStereo, "DownMixToStereo");
_LIT( KTagLostDataConcealment, "LostDataConcealment");
_LIT( KTagTrue, "True");
_LIT( KTagFalse, "False");


_LIT( KTagOuputRecord, "OutputRecord");
_LIT( KTagDevSound, "DevSound");
_LIT( KTagDevSound1, "DevSound1");
_LIT( KTagCustomInterface, "CustomInterface");
_LIT( KTagCustomCommand, "CustomCommand");
_LIT( KTagInputStream, "InputStream");
_LIT( KTagOutputStream, "OutputStream");
_LIT( KTagToneUtility, "ToneUtility");\
_LIT( KTagCustomInterfaceOutput, "CustomInterfaceOutput");
_LIT( KTagRecorder, "Recorder");
_LIT( KTagAudioPlayer, "AudioPlayer");
_LIT( KTagCustomCommandOutput, "CustomCommandOutput");
_LIT( KTagMidiClient, "MidiClient");
_LIT( KTagDrmPlayer, "DrmPlayer");
_LIT( KTagVideoPlayer, "VideoPlayer");
_LIT( KTagVideoRecord, "VideoRecord");

#ifdef __WINSCW__
_LIT( KAudioMidiFile, "c:\\testing\\data\\aqualung.mid");
_LIT( KAudioDrmFile, "c:\\testing\\data\\test.amr");
_LIT(K3gpTestFile,"c:\\testing\\data\\videoTest.3gp");
_LIT(KMp3TestFile,"c:\\testing\\data\\mp3mp2s8b17.mp3");
#else
_LIT( KAudioMidiFile, "e:\\testing\\data\\aqualung.mid");
_LIT( KAudioDrmFile, "e:\\testing\\data\\test.amr");
_LIT(K3gpTestFile,"e:\\testing\\data\\videoTest.3gp");
_LIT(KMp3TestFile,"e:\\testing\\data\\mp3mp2s8b17.mp3");
#endif

class CVideoRecorderUtility;
// MACROS
//#define ?macro ?macro_def
// Logging path
#ifdef __WINSCW__
_LIT( KAudioRoutingTestClassLogPath, "\\logs\\testframework\\" );
#else
_LIT( KAudioRoutingTestClassLogPath, "e:\\testing\\Log\\" );
#endif

// Log file
_LIT( KAudioRoutingTestClassLogFile, "AudioRoutingTestClass.txt" );


// FORWARD DECLARATIONS
class CAudioRoutingTestClass;


// CLASS DECLARATION

/**
*  CRadioUtilityTestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60_3_2
*/
NONSHARABLE_CLASS(CAudioRoutingTestClass) : public CScriptBase,
											public MTimeoutObserver,
											public MDevSoundObserver,
											public MMdaObjectStateChangeObserver,
											public MMdaAudioPlayerCallback,
											public MMdaAudioInputStreamCallback,
											public MAudioOutputObserver,
						 					public MMdaAudioOutputStreamCallback,
						 					public MMdaAudioToneObserver,
										    public MMidiClientUtilityObserver,
										    public MDrmAudioPlayerCallback,
										    public MVideoPlayerUtilityObserver,
										    public MVideoRecorderUtilityObserver
										//	public MRadioFmTunerObserver,
										//	public MRadioPlayerObserver,
										//	public MRadioRdsObserver
											/*public MRadioPresetObserver*/
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAudioRoutingTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CAudioRoutingTestClass();

		enum TG711ExpectedEvent
		{
			EInitializeComplete,

		};

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );


		// Required for MDevSoundObserver:
		void InitializeComplete(TInt /*aError*/);
		void ToneFinished(TInt /*aError*/);
		void BufferToBeFilled(CMMFBuffer* /*aBuffer*/);
		void PlayError(TInt /*aError*/);
		void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/);
		void RecordError(TInt /*aError*/);
		void ConvertError(TInt /*aError*/);
		void DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/);

		virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorcCode);

		void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
		void MapcPlayComplete(TInt aError);

	    // Audio Input Stream
    	virtual void MaiscOpenComplete(TInt aError);
	    virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
    	virtual void MaiscRecordComplete(TInt aError);


    	virtual void DefaultAudioOutputChanged( CAudioOutput& aAudioOutput,
    		                                CAudioOutput::TAudioOutputPreference aNewDefault );

	    // Audio Output Stream
    	virtual void MaoscOpenComplete(TInt aError);
	    virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
    	virtual void MaoscPlayComplete(TInt aError);

    	// Audio Tone Observer
	    virtual void MatoPrepareComplete(TInt aError);
    	virtual void MatoPlayComplete(TInt aError);

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

	    //MDrmAudioPlayerCallback
		virtual void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
		virtual void MdapcPlayComplete(TInt aError);

		//MVideoPlayerUtilityObserver
		virtual void MvpuoOpenComplete(TInt aError);
		virtual void MvpuoPrepareComplete(TInt aError);
		virtual void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
		virtual void MvpuoPlayComplete(TInt aError);
		virtual void MvpuoEvent(const TMMFEvent& aEvent);

		//MVideoRecorderUtilityObserver
		virtual void MvruoOpenComplete(TInt aError);
		virtual void MvruoPrepareComplete(TInt aError);
		virtual void MvruoRecordComplete(TInt aError);
		virtual void MvruoEvent(const TMMFEvent& aEvent);

	    // From MTimeoutObserver

	    /**
	    * @since ?Series60_version
	    * @param none
	    * Review if all the expected events have ocurred once the time is over
	    */
	    void HandleTimeout(TInt error);




    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CAudioRoutingTestClass( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        TInt Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */

	    /**
	    * Set an event as expected and set default timeout
	    * @since ?Series60_version
	    */
	    void AddExpectedEvent(TG711ExpectedEvent event, TInt ms);


	    /**
	    * Unset an event as expected
	    * @since ?Series60_version
	    */
	    TBool RemoveExpectedEvent(TG711ExpectedEvent event);


	    /**
	    * @since ?Series60_version
	    * @param none
	    * Removes all expected events
	    */
	    void RemoveAllExpectedEvents();

	    /**
	    * Verify that the event was expected, removes it from the list
	    * Signal the TestScripter with the returned error code
	    * @since ?Series60_version
	    */
	    void ProcessEvent(TG711ExpectedEvent aEvent, TInt aError);

	    /**
	    * Maps a event with a descriptor with its name
	    * @since ?Series60_version
	    */
	    TPtrC EventName( TInt aKey );

	    /*
	    * Test methods are listed below.
	    */

	    /**
	    * Sets a timeout different since the default
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetTimeout( CStifItemParser& aItem );

	    /**
	    *
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetExpectedEvents( CStifItemParser& aItem );

	    /**
	    *
	    * @since Series60_3_2
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */
	    TInt SetAllowedPanic( CStifItemParser& aItem );

	    /**
	    * Connect to the default client to the server
	    * @since ?Series60_version
	    * @param aItem Script line containing parameters.
	    * @return Symbian OS error code.
	    */

		TInt ExampleL( CStifItemParser& aItem );
		//TInt CreateDevSoundInstance( CStifItemParser& aItem );
		//TInt CreateDevSoundInstance_1( CStifItemParser& aItem );
		TInt CreateObject( CStifItemParser& aItem );
		TInt CreateAudioInput( CStifItemParser& aItem );
		void CreateInput();
		void CreateOutput();
		TInt CreateVideoInput();

		/*To create and destroy the window to start the CVideoPlayerUtility */
		TInt CreateVideoWindow();
		TInt DeleteVideoWindow();
		TInt CreateSessionAndScreen();
		TInt CreateVideoObject();
		TInt CreateWindowGroup();
		TInt EndDrawing();
		TInt CreateVideoRecorderObject();
		TInt AudioInput();
		TInt SetAudioInputL();
		TInt AudioOutput();
		TInt DefaultAudioOutput();
		TInt RegisterObserverL();
		TInt SecureOutput();
		TInt SetAudioOutputL();
		TInt SetSecureOutputL();
		TInt UnregisterObserver();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

    	// reference to TestModuleIf
    	CTestModuleIf& 				iTestModuleIf;
    	CAudioInput* 				iAudioInput;
    	CAudioOutput* 				iAudioOutput;
    	CMdaAudioRecorderUtility* 	iRecorder;
    	CVideoRecorderUtility* 		iVideoRecord;
    	CMdaAudioPlayerUtility* 	iUtil;
    	TBool						isInput;
		CMMFDevSound*				iDevSound;
		CCustomInterfaceUtility*   	iCustomInterfaceUtility;
		CCustomCommandUtility*     	iCustomCommandUtility;
		CMdaAudioInputStream*    	iAudioInputStream;
		CMdaAudioOutputStream*    	iAudioOutputStream;
		CMdaAudioToneUtility*     	iAudioTone;
		CMidiClientUtility*       	iMidiClientUtility;
		CDrmPlayerUtility* 		  	iDrmPlayerUtility;
		TPtrC 						iObject;
		TBool 						iInputOutput;
    	CVideoPlayerUtility*      	iVideoPlayerUtility;
		// Active object with a timer to timeout the test case
		CSimpleTimeout * iTimeoutController;

		// Indicates if the test case use a normal exit reason
		TBool iNormalExitReason;

		// List of expected events
		RArray<TG711ExpectedEvent> iExpectedEvents;

		// List of notification event (by the callback)
		RArray<TG711ExpectedEvent> iOcurredEvents;
		//TInt error;
		/***********INPUT STREAM VARS ******************/
		//CMdaAudioInputStream* 	iInputStream;
		RPointerArray<TDes8> 	iStreamBuffer;
		TMdaAudioDataSettings 	iStreamSettings;
		TInt 					iStreamIdx,iPlayError;
		HBufC8* 				iSoundData;
		TBool 					iReady;
		/************************************************/

		/***********DUMMY WINDOW VARS ******************/
        RWsSession iWindowSession;
        CWsScreenDevice* iScreenDevice;
        CWindowGc* iWindowGc;
        TRect iRect;
        RWindow iWindow;
        RWindowGroup iWindowGroup;
		/***********************************************/
		// ?one_line_short_description_of_data
		//?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;

    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CAudioRoutingTest_H

// End of File
