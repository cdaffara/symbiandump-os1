/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @publishedPartner
 @released
*/

#ifndef CMMCUSTOMVENDOREXT_H
#define CMMCUSTOMVENDOREXT_H

//  INCLUDES
#include <et_phone.h>
#include <featmgr/featmgr.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

// FORWARD DECLARATIONS
class MCustomVendorExtComplete;

// CLASS DECLARATION

/**
*  CMmCustomVendorExt is used to provide an extension interface for  
*  implementing vendor specific IPCs.
*
*/
class CMmCustomVendorExt : public CBase
    {
    public:
    
        /**
         * Destructor.
         */
        virtual ~CMmCustomVendorExt() 
            { 
            return; 
            }

        /**
         * Initialises the vendor extension. 
         *
         * @param aMCustomVendorExtComplete reference to request complete
         */
         virtual void InitializeL( 
            MCustomVendorExtComplete& /*aMCustomVendorExtComplete*/ )
            {
            }
        
        /**
         * SupportingIPC
         *          
         * @param aIpc IPC number of request
         * @return EFalse
         */
        virtual TBool SupportingIPC( TInt /*aIpc*/ ) 
            { 
            return EFalse; 
            }

        /**
         * DoExtFuncL
         *          
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Contains parameters for request
         * @return KErrNotSupported
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle /*aTsyReqHandle*/, 
            const TInt /*aIpc*/, 
            const TDataPackage& /*aPackage*/ ) 
            { 
            return KErrNotSupported; 
            }
            
        /**
         * RegisterNotification
         *          
         * @param aIpc IPC of the notification request
         * @return KErrNotSupported
         */
        virtual TInt RegisterNotification( const TInt /*aIpc*/ ) 
            { 
            return KErrNotSupported; 
            }
                
        /**
         * DeregisterNotification
         *          
         * @param aIpc IPC of the notification request
         * @return KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt /*aIpc*/ )
            { 
            return KErrNotSupported; 
            }        

        /**
         * ReqModeL
         *          
         * @param aIpc IPC of the request
         * @return 0
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt /*aIpc*/ ) 
            { 
            return 0; 
            }
            
        /**
         * NumberOfSlotsL
         *          
         * @param aIpc IPC of the request
         * @return 2
         */
        virtual TInt NumberOfSlotsL( const TInt /*aIpc*/ )
            { 
            return 2; 
            }

        /**
         * CancelService
         *          
         * @param aIpc IPC of the request to be cancelled
         * @param aTsyReqHandle Request handle
         * @return KErrGeneral
         */
        virtual TInt CancelService( const TInt /*aIpc*/, 
            const TTsyReqHandle /*aTsyReqHandle*/ )
            { 
            return KErrGeneral; 
            }

        /**
         * Complete completes due to timer expiration
         *          
         * @param aReqHandleType Req handle type
         * @param aError Completion value
         * @param aIPC IPC number
         * @return KErrNotSupported
         */
        virtual TInt Complete( TInt /*aReqHandleType*/, 
            TInt /*aError*/, 
            TInt /*aIPC*/ )
            { 
            return KErrNotSupported; 
            }

        /**
         * GetRequiredPlatSecCaps
         *         
         * @param aIpc IPC number
         * @return TSecurityPolicy::EAlwaysFail
         */
        virtual TSecurityPolicy GetRequiredPlatSecCaps( const TInt /*aIpc*/ )
            { 
            return TSecurityPolicy( TSecurityPolicy::EAlwaysFail ); 
            }

        /**
         * Constructor.
         */
        CMmCustomVendorExt() 
            {
            };

    };

#endif // CMMCUSTOMVENDOREXT_H

//  End of File

