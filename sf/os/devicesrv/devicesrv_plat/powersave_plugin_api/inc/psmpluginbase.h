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
* Description:  Base class for PSM plug-ins.
*
*/


#ifndef PSMPLUGINBASE_H
#define PSMPLUGINBASE_H

#include <e32base.h>
#include <psmtypes.h>

// FORWARD DECLARATIONS
class MPsmSettingsProvider;
class MPsmPluginCallback;

// Interface UID for power save mode server
const TUid KPsmPluginControlInterfaceUid = { 0x2000B189 };

/**
 *  PSM Plugin base class
 *
 *  Base class for PSM ECom plug-ins
 *
 */
class CPsmPluginBase : public CBase
    {

    public:

    /**
     * PSM ECom plug-in initialization parameters. Give this as a parameter 
     * to CPsmPluginBase() constructor. 
     */
    class TPsmPluginCTorParams
        {
        public:
        /**
         * C++ constructor.
         */
        inline TPsmPluginCTorParams( MPsmSettingsProvider& aSettingsProvider );

        /**
         * Reference to settings provider API
         */
        MPsmSettingsProvider& iSettingsProvider;
        };


    public:  // Constructors and destructor

        /**
        * Creates a new PSM plug-in base class.
        *
        * @param aImplementationUid UID of the implementation to create.
        * @return A pointer to the created object.
        */
        inline static CPsmPluginBase* NewL( const TUid& aImplementationUid,
                                            MPsmSettingsProvider& aSettingsProvider );

        /**
        * Creates a new PSM plug-in base class.
        *
        * @param aImplementationUid UID of the implementation to create.
        * @return A pointer to the created object.
        */
        inline static CPsmPluginBase* NewLC( const TUid& aImplementationUid,
                                             MPsmSettingsProvider& aSettingsProvider );

        /**
        * Destructor.
        */
        inline virtual ~CPsmPluginBase();

    public:

        /**
         * Notifies plugin about the power save mode change. There is only one 
         * active plugin at a time, meaning that when this plugin has done its mode change,
         * next plugin has process time to complete its mode change. because of this it 
         * is good not to make any heavy processes during this mode change.
         *
         * @param aMode Mode to change to.
         */
        virtual void NotifyModeChange( const TInt aMode ) = 0;

    protected:

        /**
         * C++ constructor.
         *
         * @param aInitParams. Contains initialization parameters for this base class.
         */ 
        inline CPsmPluginBase( TPsmPluginCTorParams& aInitParams );

    protected: // data

        /**
         * Reference to settings provider API
         */
        MPsmSettingsProvider& iSettingsProvider;

    private:    // Data

        /**
         * destructor key
         */
        TUid iDestructorIdKey;

    };

#include <psmpluginbase.inl>

#endif // PSMPLUGINBASE_H 
