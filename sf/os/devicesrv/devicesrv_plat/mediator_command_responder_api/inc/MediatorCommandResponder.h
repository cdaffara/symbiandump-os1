/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An interface for responding to Mediator Service commands.
*
*/

#ifndef MEDIATORCOMMANDRESPONDER_H
#define MEDIATORCOMMANDRESPONDER_H

// INCLUDES
#include <e32base.h>

#include <MediatorServiceDefs.h>

// FORWARD DECLARATION
class CMediatorCommandResponderBody;

// CLASS DECLARATION

/**
*  An observer interface for receiving Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class MMediatorCommandObserver
    {
    public: // Mediator Service Command API

        /**
        * A Mediator Service command.
        *
        * @since S60 3.1     
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version information of the command.
        * @param aData      The parameters of the command.
        * @return None.
        */
        virtual void MediatorCommandL( TUid aDomain,
                                       TUid aCategory, 
                                       TInt aCommandId,
                                       TVersion aVersion, 
                                       const TDesC8& aData ) = 0;

        /**
        * Cancel a Mediator Service command.
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        virtual void CancelMediatorCommand( TUid aDomain,
                                            TUid aCategory, 
                                            TInt aCommandId ) = 0;
    };


// CLASS DECLARATION

/**
*  A class used to respond to Mediator Service commands.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class CMediatorCommandResponder : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver  mediator command observer
        * @return a new CMediatorCommandResponder instance
        */       
        IMPORT_C static CMediatorCommandResponder* NewL(
            MMediatorCommandObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorCommandResponder();

    public: // Command registration

        /**
        * Register a command category. After registration 
        * all commands from the category may be initiated.
        * 
        * @since S60 3.1  
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommands  The command list to be registered.
        * @return TInt      an error value.
        */
        IMPORT_C TInt RegisterCommand( TUid aDomain,
                                       TUid aCategory, 
                                       const MediatorService::RCommandList& aCommands );

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
        *                   A special value for infinite timeout(KMediatorTimeoutInfinite) can be given,
        *                   meaning that the command is completed only by the responder and timeout mechanism
        *                   will never be applied for the command.
        * @return TInt      an error value.
        */
        IMPORT_C TInt RegisterCommand( TUid aDomain, 
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
        IMPORT_C TInt UnregisterCommand( TUid aDomain, 
                                         TUid aCategory,
                                         const MediatorService::RCommandList& aCommands );

        /**
        * Unregister a command.
        *
        * @since S60 3.1  
        * @param aDomain    The identifier of the domain       
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the category.
        * @return TInt      an error value.
        */
        IMPORT_C TInt UnregisterCommand( TUid aDomain, 
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
        IMPORT_C TInt IssueResponse( TUid aDomain,
                                     TUid aCategory, 
                                     TInt aCommandId,
                                     TInt aStatus, 
                                     const TDesC8& aData );

    
        
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorCommandResponder();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aObserver  mediator command observer
        * @return None.
        */
        void ConstructL( MMediatorCommandObserver* aObserver );

    private: // Data

        /**
        * Pointer to implementation class
        * Own.
        */
        CMediatorCommandResponderBody*  iBody;
        
	};

#endif      // MEDIATORCOMMANDRESPONDER_H
            
// End of File
