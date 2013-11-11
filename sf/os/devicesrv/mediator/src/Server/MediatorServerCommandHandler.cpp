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
* Description:  Managing of commands registered to Mediator
*
*/


// INCLUDE FILES
#include    <e32base.h>

#include    "MediatorServerCommandHandler.h"
#include    "MediatorServerObjects.h"
#include    "MediatorServiceDefs.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::CMediatorServerCommandHandler
// -----------------------------------------------------------------------------
//
CMediatorServerCommandHandler::CMediatorServerCommandHandler(  
            CMediatorServerObjectHandler& aObjectHandler )
    :  iObjectHandler( aObjectHandler )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::NewL
// -----------------------------------------------------------------------------
//
CMediatorServerCommandHandler* CMediatorServerCommandHandler::NewL(  
            CMediatorServerObjectHandler& aObjectHandler )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::NewL"));
    CMediatorServerCommandHandler* self 
        = new( ELeave ) CMediatorServerCommandHandler( aObjectHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::~CMediatorServerCommandHandler
// -----------------------------------------------------------------------------
//    
CMediatorServerCommandHandler::~CMediatorServerCommandHandler()
    {
    iCommandPendingList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::RegisterCommandListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::RegisterCommandListL( 
                TMediatorCategory aCategory, 
                const RCommandList& aCommands,
                TSecureId aSecureId,
                MMediatorCommandObserver* aObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::RegisterCommandListL"));
    // Check that domain exists --> if not add new
    CDomain* domain = iObjectHandler.FindDomain( aCategory.iDomain );
    if ( !domain )
        {
        domain = iObjectHandler.AddDomainL( aCategory.iDomain );
        }
    
    // Check that category exists --> if not add new
    TInt ignore = 0; // not used here    
    CCategory* category = domain->FindCategory( aCategory.iCategory, ignore );
    if ( !category )
        {
        category = domain->AddCategoryL( aCategory.iCategory );
        }
    
    // Loop through the commands and add them to list
    // Take the possible error to variable
    TInt error = KErrNone;
    TBool stop = EFalse;
    TInt index = 0;
    for ( index = 0; index < aCommands.Count() && !stop; index++ )
        {
        CCommand* newCommand = CCommand::NewL( aCommands[index] );
        CleanupStack::PushL( newCommand );
        newCommand->SetSecureId( aSecureId );   // For unregistering
        newCommand->SetObserver( aObserver );   // For getting the commands
        newCommand->SetCommitState( CItem::EAdded ); // For transaction handling
        TInt addError = category->AddCommand( newCommand );
        if ( addError )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerCommandHandler::RegisterCommandListL: addError=%d\n"), addError ) );
            ERROR_TRACE(Print(_L("[Mediator] Failed to add command %d to category %d of domain %d\n"), newCommand->Id(), 
                                                                                                       aCategory.iCategory.iUid,
                                                                                                       aCategory.iDomain.iUid ) );
            // in case of error, delete event and take error
            CleanupStack::PopAndDestroy( newCommand );
            error = addError;
            stop = ETrue;
            }
        else
            {
            // Event has been added properly --> just pop
            CleanupStack::Pop( newCommand );
            }     
        newCommand = NULL;
        }  
    
    TRACE(Print(_L("[Mediator Server]\t Commands registered:\n")));
    TRACE(Print(_L("[Mediator Server]\t Success/count: %d/%d \tstatus: %d"), index, aCommands.Count(), error ));    
    
    // Check error if we need to do partial recovery
    if ( error != KErrNone ) 
        {
        // Remove the registered commands
        category->RollbackCommands();
        }
    else
        {
        // Complete command registration
        category->CommitCommands();
        
        // Use the object handler to notify command registration
        iObjectHandler.CommandsAdded( aCategory.iDomain, 
                                      aCategory.iCategory, 
                                      aCommands );
        }
    // In the end leave if error --> client gets error code
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::UnregisterCommandListL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::UnregisterCommandListL( TMediatorCategory aCategory, 
                                                            const RCommandList& aCommands,
                                                            TSecureId aSecureId )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::UnregisterCommandListL"));
    CCategory* category = iObjectHandler.CategoryL( aCategory );
    TInt error = KErrNone;
    if ( category )
        {
        TBool stop = EFalse;
        TInt index = 0;
        
        // Loop through the list of commands and unregister those.
        for ( index = 0; index < aCommands.Count() && !stop; index++ )
            {
            TInt commandIndex = 0;
            TCommand removeCommand = aCommands[index];
            CCommand* commandPtr = category->FindCommand( removeCommand.iCommandId, 
                                                           commandIndex );
            if ( !commandPtr )
                {
                ERROR_LOG(_L("[Mediator] CMediatorServerCommandHandler::UnregisterCommandListL: Command not found\n") );
                ERROR_TRACE(Print(_L("[Mediator] Failed to remove command %d in category %d of domain %d\n"), removeCommand.iCommandId, 
                                                                                                       aCategory.iCategory.iUid,
                                                                                                       aCategory.iDomain.iUid ) );
                error = KErrMediatorCommandNotFound;
                stop = ETrue;
                }
            else
                {
                // Found the command --> is it own registration?
                if ( commandPtr->SecureId() != aSecureId )
                    {
                    ERROR_LOG(_L("[Mediator] CMediatorServerCommandHandler::UnregisterCommandListL: Secure ID mismatch\n") );
                    ERROR_TRACE(Print(_L("[Mediator] commandPtr()->SecureId()=0x%x, aSecureId=0x%x\n"), commandPtr->SecureId().iId, 
                                                                                                        aSecureId.iId ) );
                    error = KErrMediatorSecureIdMismatch;
                    stop = ETrue;
                    }
                else    // Should be ok to unregister
                    {
                    commandPtr->SetCommitState( CItem::ERemoved );
                    }
                }
            }
            
        TRACE(Print(_L("[Mediator Server]\t Commands unregistered:\n")));
        TRACE(Print(_L("[Mediator Server]\t Processed/Total: %d/%d \tstatus: %d"), index, aCommands.Count(), error ));    
        
        // Check error status --> if there's error, need to roll back
        if ( error != KErrNone )
            {
            category->RollbackCommands();    
            }
        else
            {
            category->CommitCommands();
            
            // loop through unregistered commands, and if they can be found from pending commands list,
            // inform the command issuer
            for ( TInt i = 0; i < aCommands.Count(); i++ )
                {
                // ignore return value
                IssueResponse( aCategory, aCommands[i], KNullDesC8, KErrMediatorCommandRemoved );
                }
            
            // Use the object handler to notify command registration
            iObjectHandler.CommandsRemoved( aCategory.iDomain, 
                                            aCategory.iCategory, 
                                            aCommands );
            }
        
        }
    // Leave in case of error situation
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::IssueCommandL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::IssueCommandL( 
                                        TMediatorCategory aCategory, 
                                        MediatorService::TCommand aCommand,
                                        const TDesC8& aData,
                                        TCapabilitySet aCaps,
                                        MMediatorCommandResponseObserver* aObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::IssueCommandL"));
    CCategory* category = iObjectHandler.CategoryL( aCategory );
    if ( category )
        {
        // Find the command from register list
        TInt ignore = 0;
        CCommand* commandPtr = category->FindCommand( aCommand.iCommandId, ignore );
        if ( !commandPtr )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerCommandHandler::IssueCommandL: Command %d not found in category %d of domain %d\n"), aCommand.iCommandId, 
                                                                                                                                                   aCategory.iCategory.iUid,
                                                                                                                                                   aCategory.iDomain.iUid ) );
            
            User::Leave( KErrMediatorCommandNotFound );
            }     
        // Then check the capabilities && the version information
        // Capabilities are checked so that boolean ETrue is returned
        // when all parameter caps can be found from aCaps
        if ( !aCaps.HasCapabilities( commandPtr->Policy() ) )
            {
#ifdef _DEBUG
            for ( TInt index = 0; index < ECapability_Limit; index++ )
                {
                TCapabilitySet commandCaps = commandPtr->Policy();
                TBool command = commandCaps.HasCapability( (TCapability) index );
                TBool requestor = aCaps.HasCapability( (TCapability) index );
                if ( command && !requestor )
                    {
                    ERROR_TRACE(Print(_L("[Mediator] CMediatorServerCommandHandler::IssueCommandL: capability %d missing\n"), index ));
                    ERROR_TRACE(Print(_L("[Mediator] Capability error when issuing command %d in category %d of domain %d\n"), aCommand.iCommandId, 
                                                                                                                                 aCategory.iCategory.iUid,
                                                                                                                                 aCategory.iDomain.iUid ) );
                    }
                }
#endif    
            User::Leave( KErrPermissionDenied );
            }
        // Check (major) version match
        if ( aCommand.iVersion.iMajor != commandPtr->Version().iMajor )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerCommandHandler::IssueCommandL: registered=%d, issued=%d\n"), 
                                                                                        commandPtr->Version().iMajor,
                                                                                        aCommand.iVersion.iMajor ));
            ERROR_TRACE(Print(_L("[Mediator] Version error when issuing command %d in category %d of domain %d\n"), aCommand.iCommandId, 
                                                                                                                      aCategory.iCategory.iUid,
                                                                                                                      aCategory.iDomain.iUid ) );
            // There's a major version mismatch
            User::Leave( KErrMediatorVersionMismatch );
            }
        
        // If ok, issue to command to client
        // Make the new command, set initiator and responder
        // We don't need to command data for the pending list
        CCommand* newCommand = CCommand::NewL( aCommand );
        CleanupStack::PushL( newCommand );
        
        newCommand->SetResponseObserver( aObserver );
        newCommand->SetObserver( commandPtr->Observer() );
        newCommand->SetDomain( aCategory.iDomain );
        newCommand->SetCategory( aCategory.iCategory );
        newCommand->SetTimeout( commandPtr->Timeout() );
        
        // Start command timing, if it is not an infinite command
        if ( commandPtr->Timeout() != KMediatorTimeoutInfinite )
            {
            // Start timeout timer ( request callback here )
            newCommand->StartTimer( this );    
            }
                
        iCommandPendingList.AppendL( newCommand );
        
        CleanupStack::Pop( newCommand );
        
        // Now send the command to correct responder --> we have it pending
        commandPtr->Observer()->MediatorCommandL( aCategory.iDomain,
                                                  aCategory.iCategory,
                                                  aCommand.iCommandId,
                                                  aCommand.iVersion,
                                                  aData );
        }
    }


// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::CancelCommandL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::CancelCommand( 
                                        TMediatorCategory aCategory, 
                                        MediatorService::TCommand aCommand )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::CancelCommand"));
    TBool found = EFalse;
    for ( TInt index = 0; index < iCommandPendingList.Count() && !found; index ++ )
        {
        CCommand* commandPtr = iCommandPendingList[index];
        if ( commandPtr )
            {
            TUid domain = aCategory.iDomain;
            TUid category = aCategory.iCategory;
            TInt commandId = aCommand.iCommandId;
            if ( ( domain == commandPtr->Domain() )
                 && ( category == commandPtr->Category() )
                 && ( commandId == commandPtr->Id() ) )
                {
                // We have found the correct command --> cancel it 
                TRAP_IGNORE( commandPtr->Observer()->CancelMediatorCommandL( domain,
                                                                             category,
                                                                             commandId ) );
                // Remove the command from list, free memory && stop looping
                iCommandPendingList.Remove( index );
                delete commandPtr;
                commandPtr = NULL;
                found = ETrue;                                                                  
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::CancelCommands
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::CancelCommands( MMediatorCommandObserver* aObserver,
                                                    MMediatorCommandResponseObserver* aResponseObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::CancelCommand"));
    
    // loop through list of pending commands
    for ( TInt i = iCommandPendingList.Count()-1; i >= 0; i-- )
        {
        
        CCommand* commandPtr = iCommandPendingList[i];
        
        if ( commandPtr )
            {
            
            TBool commandCanceled = EFalse;
            
            // This client has registered the command, and some other client has issued it
            // -> give an error response to the issuer
            if ( aObserver == commandPtr->Observer() )
                {
                TRAP_IGNORE( commandPtr->ResponseObserver()->CommandResponseL( commandPtr->Domain(),
                                                                               commandPtr->Category(),
                                                                               commandPtr->Id(),
                                                                               KErrMediatorCommandRemoved,
                                                                               KNullDesC8 ) );
                commandCanceled = ETrue;                                                                               
                }
            
            // This client has issued the command
            // -> inform the client which registered the command that it is canceled
            if ( aResponseObserver == commandPtr->ResponseObserver() )
                {
                TRAP_IGNORE( commandPtr->Observer()->CancelMediatorCommandL( commandPtr->Domain(),
                                                                             commandPtr->Category(),
                                                                             commandPtr->Id() ) );
                commandCanceled = ETrue;                                                                             
                }
            
            // pending command was registered and/or issued by this client, so now it can be removed
            // from the list
            if ( commandCanceled )
                {
                iCommandPendingList.Remove( i );
                delete commandPtr;
                }
            
            }
        
        }

    }
    

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::IssueResponse
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorServerCommandHandler::IssueResponse( 
                                        TMediatorCategory aCategory, 
                                        MediatorService::TCommand aCommand,
                                        const TDesC8& aData,
                                        TInt aStatus )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::IssueResponse"));
    TBool found = EFalse;
    for ( TInt index = 0; index < iCommandPendingList.Count() && !found; index ++ )
        {
        CCommand* commandPtr = iCommandPendingList[index];
        if ( commandPtr )
            {
            TUid domain = aCategory.iDomain;
            TUid category = aCategory.iCategory;
            TInt commandId = aCommand.iCommandId;
            if ( ( domain == commandPtr->Domain() )
                 && ( category == commandPtr->Category() )
                 && ( commandId == commandPtr->Id() ) )
                {
                // We have found the correct command --> ignore the leave 
                TRAP_IGNORE( commandPtr->ResponseObserver()->CommandResponseL( domain,
                                                                               category,
                                                                               commandId,
                                                                               aStatus,
                                                                               aData ) );
                // Remove the command from list, free memory && stop looping
                iCommandPendingList.Remove( index );
                delete commandPtr;
                commandPtr = NULL;
                found = ETrue;                                                                  
                }
            }
        }

    if ( !found )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServerCommandHandler::IssueResponse: Command %d not found in category %d of domain %d\n"), aCommand.iCommandId, 
                                                                                                                                             aCategory.iDomain.iUid ) );    
        }
            
    return found ? KErrNone : KErrMediatorCommandNotFound;

    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::IssueResponseL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::IssueResponseL( 
                                        TMediatorCategory aCategory, 
                                        MediatorService::TCommand aCommand,
                                        const TDesC8& aData,
                                        TInt aStatus )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::IssueResponseL"));
    
    TInt err = IssueResponse( aCategory, aCommand, aData, aStatus );
        
    if ( err != KErrNone )
        {
        User::Leave( KErrMediatorCommandNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerCommandHandler::TimerCallBack
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerCommandHandler::TimerCallBack( TUid aDomain, 
                                                   TUid aCategory, 
                                                   TInt aCommandId )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerCommandHandler::TimerCallBack"));
    TBool found = EFalse;
    for ( TInt index = 0; index < iCommandPendingList.Count() && !found; index ++ )
        {
        CCommand* commandPtr = iCommandPendingList[index];
        if ( commandPtr )
            {
            if ( ( aDomain == commandPtr->Domain() )
                 && ( aCategory == commandPtr->Category() )
                 && ( aCommandId == commandPtr->Id() ) )
                {
                // We have found the correct command --> ignore the leave 
                // Send the response to initiator with timeout error
                TRAP_IGNORE( commandPtr->ResponseObserver()->CommandResponseL( aDomain,
                                                                               aCategory,
                                                                               aCommandId,
                                                                               KErrMediatorTimeout,
                                                                               KNullDesC8 ) );
                // And clear the responder side also                                                                  
                TRAP_IGNORE( commandPtr->Observer()->MediatorCommandTimeoutL( aDomain,
                                                                              aCategory,
                                                                              aCommandId ) );                                                                  
                // Remove the command from list, free memory && stop looping
                iCommandPendingList.Remove( index );
                delete commandPtr;
                commandPtr = NULL;
                found = ETrue;                                                                  
                }
            }
        }
    }



//  End of File  
