/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Tiger app UI clas declaration
*/

#ifndef TIGERAPPUI_H
#define TIGERAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CTigerContainer;

// CLASS DECLARATION

/**
 * Application UI class that contains the CTigerContainer
 * (as required by the Symbian UI application architecture).
 */
class CTigerAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
         * Second phase constructor. Creates the CTigerContainer
         * and adds it to the control stack.
         */
        void ConstructL();

        /**
         * Destructor. Removes CTigerContainer from the control
         * stack and destroys it.
         */
        virtual ~CTigerAppUi();

    private: // Functions from base classes

        /**
         * This method is called by the EIKON framework just before it displays
         * a menu pane. Does nothing in this implemenation.
         */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

        /**
         * CEikAppUi method that is used to handle user commands.
         * Handles menu commands and application exit request.
         * @param aCommand Command to be handled.
         */
        void HandleCommandL(TInt aCommand);

        /**
         * CEikAppUi method that is used to handle key events.
         * Does nothing in this implemenation.
         * @param aKeyEvent Event to handled (ignored by this implementation).
         * @param aType Type of the key event (ignored by this implementation).
         * @return Response code. Always EKeyWasNotConsumed in this implementation.
         */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data

        /** GUI container that resides in this application UI. */
        CTigerContainer* iAppContainer;
    };

#endif

// End of File
