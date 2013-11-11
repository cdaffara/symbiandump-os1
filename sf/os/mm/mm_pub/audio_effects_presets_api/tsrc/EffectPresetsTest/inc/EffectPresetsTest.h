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
* Description:  EffectPresetsTest test module.
*
*/



#ifndef EFFECTPRESETSTEST_H
#define EFFECTPRESETSTEST_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <StifParser.h>
#include <f32file.h>
#include <s32file.h>
#include "EffectPresetTestConstants.h"

#include <mdaaudiosampleplayer.h>
#include <MdaAudioSampleEditor.h>
#include <MdaAudioTonePlayer.h>
#include <SoundDevice.h>
#include <MdaAudioInputStream.h>
#include <MdaAudioOutputStream.h>
#include <CustomInterfaceUtility.h>
#include <CustomCommandUtility.h>
#include <MidiClientUtility.h>
#include <DrmAudioSamplePlayer.h>
#include <videoplayer.h>

//#include <coedef.h>      // TCoeWinPriority
//#include <e32base.h>    // CTrapCleanup
#include <w32std.h>     // RWsSession
#include <coecntrl.h>    // CCoeControl
//#include <coemain.h>    // CCoeEnv

// CONSTANTS
//const ?type ?constant_var = ?constant;

// S60 screen size
const TUint screenWidth  = 0;
const TUint screenHeight = 0;
// MACROS
//#define ?macro ?macro_def

// Logging path
//_LIT( KEffectPresetsTestLogPath, "\\logs\\testframework\\EffectPresetsTest\\" ); 
// Log file
//_LIT( KEffectPresetsTestLogFile, "EffectPresetsTest.txt" ); 

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION
class CTestAudioEffectPreset;

/**
*  This a EffectPresetsTest class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CEffectPresetsTest) : public CTestModuleBase,
										public MMdaAudioPlayerCallback,
										public MMdaObjectStateChangeObserver,
					 					public MMdaAudioToneObserver,
					 					public MMdaAudioInputStreamCallback,
					 					public MMdaAudioOutputStreamCallback,
									    public MMidiClientUtilityObserver,		
									    public MDrmAudioPlayerCallback,			
									    public MVideoPlayerUtilityObserver		
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CEffectPresetsTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CEffectPresetsTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

		// MMdaAudioPlayerCallback
		virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
		virtual void MapcPlayComplete(TInt aError);
		// MMdaObjectStateChangeObserver
		virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorcCode);
    	// Audio Tone Observer
	    virtual void MatoPrepareComplete(TInt aError);
    	virtual void MatoPlayComplete(TInt aError);
	    // Audio Input Stream
    	virtual void MaiscOpenComplete(TInt aError);
	    virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
    	virtual void MaiscRecordComplete(TInt aError);
	    // Audio Output Stream
    	virtual void MaoscOpenComplete(TInt aError);
	    virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
    	virtual void MaoscPlayComplete(TInt aError);
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

        /**
        * From CTestModuleBase InitL is used to initialize the EffectPresetsTest. 
        *       It is called once for every instance of EffectPresetsTest after
        *       its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of EffectPresetsTest.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from EffectPresetsTest. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aTestCaseFile,
                           TTestResult& aResult );

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& /* aFailureType */,
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ );

        /**
        * From CTestModuleBase; User may add implementation for OOM test 
        * environment initialization. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                    const TInt /* aCaseNumber */ ); 

        /**
        * From CTestModuleBase; OOMHandleWarningL. User may add implementation 
        * for OOM test warning handling. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */);

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment. Usually no implementation is required.
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ );

    protected:  // New functions

        void InitLoggerL( const TFileName& aLogPath , const TFileName& aLogFile );
		void InitLoggerL();
		TInt RunTestCaseFunctionL(TInt id, CStifSectionParser *section, TTestResult &aResult) ;

		TInt TestCaseAudioEffectPresetL(CStifSectionParser *aSection, TTestResult & aResult);

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CEffectPresetsTest();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

		void deleteObject();

		TInt CreateEffectUtilityL(CStifSectionParser *aSection, TTestResult & aResult);
        TInt AssignEffectObjectL(CStifSectionParser *aSection, TTestResult & aResult);
		TInt SetUtility(TPtrC aAudioEffectUtilityName);
		TInt AssignObject(TPtrC aAssignEffectObjectName);
		/*To create and destroy the window to start the CVideoPlayerUtility */
		TInt CreateVideoWindow();
		TInt DeleteVideoWindow();
		TInt CreateSessionAndScreen();
		TInt CreateVideoObject();
		TInt CreateWindowGroup();
		TInt EndDrawing();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
		static const TInt MAX_TESTCASE_ID;
		static const TInt KTestCaseEffectPreset;

        CStifLogger *iLogger;
		CActiveScheduler* iScheduler;

		TObject iObject;
		CTestAudioEffectPreset *iTestAudioEffectPreset;
	
		CMdaAudioPlayerUtility*   iAudioPlayer;
		CMdaAudioRecorderUtility* iAudioRecorder;
		CMdaAudioConvertUtility*  iAudioConverter;
		CMdaAudioToneUtility*     iAudioTone;
		CMMFDevSound*             iDevSound;
		CMdaAudioInputStream*     iAudioInputStream;
		CMdaAudioOutputStream*    iAudioOutputStream;
		CCustomCommandUtility*    iCustomCommandUtility;
		CCustomInterfaceUtility*  iCustomInterfaceUtility;
    	CMidiClientUtility*       iMidiClientUtility;		
    	CDrmPlayerUtility* 		  iDrmPlayerUtility;		
    	CVideoPlayerUtility*      iVideoPlayerUtility;		
    public:     // Friend classes
        //?friend_class_declaration;
        RWsSession iWindowSession;
        CWsScreenDevice* iScreenDevice;
        CWindowGc* iWindowGc;
        TRect iRect;
        RWindow iWindow;
        RWindowGroup iWindowGroup;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

/**
*  Dummy CCoeControl for VideoPlayerUtility
*/
#endif      // EFFECTPRESETSTEST_H

// End of File
