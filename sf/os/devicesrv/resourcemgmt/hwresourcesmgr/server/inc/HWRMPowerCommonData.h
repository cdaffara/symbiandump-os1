/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMPOWERCOMMONDATA_H
#define HWRMPOWERCOMMONDATA_H

//  INCLUDES
#include <e32base.h>
#include "HWRMPower.h"
#include "HWRMPowerService.h"
#include "HWRMGenericTimer.h"
#include "HWRMPluginService.h"
#include "HWRMPluginHandler.h"
#include "cenrepobserver.h"


// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMPluginHandler;
class CHWRMPowerService;
class CClientsData;
class TPowerMeasurementsClientData;

// CLASS DECLARATION

/** Item of the list that holds data for a GetBatteryInfo or GetBatteryVoltageInfo request.
 * Used by the PowerCommonData iBatteryVoltageInfoQue, iBatteryInfoQue and iChargerTypeInfoQue.
 */
class TBatteryInfoData
	{
public:
	TBatteryInfoData(const CHWRMPowerService& aPowerClientService, const RMessage2& aRequestedMessage);
public:
	const CHWRMPowerService& 		iPowerClientService; // Used to identify the client that the message belongs to.
	RMessage2						iRequestedMessage;
	TSglQueLink						iSglQueLink;
	};    


/**
* Container for Power data that is common for all sessions.
* Also handles Power related cenrep.
* HWRM server upkeeps the instance.
*
*/
class CHWRMPowerCommonData : public CBase,
							 public MHWRMIndicationHandler,
							 public MHWRMGenericTimerCallback,
							 public MCenRepObserver
    {
public: // Constructors and Destructor

    /**
    * Two-phased constructor.
    *
    * @param aPluginHandler Pointer to plugin handler for Power
    */
    static CHWRMPowerCommonData* NewL(CHWRMPluginHandler* aPluginHandler);
    
    /**
    * Destructor.
    */
    virtual ~CHWRMPowerCommonData();

public: // Functions from base classes	
	/**
	* Called by the PluginHandler to indicate a new event
	* 
	* From MHWRMIndicationHandler
	*
    * @param aId  The indication ID
    * @param aData  Data associated with the indication 
	*/
	void ProcessIndicationL( const TUint32 aId, TDesC8& aData );
	
	/**
	* Called when the timer is fired
	* 
	* From MHWRMGenericTimerCallback
	*
	* @param aTimerId Id used to distinguish among different timers
	* @param aCutOff not used here
	*/
	void GenericTimerFired(TInt aTimerId, TBool /*aCutOff*/);

public: // New functions

    /**
    * Issues a plugin command to stop the Consumption measurements.
    */    
    void DoStopPowerMeasurementsL();
    
    /**
     * Called when the Maximum Reporting period in Cenrep is changed
     * 
     * @param aNewMaxPeriod the new max reporting period in Cenrep 
     * @param aOldMaxPeriod the old max reporting period in Cenrep
     */
    void MaxPeriodChangedL(TTimeIntervalMicroSeconds32& aNewMaxPeriod,
                           TTimeIntervalMicroSeconds32& aOldMaxPeriod);
    /*
     * Invoked by MaxPeriodChangeL
     * 
     * @param aTimer A generic Timer that has to be altered depending on the reporting period
     * @param aNewMaxPeriod the new max reporting period in Cenrep 
     * @param aOldMaxPeriod the old max reporting period in Cenrep
     */
    void DoMaxReportingValueChangedL(CHWRMGenericTimer* aTimer, 
                                    TTimeIntervalMicroSeconds32& aNewMaxPeriod,
                                    TTimeIntervalMicroSeconds32& aOldMaxPeriod);

    // From MCenRepObserver
    void HandleNotifyInt(TUint32 aId, TInt aNewValue);
    void HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler);


    /**
    * Issues a plugin command to set the plugin interval of charge current measurements.
    * 
    * @param aCommand distinguishes which interval is to be set (possible values: ESetVoltageIntervalMultipleCmdId, ESetCosnumptionIntervalMultipleCmdId)
    * @param aIntervalMultiple the value that the plugin interval will be set to
    */          
    void DoSetPluginIntervalMultipleL(HWRMPowerCommand::TPowerCmd aCommand,
                                    TUint aIntervalMultiple);

    /**
     * Issues a plugin command to get Battery Info. If there is an outstanding plugin request there isn't a new
 	 * one issued but the appropriate data are stored in a queue. This data are used later to complete or cancel the client.
     * 
     * @param aPowerService the service of the client session that issued the command.
     * @param aMessage the client message that requested the the Battery Info.
     */        
    void DoGetBatteryInfoL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService);
    
    /**
     * Issues a plugin command to get Battery Voltage Info. If there is an outstanding plugin request there isn't a new
 	 * one issued but the appropriate data are stored in a queue. This data are used later to complete or cancel the client.
     * 
     * @param aPowerService the service of the client session that issued the command.
     * @param aMessage the client message that requested the the Battery Info.
     */       
    void DoGetBatteryVoltageInfoL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService);
    
    /**
     * Completes any pending GetBatteryInfo requests.
     * 
     * @param aReason	the Error code that is used for completion.
     * @param aDataPckg data that will be written in the response message (can be an empty Des).
     */       
    void CompletePendingGetBatteryInfo(TInt aReason, TPckg<CHWRMPower::TBatteryConsumptionData> &aDataPckg);

    /**
     * Completes any pending GetBatteryVoltageInfo requests.
     * 
     * @param aReason	the Error code that is used for completion.
     * @param aDataPckg data that will be written in the response message (can be an empty Des).
     */       
    void CompletePendingGetBatteryVoltageInfo(TInt aReason, TPckg<CHWRMPower::TBatteryVoltageData> &aDataPckg);
    
    /**
     * Cancels any pending GetBatteryInfo requests.
     * 
     * @param aPowerService the service of the client session that issued the command. Used to identify the requests taht belong to the client.
     */          
    void CancelPendingGetBatteryInfo(CHWRMPowerService& aPowerService);

    /**
     * Cancels any pending GetBatteryVoltageInfo requests.
     * 
     * @param aPowerService the service of the client session that issued the command. Used to identify the requests taht belong to the client.
     */          
    void CancelPendingGetBatteryVoltageInfo(CHWRMPowerService& aPowerService);

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    /**
    * Issues a plugin command to stop the charging current measurements.
    */    
    void DoStopChargingCurrentMeasurementsL();

    /**
    * Issues a plugin command to stop the BatteryFull charging time measurements.
    */    
    void DoStopChargingTimeMeasurementsL();

     /**
     * Issues a plugin command to get charger type. If there is an outstanding plugin request there isn't a new
 	 * one issued but the appropriate data are stored in a queue. This data are used later to complete the request.
     * 
     * @param aPowerService the service of the client session that issued the command.
     * @param aMessage the client message that requested the charger type.
     */        
    void DoGetChargerTypeL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService);

     /**
     * Completes any pending ChargerType Info requests.
     * 
     * @param aReason	the Error code that is used for completion.
     * @param aDataPckg data that will be written in the response message (can be an empty Des).
     */       
    void CompletePendingGetChargerType(TInt aReason, TPckg<CHWRMPower::TBatteryChargerType> &aDataPckg);

    /**
     * Cancels any pending GetChargetType requests.
     * 
     * @param aPowerService the service of the client session that issued the command. Used to identify the requests taht belong to the client.
     */        
    void CancelPendingGetChargerType(CHWRMPowerService& aPowerService);

#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
private:

	/**
	* C++ default constructor.
	*/
	CHWRMPowerCommonData();
	
	/**
	* By default Symbian 2nd phase constructor is private.
	*
	* @param aPluginHandler Pointer to plugin handler for Power
	*/
	void ConstructL(CHWRMPluginHandler* aPluginHandler);

public:
    CRepository*                    iRepository;                        // Cenrep client
    CCenRepListener*                iMaxReportingPeriodNotifyHandler;   // Cenrep notifyhandler
    CClientsData*                   iPowerReportClientsData;            // List of clients that have registered for Average Power Measurements Reporting
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	CClientsData*					iChargingCurrentClientsData;	// List of clients that have registered for Average Charging Current Measurements Reporting
	CClientsData*					iChargingTimeClientsData;	    // List of clients that have registered for Battery Full Charging Time Measurements Reporting
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	
	TSglQue<TBatteryInfoData> 		iBatteryInfoQue;				// list of the pending GetBatteryInfo requests
	TSglQueIter<TBatteryInfoData> 	iBatteryInfoIter;				// iterator of the list
	
	TSglQue<TBatteryInfoData> 		iBatteryVoltageInfoQue;			// list of the pending GetBatteryVoltageInfo requests
	TSglQueIter<TBatteryInfoData> 	iBatteryVoltageInfoIter;		// iterator of the list

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    TSglQue<TBatteryInfoData> 		iChargerTypeInfoQue;			// list of the pending GetChargerType requests
	TSglQueIter<TBatteryInfoData> 	iChargerTypeInfoIter;			// iterator of the list
#endif //SYMBIAN_HWRM_EXTPOWERINFO


    
private:
	TUint							iBaseTimeInterval;				// Value of Base Time Interval is cached here from CenRep
	TUint 							iMaximumReportingPeriod;		// Value of Maximum Reporting Period is cached here from CenRep
  
    CHWRMGenericTimer*   			iPowerReportMeasurementsTimer;      // Cutoff timer. Owned.
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    CHWRMGenericTimer*  	 		iChargingCurrentMeasurementsTimer; 	// Cutoff timer. Owned.    
    CHWRMGenericTimer*  	 		iChargingTimeMeasurementsTimer; 	// Cutoff timer. Owned.    
#endif //SYMBIAN_HWRM_EXTPOWERINFO


	CHWRMPowerService*    			iPowerService;  				// Power service used to stop measurements 
																	// or setting the plugin interval
    };
   
/** IDs used to distinguish bewtween the two timers that the CommonData own */
enum TClientsIds
	{
	EPowerReportClientsId = 0
	
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    ,EChargingCurrentClientsId,
	EChargingTimeClientsId
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
	};

/** Item of the list that class CClientsData holds. One exists for each client
 * that registers for measurements. Holds a pointer to the TPowerMeasurementsClientData
 * that are owned by the HWRMPowerService of each client.   
 */
class TClientData
	{
public:
	TClientData(TPowerMeasurementsClientData& aPowerMeasurementsClientData);
public:
	TPowerMeasurementsClientData& 	iPowerMeasurementsClientData; 
	TSglQueLink						iSglQueLink;
	};

/**
 * Contains all the clients that have registered for a type of measurements reporting.
 * Handles indications for this type of measurements and informs every registered
 * client for the event. Also holds the Max Period timer.
 * 
 * One instance of this class exists for Voltage and one for Consumption measurements.
 * It is a central point where the measurements are indicated and then dispatched 
 * to each of the clients.
 * 
 * It is populated during the reporting session and reset after the end of the session
 * either because the last client called stop, or because the maximum reporting 
 * period was reached. 
 * 
 * When the last client calls stop CLientsData will stop the plugin.
 * 
 * When multiple clients registered the Greatest Common Devisor of the 
 * Requested Time Intervals is calculated and used by the plugin. 
 * All the calculations are carried by this class.
 * 
 * When the Max period is reached all the clients are completed, reset and the 
 * plugin is stopped.
 *
 */
class CClientsData : public CBase					 
	{
public:
	static CClientsData* NewL(CHWRMGenericTimer& aMeasurementsTimer);
	
	virtual ~CClientsData();
		
public:
	/**
    * Called when a measurement indication is received from the plugin.
    * 
    * @param aValue value of the indicated measurement
    */
	void BatteryMeasurementIndicationL(TDesC8& aValue);

	/**
    * Getter for the active interval multiple.
    * 
    * @param aIntervalMultiple holds the returned value
    */
    void GetPluginIntervalMultiple(TUint &aIntervalMultiple);
    
    /**
    * Setter for the plugin's voltage time interval.
    * 
    * @param aIntervalMultiple value that the plugin's voltage time interval is set to
    */
    void UpdatePluginIntervalMultiple(TUint aIntervalMultiple);

    /**
    * Checks if the plugin measurements are On.
    */        
    TBool IsMeasuringOn();
    
    /**
    * Sets the plugin measurements flag On.
    */        
    void SetMeasuringOn();
    
    /**
    * Sets the plugin measurements flag On.
    */        
    void SetMeasuringOff();
     
    /**
	* Cheks if the Clients Queue is empty
	*/       
    TBool IsClientsQueEmpty();
     
    /**
    * Creates and adds to the list a data structure 
    * containing all the appropriate client data.
    * 
    * @param aPowerMeasurementsClientData pointer to the data to be added
    */        
    void AddMeasurementsClientL(TPowerMeasurementsClientData& aPowerMeasurementsClientData);
   
     
    /**
    * Deletes all the data relevant to the client that sent the message.
    * 
    * @param aPowerMeasurementsClientData pointer to the data that will be removed and deleted
    * 
    */       
    void RemoveMeasurementsClientData(TPowerMeasurementsClientData* aPowerMeasurementsClientData);
    
    /**
    * Deletes all the data relevant to the client that sent the message.
    * 
    * @param aPowerMeasurementsClientData pointer to the data that will be removed and deleted
    * 
    * @leave KErrNotFound Client not found in the list.
    */       
    void RemoveMeasurementsClientL(TPowerMeasurementsClientData* aPowerMeasurementsClientData);

    /**
	* Adjusts the interval used by the plugin when a new client registers
	* with a time interval that causes a change of the GCD.
	* 
	*  @param aIntervalMultiple Tthe new value of the GCD
	*/       
    void RecalculatePluginInterval(TUint &aIntervalMultiple);
          	
    /**
	* Checks if a adjustement is needed for the 
	* interval used by the plugin
	* 
	* @param aIntervalMultiple The interval of the incoming client.
	*/       
    TBool NeedPluginIntervalAdjustment(TUint aIntervalMultiple);
    
    /**
	* Deletes all the measurements related data
	* Used at destruction time.
	*/	    
    void CleanUpMeasurementsData();
     
	/**
	* Calculation of the Greatest Common Divisor of two numbers.
	*/
	TUint CalculateGCD(TUint num1, TUint num2);

	/**
	* Called when the Max Reporting Period is reached. 
	* 
	*/	
	void MaxReportingPeriodReached(TInt aReason=KErrTimedOut);

private:
	/*
	* Deletes all the items of the queue and empties it.
	*/  
	void EmptyClientsQue();

	/**
	* The Measurements collected so far are written in the
	* message area of the data
	*/		
	void WriteMeasurementsInMessageL(TClientData* aClientData, TUint aCount);
	
	/**
	* Returns the ClientData that correspond to the aPowerMeasurementsClientData. 
	* Each session has a TPowerMeasurementsClientData object. Hence each client is
	* uniqely identified by this object.
	* 
	* @return Pointer to the TClientData of the client that holds the aPowerMeasurementsClientData. NULL if client is not found
	*/		
	TClientData* FindClient(TPowerMeasurementsClientData* aPowerMeasurementsClientData);
	
private:
    /**
    * C++ default constructor.
    */
	CClientsData(CHWRMGenericTimer& aMeasurementsTimer);

    /**
    * By default Symbian 2nd phase constructor is private.
    *
    * @param aPluginHandler Pointer to plugin handler for Power
    */
    void ConstructL();        

private:
	TBool 								iMeasuring;						// Indicates if the plugin measurements are On or Off
    //Changing to TUint as Charge current report needs it. Also other measurements to be changed in future.    
    TUint                               iPluginIntervalMultiple;        // the Interval Multiple that is currently in use by the plugin
	CHWRMGenericTimer&					iMeasurementsTimer;  			// Cutoff timer. Used to set and cancel the Max Period Timer
   
	TSglQue<TClientData> 				iClientsQue;					// list of the registered clients
	TSglQueIter<TClientData> 			iClientsQueIter;				// iterator of the list  
 };

#endif  // HWRMPOWERCOMMONDATA_H
            
// End of File
