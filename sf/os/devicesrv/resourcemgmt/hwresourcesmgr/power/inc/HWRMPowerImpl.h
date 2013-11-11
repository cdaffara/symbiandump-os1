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

#ifndef HWRMPOWERIMPL_H_
#define HWRMPOWERIMPL_H_

// INCLUDES
#include <e32base.h>
#include <e32cmn.h> 

#include "HWRMPower.h"
#include "HWRMClient.h"


// CONSTANTS

// FORWARD DECLARATIONS
class CHWRMBatteryMeasurementsAO;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

class CHWRMChargingStatusObs;

#endif //SYMBIAN_HWRM_EXTPOWERINFO

/**
*  Class of the power client side session.
*
*/
NONSHARABLE_CLASS(CHWRMPowerImpl) : public CBase

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
                                    ,public MHWRMBatteryChargingStatusObserver
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    {                
    public:
    
        static CHWRMPowerImpl* NewL();
        
        ~CHWRMPowerImpl();
        
    public: // New functions
        /**
        * Asynch
        * 
        * @param aStatus This will be completed when the data are ready or an error occurs.
        * @param aBatteryConsumptionData The nominal battery info is returned in this parameter. Valid only when aStatus.Int() == KErrNone
        * @see CHWRMPower::TBatteryConsumptionData
        */
        void DoGetBatteryInfo(TRequestStatus& aStatus, CHWRMPower::TBatteryConsumptionData& aBatteryConsumptionData);        
        
        /**
        * Asynch
        * 
        * @param aStatus This will be completed when the data are ready or an error occurs.
        * @param aBatteryVoltageData The nominal battery info is returned in this parameter. Valid only when aStatus.Int() == KErrNone
        * @see CHWRMPower::TBatteryVoltageData
        * 
        */
        void DoGetBatteryVoltageInfo(TRequestStatus& aStatus, CHWRMPower::TBatteryVoltageData& aBatteryVoltageData);
                                        
                                               
        /**            
        * 
        * Post Start request to HWRM Server to start battery power    
        * measurements         
        * 
        * The receipt of measurements is asynchronous, when hwrm server 
        * completes (internally issued)request to dispatch measurements to 
        * client, data member iBatteryPowerMeasurementsAO (MHWRMBatteryPowerObserver)
        * will have it's PowerMeasurement method invoked on
        * each received average measurement value
        * 
        * @param aStatus This will be completed when measurements start or an error occurs.
        * @param aInterval: time period multiple. 
        */        
        TInt DoStartAveragePowerReporting(TRequestStatus& aStatus, const TUint& aInterval);

                        
        
        
        /**          
        *  Synchronous   
        *    
        *  Post stop request to HWRM Server to stop battery     
        *   power measurements
        * 
        *  success/failure for stopping measurements.       
        */    
        void DoStopAveragePowerReportingL();  
        
        /**
         * Synchronous
         * Set callbacks for power information.
         * 
         * @param aPowerReportObserver Pointer to callback instance.
         */

         void DoSetPowerReportObserverL(MHWRMBatteryPowerObserver* aPowerReportCallback);

        
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        /**
        * Synchronous.
        * Get the charger type connected.
        * @param aChargerType Contains the type of charger connected, if return value is KErrNone.
        * @return KErrNone if charger type is successfully retrieved, else contains appropriate error code.
        * @see CHWRMPower::TBatteryChargerType .
        */
        TInt DoGetBatteryChargerType(CHWRMPower::TBatteryChargerType& aChargerType);

        /**
        * Synchronous
        * Get Battery charging status
        * @param aChargingStatus Contains the charging status, if return value is KErrNone.
        * @return KErrNone if charging status is succesfully retrieved, else contains appropriate error code.
        */
        TInt DoGetBatteryChargingStatus(CHWRMPower::TBatteryChargingStatus& aChargingStatus);

        /**
         * Synchronous
         * Set callbacks for charging information.
         * 
         * @param aChargingStatusCallback Pointer to callback instance.
         * @param aChrgRateCallback       Pointer to callback instance.
         * @param aChrgTimeCallback       Pointer to callback instance.
         */


         void DoSetBatteryChargingObserverL(
             MHWRMBatteryChargingStatusObserver*   aChargingStatusCallback,
             MHWRMBatteryChargingCurrentObserver*     aChrgCurrentCallback,
             MHWRMBatteryFullChargingTimeObserver* aChrgTimeCallback);

        /**            
        * 
        * Post Start request to HWRM Server to start charging current notification
        * 
        * The receipt of measurements is asynchronous, when hwrm server 
        * completes (internally issued)request to dispatch measurements to 
        * client, data member iBatteryChargeCurrentRateAO (MHWRMBatteryChargingCurrentObserver)
        * will have it's AverageChargingCurrentRateChange method invoked on
        * each received average measurement value
        * 
        * @param aStatus This will be completed when measurements start or an error occurs.
        * @param aTimePeriodMultiple: time period multiple. 
        * @see CHWRMPower::TIntervalMultiple
        */        
        TInt DoNotifyAverageChargingCurrent(TRequestStatus& aStatus, const TUint& aInterval);

        /**            
        * 
        * Post Start request to HWRM Server to start charging current notification
        * 
        * The receipt of measurements is asynchronous, when hwrm server 
        * completes (internally issued)request to dispatch measurements to 
        * client, data member iBatteryChargingTimeAO (MHWRMBatteryFullChargingTimeObserver)
        * will have it's BatteryFullChargingTimeChange method invoked on
        * each received average measurement value.
        * Note that time interval is decided by the plugin.
        *
        * 
        * @param aStatus This will be completed when measurements start or an error occurs.
        */        
        TInt DoNotifyBatteryFullChargingTime(TRequestStatus& aStatus);


         /**          
        *  Synchronous   
        *    
        *  Post stop request to HWRM Server to stop average current measurements.
        * 
        *  success/failure for stopping measurements.       
        */    
        void DoStopChargingNotificationL(CHWRMPower::TBatteryChargingNotifier aNotifier);              


        /*
        * CallBack implementation from MHWRMBatteryChargingStatusObserver
        */
        void ChargingStatusChange(TInt aErrCode,CHWRMPower::TBatteryChargingStatus aChrgStatus);
                        
#endif //SYMBIAN_HWRM_EXTPOWERINFO

    private:
        
        /**
        * C++ default constructor.
        */
        CHWRMPowerImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();      
        
        /**
        * Opens the power session.
        * This method must be called before any other methods can be called.
        */
        void OpenL();
        
        /**
        * Closes the session. 
        */
        void Close();
        
    private:
    
        CHWRMBatteryMeasurementsAO*         iBatteryPowerMeasurementsAO;        // Active Object use to issue GetVoltage Measurements commands
        																		// and call the registered client's callback. Owned.

        TPtr8                               iBatteryConsumptionDataPckg;
        TPtr8                               iBatteryVoltageDataPckg;
        TPtr8                               iPowerIntervalMultiplePckg;


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        TPtr8                               iBatteryChargerTypePckg;

        MHWRMBatteryChargingStatusObserver*   iChargingStatusClientObserver;    // Client listening for charging status change. Not owned
        CHWRMChargingStatusObs*               iChargingStatusImpl;              // Charing Status implemented using this observer.

        CHWRMBatteryMeasurementsAO*           iBatteryChargeCurrentRateAO;      // Active Object use to issue Get charge current rate commands 
                                                                                // and call the registered client's callback. Owned.
        CHWRMBatteryMeasurementsAO*           iBatteryChargingTimeAO;           // Active Object use to issue Get Remaining charging time 
                                                                                // and call the registered client's callback. Owned.
                                                                                
        TPtr8                                 iCurrentRateIntervalMultiplePckg;

#endif //SYMBIAN_HWRM_EXTPOWERINFO

        RHWRMClient                         iClient;                            // HWRM server client
    };

#endif /*HWRMPOWERIMPL_H_*/
