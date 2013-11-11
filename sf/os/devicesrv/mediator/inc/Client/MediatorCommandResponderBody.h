/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An implementation class for responding to Mediator Service commands.
*
*/

#ifndef MEDIATORCOMMANDRESPONDERBODY_H
#define MEDIATORCOMMANDRESPONDERBODY_H

// INCLUDES
#include <e32base.h>

#include "MediatorServerClient.h"
#include "MediatorCommandResponder.h"
#include "MediatorServiceDefs.h"

// NAMESPACE
using namespace MediatorService;

// CONSTANTS

// FORWARD DECLARATIONS
class RMediatorServerSession;

// CLASS DECLARATION

/**
*  A class used to respond to Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMediatorCommandResponderBody) : public CActive
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver  mediator command observer
        * @return a new CMediatorCommandResponderBody instance
        */       
        static CMediatorCommandResponderBody* NewL(
            MMediatorCommandObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorCommandResponderBody();

    public: // Command registration

        /**
        * Register a command category. After registering the category 
        * all commands from the category may be initiated.
        * 
        * @since S60 3.1  
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommands  The list of commands to be registered.
        * @return TInt      an error value.
        */
        TInt RegisterCommand( TUid aDomain,
                              TUid aCategory, 
                              const RCommandList& aCommands );

        /**
        * Register a command. After registering the command it may be
        * initiated.
        * 
        * @since S60 3.1   
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the category.
        * @param aVersion   The version of the command
        * @param aCaps      Capabilities required for initiating the command.
        * @param aTimeout   The timeout value for waiting a response for the 
        *                   command. This is given in milliseconds (ms).
        * @return TInt      an error value.
        */
        TInt RegisterCommand( TUid aDomain, 
                              TUid aCategory, 
                              TInt aCommandId, 
                              TVersion aVersion,
                              TCapabilitySet aCaps, 
                              TInt aTimeOut );

        /**
        * Unregister a command category.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aCommands  List of commands to be unregistered.
        * @return TInt      an error value.
        */
        TInt UnregisterCommand( TUid aDomain, 
                                TUid aCategory,
                                const RCommandList& aCommands );

        /**
        * Unregister a command.
        *
        * @since S60 3.1  
        * @param aDomain    The identifier of the domain       
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the category.
        * @return TInt      an error value.
        */
        TInt UnregisterCommand( TUid aDomain, 
                                TUid aCategory, 
                                TInt aCommandId );


    public: // Command responding

        /**
        * Issue a response to a command.
        * 
        * @since S60 3.1  
        * @param aDomain    The identifier of the domain       
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aStatus    Result status of the command.
        * @param aData      The parameters of the response.        
        * @return TInt      an error value.
        */
        TInt IssueResponse( TUid aDomain,
                            TUid aCategory, 
                            TInt aCommandId,
                            TInt aStatus, 
                            const TDesC8& aData );
        
    private: // From CActive
    
        /**
        * From CActive::RunL
        */
        void RunL();
        
        /**
        * From CActive::DoCancel()
        */
        void DoCancel();
		
		/**
		* From CActive::RunError()
		*/
		TInt RunError( TInt aError );
    
    private: // new functions
        
        /**
        * Start to receive commands asynchronously
        *
        * @since S60 3.1
        * @return None.
        */
        void StartCommandReceiving();
        
        /**
        * Re-init return data buffer
        *
        * @since S60 3.1
        * @param aSize size of the buffer
        * @return None.
        */
        void ResetDataBufferL( TInt aSize );
        
    private:
    
        /**
        * C++ default constructor.
        *
        * @param mediator command observer
        */
        CMediatorCommandResponderBody( MMediatorCommandObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data

        // A session class to mediator server
        RMediatorServer     iMediatorServer;
        
        // Observer for event callbacks (not owned)
        MMediatorCommandObserver*   iObserver;
        
        // Domain/Category cata for incoming event
        TMediatorCategory           iCategory;
        TMediatorCategoryRetBuffer  iCategoryBuffer;
        
        // Command Id data
        MediatorService::TCommand   iCommand;
        TCommandRetBuffer           iCommandBuffer;
        
        // Command data to be received
        HBufC8*                     iCommandData; 
        // Pointer to command data.
        TPtr8                       iCommandDataPtr;
        
        // Stores information whether destructor has been called, checked in RunL
        TBool* iDestroyed;
        
	};

#endif      // MEDIATORCOMMANDRESPONDERBODY_H
            
// End of File
