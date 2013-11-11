// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMMCUSTOMSTUBEXT_H
#define CMMCUSTOMSTUBEXT_H

//  INCLUDES
#include <ctsy/serviceapi/cmmcustomvendorext.h>
#include <ctsy/serviceapi/mcustomvendorextcomplete.h>

// FORWARD DECLARATIONS
class CMmMessageRouter;

// CLASS DECLARATION

/**
 * CMmCustomVendorExt is used to provide an extension interface for 
 * implementing vendor spesific IPCs.
 *
 */
class CMmCustomStubExt : public CMmCustomVendorExt
    {
    public:
    
        /**
         * Destructor.
         */
        virtual ~CMmCustomStubExt();

        /**
         * InitializeL
         *          
         * @param aMCustomVendorExtComplete Reference to vendor completion
         */
        virtual void InitializeL( 
            MCustomVendorExtComplete& aMCustomVendorExtComplete ); 
        
        /**
         * DoExtFuncL
         *          
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Contains parameters for request
         * @return KErrNotSupported
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );
            
        /**
         * SupportingIPC
         *          
         * @param aIpc IPC to verify
         * @return EFalse
         */
        virtual TBool SupportingIPC( TInt /*aIpc*/ ); 
    
        /**
         * RegisterNotification
         *          
         * @param aIpc IPC of the notification request
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt /*aIpc*/ ); 
                
        /**
         * DeregisterNotification
         *          
         * @param aIpc IPC of the notification request
         * @return KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt /*aIpc*/ );
      
        /**
         * ReqModeL
         *          
         * @param aIpc IPC of the request
         * @return 0
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt /*aIpc*/ ); 
            
        /**
         * NumberOfSlotsL
         *          
         * @param aIpc IPC of the request
         * @return 2
         */
        virtual TInt NumberOfSlotsL( const TInt /*aIpc*/ );

        /**
         * CancelService
         *          
         * @param aIpc IPC of the request to be cancelled
         * @param aTsyReqHandle Request handle
         * @return KErrGeneral
         */
        virtual TInt CancelService( const TInt /*aIpc*/, 
            const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Complete completes due to timer expiration
         *          
         * @param aReqHandleType Req handle type
         * @param aError Completion value
         * @param aIPC IPC number
         * @return KErrNotSupported
         */
        virtual TInt Complete( TInt /*aReqHandleType*/, 
            TInt /*aError*/, TInt /*aIPC*/ );

        /**
         * GetRequiredPlatSecCaps
         *          
         * @param aIpc IPC number
         * @return TSecurityPolicy(TSecurityPolicy::EAlwaysFail)
         */
        virtual TSecurityPolicy GetRequiredPlatSecCaps( const TInt /*aIpc*/ );
        
        /**
         * SetMessageRouter
         *          
         * @param aMessageRouter Pointer to messagerouter
         */
        void SetMessageRouter( CMmMessageRouter* aMessageRouter );
        
        /**
         * Constructor.
         */
        CMmCustomStubExt(); 
    
    private:    // data
        
        /*
         * Pointer to Messagerouter
         * Own.
         */                  
        CMmMessageRouter* iMessageRouter;
        
        /*
         * iFirstTime
         */         
        TBool iFirstTime;
        
        /*
         * Pointer to custom extension vendor completion.
         */ 
        MCustomVendorExtComplete* iVendorExtComplete; 
        
    };
    

#endif // CMMCUSTOMSTUBEXT_H

//  End of File
