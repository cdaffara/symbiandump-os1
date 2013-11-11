// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology 
*/

#ifndef BATTERYMEASUREMENTSAO_H_
#define BATTERYMEASUREMENTSAO_H_

// INCLUDES
#include <e32base.h>
#include "HWRMPowerClientServer.h"
#include "HWRMClient.h"

// FORWARD DECLARATIONS
class RHWRMClient;

class CHWRMBatteryMeasurementsAO : public CActive 
	{
	
public:  
	virtual ~CHWRMBatteryMeasurementsAO();
public:  
   /**  
 	*      	 
 	* Issue an asynchronous request to hwrm server to retrieve 
 	* measurements.    	     	 
 	* When hwrm server completes the request, the (derived) active object will 
 	* be signalled and its RunL() will be invoked, which in turn will notify
 	* the battery observer    	       
 	*/         	    
    virtual void GetAsyncBatch(TInt aCommand) = 0;
    
    //Interface for changing the callback pointer.
    //TAny* is used, since callbacks are not derived from common base class.
    virtual void SetCallBack(TAny* aCallBack);
    
   /**  
 	*      	 
 	* Issue a synchronous request to the hwrm server to cancel 
 	* any outstanding GetMeasurements requests.
 	*/     
    void SendCancel(TInt aCommand);

#ifdef SYMBIAN_HWRM_EXTPOWERINFO 
    //If measurements are stopped by HWRM (because of charger disconnected,...)
    //complete the callback with KErrCancel.
    virtual void CancelClientNotification();
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
protected:

	CHWRMBatteryMeasurementsAO(RHWRMClient& aClient); 
 	
    TUint iCount;
	TPckg<TUint> iCountPckg;
    RHWRMClient& iClient;
    
	};

template<typename T>
NONSHARABLE_CLASS(CHWRMBatteryMeasurementsAttributeAO):public CHWRMBatteryMeasurementsAO
        {
        typedef TFixedArray<T,20> TMeasurementsArray;
        public:

        CHWRMBatteryMeasurementsAttributeAO(RHWRMClient& aClient);
        /**  
        *        
        * Issue an asynchronous request to hwrm server to retrieve 
        * measurements.                  
        * When hwrm server completes the request, the (derived) active object will 
        * be signalled and its RunL() will be invoked, which in turn will notify
        * the battery observer                 
        */                  
        void GetAsyncBatch(TInt aCommand); 
        
        protected:
        TMeasurementsArray iResultsArray;//accessed in derived classes
        
        private:
        TPckg<TMeasurementsArray> iMeasurementsArrayPckg;

        };


#endif /*BATTERYMEASUREMENTSAO_H_*/
