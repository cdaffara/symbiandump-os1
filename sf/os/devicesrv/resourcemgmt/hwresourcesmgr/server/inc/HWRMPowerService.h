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

#ifndef HWRMPOWERSERVICE_H
#define HWRMPOWERSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMGenericTimer.h"
#include "HWRMPowerCommands.h"
#include "HWRMPower.h"
#include "HWRMService.h"
#include "HWRMPowerCommonData.h"

// CONSTANTS
// None
const TInt KHWRMPowerMaxMeasurementsBatchSize = 20;
// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMPowerCommonData;
class CHWRMPluginHandler;
class TPowerMeasurementsClientData;
class THWRMPluginPowerIntervalRequestData;

// CLASS DECLARATION

/**
*  Hardware Resource Manager server side service object for Power sessions.
*
*/
class CHWRMPowerService : public CHWRMService                         
    {    
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler The plugin handler to use with this service.
        * @param aPowerCommonData Reference to power common data
        */
        static CHWRMPowerService* NewL(CHWRMPluginHandler* aPluginHandler, CHWRMPowerCommonData& aCommonData);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMPowerService();
    
    public: // Base class methods
    
        // From CHWRMService
    	virtual TBool ExecuteMessageL(const RMessage2& aMessage);
    	virtual void ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout );
    	virtual void SuspendSubResource(TInt aSubResource);
    	virtual void ResumeSubResource(TInt aSubResource);
        virtual void ActivateSubResource(TInt aSubResource, TBool aActivate);
        
    public:
    	/**
     	* Issues a plugin command to stop the power measurements. Not 
     	* associated with a client RMessage2
     	*/
        void StopPowerMeasurementsL();

        /**
        * Issues a plugin command to Set the Plugin Interval Multiple. Not 
        * associated with a client RMessage2
        * Specifically added for charge rate current reporting.
        */
        void SetPluginIntervalMultipleL(HWRMPowerCommand::TPowerCmd aCommand,
                                        TUint aIntervalMultiple);

        
        /**
         * Gets the Battery consumption info
         * 
         * Handles EHWRMGetBatteryInfo requests.
         */      
         void GetBatteryInfoL(const RMessage2& aMessage);
         

         /**
         * Gets the Baterry voltage info
         * 
         * Handles EHWRMGetBatteryVoltageInfo requests.
         */      
         void GetBatteryVoltageInfoL(const RMessage2& aMessage);

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

         /**
        * Issues a plugin command to stop the Charging Current measurements. Not 
        * associated with a client RMessage2
        */
        void StopChargingCurrentMeasurementsL();

        /**
        * Issues a plugin command to stop the BatteryFull charging time measurements. Not 
        * associated with a client RMessage2
        */
        void StopChargingTimeMeasurementsL();


         /**
         * Gets the charger type
         * 
         * Handles EHWRMGetBatteryChargerType request.
         */      
         void GetChargerTypeL(const RMessage2& aMessage);

#endif //SYMBIAN_HWRM_EXTPOWERINFO
         
    private: 
    
        /**
        * C++ default constructor.
        */
        CHWRMPowerService(CHWRMPowerCommonData& aPowerCommonData);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler);

    private:

        /**
        * Starts the periodic reporting of avegage battery power value 
        * (period = TIntervalMultiple * BaseTimeInterval)
        * 
        * Handles EStartAverageBatteryPowerReportingCmdId requests.
        */      
        void StartAverageBatteryPowerReportingL(const RMessage2& aMessage);

        /**
        * Stops the periodic reporting of avegage battery power value
        * 
        * Handles EHWRMStopAverageBatteryPowerReporting requests.
        */      
        void StopAverageBatteryPowerReportingL(const RMessage2& aMessage);


#ifdef SYMBIAN_HWRM_EXTPOWERINFO

        /**
        * Starts the periodic reporting of average charging current value 
        * (period = TIntervalMultiple * BaseTimeInterval)
        * 
        * Handles EHWRMStartAverageChargingCurrentReporting requests.
        */      


        void StartAverageChargingCurrentReportingL(const RMessage2& aMessage);

        /**
        * Stops the periodic reporting of average charging current value/
        * 
        * Handles  requests.
        */      
        void StopAverageChargingCurrentReportingL(const RMessage2& aMessage);

        /**
        * Starts the reporting of battery full charging time measurements.
        * Period is decided by plugin.
        * 
        * Handles EHWRMStartBatteryFullChargingTimeReporting requests.
        */      
        void StartChargingTimeReportingL(const RMessage2& aMessage);


        /**
        * Stops the reporting of battery full charging time measurements.
        * 
        * Handles EHWRMStopBatteryFullChargingTimeReporting requests.
        */      
        void StopChargingTimeReportingL(const RMessage2& aMessage);

#endif //SYMBIAN_HWRM_EXTPOWERINFO
        
        /**
        * Handles EHWRMCleanupBattery requests.
        * 
        * Cleans up the power service so that session can be closed.
        */        
        void CleanupPowerL();
        
        /**
         * Executes a plugin command and handles related transaction.
         *
         * @param aCommandId Command ID of the command.  
         */
        void ExecutePluginCommand(HWRMPowerCommand::TPowerCmd aCommand);

        /**
         * Executes a plugin command and handles related transaction.
         *
         * @param aCommandId Command ID of the command.  
         * @param aSplitCommand ETrue if this command is split into multiple transactions
         * @param aPckg Command package
         */
        void ExecutePluginCommand(HWRMPowerCommand::TPowerCmd aCommand,
                                                    TBool /* aSplitCommand */,
                                                    TDesC8& aPckg );
        
        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                    HWRMPowerCommand::TPowerCmd aCommandId,
                                    TBool aSplitCommand );

        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions        
        * @param aPckg Command package
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                    HWRMPowerCommand::TPowerCmd aCommandId,
                                    TBool aSplitCommand,
                                    TDesC8& aPckg );


        /**
        * Executes a plugin command and handles related transaction.
        *
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.  
        * @param aSplitCommand ETrue if this command is split into multiple transactions        
        * @param aPckg Command package  
        * @param aArg Generic integer parameter for THWRMPowerPluginRequestData       
        */
        void ExecutePluginCommandL(const RMessage2& aMessage,
                                    HWRMPowerCommand::TPowerCmd aCommandId,
                                    TBool aSplitCommand,
                                    TDesC8& aPckg,
                                    TInt aArg );
        /**
        * Cancels a plugin command and handles the related transaction(s).
        *
        * @param aCommandId Command ID of the command.  
        */
        void CancelPluginCommandL(HWRMPowerCommand::TPowerCmd aCommandId);
          
        /**
        * Checks the response data size
        *
        * @return ETrue if aData.Size() is correct
        */
        TBool CheckResponseDataSize( TInt aCommandId, TDesC8& aData) ;
        
    private:  // data
    
        CHWRMPowerCommonData&       	iPowerCommonData;    // Pointer to Power data object common to all sessions. Not owned.
        
        TPowerMeasurementsClientData* 	iPowerVoltageCurrentMeasurementsClientData;	// pointer to the Voltage*Current(Power) data measurements of the client
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        TPowerMeasurementsClientData* 	iPowerChargingCurrentMeasurementsClientData;	// pointer to the consumption data measurements of the client
        TPowerMeasurementsClientData* 	iPowerChargingTimeMeasurementsClientData;     		// pointer to the voltage data measurements of the client
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        TBool                   	    iSuspended;          // Flag to indicate if this session is suspended.
    };

/**
* PowerService holds two objects of class TPowerMeasurementsClientData one for voltage and
* one for consumptin measurements.
* It holds the list of data measurements. Measurements from the plugin are propagated to this class
* from the CCLientsData of the PowerCommonData. 
* Caches Start and GetMeasuements messages from the client
* and is responsible for completing or cancelling them. 
*/
class TPowerMeasurementsClientData 
	{ 
public:// Constructors and Destructor
	/**
	 * C++ Constructor
	 */
	TPowerMeasurementsClientData();
	
	/**
	 * C++ Destructor
	 */
	virtual ~TPowerMeasurementsClientData();
public:

	/**
	 * Adds a Measurement to the iMeasurementsQue
	 */
    virtual void AddMeasurementL(TDesC8& aMeasurement) = 0;
	
	
	/**
	* If at least one measurement is collected the data is written to 
	* the messasge and the client is completed. If not the 
	* message hanlde is stored and the client is completed
	* when data is ready
	*/       
    virtual TBool GetMeasurementsL(RMessage2 aMessage) = 0;
	 
	/**
	* Check if we are receiving measurements.
	*/        
	TBool IsMeasuringOn();
	
	/**
	* Set the measuring flag on.
	*/        
	void SetMeasuringOn();
	
	/**
	* Set the measuring flag off.
	*/        
	void SetMeasuringOff();

	/**
	* If there exists a cached message of the client requesting measuremnts, it is completed with Error code aReason
	*/        
	void CompleteGetMeasurementsMessage(TInt aReason);
	
	/**
 	* If there exists a cached message of the client requesting measuremnts, it is completed with KErrCancel
	*/	    
	void CancelPendingGetMeasurementsMessage();
	
	/* 
	 * The Measurements collected are written in the message area of the message.
	 */
    virtual void WriteMeasurementsInMessageL() = 0;
	
	/**
	* If there exists a cached message of the client requesting to STart measuremnts, it is completed with Error code aReason
	*/        
	void CompletePendingStart(TInt aReason);
	
	/**
	* Resets the object in order to be ready for a new measurements session.
	*/   
	virtual void Reset();
	
	/**
	* Called if the client has registerd for measurements and a new measurement is sent by the plugin.
	*/   	
    virtual void NewBatteryMeasurementL(TDesC8& aValue, TUint aPluginInterval) = 0;
public:
    //Changing to TUint as Charge current report needs it.     
	TUint 	iRequestedIntervalMultiple;		//The interval multiple that the client requested
	
	TUint 							iNumOfBaseIntervalsPassed;		// number of Base intervals for which we have accumulated measurements
																	// when this value is equal or greater than the Requested Interval Multiple
																	// the average is calculated and added as a new measurement to the list
	TBool 							iMeasuring;						// flag indicating if the client has started measuremnets
	TUint 							iMeasurementsCount;				// number of collected measurements		
	
	RMessage2 						iRequestedBatchMessage;			// used to complete clients. NULL indicates no pending requests
	RMessage2 						iRequestedStartMessage;			// used to complete clients. NULL indicates no pending requests
	};
	
class TPowerIntMeasurementsClientData : public TPowerMeasurementsClientData
    {

   struct TPluginIntMeasurementFormat
    {
    TInt iError;        //Error Code.
    TInt iMeasurement;  //Measurement received from Plugin.
    };

    /**
    *  Holds a power measurement.
    */

    class TPowerIntMeasurement
        {
    public:
        TInt        iAverageValue;
        TSglQueLink iSglQueLink;
        };
    
    public:
        
    typedef TPckgBuf<TInt> TIntMeasurementsPckg;

    TPowerIntMeasurementsClientData();

    ~TPowerIntMeasurementsClientData();
    
    /**
     * Adds a Measurement to the iMeasurementsQue
     */
    virtual void AddMeasurementL(TDesC8& aMeasurement);
    
    /* 
     * The Measurements collected are written in the message area of the message.
     */
    
    virtual void WriteMeasurementsInMessageL();
    
    /**
     * Resets the object in order to be ready for a new measurements session.
     */   

    virtual void Reset();
    
    /**
     * Called if the client has registerd for measurements and a new measurement is sent by the plugin.
     */         
    
    virtual void NewBatteryMeasurementL(TDesC8& aValue, TUint aPluginInterval);

    /**
     * If at least one measurement is collected the data is written to 
     * the messasge and the client is completed. If not the 
     * message hanlde is stored and the client is completed
     * when data is ready
     */       


    virtual TBool GetMeasurementsL(RMessage2 aMessage);

    /**
     * Transfers the collecetd data from the list to a TFixedArray, appropriate for client-server
     * data trasnfer.
     */
    void TransferMeasurementsToFixedArrayL(TFixedArray<TInt,KHWRMPowerMaxMeasurementsBatchSize> &aFixedArray);

    private:
        
    TSglQue<TPowerIntMeasurement>      iMeasurementsQue;               // list of the measurements
    TSglQueIter<TPowerIntMeasurement>  iMeasurementsQueIter;

    TInt                            iAccumulator;                   // measurements are accumulated until there are enough values to calculate the average


    };

class TPowerVoltageCurrentMeasurementsClientData : public TPowerMeasurementsClientData
    {

   /* Power(Voltage and Current) measurement from plugin 
      should in this format.
    */
   struct TPluginVoltageCurrentMeasurementFormat
    {
    TInt iError;                       //Error Code.
    TInt iAverageVoltageMeasurement;  //Voltage Measurement received from Plugin.
    TInt iAverageCurrentMeasurement;  //Current Measurement received from Plugin.
    };

    /**
    *  Holds a power measurement.
    */

    class TPowerVoltageCurrentMeasurement
        {
    public:
        TInt        iAverageVoltageValue;
        TInt        iAverageCurrentValue;
        TSglQueLink iSglQueLink;
        };
    
    struct TPowerVoltageCurrent
        {
        TInt        iAverageVoltage;
        TInt        iAverageCurrent;
        };
    
    public:
        
    typedef TPckgBuf<TPowerVoltageCurrent> TVoltageCurrentMeasurementsPckg;

    TPowerVoltageCurrentMeasurementsClientData();

    ~TPowerVoltageCurrentMeasurementsClientData();
    
    /**
     * Adds a Measurement to the iMeasurementsQue
     */
    virtual void AddMeasurementL(TDesC8& aMeasurement);
    
    /* 
     * The Measurements collected are written in the message area of the message.
     */
    
    virtual void WriteMeasurementsInMessageL();
    
    /**
     * Resets the object in order to be ready for a new measurements session.
     */   

    virtual void Reset();
    
    /**
     * Called if the client has registerd for measurements and a new measurement is sent by the plugin.
     */         
    
    virtual void NewBatteryMeasurementL(TDesC8& aValue, TUint aPluginInterval);

    /**
     * If at least one measurement is collected the data is written to 
     * the messasge and the client is completed. If not the 
     * message hanlde is stored and the client is completed
     * when data is ready
     */       


    virtual TBool GetMeasurementsL(RMessage2 aMessage);

    /**
     * Transfers the collecetd data from the list to a TFixedArray, appropriate for client-server
     * data trasnfer.
     */
    void TransferMeasurementsToFixedArrayL(TFixedArray<TPowerVoltageCurrent,KHWRMPowerMaxMeasurementsBatchSize> &aFixedArray);

    private:
        
    TSglQue<TPowerVoltageCurrentMeasurement>      iMeasurementsQue;               // list of the measurements
    TSglQueIter<TPowerVoltageCurrentMeasurement>  iMeasurementsQueIter;

    TInt                            iVoltageAccumulator;                   // measurements are accumulated until there are enough values to calculate the average voltage
    TInt                            iCurrentAccumulator;                   // measurements are accumulated until there are enough values to calculate the average current


    };
    
/**
* Data storage class for plugin requests.
*/
class THWRMPluginPowerIntervalRequestData : public THWRMPluginTransactionListItem
    {
    public:
    	TUint iRequestedPluginIntervalMultiple;

        /**
	    * Convenience constructor
	    */
    	THWRMPluginPowerIntervalRequestData(TUint aRequestedPluginIntervalMultiple, TInt aCommandId)
	        : THWRMPluginTransactionListItem(0, aCommandId), //TransId is updated later
	        iRequestedPluginIntervalMultiple(aRequestedPluginIntervalMultiple)
	            {
	            };
	    /**
	    * Virtual destructor.
	    */
	    virtual ~THWRMPluginPowerIntervalRequestData()
	        {
	        };
    };
		
    
#endif  // HWRMPowerSERVICE_H
            
// End of File
