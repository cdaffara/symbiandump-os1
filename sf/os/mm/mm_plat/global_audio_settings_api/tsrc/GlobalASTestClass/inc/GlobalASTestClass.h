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
* Description: Global Audio Settings Test
*
*/



#ifndef CGLOBALASTESTCLASS_H
#define CGLOBALASTESTCLASS_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <GlobalAudioSettings.h>
#include <MAudioClientsListObserver.h>
#include <MGlobalAudioSettingsObserver.h>

// MACROS
//#define ?macro ?macro_def
// Logging path
#ifdef __WINSCW__
_LIT( KGlobalASTestClassLogPath, "\\logs\\testframework\\" );
#else
_LIT( KGlobalASTestClassLogPath, "e:\\testing\\Log\\" );
#endif

// Log file
_LIT( KGlobalASTestClassLogFile, "CGlobalASTestClass.txt" );


/**
*  CRadioUtilityTestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60_3_2
*/
NONSHARABLE_CLASS(CGlobalASTestClass) : public CScriptBase,
											public MAudioSettingsObserver,
                       						public MAudioClientsListObserver

    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CGlobalASTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CGlobalASTestClass();


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );


		// From MAudioSettingsObserver
		void SettingsChanged(CGlobalAudioSettings& aGlobalAudioSettings, MAudioSettingsObserver::TGASEventId aSetting);
		// From MAudioClientsListObserver
		void Event( const CGlobalAudioSettings& aGlobalAudioSettings, TUint aEvent );

    private:

        /**
        * C++ default constructor.
        */
        CGlobalASTestClass( CTestModuleIf& aTestModuleIf );

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

		TInt CreateGlobalAS( CStifItemParser& aItem );
		TInt DeleteGlobalAS( CStifItemParser& aItem );
		TInt IsWarningTonesEnabled( CStifItemParser& aItem );
		TInt IsMessageTonesEnabled( CStifItemParser& aItem );
		TInt IsSilentProfileEnabled( CStifItemParser& aItem );
		TInt IsVibraEnabled( CStifItemParser& aItem );
		TInt IsPublicSilenceEnabled( CStifItemParser& aItem );
		TInt RegisterAudioClientsListObserver( CStifItemParser& aItem );
		TInt UnregisterAudioClientsListObserver( CStifItemParser& aItem );
		TInt GetAudioClientsList( CStifItemParser& aItem );
		TInt GetPausedClientsList( CStifItemParser& aItem );
		TInt GetRingingType( CStifItemParser& aItem );
		TInt MessageAlertTone( CStifItemParser& aItem );
		TInt IsMessageAlertToneEnabled( CStifItemParser& aItem );
		TInt EmailAlertTone( CStifItemParser& aItem );
		TInt IsEmailAlertToneEnabled( CStifItemParser& aItem );
		TInt VideoCallAlertTone( CStifItemParser& aItem );
		TInt IsVideoCallAlertToneEnabled( CStifItemParser& aItem );
		TInt RingingAlertTone1( CStifItemParser& aItem );
		TInt IsRingingAlertTone1Enabled( CStifItemParser& aItem );
		TInt RingingAlertTone2( CStifItemParser& aItem );
		TInt IsRingingAlertTone2Enabled( CStifItemParser& aItem );
		TInt KeypadAlertTone( CStifItemParser& aItem );
        TInt IsSilenceModeActive(CStifItemParser& aItem);
    private:    // Data

		// reference to TestModuleIf
		CTestModuleIf& iTestModuleIf;

		CGlobalAudioSettings *iPhoneProfileSettings;

    };

#endif      // CGLOBALASTESTCLASS_H

// End of File
