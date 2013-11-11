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

#ifndef HWRMPLUGINTRANSACTIONLIST_H
#define HWRMPLUGINTRANSACTIONLIST_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATIONS

/**
* This class contains ongoing plugin operation data base class
* It is used as a struct.
*/
class THWRMPluginTransactionListItem
    {
    public:
    
	    TUint8                    iTransId;            // Ongoing transaction ID
	    TInt                      iCommandId;          // Command that is being handled.
	    THWRMPluginTransactionListItem* iNextData;     // Next data in the queue. Not owned.

        /**
	    * Convenience constructor
	    */
	    THWRMPluginTransactionListItem(TUint8 aTransId, TInt aCommandId)
	        :iTransId(aTransId),
	        iCommandId(aCommandId),
	        iNextData(NULL)
	            {
	            };

	    /**
	    * Virtual destructor.
	    */
	    virtual ~THWRMPluginTransactionListItem()
	        {
	        // iNextData not owned so not cleaned
	        iNextData = NULL;
	        };
    };
    
/**
*  Hardware Resource Manager server side plugin
*  transaction list handler
*
*/
class CHWRMPluginTransactionList : public CBase
    {
    public: // Constructors and Destructor
    
        /**
        * Constructor method for instance.
        */
        static CHWRMPluginTransactionList* NewL();

        /**
        * Destructor.
        * Deletes all data instances still in the list, even though normally 
        * instances are deleted by using class. This is because after
        * list destruction there is no way to find the instances to delete any more.
        */
        virtual ~CHWRMPluginTransactionList();
    
    public: // New functions

        /**
        * Finds a transaction from transaction list.
        *
        * @param aTransId   Unique transcation identifier
        * @param aRemove    If ETrue, transaction will be removed from the list.
        * @return Transaction data found or NULL if not found.
        */
        THWRMPluginTransactionListItem* FindTransaction( TUint8 aTransId, TBool aRemove);
        
        /**
        * Add transaction to transaction list
        *
        * @param aTransactionData Transaction data to add to list
        */
        void AddTransaction( THWRMPluginTransactionListItem* aTransactionData );
        
        /**
        * Removes first item from list. 
        * 
        * @return Pointer to removed item
        */
        THWRMPluginTransactionListItem* RemoveFirstItem();
        
        /**
        * Get first transaction data item in the list
        * That item can be then used to iterate further.
        *
        * @return First item in the list or NULL if empty.
        */
        inline THWRMPluginTransactionListItem* GetFirstItem();
        
        /**
        * Get count of transactions in list
        */
        inline TInt Count() const;    
         
    private: 
    
        /**
        * C++ default constructor.
        */
        CHWRMPluginTransactionList();
        
        
	private:   // data
	
	    THWRMPluginTransactionListItem* iTransactionData;      // This is valid only while transaction is list
	                                                           // Contains pointer to next data in list.
	                                                           // Creation and deletion of data is normally
	                                                           // responsibility of list user, but 
	                                                           // but all instances still listed will be deleted
	                                                           // in list destructor.
	    THWRMPluginTransactionListItem* iTransactionDataLast;  // Last transaction data in list
	                                                           
	    
	    TInt iCount;      // Number of transactions in list
	    	    
    };
    

#include "HWRMPluginTransactionList.inl"

#endif  // HWRMPLUGINTRANSACTIONLIST_H
            
// End of File
