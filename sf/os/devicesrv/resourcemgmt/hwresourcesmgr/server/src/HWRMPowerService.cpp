// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HWRMServer.h"
#include "HWRMService.h"
#include "HWRMPowerService.h"
#include "HWRMPluginHandler.h"
#include "HWRMPowerCommands.h"
#include "HWRMPowerClientServer.h"
#include "HWRMPower.h"
#include "HWRMtrace.h"

// CONSTANTS
_LIT( KPanicCategory, "HWRMPowerService" );

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

//All the measurements from the plugin are sent to client.
const TUint KRemainingChargingTimeInterval(1);

#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPowerService::CHWRMPowerService
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMPowerService::CHWRMPowerService(CHWRMPowerCommonData& aPowerCommonData)
    : iPowerCommonData(aPowerCommonData)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::CHWRMPowerService()" ));
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::CHWRMPowerService - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ConstructL(CHWRMPluginHandler* aPluginHandler)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ConstructL(0x%x)" ), aPluginHandler);

    __ASSERT_ALWAYS(aPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    
    BaseConstructL(aPluginHandler, NULL);
    
    iPowerVoltageCurrentMeasurementsClientData = new(ELeave) TPowerVoltageCurrentMeasurementsClientData();

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    iPowerChargingCurrentMeasurementsClientData = new(ELeave) TPowerIntMeasurementsClientData();

    iPowerChargingTimeMeasurementsClientData = new(ELeave)TPowerIntMeasurementsClientData();
    iPowerChargingTimeMeasurementsClientData->iRequestedIntervalMultiple = KRemainingChargingTimeInterval;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMPowerService* CHWRMPowerService::NewL(CHWRMPluginHandler* aPluginHandler, CHWRMPowerCommonData& aPowerCommonData)
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::NewL(0x%x)" ), aPluginHandler);

    CHWRMPowerService* self = new( ELeave ) CHWRMPowerService(aPowerCommonData);

    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler);
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::NewL - return 0x%x" ), self );

    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPowerService::~CHWRMPowerService()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::~CHWRMPowerService()" ) );

   	iPowerCommonData.CancelPendingGetBatteryInfo(*this);
   	iPowerCommonData.CancelPendingGetBatteryVoltageInfo(*this);
   	    
    if (iPowerVoltageCurrentMeasurementsClientData)
      	{
       iPowerVoltageCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
       iPowerCommonData.iPowerReportClientsData->RemoveMeasurementsClientData(iPowerVoltageCurrentMeasurementsClientData);
       
       delete iPowerVoltageCurrentMeasurementsClientData;
       iPowerVoltageCurrentMeasurementsClientData = NULL;
       
       if (iPowerCommonData.iPowerReportClientsData->IsClientsQueEmpty() &&
       	iPowerCommonData.iPowerReportClientsData->IsMeasuringOn())
       	{
       	TRAPD(err, iPowerCommonData.DoStopPowerMeasurementsL());
			if (err != KErrNone)
				{
				// Cannot do anything here. Just trace it
				COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::~CHWRMPowerService() - Stopping power(currentAndvoltage) measurement failed: %d" ), err);
				}
       	}
   	}

    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO

     if (iPowerChargingCurrentMeasurementsClientData)
       	{
        iPowerChargingCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
        iPowerCommonData.iChargingCurrentClientsData->RemoveMeasurementsClientData(iPowerChargingCurrentMeasurementsClientData);
        
        delete iPowerChargingCurrentMeasurementsClientData;
        iPowerChargingCurrentMeasurementsClientData = NULL;
        
        if (iPowerCommonData.iChargingCurrentClientsData->IsClientsQueEmpty() &&
        	iPowerCommonData.iChargingCurrentClientsData->IsMeasuringOn())
        	{
        	TRAPD(err, iPowerCommonData.DoStopChargingCurrentMeasurementsL());
			if (err != KErrNone)
				{
				// Cannot do anything here. Just trace it
				COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::~CHWRMPowerService() - Stopping charging current measurement failed: %d" ), err);
				}
        	}
    	}

     
     if (iPowerChargingTimeMeasurementsClientData)
         {
         iPowerChargingTimeMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
         iPowerCommonData.iChargingTimeClientsData->RemoveMeasurementsClientData(iPowerChargingTimeMeasurementsClientData);
         
         delete iPowerChargingTimeMeasurementsClientData;
         iPowerChargingTimeMeasurementsClientData = NULL;
         
         if (iPowerCommonData.iChargingTimeClientsData->IsClientsQueEmpty() &&
             iPowerCommonData.iChargingTimeClientsData->IsMeasuringOn())
             {
             TRAPD(err, iPowerCommonData.DoStopChargingTimeMeasurementsL());
             if (err != KErrNone)
                 {
                 // Cannot do anything here. Just trace it
                 COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::~CHWRMPowerService() - Stopping charging time measurement failed: %d" ), err);
                 }
             }
         }
     
 #endif //SYMBIAN_HWRM_EXTPOWERINFO

    // Base class cleanup
    BaseDestruct();
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::~CHWRMPowerService - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ExecuteMessageL
// Handles Power requests.  Called from the client session via CSession2::ServiceL
// -----------------------------------------------------------------------------
//
TBool CHWRMPowerService::ExecuteMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ExecuteMessageL(0x%x)" ), aMessage.Function());
    
    if ( aMessage.IsNull() )
        {
        User::Leave(KErrBadHandle);
        }

    // All messages are by default asynchronous because of the nature of the plugin API
    TBool completeMessage(EFalse);
    
    switch ( aMessage.Function() )
        {
        case EHWRMCleanupBattery:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMCleanupBattery") );  
            	
            CleanupPowerL();
            completeMessage = ETrue;
            
            break;
            }
            
        case EHWRMGetBatteryInfo:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetBatteryInfo") );

            // The plugin command will be issued by the service belonging to the PowerCommonData. The PowerCommonData
            // keep a list of all the pending requests. A plugin command is issued only for the first request. If a client
            // requests the same data and finds that there is a pending plugin request it won't issue a new one.
            // Will be completed all together when the plugin responds to the service.
            
            iPowerCommonData.DoGetBatteryInfoL(aMessage, *this);
 
            break;
            }
            
        case EHWRMGetBatteryVoltageInfo:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetBatteryVoltageInfo") );

            // The plugin command will be issued by the service belonging to the PowerCommonData. The PowerCommonData
            // keep a list of all the pending requests. A plugin command is issued only for the first request. If a client
            // requests the same data and finds that there is a pending plugin request it won't issue a new one.
            // Will be completed all together when the plugin responds to the service.
            
            iPowerCommonData.DoGetBatteryVoltageInfoL(aMessage, *this);

            break;
            }
                        
        case EHWRMStartAverageBatteryPowerReporting:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartAverageBatteryPowerReporting") );
            
            if (iPowerVoltageCurrentMeasurementsClientData->IsMeasuringOn() || 
            	iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{
            	User::Leave(KErrAlreadyExists);
            	}
            
     		TPckgBuf<TUint> intervalMultiplePckg;
         	aMessage.ReadL(0,intervalMultiplePckg);         	
        	
        	iPowerVoltageCurrentMeasurementsClientData->iRequestedIntervalMultiple = intervalMultiplePckg();
        	
            if (iPowerCommonData.iPowerReportClientsData->IsMeasuringOn())
            	{
            	COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartAverageBatteryPowerReporting: Power Reporting is already On") );                          
   	
            	// plugin is already ON.
            	// Just add it to the list and (if needed) update the plugin Interval Multiple
            	
            	if (iPowerCommonData.iPowerReportClientsData->NeedPluginIntervalAdjustment(iPowerVoltageCurrentMeasurementsClientData->iRequestedIntervalMultiple))
            		{            		
            		TUint currentPluginIntervalMultiple;
            	 	iPowerCommonData.iPowerReportClientsData->GetPluginIntervalMultiple(currentPluginIntervalMultiple);
            	 	TUint currentGCD = currentPluginIntervalMultiple;
            	 	
            	 	TUint newPluginIntervalMultiple;
            	 	TUint clientsInterval = intervalMultiplePckg();
            	 	TUint newGCD = iPowerCommonData.iPowerReportClientsData->CalculateGCD(currentGCD, clientsInterval);	
            	 	
            	 	newPluginIntervalMultiple = newGCD;
            	 	
            	 	// save the message so that the message is completed when the plugin responds
                	iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage = aMessage;
                	
            	 	SetPluginIntervalMultipleL(HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId, newPluginIntervalMultiple);
            		}
            	else
            		{
            		iPowerCommonData.iPowerReportClientsData->AddMeasurementsClientL(*iPowerVoltageCurrentMeasurementsClientData);
            		iPowerVoltageCurrentMeasurementsClientData->SetMeasuringOn();
            		
                	completeMessage = ETrue; // won't issue a plugin request. Accesses the common data
            		}
            	}
            else
            	{
            	// send command to the plugin to start Average Battery measurements
            	// if successful client will be added on the plugin response
            	iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage = aMessage;
            	
            	StartAverageBatteryPowerReportingL(aMessage);
            	}
            break;
            }

        
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        case EHWRMStartAverageChargingCurrentReporting:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartAverageChargingCurrentReporting") );
            
            if (iPowerChargingCurrentMeasurementsClientData->IsMeasuringOn() || 
            	iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{
            	User::Leave(KErrAlreadyExists);
            	}
            
     		TPckgBuf<TUint> intervalPckg;
         	aMessage.ReadL(0,intervalPckg);         	
        	
        	iPowerChargingCurrentMeasurementsClientData->iRequestedIntervalMultiple = intervalPckg();
        	
            if (iPowerCommonData.iChargingCurrentClientsData->IsMeasuringOn())
            	{
            	COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartAverageChargingCurrentReporting:  is already On") );                          
   	
            	// plugin is already ON.
            	// Just add it to the list and (if needed) update the plugin Interval Multiple
            	
            	if (iPowerCommonData.iChargingCurrentClientsData->NeedPluginIntervalAdjustment(iPowerChargingCurrentMeasurementsClientData->iRequestedIntervalMultiple))
            		{            		
            		TUint currentPluginIntervalMultiple;
            	 	iPowerCommonData.iChargingCurrentClientsData->GetPluginIntervalMultiple(currentPluginIntervalMultiple);
            	 	TUint currentGCD = currentPluginIntervalMultiple;
            	 	
            	 	TUint newPluginIntervalMultiple;
            	 	TUint clientsInterval = intervalPckg();
            	 	TUint newGCD = iPowerCommonData.iChargingCurrentClientsData->CalculateGCD(currentGCD, clientsInterval);	
            	 	
            	 	newPluginIntervalMultiple = newGCD;
            	 	
            	 	// save the message so that the message is completed when the plugin responds
                	iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage = aMessage;
                	
            	 	SetPluginIntervalMultipleL(HWRMPowerCommand::ESetChargeCurrentIntervalMultipleCmdId, newPluginIntervalMultiple);
            		}
            	else
            		{
            		iPowerCommonData.iChargingCurrentClientsData->AddMeasurementsClientL(*iPowerChargingCurrentMeasurementsClientData);
            		iPowerChargingCurrentMeasurementsClientData->SetMeasuringOn();
            		
                	completeMessage = ETrue; // won't issue a plugin request. Accesses the common data
            		}
            	}
            else
            	{
            	// send command to the plugin to start Average Battery measurements
            	// if successful client will be added on the plugin response
            	iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage = aMessage;
            	
            	StartAverageChargingCurrentReportingL(aMessage);
            	}
            break;
            }

        
        case EHWRMStartBatteryFullChargingTimeReporting:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartBatteryFullChargingTimeReporting") );
            
            if (iPowerChargingTimeMeasurementsClientData->IsMeasuringOn() || 
            	iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage.Handle())
            	{
            	User::Leave(KErrAlreadyExists);
            	}

            //Set to 1, as all measurements are sent to client.
            //The time interval is determined by plugin.
        	iPowerChargingTimeMeasurementsClientData->iRequestedIntervalMultiple = 1;
        	
            if (iPowerCommonData.iChargingTimeClientsData->IsMeasuringOn())
            	{
            	COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMStartBatteryFullChargingTimeReporting: Charging Time Reporting is already On") );                          
   	
            	// plugin is already ON.
            	// Just add it to the list as no plugin interval update is required.
            	
        		iPowerCommonData.iChargingTimeClientsData->AddMeasurementsClientL(*iPowerChargingTimeMeasurementsClientData);
        		iPowerChargingTimeMeasurementsClientData->SetMeasuringOn();
        		
            	completeMessage = ETrue; // won't issue a plugin request. Accesses the common data

            	}
            else
            	{
            	// send command to the plugin to start Battery Full Charging Time reporting
            	// if successful client will be added on the plugin response
            	iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage = aMessage;
            	
            	StartChargingTimeReportingL(aMessage);
            	}
            break;
            }
        
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        
        case EHWRMGetPowerMeasurements:
                    {
                    COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetPowerMeasurements") );
        
                    if (iPowerVoltageCurrentMeasurementsClientData->iRequestedBatchMessage.Handle())
                        {
                        // Not allowed to have more than one pending getBatch messages. 
                        User::Leave(KErrServerBusy);
                        }
                    
                    if (iPowerVoltageCurrentMeasurementsClientData->IsMeasuringOn())
                        {                    
                        // if there is a measurement ready the message is completed.
                        // if not the commondata will complete the message when ready 
                        completeMessage = iPowerVoltageCurrentMeasurementsClientData->GetMeasurementsL( aMessage );
                        }
                    else if (iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
                        {
                        // else waiting for plugin to start measurements. Don't complete message just save it.
                        iPowerVoltageCurrentMeasurementsClientData->iRequestedBatchMessage = aMessage;             
                        }
                    else 
                        {
                        // Measurements have been stoped by client or because Max Reporting period was reached
                        // Need to issue a start before retrieving measurements
                        User::Leave(KErrNotReady);
                        }       
                    
                    break;      
                    }
        

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        case EHWRMGetAverageChargingCurrentMeasurements:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetAverageChargingCurrentMeasurements") );

            if (iPowerChargingCurrentMeasurementsClientData->iRequestedBatchMessage.Handle())
            	{
            	// Not allowed to have more than one pending getBatch messages. 
            	User::Leave(KErrServerBusy);
            	}
            
        	if (iPowerChargingCurrentMeasurementsClientData->IsMeasuringOn())
        		{                    
                // if there is a measurement ready the message is completed.
                // if not the commondata will complete the message when ready 
                completeMessage = iPowerChargingCurrentMeasurementsClientData->GetMeasurementsL( aMessage );
        		}
        	else if (iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
        		{
            	// else waiting for plugin to start measurements. Don't complete message just save it.
        		iPowerChargingCurrentMeasurementsClientData->iRequestedBatchMessage = aMessage;     		
        		}
            else 
            	{
                // Measurements have been stoped by client or because Max Reporting period was reached
                // Need to issue a start before retrieving measurements
            	User::Leave(KErrNotReady);
            	}       
        	
            break;      
            }

        
        case EHWRMGetBatteryFullChargingTimeMeasurements:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetBatteryFullChargingTimeMeasurements") );

            if (iPowerChargingTimeMeasurementsClientData->iRequestedBatchMessage.Handle())
            	{
            	// Not allowed to have more than one pending getBatch messages. 
            	User::Leave(KErrServerBusy);
            	}
            
        	if (iPowerChargingTimeMeasurementsClientData->IsMeasuringOn())
        		{                    
                // if there is a measurement ready the message is completed.
                // if not the commondata will complete the message when ready 
                completeMessage = iPowerChargingTimeMeasurementsClientData->GetMeasurementsL( aMessage );
        		}
        	else if (iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage.Handle())
        		{
            	// else waiting for plugin to start measurements. Don't complete message just save it.
        		iPowerChargingTimeMeasurementsClientData->iRequestedBatchMessage = aMessage;     		
        		}
            else 
            	{
                // Measurements have been stoped by client or because Max Reporting period was reached
                // Need to issue a start before retrieving measurements
            	User::Leave(KErrNotReady);
            	}       
        	
            break;      
            }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

        case EHWRMStopAverageBatteryPowerReporting:
            {
            if (iPowerVoltageCurrentMeasurementsClientData->IsMeasuringOn() ||
                iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{    
            	// will leave if client is not found in the list. Hence, no client that has not issued
            	// a start can call stop for a measurements session
            	iPowerCommonData.iPowerReportClientsData->RemoveMeasurementsClientL(iPowerVoltageCurrentMeasurementsClientData);          
            	
            	iPowerVoltageCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrNone);
            	iPowerVoltageCurrentMeasurementsClientData->CompletePendingStart(KErrNone);
            	
            	//if there are no registered clients stop the plugin measurements            	
            	if(iPowerCommonData.iPowerReportClientsData->IsClientsQueEmpty())
            		{
            		// send command to the plugin to stop Average Battery power measurements            		
            		 iPowerCommonData.DoStopPowerMeasurementsL();	
            		}
            	else
            		{
            		TUint newIntervalMultiple;
            		TUint currentIntervalMultiple;
            		
            		iPowerCommonData.iPowerReportClientsData->RecalculatePluginInterval(newIntervalMultiple);
            		TUint newGCD = newIntervalMultiple;

            		iPowerCommonData.iPowerReportClientsData->GetPluginIntervalMultiple(currentIntervalMultiple);
            		TUint currentGCD = currentIntervalMultiple;
            		
            		if( newGCD != currentGCD)
            			{			            		
            			iPowerCommonData.DoSetPluginIntervalMultipleL(HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId, newIntervalMultiple);
            			}
            		}
            	}            
            
    		completeMessage = ETrue; // plugin request. is issued by the common data service
    		
        	iPowerVoltageCurrentMeasurementsClientData->Reset();
        	
            break;
            }
            
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        case EHWRMStopAverageChargingCurrentReporting:
            {
            if (iPowerChargingCurrentMeasurementsClientData->IsMeasuringOn() ||
                iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{    
            	// will leave if client is not found in the list. Hence, no client that has not issued
            	// a start can call stop for a measurements session
            	iPowerCommonData.iChargingCurrentClientsData->RemoveMeasurementsClientL(iPowerChargingCurrentMeasurementsClientData);          
            	
            	iPowerChargingCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrNone);
            	iPowerChargingCurrentMeasurementsClientData->CompletePendingStart(KErrNone);
            	
            	//if there are no registered clients stop the plugin measurements            	
            	if(iPowerCommonData.iChargingCurrentClientsData->IsClientsQueEmpty())
            		{
            		// send command to the plugin to stop Average charge current measurements            		
            		 iPowerCommonData.DoStopChargingCurrentMeasurementsL();	
            		}
            	else
            		{
            		TUint newIntervalMultiple;
            		TUint currentIntervalMultiple;
            		
            		iPowerCommonData.iChargingCurrentClientsData->RecalculatePluginInterval(newIntervalMultiple);
            		TUint newGCD = newIntervalMultiple;

            		iPowerCommonData.iChargingCurrentClientsData->GetPluginIntervalMultiple(currentIntervalMultiple);
            		TUint currentGCD = currentIntervalMultiple;
            		
            		if( newGCD != currentGCD)
            			{			            		
            			iPowerCommonData.DoSetPluginIntervalMultipleL(HWRMPowerCommand::ESetChargeCurrentIntervalMultipleCmdId, newIntervalMultiple);
            			}
            		}
            	}            
            
    		completeMessage = ETrue; // plugin request. is issued by the common data service
    		
        	iPowerChargingCurrentMeasurementsClientData->Reset();
        	
            break;
            }

        
        case EHWRMStopBatteryFullChargingTimeReporting:
            {
            if (iPowerChargingTimeMeasurementsClientData->IsMeasuringOn() ||
                iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage.Handle())
            	{    
            	// will leave if client is not found in the list. Hence, no client that has not issued
            	// a start can call stop for a measurements session
            	iPowerCommonData.iChargingTimeClientsData->RemoveMeasurementsClientL(iPowerChargingTimeMeasurementsClientData);          
            	
            	iPowerChargingTimeMeasurementsClientData->CompleteGetMeasurementsMessage(KErrNone);
            	iPowerChargingTimeMeasurementsClientData->CompletePendingStart(KErrNone);
            	
            	//if there are no registered clients stop the plugin measurements            	
            	if(iPowerCommonData.iChargingTimeClientsData->IsClientsQueEmpty())
            		{
            		// send command to the plugin to stop Average Battery voltage measurements            		
            		 iPowerCommonData.DoStopChargingTimeMeasurementsL();	
            		}
            	}            
            
    		completeMessage = ETrue; // plugin request. is issued by the common data service
    		
        	iPowerChargingTimeMeasurementsClientData->Reset();
        	
            break;
            }

#endif //SYMBIAN_HWRM_EXTPOWERINFO
                     
        case EHWRMCancelAveragePowerReporting:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMCancelAveragePowerReporting") );
            
            if (iPowerVoltageCurrentMeasurementsClientData->IsMeasuringOn() ||
           		iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{
            	iPowerVoltageCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();
            	}       

            completeMessage = ETrue; // no responses from plugin cancel commands
            break;
            }

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

         case EHWRMCancelAverageChargingCurrentReporting:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMCancelAverageChargingCurrentReporting") );
            
            if (iPowerChargingCurrentMeasurementsClientData->IsMeasuringOn() ||
           		iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage.Handle())
            	{
            	iPowerChargingCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();
            	}       

            completeMessage = ETrue; // no responses from plugin cancel commands
            break;
            }

         
         case EHWRMCancelBatteryFullChargingTimeReporting:
             {
             COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMCancelBatteryFullChargingTimeReporting") );
             
             if (iPowerChargingTimeMeasurementsClientData->IsMeasuringOn() ||
                 iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage.Handle())
                 {
                 iPowerChargingTimeMeasurementsClientData->CancelPendingGetMeasurementsMessage();
                 }       
         
             completeMessage = ETrue; // no responses from plugin cancel commands
             break;
             }

        case EHWRMGetBatteryChargerType:
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMPowerService::ExecuteMessageL - EHWRMGetBatteryChargerType") );

            // The plugin command will be issued by the service belonging to the PowerCommonData. The PowerCommonData
            // keep a list of all the pending requests. A plugin command is issued only for the first request. If a client
            // requests the same data and finds that there is a pending plugin request it won't issue a new one.
            // Will be completed all together when the plugin responds to the service.
            
            iPowerCommonData.DoGetChargerTypeL(aMessage, *this);
 

            break;
            }

#endif //SYMBIAN_HWRM_EXTPOWERINFO
            
        // Cannot identify the message, panic the client
        default:
            {
            CHWRMServer::PanicClient(aMessage, EPanicIllegalFunction);
            
            break;
            }            
        }

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ExecuteMessageL - return 0x%x" ), completeMessage );

    return completeMessage;
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::GetBatteryInfoL
// Sends a plug-in command to query the battery info
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::GetBatteryInfoL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetBatteryInfoL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EGetBatteryInfoCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetBatteryInfoL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::GetBatteryVoltageInfoL
// Sends a plug-in command to query the battery voltage info
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::GetBatteryVoltageInfoL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetBatteryVoltageInfoL" ) );

	ExecutePluginCommandL(aMessage, HWRMPowerCommand::EGetBatteryVoltageInfoCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetBatteryVoltageInfoL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::StartAverageBatteryVoltageReportingL
// Sends a plug-in command to start the average charge rate reporting
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StartAverageBatteryPowerReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartAverageBatteryPowerReportingL" ) );


    TPckgBuf<TUint> intervalMultiplePckg;
    aMessage.ReadL(0,intervalMultiplePckg);

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId, EFalse, intervalMultiplePckg);

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartAverageBatteryPowerReportingL - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMPowerService::StopAverageBatteryPowerReportingL
// Sends a plug-in command to stop the average power reporting
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopAverageBatteryPowerReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopAverageBatteryPowerReportingL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopAverageBatteryPowerReportingL - return" ) );
    }


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
// -----------------------------------------------------------------------------
// CHWRMPowerService::StartAverageBatteryVoltageReportingL
// Sends a plug-in command to start the average charge rate reporting
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StartAverageChargingCurrentReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartAverageChargingCurrentReportingL" ) );


    TPckgBuf<TUint> intervalMultiplePckg;
    aMessage.ReadL(0,intervalMultiplePckg);

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId, EFalse, intervalMultiplePckg);

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartAverageChargingCurrentReportingL - return" ) );
    }



// -----------------------------------------------------------------------------
// CHWRMPowerService::StopAverageChargingCurrentReportingL
// Sends a plug-in command to stop the average battery voltage reporting
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopAverageChargingCurrentReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopAverageChargingCurrentReportingL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopAverageChargingCurrentReportingL - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMPowerService::StartChargingTimeReportingL
// Sends a plug-in command to start the battery full charging time reporing
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StartChargingTimeReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartChargingTimeReportingL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId, EFalse);

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StartChargingTimeReportingL - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMPowerService::StopChargingTimeReportingL
// Sends a plug-in command to stop the battery full charging time reporing
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopChargingTimeReportingL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopChargingTimeReportingL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::StopChargingTimeReportingL - return" ) );
    }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMPowerService::ProcessResponseL
// Handles responses to Power requests.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout )
    {
    COMPONENT_TRACE4( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL(%d , %d, <data>, %d)" ), aCommandId, aTransId, aTimeout );

    TInt pluginErr(KErrNone);     	// Error came from plugin as data (or timeout). Used to complete RMessage.
    TInt contextErr(KErrNone);    	// Error in context, i.e. bad handle or descriptor. Used to leave.
    TBool completeRequest(ETrue);	// All messages by default are completed by the HWRMService. The data stored 
    								// in the transaction list for each plugin request are also deleted by 
    								// the HWRMService.In the case that the request was not initiated by a client 
    								// but by the PowerCommonData (using a dummy RMessage2) we handle the 
    								// completion of the request and destruction of the data locally.    
    if ( aTimeout)
        {
        pluginErr = KErrTimedOut;            
        }
    else if ( CheckResponseDataSize(aCommandId, aData) == EFalse )
        {
        pluginErr = KErrBadDescriptor;
        }
    else 
        {
        // Handle the response                      
        switch ( aCommandId )
            {
            case HWRMPowerCommand::EGetBatteryInfoCmdId:
                {
                completeRequest = EFalse;
            	// The message will be completed here so remove and delete the data from the transaction list. 
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, ETrue));
                if (data)
                	{
                    delete data;
                    data = NULL;
                	}
                
                // Unpack the battery info
                HWRMPowerCommand::TBatteryConsumptionResponsePackage consumptionDataPckg;
                consumptionDataPckg.Copy(aData);
                HWRMPowerCommand::TBatteryConsumptionResponseData consumptionData = consumptionDataPckg();
                
                pluginErr = consumptionData.iErrorCode;
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EGetBatteryConsumptionInfoCmdId error = %d" ), pluginErr );
                
                if ( pluginErr == KErrNone )
                	{                	
                    CHWRMPower::TBatteryConsumptionData clientConsumptionData;
                    clientConsumptionData.iNominalCapacity = consumptionData.iNominalCapacity;
                    clientConsumptionData.iRemainingCapacity = consumptionData.iRemainingCapacity;
                    clientConsumptionData.iRemainingPercentageCapacity = consumptionData.iRemainingPercentageCapacity;

                    TPckg<CHWRMPower::TBatteryConsumptionData> clientConsumptionPckg(clientConsumptionData);

                    // Complete and remove from the respective list all the clients (if any, beacuse some may have closed the session)
                    // that have requested Battery Info.
	                iPowerCommonData.CompletePendingGetBatteryInfo(pluginErr, clientConsumptionPckg);
                	}
                else
                	{              
                    // Complete the the clients with the plugin error
                	CHWRMPower::TBatteryConsumptionData clientConsumptionData;
                	TPckg<CHWRMPower::TBatteryConsumptionData> emptyDes(clientConsumptionData);
                	iPowerCommonData.CompletePendingGetBatteryInfo(pluginErr, emptyDes);
	                }
                }        		
                break;

            case HWRMPowerCommand::EGetBatteryVoltageInfoCmdId:
                {
                completeRequest = EFalse;
            	// The message will be completed here so remove and delete the data from the transaction list.
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, ETrue));
                if (data)
                	{
                	delete data;
        			data = NULL;
                	}
                
                // Unpack the battery info
                HWRMPowerCommand::TBatteryVoltageResponsePackage voltageDataPckg;
                voltageDataPckg.Copy(aData);
                HWRMPowerCommand::TBatteryVoltageResponseData voltageData = voltageDataPckg();
                
                pluginErr = voltageData.iErrorCode;
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EGetBatteryVoltageInfoCmdId error = %d" ), pluginErr );
                
                if ( pluginErr == KErrNone )
                	{                	
                    CHWRMPower::TBatteryVoltageData clientVoltageData;
                    clientVoltageData.iMaximumVoltage = voltageData.iMaximumVoltage;
                    clientVoltageData.iMinimumVoltage = voltageData.iMinimumVoltage;
                    clientVoltageData.iRemainingVoltage = voltageData.iRemainingVoltage;

                    TPckg<CHWRMPower::TBatteryVoltageData> clientVoltagePckg(clientVoltageData);
                    
                    // Complete and remove from the respective list all the clients (if any, beacuse some may have closed the session)
                    // that have requested Battery Info.              
	                iPowerCommonData.CompletePendingGetBatteryVoltageInfo(pluginErr, clientVoltagePckg);
                	}
                else
                	{              
                    // Complete the the clients with the plugin error
                	CHWRMPower::TBatteryVoltageData clientVoltageData;
                	TPckg<CHWRMPower::TBatteryVoltageData> emptyDes(clientVoltageData);
                	iPowerCommonData.CompletePendingGetBatteryVoltageInfo(pluginErr, emptyDes);
	                }
                }
                break;
         
            case HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStartAverageBatteryPowerReportingCmdId error = %d" ), pluginErr );
            	
            	if (pluginErr == KErrNone)
            		{            		
	            	iPowerCommonData.iPowerReportClientsData->SetMeasuringOn();
	            	
	                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
	                
	                if ( data )
	                	{               
	                	// client is added to the voltage clients' list
	                	iPowerCommonData.iPowerReportClientsData->AddMeasurementsClientL(*iPowerVoltageCurrentMeasurementsClientData);
		            	iPowerCommonData.iPowerReportClientsData->UpdatePluginIntervalMultiple(iPowerVoltageCurrentMeasurementsClientData->iRequestedIntervalMultiple);
	                	iPowerVoltageCurrentMeasurementsClientData->SetMeasuringOn();          
	                	
						// Ignore Coverity false positive as uninitialized variable.
						// Since no response is sent back to the client, only a empty message is created here. 
						// Message handle of this will be initialized to NULL in the default constructor.
						// coverity[var_decl]
	                	RMessage2 dummy;
	                	iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage = dummy;
	                    }
            		}
            	else
            		{
            		// complete any pending GetMeasurement request with KErrGeneral
            		if (iPowerVoltageCurrentMeasurementsClientData->iRequestedBatchMessage.Handle())
            			{
            			iPowerVoltageCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrGeneral);
            			
            			iPowerVoltageCurrentMeasurementsClientData->Reset();
            			}
            		}
            	}
            break;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
            
            case HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStartAverageBatteryVoltageReportingCmdId error = %d" ), pluginErr );
            	
            	if (pluginErr == KErrNone)
            		{            		
	            	iPowerCommonData.iChargingCurrentClientsData->SetMeasuringOn();
	            	
	                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
	                
	                if ( data )
	                	{               
	                	// client is added to the voltage clients' list
	                	iPowerCommonData.iChargingCurrentClientsData->AddMeasurementsClientL(*iPowerChargingCurrentMeasurementsClientData);
		            	iPowerCommonData.iChargingCurrentClientsData->UpdatePluginIntervalMultiple(iPowerChargingCurrentMeasurementsClientData->iRequestedIntervalMultiple);
	                	iPowerChargingCurrentMeasurementsClientData->SetMeasuringOn();          
	                	
						// Ignore Coverity false positive as uninitialized variable.
						// Since no response is sent back to the client, only a empty message is created here. 
						// Message handle of this will be initialized to NULL in the default constructor.
						// coverity[var_decl]
	                	RMessage2 dummy;
	                	iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage = dummy;
	                    }
            		}
            	else
            		{
            		// complete any pending GetMeasurement request with KErrGeneral
            		if (iPowerChargingCurrentMeasurementsClientData->iRequestedBatchMessage.Handle())
            			{
            			iPowerChargingCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrGeneral);
            			
            			iPowerChargingCurrentMeasurementsClientData->Reset();
            			}
            		}
            	}
            break;

            
            case HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStartBatteryFullChargingTimeReportingCmdId error = %d" ), pluginErr );
            	
            	if (pluginErr == KErrNone)
            		{            		
	            	iPowerCommonData.iChargingTimeClientsData->SetMeasuringOn();
	            	
	                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
	                
	                if ( data )
	                	{               
	                	// client is added to the voltage clients' list
	                	iPowerCommonData.iChargingTimeClientsData->AddMeasurementsClientL(*iPowerChargingTimeMeasurementsClientData);
		            	iPowerCommonData.iChargingTimeClientsData->UpdatePluginIntervalMultiple(iPowerChargingTimeMeasurementsClientData->iRequestedIntervalMultiple);
	                	iPowerChargingTimeMeasurementsClientData->SetMeasuringOn();          
	                	
						// Ignore Coverity false positive as uninitialized variable.
						// Since no response is sent back to the client, only a empty message is created here. 
						// Message handle of this will be initialized to NULL in the default constructor.
						// coverity[var_decl]
	                	RMessage2 dummy;
	                	iPowerChargingTimeMeasurementsClientData->iRequestedStartMessage = dummy;
	                    }
            		}
            	else
            		{
            		// complete any pending GetMeasurement request with KErrGeneral
            		if (iPowerChargingTimeMeasurementsClientData->iRequestedBatchMessage.Handle())
            			{
            			iPowerChargingTimeMeasurementsClientData->CompleteGetMeasurementsMessage(KErrGeneral);
            			
            			iPowerChargingTimeMeasurementsClientData->Reset();
            			}
            		}
            	}
            break;

#endif //SYMBIAN_HWRM_EXTPOWERINFO
            
            case HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStopAverageBatteryPowerReportingCmdId error = %d" ), pluginErr);
            	
            	if (pluginErr == KErrNone)
            		{
            		iPowerCommonData.iPowerReportClientsData->SetMeasuringOff(); 
            		}
            	}
            break;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO            
            
            case HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStopAverageBatteryChargingCurrentReportingCmdId error = %d" ), pluginErr);
            	
            	if (pluginErr == KErrNone)
            		{
            		iPowerCommonData.iChargingCurrentClientsData->SetMeasuringOff(); 
            		}
            	}
            break;

            
            case HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId:
            	{
            	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EStopBatteryFullChargingTimeReportingCmdId error = %d" ), pluginErr);
            	
            	if (pluginErr == KErrNone)
            		{
            		iPowerCommonData.iChargingTimeClientsData->SetMeasuringOff(); 
            		}
            	}
            break;

#endif //SYMBIAN_HWRM_EXTPOWERINFO
            
            case HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId:
            	{
            	completeRequest = EFalse;
            	
               	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, ESetPowerReportingIntervalMultipleCmdId error = %d" ), pluginErr);

            	TUint prevIntervalMultiple;
        		iPowerCommonData.iPowerReportClientsData->GetPluginIntervalMultiple(prevIntervalMultiple);

            	THWRMPluginPowerIntervalRequestData* data = static_cast<THWRMPluginPowerIntervalRequestData*>(iTransactionList->FindTransaction(aTransId, ETrue));
        		
            	if (data)
            		{
	            	if (pluginErr == KErrNone)
	            		{        	
	            		// the new interval is smaller ==> client was added that caused a drop of the GCD.
	            		// Else no message needs to be completed since the client's stop command was
	            		// completed earlier
	            		if (prevIntervalMultiple >= data->iRequestedPluginIntervalMultiple)
	            			{	            			
	            			iPowerVoltageCurrentMeasurementsClientData->SetMeasuringOn();
	            			iPowerVoltageCurrentMeasurementsClientData->CompletePendingStart(KErrNone);
	            			if(iPowerVoltageCurrentMeasurementsClientData->iMeasurementsCount > 0)
	            				{
	            				iPowerVoltageCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrNone);
	            				}
	            			iPowerCommonData.iPowerReportClientsData->AddMeasurementsClientL(*iPowerVoltageCurrentMeasurementsClientData);
	            			
							// Ignore Coverity false positive as uninitialized variable.
							// Since no response is sent back to the client, only a empty message is created here. 
							// Message handle of this will be initialized to NULL in the default constructor.
							// coverity[var_decl]
		                	RMessage2 dummy;
		                	iPowerVoltageCurrentMeasurementsClientData->iRequestedStartMessage = dummy;
	            			}
	            		// in all cases update the plugin interval
	                    iPowerCommonData.iPowerReportClientsData->UpdatePluginIntervalMultiple(data->iRequestedPluginIntervalMultiple);            		
	            		}            		
	               	else
	            		{
	            		if (prevIntervalMultiple >= data->iRequestedPluginIntervalMultiple)
	            			{
	            			//Complete both the pending Start and GetMeasurements with Error
	            			iPowerVoltageCurrentMeasurementsClientData->CompletePendingStart(pluginErr);
	            			iPowerVoltageCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrGeneral);
	            			
	            			iPowerVoltageCurrentMeasurementsClientData->Reset();
	            			}
	            		}
	            	
	            	delete data;
	            	data = NULL;
            		}
            	}
            break;          

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
            
            case HWRMPowerCommand::ESetChargeCurrentIntervalMultipleCmdId:
            	{
            	completeRequest = EFalse;
            	
               	HWRMPowerCommand::TErrorCodeResponsePackage respPckg;
            	respPckg.Copy(aData);
            	pluginErr = respPckg();
                
            	COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, ESetChargeCurrentIntervalMultipleCmdId error = %d" ), pluginErr);

            	TUint prevIntervalMultiple;
        		iPowerCommonData.iChargingCurrentClientsData->GetPluginIntervalMultiple(prevIntervalMultiple);

            	THWRMPluginPowerIntervalRequestData* data = static_cast<THWRMPluginPowerIntervalRequestData*>(iTransactionList->FindTransaction(aTransId, ETrue));
        		
            	if (data)
            		{
	            	if (pluginErr == KErrNone)
	            		{        	
	            		// the new interval is smaller ==> client was added that caused a drop of the GCD.
	            		// Else no message needs to be completed since the client's stop command was
	            		// completed earlier
	            		if (prevIntervalMultiple >= data->iRequestedPluginIntervalMultiple)
	            			{	            			
	            			iPowerChargingCurrentMeasurementsClientData->SetMeasuringOn();
	            			iPowerChargingCurrentMeasurementsClientData->CompletePendingStart(KErrNone);
	            			if(iPowerChargingCurrentMeasurementsClientData->iMeasurementsCount > 0)
	            				{
	            				iPowerChargingCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrNone);
	            				}
	            			iPowerCommonData.iChargingCurrentClientsData->AddMeasurementsClientL(*iPowerChargingCurrentMeasurementsClientData);
	            			
							// Ignore Coverity false positive as uninitialized variable.
							// Since no response is sent back to the client, only a empty message is created here. 
							// Message handle of this will be initialized to NULL in the default constructor.
							// coverity[var_decl]
		                	RMessage2 dummy;
		                	iPowerChargingCurrentMeasurementsClientData->iRequestedStartMessage = dummy;
	            			}
	            		// in all cases update the plugin interval
	                    iPowerCommonData.iChargingCurrentClientsData->UpdatePluginIntervalMultiple(data->iRequestedPluginIntervalMultiple);            		
	            		}            		
	               	else
	            		{
	            		if (prevIntervalMultiple >= data->iRequestedPluginIntervalMultiple)
	            			{
	            			//Complete both the pending Start and GetMeasurements with Error
	            			iPowerChargingCurrentMeasurementsClientData->CompletePendingStart(pluginErr);
	            			iPowerChargingCurrentMeasurementsClientData->CompleteGetMeasurementsMessage(KErrGeneral);
	            			
	            			iPowerChargingCurrentMeasurementsClientData->Reset();
	            			}
	            		}
	            	
	            	delete data;
	            	data = NULL;
            		}
            	}
            break;            

            case HWRMPowerCommand::EGetBatteryChargerTypeCmdId:
                {
                completeRequest = EFalse;
            	// The message will be completed here so remove and delete the data from the transaction list. 
                THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->FindTransaction(aTransId, ETrue));
                if (data)
                	{
                    delete data;
                    data = NULL;
                	}
                
                HWRMPowerCommand::TBatteryChargerTypeResponsePackage chargerTypePckg;
                chargerTypePckg.Copy(aData);
                HWRMPowerCommand::TBatteryChargerTypeData chargerTypeData = chargerTypePckg();
                
                pluginErr = chargerTypeData.iErrorCode;
                COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL, EGetBatteryConsumptionInfoCmdId error = %d" ), pluginErr );
                
                if ( pluginErr == KErrNone )
                	{                	
                    CHWRMPower::TBatteryChargerType chargerType;

                    //client interface and plugin interface are same.
                    chargerType = (CHWRMPower::TBatteryChargerType)chargerTypeData.iChargerType;
              
                    TPckg<CHWRMPower::TBatteryChargerType> clientChargerTypePckg(chargerType);

                    // Complete and remove from the respective list all the clients (if any, beacuse some may have closed the session)
                    // that have requested charger type Info.
	                iPowerCommonData.CompletePendingGetChargerType(pluginErr, clientChargerTypePckg);
                	}
                else
                	{              
                    // Complete the the clients with the plugin error
                	CHWRMPower::TBatteryChargerType chargerType(CHWRMPower::EBatteryChargerUnKnown);
                	TPckg<CHWRMPower::TBatteryChargerType> emptyDes(chargerType);
                	iPowerCommonData.CompletePendingGetChargerType(pluginErr, emptyDes);
	                }
                }        		
                break;

#endif //SYMBIAN_HWRM_EXTPOWERINFO
            
            default:
                {
                }
                break;              
            }
        }

    if (completeRequest)
    	{
    	contextErr = CompleteRequest(aTransId, pluginErr);    
    	}
    
    // Leave if there is error in context    
    User::LeaveIfError(contextErr);

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ProcessResponseL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::CheckResponseDataSize
// Helper for ProcessResponseL.  Checks data size returned in plugin responses.
// -----------------------------------------------------------------------------
//
TBool CHWRMPowerService::CheckResponseDataSize( TInt aCommandId, TDesC8& aData )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMPowerService::CheckResponseDataSize(0x%x)" ), aCommandId);      

    TInt requiredDataSize;

    switch ( aCommandId )
        {       
        case HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId:
        case HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId:
        case HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId:
            
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        case HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId:
        case HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId:
        case HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId:
        case HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId:            
        case HWRMPowerCommand::ESetChargeCurrentIntervalMultipleCmdId:
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        
        	{
        	requiredDataSize = sizeof(TInt); // TErrorCodeResponseData
        	}
        	break;
        	
        case HWRMPowerCommand::EGetBatteryInfoCmdId:
            requiredDataSize = sizeof(HWRMPowerCommand::TBatteryConsumptionResponseData);
            break;

        case HWRMPowerCommand::EGetBatteryVoltageInfoCmdId:
        	requiredDataSize = sizeof(HWRMPowerCommand::TBatteryVoltageResponseData);
        	break;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

        case HWRMPowerCommand::EGetBatteryChargerTypeCmdId:
            requiredDataSize = sizeof(HWRMPowerCommand::TBatteryChargerTypeData);
            break;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        	
        case HWRMPowerCommand::ENoCommandId:
        default:
            requiredDataSize = 0;
            break;
        }
    
    if ( aData.Size() != requiredDataSize )
        {
        COMPONENT_TRACE3( _L( "HWRM Server - CHWRMPowerService::CheckResponseDataSize error 0x%x, 0x%x" ), aData.Size(), requiredDataSize );
        return EFalse; // Data size is invalid
        }     

    return ETrue; // Data size is ok
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::StopPowerMeasurementsL
// Issues a plugin command to stop the power measurements. 
// Used when we need to stop the plugin without associating the plugin command
// with a client message.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopPowerMeasurementsL()    
    {   
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::StopPowerMeasurementsL()" ) );

    RMessage2 dummy;
    StopAverageBatteryPowerReportingL(dummy );

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::StopPowerMeasurementsL - return" ) );
    }

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMPowerService::StopChargingCurrentMeasurementsL
// Issues a plugin command to stop the Charging Current measurements. 
// Used when we need to stop the plugin without associating the plugin command
// with a client message.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopChargingCurrentMeasurementsL()    
    {   
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::StopChargingCurrentMeasurementsL()" ) );

    RMessage2 dummy;
    StopAverageChargingCurrentReportingL(dummy );

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::StopChargingCurrentMeasurementsL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::StopChargingTimeMeasurementsL
// Issues a plugin command to stop the BatteryFull charging time measurements. 
// Used when we need to stop the plugin without associating the plugin command
// with a client message.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::StopChargingTimeMeasurementsL()    
    {   
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::StopChargingTimeMeasurementsL()" ) );

    RMessage2 dummy;
    StopChargingTimeReportingL(dummy );

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::StopChargingTimeMeasurementsL - return" ) );
    }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMPowerService::SetPluginIntervalMultipleL
// Sets the Measurements Interval Multiple at the plugin
// Used when we need to stop the plugin without associating the plugin command
// with a client message.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::SetPluginIntervalMultipleL(HWRMPowerCommand::TPowerCmd aCommandId, TUint aIntervalMultiple)    
    {   
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPowerService::SetPluginIntervalMultipleL(0x%x)" ), aIntervalMultiple);

    TPckgBuf<TUint> intervalMultiplePackage(aIntervalMultiple);

    // Create new data 
    THWRMPluginPowerIntervalRequestData* data = new (ELeave) THWRMPluginPowerIntervalRequestData(aIntervalMultiple, aCommandId);
    CleanupStack::PushL( data );

    data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, intervalMultiplePackage, this);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::SetPluginIntervalMultipleL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::SuspendSubResource
// Suspends Power target
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::SuspendSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::SuspendSubResource" ) );
    iSuspended = ETrue;
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ResumeSubResource
// Resumes suspended Power target 
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ResumeSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ResumeSubResource" ) );
    iSuspended = EFalse;
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ActivateSubResource
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ActivateSubResource(TInt /*aSubResource*/, TBool /*aActivate*/)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ActivateSubResource" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMPowerService::DoRelease
// Helper for releasing Power resource
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::CleanupPowerL() //DoRelease
    {    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::CleanupPowerL" ) );    
    // Cancel any pending requests of the client and remove all the 
    // related data

   	iPowerCommonData.CancelPendingGetBatteryInfo(*this);
   	iPowerCommonData.CancelPendingGetBatteryVoltageInfo(*this);
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    iPowerCommonData.CancelPendingGetChargerType(*this);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
   	
    iPowerVoltageCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
	iPowerCommonData.iPowerReportClientsData->RemoveMeasurementsClientData(iPowerVoltageCurrentMeasurementsClientData);
        
	iPowerVoltageCurrentMeasurementsClientData->Reset();
        
	if (iPowerCommonData.iPowerReportClientsData->IsClientsQueEmpty() &&
	    iPowerCommonData.iPowerReportClientsData->IsMeasuringOn())
	    {
	    iPowerCommonData.DoStopPowerMeasurementsL();
	    }

    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
	iPowerChargingCurrentMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
	iPowerCommonData.iChargingCurrentClientsData->RemoveMeasurementsClientData(iPowerChargingCurrentMeasurementsClientData);
        
	iPowerChargingCurrentMeasurementsClientData->Reset();
        
	if (iPowerCommonData.iChargingCurrentClientsData->IsClientsQueEmpty() &&
	    iPowerCommonData.iChargingCurrentClientsData->IsMeasuringOn())
	    {
	    iPowerCommonData.DoStopChargingCurrentMeasurementsL();
	    }

    
	iPowerChargingTimeMeasurementsClientData->CancelPendingGetMeasurementsMessage();           
	iPowerCommonData.iChargingTimeClientsData->RemoveMeasurementsClientData(iPowerChargingTimeMeasurementsClientData);
        
	iPowerChargingTimeMeasurementsClientData->Reset();
        
	if (iPowerCommonData.iChargingTimeClientsData->IsClientsQueEmpty() &&
	    iPowerCommonData.iChargingTimeClientsData->IsMeasuringOn())
	    {
	    iPowerCommonData.DoStopChargingTimeMeasurementsL();
	    }
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    iSuspended = EFalse;        
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerService::CleanupPowerL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ExecutePluginCommandL(const RMessage2& aMessage,
                                             HWRMPowerCommand::TPowerCmd  aCommandId,
                                             TBool aSplitCommand )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ExecutePluginCommandL" ) );       
        
    // Create new data (TransId is updated later) 
    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, aCommandId, aSplitCommand);
    CleanupStack::PushL( data );

    TBuf8<1> emptyDes;    
    data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, emptyDes, this);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ExecutePluginCommandL - return" ) );        
    }

// -----------------------------------------------------------------------------
// CHWRMPowerService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::ExecutePluginCommandL(const RMessage2& aMessage,
                                             HWRMPowerCommand::TPowerCmd aCommandId,
                                             TBool aSplitCommand,
                                             TDesC8& aPckg )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ExecutePluginCommandL with pckg" ) );         
        
    // Create new data (TransId is updated later) 
    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, aCommandId, aSplitCommand);
    CleanupStack::PushL( data );    
    
    data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, aPckg, this);

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::ExecutePluginCommandL - return" ) );     
    }

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMPowerService::GetChargerTypeL
// Sends a plug-in command to query the charger type.
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::GetChargerTypeL(const RMessage2& aMessage)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetChargerTypeL" ) );

    ExecutePluginCommandL(aMessage, HWRMPowerCommand::EGetBatteryChargerTypeCmdId, EFalse/*not split*/ );

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::GetChargerTypeL - return" ) );
    }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::TPowerMeasurementsClientData
// C++ Constructor
// -----------------------------------------------------------------------------
//
TPowerMeasurementsClientData::TPowerMeasurementsClientData()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::TPowerMeasurementsClientData()" ) );
	
	iNumOfBaseIntervalsPassed = 0;
	iMeasuring = EFalse;
	iMeasurementsCount = 0;
	// Ignore Coverity false positive as uninitialized variable.
	// Since no response is sent back to the client, only a empty message is created here. 
	// Message handle of this will be initialized to NULL in the default constructor.
	// coverity[var_decl]
 	RMessage2 dummy;
 	iRequestedBatchMessage= dummy;
 	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::TPowerMeasurementsClientData - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::~TPowerMeasurementsClientData
// Destructor
// -----------------------------------------------------------------------------
//
TPowerMeasurementsClientData::~TPowerMeasurementsClientData()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::~TPowerMeasurementsClientData()" ) );
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::~TPowerMeasurementsClientData - return" ) );
	}



// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::IsMeasuringOn
// Check if voltage measuring is on.
// -----------------------------------------------------------------------------
//
TBool TPowerMeasurementsClientData::IsMeasuringOn()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::IsMeasuringOn()" ) );
	
	COMPONENT_TRACE2( _L( "HWRM Server - TPowerMeasurementsClientData::IsMeasuringOn - return %d" ), iMeasuring );	

	return iMeasuring;
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::SetMeasuringOn
// Set the measuring flag on.
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::SetMeasuringOn()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::SetMeasuringOn()" ) );

	iMeasuring = ETrue;
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::SetMeasuringOn - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::SetMeasuringOff
// Set the measuring flag off.
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::SetMeasuringOff()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::SetMeasuringOff()" ) );
	
	iMeasuring = EFalse;
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::SetMeasuringOff - return" ) );
	}


// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::CompleteGetMeasurementsMessage
// If there exists a cached message of the client requesting measuremnts,
// it is completed with Error code aReason
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::CompleteGetMeasurementsMessage(TInt aReason)
	{
	TInt err = KErrNone;
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CompleteGetMeasurementsMessage()" ) );
	
    if (iRequestedBatchMessage.Handle())
    	{
   		TRAP(err, WriteMeasurementsInMessageL());
		if (err != KErrNone)
			{
			// Error in updating the reply. Pass it to the client
			aReason = err;
			}
   		iRequestedBatchMessage.Complete(aReason);
   		
		// Ignore Coverity false positive as uninitialized variable.
		// Since no response is sent back to the client, only a empty message is created here. 
		// Message handle of this will be initialized to NULL in the default constructor.
		// coverity[var_decl]
   		RMessage2 dummy;
   		iRequestedBatchMessage = dummy;
   		
   	   	//reset the data to be ready for gathering the next batch pf measurements
		iMeasurementsCount = 0;
    	}
    
    COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CompleteGetMeasurementsMessage - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::CancelPendingGetMeasurementsMessage
// Cancels a pending GetMeasurements requests that belongs to the client that sent aMessage.
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::CancelPendingGetMeasurementsMessage()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CancelPendingGetMeasurementsMessage()" ) );
	
    if (iRequestedBatchMessage.Handle())
    	{
    	iRequestedBatchMessage.Complete(KErrCancel);
 		
    	//reset the data to be ready for gathering the next batch pf measurements
		iMeasurementsCount = 0;
		
		// Ignore Coverity false positive as uninitialized variable.
		// Since no response is sent back to the client, only a empty message is created here. 
		// Message handle of this will be initialized to NULL in the default constructor.
		// coverity[var_decl]
 		RMessage2 dummy;
 		iRequestedBatchMessage = dummy;
    	}
    
    COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CancelPendingGetMeasurementsMessage - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::CompletePendingStartWithError
// If there exists a cached message of the client requesting to Start measuremnts, 
// it is completed with Error code aReason
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::CompletePendingStart(TInt aReason)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CompletePendingStart()" ) );
	
	if (iRequestedStartMessage.Handle())
		{
		iRequestedStartMessage.Complete(aReason);
 	
		// Ignore Coverity false positive as uninitialized variable.
		// Since no response is sent back to the client, only a empty message is created here. 
		// Message handle of this will be initialized to NULL in the default constructor.
		// coverity[var_decl]
		RMessage2 dummy;
		iRequestedStartMessage = dummy;
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::CompletePendingStart - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::CompletePendingStartWithError
// Resets the object in order to be ready for a new measurements session.
// -----------------------------------------------------------------------------
//
void TPowerMeasurementsClientData::Reset()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::Reset()" ) );
	
	iNumOfBaseIntervalsPassed = 0;
	iMeasuring = EFalse;
	iMeasurementsCount = 0;
	// Ignore Coverity false positive as uninitialized variable.
	// Since no response is sent back to the client, only a empty message is created here. 
	// Message handle of this will be initialized to NULL in the default constructor.
	// coverity[var_decl]
 	RMessage2 dummy;
 	iRequestedBatchMessage= dummy;
 	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::Reset - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::TPowerIntMeasurementsClientData
// C++ Constructor
// -----------------------------------------------------------------------------
//
TPowerIntMeasurementsClientData::TPowerIntMeasurementsClientData()
	:iMeasurementsQue(_FOFF(TPowerIntMeasurement,iSglQueLink)),
	iMeasurementsQueIter(iMeasurementsQue)
    {
	iAccumulator = 0;
    }

// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::~TPowerIntMeasurementsClientData
// Destructor
// -----------------------------------------------------------------------------
//
TPowerIntMeasurementsClientData::~TPowerIntMeasurementsClientData()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::~TPowerIntMeasurementsClientData()" ) );
	
	TPowerIntMeasurement* powerMeasurement;
	iMeasurementsQueIter.SetToFirst();
	while ((powerMeasurement = iMeasurementsQueIter++) != NULL)
	    {
	    iMeasurementsQue.Remove(*powerMeasurement);
	    delete powerMeasurement;
	    }; 
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::~TPowerIntMeasurementsClientData - return" ) );
	}


// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::AddMeasurementL
// Adds an integer measurement to the Measurements List. If list is full it 
// overwrites the oldest value.
// -----------------------------------------------------------------------------
//
void TPowerIntMeasurementsClientData::AddMeasurementL(TDesC8& aMeasurement)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::AddMeasurement()" ) );
    
	TPowerIntMeasurement* powerMeasurement;

    TIntMeasurementsPckg intMeasurementPckg;
    intMeasurementPckg.Copy(aMeasurement);
    TInt measurement = intMeasurementPckg();

	if(IsMeasuringOn())
		{	
		if (iMeasurementsCount < KHWRMPowerMaxMeasurementsBatchSize)
			{
			iMeasurementsCount++;			
			powerMeasurement = new(ELeave) TPowerIntMeasurement();			
			powerMeasurement->iAverageValue = measurement;
			iMeasurementsQue.AddLast(*powerMeasurement);
			}
		else
			{
			powerMeasurement = iMeasurementsQue.First();
			iMeasurementsQue.Remove(*powerMeasurement);
			powerMeasurement->iAverageValue = measurement;
			iMeasurementsQue.AddLast(*powerMeasurement);
			}
		CompleteGetMeasurementsMessage(KErrNone);
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::AddMeasurement - return" ) );
	}

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::CClientsData::WriteMeasurementsInMessageL
// The Measurements collected are written in the message area of the message.
// -----------------------------------------------------------------------------
// 
void TPowerIntMeasurementsClientData::WriteMeasurementsInMessageL()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::WriteMeasurementsInMessageL()" ) );
	
	typedef TFixedArray<TInt,KHWRMPowerMaxMeasurementsBatchSize> TMeasurementsArray;
	TMeasurementsArray measurementsArray;
	
	TransferMeasurementsToFixedArrayL(measurementsArray);

	TPckg<TMeasurementsArray> measurementsArrayPckg(measurementsArray);
    User::LeaveIfError(iRequestedBatchMessage.Write(0,measurementsArrayPckg));
    
    TPckg<TUint> countPckg( iMeasurementsCount );
    User::LeaveIfError(iRequestedBatchMessage.Write(1,countPckg));
    
    COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::WriteMeasurementsInMessageL - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::Reset
// Resets the object in order to be ready for a new measurements session.
// -----------------------------------------------------------------------------
//
void TPowerIntMeasurementsClientData::Reset()
    {
    TPowerMeasurementsClientData::Reset();
    
	iAccumulator = 0;
    
    TPowerIntMeasurement* powerMeasurement;
	iMeasurementsQueIter.SetToFirst();
	while ((powerMeasurement = iMeasurementsQueIter++) != NULL)
	    {
	    iMeasurementsQue.Remove(*powerMeasurement);
	    delete powerMeasurement;
	    }; 
    }

// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::NewBatteryMeasurementL
// Called if the client has registerd for measurements and a new measurement 
// is sent by the plugin.
// -----------------------------------------------------------------------------
//
void TPowerIntMeasurementsClientData::NewBatteryMeasurementL(TDesC8& aValue, TUint aPluginInterval)
	{	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::NewBatteryMeasurementL()" ) );
	
	// Example of the way tuples are used. Assume the client receives the following tuples:
	// (100,2), (101,2), (100, 8) and has requested Interval = 9
	// then it calculatesfor the first 2 measurements:
	// iNumOfBaseIntervalsPassed = 2 + 2 = 4
	// iAccumulator = 100*2 + 101*2 = 402
	//
	// For the final measurement:
	// numIntervalsNeeded = iRequestedIntervalMultiple - iNumOfBaseIntervalsPassed = 9 - 4 = 5
	// numIntervalsNeeded(5) < aPluginInterval(8)
	// iAccumulator += aValue * numIntervalsNeeded = 402 + 100 * 5 = 902
	// iAverage = iAccumulator / iRequestedIntervalMultiple = 902 / 9 = 100.222222

    TPckgBuf<TPluginIntMeasurementFormat>  pluginMeasurementPckg;
    pluginMeasurementPckg.Copy(aValue);
    
    TInt measurement = pluginMeasurementPckg().iMeasurement;
	
	TInt numIntervalsNeeded = iRequestedIntervalMultiple - iNumOfBaseIntervalsPassed;
	
	if (numIntervalsNeeded <= aPluginInterval)
		{
		iAccumulator += measurement * numIntervalsNeeded;
		
		TInt average = iAccumulator / iRequestedIntervalMultiple;
        TIntMeasurementsPckg measurementPckg(average);
		AddMeasurementL(measurementPckg);
		
		iAccumulator = 0;
		iNumOfBaseIntervalsPassed = 0;
		}
	else
		{
		iNumOfBaseIntervalsPassed += aPluginInterval;
		iAccumulator += measurement * aPluginInterval;
		}

	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::NewBatteryMeasurementL - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerIntMeasurementsClientData::GetMeasurementsL
// If at least one measurement is collected the data is written to 
// the messasge and the client is completed. If not the 
// message hanlde is stored and the client is completed
// when data is ready
// -----------------------------------------------------------------------------
// 
TBool TPowerIntMeasurementsClientData::GetMeasurementsL(RMessage2 aMessage)
 	{
 	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::GetMeasurementsL()" ) );
 	
    TBool complete(EFalse);
     
 	if (IsMeasuringOn() == EFalse)
 		{
 		User::Leave(KErrNotFound);
 		}
 	else if(iMeasurementsCount > 0)
 		{
 		// if measurements are ready write to message and let ExecuteMessageL complete it
 		typedef TFixedArray<TInt,KHWRMPowerMaxMeasurementsBatchSize> TMeasurementsArray;
 		
 		TMeasurementsArray measurementsArray;
 		TransferMeasurementsToFixedArrayL(measurementsArray);
		TPckg<TMeasurementsArray> measurementsArrayPckg(measurementsArray);

		User::LeaveIfError(aMessage.Write(0,measurementsArrayPckg));

	    TPckg<TUint> countPckg( iMeasurementsCount );
	    User::LeaveIfError(aMessage.Write(1,countPckg));
	    
 		iMeasurementsCount = 0;
 		
		// Ignore Coverity false positive as uninitialized variable.
		// Since no response is sent back to the client, only a empty message is created here. 
		// Message handle of this will be initialized to NULL in the default constructor.
		// coverity[var_decl]
 		RMessage2 dummy;
 		iRequestedBatchMessage = dummy;
 		
 		complete = ETrue;
 		}
 	else
 		{
 		// if not ready save message handle and complete it when data is ready
 		iRequestedBatchMessage = aMessage;
 		complete = EFalse;
 		} 	
 	
    COMPONENT_TRACE2( _L( "HWRM Server - TPowerIntMeasurementsClientData::GetMeasurementsL - return %d" ), complete );
 	
 	return complete;
 	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::TransferMeasurementsToFixedArrayL
// Trasnfers the measurements from the list to a TFixedArray (used to trasnfer
// the data to the client).
// -----------------------------------------------------------------------------
//
void TPowerIntMeasurementsClientData::TransferMeasurementsToFixedArrayL(TFixedArray<TInt,KHWRMPowerMaxMeasurementsBatchSize> &aFixedArray)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::TransferMeasurementsToFixedArrayL()" ) );
	
	TPowerIntMeasurement* powerMeasurement;
	
	aFixedArray.Reset();
	
	for(TInt i=0;(i<KHWRMPowerMaxMeasurementsBatchSize && iMeasurementsQue.IsEmpty() == EFalse);i++)
		{
		powerMeasurement = iMeasurementsQue.First();
		iMeasurementsQue.Remove(*powerMeasurement);
		aFixedArray[i] = powerMeasurement->iAverageValue;
		delete powerMeasurement;
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerMeasurementsClientData::TransferMeasurementsToFixedArrayL - return" ) );
	}


// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::TPowerVoltageCurrentMeasurementsClientData
// C++ Constructor
// -----------------------------------------------------------------------------
//
TPowerVoltageCurrentMeasurementsClientData::TPowerVoltageCurrentMeasurementsClientData()
	:iMeasurementsQue(_FOFF(TPowerVoltageCurrentMeasurement,iSglQueLink)),
	iMeasurementsQueIter(iMeasurementsQue)
    {
	iVoltageAccumulator = 0;
    iCurrentAccumulator = 0;
    }

// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::~TPowerVoltageCurrentMeasurementsClientData
// Destructor
// -----------------------------------------------------------------------------
//
TPowerVoltageCurrentMeasurementsClientData::~TPowerVoltageCurrentMeasurementsClientData()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::~TPowerVoltageCurrentMeasurementsClientData()" ) );
	
	TPowerVoltageCurrentMeasurement* powerMeasurement;
	iMeasurementsQueIter.SetToFirst();
	while ((powerMeasurement = iMeasurementsQueIter++) != NULL)
	    {
	    iMeasurementsQue.Remove(*powerMeasurement);
	    delete powerMeasurement;
	    }; 
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::~TPowerVoltageCurrentMeasurementsClientData - return" ) );
	}


// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::AddMeasurementL
// Adds an voltage and current measurement to the Measurements List. If list is full it 
// overwrites the oldest value.
// -----------------------------------------------------------------------------
//
void TPowerVoltageCurrentMeasurementsClientData::AddMeasurementL(TDesC8& aMeasurement)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::AddMeasurement()" ) );
    
	TPowerVoltageCurrentMeasurement* powerMeasurement;

    TVoltageCurrentMeasurementsPckg voltageCurrentMeasurementPckg;
    voltageCurrentMeasurementPckg.Copy(aMeasurement);
    TInt voltageMeasurement = voltageCurrentMeasurementPckg().iAverageVoltage;
    TInt currentMeasurement = voltageCurrentMeasurementPckg().iAverageCurrent;

	if(IsMeasuringOn())
		{	
		if (iMeasurementsCount < KHWRMPowerMaxMeasurementsBatchSize)
			{
			iMeasurementsCount++;			
			powerMeasurement = new(ELeave) TPowerVoltageCurrentMeasurement();			
			powerMeasurement->iAverageVoltageValue = voltageMeasurement;
            powerMeasurement->iAverageCurrentValue = currentMeasurement;
			iMeasurementsQue.AddLast(*powerMeasurement);
			}
		else
			{
			powerMeasurement = iMeasurementsQue.First();
			iMeasurementsQue.Remove(*powerMeasurement);
			powerMeasurement->iAverageVoltageValue = voltageMeasurement;
            powerMeasurement->iAverageCurrentValue = currentMeasurement;
			iMeasurementsQue.AddLast(*powerMeasurement);
			}
		CompleteGetMeasurementsMessage(KErrNone);
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::AddMeasurement - return" ) );
	}

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::CClientsData::WriteMeasurementsInMessageL
// The Measurements collected are written in the message area of the message.
// -----------------------------------------------------------------------------
// 
void TPowerVoltageCurrentMeasurementsClientData::WriteMeasurementsInMessageL()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::WriteMeasurementsInMessageL()" ) );
	
	typedef TFixedArray<TPowerVoltageCurrent,KHWRMPowerMaxMeasurementsBatchSize> TMeasurementsArray;
	TMeasurementsArray measurementsArray;
	
	TransferMeasurementsToFixedArrayL(measurementsArray);

	TPckg<TMeasurementsArray> measurementsArrayPckg(measurementsArray);
    User::LeaveIfError(iRequestedBatchMessage.Write(0,measurementsArrayPckg));
    
    TPckg<TUint> countPckg( iMeasurementsCount );
    User::LeaveIfError(iRequestedBatchMessage.Write(1,countPckg));
    
    COMPONENT_TRACE1(_L( "HWRM Server - TPowerIntMeasurementsClientData::WriteMeasurementsInMessageL - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::Reset
// Resets the object in order to be ready for a new measurements session.
// -----------------------------------------------------------------------------
//
void TPowerVoltageCurrentMeasurementsClientData::Reset()
    {
    TPowerMeasurementsClientData::Reset();
    
	iVoltageAccumulator = 0;
    iCurrentAccumulator = 0;
    
    TPowerVoltageCurrentMeasurement* powerMeasurement;
	iMeasurementsQueIter.SetToFirst();
	while ((powerMeasurement = iMeasurementsQueIter++) != NULL)
	    {
	    iMeasurementsQue.Remove(*powerMeasurement);
	    delete powerMeasurement;
	    }; 
    }

// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::NewBatteryMeasurementL
// Called if the client has registerd for measurements and a new measurement 
// is sent by the plugin.
// -----------------------------------------------------------------------------
//
void TPowerVoltageCurrentMeasurementsClientData::NewBatteryMeasurementL(TDesC8& aValue, TUint aPluginInterval)
	{	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::NewBatteryMeasurementL()" ) );
	
	// Example of the way tuples are used. Assume the client receives the following tuples:
	// (100,2), (101,2), (100, 8) and has requested Interval = 9
	// then it calculatesfor the first 2 measurements:
	// iNumOfBaseIntervalsPassed = 2 + 2 = 4
	// iAccumulator = 100*2 + 101*2 = 402
	//
	// For the final measurement:
	// numIntervalsNeeded = iRequestedIntervalMultiple - iNumOfBaseIntervalsPassed = 9 - 4 = 5
	// numIntervalsNeeded(5) < aPluginInterval(8)
	// iAccumulator += aValue * numIntervalsNeeded = 402 + 100 * 5 = 902
	// iAverage = iAccumulator / iRequestedIntervalMultiple = 902 / 9 = 100.222222

    TPckgBuf<TPluginVoltageCurrentMeasurementFormat>  pluginMeasurementPckg;
    pluginMeasurementPckg.Copy(aValue);
    
    TInt voltageMeasurement = pluginMeasurementPckg().iAverageVoltageMeasurement;
    TInt currentMeasurement = pluginMeasurementPckg().iAverageCurrentMeasurement;
	
	TInt numIntervalsNeeded = iRequestedIntervalMultiple - iNumOfBaseIntervalsPassed;
	
	if (numIntervalsNeeded <= aPluginInterval)
		{
		iVoltageAccumulator += voltageMeasurement * numIntervalsNeeded;
		iCurrentAccumulator += currentMeasurement * numIntervalsNeeded;

        TPowerVoltageCurrent averagePowerMeasurement;
		averagePowerMeasurement.iAverageVoltage = iVoltageAccumulator / iRequestedIntervalMultiple;
        averagePowerMeasurement.iAverageCurrent = iCurrentAccumulator / iRequestedIntervalMultiple;
        
        TVoltageCurrentMeasurementsPckg measurementPckg(averagePowerMeasurement);
		AddMeasurementL(measurementPckg);
		
		iVoltageAccumulator = 0;
        iCurrentAccumulator = 0;
		iNumOfBaseIntervalsPassed = 0;
		}
	else
		{
		iNumOfBaseIntervalsPassed += aPluginInterval;
		iVoltageAccumulator += voltageMeasurement * aPluginInterval;
        iCurrentAccumulator += currentMeasurement * aPluginInterval;
		}

	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::NewBatteryMeasurementL - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::GetMeasurementsL
// If at least one measurement is collected the data is written to 
// the messasge and the client is completed. If not the 
// message hanlde is stored and the client is completed
// when data is ready
// -----------------------------------------------------------------------------
// 
TBool TPowerVoltageCurrentMeasurementsClientData::GetMeasurementsL(RMessage2 aMessage)
 	{
 	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::GetMeasurementsL()" ) );
 	
    TBool complete(EFalse);
     
 	if (IsMeasuringOn() == EFalse)
 		{
 		User::Leave(KErrNotFound);
 		}
 	else if(iMeasurementsCount > 0)
 		{
 		// if measurements are ready write to message and let ExecuteMessageL complete it
 		typedef TFixedArray<TPowerVoltageCurrent,KHWRMPowerMaxMeasurementsBatchSize> TMeasurementsArray;
 		
 		TMeasurementsArray measurementsArray;
 		TransferMeasurementsToFixedArrayL(measurementsArray);
		TPckg<TMeasurementsArray> measurementsArrayPckg(measurementsArray);

		User::LeaveIfError(aMessage.Write(0,measurementsArrayPckg));

	    TPckg<TUint> countPckg( iMeasurementsCount );
	    User::LeaveIfError(aMessage.Write(1,countPckg));
	    
 		iMeasurementsCount = 0;
 		
		// Ignore Coverity false positive as uninitialized variable.
		// Since no response is sent back to the client, only a empty message is created here. 
		// Message handle of this will be initialized to NULL in the default constructor.
		// coverity[var_decl]
 		RMessage2 dummy;
 		iRequestedBatchMessage = dummy;
 		
 		complete = ETrue;
 		}
 	else
 		{
 		// if not ready save message handle and complete it when data is ready
 		iRequestedBatchMessage = aMessage;
 		complete = EFalse;
 		} 	
 	
    COMPONENT_TRACE2( _L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::GetMeasurementsL - return %d" ), complete );
 	
 	return complete;
 	}

// -----------------------------------------------------------------------------
// TPowerVoltageCurrentMeasurementsClientData::TransferMeasurementsToFixedArrayL
// Trasnfers the measurements from the list to a TFixedArray (used to trasnfer
// the data to the client).
// -----------------------------------------------------------------------------
//
void TPowerVoltageCurrentMeasurementsClientData::TransferMeasurementsToFixedArrayL(TFixedArray<TPowerVoltageCurrent,KHWRMPowerMaxMeasurementsBatchSize> &aFixedArray)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::TransferMeasurementsToFixedArrayL()" ) );
	
	TPowerVoltageCurrentMeasurement* powerMeasurement;
	
	aFixedArray.Reset();
	
	for(TInt i=0;(i<KHWRMPowerMaxMeasurementsBatchSize && iMeasurementsQue.IsEmpty() == EFalse);i++)
		{
		powerMeasurement = iMeasurementsQue.First();
		iMeasurementsQue.Remove(*powerMeasurement);
		aFixedArray[i].iAverageVoltage = powerMeasurement->iAverageVoltageValue;
        aFixedArray[i].iAverageCurrent = powerMeasurement->iAverageCurrentValue;
		delete powerMeasurement;
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - TPowerVoltageCurrentMeasurementsClientData::TransferMeasurementsToFixedArrayL - return" ) );
	}

/* 
 * For Future Use
 * 
// -----------------------------------------------------------------------------
// CHWRMPowerService::CancelPluginCommandL
// Cancels a plugin command and handles the related transaction(s).
// Note that plug-in handler CancelCommandL's will not cause client requests to complete
// -----------------------------------------------------------------------------
//
void CHWRMPowerService::CancelPluginCommandL(HWRMPowerCommand::TPowerCmd aCommandId)
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::CancelPluginCommandL" ) );

    THWRMPluginRequestData* data = static_cast<THWRMPluginRequestData*>(iTransactionList->GetFirstItem());

    // Cancel each transaction with the cancelled command ID
    while ( data )
        {
        if ( data->iCommandId == aCommandId &&
             data->iRequestMessage.Handle() )
            {
            // Cancel transaction
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMService::CancelPluginCommandL() - Canceling pending message" ) );
            data->iRequestMessage.Complete(KErrCancel);
            
            // Record transaction ID, and move to next item
            TUint8 cancelTransId = data->iTransId;
            data = static_cast<THWRMPluginRequestData*>(data->iNextData);

            // Remove cancelled transaction items
            // First cancel plugin command. If successful, then no call to ProcessResponseL
            iPluginHandler->CancelCommandL( cancelTransId );
            iTransactionList->FindTransaction( cancelTransId, ETrue);
            }
        else
            {
            // Move to next item
            data = static_cast<THWRMPluginRequestData*>(data->iNextData);
            }
        }

    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMPowerService::CancelPluginCommandL - return" ) );     
    }
    */

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
