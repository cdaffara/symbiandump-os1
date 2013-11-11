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
#include "HWRMPrivatePSKeys.h"
#include "HWRMtrace.h"
#include "HWRMExtendedLight.h"
#include "HWRMExtendedLightStatusObserver.h"
#include "HWRMLightClientServer.h"  // for KHWRMLightMaxTargets

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::CHWRMExtendedLightStatusObserver
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMExtendedLightStatusObserver::CHWRMExtendedLightStatusObserver(MHWRMExtendedLightObserver* aCallback,
                                                                   TInt aSupportedTargets)
    : CActive(EPriorityStandard), 
      iCallback(aCallback),
      iSupportedTargets(aSupportedTargets)  
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::CHWRMExtendedLightStatusObserver(0x%x)" ), aCallback);

    CActiveScheduler::Add(this);

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::CHWRMExtendedLightStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::NewL
// Two-phased constructor 
// -----------------------------------------------------------------------------
//
CHWRMExtendedLightStatusObserver* CHWRMExtendedLightStatusObserver::NewL(MHWRMExtendedLightObserver* aCallback,
                                                                         TInt aSupportedTargets)
    {

    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::NewL(0x%x)" ), aCallback);

	CHWRMExtendedLightStatusObserver* newInstance = new (ELeave) CHWRMExtendedLightStatusObserver(aCallback, aSupportedTargets);

	CleanupStack::PushL( newInstance );

	newInstance->ConstructL();

	CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::NewL - return 0x%x" ), newInstance );

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMExtendedLightStatusObserver::~CHWRMExtendedLightStatusObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::~CHWRMExtendedLightStatusObserver()" ));

    // Cancel property separately, as the following Cancel() call doesn't seem to 
    // cause DoCancel to fire.
    if( iProperty.Handle() )
      {
      iProperty.Cancel();
      }
    
    Cancel();
    
    iProperty.Close();
    
    iLastStatusValue.Close();
    iStatusUnknownList.Close();
    iCallback = NULL; // PCLint demands

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::~CHWRMExtendedLightStatusObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightStatusObserver::ConstructL()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::ConstructL()" ));

    User::LeaveIfError( InitializeStatusArray( iLastStatusValue ) );
    User::LeaveIfError( InitializeStatusArray( iStatusUnknownList ) );
    User::LeaveIfError(iProperty.Attach(KPSUidHWResourceNotification, KHWRMLightStatus));
    
    // Call RunL to order notification and do the initial notifying of the client
    RunL();

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::RunL
// Handle notification from pubsub session.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightStatusObserver::RunL()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::RunL()" ));

    // Reorder notification
    OrderNotification();

    // get value from pubsub
    RLightStatusArray statusArray( KHWRMLightMaxTargets );
    CleanupClosePushL( statusArray );
    TInt err = InitializeStatusArray( statusArray );
    if( err == KErrNone )
        {
        TInt arraySize = sizeof( THWRMStatusInfo ) * KHWRMLightMaxTargets;
        TPtr8 arrayPtr( ( TUint8* )&statusArray[0], arraySize, arraySize );
        err = iProperty.Get( arrayPtr );
    if ( err == KErrNotFound || err == KErrNone )
        {
            NotifyCallback( statusArray );
            }
        }
    
    else
        {
        COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::RunL. Error (%d) in iProperty.Get()" ), err);
        NotifyCallback(iStatusUnknownList );
        }
        
    CleanupStack::PopAndDestroy( &statusArray );
    User::LeaveIfError( err );

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::RunL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::DoCancel
// Handle cancel order on this active object.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightStatusObserver::DoCancel()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::DoCancel()" ));

    iProperty.Cancel();

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::DoCancel - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::OrderNotification
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightStatusObserver::OrderNotification()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::OrderNotification()" ));

    iProperty.Subscribe(iStatus);
    SetActive();

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::OrderNotification - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::NotifyCallback
// Order new notification from CenRep.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightStatusObserver::NotifyCallback( RLightStatusArray& aStatusList )
    {
    COMPONENT_TRACE1( ( _L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::NotifyCallback" ) ) );


    // Loop through value and update client. Hold notifying client until end unless there are
    // multiple differently changed target statuses.
    TInt currentTarget = KHWRMLightFirstTarget;
    TInt currentStatus = 0;
    TInt lastChangedStatus = 0;
    TInt index = 0;
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        // only notify for supported targets
        if( currentTarget & iSupportedTargets )
            {            
            index = aStatusList.FindInOrder( currentTarget, FindByTarget ); 
            if( index >= 0 &&  index < KHWRMLightMaxTargets )
                {
                currentStatus = aStatusList[index].iStatus;
                index = iLastStatusValue.FindInOrder( currentTarget, FindByTarget ); 
                if( index >= 0 && index < KHWRMLightMaxTargets )
                    {
                    lastChangedStatus = iLastStatusValue[index].iStatus;
                    // If last value for masked target is different than new value, prepare to notify about that target
                    if( lastChangedStatus != currentStatus )
                        {
                        // update last status value and notify client
                        iLastStatusValue[index].iStatus = currentStatus;
                        if( iCallback )
                            {
                            iCallback->LightStatusChanged( currentTarget,
                                static_cast<CHWRMExtendedLight::TLightStatus>( currentStatus ) );    
                            }
                        }
                    }
                }
            }
            
        // shift the target ready for next loop
        currentTarget <<= 1;
        }

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::NotifyCallback - return" ) );
    
    }
    
// -----------------------------------------------------------------------------
// CHWRMExtendedLightStatusObserver::CurrentStatus
// Return Light status
// -----------------------------------------------------------------------------
//
CHWRMExtendedLight::TLightStatus CHWRMExtendedLightStatusObserver::CurrentStatus(TInt aTarget) const
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::CurrentStatus(0x%x)" ), aTarget );
    
    CHWRMExtendedLight::TLightStatus status( CHWRMExtendedLight::ELightStatusUnknown );
    RLightStatusArray currentStatus( KHWRMLightMaxTargets );
    TInt err = InitializeStatusArray( currentStatus );
    if ( err == KErrNone )
        {
        TInt arraySize = sizeof( THWRMStatusInfo ) * KHWRMLightMaxTargets;
        TPtr8 arrayPtr( ( TUint8* )&currentStatus[0], arraySize, arraySize );
        err = RProperty::Get( KPSUidHWResourceNotification, KHWRMLightStatus, arrayPtr );
        if( err == KErrNone )
        {
            TInt index = currentStatus.FindInOrder( aTarget, FindByTarget );
            if( index >= 0 && index < KHWRMLightMaxTargets )
            {
                status = static_cast<CHWRMExtendedLight::TLightStatus>(
                    currentStatus[index].iStatus );
                }
            }
        }
        
    // Clean up
    currentStatus.Close();   
    
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightStatusObserver::CurrentStatus - return 0x%x" ), status );
    
    return status;
    }

// -----------------------------------------------------------------------------
// CHWRMLightExtendedStatusObserver::FindByTarget
// Finds item from iStatusList by target.
// -----------------------------------------------------------------------------
//
TInt CHWRMExtendedLightStatusObserver::FindByTarget( const TInt* aTarget, 
    const THWRMStatusInfo& aItem )
    {
    if ( *aTarget < aItem.iTarget )
        {
        return -1;
        }
    else if ( *aTarget > aItem.iTarget )
        {
        return 1;
        }

    return 0;
    }
// -----------------------------------------------------------------------------
// CHWRMLightStatusObserver::InitializeStatusArray
// Initializes RLightStatusArray.
// -----------------------------------------------------------------------------
//
TInt CHWRMExtendedLightStatusObserver::InitializeStatusArray(
    RLightStatusArray& aArray ) const
    {
    TInt err = KErrNone;
    TInt currentTarget( KHWRMLightFirstTarget );
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        THWRMStatusInfo info;
        info.iTarget = currentTarget;
        info.iStatus = CHWRMLight::ELightStatusUnknown;
        err = aArray.Append( info );
        if( err != KErrNone )
            {
            break;
            }
        
        // shift the mask and target ready for next loop
        currentTarget <<= 1;
        }

    return err;
    }

// End of File
