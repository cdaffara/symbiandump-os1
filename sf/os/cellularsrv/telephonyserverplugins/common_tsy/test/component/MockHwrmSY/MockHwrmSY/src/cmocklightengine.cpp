// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MockSY engine for the Light plugin
// 
//

#include <e32property.h> 
#include "cmocklightengine.h"
#include "cmockhwrmpluginsy.h"
#include <hwrm/HWRMLightCommands.h>


_LIT(KLightsOnCmdId,"ELightsOnCmdId");
_LIT(KLightsOnSensorCmdId,"ELightsOnSensorCmdId");
_LIT(KLightsBlinkCmdId,"ELightsBlinkCmdId");
_LIT(KLightsOffCmdId,"ELightsOffCmdId");
_LIT(KSetLightsIntensityCmdId,"ESetLightsIntensityCmdId");
_LIT(KSetLightsSensorSensitivityCmdId,"ESetLightsSensorSensitivityCmdId");
_LIT(KLightsSetColorCmdId,"ELightsSetColorCmdId");


_LIT_SECURITY_POLICY_PASS(KAlwaysPassPolicy);

const TInt KTestCategory = 0x101FB3E7;
const TUint32 KHWRMColorStatus  = 0x00001000;
const TUint8 KHWRMRedShift             = 16;
const TUint8 KHWRMGreenShift           = 8;

/**
Factory function
*/
CMockLightEngine* CMockLightEngine::NewL(CMockHwrmPluginSY& aMockHwrmPluginSY)
	{
	CMockLightEngine* self =  new (ELeave)CMockLightEngine(aMockHwrmPluginSY);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor
*/	
CMockLightEngine::CMockLightEngine(CMockHwrmPluginSY& aMockHwrmPluginSY)
	:CMockHwrmPluginEngine(aMockHwrmPluginSY)
	{
	}


/**
*/
void CMockLightEngine::LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& cmdName = CommandName(aCmdId);
	buffer.Format(_L(">>> Cmd=%S, TransId=%d, Err=%d"),&cmdName, data.TransId(), aResultCode);
	Log(buffer);	
	}

/**
*/
void CMockLightEngine::LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode)
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
void CMockLightEngine::LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,
	TInt aExpectedCmd,const TDesC8& aExpectedData)
	{
	const TMockHwrmPluginData& data = static_cast<const TMockHwrmPluginData&>(aData);
	TMockHwrmPluginData expectedData;
	expectedData.DeserialiseL(aExpectedData);

	DoLogError(aCmdId, data, aExpectedCmd, expectedData);
	DoLogCommandData(aExpectedCmd, expectedData, _L("Expected"));
	DoLogCommandData(aCmdId, data, _L("Received"));	
	}

void CMockLightEngine::QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& aData)
	{
	CMockHwrmPluginEngine::QueueProcessedCompletionL(aCommandId, aTransId, aBuffer,aData);
	}

void CMockLightEngine::FurtherProcessEventL(const TInt aCommandId, TDesC8& aData)
	{

    TInt err = RProperty::Define( TUid::Uid(KTestCategory), KHWRMColorStatus, RProperty::EInt, KAlwaysPassPolicy, KAlwaysPassPolicy );

    if(err !=KErrNone && err!= KErrAlreadyExists)
        {
        User::Leave(err);
        }
    switch(aCommandId)
        {
        case HWRMLightCommand::ELightsSetColorCmdId:
            {
            HWRMLightCommand::TLightsColorCmdPackage colorPckg;
            colorPckg.Copy(aData);
            HWRMLightCommand::TLightsColorData color = colorPckg();
            
            TUint colorval;
            
            colorval  = (color.iRed << KHWRMRedShift) | (color.iGreen << KHWRMGreenShift) | (color.iBlue);
            RProperty::Set(  TUid::Uid(KTestCategory) , KHWRMColorStatus, colorval );
            break;
            }
        default:
            {
            break;
            }
        }
	}

/**
Log an error
*/
void CMockLightEngine::DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,
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
void CMockLightEngine::DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment)
	{
	TBuf<KMaxLogLineSize> buffer;
	switch (aCmdId)
		{
		case HWRMLightCommand::ELightsOnCmdId:
			{
			HWRMLightCommand::TLightsOnCmdPackage lightOnData;
			lightOnData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: iTarget=%d, iIntensity=%d, iFadeIn=%d"), & aComment,
				lightOnData().iTarget, lightOnData().iIntensity, lightOnData().iFadeIn);
			}
		break;

		case HWRMLightCommand::ELightsOnSensorCmdId:
			{
			HWRMLightCommand::TLightsOnSensorCmdPackage lightOnSensorData;
			lightOnSensorData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: iTarget=%d, iSensitivity=%d, iFadeIn=%d"), & aComment,
				lightOnSensorData().iTarget, lightOnSensorData().iSensitivity, lightOnSensorData().iFadeIn);
			}
		break;

		case HWRMLightCommand::ELightsBlinkCmdId:
			{
			HWRMLightCommand::TLightsBlinkCmdPackage lightBlinkData;
			lightBlinkData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: iTarget=%d, iIntensity=%d, iOnCycleDuration=%d, iOffCycleDuration=%s"), & aComment,
				lightBlinkData().iTarget, lightBlinkData().iIntensity, lightBlinkData().iOnCycleDuration, lightBlinkData().iOffCycleDuration);			
			}
		break;

		case HWRMLightCommand::ELightsOffCmdId:
			{
			HWRMLightCommand::TLightsOffCmdPackage lightOffData;
			lightOffData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: iTarget=%d, iFadeOut=%d"), & aComment,
				lightOffData().iTarget, lightOffData().iFadeOut);
			}
		break;

		case HWRMLightCommand::ESetLightsIntensityCmdId:
			{
			HWRMLightCommand::TLightsIntensityCmdPackage lightIntensityData;
			lightIntensityData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: iTarget=%d, iIntensity=%d"), & aComment,
				lightIntensityData().iTarget, lightIntensityData().iIntensity);			
			}
		break;

		case HWRMLightCommand::ESetLightsSensorSensitivityCmdId:
			{
			HWRMLightCommand::TLightsIntensityCmdPackage lightSensorSensitivityData;
			lightSensorSensitivityData.Copy(aCmdData.CommandData());
			buffer.Format(_L("%S: SensitivityData=%d"), & aComment,
				lightSensorSensitivityData());			
			}
		break;
        
        case HWRMLightCommand::ELightsSetColorCmdId:
            {
            HWRMLightCommand::TLightsColorCmdPackage lightColorData;
			lightColorData.Copy(aCmdData.CommandData());
            HWRMLightCommand::TLightsColorData color = lightColorData();
			buffer.Format(_L("%S: iTarget=%d, Red=%d, Green=%d Blue=%d DefaultColor=%d"), & aComment,
				color.iTarget, color.iRed, color.iBlue, color.iDefaultColor);
            }
        break;
		}		
	Log(buffer);
	}

const TDesC& CMockLightEngine::CommandName(TInt aCmdId)
	{
	switch (aCmdId)
		{
		case HWRMLightCommand::ELightsOnCmdId:			
			return KLightsOnCmdId;
		case HWRMLightCommand::ELightsOnSensorCmdId:
			return KLightsOnSensorCmdId;
		case HWRMLightCommand::ELightsBlinkCmdId:
			return KLightsBlinkCmdId;
		case HWRMLightCommand::ELightsOffCmdId:
			return KLightsOffCmdId;
		case HWRMLightCommand::ESetLightsIntensityCmdId:
			return KSetLightsIntensityCmdId;
		case HWRMLightCommand::ESetLightsSensorSensitivityCmdId:
			return KSetLightsSensorSensitivityCmdId;
        case HWRMLightCommand::ELightsSetColorCmdId:
            return KLightsSetColorCmdId;
		}		
		return KNullDesC;
	}
