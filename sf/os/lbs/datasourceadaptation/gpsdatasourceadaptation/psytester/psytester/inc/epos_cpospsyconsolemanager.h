/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CPOSPSYCONSOLEMANAGER_H
#define CPOSPSYCONSOLEMANAGER_H

// INCLUDES
#include "epos_mpospsyviewmanager.h"

// FORWARD DECLARATIONS
class CConsoleBase;
class CPosPSYConsoleView;
class CPosPSYTestHandler;

// DATA TYPES
enum _TestState
	{
	EDefault = 0,
	ERunAll,
	ETestsRunning,
	EIndexError,
	ETestsCompletedCorrect,
	ETestsCompletedWithError
	};

// CLASS DECLARATION

/**
* Main class for the Console application. Creates all the views and switches 
* views. Handles which mode the console is executed in.
*/
class CPosPSYConsoleManager : public CActive, public MPosPSYViewManager
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYConsoleManager* NewL(
			CPosPSYTestHandler* aPSYTestHandler
			);

        /**
        * Destructor.
        */
        virtual ~CPosPSYConsoleManager();

    public: // New functions
        
        /**
        * Function activating console user interface
        */
        void StartL();

		/**
        * Function for reporting the state of the test
        * @param aTestState the state that is reported
		*/
		void SetTestState(
		/* IN */ TInt aTestState
		);
        
    public: // Functions from base classes

        /**
        * From CActive RunL()
        */
        void RunL(); 
        
        /**
        * From CActive DoCancel
        */
        void DoCancel();

        /**
        * From CActive RunError
        */
        TInt RunError(
		/* IN */ TInt aError
		);

        /**
        * From MPosPSYViewManager
		* Switches to a new view
        * @param aViewId The view to switch to.
        */
        void SwitchViewL(
		/* IN */ const TUid& aViewId
		);

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYConsoleManager();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
		/* IN */	CPosPSYTestHandler* aPSYTestHandler
		);
    
        /**
        * Processes the ket event originating from the user
        * @param TKeyCode aChar, the key code for the key pressed by the user
        */
        void ProcessKeyPressL(
        /* IN */ TKeyCode aChar
        );

        /**
        * Function requesting a character from console uiser interface
        */
        void RequestCharacter();

        // By default, prohibit copy constructor
        CPosPSYConsoleManager( const CPosPSYConsoleManager& );
        
        // Prohibit assigment operator
        CPosPSYConsoleManager& operator= ( const CPosPSYConsoleManager& );
        
    private:    // Data

        CConsoleBase*						iConsole;
        CPosPSYConsoleView*					iOldView;
        CPosPSYConsoleView*					iActiveView;
        RPointerArray<CPosPSYConsoleView>	iViewList;
		CPosPSYTestHandler*					iTestHandler;
		TInt								iTestState;
		TInt								iTestScope;

    };

#endif      // CPOSPSYCONSOLEMANAGER_H

// End of File
