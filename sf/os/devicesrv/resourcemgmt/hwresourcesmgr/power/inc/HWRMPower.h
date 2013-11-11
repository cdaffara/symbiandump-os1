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

#ifndef HWRMPOWER_H_
#define HWRMPOWER_H_

// INCLUDES
#include <e32base.h>
#include <e32cmn.h> 

// CONSTANTS

// FORWARD DECLARATIONS

class CHWRMPowerImpl;
class MHWRMBatteryPowerObserver;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
class MHWRMBatteryChargingStatusObserver;
class MHWRMBatteryChargingCurrentObserver;
class MHWRMBatteryFullChargingTimeObserver;
#endif //SYMBIAN_HWRM_EXTPOWERINFO

/**
* HW Resource Manager Power API is a library API providing the 
* ability to request detailed information regarding the battery 
* capacity, voltage and current consumption.
*
* The API consists of the classes CHWRMPower, MHWRMBatteryConsumptionObserver
* and MHWRMBatteryVoltageObserver. If the client requires periodical measurements
* of the Average Voltage and/or Average Consumption, it should also provide callback pointer
* of the MHWRMBatteryConsumptionObserver and/or MHWRMBatteryVoltageObserver
* implementing class for the NewL() method.
*
* Usage:
*
* @code
* #include <hwrmpower.h> 
*
* // A CHWRMPower instance can be created by using NewL() or NewLC() methods. 
* // Periodical measurements are required so callbacks are provided. Assume 
* // the calling object implements  both interfaces.
* CHWRMPower* power = CHWRMPower::NewL(this, this);
*
* // Get battery info.
* TRequestStatus& status;
* TBatteryConsumptionData batteryConsumptionData;
* power->GetBatteryInfo(status, batteryConsumptionData); 
* 
* User::WaitForRequest(status);
* User::LeaveIfError(status.Int());
* 
* // Start receiving periodical power measurements.
* //
* // Note: There is a maximum reporting period defined for each product through the
* //       CenRep Key KPowerMaxReportingPeriod defined in HWRMPowerDomainCRKeys.h.
* //       The reporting will stop after this maximum reporting period by default.
* //       The application can configure this dynamically.
* //       Setting maximum report period to zero results in infinite notifications.
* //
* TRequestStatus& statusStart;
* TInt intervalMultiple = 3; // Three times the base time interval.
* //
* // Note: The base time interval is different for each product.
* //       GetBaseTimeIntervalL API returns this value.
* //
* power->StartAverageVoltageReporting(statusStart, intervalMultiple);
*  
* User::WaitForRequest(statusStart);
* User::LeaveIfError(statusStart.Int());
* 
* // Measurements are being when the ProcessVoltageMeasurement() method 
* // of the MHWRMBatteryVoltageObserver interface is called.
*  
* // Stop receiving measurements when done
* power->StopAverageVoltageReportingL(); 
*
* // To clean up, delete the created object:
* delete power;
* @endcode
*
* @publishedAll
* @released
*/
NONSHARABLE_CLASS(CHWRMPower) : public CBase
    {            
public:
	
	/**
	* Data structure used to retreive data from a call to GetBatteryVoltageInfo
	*/ 
	struct TBatteryVoltageData 
		{
		TInt iRemainingVoltage;
		TInt iMaximumVoltage;
		TInt iMinimumVoltage;
		};
		
	/**
	* Data structure used to retreive data from a call to GetBatteryInfo
	*/ 
    struct TBatteryConsumptionData 
	 	{
	 	TInt iNominalCapacity;
	 	TInt iRemainingCapacity;
	 	TInt iRemainingPercentageCapacity;
	 	};
    
    /**
    	* Data structure used to retreive data from a call to GetBatteryInfo
    	*/
    struct TBatteryPowerMeasurementData 
        {
        TInt iAverageVoltage;
        TInt iAverageCurrent;
        };  


#ifdef SYMBIAN_HWRM_EXTPOWERINFO

    /* The type of charger connected to the device.*/
    enum TBatteryChargerType 
        { 
        EBatteryChargerUnKnown = 0, 
        EBatteryChargerDedicated = 1, 
        EBatteryChargerUsbDedicated = 2,
        EBatteryChargerUsbHost = 3,
      	// Hi-speed USB host
				EBatteryChargerUsbHispeedHost 		= 4,
				// USB host with accessory charger adapter
				EBatteryChargerUsbAca 			= 5,
        };

    /* The current charging status of the device. */
    enum TBatteryChargingStatus
        {
        EChargingStatusError                    = -1,
        EChargingStatusIllegalChargerError      = -2,
        EChargingStatusChargerUnderVoltageError = -3,
        EChargingStatusChargerOverVoltageError  = -4,
        EChargingStatusTempTooHotError          = -5,
        EChargingStatusTempTooColdError         = -6,
        EChargingStatusNotConnected       = 0,  
        EChargingStatusCharging           = 1,  
        EChargingStatusNotCharging        = 2,  
        EChargingStatusAlmostComplete     = 3,  
        EChargingStatusChargingComplete   = 4,  
        EChargingStatusChargingContinued  = 5  
        };

    /* Used to stop the ongoing charging notification. 
     * EChargingNotifierAll used to stop charging current and charging 
     * time notifications. */
     
    enum TBatteryChargingNotifier
        {
        EChargingNotifierAll = 0,
        EChargingNotifierChargingCurrent = 1,
        EChargingNotifierBatteryFullChargingTime = 2
        };

#endif //SYMBIAN_HWRM_EXTPOWERINFO

public:	

    IMPORT_C ~CHWRMPower();
    IMPORT_C static CHWRMPower* NewL();
    IMPORT_C static CHWRMPower* NewLC();
     
public: // New functions

	IMPORT_C void GetBatteryInfo(TRequestStatus& aStatus, TBatteryConsumptionData& aBatteryConsumptionData);
    IMPORT_C void GetBatteryVoltageInfo(TRequestStatus& aStatus, TBatteryVoltageData& aBatteryVoltageData);
    IMPORT_C void GetBaseTimeIntervalL(TInt &aBaseTimeInterval);
    IMPORT_C TInt SetPowerReportObserver(MHWRMBatteryPowerObserver* aPowerReportCallback);
    IMPORT_C TInt StartAveragePowerReporting(TRequestStatus& aStatus, const TUint& aInterval);
    IMPORT_C void StopAveragePowerReportingL(); 
    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO 
    
    IMPORT_C TInt GetBatteryChargerType(TBatteryChargerType& aChargerType);
    IMPORT_C TInt GetBatteryChargingStatus(TBatteryChargingStatus& aChargingStatus);
    IMPORT_C TInt SetBatteryChargingObserver(
             MHWRMBatteryChargingStatusObserver*   aChargingStatusCallback,
             MHWRMBatteryChargingCurrentObserver*     aChrgCurrentCallback,
             MHWRMBatteryFullChargingTimeObserver* aChrgTimeCallback);
    IMPORT_C TInt NotifyAverageChargingCurrent(TRequestStatus& aStatus, const TUint& aInterval);
    IMPORT_C TInt NotifyBatteryFullChargingTime(TRequestStatus& aStatus);
    IMPORT_C void StopChargingNotificationL(TBatteryChargingNotifier aNotifier);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
private:

	void ConstructL();
	
private:

	CHWRMPowerImpl* iPowerImpl;	
    };

/**
* A callback interface for power measurements reporting.
*
* If the client requires periodical measurements of the Average Power,
* it needs to derive a class from the MHWRMBatteryPowerObserver interface
* and implement the PowerMeasurement() method. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
* #include <hwrmpower.h> // Link against HWRMPowerClient.lib.
*
* class CTest : public CBase, 
*               public MHWRMBatteryPowerObserver    
*    {
*    public:
*        CTest();
*        ~CTest();
*                       
*        void ConstructL();
*        static CTest* NewL();
*                
*        // from MHWRMBatteryPowerObserver
*        virtual void PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement);
*
*    private:
*        CHWRMPower* iPower;
*    };
* @endcode
*
* A callback method implementation example:
*
* @code
* void CTest::PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement);
*    {
*  	 if (aErrorCode == KErrNone)
*	 	{
*		INFO_PRINTF2(_L("INFO: Power Measurement Received, averageVolatge=%d, averageCurrent=%d"),aMeasurement.iAverageVoltage, aMeasurement.iAverageCurrent);
*		}
*	 else
*		{
*		INFO_PRINTF2(_L("INFO: Power Measurement Measurement Error, error=%d"),aErr);
*		}
*    }
* @endcode
*
* @publishedAll
* @released
*/

class MHWRMBatteryPowerObserver
    {
public:
    /** 
    * Called when a Power(Voltage and Current) consumption measurement value is received.
    *
    * @param aErr Error code. 
    * @param aMeasurement The Average Power value calculated by the plugin. Valid only when aErr == KErrNone.
    */

    virtual void PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement)=0;   
    };


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
/**
* A callback interface for charging status change.
*
* If the client requires charging status change notofication,
* it needs to derive a class from the MHWRMBatteryChargingStatusObserver interface
* and implement the ChargingStatusChange() method. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
class CTest : public CBase, 
              public MHWRMBatteryChargingStatusObserver
   {
    public:
        CTest();
        ~CTest();
                       
        void ConstructL();
        static CTest* NewL();
                
        // from MHWRMBatteryChargingStatusObserver
        virtual void ChargingStatusChange( Tint aErrCode,
                   CHWRMPower::TBatteryChargingStatus aChrgStatus);
   };
* @endcode   
* A callback method implementation example:
*
* @code

void CTest:: ChargingStatusChange(TInt aErrorCode,
                          CHWRMPower::TBatteryChargingStatus aChrgStatus);
   {
      if (aErrorCode == KErrNone)
      {
      INFO_PRINTF2(_L("Charging status = %d"), aChrgStatus);
      }
     else
      {
      INFO_PRINTF2(_L("Error, error=%d"),aErrorCode);
      }
   }
* @endcode  
*
* @publishedAll
* @released
*/
class MHWRMBatteryChargingStatusObserver
    {
public:
    /** 
    * Called when a charging status is changed.
    *
    * @param aErrCode Error code. 
    * @param aChrgStatus Charging status.
    */

    virtual void ChargingStatusChange( TInt aErrCode,
                                      CHWRMPower::TBatteryChargingStatus aChrgStatus)=0;
};

/**
* A callback interface for charging current measurements reporting.
*
* If the client requires periodical measurements of the charging current measurements,
* it needs to derive a class from the MHWRMBatteryChargingCurrentObserver interface
* and implement the AverageChargingCurrentChange() method. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
class CTest : public CBase, 
              public MHWRMBatteryChargingStatusObserver
   {
    public:
        CTest();
        ~CTest();
                       
        void ConstructL();
        static CTest* NewL();
                
        // from MHWRMBatteryChargingStatusObserver
        virtual void AverageChargingCurrentRateChange(
                           TInt aErrCode, TInt aMeasurement);

   };
* @endcode   
* A callback method implementation example:
*
* @code

// Notification handler
void CTest::ChargingCurrentChange(
                           TInt aErrorCode, TInt aMeasurement)
  {
      if (aErrorCode == KErrNone)
      {
      INFO_PRINTF2(_L("Charging current = %d"), aMeasurement);
      }
     else
      {
      INFO_PRINTF2(_L("Error, error=%d"),aErrorCode);
      }
   }


* @endcode  
*
* @publishedAll
* @released
*/

class MHWRMBatteryChargingCurrentObserver
    {
   /** 
    * Called when a charging current measurement value is received.
    *
    * @param aErrCode Error code. 
    * @param aMeasurement The Average Charging current value calculated by the plugin.
             Valid only when aErr == KErrNone.
    */
    public:
        virtual void AverageChargingCurrentChange(TInt aErrCode, TInt aMeasurement)=0;
    };

/**
* A callback interface for remaining charging time notification.
*
* If the client requires remaining charging time notification,
* it needs to derive a class from the MHWRMBatteryFullChargingTimeObserver interface
* and implement the BatteryFullChargingTimeChange() method. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
class CTest : public CBase, 
              public MHWRMBatteryFullChargingTimeObserver
   {
    public:
        CTest();
        ~CTest();
                       
        void ConstructL();
        static CTest* NewL();
                
        // from MHWRMBatteryChargingStatusObserver
        virtual void BatteryFullChargingTimeChange(
                           TInt aErrCode, TInt aMeasurement);

   };
* @endcode   
* A callback method implementation example:
*
* @code

// Notification handler
void CTest::BatteryFullChargingTimeChange(
                              TInt aErrorCode, TUint aTime)
   {
      if (aErrorCode == KErrNone)
      {
      INFO_PRINTF2(_L("Remaining charging time = %d"), aTime);
      }
     else
      {
      INFO_PRINTF2(_L("Error, error=%d"),aErrorCode);
      }
   }


* @endcode  
*
* @publishedAll
* @released
*/

class MHWRMBatteryFullChargingTimeObserver
    {
   /** 
    * Called when a full charging current time measurement value is received.
    *
    * @param aErrCode Error code. 
    * @param aTime Time required for full charging of Battery.
             Valid only when aErrCode == KErrNone.
    */

    public:
        virtual void BatteryFullChargingTimeChange(TInt aErrCode, TUint aTime)=0;
    };
#endif //SYMBIAN_HWRM_EXTPOWERINFO

#endif /*HWRMPOWER_H_*/
// End of File	
