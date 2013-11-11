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
* Description:  Header file for the UPnP AV Control implementation
*
*/


#ifndef MEDIATORTESTPLUGIN_H
#define MEDIATORTESTPLUGIN_H

// INCLUDES
#include "MediatorPluginBase.h"

#include "MediatorNotifications.h"
#include "MediatorEventConsumer.h"
#include "MediatorCommandResponder.h"

// FORWARD DECLARATIONS

// TYPEDEFS

// NAMESPACE
using namespace MediatorService;

/**
* 
*
* @since Series 60 3.0
*/
class CMediatorTestPlugin : public CMediatorPluginBase, 
                            public MMediatorNotifications,
                            public MMediatorEventObserver,
                            public MMediatorCommandObserver

	{
    public:

        /**
         * Creates an instance of the implementation.
         *
         * @since Series 60 3.0
         * @return instance of this class.
         */
	    static CMediatorTestPlugin* NewL();

        /**
         * Destructor.
         */
        ~CMediatorTestPlugin();
        
    public: // From Base class
        
        void StartL();
        
    public: // From 
    
        /**
        * Events have been registered. The events can now be subscribed.
        * The list can contain 1-n new events into the category.
        *
        * @since Series 60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events that have been registered.
        * @return None.
        */
        void MediatorEventsAddedL( TUid aDomain, 
                                           TUid aCategory, 
                                           const REventList& aEvents );

        /**
        * New commands have been registered. The commands can now be subscribed.
        * The list can contain 1-n new commands into the category.
        *
        * @since Series 60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  List of commands that have been registered.
        * @return None.
        */
        void MediatorCommandsAddedL( TUid aDomain, 
                                             TUid aCategory,
                                             const RCommandList& aCommands );
         
        /**
        * A category has been removed. All subscriptions are cancelled.
        *
        * @since Series 60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.        
        * @return None.
        */
        void MediatorCategoryRemovedL( TUid aDomain, TUid aCategory );

        /**
        * List of events has been removed. All subscriptions are cancelled.
        * List can contain 1-n events from the specified category.
        *
        * @since Series 60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events removed.
        * @return None.
        */
        void MediatorEventsRemovedL( TUid aDomain, 
                                             TUid aCategory,
                                             const REventList& aEvents );
                                             
        /**
        * List of commands have been removed. All subscriptions are cancelled.
        * List can contain 1-n commands from the specified category.
        *
        * @since Series 60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  The list of commands removed
        * @return None.
        */
        void MediatorCommandsRemovedL( TUid aDomain, 
                                               TUid aCategory,
                                               const RCommandList& aCommands ); 
                                               
    	void CancelMediatorCommand( TUid aDomain,
        	                        TUid aCategory, 
            	                    TInt aCommandId );


    	void MediatorCommandL( TUid aDomain,
        	                   TUid aCategory, 
            	               TInt aCommandId,
                	           TVersion aVersion, 
                    	       const TDesC8& aData );
                                               
    public: 
                                                   
        /**
        * A Mediator Service event.
        *
        * @since Series 60 3.1        
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aData      The parameters of the event.
        * @return None.
        */
        void MediatorEventL( TUid aDomain,
                                     TUid aCategory, 
                                     TInt aEventId, 
                                     const TDesC8& aData );                                               
        
    private:
        HBufC16* ToUnicodeL(const TDesC8& aBuffer);
        
    private:
        
        
        CMediatorTestPlugin();
        
        void ConstructL(); 
          
        
    private: 
        CMediatorNotifications* iMediatorNotifications;
        
        CMediatorEventConsumer* iEventConsumer;
        CMediatorCommandResponder* iCommandResponder;
    };

#endif

