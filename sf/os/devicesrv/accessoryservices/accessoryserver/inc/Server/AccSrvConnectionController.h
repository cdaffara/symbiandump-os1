/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory Connection Controller
*
*/



#ifndef CACCSRVCONNECTIONCONTROLLER_H
#define CACCSRVCONNECTIONCONTROLLER_H

//  INCLUDES
#include "AccSrvNotificationQueue.h"
#include "AccSrvConnectionHandler.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TAccPolGenericID;
class CAccPolAccessoryPolicy;
class CAccSrvServerModel;
class CAccSrvNotificationQueue;
class CAccSrvConnectionStatusHandler;
class CAccSrvModeHandler;
class CAccSrvASYProxyHandler;
class CAccSrvSettingsHandler;
class CAccSrvWiredConnectionPublisher;

// CLASS DECLARATION

/**
*  Interface class to be used from Handlers to Connection Controller
*  for call-backs.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( MAccSrvConnectionControllerObserver )
    {
    public:

        /**
        * Called when connection status changed handling should be started.
        * @since S60 3.1
        * @return void
        */
        virtual void HandleConnectionStatusChangedL() = 0;

        /**
        * Called when accessory mode changed handling should be started.
        * @since S60 3.1
        * @param aDbId Unique identifier of a Generic ID. If not exist,
        *              Accessory Mode is updated according to the currecnt
        *              Combined Accessory Connection Status.
        * @param aAudioOutputStatus Current audio routing status.
        *                           Used when aDbId exists.
        * @return void
        */
        virtual void HandleAccessoryModeChangedL( TInt aDbId = KErrUnknown, 
                                                  TBool aAudioOutputStatus = EFalse ) = 0;

        /**
        * Called when all critical capabilities are initialized and connection
        * handling should proceed.
        * @since S60 3.1
        * @param aGenericId A Generic ID of the connected accessory.
        * @param aCaller    A handle to the caller of this call-back method.
        *                   Used to delete the caller, because all tasks for that
        *                   handler are done.
        * @param aError     Error code for connect validation. Connection handling
        *                   will continue with KErrNone, otherwise connection is denied.
        * @return void
        */
        virtual void HandleConnectValidationL( const TAccPolGenericID& aGenericId,
                                               const CAccSrvConnectionHandler* aCaller,
                                               TInt aError ) = 0;

        /**
        * Handle possible single connection and multiple connection status.
        * changes after a connection update
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return void
        */
		virtual void HandleConnectionUpdateValidationL( const TAccPolGenericID& aGenericID, 
												TAccPolGenericID& aOldGenericId, 
												const CAccSrvConnectionHandler* aCaller,
												TInt aError  ) = 0;

        /**
        * Cancel connection handling 
        *
        * @since S60 3.1
        * @return void
        */
		virtual void ConnectionHandlingCancel( const CAccSrvConnectionHandler* aCaller) = 0;


        /**
        * Interface to queue. Allows Handler to register messages to queue.
        * @since S60 3.1
        * @param aMsgID    Message to which the observer is registered to.
        * @param aCallback Handle to observer.
        * @return void
        */
        virtual void RegisterControlMessageL( TMsgID aMsgID,
                                              MQueueObserver* aCallback ) = 0;

        /**
        * Interface to queue. Allows Handler to cancel messages from queue.
        * @since S60 3.1
        * @param aMsgID    Message to which the observer is registered to.
        * @param aCallback Handle to observer.
        * @return void
        */
        virtual void CancelControlMessage( TMsgID aMsgID,
                                              MQueueObserver* aCallback ) = 0;

        /**
        * Interface to ASY handler. Allows Handler to send commands to ASY.
        * @since S60 3.1
        * @param aCmdId Defines the ASY command in question.
        * @param aASYCommandParamRecord Data for ASY command.
        * @param aPtrBuf Pointer to CBufFlat buffer where CAccPolObjectCon object
        *                is externalized to.
        * @return void
        */
        virtual TInt HandleASYCommsL( TProcessCmdId aCmdId,
                                      TASYCommandParamRecord& aASYCommandParamRecord,
                                      TPtr8* aPtrBuf=NULL ) = 0;

        /**
        * Interface to ServerModel. Allows Handler to access Server Model.
        * @since S60 3.1
        * @return void
        */
        virtual CAccSrvServerModel& ServerModelHandle() = 0;
        
        
        /**
        * Set default accessory information so mode handler can show default 
        * accessory selected information note when accessory mode is changed
        *
        * @since S60 3.1
        * @param aGenericID Generic ID information note to be shown
        * @param aDefaultValue Default selection value
        * @return void
        */
		virtual void SetDefaultAccessoryInformation( const TAccPolGenericID& aGenericID, 
													 const TUint32 aDefaultValue) = 0;

    };

/**
*  Connection Controller handles accessory connection related tasks.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvConnectionController ) : public MAccSrvConnectionControllerObserver
    {
    public  :// Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvConnectionController* NewL( CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        virtual ~CAccSrvConnectionController();

    public: // New functions

        /**
        * Start to handle accessory connection according to given Generic ID
        *
        * @since S60 3.1
        * @param aGenericId      Generic ID of the connected accessory.
        * @param aCallerThreadID ConnectAccessory was called from this thread.
        * @param aEvaluateConnectionRules If true evaluate connection rules
        * @param aUpdateConnection Indicates new connection or update for existing connection
        * @return void
        */
        void HandleConnectL( TAccPolGenericID& aGenericId,
                             TUint aCallerThreadID, 
                             TBool aEvaluateConnectionRules,
                             TBool aUpdateConnection );

        /**
        * Handle possible single connection and multiple connection status.
        * changes after a change of critical capability value.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return void
        */
        void HandleConnectionChangeValidationL( const TAccPolGenericID& aGenericID );

        /**
        * Start to handle accessory disconnection according to given Generic ID
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the disconnected accessory
        * @return void
        */
        void HandleDisconnectL( const TAccPolGenericID& aGenericId );

        /**
        * Get handle to Server Model.
        * @since S60 3.1
        * @return void
        */
        CAccSrvServerModel& ServerModel() const;

        /**
        * Get handle to Notification Queue.
        * @since S60 3.1
        * @return void
        */
        CAccSrvNotificationQueue& NotificationQueue() const;

        /**
        * Get handle to ASY Proxy Handler.
        * @since S60 3.1
        * @return void
        */
        CAccSrvASYProxyHandler& ASYProxyHandler() const;

        /**
        * Get handle to Settings Handler.
        * @since S60 3.1
        * @return void
        */
        CAccSrvSettingsHandler& SettingsHandler() const;

        /**
        * Get handle to Accessory Policy.
        * @since S60 3.1
        * @return void
        */
        CAccPolAccessoryPolicy& Policy() const;

        /**
        * Accessory Connection Status and Accessory Mode asynchronous client
        * requests are completed. Used when ASYs are loaded.
        * @since S60 3.1
        * @return void
        */
        void InitializeConnectionStatusL();

    public: // Functions from base classes

        /**
        * From MAccSrvConnectionControllerObserver
        */
        void HandleConnectionStatusChangedL();

        /**
        * From MAccSrvConnectionControllerObserver
        */
        void HandleAccessoryModeChangedL( TInt aDbId = KErrUnknown,
                                          TBool aAudioOutputStatus = EFalse );

        /**
        * From MAccSrvConnectionControllerObserver
        */
        void HandleConnectValidationL( const TAccPolGenericID& aGenericId,
                                       const CAccSrvConnectionHandler* aCaller,
                                       TInt aError );

        /**
        * From MAccSrvConnectionControllerObserver
        */
		void HandleConnectionUpdateValidationL( const TAccPolGenericID& aGenericID, 
												TAccPolGenericID& aOldGenericId,
												const CAccSrvConnectionHandler* aCaller, 
												TInt aError  );
		

		void HandleConnectionUpdateValidationL( const TAccPolGenericID& aGenericID, 
                                                TAccPolGenericID& aOldGenericId,
		                                        const CAccSrvSettingsHandler* aCaller, 
		                                        TInt aError  );
       
        
        /**
        * From MAccSrvConnectionControllerObserver
        */
		void ConnectionHandlingCancel( const CAccSrvConnectionHandler* aCaller);        


        /**
        * From MAccSrvConnectionControllerObserver
        */
        void RegisterControlMessageL( TMsgID aMsgID,
                                      MQueueObserver* aCallback );

        /**
        * From MAccSrvConnectionControllerObserver
        */
        void CancelControlMessage( TMsgID aMsgID,
                                      MQueueObserver* aCallback );

        /**
        * From MAccSrvConnectionControllerObserver
        */
        TInt HandleASYCommsL( TProcessCmdId aCmdId,
                              TASYCommandParamRecord& aASYCommandParamRecord,
                              TPtr8* aPtrBuf=NULL );

        /**
        * From MAccSrvConnectionControllerObserver
        */
       CAccSrvServerModel& ServerModelHandle();

        /**
        * From MAccSrvConnectionControllerObserver
        */
		void SetDefaultAccessoryInformation( const TAccPolGenericID& aGenericID, 
													 const TUint32 aDefaultValue);

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccSrvConnectionController();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CAccSrvServerModel* aServerModel );

        /**
        * Change accessory connection status to Connected.
        */
        void PromoteToConnectedL( const TAccPolGenericID& aGenericID );

        /**
        * Change accessory connection status to Detected.
        */
        void LowerToDetectedL( const TAccPolGenericID& aGenericID );

    public:     // Data

    protected:  // Data

    private:    // Data

        CAccPolAccessoryPolicy*                 iPolicy;                 //Owned
        CAccSrvServerModel*                     iServerModel;            //Not Owned
        CAccSrvNotificationQueue*               iNotificationQueue;      //Owned
        CAccSrvConnectionStatusHandler*         iConnectionStatusHandler;//Owned
        CAccSrvModeHandler*                     iModeHandler;            //Owned
        CAccSrvASYProxyHandler*                 iASYProxyHandler;        //Owned
        CAccSrvSettingsHandler*                 iSettingsHandler;        //Owned
        RPointerArray<CAccSrvConnectionHandler> iConnectionHandler;      //Owned

	    TUint32                                 iInformationNoteUID;	  
	    TUint32                                 iInformationNoteDefault;

        CAccSrvWiredConnectionPublisher*        iWiredConnPublisher; // Owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
  };

#endif      // CACCSRVCONNECTIONCONTROLLER_H

// End of File
