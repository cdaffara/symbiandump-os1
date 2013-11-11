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

#ifndef SATMESSAGING_H
#define SATMESSAGING_H

// INCLUDES
#include <e32base.h>
#include <ctsy/serviceapi/ctsysatmessagingbase.h>

// FORWARD DECLARATIONS
class CMmMessageRouter;

// CLASS DECLARATION

/**
 * Sat Messaging dummy class.
 *
 */
class CTsySatMessaging : public CTsySatMessagingBase
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *            
         * @param aRouter Pointer to LTSY Message Router
         */
        IMPORT_C static CTsySatMessaging* NewL( 
            CMmMessageRouter* aRouter );

        /**
         * Destructor.
         */
        virtual ~CTsySatMessaging();
        
        /**
         * From MStkTsySatService     
         * Complete SendSmsMessage request
         *          
         * @param aError error value
         * return error value
         */
        TInt CompleteSendSmsMessage( TInt aError );
        
        /**
         * From MStkTsySatService     
         * Is the MoSmControl activated by Sim.
         *          
         * return The status of control activated
         */
        TBool IsMoSmControlBySimActivated();

        /**
         * ExtFunc
         *          
         * @param aTsyReqHandle Tsy request handle
         * @param aIpc Ipc number of request
         * @param aPackage Datapackage
         */
        virtual TInt ExtFunc( 
            const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );
                    
    private:

        /**
         * C++ default constructor.
         */  
        CTsySatMessaging(
            CMmMessageRouter* aRouter );
              
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
    
    private: // data
    
        /**
         * Pointer to message router object.
         */
        CMmMessageRouter* iMessageRouter;
        
    };

#endif  // SATMESSAGING_H

// End of File
