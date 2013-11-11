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
// MockSY engine for the Power plugin
// 
//

#include "cmockpowerengine.h"
#include "cmockhwrmpluginsy.h"
#include <hwrm/HWRMPowerCommands.h>

#include "e32property.h" 

_LIT(KPowerGetBatteryInfoCmdId,"EGetBatteryInfoCmdId");
_LIT(KPowerGetBatteryVoltageInfoCmdId,"EGetBatteryVoltageInfoCmdId");
_LIT(KPowerStartAverageBatteryPowerReportingCmdId,"EStartAverageBatteryPowerReportingCmdId");
_LIT(KPowerStopAverageBatteryPowerReportingCmdId,"EStopAverageBatteryPowerReportingCmdId");
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
_LIT(KPowerStartAverageBatteryChargeRateReportingCmdId,"EStartAverageBatteryChargeRateReportingCmdId");
_LIT(KPowerStartBatteryFullChargingTimeReportingCmdId,"EStartBatteryFullChargingTimeReportingCmdId");
_LIT(KPowerStopAverageBatteryChargingCurrentReportingCmdId,"EStopAverageBatteryChargingCurrentReportingCmdId");
_LIT(KPowerStopBatteryFullChargingTimeReportingCmdId,"EStopBatteryFullChargingTimeReportingCmdId");
_LIT(KPowerGetBatteryChargerTypeCmdId,"EGetBatteryChargerTypeCmdId");
#endif //SYMBIAN_HWRM_EXTPOWERINFO

/**
* Factory function
*/
CMockPowerEngine* CMockPowerEngine::NewL(CMockPowerSY& aMockHwrmPluginSY)
	{
	CMockPowerEngine* self =  new (ELeave)CMockPowerEngine(aMockHwrmPluginSY);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
* C++ Constructor
*/	
CMockPowerEngine::CMockPowerEngine(CMockPowerSY& aMockHwrmPluginSY)
	:iMockPowerSY(aMockHwrmPluginSY)
	{
    iPowerVoltageNotificationCounter = 0;
    iPowerCurrentNotificationCounter = 0;
    iPowerNotificationPeriod = 0;
    iPowerReporting = EFalse;
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    iChargingCurrentCounter = 0;
    iChargeTimeCounter = 0;
    iChargingCurrentNotificationPeriod = 0;
    iChargeTimeNotificationPeriod = 0;
    iChargingCurrentReporting = EFalse;
    iChargingTimeReporting = EFalse;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	}

/**
* Queue an event
*/
void CMockPowerEngine::DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
		TInt aResultCode, TBool aLeave, TInt aDelay)
	{
	iStarted = ETrue; // got a first event queued by the client
	CMockSYEngine::DoQueueEventL(aType,aCmdId, aData,aResultCode,aLeave,aDelay);
	}

/**
* Handler for the SY: handle ProcessCommand. Return KErrCorrupt if it's not the expected command
*/
TInt CMockPowerEngine::ExecuteProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
	{
	if (!iStarted)
		{
		QueueProcessedEventL(aCommandId,aTransId,aData);
		}

	iLatestTransId = aTransId;
	TMockHwrmPluginData commandData(aTransId,aData);
	TInt err = ExecuteCommandL(aCommandId, commandData);
	return err;
	}

/**
* Handler for the SY: handle CancelCommand. Return KErrCorrupt if it's not the expected command
*/
TInt CMockPowerEngine::ExecuteCancelCommandL(const TUint8 aTransId, const TInt aCommandId)
	{
	iLatestTransId = aTransId;
	TMockHwrmPluginData commandData(aTransId);
	TInt err = ExecuteCommandL(-aCommandId, commandData);
	return err;
	}

/**
* Generate a completion event
*/
void CMockPowerEngine::DoCompleteEventFurther(const TMockSYEvent& aEvent)
	{
	TMockHwrmPluginData completeData;
	completeData.DeserialiseL(aEvent.iData);
	if (completeData.HasTransId())
		{
		// client provide a trans id, use it
 		iMockPowerSY.DoProcessResponse(aEvent.iCmdId, completeData.TransId(), completeData.CommandData());
		}
	else
		{
		// client didn't provide a trans id, the trans id of the latest command
 		iMockPowerSY.DoProcessResponse(aEvent.iCmdId, iLatestTransId, completeData.CommandData());
		}
	}

/**
* Generate a completion event
*/
void CMockPowerEngine::DoCompleteEventL(const TMockSYEvent& aEvent)
	{	
    if (iPowerReporting && 
		 (aEvent.iCmdId == HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId ||
 		  aEvent.iCmdId == HWRMPowerCommand::EAverageBatteryPowerIndicationId))
		{
		// while the measuring is On completion events are generated and queued for completion. 
		RBuf8 buffer;
	
		HWRMPowerCommand::TBatteryPowerReport powerReport;
		powerReport.iErrorCode = KErrNone;
		powerReport.iAverageVoltage = iPowerVoltageNotificationCounter++; // increase counter number 
		powerReport.iAverageCurrent = iPowerCurrentNotificationCounter++; // increase counter number 

		//retrieve the TransId. Keep using the same TransID to inform the Original GetMeasurements command
		TMockHwrmPluginData completeData;
		completeData.DeserialiseL(aEvent.iData);

		HWRMPowerCommand::TBatteryPowerReportPackage powerReportPckg(powerReport);		
		TMockHwrmPluginData respVoltageReport(completeData.TransId(), powerReportPckg);
		respVoltageReport.SerialiseL(buffer);
		
		// copy serialised data to a HBufC8 pointer
		HBufC8* resp = HBufC8::New(buffer.Length());
		TPtr8 respPtr = resp->Des();
		respPtr = buffer;
	
		// add event to the pending queue	
		CMockSYEngine::DoQueueEventL(TMockSYEvent::ECompletion, 
									 HWRMPowerCommand::EAverageBatteryPowerIndicationId,
									 resp,
									 KErrNone,
									 EFalse,
									 iPowerNotificationPeriod );
		buffer.Close();
		}
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    else if (iChargingCurrentReporting && 
		 (aEvent.iCmdId == HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId ||
 		  aEvent.iCmdId == HWRMPowerCommand::EAverageBatteryChargingCurrentIndicationId))
		{
		// while the measuring is On completion events are generated and queued for completion. 
		RBuf8 buffer;
	
		HWRMPowerCommand::TBatteryChargeCurrentReport chargingCurrentReport;
		chargingCurrentReport.iErrorCode = KErrNone;
		chargingCurrentReport.iChargingCurrent = iChargingCurrentCounter++; // increase counter number 

		//retrieve the TransId. Keep using the same TransID to inform the Original GetMeasurements command
		TMockHwrmPluginData completeData;
		completeData.DeserialiseL(aEvent.iData);

		HWRMPowerCommand::TBatteryChargingCurrentReportPackage chargingCurrentReportPckg(chargingCurrentReport);		
		TMockHwrmPluginData respChargingCurrentReport(completeData.TransId(), chargingCurrentReportPckg);
		respChargingCurrentReport.SerialiseL(buffer);
		
		// copy serialised data to a HBufC8 pointer
		HBufC8* resp = HBufC8::New(buffer.Length());
		TPtr8 respPtr = resp->Des();
		respPtr = buffer;
	
		// add event to the pending queue	
		CMockSYEngine::DoQueueEventL(TMockSYEvent::ECompletion, 
									 HWRMPowerCommand::EAverageBatteryChargingCurrentIndicationId,
									 resp,
									 KErrNone,
									 EFalse,
									 iChargingCurrentNotificationPeriod );
		buffer.Close();
		}
    else if (iChargingTimeReporting && 
		 (aEvent.iCmdId == HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId ||
 		  aEvent.iCmdId == HWRMPowerCommand::EBatteryFullChargingTimeIndicationId))
		{
		// while the measuring is On completion events are generated and queued for completion. 
		RBuf8 buffer;
	
		HWRMPowerCommand::TBatteryFullChargingTimeReport chargeTimeReport;
		chargeTimeReport.iErrorCode = KErrNone;
		chargeTimeReport.iChargingTime = iChargeTimeCounter++; // increase counter number 

		//retrieve the TransId. Keep using the same TransID to inform the Original GetMeasurements command
		TMockHwrmPluginData completeData;
		completeData.DeserialiseL(aEvent.iData);

		HWRMPowerCommand::TBatteryFullChargingTimeReportPackage chargeTimeReportPckg(chargeTimeReport);		
		TMockHwrmPluginData respChargeTimeReport(completeData.TransId(), chargeTimeReportPckg);
		respChargeTimeReport.SerialiseL(buffer);
		
		// copy serialised data to a HBufC8 pointer
		HBufC8* resp = HBufC8::New(buffer.Length());
		TPtr8 respPtr = resp->Des();
		respPtr = buffer;
	
		// add event to the pending queue	
		CMockSYEngine::DoQueueEventL(TMockSYEvent::ECompletion, 
									 HWRMPowerCommand::EBatteryFullChargingTimeIndicationId,
									 resp,
									 KErrNone,
									 EFalse,
									 iChargeTimeNotificationPeriod );
		buffer.Close();
		}
#endif //SYMBIAN_HWRM_EXTPOWERINFO

	DoCompleteEventFurther(aEvent);
	}

/**
*/
void CMockPowerEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L(">>> Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(), aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockPowerEngine::LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode)
	{
	TMockHwrmPluginData data;
	data.DeserialiseL(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L("<<< Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(),aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockPowerEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TMockHwrmPluginData expectedData;
	expectedData.DeserialiseL(aExpectedData);

	DoLogError(aCmdId, data, aExpectedCmd, expectedData);
	DoLogCommandData(aExpectedCmd, expectedData, _L("Expected"));
	DoLogCommandData(aCmdId, data, _L("Received"));	
	}

void CMockPowerEngine::QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& /*aData*/)
	{	
	switch ( aCommandId )
		{
		case HWRMPowerCommand::EGetBatteryInfoCmdId:
			{
			// create the data and the package to respond to the server. 
			HWRMPowerCommand::TBatteryConsumptionResponseData consumptionData;
			consumptionData.iErrorCode = KErrNone;
			consumptionData.iNominalCapacity = 200000;
			consumptionData.iRemainingCapacity = 120000;
			consumptionData.iRemainingPercentageCapacity = 60;
	
			HWRMPowerCommand::TBatteryConsumptionResponsePackage consumptionDataPckg(consumptionData);
			TMockHwrmPluginData respConsumptionData(aTransId, consumptionDataPckg);
			respConsumptionData.SerialiseL(aBuffer);
			
			break;
			}
		case HWRMPowerCommand::EGetBatteryVoltageInfoCmdId:
			{
			// create the data and the package to respond to the server. 
			HWRMPowerCommand::TBatteryVoltageResponseData voltageData;
			voltageData.iErrorCode = KErrNone;
			voltageData.iMaximumVoltage = 1000;
			voltageData.iMinimumVoltage = 1;
			voltageData.iRemainingVoltage = 69;
	
			HWRMPowerCommand::TBatteryVoltageResponsePackage voltageDataPckg(voltageData);
			TMockHwrmPluginData respVoltageData(aTransId, voltageDataPckg);
			respVoltageData.SerialiseL(aBuffer);
			
			break;
			}

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        case HWRMPowerCommand::EGetBatteryChargerTypeCmdId:
            {
            HWRMPowerCommand::TBatteryChargerTypeData chargerData;
            chargerData.iErrorCode   = KErrNone;
            chargerData.iChargerType = HWRMPowerCommand::EBatteryChargerUsbDedicated;  

            HWRMPowerCommand::TBatteryChargerTypeResponsePackage chargeTypeDataPckg(chargerData);
			TMockHwrmPluginData respChargeTypeData(aTransId, chargeTypeDataPckg);
			respChargeTypeData.SerialiseL(aBuffer);
            break;
            }
#endif //SYMBIAN_HWRM_EXTPOWERINFO

		default:
			{
			//default response is an error-code message
			HWRMPowerCommand::TErrorCodeResponsePackage resultPckg(KErrNone);
			TMockHwrmPluginData respData(aTransId,resultPckg);
			respData.SerialiseL(aBuffer);
			
			break;
			}
		}
	}

void CMockPowerEngine::FurtherProcessEventL(const TInt aCommandId, TDesC8& aData)
	{
	switch (aCommandId)
		{

        case HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId:
			{
			// starting the measurements. Set flag ETrue and the counter to 0.
			iPowerReporting = ETrue;
			iPowerVoltageNotificationCounter = 0;
            iPowerCurrentNotificationCounter = 0;
			
			// Setting the interval multiple to the requested valuue
			TPckgBuf<TUint> requestedIntervalMultiplePckg;
			requestedIntervalMultiplePckg.Copy(aData);			    
			iPowerNotificationPeriod = requestedIntervalMultiplePckg();
			}
		break;

        case HWRMPowerCommand::ESetPowerReportingIntervalMultipleCmdId:
			{
			// Setting the interval multiple to the requested valuue
			TPckgBuf<TUint> requestedIntervalMultiplePckg;
			requestedIntervalMultiplePckg.Copy(aData);
			iPowerNotificationPeriod = requestedIntervalMultiplePckg();
			}
		break;
    	case HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId:
    		{
    		iPowerReporting = EFalse;
    		}
		break;
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        case HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId:
			{
			// starting the measurements. Set flag ETrue and the counter to 0.
			iChargingCurrentReporting = ETrue;
			iChargingCurrentCounter = 0;
			
			// Setting the interval multiple to the requested valuue
			HWRMPowerCommand::TChrgCurrentIntervalMultiplePackage requestedIntervalMultiplePckg;
			requestedIntervalMultiplePckg.Copy(aData);			    
			iChargingCurrentNotificationPeriod = requestedIntervalMultiplePckg();
			}
		break;

        case HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId:
			{
			// starting the measurements. Set flag ETrue and the counter to 0.
			iChargingTimeReporting = ETrue;
			iChargeTimeCounter = 0;
			
			//Notification time is decided by plugin
			iChargeTimeNotificationPeriod = 1;
			}
		break;
		
        case HWRMPowerCommand::ESetChargeCurrentIntervalMultipleCmdId:
			{
			// Setting the interval multiple to the requested valuue
			HWRMPowerCommand::TChrgCurrentIntervalMultiplePackage requestedIntervalMultiplePckg;
			requestedIntervalMultiplePckg.Copy(aData);
			iChargingCurrentNotificationPeriod = requestedIntervalMultiplePckg();
			}
		break;
		
		case HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId:
			{
			iChargingCurrentReporting = EFalse;
			}
		break;

        case HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId:
			{
			iChargingTimeReporting = EFalse;
			}
		break;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        
		default:
		break;		
		}
	}

/**
Log an error
*/
void CMockPowerEngine::DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,
	TInt aExpectedCmd,const TMockHwrmPluginData& aExpectedData)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& expectedCmdName = CommandName(aExpectedCmd);
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L("ERROR: Expected %S, TransId=%d; Received %S,TransId=%d, "),
		&expectedCmdName, aExpectedData.TransId(), &cmdName, aData.TransId());
	Log(buffer);
	}

/**
Log command data
*/
void CMockPowerEngine::DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment)
	{
	TBuf<KMaxLogLineSize> buffer;

    if (aCmdId == HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId)
        {
        TPckgBuf<TUint> intervalMultipleData;
		intervalMultipleData.Copy(aCmdData.CommandData());
		buffer.Format(_L("%S: Power intervalMultipleData=%d"), & aComment,
				intervalMultipleData());
        }
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    else if (aCmdId == HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId)
		{
		HWRMPowerCommand::TChrgCurrentIntervalMultiplePackage intervalMultipleData;
		intervalMultipleData.Copy(aCmdData.CommandData());
		buffer.Format(_L("%S: intervalMultipleData=%d"), & aComment,
				intervalMultipleData());			
		}		
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	Log(buffer);
	}

const TDesC& CMockPowerEngine::CommandName(TInt aCmdId)
	{
	
	switch (aCmdId)
		{
		case HWRMPowerCommand::EGetBatteryInfoCmdId:			
			return KPowerGetBatteryInfoCmdId;
		case HWRMPowerCommand::EGetBatteryVoltageInfoCmdId:
			return KPowerGetBatteryVoltageInfoCmdId;
        case HWRMPowerCommand::EStartAverageBatteryPowerReportingCmdId:
            return KPowerStartAverageBatteryPowerReportingCmdId;            
        case HWRMPowerCommand::EStopAverageBatteryPowerReportingCmdId:
            return KPowerStopAverageBatteryPowerReportingCmdId;            
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        case HWRMPowerCommand::EStartAverageBatteryChargingCurrentReportingCmdId:
            return KPowerStartAverageBatteryChargeRateReportingCmdId;
        case HWRMPowerCommand::EStartBatteryFullChargingTimeReportingCmdId:
            return KPowerStartBatteryFullChargingTimeReportingCmdId;            
        case HWRMPowerCommand::EStopAverageBatteryChargingCurrentReportingCmdId:
            return KPowerStopAverageBatteryChargingCurrentReportingCmdId;
        case HWRMPowerCommand::EStopBatteryFullChargingTimeReportingCmdId:
            return KPowerStopBatteryFullChargingTimeReportingCmdId;
        case HWRMPowerCommand::EGetBatteryChargerTypeCmdId:
            return KPowerGetBatteryChargerTypeCmdId;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
		}	
		
		return KNullDesC;
	}

/**
*/
void CMockPowerEngine::Reset()
	{
	CMockSYEngine::Reset();
	iStarted = EFalse;
	}

/**
*/
void CMockPowerEngine::QueueProcessedEventL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData)
	{
	// Request
	RBuf8 buffer;
	
	// Request
	
	FurtherProcessEventL(aCommandId, aData);

	TMockHwrmPluginData reqData(aTransId,aData);
	reqData.SerialiseL(buffer);
	// copy serialised data to a HBufC8 pointer
	HBufC8* req = HBufC8::New(buffer.Length());
	TPtr8 reqPtr = req->Des();
	reqPtr = buffer;
	// queue that as expected request (take ownership of req)
	CMockSYEngine::DoQueueEventL(TMockSYEvent::EMessage,aCommandId,req,KErrNone,EFalse,0);
	buffer.Close();
	
	QueueProcessedCompletionL(aCommandId, aTransId, buffer, aData);
	// copy serialised data to a HBufC8 pointer
	HBufC8* resp = HBufC8::New(buffer.Length());
	TPtr8 respPtr = resp->Des();
	respPtr = buffer;
	// queue that as response (take ownership of req)
	CMockSYEngine::DoQueueEventL(TMockSYEvent::ECompletion,aCommandId,resp,KErrNone,EFalse,1);
	buffer.Close();
	}

