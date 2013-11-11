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




#ifndef HWRMPLUGINHANDLER_H
#define HWRMPLUGINHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "HWRMPluginService.h"
#include "HWRMPluginTransactionList.h"
#include "HWRMGenericTimer.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMService;

// CLASS DECLARATIONS

/**
 *  Callback interface for the server side plugin handler
 */
 class MHWRMIndicationHandler
     {
     public:

         /**
         * Method to return events from plugin handler
         *
         * @param aId  The indication ID
         * @param aData  Data associated with the indication 
         *
         */
         virtual void ProcessIndicationL( const TUint32 aId, TDesC8& aData ) = 0;
     };

/**
* This class contains ongoing plugin operation data.
* It is used as a struct.
*/
class THWRMTransactionData : public THWRMPluginTransactionListItem
    {
    public:
    
	    CHWRMService*     iCompletionCallback; // Pointer to currently using service. Not owned.
	    TTime             iObsoletionTime;     // Time after which the transaction is obsolete

        /**
	    * Convenience constructor
	    */
	    THWRMTransactionData(CHWRMService* aCompletionCallback, 
	                         TUint8 aTransId, 
	                         TInt aCommandId,
	                         const TTime& aObsoletionTime)
	        : THWRMPluginTransactionListItem(aTransId, aCommandId),
	        iCompletionCallback(aCompletionCallback),
	        iObsoletionTime(aObsoletionTime)
	            {
	            };
	            
	    /**
	    * Virtual destructor.
	    */
	    virtual ~THWRMTransactionData()
	        {
	        // iCompletionCallback not owned so not cleaned. Just NULL it to make pc-lint happy.
	        iCompletionCallback = NULL;
	        };
    };
    
/**
*  Hardware Resource Manager server side plugin
*  handler implementation.
*
*/
class CHWRMPluginHandler : public CBase,
                           public MHWRMPluginCallback,
                           public MHWRMGenericTimerCallback
                                 
    {
    public: // Constructors and Destructor
    
        /**
        * Constructor method for instance.
        *
        * @param aMatch Determines the API that the loaded plugin implements.
        * @param aRequestTimeout Timeout in microseconds for requests
        */
        static CHWRMPluginHandler* NewL(const TDesC8& aMatch, TInt aRequestTimeout);

        /**
        * Destructor.
        */
        virtual ~CHWRMPluginHandler();
    
    public: // New functions

        /**
        * Method to invoke a particular command in the plugin.
        * Only one concurrent request is supported, will leave
        * with KErrNotSupported if request is attempted while
        * another is executing
        *
        * @param aCommandId          Command ID
        * @param aData               Data associated with command
        * @param aCompletionCallback Callback for completion.
        *                            If NULL, no callback is attempted.
        * @return Transaction ID (can be used later to cancel)
        *         Returns zero if transaction was already completed.
        */
        TUint8 ProcessCommandL( TInt aCommandId,
                              TDesC8& aData,
                              CHWRMService* aCompletionCallback);

        /**
        * Method to cancel currently executing command
        */
        void CancelCommandL( TUint8 aTransId );

		/**
		* Registers a handler for receiving HWRM plug-in indications
		*
		* @param aCallback  The indication handler to be registered
		*/
        void RegisterForIndicationsL(MHWRMIndicationHandler* aCallback);
        
        /**
		* Deregisters handlers for receiving HWRM plug-in indications
		*
		* @param aCallback  The indication handler to be deregistered		
		*/
		void DeregisterForIndications(MHWRMIndicationHandler* aCallback);	
        
    	// MHWRMPluginCallback callback methods
    	
        /**
        * Method to return data in response to a message from a HWRM plugin.
        *
        * @param aCommandId Command identifier
        * @param aTransId   Unique transcation identifier
        * @param aData      Data returned from call
        *                   Since none of the commands require return value,
        *                   this package contains standard TInt error code.
        */
        void ProcessResponseL( TInt aCommandId,
                               TUint8 aTransId, 
                               TDesC8& aData ); 
        /**
         * Method to return indications from HWRM plugin.
         *
         * @param aIndId An indication ID
         * @param aData  Data associated with the event ID. 
         */
 		void EventL( const TUint32 aIndId, TDesC8& aData );
                               
    	// From MHWRMGenericTimerCallback    	
    	virtual void GenericTimerFired(TInt aTimerId, TBool aCutOff);

    private: 
    
        /**
        * C++ default constructor.
        * @param aRequestTimeout Timeout in microseconds for requests
        */
        CHWRMPluginHandler(TInt aRequestTimeout);
        
        /**
        * This 2nd phase constructor.
        *
        * @param aMatch Matching parameter used to determine correct 
        *               ECom implemetation
        */
        void ConstructL( const TDesC8& aMatch );
        
	private:   // data
	
	    CHWRMPluginService*         iPlugin;           // Reference to plugin
	    TUint8                      iTransIdCounter;   // Counter to create aTransIds
        CHWRMPluginTransactionList*	iTransactionList;  // List of open transactions       
        CHWRMGenericTimer*          iPluginTimer;      // Plugin call timeout timer.
        TTimeIntervalMicroSeconds32 iRequestTimeout;   // Maximum time request can be open.

        RPointerArray<MHWRMIndicationHandler> iIndicationCallbacks; // elements not owned
    };

#endif  // HWRMPLUGINHANDLER_H
            
// End of File
