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

#ifndef CMMMOCKMESSHANDLER_H
#define CMMMOCKMESSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "mmmmesshandlerbase.h"

//  FORWARD DECLARATIONS
class CMmMessageRouter;
class CMmCustomStubExt;

// CLASS DECLARATION

/**
 * Dummy functionality as Stub.
 *
 */
class CMmMockMessHandler : public CBase, public MMmMessHandlerBase
    {
    public:  // Constructors and destructor
           
        /**
         * Two-phased constructor.
         * @param aMessageRouter Pointer to message router.
         */
        static CMmMockMessHandler* NewL( 
            CMmMessageRouter* aMessageRouter );   
            
        /**
         * Destructor.
         */
        virtual ~CMmMockMessHandler();

        /**
         * ExtFuncL
         *          
         * @param aIpc
         * @param aDataPackage
         * @return Error value
         */
        virtual TInt ExtFuncL( TInt aIpc,
            const CMmDataPackage* aDataPackage );
     
        /**
         * Returns Network info needed in boot
         *          
         */
        void GetHomeNetworkInfo();

    protected: 
         
        /**
         * C++ default constructor.
         */
        CMmMockMessHandler();
         
        /**
         * Initialises object attributes.
         *          
         * @param aMessageManager
         */
        void ConstructL( CMmMessageRouter* aMessageManager );
        
        /**
         * Call back method for periodic timer
         *          
         * @param aThis
         */
        static TInt TimerCallback(TAny* aThis );

    private:
        
        /**
         * GetHomeNetwork
         *      
         */
        void GetHomeNetwork();

    private:    // Data
        
        /*
         * Pointer to the Message Manager
         * Own.
         */                  
        CMmMessageRouter* iMessageRouter;

        /*
         * Network info store
         */         
        RMobilePhone::TMobilePhoneNetworkInfoV1 iMyNetworkInfo;
        
        /*
         * Periodic timer asynhronous completion
         * Own.         
         */         
        CPeriodic* iTimer;
        
    };

#endif // CMMMOCKMESSHANDLER_H

// End of File
