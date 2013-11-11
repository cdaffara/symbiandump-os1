/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory connection handler
*
*/



#ifndef CACCSRVCONNECTIONHANDLER_H
#define CACCSRVCONNECTIONHANDLER_H

// INCLUDE FILES
#include "AccClientServerMessages.h"
#include "AccSrvHandlerBase.h"
#include "AccSrvQueueObserver.h" // MQueueObserver
#include <AccPolGenericID.h>
#include <AccPolValueRecord.h> //TAccPolValueType

// CONSTANTS

// MACROS

// DATA TYPES
struct TAccSrvRequestData
    {
    TAccPolNameRecord iName; //GetValue operation is made for this name
    TInt iTrId; //Corresponding transaction id
    TBool iUpdateNotification; //response for accessory update notification
    };

enum TConnectionHandlerStates
{
	EGetSelectionFromUser = 0,
	EInitializeConnection,
	EWaitForUINotifier
};


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAccSrvConnectionControllerObserver;
class CAccPolAccessoryPolicy;
class CAccSrvASYProxyHandler;
class CAccPolicyDB;
class CAccSrvServerModel;

// CLASS DECLARATION

/**
 * Accessory connection and disconnection related tasks are handled here.
 *
 * @lib AccServer.lib
 * @since S60 3.1
 */
NONSHARABLE_CLASS( CAccSrvConnectionHandler ) : public CAccSrvHandlerBase,
                                                public MQueueObserver
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CAccSrvConnectionHandler* NewL( MAccSrvConnectionControllerObserver* aCallback,
        									   CAccSrvServerModel * aModel,	
                                               CAccPolAccessoryPolicy* aPolicy );

        /**
         * Destructor.
         */
        virtual ~CAccSrvConnectionHandler();

    public: // New functions

        /**
         * Starts Accessory connection handling asynchronously.
         * @since S60 3.1
         * @param aGenericID Generic ID in question.
         * @param aEvaluateConnectionRules If true evaluate connection rules
         * @param aUpdateConnection Indicates new connection or update for existing connection
         * @return void
         */
        void StartConnectionHandlingL( TAccPolGenericID& aGenericID, 
                                       TBool aEvaluateConnectionRules, 
                                       TBool aUpdatedConnection );
        
        /**
         * Get connection handling generic id unique id
         * @since S60 3.1
         * @return Unique id for connection handler
         */
        TInt UniqueIDForConnectionHandler();
        
        /**
         * Cancel connection handling
         * @since S60 3.1
         * @return void
         */
        void CancelConnectionHandling();

        /**
         * Resets all values. After completion this handler is 
         * available for handing next connection
         * @since S60 3.1
         * @return void 
         */
        void ResetHandler();

        /**
         * Indicates is this handler available for connection handling
         * @since S60 3.1
         * @return EFalse if currently handling a connection, otherwise ETrue
         */
        TBool IsFree();

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * From CActive
        * Handles an active object's request completion event.
        * Asynchronous connection handling is processed.
        *
        * @since S60 3.1
        * @return void
        */
        void RunL();

        /**
        * From CActive
        * The active scheduler calls this function if this active object's
        * RunL() function leaves.
        *
        * @since S60 3.1
        * @param aError The leave code
        * @return KErrNone, when error has been handled
        */
        TInt RunError(TInt aError);

        /**
        * From MQueueObserver
        * Provides empty definition for base class pure virtual declaration.
        */
        TBool CompleteMessageL( TMsgID aMsgID,
                                TInt aErrorCode,
                                TInt aUniqueID );

        /**
        * From MQueueObserver
        * Provides empty definition for base class pure virtual declaration.
        */
        TBool CompleteProcessCommandL( TProcessCmdId aCmdn,
                                       TInt aTrId,
                                       TASYCommandParamRecord& aASYCommandParamRecord,
                                       TPtr8* aPtrBuf=NULL );

        /**
        * From MQueueObserver
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        * Critical capabilities are queried from ASY during accessory connection.
        * response from ASY for these queries are handled here.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
        void HandleValueMessageL( TMsgID aMsgID,
                                  TAccValueTypeTBool aValue,
                                  TInt aErrorCode,
                                  TInt aTrId );

        /**
        * From MQueueObserver
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        * Critical capabilities are queried from ASY during accessory connection.
        * response from ASY for these queries are handled here.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
         void HandleValueMessageL( TMsgID aMsgID,
                                   TAccValueTypeTInt aValue,
                                   TInt aErrorCode,
                                   TInt aTrId );

        /**
        * From MQueueObserver
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        * Critical capabilities are queried from ASY during accessory connection.
        * response from ASY for these queries are handled here.
        *
        * @since S60 3.1
        * @param aMsgID       Message in question.
        * @param aCmdResponse Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
         void HandleValueMessageL( TMsgID aMsgID,
                                   TDesC8& aValue,
                                   TInt aErrorCode,
                                   TInt aTrId );                                          
        /**
        * From MQueueObserver
        * Called when Queue passes message completion handling to registered observers.
        * This is meant for GetValue() and SetValue() type of operations.
        * Critical capabilities are queried from ASY during accessory connection.
        * response from ASY for these queries are handled here.
        *
        * @param aMsgID       Message in question.
        * @param aPtrBuf      Value of the operation, used in GetValue()
        *                     type of operations.
        * @param aErrorCode   Error Code of the operation.
        * @param aTrId        Transaction Id of the operation.
        * @return void
        */
         void HandleValueMessageL( TMsgID aMsgID,
                                   TInt aTrId,
                                   TPtr8* aPtrBuf,
                                   TInt aErrorCode );                                          
    private:

        /**
        * Constructor with notify handling.
        */
        CAccSrvConnectionHandler( MAccSrvConnectionControllerObserver* aCallback,
        						  CAccSrvServerModel * aModel,
                                  CAccPolAccessoryPolicy* aPolicy );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Initializes critical capabilities and supported BT-profilies.
        */
        TBool InitializeCriticalCapsL();

        /**
        * Sends GetValue-command towards ASY.
        */
        void StartASYCommandHandlingL( TAccPolValueType aValueType,
                                       TASYCommandParamRecord& aASYCommandParam );

        /**
        * Stores name and transaction ID while ASY query is ongoing.
        * During response handling name in question can be retrieved from here
        * according to the transaction Id with FindRequestData() method.
        */
        void StoreRequestDataL( TAccPolNameRecord aName, 
                               TInt aTrId, 
                               TBool aUpdateConnectionNotification );

        /**
        * Find stored request data. See StoreRequestData()
        */
        TInt FindRequestData( TInt aTrId );

        /**
        * Common HandleValueMessageL() implementation for TAccValueTypeTInt and
        * TAccValueTypeTBool types.
        */
        void HandleValueMessagesL( TMsgID aMsgID,
                                   TInt aValue,
                                   TInt aErrorCode,
                                   TInt aTrId );

        /**
        * Generic ID is updated according supported profiles received from BT-ASY.
        */
        void ResolveGenericIDL( TDesC8& aValue );

        /**
        * Common capabilities for all profiles are updated here.
        */
        void ResolveGidL();

        /**
        * Headset Profile (HSP) specific parts of Generic ID are updated.
        */
        void ResolveHSPSpecificGidL( TUint16 aSupportedFeatures );

        /**
        * Handsfree Profile (HFP) specific parts of Generic ID are updated.
        */
        void ResolveHFPSpecificGidL( TUint16 aSupportedFeatures );

        /**
        * Audio Video Remote Control Profile (AVRCP) specific parts of Generic ID are updated.
        */
        void ResolveAVRCPSpecificGidL( TUint16 aSupportedFeatures );

        /**
        * Advanced Audio Distribution Profile (A2DP) specific parts of Generic ID are updated.
        */
        void ResolveA2DPSpecificGidL( TUint16 aSupportedFeatures );
                
        /**
        * Update KAccSetVolumeLevel capability value.
        */
        void UpdateSetVolumeLevelCapabilityL( TInt32 aCapability );
           
        /**
        * Setup selection dialog with selection list
        */
        void SetupSelectionDialogL( TUint32 aSelectionListBitmask);

        /**
        * Open selection dialog with selection list
        */
        void OpenSelectionDialogL();
		
		

    public:     // Data

    protected:  // Data

    private:    // Data

        //Generic ID is stored here during connection handling asynchronous operatrion(s)
        TAccPolGenericID                iGenericID;

        //Handle to the Accessory Policy
        CAccPolAccessoryPolicy*         iPolicy;//Not owned

        //Name and corresponding Transaction ID for each ASY request is stored here.
        RArray<TAccSrvRequestData>      iRequestDataArray;
        
        //pointer to server model
        CAccSrvServerModel * iModel;
        
        //connection handler state...
        TConnectionHandlerStates iState;
        
		//user selection reply     
	  	TInt iReplyValue;
		
		//user selection reply package    	
		TPckg<TInt> iReplyPck;
	
		//ui notifier
		RNotifier iNotifier;
		
		//is connection handling update for old connection
		TBool iConnectionUpdate;
        
		//selection bitmask
        TInt iSelectionListBitmask;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

};

#endif      // CACCSRVCONNECTIONHANDLER_H

// End of File
