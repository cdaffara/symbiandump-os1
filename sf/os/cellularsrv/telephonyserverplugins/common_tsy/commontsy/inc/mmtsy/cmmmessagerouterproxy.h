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



#ifndef CMMMESSAGEROUTERPROXY_H
#define CMMMESSAGEROUTERPROXY_H

#include <e32base.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>

// FORWARD DECLARATIONS
class MMmMessHandlerBase;
class CTsyDelegates;
class CMmPhoneTsy;

// CLASS DECLARATION

/**
 *  Routes between Tsy objects and Message Handler objects
 */
class CMmMessageRouterProxy : public CBase, public MmMessageManagerCallback
    {
    public:  // Constructors and destructor

        /**
         * Creates a new CMmMessageRouterProxy object
         *         
         *
         * @param aTsyDelegates reference to TsyDelegates
         * @return Pointer to CMmMessageRouterProxy object
         */
        static CMmMessageRouterProxy* NewL( 
            CTsyDelegates& aTsyDelegates ); 
            
        /**
         * Destructor
         */
         ~CMmMessageRouterProxy();
        
        /**
         * Creates and initialises class attributes
         */
        void ConstructL();
          
    public:
    
        /**
         * Returns pointer to the message handler for this 
         * request, based on IPC number
         *
         * @param aIpc Ipc of request
         * @return pointer to MMmMessHandlerBase
         */
        MMmMessHandlerBase* RouteRequest( TInt aIpc );

        /**
         * Completes a request or notification via the specific Tsy object,
         * based on IPC number and data package
         *              
         *
         * @param aIpc Ipc of request
         * @param aDataPackage Data package
         * @param aResult Result value
         */
        void RouteCompletion( TInt aIpc, 
            CMmDataPackage* aDataPackage, TInt aResult );
            
        /**
         * Sets the pointer to PhoneTsy (Common TSY)
         *          
         *
         * @param aMmPhone Pointer to PhoneTsy
         */
        void SetPhoneTsy( CMmPhoneTsy* aMmPhone );
        
        /**
         * Completes a request or notification via the specific Tsy object,
         * based on IPC number and data package
         *              
         *
         * @param aIpc Ipc of request
         * @param aDataPackage Data package
         */
        void RouteCompletionL( TInt aIpc,
            CMmDataPackage* aDataPackage, TInt aResult );
         
        /**
         * Completes requests from LTSY
         *
         *
         * @param aIpc Ipc of request
         * @param aDataPackage Data from LTSY
         * @param aResult Result value
         */
        void Complete( TInt aIpc, CMmDataPackage* aData, TInt aResult );
        
        /**
         * Completes requests from LTSY
         *
         *
         * @param aIpc Ipc of request
         * @param aResult Result value
         */            
        void Complete( TInt aIpc, TInt aResult );
         
    private:
        
        /**
         * C++ default constructor.
         */
        CMmMessageRouterProxy();
                      
    protected:     // Data

        /** 
         * Pointer to Tsy delegates 
         */                      
        CTsyDelegates* iTsyDelegates;

        /** 
         * pointer to Phone Tsy             
         */             
        CMmPhoneTsy* iMmPhone;
        
    };
    

#endif // CMMMESSAGEROUTERPROXY_H

//  End of file
