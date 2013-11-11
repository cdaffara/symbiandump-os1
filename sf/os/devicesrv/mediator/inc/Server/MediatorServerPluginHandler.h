/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resolves and loads ECom plugins into new threads.
*
*/

#ifndef MEDIATORSERVERPLUGINHANDLER_H
#define MEDIATORSERVERPLUGINHANDLER_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// CLASS DECLARATION

/**
*  Plugin handler class for Mediator Server.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServerPluginHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMediatorServerPluginHandler* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorServerPluginHandler();

    public: // New functions
        
        /**
        * Thread's entry point
        * @since S60 3.1
        * @param aParameter Pointer of parameter passed to thread
        * @return Error code of thread creating
        *         KErrNone if succesfully
        */
        static TInt ThreadEntryPoint( TAny* aParameter );

        /**
        * Starts the thread creating
        * @since S60 3.1
        * @param aMediatorPluginUid Implementation uid for Mediator plugin
        * @param aThreadName Thread name
        * @return None.
        */
        void StartThreadL( TUid aMediatorPluginUid );
        
        /**
        * Loads plugins
        * @since S60 3.1
        * @param aPtr pointer to CIdle callback instance
        * @return TInt indicating the status of the callback.
        */
        static TInt LoadPlugins( TAny* aPtr );
        
        /**
        * Loads ECom plugins
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void LoadPluginsL( );
        
        /**
        * Returns count of plugins in this handler
        * @since S60 3.1
        * @param None.
        * @return TInt plugin count.
        */
        TInt PluginCount() { return iPluginImplInfoArray.Count(); }

    private:  // New functions
        
        /**
        * Finds ECom interface implementations
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void FindImplementationsL();
        
        /**
        * Starts a new thread for given plugin
        * @since S60 3.1
        * @param aMediatorPluginUid UID for the plugin to be loaded
        * @return None.
        */
        static void DoStartThreadL( TUid aMediatorPluginUid );
        
    private:

        /**
        * C++ default constructor.
        */
        CMediatorServerPluginHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        
        /**
        *  A class for passing parameter data for new plugin thread
        *
        *  @lib MediatorServer
        *  @since S60 3.1
        */
        class TMediatorThreadParam
            {
            public:
                // plugin uid
                TUid iMediatorPluginUid;
            };
        
        /**
        * Implementation info of Mediator plugin instances.
        */
        RImplInfoPtrArray iPluginImplInfoArray;
         
        /**
        * To detect idle time 
        * Own.
        */
        CIdle* iIdle;
        
    };

#endif      // MEDIATORSERVERPLUGINHANDLER_H   
            
// End of File
