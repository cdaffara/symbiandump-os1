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

#ifndef HWRMBATTERYCHARGECURRENTRATEAO_H_
#define HWRMBATTERYCHARGECURRENTRATEAO_H_

#include "HWRMBatteryMeasurementsAO.h"
#include "HWRMPower.h"

// FORWARD DECLARATIONS
// None

/**
* Active object wrapper
*
*/
NONSHARABLE_CLASS(CHWRMBatteryChargeRateCurrentAO) : public CHWRMBatteryMeasurementsAttributeAO<TInt>
    {
    public:        
        	    
        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMBatteryChargeRateCurrentAO.
        *
        * @param aCallback Pointer to a callback instance.
        * @return A pointer to a new instance of the 
        *  CHWRMBatteryChargeRateCurrentAO class.
        */
        static CHWRMBatteryChargeRateCurrentAO* NewL(MHWRMBatteryChargingCurrentObserver* aCallback, RHWRMClient& aClient);

        /**
        * Destructor.
        */
        virtual ~CHWRMBatteryChargeRateCurrentAO(); 

        /**
        * Set the callback.
        * This can be used to change the callback.
        */
        virtual void SetCallBack(TAny* aCallBack);

        //If measurements are stopped by HWRM (because of charger disconnected,...)
        //complete the callback with KErrCancel.
        virtual void CancelClientNotification();
        
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
    	CHWRMBatteryChargeRateCurrentAO(MHWRMBatteryChargingCurrentObserver* aCallback, RHWRMClient& aClient);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();           	
    	
    private:        
        
        MHWRMBatteryChargingCurrentObserver*     iCallback;// not owned by this class.         
    };

#endif /*HWRMBATTERYCHARGECURRENTRATEAO_H_*/
//End of File    
