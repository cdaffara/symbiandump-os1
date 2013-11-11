/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of CSuplPsyCellIdHandler.
*
*/

#ifndef SUPLPSYCELLIDHANDLER_H
#define SUPLPSYCELLIDHANDLER_H

// INCLUDES
#include <e32def.h>
#include <e32base.h>
#include <etelmm.h>

#include "suplpsycellidinfo.h"

/**
*  Observer class for listening to Etel Events
*  @lib nlacidsuplpsy.lib
*  @since S60 v3.1
*/

class MBasicCellIDListener
	{
	public:
		// Cell Id Information & Error while completing request
		virtual void BasicCellIDRequestCompletedL(TInt aErrorCode) = 0;

	};

/**
*  Class for getting the Cell Id information	
*  @lib nlacidsuplpsy.lib
*  @since S60 v3.1
*/

class CSuplPsyCellIdHandler : public CActive
    {
   
    public: 
    
        enum TNetworkTypeInfo
			{
			EGSM = 0,			
			EWCDMA,
			EUnknown
			};
    
    public:  
        
   	   /**
        * Performs construction of an object
        * @since S60  v3.1
        * @param None
        * @return None
        */
   		static CSuplPsyCellIdHandler* NewL(MBasicCellIDListener& aObserver);
   		
        /**
        * Performs destructor
        * @since S60  v3.1
        * @param None
        * @return None
        */
        ~CSuplPsyCellIdHandler();
        
        /**
        * Retrieves cell id information
        * @since S60  v3.1
        * @param None
        * @return None
        */        
        void GetCellID( TCellIdInfo& aCellIdInfo );
        
       /**
        * Returns type of network
        * @since S60  v3.1
        * @param None
        * @return Network type
        */        
        TNetworkTypeInfo NetworkType() const;
        
     private:
     
       /**
        * Handles the cell id information
        * @since S60  v3.1
        * @param None
        * @return None
        */        
     	void HandleCellInfoL();
     	
     	 /**
        * Reads cell ID stored in Central Repository
        * @since S60  v3.1
        * @param None
        * @return None
        */        
     	void ReadCurrentCellIdFromCRL();

     	 /**
        * Save Current cell ID in Central Repository
        * @since S60  v3.1
        * @param None
        * @return None
        */        
     	void SaveCurrentCellIdToCRL();     	
     	
     private:
     
       /**
        * Performs construction of an object
        * @since S60  v3.1
        * @param aObserver, MBasicCellIDChecker observer for request completion.
        * @return None
        */
        CSuplPsyCellIdHandler(MBasicCellIDListener& aObserver);

     	/**
        * Performs Second phase constrcution
        * @since S60  v3.1
        * @param None
        * @return None
        */
        void ConstructL();

		/**
		* ConnectToETelL, Connects to Telephony server & RMobilePhone
		* @since S60 v3.1u     
		* @param      None, 
		* @return     TInt, returns system wide error code
		*/
		TInt  ConnectToETelL();

    protected:  // CActive        
    	 /**
        * Performs cancelling of asynchronour request
        * @since S60  v3.1
        * @param None
        * @return None
        */ 
        void DoCancel();

        /**
        * Performs RunL
        * @since S60  v3.1
        * @param None
        * @return None
        */ 
        void RunL();  
        
        /**
        * Performs RunError
        * @since S60  v3.1
        * @param aError, Error code containing leave reason
        * @return TInt , informs to activescheduler about handling of leave.
        */ 
        TInt RunError(TInt aError);  
        
       
    private:    // Data
  
      	//For access to a mobile phone
        RMobilePhone iPhone; 
        
        // Etel Server
		RTelServer iTelServer;
		
		//TSYName
		TFileName iTsyName; 
        
        // For Network information 
        RMobilePhone::TMobilePhoneNetworkInfoV1 	iNetworkInfo;
        
        //for network information
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPkg;
        
     	 // For location information
        RMobilePhone::TMobilePhoneLocationAreaV1 iLocationInfo;

        //Observer for request completion
        MBasicCellIDListener& iObserver;

		TNetworkTypeInfo iNetworkType;
						
		TBool iIsInitilized;
		
		TCellIdInfo* iCellIdInfo;
    };

#endif      // SUPLPSYCELLIDHANDLER_H
            
// End of File
