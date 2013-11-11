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
#include "HWRMServer.h"
#include "HWRMService.h"
#include "HWRMFmTxService.h"
#include "HWRMPluginHandler.h"
#include "HWRMReservationHandler.h"
#include "HWRMFmTxCommands.h"
#include "HWRMFmTxClientServer.h"
#include "HWRMFmTxRdsTextConverter.h"
#include "HWRMtrace.h"

// CONSTANTS
_LIT( KPanicCategory, "HWRMFmTxService" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxService::CHWRMFmTxService
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxService::CHWRMFmTxService(CHWRMPluginHandler& aWatcherPluginHandler,
                                   CHWRMFmTxCommonData& aFmTxCommonData, 
                                   CHWRMFmTxRdsTextConverter& aRdsTextConverter)
    : iWatcherPluginHandler(aWatcherPluginHandler),
      iFmTxCommonData(aFmTxCommonData), 
      iRdsTextConverter(aRdsTextConverter)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::CHWRMFmTxService()" ));
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::CHWRMFmTxService - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                  CHWRMReservationHandler* aReservationHandler)
    {
    COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxService::ConstructL(0x%x, 0x%x)" ), aPluginHandler, aReservationHandler);

    __ASSERT_ALWAYS(aPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    __ASSERT_ALWAYS(aReservationHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    
    BaseConstructL(aPluginHandler, aReservationHandler);
    iWatcherTransactionList = CHWRMPluginTransactionList::NewL();
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmTxService* CHWRMFmTxService::NewL(CHWRMPluginHandler* aPluginHandler,
										 CHWRMPluginHandler& aWatcherPluginHandler,
                                         CHWRMReservationHandler* aReservationHandler,
                                         CHWRMFmTxCommonData& aFmTxCommonData,
                                         CHWRMFmTxRdsTextConverter& aRdsTextConverter)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::NewL(0x%x)" ), aPluginHandler);

    CHWRMFmTxService* self = new( ELeave ) 
    				CHWRMFmTxService(aWatcherPluginHandler, aFmTxCommonData, aRdsTextConverter);

    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::NewL - return 0x%x" ), self );

    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxService::~CHWRMFmTxService()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::~CHWRMFmTxService()" ) );

    DoRelease();

    // Base class cleanup
    BaseDestruct();
    // Complete any pending watchertransaction requests
    while ( iWatcherTransactionList->GetFirstItem() )
        {
        THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>( 
                                            iWatcherTransactionList->RemoveFirstItem() );
        if (data->iRequestMessage.Handle())
            {
            COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMService::~CHWRMFmTxService() - Canceling pending message" ) ) );

            data->iRequestMessage.Complete(KErrCancel);
            }

        TRAPD(err, iWatcherPluginHandler.CancelCommandL(data->iTransId));

        if ( err != KErrNone )
            {
            COMPONENT_TRACE3( _L( "HWRM Server - CHWRMService::~CHWRMFmTxService - Cancelling Command (transid: %d) failed: %d" ), data->iTransId, err );
            }

        delete data;
        }
    // Destroy watchertransaction list
    delete iWatcherTransactionList;

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::~CHWRMFmTxService - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecuteMessageL
// Handles FM Tx requests.  Called from the client session via CSession2::ServiceL
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxService::ExecuteMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ExecuteMessageL(0x%x)" ), aMessage.Function());
    
    if ( aMessage.IsNull() )
        {
        User::Leave(KErrBadHandle);
        }

    // All messages are by default asynchronous because of the nature of the plugin API
    TBool completeMessage(EFalse);

    switch ( aMessage.Function() )
        {
        case EHWRMFmTxCleanup:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxCleanup") );
            DoRelease();        
            completeMessage = ETrue;
            }
            break;

        case EHWRMFmTxOn:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxOn") );
            EnableL( aMessage );
            }
            break;

        case EHWRMFmTxOff:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxOff") );
            completeMessage = DisableL( aMessage );
            }
            break;

        case EHWRMFmTxGetFreqRange:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxGetFreqRange") );
            GetFrequencyRangeL( aMessage );
                         
            }
            break;

        case EHWRMFmTxSetFreq:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetFreq") );              
            completeMessage = SetFrequencyL( aMessage );
            }
            break;

        case EHWRMFmTxReserve:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxReserve") );

            // If returned mask is non-zero, reservation was done in suspended mode.
            iSuspended = (iReservationHandler->ReserveL(aMessage, aMessage.Int1(), this, KHWRMAllSubResources));
            iReserved = ETrue;

            completeMessage = ETrue; // Complete needed, as dummy used.             
            }
            break;

        case EHWRMFmTxRelease:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxRelease") );

            DoRelease();

            completeMessage = ETrue; // Complete needed, as dummy used.             
            }
            break;          

        case EHWRMFmTxGetNextClearFreq:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxGetNextClearFreq") );
            
            GetNextClearFrequencyL( aMessage,
                                    1/*1 clear frequency required*/ );
            }
            break;      
            
        case EHWRMFmTxGetNextClearFreqArray:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxGetNextClearFreqArray") );
            
            TPckgBuf<TUint> clientClearFreqCountPckg;
            aMessage.ReadL(1,clientClearFreqCountPckg);
            
            GetNextClearFrequencyL( aMessage,
                                    clientClearFreqCountPckg() /*number of clear frequencies required*/ );
            }
            break;      
            

        case EHWRMFmTxCancelGetNextClearFreq:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxCancelGetNextClearFreq") );

            CancelPluginCommandL( HWRMFmTxCommand::ETxScanRequestCmdId );
            completeMessage = ETrue; // no responses from plugin cancel commands
            }
            break;

        case EHWRMFmTxSetNextClearFreq:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetNextClearFreq") );

            SetNextClearFrequencyL( aMessage,
                                    1/*1 clear frequency required*/ );
            }
            break;  
        
        case EHWRMFmTxCancelSetNextClearFreq:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxCancelSetNextClearFreq") );

            // no hope of cancelling the ESetTxFrequencyCmdId part, so try the ETxScanRequestCmdId
            CancelPluginCommandL( HWRMFmTxCommand::ETxScanRequestCmdId );
            completeMessage = ETrue; // no responses from plugin cancel commands
            }
            break;        

        case EHWRMFmTxSetRdsPty:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRdsPty") );
            SetRdsPtyL( aMessage );
            }
            break;

        case EHWRMFmTxSetRdsPtyn:        
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRdsPtyn") );
            SetRdsPtynL( aMessage );
            }
            break;

        case EHWRMFmTxSetRdsMs:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRdsMs") );
            SetRdsMsL( aMessage );
            }
            break;
            
        case EHWRMFmTxSetRdsLanguageId:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRdsLanguageId") );
            SetRdsLanguageIdL( aMessage );
            }
            break;

        case EHWRMFmTxSetRadiotext:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRadiotext") );
            SetRadiotextL( aMessage );
            }
            break;

        case EHWRMFmTxSetRadiotextPlus1:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRadiotext") );             
            TRtPlusTag tag; // create a RT tag
            tag.iContentType  = (TRtPlusContentType)aMessage.Int1();
            tag.iStartMarker  = 0;
            tag.iLengthMarker = aMessage.GetDesLengthL(0);  
            SetRadiotextL( aMessage, tag );
            }
            break;

        case EHWRMFmTxSetRadiotextPlus2:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRadiotext") );
            // read RT tag from client
            THWRMRtPlusTagCmdPackage tagPckg;
            if ( aMessage.Read(1, tagPckg) != KErrNone )
                {
                User::Leave(KErrArgument);
                }
            TRtPlusTag tag = tagPckg();
            SetRadiotextL( aMessage, tag );  
            }
            break;

        case EHWRMFmTxSetRadiotextPlus3:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxSetRadiotext") );
            // read RT tags from client
            THWRMRtPlusTagCmdPackage tagPckg1, tagPckg2;
            if ( aMessage.Read(1, tagPckg1) != KErrNone ||
                 aMessage.Read(2, tagPckg2) != KErrNone )
                {
                User::Leave(KErrArgument);
                }
            TRtPlusTag tag1 = tagPckg1();
            TRtPlusTag tag2 = tagPckg2();
            SetRadiotextL( aMessage, tag1, tag2 );
            }
            break;

        case EHWRMFmTxClearRdsData:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxClearRdsData") );
            ClearRdsDataL( aMessage );
            }
            break;
            
        case EHWRMFmTxClearRtData:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecuteMessageL - EHWRMFmTxClearRtData") );
            ClearRadiotextDataL( aMessage );
            }
            break;

        // Not currently supported
        case EHWRMFmTxAutoFreqOn:
        case EHWRMFmTxAutoFreqOff:
            {
            User::Leave(KErrNotSupported);
            }
            break;

        // Cannot identify the message, panic the client
        default:
            {
            CHWRMServer::PanicClient(aMessage, EPanicIllegalFunction);
            }
            break;
        }

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ExecuteMessageL - return 0x%x" ), completeMessage );

    return completeMessage;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::EnableL
// Sends a plug-in command to switch FM Tx ON
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::EnableL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::EnableL(0x%x)" ), aMessage.Int0() );

    LeaveIfOtherReservationL();

    // We cannot enable from "power save" state
    if ( iFmTxCommonData.IsAccessoryPowerSaveOn() )
        {
		// We cannot enable during "power save" due to mutually exclusive accessory connection.			
        COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::EnableL - not ready in \"power save\" state" ) );
        User::Leave(KErrNotReady);
        }
    else if ( iFmTxCommonData.IsFmTxHardwareOn() )
        {
        // If already On, then send the new frequency instead
        COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxService::EnableL - HW already on" ) ) ); 
        HWRMFmTxCommand::TSetFrequencyPackage pckg( aMessage.Int0() );
        ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxFrequencyCmdId, EFalse/*not split*/, pckg, aMessage.Int0() );
        }
    else
        {
        //start observing       
        ExecutePluginCommandL(aMessage, HWRMFmTxCommand::EStartObservingCmdId, EFalse/*not split*/ );
        iFmTxCommonData.UpdateFrequency( aMessage.Int0() );
        }

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::EnableL - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::DisableL
// Sends a plug-in command to switch FM Tx OFF
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxService::DisableL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::DisableL" ) );

    LeaveIfOtherReservationL();

    // We don't need to disable HW from some states
	if ( !iFmTxCommonData.IsFmTxHardwareOn() )
        {
        COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::DisableL - HW already off due to power-save mode" ) );
        iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransRequestedOff);
        return ETrue;
        }

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ETxOffCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::DisableL - return" ) );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::GetFrequencyRangeL
// Sends a plug-in command to query the FM Tx frequency range
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::GetFrequencyRangeL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::GetFrequencyRangeL" ) );

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::EGetTxFrequencyRangeCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::GetFrequencyRangeL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetFrequencyL
// Sends a plug-in command to set the FM Tx frequency
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxService::SetFrequencyL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetFrequencyL(0x%x)" ), aMessage.Int0() );

    LeaveIfOtherReservationL();
    TInt retVal = EFalse;

    // Check Tx status
    if ( iFmTxCommonData.IsFmTxHardwareOn() )
        {
        // Package & send the request
        HWRMFmTxCommand::TSetFrequencyPackage pckg( aMessage.Int0() );
        ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxFrequencyCmdId, EFalse/*not split*/, pckg, aMessage.Int0() );
        retVal = EFalse; // don't complete message yet
        }
    else
        {
        // Just save the frequency for future use
        iFmTxCommonData.UpdateFrequency( aMessage.Int0() );
        retVal = ETrue; // complete message now
        }

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetFrequencyL - return %d" ), retVal );
    return retVal;
    }        

// -----------------------------------------------------------------------------
// CHWRMFmTxService::GetNextClearFrequencyL
// Sends a plug-in command to scan for an available (clear) frequency
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::GetNextClearFrequencyL(const RMessage2& aMessage,
                                              TUint aClearFreqsRequired)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::GetNextClearFrequencyL(%d)" ), aClearFreqsRequired );

    LeaveIfOtherReservationL();

    LeaveIfTransmitterOffL();

    // package up the request
    HWRMFmTxCommand::TScanRequestPackage pckg(aClearFreqsRequired);

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ETxScanRequestCmdId, EFalse/*not split*/, pckg );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::GetClearFrequencyL - return" ) ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetNextClearFrequencyL
// Sends the 1st part of a split command, to scan for an available (clear) frequency
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetNextClearFrequencyL(const RMessage2& aMessage,
                                              TUint  aClearFreqsRequired)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetNextClearFrequencyL(%d)" ), aClearFreqsRequired );

    LeaveIfOtherReservationL();

    LeaveIfTransmitterOffL();

    // package up the request
    HWRMFmTxCommand::TScanRequestPackage pckg(aClearFreqsRequired);

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ETxScanRequestCmdId, ETrue/*split*/, pckg );
    // wait until response before calling SetFrequencyL

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetNextClearFrequencyL - return" ) ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRdsPtyL
// Sends a plug-in command to set the Programme TYpe
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRdsPtyL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtyL(0x%x)" ), aMessage.Int0() );

    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    // package up the request    
    HWRMFmTxCommand::TRdsPtyPackage pckg( aMessage.Int0() );
         
    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxRdsPtyCmdId, EFalse/*not split*/, pckg );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtyL - return" )  );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRdsPtynL
// Sends a plug-in command to set the Programme TYpe Name
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRdsPtynL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtynL") );
    
    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    // Check input param
    TRdsProgrammeTypeName clientPtyn;
    if ( aMessage.Read(0,clientPtyn) != KErrNone )
        {
        COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtynL") );
        User::Leave(KErrArgument);
        }
    
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtynL new PTYN = %S"), &clientPtyn );
    
    // package up the request
    HWRMFmTxCommand::TRdsPtyn newPtyn;
    iRdsTextConverter.ConvertFromUnicodeL( clientPtyn, newPtyn ); // Insert the new PTYN
    HWRMFmTxCommand::TRdsPtynPackage pckg(newPtyn);

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxRdsPtynCmdId, EFalse/*not split*/, pckg );   
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsPtynL - return") );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRdsMsL
// Sends a plug-in command to set the Music/Speech flag
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRdsMsL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsMsL") );
    
    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    // package up the request
    HWRMFmTxCommand::TRdsMsPackage pckg( aMessage.Int0() );
    
    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxRdsMsCmdId, EFalse/*not split*/, pckg );         
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsMsL - return") );       
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRdsLanguageIdL
// Sends a plug-in command to set the RDS language ID
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRdsLanguageIdL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsLanguageIdL") );
    
    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    // package up the request
    HWRMFmTxCommand::TRdsLangIdPackage pckg( aMessage.Int0() );
         
    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxRdsLangIdCmdId, EFalse/*not split*/, pckg );         
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRdsLanguageIdL - return") );       
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ClearRdsDataL
// Sends a plug-in command, to set the RDS data back to default values
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ClearRdsDataL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ClearRdsDataL") );
    
    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    // package up the requests
    HWRMFmTxCommand::TRdsPtyn newPtyn;
    newPtyn.Zero();
    HWRMFmTxCommand::TRdsPtyPackage    ptyPckg( KRdsPtyNone );    
    HWRMFmTxCommand::TRdsPtynPackage   ptynPckg( newPtyn );
    HWRMFmTxCommand::TRdsMsPackage     msPckg( ETrue ); // defaults to Music
    HWRMFmTxCommand::TRdsLangIdPackage langIdPckg( KRdsLanguageUnknown );
    
    // Clear RDS values
    ExecutePluginCommandL( aMessage, HWRMFmTxCommand::ESetTxRdsPtyCmdId,    ETrue/*split*/, ptyPckg );
    ExecutePluginCommandL( aMessage, HWRMFmTxCommand::ESetTxRdsPtynCmdId,   ETrue/*split*/, ptynPckg );
    ExecutePluginCommandL( aMessage, HWRMFmTxCommand::ESetTxRdsMsCmdId,     ETrue/*split*/, msPckg );
    ExecutePluginCommandL( aMessage, HWRMFmTxCommand::ESetTxRdsLangIdCmdId, ETrue/*split*/, langIdPckg );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ClearRdsDataL - return") );       
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ClearRadiotextDataL
// Sends a plug-in command, to clear the RT data
// -----------------------------------------------------------------------------
//  
void CHWRMFmTxService::ClearRadiotextDataL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ClearRadiotextDataL") );
    
    LeaveIfOtherReservationL();
    
    LeaveIfTransmitterOffL();
        
    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::EClearTxRtCmdId, EFalse/*not split*/ );            
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ClearRadiotextDataL - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRadiotextL
// Sends a plug-in command, to set the RT data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRadiotextL(const RMessage2& aMessage)
    {
    TRtPlusTag dummyTag1, dummyTag2; // create some dummy tags
    dummyTag1.iContentType  = dummyTag2.iContentType  = ERtPlusInfoOther;
    dummyTag1.iStartMarker  = dummyTag2.iStartMarker  = 0;
    dummyTag1.iLengthMarker = dummyTag2.iLengthMarker = 0;  

    SetRadiotextL( aMessage, dummyTag1, dummyTag2 );    
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRadiotextL
// Sends a plug-in command, to set the RT data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRadiotextL(const RMessage2& aMessage, TRtPlusTag aTag)
    {
    TRtPlusTag dummyTag; // create a dummy tag
    dummyTag.iContentType  = ERtPlusInfoOther;
    dummyTag.iStartMarker  = 0;
    dummyTag.iLengthMarker = 0; 
    
    SetRadiotextL( aMessage, aTag, dummyTag );  
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::SetRadiotextL
// Sends a plug-in command, to set the RT data
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SetRadiotextL(const RMessage2& aMessage, TRtPlusTag aTag1, TRtPlusTag aTag2)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRadiotextL") );

    LeaveIfOtherReservationL();     
    
    LeaveIfTransmitterOffL();

    LeaveIfTagInvalidL(aTag1);
    LeaveIfTagInvalidL(aTag2);

    // Check input descriptor
    TBuf<HWRMFmTxCommand::KMaxRtDataLength>  clientRtData;
    if ( aMessage.GetDesLengthL(0) <  HWRMFmTxCommand::KMaxRtDataLength )
        {
        if ( aMessage.Read(0,clientRtData) != KErrNone )
            {
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SetRadiotextL, read error") );
            User::Leave(KErrArgument);              
            }
        }   
    else
        {
        COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::SetRadiotextL, length error %d"), aMessage.GetDesLengthL(0) );
        User::Leave(KErrArgument);
        }       

    // package up the request
    HWRMFmTxCommand::TRtData newRtData;
    iRdsTextConverter.ConvertFromUnicodeL( clientRtData, newRtData.iText ); // Convert to 8bit RDS format
    newRtData.iTag1.iContentType  = aTag1.iContentType;
    newRtData.iTag1.iStartMarker  = aTag1.iStartMarker;
    newRtData.iTag1.iLengthMarker = aTag1.iLengthMarker;
    newRtData.iTag2.iContentType  = aTag2.iContentType;
    newRtData.iTag2.iStartMarker  = aTag2.iStartMarker;
    newRtData.iTag2.iLengthMarker = aTag2.iLengthMarker;
    HWRMFmTxCommand::TRtPackage pckg(newRtData);

    ExecutePluginCommandL(aMessage, HWRMFmTxCommand::ESetTxRtCmdId, EFalse/*not split*/, pckg );            
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ProcessResponseL
// Handles responses to FM Tx requests.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout )
    {
    COMPONENT_TRACE4( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL(%d , %d, <data>, %d)" ), aCommandId, aTransId, aTimeout );

    TInt pluginErr(KErrNone);  // Error came from plugin as data (or timeout). Used to complete RMessage.
    TInt contextErr(KErrNone); // Error in context, i.e. bad handle or descriptor. Used to leave.
    TBool watcherMsg( EFalse );
    TBool completeWatcherMsg( EFalse );

    if ( aTimeout)
        {
        pluginErr = KErrTimedOut;            
        }
    else if ( CheckResponseDataSize(aCommandId, aData) == EFalse )
        {
        pluginErr = KErrBadDescriptor;
        }
    else 
        {
        // Handle the response                      
        switch ( aCommandId )
            {
            case HWRMFmTxCommand::EStartObservingCmdId:
                {
                HWRMFmTxCommand::TConnStatusPackage pckg;
                pckg.Copy( aData );
                HWRMFmTxCommand::TConnStatusData data( pckg() );
                watcherMsg = ETrue;
                
                if( data.iConnected )
                    {
                    completeWatcherMsg = ETrue;
                    pluginErr = KErrNotReady;
                    // Update common data power save state
                    iFmTxCommonData.UpdatePowerSaveState( 
                                            CHWRMFmTxCommonData::EHWRMFmTxPowerSaveAccessory );
                    // Store the new status
                    iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransOff);
                    }
                else
                    {
                    // Normal case - send the On request                    
                    HWRMFmTxCommand::TSetFrequencyPackage pckg( iFmTxCommonData.Frequency() );
                    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>( 
                                           iWatcherTransactionList->FindTransaction( aTransId,
                                                                                     EFalse ) );
                    if(data)
                    	{
                    	TRAP( pluginErr, ExecutePluginCommandL( data->iRequestMessage,
                    			HWRMFmTxCommand::ETxOnCmdId,
                    			EFalse/*not split*/,
                    			pckg ) );
                    	}
                    else
                    	{
                    	pluginErr = KErrNotFound; // Data is null
                    	}
                    
                    }
                }
                break;              
            
            case HWRMFmTxCommand::ETxOnCmdId:
                {
                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ETxOnCmdId pluginErr = %d" ), pluginErr );

                if ( pluginErr == KErrNone || pluginErr == KErrInUse )
                    {
                    pluginErr = KErrNone; // If already enabled, complete client request with KErrNone
                    // Update common data power save
                    iFmTxCommonData.UpdatePowerSaveState( 
                                        CHWRMFmTxCommonData::EHWRMFmTxPowerSaveOff );
                    // Store the new status
                    iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransOn);
                    }
                }
                break;              

            case HWRMFmTxCommand::ETxOffCmdId:
                {
                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ETxOffCmdId pluginErr = %d" ), pluginErr );

                if ( pluginErr == KErrNone || pluginErr == KErrNotReady )
                    {
                    pluginErr = KErrNone; // If already disabled, complete client request with KErrNone
                    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
                        
                    // Store the new status
                    if ( data && data->iRequestMessage.Handle() )
                        {
                        // User has disabled FM Tx 
                        iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransRequestedOff);
                        ExecutePluginCommand(HWRMFmTxCommand::EStopObservingCmdId);
                        }
                    else
                        {
                        // FM Tx has been automatically powered down
                        iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransOff);
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::EGetTxFrequencyRangeCmdId:
                {
                // Unpack the FM Tx frequency range
                HWRMFmTxCommand::TFrequencyRangePackage freqRangePckg;
                freqRangePckg.Copy(aData);
                HWRMFmTxCommand::TFrequencyRangeData freqRange = freqRangePckg();

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, EGetTxFrequencyRangeCmdId error = %d" ), freqRange.iErrorCode );

                // Validate response values.  Tx frequency range will not be changed dynamically
                if ( freqRange.iErrorCode != KErrNone ||
                     freqRange.iMinFrequency == 0 ||
                     freqRange.iMaxFrequency == 0 ||
                     freqRange.iStepSize == 0 )
                    {
                    // Set backup frequency range defaults
                    freqRange.iMinFrequency = KHWRMFmTxBackupMinFreq;
                    freqRange.iMaxFrequency = KHWRMFmTxBackupMaxFreq;
                    freqRange.iStepSize     = KHWRMFmTxBackupStepSize;
                    }

                // Store the frequency range
                iFmTxCommonData.SetFrequencyRange( freqRange.iMinFrequency, freqRange.iMaxFrequency, freqRange.iStepSize );

                // Return frequency range to client
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
                if ( data && data->iRequestMessage.Handle() )
                    {
                    TFmTxFrequencyRange clientFreqRange;
                    clientFreqRange.iMinFrequency = freqRange.iMinFrequency;
                    clientFreqRange.iMaxFrequency = freqRange.iMaxFrequency;
                    clientFreqRange.iStepSize     = freqRange.iStepSize;
                    TPckg<TFmTxFrequencyRange> clientFreqRangePckg(clientFreqRange);
                    pluginErr = data->iRequestMessage.Write(0,clientFreqRangePckg);
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxFrequencyCmdId:
                {   
                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();
                
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxFrequencyCmdId pluginErr = %d" ), pluginErr );
                
                if ( pluginErr == KErrNone )
                    {
                    // Get new Tx frequency from the request
                    THWRMFmTxPluginRequestData* data = static_cast<THWRMFmTxPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
                    if ( data )
                        {
                        // Store the new frequency
                        iFmTxCommonData.UpdateFrequency( data->iArg );
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ETxScanRequestCmdId:
                {
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
                    
                // Unpack the scan response error code
                HWRMFmTxCommand::TScanResponsePackage scanPckg;
                scanPckg.Copy(aData);
                HWRMFmTxCommand::TScanResponseData scanResp = scanPckg();               
                COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ETxScanRequestCmdId error = %d, found = %d" ), scanResp.iErrorCode, scanResp.iFrequenciesFound );
                                
                if ( scanResp.iErrorCode != KErrNone ||
                     scanResp.iFrequenciesFound == 0 ||
                     scanResp.iFrequenciesFound > KClearFrequencyArrayMax ) 
                    {
                    pluginErr = scanResp.iErrorCode;
                    }
                else if ( data && data->iRequestMessage.Handle() )
                    {               
                    TInt firstClearFreq = scanResp.iChannels[0];
                    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL scanResp.iChannels[0] = %d" ), firstClearFreq );
                        
                    if ( data->iRequestMessage.Function() == EHWRMFmTxGetNextClearFreqArray )
                        {
                        // return clear frequency array to clients
                        TPckg<TClearFrequencyArray> clientClearFreqArrayPckg( scanResp.iChannels );
                        pluginErr = data->iRequestMessage.Write(0,clientClearFreqArrayPckg);
                        if ( pluginErr == KErrNone )
                            {
                            // return number of clear frequencies found
                            TPckg<TUint> clientClearFreqCountPckg( scanResp.iFrequenciesFound );
                            pluginErr = data->iRequestMessage.Write(1,clientClearFreqCountPckg);                            
                            }
                        }
                    else // EHWRMFmTxGetNextClearFreq or EHWRMFmTxSetNextClearFreq
                        {       
                        // return clear frequency to client
                        TPckg<TInt> clientClearFreqPckg( firstClearFreq );
                        pluginErr = data->iRequestMessage.Write(0,clientClearFreqPckg);
                        }

                    if ( pluginErr == KErrNone &&
                         data->iRequestMessage.Function() == EHWRMFmTxSetNextClearFreq )
                        {
                        // Continue split command
                        HWRMFmTxCommand::TSetFrequencyPackage pckg( firstClearFreq );
                        TRAP(pluginErr, ExecutePluginCommandL(data->iRequestMessage, HWRMFmTxCommand::ESetTxFrequencyCmdId, ETrue/*split*/, pckg, firstClearFreq ));
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxRdsPsCmdId:
                {
                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();  
                
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxRdsPsCmdId pluginErr = %d" ), pluginErr ); 
                }
                break;
                
            case HWRMFmTxCommand::ESetTxRdsPtyCmdId:
                {
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));

                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();                      

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxRdsPtyCmdId pluginErr = %d" ), pluginErr );  

                if ( data && data->iRequestMessage.Handle() && pluginErr == KErrNone )
                    {                   
                    if ( data->iRequestMessage.Function() == EHWRMFmTxClearRdsData )
                        {
                        iFmTxCommonData.UpdateRdsPty( KRdsPtyNone );
                        }
                    else
                        {
                        iFmTxCommonData.UpdateRdsPty( data->iRequestMessage.Int0() );
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxRdsPtynCmdId:
                {
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));

                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();                      

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxRdsPtynCmdId pluginErr = %d" ), pluginErr );  

                if ( data && data->iRequestMessage.Handle() && pluginErr == KErrNone )
                    {
                    if ( data->iRequestMessage.Function() == EHWRMFmTxClearRdsData )
                        {
                        TBuf<1> emptyDes;
                        iFmTxCommonData.UpdateRdsPtyn( emptyDes );
                        }
                    else
                        {
                        TRdsProgrammeTypeName clientPtyn;           
                        if ( data->iRequestMessage.Read(0,clientPtyn) == KErrNone )
                            {
                            iFmTxCommonData.UpdateRdsPtyn( clientPtyn );
                            }
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxRdsMsCmdId:
                {
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));

                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();                      

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxRdsMsCmdId pluginErr = %d" ), pluginErr ); 

                if ( data && data->iRequestMessage.Handle() && pluginErr == KErrNone )
                    {                                   
                    if ( data->iRequestMessage.Function() == EHWRMFmTxClearRdsData )
                        {
                        iFmTxCommonData.UpdateRdsMs( ETrue /*defaults to Music*/ );
                        }                   
                    else
                        {
                        iFmTxCommonData.UpdateRdsMs( data->iRequestMessage.Int0() );
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxRdsLangIdCmdId:
                {
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));

                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();                      

                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, ESetTxRdsLangIdCmdId pluginErr = %d" ), pluginErr );                    

                if ( data && data->iRequestMessage.Handle() && pluginErr == KErrNone )
                    {           
                    if ( data->iRequestMessage.Function() == EHWRMFmTxClearRdsData )
                        {
                        iFmTxCommonData.UpdateRdsLanguageId( KRdsLanguageUnknown );
                        }                                       
                    else
                        {                           
                        iFmTxCommonData.UpdateRdsLanguageId( data->iRequestMessage.Int0() );
                        }
                    }
                }
                break;

            case HWRMFmTxCommand::ESetTxRtCmdId:
            case HWRMFmTxCommand::EClearTxRtCmdId:
                {
                // Unpack the error code
                HWRMFmTxCommand::TErrorCodeResponsePackage errPckg;
                errPckg.Copy(aData);
                pluginErr = errPckg();                                  
                
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, RT pluginErr = %d" ), pluginErr );     
                }
                break;

            case HWRMFmTxCommand::EStopObservingCmdId:
                {
                watcherMsg = ETrue;
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL, watcher cmd pluginErr = %d" ), pluginErr );
                }
                break;

            default:
                {
                iFmTxCommonData.UpdateStatus(CHWRMFmTxCommonData::EFmTxStateTransError);
                }
                break;              
            }
        }

    // Complete request and delete transaction
    if( !watcherMsg )
        {
        contextErr = CompleteRequest( aTransId, pluginErr );
        }
    else
        {
        contextErr = CompleteRequest( aTransId,
                                      pluginErr,
                                      iWatcherTransactionList,
                                      completeWatcherMsg );
        }

    // Leave if there is error in context    
    User::LeaveIfError(contextErr);

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ProcessResponseL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::CheckResponseDataSize
// Helper for ProcessResponseL.  Checks data size returned in plugin responses.
// -----------------------------------------------------------------------------
//
TBool CHWRMFmTxService::CheckResponseDataSize( TInt aCommandId, TDesC8& aData )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::CheckResponseDataSize(0x%x)" ), aCommandId);      

    TInt requiredDataSize;

    switch ( aCommandId )
        {       
        case HWRMFmTxCommand::ETxOnCmdId:           // fall through
        case HWRMFmTxCommand::ETxOffCmdId:          // fall through
        case HWRMFmTxCommand::ESetTxFrequencyCmdId: // fall through
        case HWRMFmTxCommand::ESetTxRdsPsCmdId:     // fall through
        case HWRMFmTxCommand::ESetTxRdsPtyCmdId:    // fall through
        case HWRMFmTxCommand::ESetTxRdsPtynCmdId:   // fall through
        case HWRMFmTxCommand::ESetTxRdsMsCmdId:     // fall through
        case HWRMFmTxCommand::ESetTxRdsLangIdCmdId: // fall through
        case HWRMFmTxCommand::ESetTxRtCmdId:        // fall through
        case HWRMFmTxCommand::EClearTxRtCmdId:      // fall through
        case HWRMFmTxCommand::EStopObservingCmdId:  // fall through      
            requiredDataSize = sizeof(TInt);
            break;
              
        case HWRMFmTxCommand::EGetTxFrequencyRangeCmdId:
            requiredDataSize = sizeof(HWRMFmTxCommand::TFrequencyRangeData);
            break;

        case HWRMFmTxCommand::ETxScanRequestCmdId:
            requiredDataSize = sizeof(HWRMFmTxCommand::TScanResponseData);
            break;          

        case HWRMFmTxCommand::EStartObservingCmdId:
            requiredDataSize = sizeof(HWRMFmTxCommand::TConnStatusData);
            break;

        case HWRMFmTxCommand::ENoCommandId: // fall through            
        default:
            requiredDataSize = 0;
            break;
        }
    
    if ( aData.Size() != requiredDataSize )
        {
        COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxService::CheckResponseDataSize error 0x%x, 0x%x" ), aData.Size(), requiredDataSize );
        return EFalse; // Data size is invalid
        }     

    return ETrue; // Data size is ok
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::SuspendSubResource
// Suspends FM Tx target
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::SuspendSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::SuspendSubResource" ) );
    iSuspended = ETrue;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ResumeSubResource
// Resumes suspended FM Tx target 
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ResumeSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ResumeSubResource" ) );
    iSuspended = EFalse;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ActivateSubResource
// FM Tx doesn't support activation/deactivation based on device state
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ActivateSubResource(TInt /*aSubResource*/, TBool /*aActivate*/)
    {
    //COMPONENT_TRACE3( _L( "HWRM Server - CHWRMFmTxService::ActivateSubResource(0x%x, 0x%x)" ), aSubResource, aActivate );
    // Do nothing    
    //COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ActivateSubResource - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::LeaveIfOtherReservationL
// Helper for ExecuteMessageL methods
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::LeaveIfOtherReservationL()
    {
    // Check reservation.
    if ( iSuspended || iReservationHandler->IsReserved(this, KHWRMAllSubResources) )
        {
        User::Leave(KErrInUse);
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::LeaveIfTransmitterOffL
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::LeaveIfTransmitterOffL()
    {
    // Check Tx status
    if ( !iFmTxCommonData.IsFmTxHardwareOn() )
        {
        COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxService::LeaveIfTransmitterOffL()" ) ) );
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::LeaveIfTagInvalidL
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::LeaveIfTagInvalidL( TRtPlusTag aTag )
    {
    const TInt KMaxRtPlusMarkerPos( HWRMFmTxCommand::KMaxRtDataLength - 1 );
    
    // Check RT Plus tag
    if ( aTag.iContentType < ERtPlusItemTitle || aTag.iContentType > ERtPlusGetData ||
         aTag.iStartMarker < 0 || aTag.iLengthMarker < 0 ||
         aTag.iLengthMarker > KMaxRtPlusMarkerPos ||
         aTag.iStartMarker > aTag.iLengthMarker )
        { 
        COMPONENT_TRACE4( _L( "HWRM Server - CHWRMFmTxService::LeaveIfTagInvalidL, tag error (%d, %d, %d)"), 
            aTag.iContentType, aTag.iStartMarker, aTag.iLengthMarker ); 
        User::Leave(KErrArgument); 
        } 
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::DoRelease
// Helper for releasing FM Tx resource
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::DoRelease()
    {
    if ( iReserved )
        {    
        iReservationHandler->Release(this, KHWRMAllSubResources);
        }

    iReserved  = EFalse;
    iSuspended = EFalse;        
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecutePluginCommand
// Handles plug-in requests that are not client initiated
// Called by Common Data service instance
// -----------------------------------------------------------------------------
//  
void CHWRMFmTxService::ExecutePluginCommand(HWRMFmTxCommand::TFmTxCmd aCommand)
    {
    COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxService::ExecutePluginCommand(), non-client version" ) );

    RMessage2 dummy;
    TInt ignoreErr; // avoid compiler warning with TRAPD
    TRAP(ignoreErr, ExecutePluginCommandL(dummy, aCommand, EFalse/*not split*/ ));

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommand - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecutePluginCommand
// Handles plug-in requests that are not client initiated
// Called by Common Data service instance
// -----------------------------------------------------------------------------
//  
void CHWRMFmTxService::ExecutePluginCommand(HWRMFmTxCommand::TFmTxCmd aCommand,
                                            TBool /* aSplitCommand */,
                                            TDesC8& aPckg )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL, non-client version" ) );
        
    RMessage2 dummy;
    TInt ignoreErr; // avoid compiler warning with TRAPD
    TRAP(ignoreErr, ExecutePluginCommandL(dummy, aCommand, EFalse/*not split*/, aPckg ));

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL - return" ) );      
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ExecutePluginCommandL(const RMessage2& aMessage,
                                             HWRMFmTxCommand::TFmTxCmd  aCommandId,
                                             TBool aSplitCommand )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL" ) );       
        
    // Create new data (TransId is updated later) 
    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, aCommandId, aSplitCommand);
    CleanupStack::PushL( data );    

    TBuf8<1> emptyDes;    
    if( aCommandId == HWRMFmTxCommand::EStartObservingCmdId ||
        aCommandId == HWRMFmTxCommand::EStopObservingCmdId )
        {
        data->iTransId = iWatcherPluginHandler.ProcessCommandL(aCommandId, emptyDes, this);
        // data still needed, do not destroy, just pop
        CleanupStack::Pop( data );
        // Add data to list
        iWatcherTransactionList->AddTransaction( data );
        }
    else
        {
        data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, emptyDes, this);
        // data still needed, do not destroy, just pop
        CleanupStack::Pop( data );
        // Add data to list
        iTransactionList->AddTransaction( data );
        }

    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL - return" ) );        
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ExecutePluginCommandL(const RMessage2& aMessage,
                                             HWRMFmTxCommand::TFmTxCmd aCommandId,
                                             TBool aSplitCommand,
                                             TDesC8& aPckg )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL with pckg" ) );         
        
    // Create new data (TransId is updated later) 
    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, aCommandId, aSplitCommand);
    CleanupStack::PushL( data );    
    
    data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, aPckg, this);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL - return" ) );     
    }
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::ExecutePluginCommandL(const RMessage2& aMessage,
                                             HWRMFmTxCommand::TFmTxCmd aCommandId,
                                             TBool aSplitCommand,
                                             TDesC8& aPckg,
                                             TInt aArg )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL aArg = %d" ), aArg );  
        
    // Create new data (TransId is updated later) 
    THWRMPluginRequestData* data = new (ELeave) THWRMFmTxPluginRequestData(aArg, aMessage, 0, aCommandId, aSplitCommand);
    CleanupStack::PushL( data );    
 
    data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, aPckg, this);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::ExecutePluginCommandL - return" ) ) ;
    }       
    
// -----------------------------------------------------------------------------
// CHWRMFmTxService::CancelPluginCommandL
// Cancels a plugin command and handles the related transaction(s).
// Note that plug-in handler CancelCommandL's will not cause client requests to complete
// -----------------------------------------------------------------------------
//
void CHWRMFmTxService::CancelPluginCommandL(HWRMFmTxCommand::TFmTxCmd aCommandId)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::CancelPluginCommandL" ) );

    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->GetFirstItem());

    // Cancel each transaction with the cancelled command ID
    while ( data )
        {
        if ( data->iCommandId == aCommandId &&
             data->iRequestMessage.Handle() )
            {
            // Cancel transaction
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMService::CancelPluginCommandL() - Canceling pending message" ) );
            data->iRequestMessage.Complete(KErrCancel);
            
            // Record transaction ID, and move to next item
            TUint8 cancelTransId = data->iTransId;
            data = static_cast<THWRMPluginRequestData*>(data->iNextData);

            // Remove cancelled transaction items
            // First cancel plugin command. If successful, then no call to ProcessResponseL
            iPluginHandler->CancelCommandL( cancelTransId );
            iTransactionList->FindTransaction( cancelTransId, ETrue);
            }
        else
            {
            // Move to next item
            data = static_cast<THWRMPluginRequestData*>(data->iNextData);
            }
        }

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxService::CancelPluginCommandL - return" ) );     
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
