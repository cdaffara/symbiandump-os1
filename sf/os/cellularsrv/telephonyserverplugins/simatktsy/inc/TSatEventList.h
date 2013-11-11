/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TSatEventList.h
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Event list - related functionality of Sat Tsy
* Version     : 1.0
*
*/



#ifndef TSATEVENTLIST_H
#define TSATEVENTLIST_H

//  INCLUDES
#include <etelsat.h>    // Symbian Etel Sat related types

// DESCRIPTION
/**
* Handles event list management for mmsat, used to keep track of events
* that need to be send to NAA.
* @lib Commonsimatktsy
* @since 3.1
*/
NONSHARABLE_CLASS ( TSatEventList )
    {
    public:

        /**
        * C++ constructor
        */
        TSatEventList();

        /**
        * Check if event is ín the set of enabled events
        * @param aEvent event to check
        * @return true if the event is enabled
        */
        TBool IsEnabled( RSat::TEventList aEvent ) const;

        /**
        * Remove event from set of enabled events
        * @param aEvent event to be removed
        * @return none
        */
        void RemoveFromEventList( RSat::TEventList aEvent );

        /**
        * Add event to set of enabled events
        * @param aEvent event to be added
        * @return none
        */
        void AddToEventList( RSat::TEventList aEvent );

        /**
        * Set the list of enabled events
        * @param aEvent bit mask containing events
        * @return none
        */        
        void SetEventList( TUint32 aEvent );
            
    private:

        // Set of events
        TUint32 iEvents; 
    };

#endif // TSATEVENTLIST_H

// End of File