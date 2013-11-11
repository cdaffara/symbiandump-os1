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
// @file CT_LbsServer.cpp
// This is the class implementation for the LBS  Test Server
// 
//

// User includes
#include "ctlbsserver.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

// Header files of LBS  API Test Steps
#include "ctlbsstepinstallscheduler.h"
#include "ctlbsstepcreateverifymoduledata.h"
#include "ctlbsstepcreateverifyposinfos.h"
#include "ctlbsstepverifyposinfos.h"
#include "ctlbsstepverifyroughposinfos.h"
#include "ctlbsstepverifymodinfo.h"
#include "ctlbsstepverifyloginfo.h"
#include "ctlbsstepverifymodstatus.h"
#include "ctlbsstepsetautonomousmode.h"
#include "ctlbsstepsetprivhandler.h"
#include "ctlbsstepresetdefaultadmin.h"
#include "ctlbsstepstartlbs.h"
#include "ctlbsstepstoplbs.h"
#include "ctlbsstepcreateverifymodinfo.h"
#include "ctlbsstepclearlog.h"
#include "ctlbsstepconfighybridmodule.h"
#include "ctlbsstepsetupstandaloneprivacymode.h"
#include "ctlbsstepsetupstandaloneprivacyandsimplectlr.h"
#include "ctlbsstepsetupstandaloneprivacywithnoctlr.h"
#include "ctlbsstepsetupsppmprivacymode.h"
#include "ctlbsstepsetbehaviourmodeoriginal.h"
#include "ctlbsstepsetptamode.h"
#include "ctlbsstepsetatamode.h"
#include "ctlbsstepsetptbmode.h"
#include "ctlbsstepsetuppsy.h"
#include "ctlbsstepconfignpppsy.h"
#include "ctlbsstepconfigsuplpsy.h"
CT_LbsServer::~CT_LbsServer()
	{
	delete iSharedData;
	delete iScheduler;
	}
	
	
/**
  Function : CT_LbsServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsServer::CT_LbsServer()
	{
	}

/**
  Function : ConstructL
  Description : 
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_LbsServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);

	iScheduler = new (ELeave) CActiveScheduler();

	// check what this does, do we need it...	
	Logger().ShareAuto();	// enable the log system for all test threads via iParent.Logger()
	}

	
/**
  Function : CreateTestStep
  Description : Creates a test step based on the step name read from the script file
  @internalTechnology
  @param : aStepName   The step name which needs to be created
  @return : Created object of type CTestStep
  @precondition : none
  @postcondition : none
*/
CTestStep* CT_LbsServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Common test steps, used by many different test cases.
	if (aStepName == KLbsStep_InstallScheduler)
		{
		return CT_LbsStep_InstallScheduler::New(*this);
		}

	if (aStepName == KLbsStep_CreateVerifyModuleData)
		{
		return CT_LbsStep_CreateVerifyModuleData::New(*this);
		}

	if (aStepName == KLbsStep_CreateVerifyPosInfos)
		{
		return CT_LbsStep_CreateVerifyPosInfos::New(*this);
		}

	if (aStepName == KLbsStep_CreateVerifyModInfo)
		{
		return CT_LbsStep_CreateVerifyModInfo::New(*this);
		}
	
	if (aStepName == KLbsStep_VerifyPosInfos)
		{
		return CT_LbsStep_VerifyPosInfos::New(*this);
		}

	if (aStepName == KLbsStep_VerifyRoughPosInfos)
		{
		return CT_LbsStep_VerifyRoughPosInfos::New(*this);
		}

	if(aStepName == KLbsStep_VerifyModInfo)	
		{
		return CT_LbsStep_VerifyModInfo::New(*this);
		}

	if (aStepName == KLbsStep_VerifyLogInfo)
		{
		return CT_LbsStep_VerifyLogInfo::New(*this);
		}
		
	if (aStepName == KLbsStep_VerifyModuleStatus)
		{
		return CT_LbsStep_VerifyModStatus::New(*this);
		}										
  		
  	if (aStepName == KLbsStep_StartLbs)
  		{
  		return CT_LbsStep_StartLbs::New(*this);
  		}
  		
  	if (aStepName == KLbsStep_StopLbs)
  		{
  		return CT_LbsStep_StopLbs::New(*this);
  		}						
		
	if (aStepName == KLbsStep_SetAutonomousMode)
		{
		return CT_LbsStep_SetAutonomousMode::New(*this);
		}
		
	if (aStepName == KLbsStep_SetPrivacyHandler)
		{
		return CT_LbsStep_SetPrivacyHandler::New(*this);
		}		
		
	if (aStepName == KLbsStep_ResetDefaultAdmin)
		{
		return CT_LbsStep_ResetDefaultAdmin::New(*this);
		}		
		
	if (aStepName == KLbsStep_ClearLog)
		{
		return CT_LbsStep_ClearLog::New(*this);
		}
	
	if (aStepName == KLbsStep_SetBehaviourModeOriginal)
		{
		return CT_LbsStep_SetBehaviourModeOriginal::New(*this);
		}		
    
	if (aStepName == KLbsStep_SetPTAMode)
		{
		return CT_LbsStep_SetPTAMode::New(*this);
		}		
    
    if (aStepName == KLbsStep_ConfigHybridModule)
    	{
    	return CT_LbsStep_ConfigHybridModule::New(*this);
    	}
      
    if (aStepName == KLbsStep_SetupStandalonePrivacyMode)
    	{
    	return CT_LbsStep_SetupStandalonePrivacyMode::New(*this);
    	}
      
    if (aStepName == KLbsStep_SetupStandalonePrivacyAndSimpleCtlr)
    	{
    	return CT_LbsStep_SetupStandalonePrivacyAndSimpleCtlr::New(*this);
    	}
      
    if (aStepName == KLbsStep_SetupStandalonePrivacyWithNoCtlr)
    	{
    	return CT_LbsStep_SetupStandalonePrivacyWithNoCtlr::New(*this);
    	}
    
    if (aStepName == KLbsStep_SetupSPPMPrivacyMode)
    	{
    	return CT_LbsStep_SetupSPPMPrivacyMode::New(*this);
    	}
    
   	if (aStepName == KLbsStep_SetATAMode)
		{
		return CT_LbsStep_SetATAMode::New(*this);
		}	
   	
   	if (aStepName == KLbsStep_SetPTBMode)
		{
		return CT_LbsStep_SetPTBMode::New(*this);
		}   	
      
   	if (aStepName == KLbsStep_SetUpPsy)
		{
		return CT_LbsStep_SetUpPsy::New(*this);
		} 

    if (aStepName == KLbsStep_ConfigNppPsy)
        {
        return CT_LbsStep_ConfigNppPsy::New(*this);
        }

    if (aStepName == KLbsStep_ConfigSuplPsy)
        {
        return CT_LbsStep_ConfigSuplPsy::New(*this);
        }
    return NULL;
	}
