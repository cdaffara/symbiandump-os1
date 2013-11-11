// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcustomstubextTraces.h"
#endif

#include "cmmmessagerouter.h"
#include "cmmcustomstubext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

const TUint KPhoneInfoDescSize = 15;

// ======== MEMBER FUNCTIONS ========

CMmCustomStubExt::CMmCustomStubExt()
    : iMessageRouter( NULL ), 
    iFirstTime ( ETrue ),
    iVendorExtComplete( NULL )
    {
    }

CMmCustomStubExt::~CMmCustomStubExt() 
    {  
    }
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::DoExtFuncL
// 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt /*aIpc*/, 
    const TDataPackage& /*aPackage*/ ) 
    { 
    TInt ret = KErrNotSupported;
       
    if( iFirstTime )
        {
        // crate datapackage
        CMmDataPackage data;
        CMmDataPackage* package;
        package = NULL;
        
        // phone id        
        TBuf8<KPhoneInfoDescSize> tmp;
        _LIT8( phoneid,"000000000000000" );            
        tmp.Copy( phoneid );
        data.PackData( &tmp );
        package = &data; 
        iMessageRouter->MessageManager()->Complete( 
            EMobilePhoneGetPhoneId, package, KErrNone ); 
         
        // Create dummy IMSI
        TBuf8<KPhoneInfoDescSize> temp;
        _LIT8( imsi,"000000000000000" );
        temp.Copy( imsi );
        data.PackData( &temp );
        package = &data;
        iMessageRouter->MessageManager()->Complete( 
            EMobilePhoneGetSubscriberId, package, KErrNone );
         
        iFirstTime = EFalse;  
        }           
    else
        {
        // test the new ReqComplete interface
        iVendorExtComplete->ReqCompleted( aTsyReqHandle, KErrNotSupported );
        ret = KErrNone;
        } 
    
 OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSTUBEXT_DOEXTFUNCL_1,  "TSY: CMmCustomStubExt::DoExtFuncL. ret: %d ", ret );
    return ret; 
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::SupportingIPC
// 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CMmCustomStubExt::SupportingIPC( 
    TInt /*aIpc*/ ) 
    {
     return iFirstTime;  
    } 

// ---------------------------------------------------------------------------
// CMmCustomStubExt::InitializeL
//
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmCustomStubExt::InitializeL( 
    MCustomVendorExtComplete& aMCustomVendorExtComplete ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSTUBEXT_INITIALIZEL_1,  "TSY: CMmCustomStubExt::InitializeL. " );
    
    iVendorExtComplete = &aMCustomVendorExtComplete; 
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::SetMessageRouter
//
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmCustomStubExt::SetMessageRouter( 
    CMmMessageRouter* aMessageRouter )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCUSTOMSTUBEXT_SETMESSAGEROUTER_1,  "TSY: CMmCustomStubExt::SetMessageRouter. " );
    iMessageRouter = aMessageRouter;
    iFirstTime  = ETrue;
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::RegisterNotification
// Notification cannot be registered
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::RegisterNotification( 
    const TInt /*aIpc*/ ) 
    { 
    return KErrNotSupported; 
    }
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::DeregisterNotification
// Because notification cannot be registered, they cannot unregister either
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::DeregisterNotification( 
    const TInt /*aIpc*/ )
    { 
    return KErrNotSupported; 
    }

// ---------------------------------------------------------------------------
// CMmCustomStubExt::ReqModeL
// Dummy implementation for ReqModeL 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmCustomStubExt::ReqModeL( 
    const TInt /*aIpc*/ ) 
    { 
    return 0; 
    }
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::NumberOfSlotsL
// Returns number of slots
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmCustomStubExt::NumberOfSlotsL( 
    const TInt /*aIpc*/ )
    { 
    return 2; 
    }    
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::CancelService
// Cancel services  
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//    
TInt CMmCustomStubExt::CancelService( 
    const TInt /*aIpc*/, 
    const TTsyReqHandle /*aTsyReqHandle*/ )
    { 
    return KErrGeneral; 
    }
    
// ---------------------------------------------------------------------------
// CMmCustomStubExt::Complete
// Completes request  
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//     
TInt CMmCustomStubExt::Complete( TInt /*aReqHandleType*/, 
    TInt /*aError*/, 
    TInt /*aIPC*/ )
    {
    return KErrNotSupported; 
    }    
            
// ---------------------------------------------------------------------------
// CMmCustomStubExt::GetRequiredPlatSecCaps
// Return required platform security caps, EAlwaysPass 
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//              
TSecurityPolicy CMmCustomStubExt::GetRequiredPlatSecCaps( 
    const TInt /*aIpc*/ )
    { 
    return TSecurityPolicy( TSecurityPolicy::EAlwaysPass ); 
    }
        
//  End of File 
