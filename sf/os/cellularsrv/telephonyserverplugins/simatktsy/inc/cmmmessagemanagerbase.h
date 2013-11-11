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



#ifndef CMMSATMESSAGEMANAGERBASE_H
#define CMMSATMESSAGEMANAGERBASE_H

//  INCLUDES
#include <e32base.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

#include "CSatDataPackage.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CTsyDelegates;
class CSatMessageRouterProxy;
class MmMessageManagerCallback;
class MMessageRouter;

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
            //sat objects
			ESatTsyObjType,
			ESatIconTsyObjType,
			ESatNotificationsTsyObjType,
			ESatNotifySimSessionEndObjType,
			ESatNotifyRefreshObjType,
			ESatNotifyLocalInfoObjType,
			ESatNotifySetUpEventListObjType,
			ESatNotifyMoSmControlRequestObjType,
			ESatNotifyLaunchBrowserObjType,
			ESatNotifyCallControlRequestObjType,
			ESatDataDownloadTsyObjType,
			ESatCCTsyObjType,
			ESatEventDownloadTsyObjType,
			ESatNotifyDisplayTextObjType,
			ESatNotifyGetInkeyObjType,
			ESatNotifyPlayToneObjType,
			CSatNotifySetUpMenu,
			ESatNotifySelectItemObjType,
			ESatNotifySendSmsObjType,
			ESatNotifySendSsObjType,
			ESatNotifySendUssdObjType,
			ESatNotifySetUpIdleModeTextObjType,
			ESatNotifySetUpCallObjType,
			ESatNotifyPollIntervalObjType,
			ESatNotifySendDtmfObjType,
			ESatNotifyPollingOffObjType,
			ESatNotifyTimerMgmtObjType,
			ESatNotifyLanguageNotificationObjType,
            ESatNotifyOpenChannelObjType,
            ESatNotifyGetChannelStatusObjType,
            ESatNotifyCloseChannelObjType,
            ESatNotifyReceiveDataObjType,
            ESatNotifySendDataObjType,
            ESatNotifyMoreTimeObjType,
            ESatNotifyGetInputObjType,
            ESatNotifySetUpMenuObjType,
            EMaxNumOfTsyObjects
            };
            
            
    public:  // Constructors and destructor
        
        /**
         * Creates a new CMmMessageManagerBase object
         *
         *
         * @return reference to created CMmMessageManagerBase type of object
         */
        static CMmMessageManagerBase* NewL(MMessageRouter& aMessageRouter);

        /**
         * Destructor
         */
        virtual ~CMmMessageManagerBase();

        /**
         * Creates and initialises class attributes
         */
        void ConstructL();
  
        
        /**
         * Returns pointer to messagemanagercallback
         *
         *
         * @return pointer to messagemanagercallback object 
         */
        MmMessageManagerCallback* GetMessageManagerCallback();

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

    private:

        /**
         * C++ default constructor.
         */
        CMmMessageManagerBase(MMessageRouter& aMessageRouter);

    protected:  // Data

        /**
         * writing package for Handlers 
         */              
        CSatDataPackage iDataPackage;
        
        /** 
         * Pointer to Messagerouter
         * own
         */         
        CSatMessageRouterProxy* iMessageRouterProxy;
        
        /**
         * Pointer to tsydelegates
         * own
         */
         CTsyDelegates* iTsyDelgates;
         
       
        /**
         * Pointer to messagerouter
         */
         MMessageRouter& iMessageRouter;
         
    };
    
#endif // CMMMESSAGEMANAGERBASE_H

//  End of file
