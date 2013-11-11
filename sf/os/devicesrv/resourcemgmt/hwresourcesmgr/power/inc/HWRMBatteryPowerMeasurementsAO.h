// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


/** @file
@internalTechnology */

#ifndef HWRMBATTERYPOWERMEASUREMENTSAO_H
#define HWRMBATTERYPOWERMEASUREMENTSAO_H

#include "HWRMBatteryMeasurementsAO.h"
#include "HWRMPower.h"

// FORWARD DECLARATIONS
// None

/**
* Active object wrapper
*
*/
NONSHARABLE_CLASS(CHWRMBatteryPowerMeasurementsAO) : public CHWRMBatteryMeasurementsAttributeAO<CHWRMPower::TBatteryPowerMeasurementData>
    {
    public:        
        	    
        /**
        * This is a two-phase constructor method that is used to
        * create a new instance of CHWRMBatteryVoltageMeasurementsAO.
        *
        * @param aCallback Pointer to a callback instance.
        * @return A pointer to a new instance of the 
        *  CHWRMBatteryPowerMeasurementsAO class.
        */
        static CHWRMBatteryPowerMeasurementsAO* NewL(MHWRMBatteryPowerObserver* aCallback, RHWRMClient& aClient);

        /**
        * Destructor.
        */
        virtual ~CHWRMBatteryPowerMeasurementsAO();   

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
    	CHWRMBatteryPowerMeasurementsAO(MHWRMBatteryPowerObserver* aCallback, RHWRMClient& aClient);
                                                   
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();           	
    	
    private:        
        
        MHWRMBatteryPowerObserver*     iCallback;// not owned by this class.         
    };

#endif /*HWRMBATTERYPOWERMEASUREMENTSAO_H*/
//End of File    
