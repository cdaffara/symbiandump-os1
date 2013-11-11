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
* Description:  Base class implementation for Mediator plugins
*
*/


#ifndef MEDIATORPLUGINBASE_H
#define MEDIATORPLUGINBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>


// CLASS DECLARATION

/**
*  Pure virtual base class for Mediator Plugins
*
*  @lib MediatorPluginBase.lib
*  @since S60 3.1
*/
class CMediatorPluginBase : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param iMediatorImplementationUid     mediator implementation UID
        * @return a new CMediatorPluginBase instance
        */
        IMPORT_C static CMediatorPluginBase* NewL( 
                                TUid iMediatorImplementationUid );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMediatorPluginBase();

    public: // New functions
        
        /**
        * Mediator plugin's StartL is called when plugin is constructed
        * This should start the plugin. 
        * !!Note!! that no Mediator sessions should be created before this
        * method is called.
        * Leave should be used in case of error
        * @since S60 3.1
        * @return None.
        */
        virtual void StartL() = 0;
        
         /**
        * CIdle callback for starting the plugin
        * @since S60 3.1
        * @param aThis pointer to callback instance
        * @return TInt indicating the success of the callback.
        */
        static TInt StartPlugin( TAny* aThis );
        
        /**
        * Starts plugin activities ( calls StartL() )
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void StartPluginL( );
        
    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
    private:    // Data

        /**
        * Identifier of this instance
        */
        TUid iDtor_ID_Key;
        
        /**
        * Idle notifier
        * Own.
        */
        CIdle* iIdle;
        
        /**
        * Reserved pointer for future extension
        */
        TAny* iReserved;
    };

#endif // MEDIATORPLUGINBASE_H
            
// End of File