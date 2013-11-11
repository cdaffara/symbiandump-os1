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
* Description:  Main functions to start the server
*
*/


#include    <e32base.h>
#include    <barsc.h>       // RResourceFile
#include    <barsread.h>    // TResourceReader

#include    "MediatorServer.h"
#include    "MediatorCommon.h"
#include    "MediatorServerSession.h"
#include    "MediatorServerEventHandler.h"
#include    "MediatorServerCommandHandler.h"
#include    "MediatorServerObjectHandler.h"
#include    "MediatorServerPluginHandler.h"

#include    <allowedsids.rsg>
#include    "MediatorDebug.h"
#include    "Debug.h"

// CONSTANTS
// Resource directory
_LIT(KResourceEventDir, "z:\\private\\10207449\\events\\");

// Resource file containing exceptionally handled SIDs
_LIT(KAllowedSidsFile, "z:\\private\\10207449\\allowedsids.rsc");

// Domain resource index ( = 1)
const TInt KEventResourceIndex  = 1;

// Version number count within TVersion structure
const TInt KVersionNumberCount  = 3;

// NAMESPACE
using namespace MediatorService;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServer::CMediatorServer
// -----------------------------------------------------------------------------
//
CMediatorServer::CMediatorServer()
	: CPolicyServer( CActive::EPriorityHigh, KMediatorServerPolicy )
	{
	LOG(_L("[Mediator Server]\t CMediatorServer::CMediatorServer"));
	}

// -----------------------------------------------------------------------------
// CMediatorServer::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorServer::ConstructL()
    {
    LOG(_L("[Mediator Server]\t CMediatorServer::ConstructL"));
    StartL( KMediatorServerName );
    
    // Create object, event and command handlers
    iObjectHandler = CMediatorServerObjectHandler::NewL();
    iEventHandler = CMediatorServerEventHandler::NewL( *iObjectHandler );
    iCommandHandler = CMediatorServerCommandHandler::NewL( *iObjectHandler );
    iPluginHandler = CMediatorServerPluginHandler::NewL();
    
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() );
    CleanupClosePushL( fsSession );
    
    // Do static event registrations
    ProcessResourceEventsL( fsSession );
    
    // read SIDs
    ReadAllowedSidsFileL( fsSession );
    
    CleanupStack::PopAndDestroy( &fsSession );
    
    RMediatorDebug::Initialize( this );
    
    LOG(_L("[Mediator Server]\t CMediatorServer::ConstructL end"));
    }

// -----------------------------------------------------------------------------
// CMediatorServer::NewLC
// -----------------------------------------------------------------------------
//
CMediatorServer* CMediatorServer::NewLC()
	{
	LOG(_L("[Mediator Server]\t CMediatorServer::NewLC"));
	CMediatorServer *self = new (ELeave) CMediatorServer();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CMediatorServer::~CMediatorServer
// -----------------------------------------------------------------------------
//    
CMediatorServer::~CMediatorServer()
	{
	RMediatorDebug::Uninitialize();
	
	LOG(_L("[Mediator Server]\t CMediatorServer::~CMediatorServer"));
	delete iEventHandler;
	delete iCommandHandler;
	delete iObjectHandler;
	delete iPluginHandler;
	iAllowedSids.Close();
	}


// -----------------------------------------------------------------------------
// CMediatorServer::NewSessionL
// Creates a new session to the server if version information is correct.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2 *CMediatorServer::NewSessionL( const TVersion &aVersion, 
										 const RMessage2 &/*aMessage*/) const
	{
	LOG(_L("[Mediator Server]\t CMediatorServer::NewSessionL"));
	// check we're the right version
	TVersion version( KMediatorServerMajor, 
					  KMediatorServerMinor, 
					  KMediatorServerBuild);
	if( !User::QueryVersionSupported( version, aVersion ) )
	    {
	    ERROR_LOG(_L("[Mediator] CMediatorServer::NewSessionL: Version conflict!\n") );
	    ERROR_TRACE(Print(_L("[Mediator] Current version:%d.%d.%d\n"), version.iMajor, version.iMinor, version.iBuild ) );
	    ERROR_TRACE(Print(_L("[Mediator] Connected version:%d.%d.%d\n"), aVersion.iMajor, aVersion.iMinor, aVersion.iBuild ) );
	    User::Leave( KErrNotSupported );
	    }
		
	// make new session
	return CMediatorServerSession::NewL();
	}

// -----------------------------------------------------------------------------
// CMediatorServer::EventHandler
// Returns a reference to event handler
// -----------------------------------------------------------------------------
//
CMediatorServerEventHandler& CMediatorServer::EventHandler()
    {
    __ASSERT_DEBUG( iEventHandler, PanicServer( EMediatorServerNoEventHandler ));
    return *iEventHandler;
    }
        
// -----------------------------------------------------------------------------
// CMediatorServer::EventHandler
// Returns a reference to command handler
// -----------------------------------------------------------------------------
//
CMediatorServerCommandHandler& CMediatorServer::CommandHandler()
    {
    __ASSERT_DEBUG( iCommandHandler, PanicServer( EMediatorServerNoCommandHandler ));
    return *iCommandHandler;
    }

// -----------------------------------------------------------------------------
// CMediatorServer::ObjectHandler
// Returns a reference to object handler
// -----------------------------------------------------------------------------
//
CMediatorServerObjectHandler& CMediatorServer::ObjectHandler()
    {
    __ASSERT_DEBUG( iObjectHandler, PanicServer( EMediatorServerNoObjectHandler ));
    return *iObjectHandler;
    }

// -----------------------------------------------------------------------------
// CMediatorServer::PluginHandler
// Returns a reference to plugin handler
// -----------------------------------------------------------------------------
//
CMediatorServerPluginHandler& CMediatorServer::PluginHandler()
    {
    __ASSERT_DEBUG( iPluginHandler, PanicServer( EMediatorServerNoPluginHandler ));
    return *iPluginHandler;
    }

// -----------------------------------------------------------------------------
// CMediatorServer::CustomSecurityCheckL
// Performs a custom security check.
// -----------------------------------------------------------------------------
//

CPolicyServer::TCustomResult CMediatorServer::CustomSecurityCheckL( const RMessage2& aMsg, 
                                                                    TInt& /*aAction*/,
                                                                    TSecurityInfo& /*aMissing*/ )
    {
    LOG(_L("[Mediator Server]\t CMediatorServer::CustomSecurityCheckL"));
    // firstly check if this SwEvent capability, which is OK
    if ( aMsg.HasCapability( ECapabilitySwEvent ) )
        {
        return EPass;
        }
    
    // secondly check if special treatment for this SID is allowed
    if ( AllowCapabilityException( static_cast<TUid>( aMsg.SecureId() ) ) )
        {
        return EPass;
        }
    
    ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::CustomSecurityCheckL: security check fail for client 0x%08X\n"), aMsg.SecureId().iId));

    // client has no right to send this message
    return EFail;
    }

// -----------------------------------------------------------------------------
// CMediatorServer::ProcessResourceEventsL
// Does static event resource registration processing
// -----------------------------------------------------------------------------
//
void CMediatorServer::ProcessResourceEventsL( RFs& aFsSession )
    {
    LOG(_L("[Mediator Server]\t CMediatorServer::ProcessResourceEventsL"));
    
    // Gather all files from registration directory
    CDir* resourceList;
	
    TInt err = aFsSession.GetDir( KResourceEventDir,
		                         KEntryAttNormal,
		                         ESortByName, 
		                         resourceList );
    
    // If events-directory does not exist, it means that there are no static events registered
    if ( err != KErrNone && err != KErrPathNotFound )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::ProcessResourceEventsL: err=%d\n"), err ) );
        User::Leave( err );
        }
    
    if ( err != KErrPathNotFound )    		                                  
        {
        CleanupStack::PushL( resourceList );
        TRACE(Print(_L("[Mediator Server]\t ProcessResourceEventsL file count %d\n"), resourceList->Count() ));
    
        // Loop through the resource files and process the resources
        for ( TInt index = 0; index < resourceList->Count(); index++ )
            {
            TEntry resourceFile = (*resourceList)[ index ];
            TBufC<KMaxFileName> fileName = resourceFile.iName;
            // TRAP error, but continue
            TRAPD( error, ProcessResourceFileL( aFsSession, fileName ) );
            
            if ( error != KErrNone )
                {
                ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::ProcessResourceEventsL: error=%d\n"), error ) );
                }
                
            TRACE(Print(_L("[Mediator Server]\t ProcessResourceEventsL file %d process status %d\n"), index, error )); 
            }
        CleanupStack::PopAndDestroy( resourceList );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServer::ProcessResourceFileL
// Processes one resource file
// -----------------------------------------------------------------------------
//
void CMediatorServer::ProcessResourceFileL( RFs& aFsSession, const TDesC& aFileName )
    {
    LOG(_L("[Mediator Server]\t CMediatorServer::ProcessResourceFileL"));
    
    // Parse full file name
    TParse parse;
    parse.Set( aFileName, &KResourceEventDir, NULL );
    TFileName fullName = parse.FullName();
    
    // Get this thread's secure id
    RThread thread = RThread();
    TSecureId secureId = thread.SecureId();
    
    // Open resource file
    RResourceFile resourceFile;
    resourceFile.OpenL( aFsSession, fullName );
	CleanupClosePushL( resourceFile );
		
    // Read resource information		
	HBufC8* domainResource = resourceFile.AllocReadLC( KEventResourceIndex );
	TResourceReader reader;
	reader.SetBuffer( domainResource );
	
	// Read domain from resource
	TUid domainUid = TUid::Uid( reader.ReadInt32() );
	// Process categories
	TInt categoryCount = reader.ReadInt16();
	for ( TInt index = 0; index < categoryCount; index++ )
	    {
	    TUid categoryUid = TUid::Uid( reader.ReadInt32() );
	    // Process events within category
	    TInt eventCount = reader.ReadInt16();
	    REventList eventList;
	    for ( TInt index2 = 0; index2 < eventCount; index2++ )
	        {
	        MediatorService::TEvent newEvent = ReadEvent( reader );
	        eventList.AppendL( newEvent );
	        }
	    // Register events to event handler, if there's something to register
	    if ( eventList.Count() > 0 )
	        {
	        TMediatorCategory newCategory;
    	    newCategory.iDomain = domainUid;
    	    newCategory.iCategory = categoryUid;
    	    // TRAP error, but continue anyway
    	    TRAPD( error, 
    	           EventHandler().RegisterEventListL( newCategory, 
    	                                              eventList, 
    	                                              secureId ) );
            
            if ( error != KErrNone )
                {
                ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::ProcessResourceFileL: error=%d\n"), error ) );
                }
            
    	                                              
    	    TRACE(Print(_L("[Mediator Server]\t ProcessResourceFileL events registered with status %d\n"), error ));                                          
	        }  
	    eventList.Close();
	    }
		
	CleanupStack::PopAndDestroy( domainResource);
	CleanupStack::PopAndDestroy( &resourceFile );
    }


// -----------------------------------------------------------------------------
// CMediatorServer::ReadEvent
// Reads one event from resource
// -----------------------------------------------------------------------------
//
MediatorService::TEvent CMediatorServer::ReadEvent( TResourceReader& aReader )
    {
    MediatorService::TEvent newEvent;
    
    // Read Event id
    newEvent.iEventId =  aReader.ReadInt16();
    
    TRACE(Print(_L("[Mediator Server]\t ProcessResourceFileL new event id %d\n"), newEvent.iEventId )); 
    
    // Read required capabilities
    TCapabilitySet capabilitySet;
    capabilitySet.SetEmpty();
    TInt capabilityCount = aReader.ReadInt16();
    for ( TInt index = 0; index < capabilityCount; index++ )
        {
        TCapability newCapability = (TCapability) aReader.ReadInt16();
        capabilitySet.AddCapability( newCapability );  
        }
    newEvent.iCaps = capabilitySet;
    
    // Read version information
    TInt versionCount = aReader.ReadInt16();
    if ( versionCount == KVersionNumberCount ) // Should have 3 values in version structure
        {
        TInt major = aReader.ReadInt16();
        TInt minor = aReader.ReadInt16();
        TInt build = aReader.ReadInt16();
        newEvent.iVersion = TVersion( major, minor, build );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::ReadEvent: Event %d has invalid data, versionCount=%d\n"), newEvent.iEventId, versionCount ) );
        // Something wrong with the resource --> use zeros as version info
        newEvent.iVersion = TVersion( 0, 0, 0 );
        }    
    // Return event 
    return newEvent;
    }

// -----------------------------------------------------------------------------
// CMediatorServer::ReadAllowedSidsFileL
// Read resource file containing exeptionally handled SIDs
// -----------------------------------------------------------------------------
//
void CMediatorServer::ReadAllowedSidsFileL( RFs& aFsSession )
    {
    LOG(_L("[Mediator Server]\t CMediatorServer::ReadAllowedSidsFileL"));
    
    // read settings from resource files
    TFileName resourceFileName (KAllowedSidsFile);
    
    RResourceFile resourceFile;
    resourceFile.OpenL(aFsSession, resourceFileName);
    
    CleanupClosePushL(resourceFile);
    
    // Read the SID structure
    HBufC8* res = resourceFile.AllocReadLC(SID_INFO);
    TResourceReader theReader;
    theReader.SetBuffer(res);

    TInt sidCount = theReader.ReadUint16();
    
    // read SIDs
    TUid sid;
        
    for (TInt i = 0; i < sidCount; i++)
        {
        sid.iUid = theReader.ReadUint32();
        TRACE(Print(_L("[Mediator Server]\t Allowed SID: 0x%08X\n"), sid.iUid)); 
        User::LeaveIfError( iAllowedSids.Append( sid ) );
        }    
    
    CleanupStack::PopAndDestroy( res );
    CleanupStack::PopAndDestroy( &resourceFile );
    }

// -----------------------------------------------------------------------------
// CMediatorServer::AllowCapabilityException
// Returns whether capability requirements can be ignored for the specified client.
// -----------------------------------------------------------------------------
//
TBool CMediatorServer::AllowCapabilityException( const TUid& aSid )
    {
    return ( ( iAllowedSids.Find ( aSid ) != KErrNotFound ) ? ETrue : EFalse );
    }

    
// -----------------------------------------------------------------------------
// PanicServer
// Server panic handler
// Panic our own thread
// -----------------------------------------------------------------------------
//
void CMediatorServer::PanicServer(TInt aPanic)
    {
    ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::PanicServer: Reason = %d\n"), aPanic));
    User::Panic( KMediatorServerPanic, aPanic );
    }

// -----------------------------------------------------------------------------
// PanicClient
// Client panic handler
// RMessage2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServer::PanicClient( const RMessage2& aMessage, TInt aPanic )
    {
    ERROR_TRACE(Print(_L("[Mediator] CMediatorServer::PanicClient: Reason = %d\n"), aPanic));
    aMessage.Panic( KMediatorServerPanic, aPanic );
    }

// End of File
