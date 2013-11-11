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
* Description: WmaDecoderIntfcTest
*
*/



#ifndef CWmaDecoderIntfcTestClass_H
#define CWmaDecoderIntfcTestClass_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <customInterfaceUtility.h>
#include <SoundDevice.h>
#include <WmaDecoderIntfc.h>
#include <MdaAudioInputStream.h>
#include <Mda/Common/Audio.h>
#include "TimeoutController.h"


// CONSTANTS
const TInt KErrBadTestParameter= -1000;         // Error on configuration file

const TInt KErrEventPending =-2000;             // Timeout and not callback
const TInt KErrCallbackErrorCode = -2001;       // Callback return error code
const TInt KErrUnexpectedValue =-2002;          // Unexpected value at setting
const TInt KErrTimeoutController = -2007;		// Unexpected notification

const TInt KShortTimeout = 1000;
const TInt KMediumTimeout = 4000;
const TInt KLongTimeout = 100000;


_LIT( KMsgBadTestParameters, "[Error] No valid test case parameters");

// General
_LIT( KTagWma, "Wma");
_LIT( KTagOutput32Bit, "Output32Bit");
_LIT( KTagDownMixToStereo, "DownMixToStereo");
_LIT( KTagLostDataConcealment, "LostDataConcealment");
_LIT( KTagTrue, "True");
_LIT( KTagFalse, "False");

/* By FC
 * On 06/12/08
 */
_LIT( KTagAudioInStream, "AudioInStream");
_LIT( KTagAudioOutStream, "AudioOutStream");
_LIT( KTagDevSound, "DevSound");
// End by FC

// MACROS
//#define ?macro ?macro_def
// Logging path
#ifdef __WINSCW__
_LIT( KWmaDecoderIntfcTestClassLogPath, "\\logs\\testframework\\" );
#else
_LIT( KWmaDecoderIntfcTestClassLogPath, "e:\\testing\\Log\\" );
#endif

// Log file
_LIT( KWmaDecoderIntfcTestClassLogFile, "WmaDecoderIntfcTestClass.txt" );


// FORWARD DECLARATIONS
class CWmaDecoderIntfcTestClass;


// CLASS DECLARATION

/**
*  CRadioUtilityTestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60_3_2
*/
NONSHARABLE_CLASS(CWmaDecoderIntfcTestClass) : public CScriptBase,
											public MTimeoutObserver,
											public MDevSoundObserver,
											public MMdaAudioInputStreamCallback
										//	public MRadioFmTunerObserver,
										//	public MRadioPlayerObserver,
										//	public MRadioRdsObserver
											/*public MRadioPresetObserver*/
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CWmaDecoderIntfcTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CWmaDecoderIntfcTestClass();

		enum TWmaExpectedEvent
		{
			EInitializeComplete,
			EMaiscOpenComplete,

		};

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


	    // From MTimeoutObserver

	    /**
	    * @since ?Series60_version
	    * @param none
	    * Review if all the expected events have ocurred once the time is over
	    */
	    void HandleTimeout(TInt error);


	    // Audio Input Stream
	    virtual void MaiscOpenComplete(TInt aError);
	    virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	    virtual void MaiscRecordComplete(TInt aError);


    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1
        * @return
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
        CWmaDecoderIntfcTestClass( CTestModuleIf& aTestModuleIf );

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
        void Delete();

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
	    void AddExpectedEvent(TWmaExpectedEvent event, TInt ms);


	    /**
	    * Unset an event as expected
	    * @since ?Series60_version
	    */
	    TBool RemoveExpectedEvent(TWmaExpectedEvent event);


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
	    void ProcessEvent(TWmaExpectedEvent aEvent, TInt aError);

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
		TInt CreateDevSoundInstance( CStifItemParser& aItem );
		TInt CreateInputStreamInstance( CStifItemParser& aItem );
		TInt CreateWmaDecoderIntfc( CStifItemParser& aItem );
		TInt GetFormat( CStifItemParser& aItem );
		TInt GetBitsPerSampleIn( CStifItemParser& aItem );
		TInt GetNumOfChannelsIn( CStifItemParser& aItem );
		TInt GetSamplesPerSec( CStifItemParser& aItem );
		TInt GetAvgBytesPerSec( CStifItemParser& aItem );
		TInt GetBlockAlign( CStifItemParser& aItem );
		TInt GetEncodeOptions( CStifItemParser& aItem );
		TInt GetEncodeOptions1( CStifItemParser& aItem );
		TInt GetEncodeOptions2( CStifItemParser& aItem );
		TInt GetChannelMaskIn( CStifItemParser& aItem );
		TInt GetSupportedFormats( CStifItemParser& aItem );
		TInt GetSupportedTools( CStifItemParser& aItem );
		TInt GetSupportedMaxChannelsIn( CStifItemParser& aItem );
		TInt GetSupportedMaxSampleRate( CStifItemParser& aItem );
		TInt GetSupportedMaxBitrate( CStifItemParser& aItem );
		TInt GetControllableTools( CStifItemParser& aItem );
		TInt GetTool( CStifItemParser& aItem );


		TInt SetFormat( CStifItemParser& aItem );
		TInt SetBitsPerSampleIn( CStifItemParser& aItem );
		TInt SetNumChannelsIn ( CStifItemParser& aItem );
		TInt SetSamplesPerSec ( CStifItemParser& aItem );
		TInt SetAvgBytesPerSec ( CStifItemParser& aItem );
		TInt SetBlockAlign ( CStifItemParser& aItem );
		TInt SetEncodeOptions ( CStifItemParser& aItem );
		TInt SetEncodeOptions1 ( CStifItemParser& aItem );
		TInt SetEncodeOptions2( CStifItemParser& aItem );
		TInt SetChannelMaskIn ( CStifItemParser& aItem );

		TInt EnableTool ( CStifItemParser& aItem );
		TInt DisableTool ( CStifItemParser& aItem );
		TInt ApplyConfig( CStifItemParser& aItem );



    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

    // reference to TestModuleIf
    CTestModuleIf& iTestModuleIf;

   // 	CConfigurationComponentsFactory* iFactory;
			CMMFDevSound*	iDevSound;
		//	CRestrictedAudioOutput* iRestrictedAudioOutput;

		CWmaDecoderIntfc* iWmaDecoder;
	  RArray<CWmaDecoderIntfc::TFormat> iSupportedFormats;
	  RArray<CWmaDecoderIntfc::TTool> iSupportedTools;
	  RArray<CWmaDecoderIntfc::TTool> iControllableTools;

	  /* By FC
	   * On 06/12/08
	   */
	  CMdaAudioInputStream* 	iAudioInputStream;
	 // CMdaAudioOutputStream*	iAudioOutStream;
	  /* End By FC */

    // Active object with a timer to timeout the test case
    CSimpleTimeout * iTimeoutController;

    // Indicates if the test case use a normal exit reason
    TBool iNormalExitReason;

    // List of expected events
    RArray<TWmaExpectedEvent> iExpectedEvents;

    // List of notification event (by the callback)
    RArray<TWmaExpectedEvent> iOcurredEvents;

	TMdaAudioDataSettings 		iStreamSettings;
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

#endif      // CWmaDecoderIntfcTestClass_H

// End of File
