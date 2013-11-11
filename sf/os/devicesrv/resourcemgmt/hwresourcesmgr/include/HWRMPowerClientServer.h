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

#ifndef HWRMPOWERCLIENTSERVER_H
#define HWRMPOWERCLIENTSERVER_H

// INCLUDES
// None
#include <hwrmpower.h>

// CONSTANTS
// Maximum number of possible power targets
const TInt KHWRMPowerMaxTargets(1);   

// MACROS
// None

// DATA TYPES
// None

// Opcodes used in message passing between client and server
// Power opcodes start at 3000
enum THWRMPowerServRequest
    {
	// Services for Power
	
	/**
	* Gets the Battery consumption info
	* 
	* parameters:
	* 0: TPckg<TBatteryConsumptionData> On return, the consumption info
	*/
    EHWRMGetBatteryInfo = 3000,
	
	/**
	* Gets the Baterry voltage info
	* 
	* parameters:
	* 0: TPckg<TBatteryVoltageData> On return, the voltage info
	*/
    EHWRMGetBatteryVoltageInfo,
	

    /**
    * Starts the periodic reporting of avegage battery Power value
    *
    * parameters:
    * 0: TUint intervalMultiple
    */
    EHWRMStartAverageBatteryPowerReporting,

    /**
    * Get a batch of average battery power measurements from the server
    *
    * parameters:
    * None
    * 
    * return:
    * 0: TMeasurementsArray
    */


    EHWRMGetPowerMeasurements,
     
    /**
    * Stops the periodic reporting of avegage battery Power value
    *
    * parameters:
    * None
    */
    EHWRMStopAverageBatteryPowerReporting,


    /**
     * Cancel an average battery power measurements request
     *
    * parameters:
    * None
     */

    EHWRMCancelAveragePowerReporting,

    
    /**
    * Cleans up the powewr service so that session can be closed.
    *
  	* parameters:
  	* None
    */
    EHWRMCleanupBattery,

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

    /**
    * Get Battery Charger Type
    *
    */

    EHWRMGetBatteryChargerType,

    /**
	* Starts the periodic reporting of average charging current value(period = TIntervalMultiple * BaseTimeInterval)
	* 
	* parameters:
	* 0: TIntervalMultiple
	*/
    EHWRMStartAverageChargingCurrentReporting,

    /**
    * Get a batch of average charging current measurements from the server
    */
    EHWRMGetAverageChargingCurrentMeasurements,

     /**
    * Stops the periodic reporting of avegage charging current value
    *
	* parameters:
	* None
    */
    EHWRMStopAverageChargingCurrentReporting,

    /**
     * Cancel an average battery voltage measurements request
     *
   	* parameters:
   	* None
     */
    EHWRMCancelAverageChargingCurrentReporting,

    /**
	* Starts the reporting of Battery Full charging time.
	*/
    EHWRMStartBatteryFullChargingTimeReporting,

    /**
    * Get a batch of remaing charging time measurements.
    */
    EHWRMGetBatteryFullChargingTimeMeasurements,

     /**
    * Stops the reporting of Battery Full charging time.
    *
	* parameters:
	* None
    */
    EHWRMStopBatteryFullChargingTimeReporting,

    /**
     * Cancel remaining charging time measurements request
     *
   	* parameters:
   	* None
     */
    EHWRMCancelBatteryFullChargingTimeReporting,

#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    EHWRMEndOfPowerOpCodes
    };


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

#endif  // HWRMPOWERCLIENTSERVER_H
            
// End of File
