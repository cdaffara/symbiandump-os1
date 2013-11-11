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

#ifndef HWRMBATTERYCHARGETIMEMEASUREMENTSAO_H_
#define HWRMBATTERYCHARGETIMEMEASUREMENTSAO_H_

#include "HWRMBatteryMeasurementsAO.h"
#include "HWRMPower.h"

// FORWARD DECLARATIONS
// None

/**
* Active object wrapper
*
*/
NONSHARABLE_CLASS(CHWRMBatteryChargeTimeMeasurementsAO) : public CHWRMBatteryMeasurementsAttributeAO<TInt>
    {
    public:        
        	    
        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMBatteryChargeTimeMeasurementsAO.
        *
        * @param aCallback Pointer to a callback instance.
        * @return A pointer to a new instance of the 
        *  CHWRMBatteryChargeTimeMeasurementsAO class.
        */
        static CHWRMBatteryChargeTimeMeasurementsAO* NewL(MHWRMBatteryFullChargingTimeObserver* aCallback, RHWRMClient& aClient);

        /**
        * Destructor.
        */
        virtual ~CHWRMBatteryChargeTimeMeasurementsAO();  


        //If measurements are stopped by HWRM (because of charger disconnected,...)
        //complete the callback with KErrCancel.
        virtual void CancelClientNotification();

        /**
        * Set the callback.
        * This can be used to change the callback.
        */

        virtual void SetCallBack(TAny* aCallBack);
        
    protected:

        // From CActive
        void RunL();
        void DoCancel();

    private:

        /**
        * Constructor
        *
        * @param aCallback Pointer to a callback instance.
        */
    	CHWRMBatteryChargeTimeMeasurementsAO(MHWRMBatteryFullChargingTimeObserver* aCallback, RHWRMClient& aClient);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();           	
    	
    private:        
        
        MHWRMBatteryFullChargingTimeObserver*     iCallback;// not owned by this class.         
    };

#endif /*HWRMBATTERYCHARGETIMEMEASUREMENTSAO_H_*/
//End of File    
