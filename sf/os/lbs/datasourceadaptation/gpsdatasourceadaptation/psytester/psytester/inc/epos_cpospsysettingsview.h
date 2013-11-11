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



#ifndef CPOSPSYSETTINGSVIEW_H
#define CPOSPSYSETTINGSVIEW_H

#include <e32base.h>
#include "epos_cpospsyconsoleview.h"
#include "epos_cpospsytesthandler.h"

// CONSTANTS
const TInt KMemoryMaxBufSize = 6;

// CLASS DECLARATION

/**
*  Class that implements the MUtfwProgress interface.
*  
*/
class CPosPSYSettingsView : public CPosPSYConsoleView
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor. The ownership of both the parameters belongs
        * to the caller.
        * @param aConsole a console used for writing characters
        * @param aPSYTestHandler handler of test information
        */
        static CPosPSYSettingsView* NewL(
        /* IN */ CConsoleBase* aConsole,
		/* IN */ CPosPSYTestHandler* aPSYTestHandler
		);
        
        /**
        * Destructor.
        */
        virtual ~CPosPSYSettingsView();

    public: // Functions from base classes

        /**
        * From CPosConsoleView
		* Activates the view
        */
        void ActivateL();

        /**
        * From CPosConsoleView
		* Handles a key event
        * @param TKeyCode aChar, the key code for the key pressed by the user
        */
        void HandleKeyEventL(
        /* IN */ TKeyCode aChar
        );
	
		/**
		* From CPosConsoleView
		* Function that handles a change of state in the tests
		* @param aTestEvent the event that has occured
		*/
		void HandleTestEvent(
		/* IN */ TInt aTestEvent
		);

    protected: // Functions from base classes

        /**
        * From CPosConsoleView
		* Displays a help message in the view
        */
        void PrintHelp();
        
        /**
        * From CPosConsoleView
		* Re-Draws the screen
        */
        void ReDraw();

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYSettingsView(
		/* IN */ CConsoleBase* aConsole
		);

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
		/* IN */ CPosPSYTestHandler* aPSYTestHandler
		);

		TInt ParseBufferL();

        // By default, prohibit copy constructor
        CPosPSYSettingsView( const CPosPSYSettingsView& );
        // Prohibit assigment operator
        CPosPSYSettingsView& operator= ( const CPosPSYSettingsView& );

    private:    // Data

       TInt iMaxWidth;
	   TInt iXPos;
	   TInt iYPos;
	   RArray<TInt> iReadBuf;
	   CPosPSYTestHandler* iTestHandler;
      
    };

#endif      // CPOSPSYSETTINGSVIEW_H
            
// End of File
