/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_suplproxypmsuitestepbase.h
*/

#ifndef TESUPLPROXYPMSUITESTEPBASE_H
#define TESUPLPROXYPMSUITESTEPBASE_H

#include <test/testexecutestepbase.h>

#include <lbs/lbsnetprotocolbase.h>     // For MLbsNetworkProtocolObserver
#include <lbs/lbsnetcommon.h>           // For TLbsNetPosCapabilities
#include <lbs/lbsassistancedatabase.h>  // For TLbsAsistanceDataGroup
#include <lbs/lbsadmin.h>               // For settings req to load the SuplProxyPM

#include <lbs/epos_comasuplposhandlerbase.h>    // For the POS Msg plugin
#include <lbs/epos_comasuplpossessionbase.h>    // For the POS Msg plugin
#include <lbs/epos_momasuplobserver.h>          // For MOMASuplObserver

#include <ecom/ecom.h>

#include "lbsprocesssupervisor.h"           // For RLbsProcess supervisor


// Test Suite UID for publishing properties.
const TInt KSuplProxyPMSuiteUid     = 0x102871F5;
const TUid KSuplProxyPMSuite        = {KSuplProxyPMSuiteUid};

//Key values for test control properties
const TUint KLbsPosSessionInitStepKey        = 0x10000000;

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_SuplProxyPMSuiteStepBase : public CTestStep, public MOMASuplObserver
	{
public:
	virtual ~CTe_SuplProxyPMSuiteStepBase();
	CTe_SuplProxyPMSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

	// [TestCode acting as the SUPL FW] From MOMASuplObserver -------
	// Callback from the POS message plugin through MOMASuplObserver interface
    // Reports Version of SUPL Protocol supported by SUPL FW
	TReal SuplVersion();
	//Terminates the SUPL Session in the SUPL FW. POS Message
	//plug-in invokes this method to terminate the SUPL Session
	void TerminateSession();
	// ---------------------------------------------------------------
	void SetTestCasePropertyL(const TUid TestServerUid, const TUint KeyType, TInt aIndex);
protected:
    //void LoadSuplProxyProtocolModuleL();
    void LoadSuplPosMessagePluginL();
    
    TInt StartNetGateway();
    void InitialiseInterfacesL();    
    void ShutDownInterfacesL();     
    TInt StopNetGateway();
    
    TInt SetupLbsAdminSettingsL();
    void SetProtocolModuleSettingsL();
    
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
       
    // Active scheduler
	CActiveScheduler* iSched;
	
	// For calls to the POS message plugin  [test code acts as the MW SUPL FW]
	COMASuplPosHandlerBase* iSuplPosHandler;
	
    RLbsProcessSupervisor iProcessSupervisor;
	};	

#endif
