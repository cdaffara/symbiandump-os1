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
* Description:  Manages commands registered to Mediator.
*
*/

#ifndef MEDIATORSERVERCOMMANDHANDLER_H
#define MEDIATORSERVERCOMMANDHANDLER_H

// INCLUDES
#include <e32base.h>
#include "MediatorCommon.h"
#include "MediatorServerObjectHandler.h"
#include "MediatorServiceDefs.h"
#include "MediatorCommandTimer.h"


// NAMESPACE
using namespace MediatorService;

// FORWARD DECLARATIONS
class CMediatorServerObjectHandler;
class CDomain;
class CCategory;
class CItem;
class CCommand;


// CLASS DECLARATION

/**
*  Command handler class for Mediator Server.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServerCommandHandler : public CBase, public MMediatorTimerCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObjectHandler object handler
        * @return A new CMediatorServerCommandHander instance
        */
        static CMediatorServerCommandHandler* NewL(  
                    CMediatorServerObjectHandler& aObjectHandler );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorServerCommandHandler();

    public: // New functions
        
        /**
        * Register list of new commands.
        *
        * Checks that no existing commands are registered
        * @since S60 3.1
        * @param aCategory  information for the category
        * @param aCommands  list of commands to be registered
        * @param aSecureId  secure Id of the client
        * @param aObserver  observer instance for incoming commands
        * @return none.
        */
        void RegisterCommandListL( TMediatorCategory aCategory, 
                                   const RCommandList& aCommands,
                                   TSecureId aSecureId,
                                   MMediatorCommandObserver* aObserver );
                                 
         
        /**
        * Unregisters command(s)
        * @since S60 3.1
        * @param aCategory  information for the category
        * @param aCommands  list of commands to be unregistered
        * @param aSecureId  secure Id of the client
        * @return none.
        */
        void UnregisterCommandListL( TMediatorCategory aCategory, 
                                     const RCommandList& aCommands,
                                     TSecureId aSecureId  );   
       
        
        /**
        * Issue command
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aCommand   Command information
        * @param aData      Command parameter data
        * @param aCaps      Capabilities of the client
        * @param aObserver  Command response observer
        * @return none.
        */

        void IssueCommandL( TMediatorCategory aCategory, 
                            MediatorService::TCommand aCommand,
                            const TDesC8& aData,
                            TCapabilitySet aCaps,
                            MMediatorCommandResponseObserver* aObserver );  
        
        /**
        * Issue response
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aCommand   Command information
        * @param aData      Command parameter data
        * @param aStatus    Status of the response
        * @return error code    KErrNone                    response was issued
        *                       KMediatorErrCommandNotFound command was not found
        */

        TInt IssueResponse( TMediatorCategory aCategory, 
                            MediatorService::TCommand aCommand,
                            const TDesC8& aData,
                            TInt aStatus );  

        
        /**
        * Issue response
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aCommand   Command information
        * @param aData      Command parameter data
        * @param aStatus    Status of the response
        * @return none.
        * @leave KMediatorErrCommandNotFound    command was not found
        */

        void IssueResponseL( TMediatorCategory aCategory, 
                             MediatorService::TCommand aCommand,
                             const TDesC8& aData,
                             TInt aStatus );                              
        
        /**
        * Cancel command
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aCommand   Command information
        * @return none.
        */

        void CancelCommand( TMediatorCategory aCategory, 
                            MediatorService::TCommand aCommand );
                            
        
        /**
        * Cancels all the pending commands that are either 
        * registered or initiated by the client
        *
        * @since S60 3.1
        * @param aObserver          command observer
        * @param aResponseObserver  command response observer (initiator)
        * @return none.
        */
                
        void CancelCommands( MMediatorCommandObserver* aObserver, MMediatorCommandResponseObserver* aResponseObserver );
                                                        
        /**
        * Returns count of pending commands
        *
        * @since S60 3.1
        * @return count of pending commands
        */
        
        TInt CommandPendingCount() { return iCommandPendingList.Count(); }
        
    public: // From timer
    
        /**
        * Timer callback implementation
        *
        * @since S60 3.1
        * @param aDomain    domain of the command
        * @param aCategory  category of the command
        * @param aCommandId id of the command
        * @return None.
        */      
        void TimerCallBack( TUid aDomain, 
                            TUid aCategory, 
                            TInt aCommandId );                            
        
    private:

        /**
        * C++ default constructor.
        * 
        * @param aObjectHandler object handler
        */
        CMediatorServerCommandHandler(  
                    CMediatorServerObjectHandler& aObjectHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        
        // Handle to object handler
        CMediatorServerObjectHandler& iObjectHandler;
        
        // List of pending commands
        RPointerArray<CCommand>  iCommandPendingList;
        
    };

#endif      // MEDIATORSERVERCOMMANDHANDLER_H  
            
// End of File
