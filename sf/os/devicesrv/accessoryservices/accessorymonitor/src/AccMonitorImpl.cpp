/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The implementation for Accessory Monitoring API virtual
*        functions.
*
*/


// INCLUDES
#include <AccMonitor.h>
#include "AccMonitorImpl.h"
#include "AccMonitorContainer.h"
#include "acc_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAccMonitorImpl::CAccMonitorImpl
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorImpl::CAccMonitorImpl()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::CAccMonitorImpl()" );
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::CAccMonitorImpl - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::ConstructL
// 
// Costructs an instance of the container.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::ConstructL()" );
    iContainer = CAccMonitorContainer::NewL();
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::ConstructL - return" );
    }
    
// ---------------------------------------------------------------------------
// CAccMonitorImpl::CAccMonitorImpl
// 
// Default C++ constructor.
// ---------------------------------------------------------------------------
CAccMonitorImpl::~CAccMonitorImpl()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::~CAccMonitorImpl()" );
    delete iContainer;
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::~CAccMonitorImpl - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::GetConnectedAccessoriesL
// 
// Get Connected accessories, other items commented in header file.
// ---------------------------------------------------------------------------
TInt CAccMonitorImpl::GetConnectedAccessoriesL(
    RConnectedAccessories& aAccessoriesArray ) const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::GetConnectedAccessoriesL - return" );
    return iContainer->GetConnectedAccessoriesL( aAccessoriesArray );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Start Observing, other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::StartObservingL(
    MAccMonitorObserver* aObserver )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - All Accessory connections" );
    
    TBool val = IsObserving();
    if( val )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        iContainer->StartObservingL( aObserver );
        }
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - All Accessory connections - return" );
    }
  
// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Start Observing, other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::StartObservingL( 
    MAccMonitorObserver* aObserver,
    const CAccMonitorInfo* aInfo )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - Accessory type connections" );
    
    TBool val = IsObserving();
    if( val )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        iContainer->StartObservingL( aObserver, aInfo );
        }
      
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - All Accessory connections - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StartObserving
// 
// Start Observing, other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::StartObservingL( 
    MAccMonitorObserver* aObserver,
    const RAccMonCapabilityArray& aArray )
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - Specific accessory connections" );
    
    TBool val = IsObserving();
    if( val )
        {
        User::Leave( KErrAlreadyExists );
        }
    else
        {
        iContainer->StartObservingL( aObserver, aArray );
        }
    
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StartObservingL() - All Accessory connections - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::StopObserving
// 
// Stop Observing, other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::StopObserving()
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StopObserving()" );
    iContainer->StopObserving();
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::StopObserving() - return" );
    }

// ---------------------------------------------------------------------------
// CAccMonitor::IsObserving
// 
// Checks if observer is active, other items commented in header file.
// ---------------------------------------------------------------------------
TBool CAccMonitorImpl::IsObserving() const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::IsObserving()" );
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorImpl::IsObserving() - return %d", iContainer->IsObserving() );
    return iContainer->IsObserving();
    }
  
// ---------------------------------------------------------------------------
// CAccMonitor::GetCapabilityValue
// 
// Gets a value for some capability, other items commented in header file.
// ---------------------------------------------------------------------------
void CAccMonitorImpl::GetCapabilityValueL( 
    const CAccMonitorInfo* aInfo,
    const TUint32 aCapability,
    TInt& aValue ) const
    {
    COM_TRACE_( "[AccFW:AccMonitor] CAccMonitorImpl::GetCapabilityValue()" );
    iContainer->GetValueForCapabilityL( aInfo,  aCapability, aValue );
    COM_TRACE_1( "[AccFW:AccMonitor] CAccMonitorImpl::GetCapabilityValue() - return aValue: %d", aValue );
    }
  
// End of file
