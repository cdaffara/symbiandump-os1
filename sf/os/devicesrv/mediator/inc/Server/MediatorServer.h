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
* Description:  Mediator Server executable header file
*
*/



#ifndef MEDIATORSERVER_H
#define MEDIATORSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <barsread.h>
#include <MediatorServiceDefs.h>

// NAMESPACE

// CONSTANTS
// ----------------------------------------------------------------------------------------
// Server's policy
// ----------------------------------------------------------------------------------------

//Total number of ranges
const TUint KMediatorServerRangeCount = 2;

//Definition of the ranges of IPC numbers
const TInt KMediatorServerRanges[KMediatorServerRangeCount] = 
        {
        0,  // 1st range messages 0 - 4
        5   // 2nd range messages 5 - 
        }; 

//Policy to implement for each of the above ranges        
const TUint8 KMediatorServerElementsIndex[KMediatorServerRangeCount] = 
        {
        CPolicyServer::ECustomCheck,  // Applies to 1st range 
        CPolicyServer::EAlwaysPass, //applies to 2nd range
        };

//Specific capability checks
const CPolicyServer::TPolicyElement KMediatorServerElements[] = 
        {
        //policy for range "0"; custom check is done
        {_INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient}  
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy KMediatorServerPolicy =
        {
        CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
        KMediatorServerRangeCount,
        KMediatorServerRanges,
        KMediatorServerElementsIndex,
        KMediatorServerElements
        };



// DATA TYPES

/*
* Panic definitions
*/
enum TUPnPMediaServerPanic
    {
    EMSBadRequest,
    EMSBadDescriptor,
    EMSBadMediatorServerMessage,
    };


// FORWARD DECLARATIONS
class CMediatorServerEventHandler;
class CMediatorServerCommandHandler;
class CMediatorServerObjectHandler;
class CMediatorServerPluginHandler;

// CLASS DECLARATION

/**
*  Mediator Server Core class
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServer : public CPolicyServer
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMediatorServer* NewLC();
        
        /**
        * Destructor.
        */
        ~CMediatorServer();

    public: // New functions
    
        /**
        * Returns reference to event handler
        *
        * @since S60 3.1
        * @param None.
        * @return CMediatorServerEventHandler reference to event handler
        */
        CMediatorServerEventHandler& EventHandler();
        
        /**
        * Returns reference to command handler
        *
        * @since S60 3.1
        * @param None.
        * @return CMediatorServerCommandHandler reference to command handler
        */
        CMediatorServerCommandHandler& CommandHandler();
        
        /**
        * Returns reference to object handler
        *
        * @since S60 3.1
        * @param None.
        * @return CMediatorServerObjectHandler reference to object handler
        */
        CMediatorServerObjectHandler& ObjectHandler();

        /**
        * Returns reference to plugin handler
        *
        * @since S60 3.1
        * @param None.
        * @return CMediatorServerPluginHandler reference to plugin handler
        */
        CMediatorServerPluginHandler& PluginHandler();
        
        /**
        * Method to panic server in case of serious error
        *
        * @since S60 3.1
        * @param aPanic panic reason.
        * @return None
        */
        static void PanicServer( TInt aPanic );

        /**
        * Method to panic client if e.g. message is malformed
        *
        * @since S60 3.1
        * @param aMessage message from client
        * @param aPanic panic reason.
        * @return None
        */
        static void PanicClient( const RMessage2& aMessage, TInt aPanic );
        
    
    protected: // from CPolicyServer
        /**
        * Performs a custom security check.
        *
        * @since S60 3.1
        * @param aMsg The message to check.
        * @param aAction A reference to the action to take if the security check fails. 
        * @param aMissing A reference to the list of security attributes missing from the checked process.
        * @return A value from TCustomResult. 
        */
        CPolicyServer::TCustomResult CustomSecurityCheckL( const RMessage2& aMsg, 
                                                           TInt& aAction,
                                                           TSecurityInfo& aMissing );
        
    private: // New functions
    
        /**
        * Reads resource files and registers events
        *
        * @since S60 3.1
        * @param aFsSession reference to file server session handle
        * @return None
        */
        void ProcessResourceEventsL( RFs& aFsSession );
        
        /**
        * Reads a single resource file and process it
        *
        * @since S60 3.1
        * @param aFsSession reference to file server session handle
        * @param aFileName file name of the resource file
        * @return None
        */
        void ProcessResourceFileL( RFs& aFsSession, const TDesC& aFileName );
        
        /**
        * Reads event using resource reader and returns it
        *
        * @since S60 3.1
        * @param aReader reference to resource reader used
        * @return TEvent structure 
        */
        MediatorService::TEvent ReadEvent( TResourceReader& aReader );
        
        /**
        * Read resource file containing exeptionally handled SIDs
        *
        * @since S60 3.1
        * @param aFsSession reference to file server session handle
        * @return None
        */
        void ReadAllowedSidsFileL( RFs& aFsSession );
        
        /**
        * Returns whether capability requirements can be ignored for the specified client.
        *
        * @since S60 3.1
        * @param aSid   Secure id of the client
        * @return TBool ETrue if capability requirement can be ignored, otherwise EFalse.
        */
        TBool AllowCapabilityException( const TUid& aSid );
    
    private:

        /**
        * C++ default constructor.
        */
        CMediatorServer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Functions from base classes
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;
    

    private:    // Data
        
        // Pointer to event handler instance
        CMediatorServerEventHandler*    iEventHandler;
        
        // Pointer to command handler instance
        CMediatorServerCommandHandler*  iCommandHandler;
        
        // Pointer to object handler instance
        CMediatorServerObjectHandler*   iObjectHandler;
        
        // Pointer to plugin handler instance
        CMediatorServerPluginHandler*   iPluginHandler;
        
        // Array of SIDs that don't require SwEvent capability
        RArray<TUid> iAllowedSids;
        

    };


#endif // MEDIATORSERVER_H
           
// End of File
