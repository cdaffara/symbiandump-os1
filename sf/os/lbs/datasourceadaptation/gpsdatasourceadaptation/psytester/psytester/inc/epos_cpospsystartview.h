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



#ifndef CPOSPSYSTARTVIEW_H
#define CPOSPSYSTARTVIEW_H

// INCLUDES
#include "epos_cpospsyconsoleview.h"
#include "epos_cpospsystartview.h"

// CLASS DECLARATION

/**
* Class that handles user interaction 
* through a console base UI.
*/
class CPosPSYStartView : public CPosPSYConsoleView
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor. 
		* The ownership of both the parameters belongs to the caller.
        * @param aConsole a console used for writing characters
        * @param aModules test modules to execute
        */
        static CPosPSYStartView* NewL(
		/* IN */ CConsoleBase* aConsole
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYStartView();

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
        CPosPSYStartView(    
        /* IN */ CConsoleBase* aConsole
		);

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();   

        // By default, prohibit copy constructor
        CPosPSYStartView( const CPosPSYStartView& );
        
        // Prohibit assigment operator
        CPosPSYStartView& operator= ( const CPosPSYStartView& );

    };

#endif // CPOSPSYSTARTVIEW_H

// End of File
