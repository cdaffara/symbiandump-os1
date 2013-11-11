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
* Name        : csatmessagerouterproxy.h
* Part of     : Common TSY / commonsimatktsy
* Interface   : Internal, Common TSY Plugin API
* Routes between Tsy objects and Message Handler objects
* Version     : 1.0
*
*/



#ifndef CSATMESSAGEROUTERBASE_H
#define CSATMESSAGEROUTERBASE_H


//  INCLUDES
#include <e32base.h>
#include "cmmmessagemanagerbase.h"

//#include "CMmMessageRouterBase.h"

#include <ctsy/pluginapi/mmmessagemanagercallback.h>

// FORWARD DECLARATIONS
class MMmMessHandlerBase;
class CSatDataPackage;
class CMmPhoneTsy;
class CMmMessageManager;
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CTsyDelegates;

/**
*  Routes between Tsy objects and Message Handler objects
*  
*  @lib (CommonSimAtkTsy)
*  @since 3.1
*/
class CSatMessageRouterProxy : public CBase, public MmMessageManagerCallback
    {

    // public methods
    public:

        /**
        * Creates a new CSatMessageRouterBase object
		* @param aMessageManager: Pointer to the message manager object
		* @param aRouter: Pointer to the CommonTsy routerbase object
        * @return reference to created CSatMessageRouterBase type of object
        */
        static CSatMessageRouterProxy* NewL( 
			CTsyDelegates& aTsyDelegates );

        /**
        * Destructor
        */
        virtual ~CSatMessageRouterProxy( void );

    protected:

        /**
        * C++ Default constructor
        * @param aRouter: Pointer to the CommonTsy routerbase object
        */
        CSatMessageRouterProxy(CTsyDelegates& aTsyDelegates );

    private: // Constructors

        /**
        * Class attributes are created in ConstructL 
        */
        void ConstructL( void );

    public:

        /**
        * Returns pointer to the message handler for this 
		* request, based on IPC number
		* @param aIpc: Request IPC
		* @return Message handler objact
        */
        virtual MMmMessHandlerBase* RouteRequest( TInt /*aIpc*/ );

        /**
        * Completes a request or notification via the specific Tsy object,
		* based on IPC number and data package
		* @param aIpc: Request IPC number
		* @param aDataPackage: The data needed for the request
		* @param aResult: Possible error value
        */
        virtual void RouteCompletion( TInt aIpc, 
            CSatDataPackage* aDataPackage, TInt aResult );
            
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

   			
    // private methods
    private:

		/**
		* Returns the pointer to the Tsy object needed for completion, based 
		* on the IPC number and the data package
		* @param aIpc: Request IPC number
		* @param aDataPackage: Data package
		* @return Tsy object
		*/
        CBase* GetObjectForCompletion( const TInt aIpc, 
            CSatDataPackage* aDataPackage );

        /**
		* Returns the type of Tsy object needed for completion, based 
		* on the IPC number
		* @param aIpc: Request IPC
		* @return Type of object
		*/
		CMmMessageManagerBase::TTsyObjects GetTsyObjectFromIpc( 
            const TInt aIpc ) const; 
    
        /**
        * Completes a request or notification via the specific Tsy object,
		* based on IPC number and data package
		* @param aIpc: Request IPC number
		* @param aDataPackage: The data needed for the request
		* @param aResult
        */
        void RouteCompletionL( TInt aIpc, CSatDataPackage* aDataPackage, 
            TInt aResult );

    private:    // Data unowned

        CTsyDelegates& iTsyDelegates;

    };


#endif // CSatMessageRouterBase_H

//  End of file
