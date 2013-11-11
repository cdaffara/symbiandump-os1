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
* Description: DRM Player  DRMAudioPlay test module.
*
*/



#ifndef DRMAUDIOPLAY_H
#define DRMAUDIOPLAY_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

//#include "badesca.h"
//#include "ecom.h"
//#include "mmfcontrollerpluginresolver.h"
#include <StifParser.h>
#include "SimpleSoundPlayer.h"
#include "MediaEvent.h"
#include "FileNameParameters.h"
#include "Des8Parameters.h"
#include "UrlParameters.h"
#include <f32file.h>
#include <s32file.h>
#include <mmf/common/mmcaf.h>


	const TInt KErrConfigInvalid = KErrNone;	//-1100;
	const TInt KErrExpectedValueDifferent = -1502;	//-1101
	const TInt KErrSkippedEvents = -1102;
	const TInt KErrNotExecuted = -1003;
	const TInt KErrExecuted = KErrNone;
	const TInt KErrLeft = -1503;

	_LIT(KSectionStart, "[general]");
	_LIT(KSectionEnd, "[general_end]");

	_LIT(KTestCaseConfigStart, "[TestCase]");
	_LIT(KTestCaseConfigEnd, "[End_TestCase]");

	_LIT(KTagId, "id");
	_LIT(KTagTitle, "Title");

	_LIT(KTagSoundFile, "SoundFile");
	_LIT(KTagSoundUrl, "SoundUrl");
	_LIT(KTagSoundDes, "SoundDes");

	_LIT(KTagRightsFile, "RightsFile");
#ifdef __WINS__
	_LIT(KTagOMADRMRightsDir, "c:\\private\\101F51F2\\Import\\");
#else
    _LIT(KTagOMADRMRightsDir, "\\private\\101F51F2\\Import\\");
#endif

#ifdef __WINS__
    _LIT(KTagOMADRMContentDir, "c:\\private\\101F51F2\\OMA DRM Agent\\");
#else
    _LIT(KTagOMADRMContentDir, "\\private\\101F51F2\\OMA DRM Agent\\");
#endif

	const TInt KMaxFileLength = 256;

	_LIT(KTagRepeatTimes, "RepeatTimes");
	_LIT(KTagSilenceDuration, "SilenceDuration");

	_LIT(KTagRampDuration, "RampDuration");

	_LIT(KTagPosition, "Position");
	_LIT(KTagErrorRange, "ErrorRange");

	_LIT(KTagDelay, "Delay");
	_LIT(KTagBalance, "Balance");

	_LIT(KTagPauseDelay, "PauseDelay");
	_LIT(KTagPlayDelay, "PlayDelay");
	_LIT(KTagStopDelay, "StopDelay");

	_LIT(KTagExpectedDuration, "ExpectedDuration");
    _LIT(KTagMaxVolume, "MaxVolume");
    _LIT(KTagSetVolume, "SetVolume");
    _LIT(KTagSetBalance, "SetBalance");
    _LIT(KTagGetBalance, "GetBalance");
    _LIT(KTagGetBitRate, "GetBitRate");
    _LIT(KTagSetPriority, "SetPriority");
    _LIT(KTagSetRepeats, "SetRepeats");
	_LIT(KTagWindowStart, "StartPosition");
	_LIT(KTagWindowEnd, "EndPosition");
	_LIT(KTagGetMetaDataEntry, "GetMetaDataEntry");
	_LIT(KTagGetNumberOfMetaDataEntries, "GetNumberOfMetaDataEntries");


	_LIT(KTagMime, "MimeType");
	_LIT(KTagIapId, "IapId");

	_LIT(KTagInterfaceId, "InterfaceId");

	_LIT(KTagPriority, "Priority");
	_LIT(KTagPriorityPreference, "PriorityPreference");

	_LIT(KTagImmediate, "Immediate");
	_LIT(KTagDesReadOnly, "DesReadOnly");
	_LIT(KTagMetaInfoTest, "MetaInfoTest");
	_LIT(KTagFileType, "FileType");

	_LIT(KLogFileTag, "logFileName");
	_LIT(KLogPathTag, "logPath");

	_LIT(KDefLogFile, "DRMAudioPlay.txt");
	_LIT(KDefLogPath, "");

	_LIT(KTagDRMRightsFile, "DRMRightsFile");

	//Log literals
	//_LIT(KTestCaseTitleFormat, "MMFTS.PLAY.%d-%S");
	_LIT(KTestCaseTitleFormat, "%$2$S - %$1$d");
	//_LIT(KDefaultLogger, "Using Default Logger");
	_LIT(KLoggerStarted, "Started logger for INI file [%S]");
	_LIT(KOpeningConfig, "Opening configuration file [%S]");
	_LIT(KOpeningLogFilepath, "Opening logfile path [%S]");
	_LIT(KConfigRequired, "TestCases need to be configured in file");
	_LIT(KConfigInvalid, "TestCases need to be configured properly (file might have change between GetTestCases and RunTestCase)");

	//Resultados
	_LIT(KTestCaseResultNotImplemented, "TestCase not implemented");
	_LIT(KTestCaseResultSuccess, "successful");
	_LIT(KTestCaseResultFail, "Failed");
	_LIT(KTestCaseResultLeft, "A LEAVE occurred");

	_LIT(KTestCaseResultFailExpectedValueDifferent, "Expected result is different from actual result");
	_LIT(KTestCaseResultFailSkippedEvents, "Not enough events took place to complete test");

	_LIT(KTestCaseNotFound, "The TestCase id is invalid");
// MACROS
//#define ?macro ?macro_def

// Logging path
//_LIT( KDRMAudioPlayLogPath, "" );
// Log file
//_LIT( KDRMAudioPlayLogFile, "DRMAudioPlay.txt" );

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION

/**
*  This a DRMAudioPlay class.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CDRMAudioPlay) : public CTestModuleBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDRMAudioPlay* NewL();

        /**
        * Destructor.
        */
        virtual ~CDRMAudioPlay();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1
        * @return
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the DRMAudioPlay.
        *       It is called once for every instance of DRMAudioPlay after
        *       its creation.
        * @since ?Series60_version
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first
        *               created instance of DRMAudioPlay.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases
        *   from DRMAudioPlay.
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
		TBool IsDurationInRange(TInt aRepeatTimes, TTimeIntervalMicroSeconds aToneDuration, TTimeIntervalMicroSeconds aRepeatSilence, TTimeIntervalMicroSeconds aErrorRange , TTime aStartTime , TTime aEndTime ) ;

		TInt TestCasePlayFileL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWithVolumeRampL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWithRepeatsL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWithPositionL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWithBalanceL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWithPauseL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileStopOpenL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileReportPositionL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileReportDurationL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileMaxVolumeL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileSetVolumeL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileSetBalanceL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileGetBalanceL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileGetBitRateL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileSetPriorityL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFileSetRepeatsL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWindowL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileWindowClearL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFilesWithPrioritiesL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFilesWithVolumesL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFilesGetMetaDataEntryL(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayFilesGetNumberOfMetaDataEntriesL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayDesL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayDesStopOpenL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayUrlL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayUrlStopOpenL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileMetaInfoL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFileFormatsL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerStopOpenL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithPauseL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithRampL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithSetPositionL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithGetPositionL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithDurationL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithWindowL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayHandlerWithClearWindowL(CStifSectionParser *section , TTestResult &aResult) ;
		TInt TestCasePlayFilesGetAudioLoadingProgress(CStifSectionParser *section , TTestResult &aResult);
		TInt TestCasePlayFilesRegisterForAudioLoadingNotification(CStifSectionParser *section , TTestResult &aResult);
		TInt TestCasePlayFilesImplementationInformation(CStifSectionParser *section , TTestResult &aResult);
		TInt TestCasePlayRepeatDRMFile(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayPauseRepeatDRMFile(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayRepeatDRMFileForever(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayRepeatDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayPauseRepeatDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult) ;
        TInt TestCasePlayRepeatForeverDRMFileWithPlayWindow(CStifSectionParser *section , TTestResult &aResult) ;
    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CDRMAudioPlay();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        //TTimeIntervalMicroSeconds GetDelayL(CStifSectionParser *aSection, const TDesC &aTag=KTagDelay);
		TTimeIntervalMicroSeconds32 GetTimeIntervalL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefault, const TTimeIntervalMicroSeconds32 &aDefaultInterval);
		TTimeIntervalMicroSeconds GetTimeIntervalL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefault, const TTimeIntervalMicroSeconds &aDefaultInterval=TTimeIntervalMicroSeconds(-1) );

		HBufC8 *GetDescriptor8LC(const TFileName &aFileName,CStifSectionParser *aSection );

		TInt GetIntL(CStifSectionParser *aSection, const TDesC &aTag, TBool &aUsingDefault, const TInt &aDefaultInt, TBool aContinue=EFalse) ;

		void TestCasePreCheck(CStifSectionParser *section, TFileName& aFileName);
		void ProcessRightsL(const TDesC8& aRightsXMLContent);
    public:     // Data
        static const TInt MAX_TESTCASE_ID;
		static const TInt KDefaultRepeats;
		static const TInt KDefaultRepeatForever;

#define KDefaultSilenceDuration  (TTimeIntervalMicroSeconds ((TInt64)500000))
#define KDefaultRampDuration  (TTimeIntervalMicroSeconds ((TInt64)2000000))
#define KDefaultPosition  (TTimeIntervalMicroSeconds ((TInt64) 2000000))
#define KDefaultErrorRange (TTimeIntervalMicroSeconds ((TInt64) 250000))
		static const TInt KDefaultPauseDelay;
		static const TInt KDefaultPlayDelay;
		static const TInt KDefaultStopOpenPlayDelay;
		static const TInt KDefaultReportDelay;
		//Cases
		static const TInt KCasePlayFile;
		static const TInt KCasePlayFileWithRepeats;
		static const TInt KCasePlayFileWithRamp;
		static const TInt KCasePlayFileWithPause;
		static const TInt KCasePlayFileWithPosition;
		static const TInt KCasePlayFileWithBalance;
		static const TInt KCasePlayFileStopOpen;
		static const TInt KCasePlayFileReportPosition;
		static const TInt KCasePlayFileReportDuration;
        static const TInt KCasePlayFileMaxVolume;
        static const TInt KCasePlayFileSetVolume;
        static const TInt KCasePlayFileSetBalance;
        static const TInt KCasePlayFileGetBalance;
        static const TInt KCasePlayFileGetBitRate;
        static const TInt KCasePlayFileSetPriority;
        static const TInt KCasePlayFileSetRepeats;
		static const TInt KCasePlayFileWindow;
		static const TInt KCasePlayFileClearWindow;
		static const TInt KCasePlayFilesWithPriorities;
		static const TInt KCasePlayFilesWithVolumes;
		static const TInt KCasePlayFilesGetMetaDataEntry;
		static const TInt KCasePlayFilesGetNumberOfMetaDataEntries;
		static const TInt KCasePlayFileGetAudioLoadingProgress;
		static const TInt KCasePlayFileRegisterForAudioLoadingNotification;
		static const TInt KCasePlayFileImplementationInformation;
		static const TInt KCasePlayDes;
		static const TInt KCasePlayDesStopOpen ;
		static const TInt KCasePlayUrl;
		static const TInt KCasePlayUrlStopOpen ;
		static const TInt KCasePlayFileMetaInfo;
		static const TInt KCaseImplementationInformation;
		static const TInt KCasePlayHandler;
		static const TInt KCasePlayHandlerStopOpen;
		static const TInt KCasePlayHandlerWithPause;
		static const TInt KCasePlayFileHandlerWithRamp;
		static const TInt KCasePlayFileHandlerWithSetPosition;
		static const TInt KCasePlayFileHandlerWithGetPosition;
		static const TInt KCasePlayFileHandlerWithDuration;
		static const TInt KCasePlayFileHandlerWithWindow;
		static const TInt KCasePlayFileHandlerWithClearWindow;
		static const TInt KCasePlayRepeatDRMFile;
        static const TInt KCasePlayPauseRepeatDRMFile;
		static const TInt KCasePlayRepeatDRMFileForever;
		static const TInt KCasePlayRepeatDRMFileWithPlayWindow;
        static const TInt KCasePlayPauseRepeatDRMFileWithPlayWindow;
		static const TInt KCasePlayRepeatForeverDRMFileWithPlayWindow;

    protected:  // Data

    private:    // Data

        CStifLogger *iLogger;
		CActiveScheduler* iScheduler;
		TBool iDRMPreCheckPassed;
    public:     // Friend classes
    	RFs				iFs;
    //	RFs				iFs2;
		RFile			iFile;
	//	RFile			iFile2;

    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // DRMAUDIOPLAY_H

// End of File
