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
#include "HWRMVibraClientServer.h"
#include "HWRMService.h"
#include "HWRMVibraCommonService.h"
#include "HWRMVibraCommands.h"
#include "HWRMPluginHandler.h"
#include "HWRMtrace.h"
#include "HWRMVibraCommonData.h"
#include "HWRMVibra.h"
#include "HWRMReservationHandler.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMDomainPSKeys.h"
#include "HWRMPrivatePSKeys.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "HWRMVibraCommonService" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::CHWRMVibraCommonService
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraCommonService::CHWRMVibraCommonService()
    {    
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                   CHWRMReservationHandler* aReservationHandler)
    {    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ConstructL(0x%x)" ), aPluginHandler);

    BaseConstructL(aPluginHandler, aReservationHandler);
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ConstructL - return " ));
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMVibraCommonService* CHWRMVibraCommonService::NewL(CHWRMPluginHandler* aPluginHandler,
                                           CHWRMReservationHandler* aReservationHandler)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraCommonService::NewL(0x%x, 0x%x)" ), aPluginHandler, aReservationHandler );

    CHWRMVibraCommonService* self = new( ELeave ) CHWRMVibraCommonService();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::NewL - return 0x%x" ), self);
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMVibraCommonService::~CHWRMVibraCommonService()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::~CHWRMVibraCommonService()" ));

    // Base class cleanup
    BaseDestruct();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::~CHWRMVibraCommonService - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ExecuteMessageL
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
TBool CHWRMVibraCommonService::ExecuteMessageL( const RMessage2& aMessage )
#else
TBool CHWRMVibraCommonService::ExecuteMessageL( const RMessage2& /* aMessage */ )
#endif
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ExecuteMessageL(0x%x)" ), aMessage.Function() );

    // This common service is never supposed to get messages for execution
    __ASSERT_ALWAYS(EFalse, User::Panic(KPanicCategory, EPanicBadHandle));

    return EFalse;
    }

    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ProcessResponseL
// Handles Vibra requests responses.
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMVibraCommonService::ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout)
#else
void CHWRMVibraCommonService::ProcessResponseL( TInt /* aCommandId */,  TUint8 aTransId, TDesC8& aData, TBool aTimeout)
#endif
    {
    COMPONENT_TRACE4(_L( "HWRM Server - CHWRMVibraCommonService::ProcessResponseL(0x%x, 0x%x, <data>, 0x%x)" ), aCommandId, aTransId, aTimeout );

    TInt pluginErr(KErrNone);  // Error came from plugin as data (or timeout). Used to complete RMessage.
    TInt contextErr(KErrNone);  // Error in context, i.e. bad handle or descriptor. Used to leave.

    // Unpack the package. All vibra messages contain only possible error code
    // in return package.
    if ( !aTimeout && aData.Size() != sizeof(TInt) )
        {
        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraCommonService::ProcessResponseL - Data size mismatch, expected: 0x%x, got 0x%x" ), sizeof(TInt), aData.Size() );
        contextErr = KErrBadDescriptor;
        pluginErr = KErrBadDescriptor;
        }      
    else
        {        
        // If response was caused by timeout, set that as the error.
        // Timeout handled this way instead of just returning KErrTimeout in aData because
        // aData format is not known in plugin handler and in theoretically some new commands
        // may also return different response than just error code in future.
        if ( aTimeout )
            {
            pluginErr = KErrTimedOut;            
            }
        else
            {
            HWRMVibraCommand::TErrorCodeResponsePackage errPckg;
            errPckg.Copy(aData);
            pluginErr = errPckg();
            }

        if ( pluginErr != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ProcessResponseL - Error: %d" ), pluginErr );
            }

        // It is on each service's responsibility to update vibra status p&s keys
        }
        
    // Complete request and remove delete transaction
    contextErr = CompleteRequest(aTransId, pluginErr);    

    // Leave if there is error in context    
    User::LeaveIfError(contextErr);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ProcessResponseL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::SuspendSubResource
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::SuspendSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::SuspendSubResource" ) );

    // This common service is never supposed to get messages for execution
    __ASSERT_ALWAYS(EFalse, User::Panic(KPanicCategory, EPanicBadHandle));
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ResumeSubResource
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::ResumeSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ResumeSubResource" ) );

    // This common service is never supposed to get messages for execution
    __ASSERT_ALWAYS(EFalse, User::Panic(KPanicCategory, EPanicBadHandle));
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ActivateSubResource
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::ActivateSubResource(TInt /*aSubResource*/, TBool /*aActivate*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ActivateSubResource" ) );

    // This common service is never supposed to get messages for execution
    __ASSERT_ALWAYS(EFalse, User::Panic(KPanicCategory, EPanicBadHandle));
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::SetClientData
// Sets client's parameters.
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::SetClientData( TVibraClientData aData )
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::SetClientData" ) );

    // We cannot do anything if insertion of data fails. That is not dealt as ciritical
    // problem anyway, because this request just won't get continued.
    TInt err = iVibraClients.Insert( aData, 0 );

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::SetClientData err %d" ), err );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::HasData
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonService::HasData( CHWRMService* aClient )
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::HasData" ) );
    
    TBool hasData( EFalse );
    TInt count( iVibraClients.Count() );

    for ( TInt i = 0; i < count; i++ )
        {
        if( iVibraClients[i].iClient == aClient )
            {
            hasData = ETrue;
            break;
            }
        }
    
    return hasData;
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ResetClientData
// Resets client's data.
// If one service has finished with (all) vibrations, it calls this method. 
// At this point all entries of service are removed and other service's 
// vibration is continued if any available.
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::ResetClientData( CHWRMService* aClient )
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ResetClientData" ) );
    
    // Remove service's items from list
    TInt count( iVibraClients.Count() );

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ResetClientData - count %d" ), count );

    for ( TInt i=count-1; count > 0 && i >= 0; i-- )
        {
        if( iVibraClients[i].iClient == aClient )
            {
            iVibraClients.Remove( i );
            }
        }

    // Restart next available "suspended" request. It is ok if original request's 
    // timer fires just after vibration has been restarted, because we are
    // still running start/stop requests in context of same thread.
    count = iVibraClients.Count();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ResetClientData - count %d" ), count );

    if( count > 0 )
        {
        // Start vibration with last suspended intensity. If restart fails, we
        // cannot do anything about that and should not leave calling service.
        TRAPD( err, RestartVibraL() );
        if( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ResetClientData - err %d" ), err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::RestartVibraL
// Restarts vibration with last suspended intensity
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonService::RestartVibraL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::RestartVibraL" ) );
    
	// Ignore Coverity false positive as uninitialized variable.
	// Since no response is sent back to the client, only a empty message is created here. 
	// Message handle of this will be initialized to NULL in the default constructor.
	// coverity[var_decl]
    RMessage2 dummy;

    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(dummy, 0, 0, EFalse); 
    CleanupStack::PushL( data );

    if( iVibraClients[0].iIntensity == KMaxTInt )
        {
        TBuf8<1> emptyDes;
        data->iTransId = iPluginHandler->ProcessCommandL( 
            HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId, emptyDes, this );
        }
    else
        {
        HWRMVibraCommand::TStartVibraCmdDataPackage dataPckg(iVibraClients[0].iIntensity);    
        data->iTransId = iPluginHandler->ProcessCommandL(
            HWRMVibraCommand::EStartVibraCmdId, dataPckg, this );    
        }

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );
    
    // Add data to list
    iTransactionList->AddTransaction( data );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::ExecutingPriorityHigher
// Checks whether vibrating client's priority higher than new request making
// client's priority
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonService::ExecutingPriorityHigher(TSecureId aSid)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::ExecutingPriorityHigher" ) );

    TBool ret( EFalse );

    TInt count = iVibraClients.Count();

    if( count > 0 )
        {
        TBool trusted( EFalse );
        // Last client requested vibration always the first one in list
        TInt activePriority = iReservationHandler->GetPolicy()->GetPriority( iVibraClients[0].iSid, trusted );
        TInt reqPriority = iReservationHandler->GetPolicy()->GetPriority( aSid, trusted );
    
        if( activePriority < reqPriority )
            {
            ret = ETrue;
            }
        }
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::ExecutingPriorityHigher - ret %d" ), ret);
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonService::VibraPulseAllowed
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonService::VibraPulseAllowed(TSecureId aSid)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonService::VibraPulseAllowed" ) );

    TBool ret = iReservationHandler->GetPolicy()->FeedbackClient( aSid );
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonService::VibraPulseAllowed - ret %d" ), ret );
    
    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
