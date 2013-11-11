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



#ifndef CPOSPSYCONSOLEVIEW_H
#define CPOSPSYCONSOLEVIEW_H

// INCLUDES
#include <e32base.h>
#include <e32keys.h>

// CONSTANTS
_LIT(KNewUILine, "\n");
const TUid KStartView = {0x0};
const TUid KPSYListView = {0x1};
const TUid KPSYSettingsView = {0x2};
const TUid KCmdLineView = {0x3};
const TUid KPreviousView = {0x10};
const TInt KMaxLineWidth = 80;

const TInt KDefaultYPos = 2;

// FORWARD DECLARATIONS
class MPosPSYViewManager;

// CLASS DECLARATION

/**
* Class that handles user interaction 
* through a console base UI.
*/
class CPosPSYConsoleView : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CPosPSYConsoleView();

    public: // New functions

        /**
        * Activates the view
        */
        virtual void ActivateL();

        /**
        * DeActivates the view
        */
        virtual void DeActivate();

        /**
        * Sets the viewmanger
        * @param aViewManager the viewManager
        */
        void SetViewManager(
		/* IN */    MPosPSYViewManager* aViewManager
        );

        /**
        * Handles a key event
        * @param TKeyCode aChar, the key code for the key pressed by the user
        */
        virtual void HandleKeyEventL(
        /* IN */    TKeyCode aChar
        ) = 0;

		/**
		* Handles a change of state in the tests
		* @param aTestEvent the event that has occured
		*/
		virtual void HandleTestEvent(
		/* IN */ TInt aTestEvent
		) = 0;

		/**
		* Tells if the key the user pressed is a request help key
		* @param aKey the key pressed by the user
		* @return valude telling if help was requested or not
		*/
		TBool IsHelpRequested(
		/* IN */ TKeyCode aKey
		);
		
		/**
		* Tells if the key the user pressed a numeric key
		* @param aKey the key pressed by the user
		* @return valude telling if a numeric key was pressed or not
		*/
		TBool IsNumeric(
		/* IN */ TKeyCode aKey
		);

    protected: // New functions
    
        /**
        * C++ default constructor. The ownership of both the parameters belongs
        * to the caller.
        * @param aConsole a console used for writing characters
        */
        CPosPSYConsoleView(    
        /* IN */ CConsoleBase* aConsole
		);            

        /**
        * Function for printing help to the user.
        */
        virtual void PrintHelp() = 0;
        
        /**
        * Re-Draws the screen
        */
        virtual void ReDraw() = 0;

        // By default, prohibit copy constructor
        CPosPSYConsoleView( const CPosPSYConsoleView& );
        
        // Prohibit assigment operator
        CPosPSYConsoleView& operator= ( const CPosPSYConsoleView& );
        
    protected:    // Data

        CConsoleBase*       iConsole;
        MPosPSYViewManager*   iViewManager;

    };

#endif      // CPOSPSYCONSOLEVIEW_H

// End of File
