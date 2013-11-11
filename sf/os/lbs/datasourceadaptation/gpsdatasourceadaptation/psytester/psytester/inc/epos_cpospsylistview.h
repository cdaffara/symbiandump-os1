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



#ifndef CPOSPSYLISTVIEW_H
#define CPOSPSYLISTVIEW_H

// INCLUDES
#include "epos_cpospsyconsoleview.h"
#include "epos_cpospsytesthandler.h"

// CONSTANTS
const TInt KMaxBufSize = 5;
const TInt KTestAllPSYs = 0;

const TInt KListPrevious = -1;
const TInt KListCurrent = 0;
const TInt KListNext = 1;
const TInt KListStart = 100;

// CLASS DECLARATION

/**
* Class that handles user interaction 
* through a console based UI.
*/
class CPosPSYListView : public CPosPSYConsoleView
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor. 
		* The ownership of both the parameters belongs to the caller.
        * @param aConsole a console used for writing characters
        * @param aPSYTestHandler handler of test information
        */
        static CPosPSYListView* NewL(
		/* IN */ CConsoleBase* aConsole,
		/* IN */ CPosPSYTestHandler* aPSYTestHandler
        );

        /**
        * Destructor.
        */
        virtual ~CPosPSYListView();

    public: // Functions from base classes

        /**
        * From CPosPSYConsoleView
		* Activates the view
        */
        void ActivateL();
                
        /**
        * From CPosPSYConsoleView
		* Handles a key event
        * @param TKeyCode aChar, the key code for the key pressed by the user
        */
        void HandleKeyEventL(
        /* IN */ TKeyCode aChar
        );
	
		/**
		* From CPosPSYConsoleView
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
        CPosPSYListView(    
        /* IN */ CConsoleBase* aConsole
        );

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(
		/* IN */ CPosPSYTestHandler* aPSYTestHandler);   

		void ListPSYs(
		/* IN */ TInt aListDir
		);

		void PrintNoFound();

		TInt ParseBufferL();

        // By default, prohibit copy constructor
        CPosPSYListView( const CPosPSYListView& );
        
        // Prohibit assigment operator
        CPosPSYListView& operator= ( const CPosPSYListView& );

	private : //Data
		
		TInt iCurrentIndex;
		RArray<TInt> iReadBuf;
		CPosPSYTestHandler* iTestHandler;

    };

#endif      // CPOSPSYLISTVIEW_H

// End of File
