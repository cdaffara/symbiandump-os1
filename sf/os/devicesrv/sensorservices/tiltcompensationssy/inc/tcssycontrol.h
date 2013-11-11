/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control class of Tilt Compensation SSY.
*
*/



#ifndef TCSSYCONTROL_H
#define TCSSYCONTROL_H

// INCLUDES
#include <ssycontrol.h>

#include "tcchannel.h"
#include "common.h"

// FORWARD DECLARATIONS
class MSsyCallback;

// CLASS DECLARATION

/**
*  SSY plug-in implementation for Tilt Compensation plug-in.
*
*  @lib None
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CTCSsyControl ): public CSsyControl
    {
    public:

        /**
        * Two phased constructor.
        *
        * @param aSsyCallback Callback class from sensor server
        * @return CTCSsyControl
        */
        static CTCSsyControl* NewL( MSsyCallback* aSsyCallback );

        /**
        * Destructor
        */
        ~CTCSsyControl();
        
    private: // From CSsyControl

        // @see CSsyControl
        void OpenChannelL( TSensrvChannelId aChannelId );

        // @see CSsyControl
        void CloseChannelL( TSensrvChannelId aChannelId );
        
    private:
    
        // Is channel supported
        CTCChannel* ChannelSupported( TSensrvChannelId aChannelId ) const;

    private:
        
        // Default C++ constructor
        CTCSsyControl();
        
        // Constructor
        void ConstructL( MSsyCallback* aSsyCallback );

    private: // Data
    
        /** Channels supported by Tilt SSY */
        RChannelArray iChannels;
};

#endif // TCSSYCONTROL_H

// End of File
