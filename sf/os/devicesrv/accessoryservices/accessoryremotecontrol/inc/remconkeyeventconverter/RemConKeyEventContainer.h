/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container/accessor class for defined key events
*
*/



#ifndef CREMCONKEYEVENTCONTAINER_H
#define CREMCONKEYEVENTCONTAINER_H

//  INCLUDES
#include "RemConKeyEvent.h"

// CLASS DECLARATION

/**
*  Container/accessor class for defined key events.
*
*  @lib RemConKeyEventConverter.dll (ECom plugin)
*  @since S60 3.0
*/
class CRemConKeyEventContainer : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CRemConKeyEventContainer* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CRemConKeyEventContainer();

    public: // New functions
        
        /**
        * Add a key event instance to container.
        * @since S60 3.0
        * @param CRemConKeyEvent Key event to be added.
        * @return void
        */
        void AddEventL( const CRemConKeyEvent* aKeyEvent );

        /**
        * Return matching key event instances.
        * @since S60 3.0
        * @return Array of key event instances.
        */
        const RPointerArray<CRemConKeyEvent>& GetEvents() const;

    private:

        /**
        * C++ default constructor.
        */
        CRemConKeyEventContainer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // Array of key event instances
        RPointerArray<CRemConKeyEvent> iKeyEventArray;

    };

#endif      // CREMCONKEYEVENTCONTAINER_H   
            
// End of File
