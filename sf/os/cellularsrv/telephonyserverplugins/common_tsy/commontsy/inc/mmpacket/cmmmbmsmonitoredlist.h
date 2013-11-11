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



#ifndef CMmMBMSMonitoredList_H
#define CMmMBMSMonitoredList_H

//  INCLUDES
#include <etelpckt.h>
#include <pcktlist.h>
#include "Cmmpacketservicetsy.h"
#include "mbmstypes.h"



// CLASS DECLARATION

/**
*  CMmMBMSMonitoredList contains MBMS context related functionality. 
*/
NONSHARABLE_CLASS( CMmMBMSMonitoredList ) : public CBase
    {
    public: // Constructor and destructor    

        /**
         * NewL method is used to create a new instance of CMmMBMSMonitoredList
         * class.         
         *                  
         * @param aMmPacketService Pointer to the Packet Service object
         * @param aMaxEntries Maximum number of monitored entries
         */
        static CMmMBMSMonitoredList* NewL
                (
                CMmPacketServiceTsy* aMmPacketService,
                TInt aMaxEntries
                );

        /**
         * Destructor
         */
        ~CMmMBMSMonitoredList();
       
        /**
         * Handles all actions
         * 
         * @param aList a List of new entries
         * @param aAction the action to take
         * @return TInt KErrNone or a system error code               
         */
        TInt ProcessEntriesL( const CPcktMbmsMonitoredServiceList *aList, TMbmsAction aAction );        

        /**
         * Returns number of monitored lists.
         *
         * @return The number of the monitored lists
         */ 
        TInt NumberOfMonitoredList();
       
         /**
         * Save changes to temp list
         * @param aList NULL for an empty list or a pointer to a descriptor that hold the read stream used to restore the list from
         * @param aAction the action to take place
         *
         * @return KErrNone if success or error code  
         */  
         TInt SaveUpdateValuesToTempL( TDes8* aList, TMbmsAction aAction );        
 
 		 /**
         * Pack data to send to the LTSY  
         *  
         * @return KErrNone if success or error code  
         */ 
 		 TInt PackDataL();  
 		 
 	     /**
         * This function add the succesfully added monitor entries to main monitor service list
         * @param aFailedMonitorEntries a list of not added monitor service entries
         * @return KErrNone if success or error code  
         */
 		 TInt AddTempEntriesToCacheL(const CPcktMbmsMonitoredServiceList* aFailedMonitorEntries);
        
        /**
         * This function sets the availability status of the service list 
         * 
         * @param aList a list of services that should be set to available (the rest will be unavailable)
         */
        void SetServicesAvailabilityL(CPcktMbmsMonitoredServiceList *aAvailableList);
        
        
        /**
         * This function sets return a pointer to the service list 
         * 
         * @return a pointer to the service list
         */
	    CPcktMbmsMonitoredServiceList* ServiceList();
			
		/**
         * This function clears the temporary monitor servicelist 
         */
		void ResetTempListL();
        
    private: // New functions
      
        /** 
         * C++ default constructor.
         */
        CMmMBMSMonitoredList();
    
        /**
         * Class attributes are created in ConstructL.
         * 
         * @param aMmPacketService Pointer to the Packet Service object
         * @param aMaxEntries Maximum number of monitored entries
         */
        void ConstructL(CMmPacketServiceTsy* aMmPacketService, TInt aMaxEntries);
        
        /**
         * Initialise tsy req handle store.
         */
        void InitModulesL();
                
        /**
         * Adds entries from given list to monitor list
         *
         * @param CPcktMbmsMonitoredServiceList Items to check 
         * @return TInt Possible error value
         */
        TInt AddEntriesL( const CPcktMbmsMonitoredServiceList *aList );        
                        
        /**
         * Remove one or more entries from existing service list.
         *
         * @param CPcktMbmsMonitoredServiceList List not to clear
         */
        void DeleteEntriesL( const CPcktMbmsMonitoredServiceList *aList );        
        
        /**
         * Method checks if there are already existing entries in main list. If any
         * found duplicate entries are returned.
         *
         * @param aEntry Items to check 
         * @return ETrue if such an entry exists 
         */
        TBool EntryExistL( const RPacketService::TMbmsServiceAvailabilityV1 &aEntry );               
        
        /**
         * Since monitor list does not support clear, this function clears existing
         * list
         *
         * @param CPcktMbmsMonitoredServiceList List to clear
         */
        void ClearListL( CPcktMbmsMonitoredServiceList *aList );
        
    private: // Data
    
                 
         /**
         * actual service list
         */ 
        CPcktMbmsMonitoredServiceList *iServiceList;
        
        
        /**
         *  Pointer to the Packet Service TSY
         *  Not own.        
         */
        CMmPacketServiceTsy* iMmPacketService;  
        
              
       	/**
         * Structure contains Update MBMS related data 
         */
         TMmMbmsMonitorServiceListParams iMbmsMonitoredListData;
         
	         
    };

#endif // CMmMBMSMonitoredList_H
   
// End of File
