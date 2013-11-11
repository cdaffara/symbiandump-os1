// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <s32file.h>    // For RFileReadStream

#include "HWRMPolicy.h"
#include "HWRMtrace.h"

// CONSTANTS
// id used for client ALL
_LIT_SECURE_ID(KAllPolicySid,0x00000000);

// Maximum policy file line length
const TInt KMaxLineLength(100);
// New line character used to terminate each line in policy file
const TInt KNewLine('\n');

// format specifier used to check for feedback clients
_LIT8( KFeedback, "FEEDBACK" );


// format specifier used to match valid UIDs or SIDs
_LIT8( KMatchUid, "0x????????" );
// match for ALL client ID
_LIT8( KMatchAll, "ALL" );


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPolicy::CHWRMPolicy
// C++ constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::CHWRMPolicy()
    {
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::~CHWRMPolicy
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::~CHWRMPolicy()
    {
    // Delete all the array objects
    iClientArray.ResetAndDestroy();
    // Delete the default policy
    if( iAllPolicyClient )
        {
        delete iAllPolicyClient;
        }
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy* CHWRMPolicy::NewL( const TDesC& aFilename )
    {
    CHWRMPolicy* self = NewLC( aFilename );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::NewLC
// Two-phased constructor. Leaves instance on the cleanup stack.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy* CHWRMPolicy::NewLC( const TDesC& aFilename )
    {
    CHWRMPolicy* self = new( ELeave ) CHWRMPolicy();
    
    CleanupStack::PushL( self );
    self->ConstructL( aFilename );

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::ConstructL
// By default Symbian 2nd phase constructor is private.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHWRMPolicy::ConstructL( const TDesC& aFilename )
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
    internalFilename.Append( KDrivename );            // "Z:"
    internalFilename.Append( filename );              // Private-folder of HWRM
    internalFilename.Append( KCustomerPolicyPrefix ); // "Customer" -prefix
    internalFilename.Append( aFilename );             // Actual filename
    filename.Insert(0, KDrivename);
    filename.Append( KProductPolicyPrefix ); // "Product" -prefix
    filename.Append( aFilename );
    
    // Parse the files and construct the array of policy clients
    // Add product specific file first if they need to change priorities of
    // customer defined processes.
    ParsePriorityFileL( fs, filename );
    ParsePriorityFileL( fs, internalFilename );
   
    // completed parsing the files; now add ALL client if one does not 
    // already exist (check is made in AddPolicyClientL method)
    AddPolicyClientL( KDefaultNormalPriority, KAllPolicySid, EFalse, KNullDesC8 );


    CleanupStack::PopAndDestroy(); // fs
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::GetPriority
// Method to retrieve the priority of a client with the given ID and if
// it is trusted or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHWRMPolicy::GetPriority( const TSecureId aSid, TBool& aTrusted ) const
    {
    TInt ret( KErrNotFound );
    
    // find the client
    TInt pos = FindClient( aSid );
    
    if( pos != KErrNotFound )
        {
        // if client exists return the priority
        ret = iClientArray[pos]->Priority();
        aTrusted = ETrue;
        }
    else
        {
        // client does not exist so return priority for all other clients
        ret = iAllPolicyClient->Priority();
        aTrusted = EFalse;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CHWRMPolicy::FeedbackClient
// Returns ETrue if requested sid is feedback client.
// -----------------------------------------------------------------------------
//
TBool CHWRMPolicy::FeedbackClient(TSecureId aSid) const
    {
    TBool isFeedback( EFalse );
    
    // find the client
    TInt pos = FindClient( aSid );
    
    if( pos != KErrNotFound )
        {
        isFeedback = iClientArray[pos]->FeedbackClient();
        }

    return isFeedback;
    }




// -----------------------------------------------------------------------------
// CHWRMPolicy::ParsePriorityFileL
// Method constructs the array of policy clients from the 
// given file 
// A valid line in the file contains priority, id and app name
// priority must be a valid integer from KLowestPriority to KHighestPriority.
// id must be a valid SID or ALL indicating the priority for all clients 
//      not specified in the file.
// app name can be any string indicating the name of the client.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHWRMPolicy::ParsePriorityFileL(  RFs& aFs, const TDesC& aFilename )
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
    TChar newLine(KNewLine);  
    RFileReadStream stream;    
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPolicy::ParsePriorityFileL - Opening policy file: %S" ), &aFilename);
    
    // Open the file and attach to stream 
    err = stream.Open(aFs, aFilename, EFileRead);
    
    // Return without error if file is not found
    if ( err != KErrNone )
        {
        COMPONENT_TRACE3( _L( "HWRM Server - CHWRMPolicy::ParsePriorityFileL - Policy file open failed: %S, error: %d" ), &aFilename, err);
        }
    else
        {
        CleanupClosePushL( stream );
       
        while( err != KErrEof )
            {
            // read from the file upto the newline character
            TRAP( err, stream.ReadL( fileline, newLine ) );

            lex.Assign( fileline );            
            //if( err != KErrEof && lex.Val( priority ) == KErrNone )
            if( lex.Val( priority ) == KErrNone )
                {
                // got the priority now validate it
                if( priority >= KHighestPriority && priority <= KLowestPriority )
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
                        if( appname.Size() == 0 && (feedback.Size() != 0 && !feedbackClient) )
                            {
                            appname.Set( feedback );
                            }
                        // Create the policy
                        AddPolicyClientL( priority, sid, feedbackClient, appname );
                        }
                    }
                }
            }

        // Close stream
        CleanupStack::PopAndDestroy();
    	}
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::ConvertId
// Helper method to convert and validate SID from a descriptor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHWRMPolicy::ConvertId( const TDesC8& aSidDes, TSecureId& aSid ) const
    {
    TInt ret(KErrNone);
    TUint32 id;

    if( aSidDes.Match( KMatchUid ) == 0 )
        {
        // this is a matching id
        TLex8 lex( aSidDes.Right(8) );
        
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
    else if( aSidDes.Match( KMatchAll ) == 0 )
        {
        // matching id for all remaining clients so no appname
        aSid.iId = KAllPolicySid;
        }
    else
        {
        ret = KErrCorrupt;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::AddPolicyClientL
// Adds a policy client to the array, first checks that no other 
// client has been registered with the identical sid.
// If one already exists KErrAlreadyExists is returned.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHWRMPolicy::AddPolicyClientL( const TInt aPriority, const TSecureId& aSid,
                                    const TBool aFeedbackClient, const TDesC8& aAppName )
    {
    TInt ret(KErrNone);
    
    // search for existing client
    if( aSid == KAllPolicySid && iAllPolicyClient == NULL )
        {
        // Default policy does not exist, store seperately for efficiency
        iAllPolicyClient = CPolicyClient::NewL( aPriority, aSid, aFeedbackClient, aAppName );
        }
    else if( FindClient( aSid ) == KErrNotFound )
        {
        // does not already exist so create and add to the array
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPolicy::AddPolicyClientL - adding client with sid=0x%x" ), aSid.iId);
        // create new client to be added to the array and put on cleanup stack
        CPolicyClient* ptrClient = 
                            CPolicyClient::NewLC( aPriority, aSid, aFeedbackClient, aAppName );
        // append the new client to the array
        User::LeaveIfError( iClientArray.Append( ptrClient ) );
        // array now owns the new client so pop it off the cleanup stack
        CleanupStack::Pop( ptrClient );
        }
    else
        {
        // client already exists so return KErrAlreadyExists
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPolicy::AddPolicyClientL - client already exists with sid=0x%x" ), aSid.iId);
        ret = KErrAlreadyExists;
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::FindClient
// Searches for client with the given sid.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHWRMPolicy::FindClient( const TSecureId& aSid ) const
    {
    for( TInt i = 0; i < iClientArray.Count(); i++ )
        {
        if( aSid == iClientArray[i]->Sid() )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPolicy::FindClient - client found with sid=0x%x" ), aSid.iId);
            return i;
            }
        }
    return KErrNotFound;
    }


// ============================ MEMBER FUNCTIONS ===============================
// ======================= Embedded CPolicyClient class ========================

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::CPolicyClient* 
CHWRMPolicy::CPolicyClient::NewL( const TInt aPriority,const TSecureId& aSid,
                                  const TBool aFeedbackClient, const TDesC8& aAppName)
    {
    CHWRMPolicy::CPolicyClient* self = NewLC( aPriority, aSid, aFeedbackClient, aAppName );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::NewLC
// Two-phased constructor. Leaves instance on the cleanup stack.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::CPolicyClient*
CHWRMPolicy::CPolicyClient::NewLC( const TInt aPriority, const TSecureId& aSid,
                                   const TBool aFeedbackClient, const TDesC8& aAppName)
    {
    CHWRMPolicy::CPolicyClient* self =
        new( ELeave ) CHWRMPolicy::CPolicyClient( aPriority, aSid, aFeedbackClient );
    
    CleanupStack::PushL( self );
    self->ConstructL( aAppName );

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::~CPolicyClient
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::CPolicyClient::~CPolicyClient()
    {
    delete iAppName;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::CPolicyClient
// C++ constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CHWRMPolicy::CPolicyClient::CPolicyClient( TInt aPriority, 
                                           const TSecureId& aSid,
                                           const TBool aFeedbackClient)
    : iPriority( aPriority ), iSid( aSid ), iFeedbackClient( aFeedbackClient )
    {
    }


// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::ConstructL
// By default Symbian 2nd phase constructor is private.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHWRMPolicy::CPolicyClient::ConstructL( const TDesC8& aAppName )
    {
    // create the descriptor for the appname
    iAppName = HBufC::NewL( aAppName.Length() );
    // and copy the 8 bit descriptor into it
    iAppName->Des().Copy(aAppName);
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::Priority
// Returns the clients priority.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHWRMPolicy::CPolicyClient::Priority() const
    {
    return iPriority;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::Id
// Returns the clients ID.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TSecureId CHWRMPolicy::CPolicyClient::Sid() const
    {
    return iSid;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::FeedbackClient
// Returns ETrue if feedback client.
// -----------------------------------------------------------------------------
//
TBool CHWRMPolicy::CPolicyClient::FeedbackClient() const
    {
    return iFeedbackClient;
    }

// -----------------------------------------------------------------------------
// CHWRMPolicy::CPolicyClient::AppName
// Returns the client application name.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CHWRMPolicy::CPolicyClient::AppName() const
    {
    return *iAppName;
    }


// End of File
