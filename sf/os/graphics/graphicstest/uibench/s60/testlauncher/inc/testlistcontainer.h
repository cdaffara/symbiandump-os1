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

#ifndef FILELISTCONTAINER_H
#define FILELISTCONTAINER_H


#include "testlistengine.h"

#include <coecntrl.h>
#include <aknlists.h>
#include <eiklbx.h> // For Listbox


/**
*  CFileListContainer  container control class.
*
*/
class CFileListContainer : public CCoeControl, MCoeControlObserver
    {
    public:
        /**
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);
        
        virtual ~CFileListContainer();
        void SetFileListL(TInt aDirectory, TInt aSizeDate);
        void LaunchCurrentL();

        /**
        * Function - OfferKeyEventL
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown.
        * @return Indicates whether or not the key event was used by this control.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private:
        // From CoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void Draw(const TRect& aRect) const;
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    public:
        CFileListEngine* iAppEngine;

    private:
        // Listbox
        CAknDoubleNumberStyleListBox* iListBox;
    };

#endif
