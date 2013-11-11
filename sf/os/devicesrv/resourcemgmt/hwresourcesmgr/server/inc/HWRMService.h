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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMSERVICE_H
#define HWRMSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMPluginTransactionList.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMPluginHandler;
class CHWRMReservationHandler;

// CLASS DECLARATION

/**
* Data storage class for plugin requests.
*/
class THWRMPluginRequestData : public THWRMPluginTransactionListItem
    {
public:
    RMessage2   iRequestMessage; // Can have NULL handle for timer triggered requests.
    TBool       iSplitCommand; // Indicates if command was split to two adaptation calls.
    TInt        iSplitCommandId; //Id of the command to be executed next.
    TAny*       iSplitCommandData; //Owned. Data associated with split command.

    /**
    * Convenience constructor
    */
    THWRMPluginRequestData(const RMessage2& aRequestMessage, TUint8 aTransId, TInt aCommandId, 
                           TBool aSplitCommand, TInt aSplitCommandId=0, TAny* aSplitCommandData=NULL)
        : THWRMPluginTransactionListItem(aTransId, aCommandId),
          iRequestMessage(aRequestMessage), iSplitCommand(aSplitCommand), iSplitCommandId(aSplitCommandId), iSplitCommandData(aSplitCommandData)
            {
            };
    /**
    * Virtual destructor.
    */
    virtual ~THWRMPluginRequestData()
        {
        };
    };

/**
*  Hardware Resource Manager server side service objects base class.
*  Each session has one service object.
*
*/
class CHWRMService : public CBase
    {
public: // New functions
	/**
	* This function has to be implemented by the derived class.
	* CHWRMSession calls this method to handle plugin related messages.
	*
	* @param aMessage Service message to be completed in ProcessResponseL.
	* @return ETrue if completion is needed for message, EFalse otherwise.
	*/
	virtual TBool ExecuteMessageL(const RMessage2& aMessage)=0;
	
	/**
	* This function has to be implemented by the derived class.
	* Callback method for plugin handlers to notify this service
	* that request has been processed. 
	* This method is not called in case request is canceled.
	*
	* @param aCommandId The command id for which the response came
	* @param aTransId Related transaction Id
	* @param aData Response data
	* @param aTimeout Request was canceled by timeout. aData parameter
	*                 is empty descriptor if aTimeout is ETrue.
	*/
	virtual void ProcessResponseL( TInt aCommandId, 
	                               TUint8 aTransId, 
	                               TDesC8& aData,
	                               TBool aTimeout )=0;
	
    /**
    * Method to suspend the use of subresource of service.
    * If service only has one subresource, whole service is suspended.
    *
    * @param aSubResource Index of the subresource that was suspended. 
    *         Note that this is always a single subresource.
    *         Index 0 corresponds to subresource 0x1,
    *         index 1 corresponds to subresource 0x2,
    *         index 2 corresponds to subresource 0x4,
    *         index 3 corresponds to subresource 0x8, etc.
    */
    virtual void SuspendSubResource(TInt aSubResource) = 0;
    
    /**
    * Method to resume suspended resource.
    *
    * @param aSubResource Index of the subresource that was resumed. 
    *         Note that this is always a single subresource.
    *         Index 0 corresponds to subresource 0x1,
    *         index 1 corresponds to subresource 0x2,
    *         index 2 corresponds to subresource 0x4,
    *         index 3 corresponds to subresource 0x8, etc.
    */
    virtual void ResumeSubResource(TInt aSubResource) = 0;
    
    /**
    * Method for notifying activation/deactivation of reserved subresource
    *
    * @param aSubResource Index of the subresource that was activated/deactivated.
    *         Note that this is always a single subresource.
    *         Index 0 corresponds to subresource 0x1,
    *         index 1 corresponds to subresource 0x2,
    *         index 2 corresponds to subresource 0x4,
    *         index 3 corresponds to subresource 0x8, etc.
    * @param aActivate Indicates if resource was activated or deactivated
    */
    virtual void ActivateSubResource(TInt aSubResource, TBool aActivate) =0;

        	    	
protected: // Construction

    /**
    * BaseConstructL must be called by subclasses ConstructL methods.
    *
    * @param aPluginHandler The plugin handler to use with this service.
    * @param aReservationHandler The reservation handler to use with this service.
    */
    void BaseConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);

    /**
    * BaseDestruct must be called by subclasses destructors.
    */
    void BaseDestruct();
    
    /**
    * CompleteRequest can be called from ProcessResponseL to 
    * complete any outstanding request and remove transaction
    * from transaction list.
    *
    * @param aTransId Transaction identifier
    * @param aCompletionCode Code used to complete message
    * @return Error code if the transaction is not found.
    */
    TInt CompleteRequest(TUint8 aTransId, TInt aCompletionCode);
    
    /**
        * CompleteRequest can be called from ProcessResponseL to 
        * complete any outstanding request and remove transaction
        * from transaction list.
        *
        * @param aTransId Transaction identifier
        * @param aCompletionCode Code used to complete message
        * @param aTransactionList List which contains transaction to remove
        * @param aCompleteMsg Truth value used for checking whether 
        *        message should be completed or not.
        * @return Error code if the transaction is not found.
        */
        TInt CompleteRequest(TUint8 aTransId, TInt aCompletionCode, 
            CHWRMPluginTransactionList*	aTransactionList, TBool aCompleteMsg);
        
        /**
    * Checks that no transaction for this message still remains in transaction list
    *
    * @param aHandle Message handle.
    * @return ETrue if list still contains transaction data for checked message.
    */
    TBool CheckForMessage(TInt aHandle);
    
protected: // data
    
    CHWRMPluginHandler* iPluginHandler;  // Reference to plugin handler, not owned.
                                         // Subclasses must set this in ConstructL
    
    CHWRMPluginTransactionList*	iTransactionList;  // List of open transactions       
    
    CHWRMReservationHandler* iReservationHandler;  // Reservation handler, not owned
    };

#endif  // HWRMSERVICE_H
            
// End of File
