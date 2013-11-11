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



#ifndef CMMMESSAGEMANAGERBASE_H
#define CMMMESSAGEMANAGERBASE_H

//  INCLUDES
#include <e32base.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagerouterproxy.h"
#include <ctsy/pluginapi/mmessagerouter.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CTsyDelegates;

// CLASS DECLARATION

/**
 *  Forwards Tsy requests from Symbian OS to Domestic OS
 */
class CMmMessageManagerBase : public CBase
    {
    public:
        
        /** TTsyObjects */
       enum TTsyObjects
            {
            EPhoneTsy,
            ELineTsy,
            ECallTsy,
            EFaxTsy,
            ESmsMessagingTsy,
            EBroadcastMessagingTsy,
            EUssdMessagingTsy,
            EPhoneStoreTsy,
            ESmsStoreTsy,
            ENamStoreTsy,
            EONStoreTsy,
            EENStoreTsy,
            EPhoneBookStoreTsy,
            EConferenceCallTsy,
            EPacketDataTsy,
            ESatTsy,
            ECustomTsy,
            EDtmfTsy,
            ESecurityTsy,
            //ECustomExtTsy

            EMaxNumOfTsyObjects
            };
            
    public:  // Constructors and destructor
        
        /**
         * Creates a new CMmMessageManagerBase object
         *
         *
         * @return reference to created CMmMessageManagerBase type of object
         */
        IMPORT_C static CMmMessageManagerBase* NewL();

        /**
         * Destructor
         */
        virtual ~CMmMessageManagerBase();

        /**
         * Creates and initialises class attributes
         */
        void ConstructL();
        
        /**
         * Stores Phonetsy pointer
         *          
         *
         * @param aMmPhone pointer to Phonetsy
         */
        IMPORT_C void SetPhoneTsy( CMmPhoneTsy* aMmPhone );
        
        /**
         * Returns pointer to messagemanagercallback
         *
         *
         * @return pointer to messagemanagercallback object 
         */
        IMPORT_C MmMessageManagerCallback* GetMessageManagerCallback();

    public:  // New functions
    
        /**
         * Forwards a request to DOS layer, request doesn't contain data
         *          
         *
         * @param aIpc
         * @return error value
         */
        virtual TInt HandleRequestL( 
            const TInt aIpc );

        /**
         * Forwards a request to DOS layer, request contains 
         * packaged data by Common TSY
         *          
         *
         * @param aIpc
         * @param aDataPackage
         * @return error value
         */
        virtual TInt HandleRequestL( 
            const TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
         * Forwards a request to DOS layer, request contains 
         * packaged data by Etel Server.
         *          
         *
         * @param aIpc
         * @param aDataPackage
         * @return error value
         */
        virtual TInt HandleRequestL(    
            const TInt aIpc,
            const TDataPackage* aDataPackage );

        /**
         * Registers a Tsy object for request and notification completions
         *          
         *
         * @param aTsyObjectType
         * @param aTsyObject
         */
        virtual void RegisterTsyObject( TTsyObjects aTsyObjectType, 
            CBase* aTsyObject );

        /**
         * Deregisters a Tsy object for request and notification completions
         *          
         *
         * @param aTsyObject
         */
        virtual void DeregisterTsyObject( CBase* aTsyObject );

        /** 
         * Sets the message router object
         *              
         *
         * @param aMessageRouter
         */
        IMPORT_C void SetMessageRouter( MMessageRouter* aMessageRouter );
        
    private:

        /**
         * C++ default constructor.
         */
        CMmMessageManagerBase();

    protected:  // Data

        /**
         * writing package for Handlers 
         */              
        CMmDataPackage iDataPackage;
        
        /** 
         * Pointer to Messagerouter
         * own
         */         
        CMmMessageRouterProxy* iMessageRouterProxy;
        
        /**
         * Pointer to tsydelegates
         * own
         */
         CTsyDelegates* iTsyDelgates;
         
        /**
         * Pointer to phonetsy
         * not own
         */
         CMmPhoneTsy* iMmPhone;
         
        /**
         * Pointer to messagerouter
         */
         MMessageRouter* iMessageRouter;
         
    };
    
#endif // CMMMESSAGEMANAGERBASE_H

//  End of file
