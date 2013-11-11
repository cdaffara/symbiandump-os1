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
* Description:  PSM Srv plugin.
*
*/


#ifndef PSMSRVPLUGIN_H
#define PSMSRVPLUGIN_H

#include <e32base.h>
#include <psmtypes.h>

// FORWARD DECLARATIONS
class CPsmPluginBase;
class CPsmManager;
class CPsmSettingsHandler;

/**
 *  PSM Srv Plugin
 *
 *  Responsible for loading PSM ECom plug-in and maintaining it
 *
 *  @since S60 5.0
 */
class CPsmSrvPlugin : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM plug-in loader.
         *
         * @return A pointer to the created object.
         */
        static CPsmSrvPlugin* NewL( CPsmManager& aManager, TUid aImplUid );
        
        /**
         * Creates a new PSM plug-in loader. Newly created instance is left in the cleanup stack.
         *
         * @return A pointer to the created object.
         */
        static CPsmSrvPlugin* NewLC( CPsmManager& aManager, TUid aImplUid );

        /**
         * Destructor.
         */
        virtual ~CPsmSrvPlugin();

    public: // Functions

        /**
         * Notifies plugin about mode change
         */
        void NotifyModeChange( const TPsmsrvMode aMode );

    private:

        /**
        * C++ constructor.
        */
        CPsmSrvPlugin( CPsmManager& aManager, TUid aImplUid );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        /**
         * Reference to manager
         */
        CPsmManager& iManager;

        /**
         * Plugins implementation UID
         */
        TUid iImplUid;
        
        /**
         * Currently active plugin
         */
        CPsmPluginBase* iActivePlugin;

        /**
         * Own settings handler for each plugin
         */
        CPsmSettingsHandler* iSettingsHandler;

    };

#endif // PSMSRVPLUGIN_H 
