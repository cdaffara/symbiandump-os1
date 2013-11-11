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

// INCLUDE FILES
#include <centralrepository.h>
#include "HWRMtrace.h"
#include "HWRMPower.h"
#include "HWRMPowerImpl.h"
#include "HWRMPowerDomainCRKeys.h"



// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

/**
 * Two-phased constructor for creating a CHWRMPower client.
 */
EXPORT_C CHWRMPower* CHWRMPower::NewL()
    {  
    CHWRMPower* power = NewLC();
    
    CleanupStack::Pop();

    return power;        
    }

/**
 * Two-phased constructor for creating a CHWRMPower client.
 * Leaves instance to CleanupStack.
 * 
 */
EXPORT_C CHWRMPower* CHWRMPower::NewLC()
	{		
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::NewLC()" ));
	
	CHWRMPower* power = new( ELeave ) CHWRMPower();
    
    CleanupStack::PushL( power );    
    power->ConstructL();         
	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::NewLC - return" ));
	
	return power;		
	}


/**
 * Symbian 2nd phase constructor can leave.
 * 
 * @param aVoltageCallback Pointer to callback instance.
 * @param aConsumptionCallback Pointer to callback instance.
 */
void CHWRMPower::ConstructL()
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::Construct()" ));
	
	iPowerImpl = CHWRMPowerImpl::NewL();	
	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::Construct - return" ));
	}

/**
 * C++ Destructor
 */
EXPORT_C CHWRMPower::~CHWRMPower()
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::~CHWRMPower()" ));
	
	delete iPowerImpl;
	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::~CHWRMPower()- return" ));
	}

/**
* Asynch
* 
* Get the Battery Info
* 
* @param aStatus This will be completed when the data are ready or an error occurs.
* @param aBatteryConsumptionData The nominal battery info is returned in this parameter. Valid only when aStatus.Int() == KErrNone
* @see CHWRMPower::TBatteryConsumptionData
*/
EXPORT_C void CHWRMPower::GetBatteryInfo(TRequestStatus& aStatus, TBatteryConsumptionData& aBatteryConsumptionData)
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryInfo()" ));
		
	iPowerImpl->DoGetBatteryInfo(aStatus, aBatteryConsumptionData);
	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryInfo - return" ));
	}

/**
* Asynch
* 
* Get the Battery Voltage Info
* 
* @param aStatus This will be completed when the data are ready or an error occurs.
* @param aBatteryVoltageData The nominal battery info is returned in this parameter. Valid only when aStatus.Int() == KErrNone
* @see CHWRMPower::TBatteryVoltageData
* 
*/
EXPORT_C void CHWRMPower::GetBatteryVoltageInfo(TRequestStatus& aStatus, TBatteryVoltageData& aBatteryVoltageData)
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryVoltageInfo()" ));
	
	iPowerImpl->DoGetBatteryVoltageInfo(aStatus, aBatteryVoltageData);
	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryVoltageInfo - return" ));
	}

/**            
* 
* Get the Base Time interval used by the plugin
* 
* @param aBaseTimeInterval The Base Time Interval is returned in this parameter.
*  
*/        
EXPORT_C void CHWRMPower::GetBaseTimeIntervalL(TInt &aBaseTimeInterval)
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBaseTimeIntervalL()" ));
	
	CRepository* powerCenRep = CRepository::NewLC(KCRUidPowerSettings);

	//cache it in private member?
	
    User::LeaveIfError(powerCenRep->Get(KPowerBaseTimeInterval, aBaseTimeInterval));
    
    CleanupStack::PopAndDestroy(powerCenRep);
    
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBaseTimeIntervalL - return" ));
	}

/**            
 * 
 * Post Start request to HWRM Server to start battery Power 	
 * measurements
 * 
 * @param aStatus This will be completed when measurements start or an error occurs.
 * @param aInterval: time period multiple. 
 */        
EXPORT_C TInt CHWRMPower::StartAveragePowerReporting(TRequestStatus& aStatus, const TUint& aInterval)
    {
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::StartAveragePowerReporting()" ));
	
	TInt err = iPowerImpl->DoStartAveragePowerReporting(aStatus, aInterval);

    COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::StartAveragePowerReporting( - return 0x%x" ), err );
    
    return err;
	}

/**          
*  Synchronous   
*    
*  Post stop request to HWRM Server to stop battery 	
* 	power measurements
* 
*  success/failure for stopping measurements.       
*/    
EXPORT_C void CHWRMPower::StopAveragePowerReportingL()
	{	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::StopAveragePowerReportingL()" ));
	
	iPowerImpl->DoStopAveragePowerReportingL();

	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::StopAveragePowerReportingL - return" ));
	}


EXPORT_C TInt CHWRMPower::SetPowerReportObserver(MHWRMBatteryPowerObserver* aPowerReportCallback)
    {
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::SetPowerReportObserverL()" ));
		
	TRAPD(err,iPowerImpl->DoSetPowerReportObserverL(aPowerReportCallback));

    COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::SetPowerReportObserverL err=%d- return" ), err);

    return err;

	}


#ifdef SYMBIAN_HWRM_EXTPOWERINFO

/**
* Synchronous
* Get the charger type. 
* @param  aChargerType Contains the charger type if request is successful.
* @return KErrNone if successful.Otherwise appropriate Error code
* @see CHWRMPower::TBatteryChargerType
*
*/

EXPORT_C TInt CHWRMPower::GetBatteryChargerType(TBatteryChargerType& aChargerType)
    {
    COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryChargerType()" ));

    aChargerType = CHWRMPower::EBatteryChargerUnKnown;

    TInt err = iPowerImpl->DoGetBatteryChargerType(aChargerType);    	
    
    COMPONENT_TRACE3(_L( "HWRM PowerClient - Charger Type = %d err = %d" ), aChargerType, err);        
    
    COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryChargerType() - return" ));

    return err;
    }
/**
* Synchronous
* Get the charging status.
* @param  aChargingStatus Contains the charging status if request is successful.
* @return KErrNone if successful.Otherwise appropriate Error code
* @see CHWRMPower::TBatteryChargingStatus
*
*/

EXPORT_C TInt CHWRMPower::GetBatteryChargingStatus(TBatteryChargingStatus& aChargingStatus)
    {
    COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryChargingStatus()" ));
    
    TInt err = iPowerImpl->DoGetBatteryChargingStatus(aChargingStatus);

    COMPONENT_TRACE3(_L( "HWRM PowerClient - err=%d aChargingStatus=%d" ), err, aChargingStatus);

    COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::GetBatteryChargingStatus - return" ));

    return err;
    }

/**
 * Synchronous
 * Set callbacks for charging information.
 * 
 * @param aChargingStatusCallback Pointer to callback instance.
 * @param aChrgRateCallback       Pointer to callback instance.
 * @param aChrgTimeCallback       Pointer to callback instance.
 */


EXPORT_C TInt CHWRMPower::SetBatteryChargingObserver(
             MHWRMBatteryChargingStatusObserver*   aChargingStatusCallback,
             MHWRMBatteryChargingCurrentObserver*     aChrgCurrentCallback,
             MHWRMBatteryFullChargingTimeObserver* aChrgTimeCallback)
    {
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::SetBatteryChargingObserver()" ));
		
	TRAPD(err,iPowerImpl->DoSetBatteryChargingObserverL(aChargingStatusCallback, aChrgCurrentCallback,
                                             aChrgTimeCallback));

    COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::SetBatteryChargingObserver err=%d- return" ), err);

    return err;

	}


/**            
 * 
 * Post Start request to HWRM Server to start Battery Charging Current	
 * measurements
 * 
 * @param aStatus This will be completed when measurements start or an error occurs.
 * @param aInterval: time period multiple. 
 */        
EXPORT_C TInt CHWRMPower::NotifyAverageChargingCurrent(TRequestStatus& aStatus, const TUint& aInterval)
	{	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::NotifyAverageChargingCurrent()" ));
	
	TInt err = iPowerImpl->DoNotifyAverageChargingCurrent(aStatus, aInterval);

    COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::NotifyAverageChargingCurrent( - return 0x%x" ), err );
    
    return err;
	}

/**            
 * 
 * Post Start request to HWRM Server to remaining charging time	
 * measurements
 * 
 * @param aStatus This will be completed when measurements start or an error occurs.
 */        
EXPORT_C TInt CHWRMPower::NotifyBatteryFullChargingTime(TRequestStatus& aStatus)
	{	
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::NotifyBatteryFullChargingTime()" ));
	
	TInt err = iPowerImpl->DoNotifyBatteryFullChargingTime(aStatus);

    COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::NotifyBatteryFullChargingTime( - return 0x%x" ), err );
    
    return err;
	}


/**          
*  Synchronous   
*    
*  Post stop request to HWRM Server to stop average current measurements.
* 
*  success/failure for stopping measurements.       
*/    
EXPORT_C void CHWRMPower::StopChargingNotificationL(TBatteryChargingNotifier aNotifier)
	{	
	COMPONENT_TRACE2(_L( "HWRM PowerClient - CHWRMPower::StopAverageVoltageReporting() aNotifier=%d" ), aNotifier);
	
	iPowerImpl->DoStopChargingNotificationL(aNotifier);

	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPower::StopAverageVoltageReporting - return" ));
	}
#endif //SYMBIAN_HWRM_EXTPOWERINFO

//  End of File
