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
* Description:  Control class of Orientation SSY
*
*/



#ifndef __SSYCONTROLORIENTATION_H__
#define __SSYCONTROLORIENTATION_H__

// INCLUDES
#include <e32base.h>
#include <ssycontrol.h>
#include "SsyChannel.h"

// FORWARD DECLARATIONS
class TSSsyChannelInfo;
class MSsyCallback;


// CLASS DECLARATION

/**
 *  CSSYControlOrientation control class
 *
 *  Controller class for orientation ssy.
 *
 *  @lib orientationssy.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CSSYControlOrientation ) : public CSsyControl
    {
    public:

        /**
         * Two phase constructor.
         *
         * @param aSsyCallback Callback class from sensor server
         */
        static CSSYControlOrientation* NewL( MSsyCallback* aSsyCallback );

        /**
         * Destructor
         */
        virtual ~CSSYControlOrientation();

        /**
         * From CSsyControl. Request for SSY to open a channel asynchronously.
         *
         * @param aChannelID Identifies channel to open
         */
        void OpenChannelL( TSensrvChannelId aChannelID );

        /**
         * From CSsyControl. Request for SSY to close a channel asynchronously.
         *
         * @param aChannelID Identifies channel to close
         */
        void CloseChannelL( TSensrvChannelId aChannelID );

    protected:

        /**
         * C++ constructor.
         *
         * @param aSsyCallback Callback class from sensor server
         */
        CSSYControlOrientation( MSsyCallback* aSsyCallback );

        /**
         * Symbian 2nd phase constructor.
         */            
        void ConstructL();

    private:

        /**
         * Checks whether a specific channel is supported or not. Used internally.
         * 
         * @param aChannelID Identifies channel to check is supported
         * @param aIndex Contains the index of the channel if supported
         * @return TBool True if channel is supported
         */
        TBool IsChannelSupported( const TSensrvChannelId aChannelID, TInt& aIndex );    
        
        /**
         * Creates and registers channels from configuration file. Used internally.
         *
         * @param aChannelConfigurations Channel definitions this SSY provides
         * @param aNumberofConfigurations Number of channels this SSY provides
         */
        void CreateAndRegisterChannelsL( const TSSsyChannelInfo* aChannelConfigurations,
                                         const TUint aNumberofConfigurations );
    private:

        /** 
         * Sensor specific properties common for all the channels. 
         */
        CSSYProperty*      iSensorProperties;

        /** 
         * Ssy callback class. 
         */
        MSsyCallback*              iCallback;

        /** 
         * Array of pointers containing the channels for orientation ssy. 
         */
        RPointerArray<CSSYChannel> iChannelArray;
};


#endif // __SSYCONTROLORIENTATION_H__

// End of File
