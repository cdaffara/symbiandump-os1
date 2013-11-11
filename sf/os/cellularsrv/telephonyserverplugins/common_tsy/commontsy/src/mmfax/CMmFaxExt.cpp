// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include "cmmfaxext.h"
#include "cmmphonetsy.h"
#include "cmmlinelist.h"
#include "cmmfaxcalltsy.h"
#include "cmmfaxlinetsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmcallgsmwcdmaext.h"


// ======== MEMBER FUNCTIONS ========

CMmFaxExt::CMmFaxExt(
    CMmCallTsy* aMmCall )   // call that owns this object
    : iMmCall( reinterpret_cast<CMmFaxCallTsy*>( aMmCall ) )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_CTOR_1, "TSY: CMmFaxExt::CMmFaxExt: Call Id:%d", iMmCall->CallId() );
    iFax           = NULL;
    iFaxSession    = NULL;
    iFaxCompletion = NULL;
    }

void CMmFaxExt::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_CONSTRUCTL_1, "TSY: CMmFaxExt::ConstructL");

    // Create CMmFaxCompletion class for Fax Server.
    iFaxCompletion = new (ELeave) CMmFaxCompletion();
    iDataPortLoaned = EFalse;
    }

CMmFaxExt* CMmFaxExt::NewL(
        CMmCallTsy* aMmCall )  // The call object which owns CMmFaxExt  
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_NEWL_1, "TSY: CMmFaxExt::NewL");

    CMmFaxExt* self = new ( ELeave ) CMmFaxExt( aMmCall );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CMmFaxExt::~CMmFaxExt()
    {
    //This must execute here, if status is Idle (Fax Client API) on HangUp,
    //this will never be executed. so we do it here. It is ok to call
    //cleanup multiple times.
    CleanUpFaxServer();

    if ( iFax )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DTOR_1, "TSY: CMmFaxExt::~CMmFaxExt: deleting iFax");
        delete iFax;
        iFax = NULL;
        }

    if (iFaxCompletion)
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DTOR_2, "TSY: CMmFaxExt::~CMmFaxExt: deleting iFaxCompletion");
        delete iFaxCompletion;
        iFaxCompletion = NULL;
        }

    // mark fax as unopened, so that it can be opened again.
	if( iMmCall )
		{
		(reinterpret_cast<CMmFaxLineTsy*>( iMmCall->Line() ))->
			iFaxOpened = EFalse;
		}

    iMmCall = NULL;
    iFaxSession = NULL;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::CompleteOperation
// Completes ongoing read/write operation on CMmFaxCompletion object.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxExt::CompleteOperation(
        TInt aError ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_COMPLETEOPERATION_1, "TSY: CMmFaxExt::CompleteOperation");
     
    if ( iFaxCompletion )
        {
        iFaxCompletion->CompleteOperation( aError );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmFaxExt::ConfigureCompletion
// Set fax request handle to CMmFaxCompletion class object.
// Returns: None
// ---------------------------------------------------------------------------
//
void CMmFaxExt::ConfigureCompletion(
        const TTsyReqHandle aTsyReqHandle,  
        CTelObject* aTelObject )            
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_CONFIGURECOMPLETION_1, "TSY: CMmFaxExt::Configure");

    iFaxCompletion->Configure( aTsyReqHandle, aTelObject );
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::CreateFaxObject
// Creates CMmTsyFax object and returns it.
// Returns: Pointer to created object
// ---------------------------------------------------------------------------
//
CTelObject* CMmFaxExt::OpenNewObjectByNameL(
        const TDesC& aName )  
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_OPENNEWOBJECTBYNAMEL_1, "TSY: CMmFaxExt::OpenNewObjectByNameL");

    // check the name of opened object..
    _LIT(KFaxObjectName, "FAX");
    if ( aName != KFaxObjectName )
        {
        User::Leave(KErrNotSupported);
        }

    //  We only want one fax object to be opened per phone.
    //  Previously only the connected call could open a fax object, so it was
    //  easy to check whether one had already been opened. Now a fax call can
    //  open a fax object at any time making it less clear how to check that
    //  no other call has opened one.
    RPhone::TLineInfo lineInfo = iMmCall->Line()->LineInfo();
    TBool& faxOpened = (reinterpret_cast<CMmFaxLineTsy*>(
        iMmCall->Owner() ))->iFaxOpened;
    if ( lineInfo.iStatus != GetCallStatus() )
        { 
		// another fax call is in progress so this call cannot open a fax object
        User::Leave( KErrEtelNotCallOwner );
        }

    if ( faxOpened )
        {
        // Another call on this line has already opened fax.
        User::Leave( KErrAlreadyExists );
        }


    // create new fax object..
    iFax = CMmTsyFax::NewL( this );

    // set faxopened flag of the line object
    faxOpened = ETrue;

    return iFax;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::FaxConnectHandler
// Checks if some another uses fax line.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmFaxExt::FaxConnectHandler()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_FAXCONNECTHANDLER_1, "TSY: CMmFaxExt::FaxConnectHandler");

    TInt errorCode( KErrNone );

    if ( (reinterpret_cast<CMmFaxLineTsy*>( iMmCall->Line() ))->iFaxOpened 
          && NULL == iFax )
        {
        iMmCall->SetUnowned();
        errorCode = KErrEtelNotFaxOwner;
        }
    else
        {
        TRAP( errorCode, GetFaxBaseL() );
        if ( KErrNone != errorCode )
            {
            iMmCall->SetUnowned();
            }
        }
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::AnswerIncomingCall
// Starts waiting for incoming fax call
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmFaxExt::AnswerIncomingCall(
        const TTsyReqHandle& aTsyReqHandle ) // function identification handle
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_ANSWERINCOMINGCALL_1, "TSY: CMmFaxExt::AnswerIncomingCall");
    
    TInt ret( KErrGeneral );

    // check and set call ownership..
    MCallBaseTSY::TCallOwnership ownerShip = iMmCall->CheckOwnership( 
      aTsyReqHandle );
    if ( ownerShip == MCallBaseTSY::EOwnedUnowned 
         || ownerShip == MCallBaseTSY::EOwnedPriorityClient ) 
        {
        iMmCall->SetOwnership( aTsyReqHandle );

        // open faxserver library and faxserver session (iFaxSession)
        ret = FaxConnectHandler(); 
        if ( KErrNone == ret )
            {
            ret = OpenFaxServer(NULL, EWaitForRingAndReceive);
            if ( KErrNone == ret )
                {
                ConfigureCompletion(aTsyReqHandle, iMmCall);
                iFaxSession->RxConnect();
                }
            }

        }
    else // ownership test failed
        {
        ret = KErrEtelNotCallOwner;
        }

    // We don't complete erroneous request here since
    // CTsyCall::AnswerIncomingCall (calling function) does it for us..
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmFaxExt::Dial
// Start fax call connection.
// Returns: Error value
// ---------------------------------------------------------------------------
//
TInt CMmFaxExt::Dial(
        const TTsyReqHandle& aTsyReqHandle, 
        TDesC* aTelNumber )      
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DIAL_1, "TSY: CMmFaxExt::Dial");
    
    TInt ret( KErrGeneral );

    // check and set call ownership..
    MCallBaseTSY::TCallOwnership ownerShip = iMmCall->CheckOwnership( 
      aTsyReqHandle );
    if( ownerShip == MCallBaseTSY::EOwnedUnowned
         || ownerShip == MCallBaseTSY::EOwnedPriorityClient ) 
        {
        iMmCall->SetOwnership( aTsyReqHandle );

        ret = FaxConnectHandler();
        if( KErrNone == ret )
            {
            TFaxMode faxMode;
            if( RCall::ETransmit == iFaxSettings.iMode )
                {
                faxMode = EDialAndTransmit;
                }
            // we're receiving
            else    
                {
                if( RCall::EFaxPoll  == iFaxSettings.iFaxRetrieveType )
                    {
                    faxMode = EDialAndReceivePoll;
                    }
                else 
                    {
                    faxMode = EDialAndReceiveFaxBack; 
                    }
                }

            ret = OpenFaxServer( aTelNumber, faxMode );
            if( KErrNone == ret )
                {
                ConfigureCompletion( aTsyReqHandle, iMmCall );
                if( EDialAndTransmit == faxMode )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DIAL_2, "TSY: CMmFaxExt::FaxDial:TxConnect");
                    iFaxSession->TxConnect();
                    }
                else
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DIAL_3, "TSY: CMmFaxExt::FaxDial:RxConnect");
                    iFaxSession->RxConnect();
                    }
                }
            } // faxconnect
        }
    else 
        {
        // ownership test failed
        ret = KErrEtelNotCallOwner;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::HangUp
// Hangup fax call.
// Returns:None
// ---------------------------------------------------------------------------
//
void CMmFaxExt::HangUp()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_HANGUP_1, "TSY: CMmFaxExt::HangUp");
    CleanUpFaxServer();
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::FaxSession
// Returns Fax session pointer.
// Returns:Pointer to Fax session 
// ---------------------------------------------------------------------------
//
CFaxSession* CMmFaxExt::GetFaxSession()
    {
    return iFaxSession;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::GetFaxBaseL
// Loads fax server.
// Returns:None 
// ---------------------------------------------------------------------------
//
void CMmFaxExt::GetFaxBaseL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_GETFAXBASEL_1, "TSY: CMmFaxExt::GetFaxBaseL");

    TInt errorCode = iFaxServerLib.Load( KFaxServerName );

    if ( KErrNone == errorCode )
        {
#if defined (_UNICODE)
            if( iFaxServerLib.Type()[1] != TUid::Uid( 
                  KUidUnicodeDynamicFaxServer ) )
                {
                errorCode = KErrBadLibraryEntryPoint;
                }
#else
            if( iFaxServerLib.Type()[1] != TUid::Uid( KUidDynamicFaxServer ) )
                {
                errorCode = KErrBadLibraryEntryPoint;
                }
#endif
        
        if ( KErrNone == errorCode )
            {   
            TFaxServerEntry libEntry = reinterpret_cast<TFaxServerEntry>(
                iFaxServerLib.Lookup( 1 ) );
            if ( libEntry != NULL )
                {
                // call CFaxSession::NewL(). LibEntry may leave.
                TRAP( errorCode, iFaxSession = (*libEntry)(); );  
                if ( KErrNone == errorCode )
                    {
                    // fax server session has been created,
                    // Now set completion callback, it's methods called by
                    // fax server when something significant happens
                    iFaxSession->SetCallBack( iFaxCompletion );
                    }
                else
                    {
                    // failed to create fax server session
                    iFaxServerLib.Close();
                    }
                }
            else
                {
                // entry point was not found
                errorCode = KErrBadLibraryEntryPoint;
                iFaxServerLib.Close();
                }
            }
        else
          {
            // Uid type test failed
            iFaxServerLib.Close();
          }
        }
    User::LeaveIfError( errorCode );
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::OpenFaxServer
// Open CFaxSession object with desired settings.
// Returns: Error Code 
// ---------------------------------------------------------------------------
//
TInt CMmFaxExt::OpenFaxServer(
        const TDesC* aTelNumber,  // phone number
        const TFaxMode aFaxMode ) // fax mode
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_OPENFAXSERVER_1, "TSY: CMmFaxExt::OpenFaxServer");

    TInt errorCode ( KErrNone );
    TFaxServerSessionSettings faxSettings;
    if ( aTelNumber )
        {
        faxSettings.iPhoneNumber.Copy( *aTelNumber );
        }
    else
        {
        faxSettings.iPhoneNumber.Zero();
        }

    // Dataport names
    _LIT( KDataPortPort, "DATAPORT::0" );
    _LIT( KDataPortName, "DATAPORT");

    faxSettings.iLogging          = ETrue;
    faxSettings.iMode             = aFaxMode;
    faxSettings.iFaxClass         = iFaxSettings.iFaxClass;
    faxSettings.iPortDriverName   = KDataPortName;
    faxSettings.iCommPortName     = KDataPortPort;
    faxSettings.iFaxId            = iFaxSettings.iFaxId;
    faxSettings.iMaxSpeed         = iFaxSettings.iMaxSpeed;
    faxSettings.iMinSpeed         = iFaxSettings.iMinSpeed;
    faxSettings.iPreferredECM     = iFaxSettings.iPreferredECM;
    faxSettings.iFaxOnDemandDelay = iFaxSettings.iFaxOnDemandDelay;
    faxSettings.iTxResolution     = iFaxSettings.iTxResolution;
    faxSettings.iTxCompression    = iFaxSettings.iTxCompression;
    faxSettings.iTxPages          = iFaxSettings.iTxPages;
    faxSettings.iRxResolution     = iFaxSettings.iRxResolution;
    faxSettings.iRxCompression    = iFaxSettings.iRxCompression;
    RFax::TProgress* faxProgress  = iMmCall->CreateFaxProgressChunk();

    if ( NULL == faxProgress )
        {
        errorCode = KErrEtelFaxChunkNotCreated;
        }
    else
        {
        // allocate dataport for fax...
        RCall::TCommPort commPort;
        commPort.iCsy.Copy( KDataPortName );
        commPort.iPort.Copy( KDataPortPort );

        //Create package
        CCallDataPackage package;
        //Set call id and call mode
        package.SetCallIdAndMode(
            iMmCall->CallId(), RMobilePhone::ECircuitDataService );
        //Pack commport
        package.PackData( &commPort );

        //Send request to the Domestic OS layer.
        TInt trapError( KErrNone );
        TRAP( trapError,
            errorCode = iMmCall->Phone()->MessageManager()->
                HandleRequestL( EEtelCallLoanDataPort, &package );
            );

        if ( KErrNone != trapError )
            {
            //error handling, leaved.
            errorCode = trapError;
            }

        // if dataport is allocated, open fax server
        if (errorCode == KErrNone)
            {
            errorCode = iFaxSession->FxOpen( faxSettings, faxProgress );

            if (errorCode != KErrNone)
                {
                // error, free dataport
                //Send request to the Domestic OS layer.
                trapError = KErrNone;
                TRAP( trapError,
                    errorCode = iMmCall->Phone()->MessageManager()->
                        HandleRequestL( EEtelCallRecoverDataPort, &package );
                    );

                if ( KErrNone != trapError )
                    {
                    //error handling, leaved.
                    errorCode = trapError;
                    }
                }
            else
                {
                iDataPortLoaned = ETrue;
                }
            }
        }
    
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::CleanUpFaxServer
// Closes Fax Server, deletes fax progress chunk and closes Fax Server library
// Returns: None 
// ---------------------------------------------------------------------------
//
void CMmFaxExt::CleanUpFaxServer()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_CLEANUPFAXSERVER_1, "TSY: CMmFaxExt::CleanUpFaxServer");

    if ( iFaxSession )
        {
        iFaxSession->FxClose();
        iMmCall->DeleteFaxProgressChunk();
        iFaxServerLib.Close();

        if ( iDataPortLoaned )
            {
            // free dataport
            _LIT( KDataPortPort, "DATAPORT::0" );
            _LIT( KDataPortName, "DATAPORT");

            RCall::TCommPort commPort;
            commPort.iCsy.Copy( KDataPortName );
            commPort.iPort.Copy( KDataPortPort );

            //Create package
            CCallDataPackage package;
            //Set call id and call mode
            package.SetCallIdAndMode(
                iMmCall->CallId(), RMobilePhone::ECircuitDataService );
            //Pack commport
            package.PackData( &commPort );

            //Send request to the Domestic OS layer.
            TRAPD( trapError,
                iMmCall->Phone()->MessageManager()->
                    HandleRequestL( EEtelCallRecoverDataPort, &package );
                );
            }

        iFaxSession = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// CMmFaxExt::GetFaxSettings
// Get fax settings from CMmFaxExt class object's member variable.
// Returns: Error Value 
// ---------------------------------------------------------------------------
//
 TInt CMmFaxExt::GetFaxSettings(
        RCall::TFaxSessionSettings* aSettings ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_GETFAXSETTINGS_1, "TSY: CMmFaxExt::GetFaxSettings");
    *aSettings = iFaxSettings;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::SetFaxSettings
// Set fax settings to CMmFaxExt class object's member variable.
// Returns: Error Value 
// ---------------------------------------------------------------------------
//
 TInt CMmFaxExt::SetFaxSettings(
        const RCall::TFaxSessionSettings* aSettings ) // in: fax settings.
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_SETFAXSETTINGS_1, "TSY: CMmFaxExt::SetFaxSettings");
        iFaxSettings = *aSettings;
        return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::GetCallStatus
// Returns call status from CMmCallTsy object.
// Returns: Call status 
// ---------------------------------------------------------------------------
//
RCall::TStatus CMmFaxExt::GetCallStatus() const
    {
    return iMmCall->Status();
    }

// ---------------------------------------------------------------------------
// CMmFaxExt::GetCallStatus
// Sets terminate flag true on CMmTsyFax object.
// Returns:  None 
// ---------------------------------------------------------------------------
//
void CMmFaxExt::DialCancel()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXEXT_DIALCANCEL_1, "TSY: CMmFaxExt::DialCancel");
    if ( iFax ) 
        {
        iFax->Terminate();
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =======================

//  End of File 
