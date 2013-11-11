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
// This file contains the following sections:
// - Command definitions for HWRM Power plugin API
// - Command parameter definitions for HWRM Power plugin API
// - Response parameter definitions for HWRM Power plugin API
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HWRMPOWERCOMMANDS_H_
#define HWRMPOWERCOMMANDS_H_


// Include plugin service so that including only this header is necessary to use Power plugin API.
#include <hwrm/hwrmpluginservice.h>

// CONSTANTS

// Identifies the ECom implementation to be used.
_LIT8(KHWRMPowerApiEComMatch, "HWRMPower");


/**
*  Plugin interface class just encapsulates the Power command enumerations.
*  Responsible for defining the command IDs for each message.
*/
namespace HWRMPowerCommand
    {
    /** typedef'd package of TInt containing the error code response. */
    typedef TPckgBuf<TInt> TErrorCodeResponsePackage;

    // Data packages for responses
    
    /**
	*
	* @publishedPartner
	* @released
	*/
	struct TBatteryVoltageResponseData 
		{
		TInt iErrorCode;
		TInt iRemainingVoltage;
		TInt iMaximumVoltage;
		TInt iMinimumVoltage;
		};
	 /** data response package for EGetBatteryVoltageInfoCmdId */
	typedef TPckgBuf<TBatteryVoltageResponseData> TBatteryVoltageResponsePackage;
		
    /**
	*
	* @publishedPartner
	* @released
	*/
	struct TBatteryConsumptionResponseData 
	 	{
		TInt iErrorCode;
	 	TInt iNominalCapacity;
	 	TInt iRemainingCapacity;
	 	TInt iRemainingPercentageCapacity;
	 	};
	/** data response package for EGetBatteryInfoCmdId */
	typedef TPckgBuf<TBatteryConsumptionResponseData> TBatteryConsumptionResponsePackage;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
    enum TBatteryChargerType 
        { 
        EBatteryChargerUnKnown = 0, 
        EBatteryChargerDedicated = 1, 
        EBatteryChargerUsbDedicated = 2,
        EBatteryChargerUsbHost = 3,
        EBatteryChargerUsbHispeedHost = 4,
        EBatteryChargerUsbAca = 5,
        };

    struct TBatteryChargerTypeData
        {
        TInt iErrorCode;
        TBatteryChargerType iChargerType;
        };
    
    typedef TPckgBuf<TBatteryChargerTypeData> TBatteryChargerTypeResponsePackage;
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
	
    /**
    * Power(Current and Voltage) Measurement Report structure.
    * @publishedPartner
    * @released
    */
    struct TBatteryPowerReport 
    {
        TInt iErrorCode;
        TInt iAverageVoltage;
        TInt iAverageCurrent;
    };

    /** data package send for measurements indication EAverageBatteryPowerIndicationId */
    typedef TPckgBuf<TBatteryPowerReport> TBatteryPowerReportPackage;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
    struct TBatteryChargeCurrentReport 
    {
    TInt iErrorCode;
    TInt iChargingCurrent;
    };

    typedef TPckgBuf<TBatteryChargeCurrentReport> TBatteryChargingCurrentReportPackage;

    struct TBatteryFullChargingTimeReport 
    {
    TInt  iErrorCode;
    TUint iChargingTime;
    };

    typedef TPckgBuf<TBatteryFullChargingTimeReport> TBatteryFullChargingTimeReportPackage;
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
 			
    /** data package used for the following requests
    * EStartAverageBatteryPowerReportingCmdId
    * ESetPowerReportingIntervalMultipleCmdId
    */
    typedef TPckgBuf<TUint> TPowerIntervalMultiplePackage;


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    /** data package used for the following request
    * 
    * EStartAverageBatteryChargeRateReportingCmdId
    * EStopAverageBatteryChargeRateReportingCmdId
    * ESetChargeCurrentIntervalMultipleCmdId
    */
    typedef TPckgBuf<TUint> TChrgCurrentIntervalMultiplePackage;
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	
	/**
    * Command definitions
    * All commands contain a command id and a set of parameters. The command ids
    * are defined first and after that parameters for each command.
    *
    * This interface is asynchronous. A command is interpreted of being completed
    * after a response for that command is received.
    *
    * The field input means the parameter for the command.
    * The field output means the parameter for the response.
	*
	* @publishedPartner
	* @released
    */
    enum TPowerCmd
        {
        /**
        * No command. May be used for sanity checks, but 
        * never as an actual command ID.
        *
        * input    None   
        * output   None   
        */
        ENoCommandId = 5000,
        
        /**
        * Get Battery Voltage Info.
        *
        * input   None
        * output  TBatteryVoltageResponsePackage
        *          1. Error code
        *             - KErrNone if operation succeeds.
        *          2. Remaining Voltage, Madximum Voltage, Minimum Voltage
        */
        EGetBatteryVoltageInfoCmdId,
        
        /**
        * Get Battery Info.
        *
        * input   None
        * output  TBatteryConsumptionResponsePackage
        *          1. Error code
        *             - KErrNone if operation succeeds.
        *          2. Nominal Capacity, Remaining Capacity, Remaining Percentage Capacity
        */
        EGetBatteryInfoCmdId,

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        /**
        * Start Average Charge Current Rate Reporting.
        *
        * input    TUint IntervalMultiple
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStartAverageBatteryChargingCurrentReportingCmdId,

        /**
        * Stop Average Charge Current Rate Reporting.
        *
        * input    None   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStopAverageBatteryChargingCurrentReportingCmdId,

        /**
        * Update the time interval used for the average charging current 
        * measurements. 
        * input    TChrgCurrentIntervalMultiplePackage   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */

        ESetChargeCurrentIntervalMultipleCmdId,

        /**
        * Start Battery Full Charging Time Reporting.
        *
        * input    CHWRMPower::TIntervalMultiple
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStartBatteryFullChargingTimeReportingCmdId,

        /**
        * Stop Battery Full Charging Time Reporting.
        *
        * input    None   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStopBatteryFullChargingTimeReportingCmdId,
        
        
        /**
        * Get Charger Type.
        *
        * input   None
        * output  TBatteryChargerTypeResponsePackage
        *          1. Error code
        *             - KErrNone if operation succeeds.
        *          2. Charger Type
        */        
        EGetBatteryChargerTypeCmdId,
        
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        /**
        * Start Average Battery Power Reporting.
        *
        * input    TUint IntervalMultiple
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStartAverageBatteryPowerReportingCmdId,

        /**
        * Stop Power. 
        *
        * input    None   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EStopAverageBatteryPowerReportingCmdId,

        /**
        * Update the time interval used for the average battery power measurements. 
        *
        * input    TUint IntervalMultiple   
        * output   TErrorCodeResponsePackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        ESetPowerReportingIntervalMultipleCmdId,
        };
        
        
	/**
	* Power indications definitions
	* All commands contain a indication id and a set of parameters. The indication ids
	* are defined first and after that parameters for each incidcation.
	*
	* These incidcation are used when measurements from the battery are
	* reported through a callback.
	*
	* The field output means the parameter for the response.
	*
	* @publishedPartner
	* @released
	*/
	enum TPowerIndications
    	{        
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        /**
        * Used by the plugin to send an Average Charging Current Report.
        * 
        * output   TBatteryChargeRateReport   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EAverageBatteryChargingCurrentIndicationId,

        /**
        * Used by the plugin to send an Average Charging Current Report.
        * 
        * output   TBatteryChargeRateReport   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        */
        EBatteryFullChargingTimeIndicationId,
        
#endif //SYMBIAN_HWRM_EXTPOWERINFO

        /**
        * Used by the plugin to send an Average Battery Power Report.
        * 
        * output   TBatteryPowerReportPackage   Error code     
        *    KErrNone if operation succeeds.   
        *    KErrGeneral if there is any problem.
        * 
        */
        EAverageBatteryPowerIndicationId 

    	};
    }

#endif      // HWRMPowerCOMMANDS_H   
            
// End of File
