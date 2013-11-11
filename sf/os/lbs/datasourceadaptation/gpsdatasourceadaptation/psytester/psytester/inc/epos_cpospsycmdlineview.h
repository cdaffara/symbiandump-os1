// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Class that handles command line for psy tester
// 
//

#ifndef CPOSPSYCMDLINEVIEW_H
#define CPOSPSYCMDLINEVIEW_H

// INCLUDES
#include "epos_cpospsyconsoleview.h"

// CLASS DECLARATION

/**
* Class that handles user interaction 
* through a console base UI.
*/
class CPosPSYCmdlineView : public CPosPSYConsoleView
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor. 
		* The ownership of both the parameters belongs to the caller.
        * @param aConsole a console used for writing characters
        * @param aModules test modules to execute
        */
        static CPosPSYCmdlineView* NewL(
		/* IN */ CConsoleBase* aConsole
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYCmdlineView();

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
        * From CPosPSYConsoleView
		* Displays a help message in the view
        */
        void PrintHelp();
        
        /**
        * From CPosPSYConsoleView
		* Re-Draws the screen
        */
        void ReDraw();

    private:
    
        /**
        * C++ default constructor.
        */
        CPosPSYCmdlineView(    
        /* IN */ CConsoleBase* aConsole
		);

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();   

        // By default, prohibit copy constructor
        CPosPSYCmdlineView( const CPosPSYCmdlineView& );
        
        // Prohibit assigment operator
        CPosPSYCmdlineView& operator= ( const CPosPSYCmdlineView& );

    };

#endif // CPosPSYCmdlineView_H

// End of File
