/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM server message.
*
*/


#ifndef PSMSRVMESSAGE_H
#define PSMSRVMESSAGE_H

#include <e32base.h>
#include "psmmodechangeobserver.h"

class CPsmManager;

/**
 *  PSM Message
 *
 *  Wraps the message for PSM servers internal message handling
 *
 *  @since S60 5.0
 */
class CPsmMessage : public CBase,
                    public MPsmModeChangeObserver
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor for external message
         *
         * @return New CSensrvMessage instance
         */
        static CPsmMessage* NewL( CPsmManager& aPsmManager );

        /**
         * Two-phased constructor for external message
         *
         * @return New CSensrvMessage instance
         */
        static CPsmMessage* NewLC( CPsmManager& aPsmManager );

        /**
         * Destructor.
         */
        virtual ~CPsmMessage();

        /**
         * Initializes the message. Must be done before message is
         * used first time and before use after each complete.
         *
         * @param aMessage Client message to wrap
         */
        void Initialize( const RMessage2& aMessage );

        /**
         * Completes the message
         *
         * @param aReason Completion reason identifier. Usually error code.
         */
        void Complete( TInt aReason ) const;

        /**
         * Observes the mode change completion
         */
        void NotifyPsmModeChangeComplete( const TInt aError );
		
		/**
         * Returns the message handle
         */
		TInt MessageHandle( ) const;

    private:

        /**
         * C++ constructor.
         */
        CPsmMessage( CPsmManager& aPsmManager );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:    // Data

        /**
         * Wrapped message
         */
        RMessage2 iMessage;

        /**
         * Reference to PSM manager
         */
        CPsmManager& iPsmManager;

    };

#endif // PSMSRVMESSAGE_H
