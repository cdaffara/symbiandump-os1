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
* Description:  PSM plugin loader.
*
*/


#ifndef PSMPLUGINLOADER_H
#define PSMPLUGINLOADER_H

#include <e32base.h>
#include <psmtypes.h>

// FORWARD DECLARATIONS
class CPsmPluginBase;
class CPsmManager;
class CPsmSrvPlugin;

/**
 *  PSM Plugin loader
 *
 *  Responsible for loading PSM ECom plug-ins
 *
 *  @since S60 5.0
 */
class CPsmPluginLoader : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM plug-in loader.
         *
         * @return A pointer to the created object.
         */
        static CPsmPluginLoader* NewL( CPsmManager& aManager );

        /**
         * Creates a new PSM plug-in loader. Newly created instance is left in the cleanup stack.
         *
         * @return A pointer to the created object.
         */
        static CPsmPluginLoader* NewLC( CPsmManager& aManager );

        /**
         * Destructor.
         */
        virtual ~CPsmPluginLoader();

        /**
         * Initializes plugins
         */
        void InitializePluginsL();

        /**
         * Notifies plugins
         */
        void NotifyPlugins();

        /**
         * Rerturns plugin count
         */
        TInt PluginCount() const { return iPluginArray.Count(); };

    private:

        /**
        * C++ constructor.
        */
        CPsmPluginLoader( CPsmManager& aManager );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        /**
         * Plugin array
         */
        RPointerArray<CPsmSrvPlugin> iPluginArray;

        /**
         * Reference to manager
         */
        CPsmManager& iManager;

    };

#endif // PSMPLUGINLOADER_H 
