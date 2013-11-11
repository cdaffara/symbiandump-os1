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

#ifndef CMMMESSAGEROUTER_H
#define CMMMESSAGEROUTER_H

//  INCLUDES
#include <ctsy/mltsyfactory.h>
#include <ctsy/pluginapi/mmessagerouter.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/ctsysatmessagingbase.h>
#include "cmmmockmesshandler.h"

// FORWARD DECLARATIONS
class CMmMockMessHandler;
class CTsySatMessaging;

// CLASS DECLARATION

/**
 * Routes between Tsy objects and Message Handler objects
 *
 */
class CMmMessageRouter: public CBase, 
	public MLtsyFactoryV1, public MMessageRouter
    {        
    public:    // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aMessageManager
         */      
        static CMmMessageRouter* NewL();

        /**
         * Destructor
         */
        virtual ~CMmMessageRouter();

        // New functions
    
        /**
         * Returns pointer to the message handler for this 
         * request, based on IPC number
         *          
         * @param aIpc IPC number of request
         * @return Pointer to the message handler
         */
        virtual MMmMessHandlerBase* RouteRequest( TInt aIpc );   

        /**
         * Returns pointer to message manager callback
         *          
         * @return Pointer to the message manager callback
         */
        MmMessageManagerCallback* MessageManager();
        
        /**
         * From MLtsyFactoryV1
         * Returns pointer to message router interface
         *          
         * @param aMessageManager Message manager callback
         * @return Pointer to the message router
         */
        MMessageRouter* GetMessageRouter( 
            MmMessageManagerCallback& aMessageManager );


        /**
         * Returns pointer to SAT messaging base
         *          
         * @return Pointer to the SAT messaging service
         */
        CTsySatMessagingBase* GetSatMessagingService();  

        /**
         * From MLtsyFactoryV1
         * Returns pointer to SAT messaging base
         *          
         * @param aSatMessagingService SAT service
         * @return Pointer to the SAT messaging service
         */        
        CTsySatMessagingBase* GetSatMessagingService(MCtsySatService &aSatMessagingService);  
        
        /**
         * From MLtsyFactoryBase
         * Returns interface version used by the LTSY 
         *          
         * @return Interface version
         */
        MLtsyFactoryBase::TCtsyInterfaceVersion Version();
        
        /**
         * From MLtsyFactoryBase
         * Release the created LTSY object(s)
         *          
         */
        void Release(); 
        
        /**
         * From MMessageRouter
         * Directs requests to LTSY
         * 
         * @param aIpc ipc number of request
         * @param aDataPackage data to LTSY
         * @return error value
         */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );
        
    private:
        
        /**
         * C++ default constructor
         */
        CMmMessageRouter();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Initialises needed store(s) and timer(s).
         *          
         */
        void InitL();

        /**
         * Initialises messagehandlers modules.
         *          
         */
        void InitModulesL();

    protected:// Data
        
        /**
         * Pointer to mock message handler
         * Own.         
         */ 
        CMmMockMessHandler* iMmMockMessHandler;
        
    private:    // Data

        /**
         * Pointer to Call Message manager
         * Own.         
         */         
        MmMessageManagerCallback* iMessageManager;
        
        /**
         * Pointer to SAT service provider
         * Own.         
         */
        CTsySatMessagingBase* iSatService;

    };


#endif // CMMMESSAGEROUTER_H

//  End of File
