 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 //

#ifndef __HELLOWORLDBASICAPPUI_H__
#define __HELLOWORLDBASICAPPUI_H__


#include <aknappui.h>


class CHelloWorldBasicAppView;
class CFileListContainer;


/**
* CHelloWorldBasicAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CHelloWorldBasicAppUi : public CAknAppUi
    {
    public:
        void ConstructL();
        CHelloWorldBasicAppUi();
        virtual ~CHelloWorldBasicAppUi();

    private:
        /**
        * From CEikAppUi
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL(TInt aCommand);

        /**
        *  Called by the framework when the application status pane
 		*  size is changed.
        */
		void HandleStatusPaneSizeChange();
		
        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event.
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private:
        CHelloWorldBasicAppView* iAppView;
        CFileListContainer* iAppContainer;
    };

#endif // __HELLOWORLDBASICAPPUI_H__
