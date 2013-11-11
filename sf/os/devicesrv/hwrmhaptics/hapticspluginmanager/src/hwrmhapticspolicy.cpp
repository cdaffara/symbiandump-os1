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
* Description:  Haptics policy implementation.
*
*/


#include <s32file.h>
#include <f32file.h>
#include <hwrmhaptics.h> // min and max device priority values
#include <pathinfo.h>

#include "hwrmhapticspolicy.h"
#include "hwrmhapticstrace.h"

// Internal policy filename prefix
_LIT( KInternalPolicyPrefix, "internal" );

//drive char and colon string length, e.g. "c:"
const TInt KDriveAndColon = 2; 

// Maximum policy file line length
const TInt KMaxLineLength( 100 );

// New line character used to terminate each line in policy file
const TInt KNewLine( '\n' );

// format specifier used to check for feedback clients
_LIT8( KFeedback, "FEEDBACK" );

// format specifier used to match valid UIDs or SIDs
_LIT8( KMatchUid, "0x????????" );

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy* CHWRMHapticsPolicy::NewL( const TDesC& aFilename )
    {
    CHWRMHapticsPolicy* self = NewLC( aFilename );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor. Leaves instance on the cleanup stack.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy* CHWRMHapticsPolicy::NewLC( const TDesC& aFilename )
    {
    CHWRMHapticsPolicy* self = new( ELeave ) CHWRMHapticsPolicy();
    
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::~CHWRMHapticsPolicy()
    {
    // Delete all the array objects
    iClientArray.ResetAndDestroy();
    iClientArray.Close();
    }

// ---------------------------------------------------------------------------
// Method to retrieve the priority of a client with the given ID and if
// it is trusted or not.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPolicy::GetPriority( const TSecureId aSid, 
                                      TBool& aTrusted, 
                                      TInt& aPriority ) const
    {
    TInt ret( KErrNotFound );
    
    // by default not found, i.e. not trusted
    aTrusted = EFalse;
    
    // find the client
    TInt pos = FindClient( aSid );
    
    if( pos != KErrNotFound )
        {
        // if client exists return the priority
        ret = iClientArray[pos]->Priority();
        aTrusted = ETrue;
        }

    aPriority = ret;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::CHWRMHapticsPolicy()
    {
    }

// ---------------------------------------------------------------------------
// By default Symbian 2nd phase constructor is private.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPolicy::ConstructL( const TDesC& aFilename )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Buffer to store the full path and filename
    TFileName filename;
    TFileName internalFilename;
    
    // Get private file path
    User::LeaveIfError( fs.PrivatePath( filename ) );

    // add the filename to path
    internalFilename.Append( PathInfo::RomRootPath().Left( KDriveAndColon ) );
    internalFilename.Append( filename ); // private-folder of haptics server
    internalFilename.Append( KInternalPolicyPrefix ); // "Internal" -prefix
    internalFilename.Append( aFilename ); // actual filename
    filename.Insert( 0, PathInfo::RomRootPath().Left( KDriveAndColon ) );
    filename.Append( aFilename );
    
    // Parse the file and construct the array of policy clients
    // Add product specific file first if they need to change priorieties of
    // S60 defined processes.
    ParsePriorityFileL( fs, filename );
    ParsePriorityFileL( fs, internalFilename );

    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// Method constructs the array of policy clients from the 
// given file 
// A valid line in the file contains priority, id and app name
// priority must be a valid integer from KLowestPriority to KHighestPriority.
// id must be a valid SID or ALL indicating the priority for all clients 
//      not specified in the file.
// app name can be any string indicating the name of the client.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPolicy::ParsePriorityFileL( RFs& aFs, 
                                             const TDesC& aFilename )
    {
    TLex8 lex;
    TInt priority;
    TPtrC8 id;
    TSecureId sid;
    TPtrC8 feedback;
    TBool feedbackClient;
    TPtrC8 appname( KNullDesC8 );
    TInt err( KErrNone );
    
    // Buffer to read each line of the file into
    TBuf8<KMaxLineLength> fileline;
    TChar newLine( KNewLine );  
    RFileReadStream stream;    
    
    COMPONENT_TRACE( ( _L( "CHWRMHapticsPolicy::ParsePriorityFileL - Opening policy file: %S" ), &aFilename ) );
    
    // Open the file and attach to stream 
    err = stream.Open( aFs, aFilename, EFileRead );
    
    // Return without error if file is not found
    if ( err != KErrNone )
        {
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPolicy::ParsePriorityFileL - Policy file open failed: %S, error: %d" ), &aFilename, err ) );
        }
    else
        {
        CleanupClosePushL( stream );
       
        while( err != KErrEof )
            {
            // read from the file upto the newline character
            TRAP( err, stream.ReadL( fileline, newLine ) );

            lex.Assign( fileline );
            if( lex.Val( priority ) == KErrNone )
                {
                // got the priority now validate it
                if( priority >= KHWRMHapticsMinDevicePriority &&
                    priority <= KHWRMHapticsMaxDevicePriority )
                    {
                    // get the id
                    id.Set( lex.NextToken() );

                    // convert it
                    if( ConvertId( id, sid ) != KErrCorrupt )
                        {
                        feedbackClient = EFalse;
                        
                        // check whether feedback client (may be empty)
                        feedback.Set( lex.NextToken() );
                        if( feedback.MatchF( KFeedback ) == 0 )
                            {
                            feedbackClient = ETrue;
                            }
                        
                        // now get the appname (may be empty)
                        appname.Set( lex.NextToken() );
                        
                        // if last token was empty and previous not, but it was
                        // not feedback token, then it must be appname
                        if( appname.Size() == 0 && 
                            ( feedback.Size() != 0 && !feedbackClient ) )
                            {
                            appname.Set( feedback );
                            }
                        
                        // Create the policy
                        AddPolicyClientL( priority, sid, 
                                          feedbackClient, appname );
                        }
                    }
                }
            }

        // Close stream
        CleanupStack::PopAndDestroy( &stream );
        }
    }

// ---------------------------------------------------------------------------
// Helper method to convert and validate SID from a descriptor
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsPolicy::ConvertId( const TDesC8& aSidDes, 
                                    TSecureId& aSid ) const
    {
    TInt ret( KErrNone );
    TUint32 id;

    if( aSidDes.Match( KMatchUid ) == 0 )
        {
        // this is a matching id
        TLex8 lex( aSidDes.Right( 8 ) );
        
        if( lex.Val( id, EHex ) != KErrNone )
            {
            // Failed to convert to int
            ret = KErrCorrupt;
            }
        else
            {
            // conversion ok so set the sid
            aSid.iId = id;
            }
        }
    else
        {
        ret = KErrCorrupt;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// Adds a policy client to the array, first checks that no other 
// client has been registered with the identical sid.
// If one already exists KErrAlreadyExists is returned.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsPolicy::AddPolicyClientL( const TInt aPriority, 
                                           const TSecureId& aSid,
                                           const TBool aFeedbackClient,
                                           const TDesC8& aAppName )
    {
    TInt ret( KErrNone );
    
    // search for existing client
    if( FindClient( aSid ) == KErrNotFound )
        {
        // does not already exist so create and add to the array
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPolicy::AddPolicyClientL - adding client with sid=0x%x" ), aSid.iId ) );

        // create new client to be added to the array and put on cleanup stack
        CPolicyClient* ptrClient = CPolicyClient::NewLC( aPriority, aSid,
                                                         aFeedbackClient,
                                                         aAppName );
        
        // append the new client to the array
        iClientArray.AppendL( ptrClient );

        // array now owns the new client so pop it off the cleanup stack
        CleanupStack::Pop( ptrClient );
        }
    else
        {
        // client already exists so return KErrAlreadyExists
        COMPONENT_TRACE( ( _L( "CHWRMHapticsPolicy::AddPolicyClientL - client already exists with sid=0x%x" ), aSid.iId ) );
        ret = KErrAlreadyExists;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Searches for client with the given sid.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsPolicy::FindClient( const TSecureId& aSid ) const
    {
    TInt index = KErrNotFound;
    
    for( TInt i = 0; i < iClientArray.Count() && index == KErrNotFound; i++ )
        {
        if( aSid == iClientArray[i]->Sid() )
            {
            COMPONENT_TRACE( ( _L( "CHWRMHapticsPolicy::FindClient - client found with sid=0x%x" ), aSid.iId ) );
            index = i;
            }
        }
    
    return index;
    }

// ====================== Embedded CPolicyClient class =======================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::CPolicyClient* 
CHWRMHapticsPolicy::CPolicyClient::NewL( const TInt aPriority,
                                         const TSecureId& aSid,
                                         const TBool aFeedbackClient,
                                         const TDesC8& aAppName )
    {
    CHWRMHapticsPolicy::CPolicyClient* self = NewLC( aPriority, aSid, 
                                                     aFeedbackClient,
                                                     aAppName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor. Leaves instance on the cleanup stack.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::CPolicyClient*
CHWRMHapticsPolicy::CPolicyClient::NewLC( const TInt aPriority, 
                                          const TSecureId& aSid,
                                          const TBool aFeedbackClient, 
                                          const TDesC8& aAppName)
    {
    CHWRMHapticsPolicy::CPolicyClient* self =
        new( ELeave ) CHWRMHapticsPolicy::CPolicyClient( aPriority, aSid, 
                                                         aFeedbackClient );
    
    CleanupStack::PushL( self );
    self->ConstructL( aAppName );
    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::CPolicyClient::~CPolicyClient()
    {
    delete iAppName;
    }

// ---------------------------------------------------------------------------
// Returns the clients priority.
// ---------------------------------------------------------------------------
//
TInt CHWRMHapticsPolicy::CPolicyClient::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------------------------
// Returns the clients ID.
// ---------------------------------------------------------------------------
//
TSecureId CHWRMHapticsPolicy::CPolicyClient::Sid() const
    {
    return iSid;
    }

// ---------------------------------------------------------------------------
// Returns ETrue if feedback client.
// ---------------------------------------------------------------------------
//
TBool CHWRMHapticsPolicy::CPolicyClient::FeedbackClient() const
    {
    return iFeedbackClient;
    }

// ---------------------------------------------------------------------------
// Returns the client application name.
// ---------------------------------------------------------------------------
//
const TDesC& CHWRMHapticsPolicy::CPolicyClient::AppName() const
    {
    return *iAppName;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsPolicy::CPolicyClient::CPolicyClient( TInt aPriority, 
                                                  const TSecureId& aSid,
                                                  const TBool aFeedbackClient)
    : iPriority( aPriority ), iSid( aSid ), iFeedbackClient( aFeedbackClient )
    {
    }

// ---------------------------------------------------------------------------
// By default Symbian 2nd phase constructor is private.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsPolicy::CPolicyClient::ConstructL( const TDesC8& aAppName )
    {
    // create the descriptor for the appname
    iAppName = HBufC::NewL( aAppName.Length() );
    
    // and copy the 8 bit descriptor into it
    iAppName->Des().Copy( aAppName );
    }

// End of File
