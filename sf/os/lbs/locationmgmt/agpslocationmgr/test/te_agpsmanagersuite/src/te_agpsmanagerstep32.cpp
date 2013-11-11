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

/**
 @file te_agpsmanagerstep32.cpp
*/
 
#include "te_agpsmanagerstep32.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_AgpsManagerStep32::CTe_AgpsManagerStep32()
	{
	SetTestStepName(KAgpsManagerStep32);
	}	

//
// Valid Positioning Method from the NRH in TerminalBased H/W, followed by a LS request.
// The LS request TT is in the future when the RequestAssistanceData is called by the GPS module.
// We check that the assistance data is requested and no self-locate session is started with the NG.
// Then the location update satisfies the NRH request. The location request is reissued to the GPS module
// and when the RequestAssistanceData is called by the GPS module, self-location session must be started.
// Then the location update is generated and the self-location request finishes normally.
//
void CTe_AgpsManagerStep32::doTestStepImplL()
	{	
	//Inject a location request from NRH for Terminal Based		
	iNrhToManagerRequestInjector->RequestPositionUpdate(iRequestedQuality,iRequestedPosMethods,
			iRequestedTargetTimeNRH);
	
	// Verify that the Manager called SetGpsOptions to set PTB followed by a location
	// request
	iGpsModuleObserver->WaitForCallBack();
	TESTL(iGpsModuleObserver->WereGpsOptionsSet(iResultedGpsOptions));
	TESTL(ELbsGpsOptionsClass == iResultedGpsOptions.ClassType());
	TESTL(CLbsAdmin::EGpsPreferTerminalBased == iResultedGpsOptions.GpsMode());
				
	TESTL(iGpsModuleObserver->WasLocationUpdateRequested(iResultedTargetTime, iResultedQuality));
			
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iGpsModuleObserver->IsMoreActivityObserved());
		
	
	//Inject a location request from LS for Terminal Based with a TT in the future, but still overlapping
	TTime requestedTargetTimeLS;
	requestedTargetTimeLS = iRequestedTargetTimeNRH + KOverlapTargetTime;
	
	iLsToManagerRequestInjector->RequestPositionUpdate(iRequestedQuality,iRequestedPosMethods,
		requestedTargetTimeLS);
		
	// Verify that a new location request is issued
	iGpsModuleObserver->WaitForCallBack();
	TESTL(iGpsModuleObserver->WasLocationUpdateRequested(iResultedTargetTime, iResultedQuality));
				
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iGpsModuleObserver->IsMoreActivityObserved());
	
	//Now the GPS module issues a request for assistance data
	iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataNone);
	
	//Check that we received the system status advice as a result of the LS request
	iNetworkGatewayChannel->WaitForCallBack();
	TESTL(iNetworkGatewayChannel->WasSystemStatusAdvised());
	
	//Check that assistance data is requested and no self-locate session is started
	iNetworkGatewayChannel->WaitForCallBack();
	TESTL(iNetworkGatewayChannel->WasAssistanceDataRequested());
	
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iNetworkGatewayChannel->IsMoreActivityObserved());
	
	//Now the GPS module issues a location update. It must sutisfy the NRH request, 
	//but it is before the target time for the LS request.  
	iGpsToManagerMessageInjector->InjectLocationUpdate(KErrNone, &iResultedStandardPositionPtr, 1, iRequestedTargetTimeNRH);	
	
	// Verify that a new location request is issued
	iGpsModuleObserver->WaitForCallBack();
	TESTL(iGpsModuleObserver->WasLocationUpdateRequested(iResultedTargetTime, iResultedQuality));
					
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iGpsModuleObserver->IsMoreActivityObserved());
	
	//
	//User::After(KOverlapTargetTime.Int()*1000000);
	
	//Now the GPS module issues a request for assistance data
	iGpsToManagerMessageInjector->InjectAssistanceDataRequest(EAssistanceDataNone);
		
	//Check that the self-locate session is started
	iNetworkGatewayChannel->WaitForCallBack();
	TESTL(iNetworkGatewayChannel->WasSelfLocationRequested());
	
	// Ensure that no other messages were sent to the NG
	TESTL(!iNetworkGatewayChannel->IsMoreActivityObserved());
	
	//Inject a location request from NRH for Terminal Based as a result of self-location request		
	iNrhToManagerRequestInjector->RequestPositionUpdate(iRequestedQuality,iRequestedPosMethods,
		requestedTargetTimeLS);
	
	// Verify that a new location request is issued. The current implementation of the AGPS manager
	//issues a new request even thoug the request params are not changed.
	iGpsModuleObserver->WaitForCallBack();
	TESTL(iGpsModuleObserver->WasLocationUpdateRequested(iResultedTargetTime, iResultedQuality));
	
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iGpsModuleObserver->IsMoreActivityObserved());
		
		
	//Now the GPS module issues a location update. It must satisfy the LS & NRH request.
	iGpsToManagerMessageInjector->InjectLocationUpdate(KErrNone, &iResultedStandardPositionPtr, 1, requestedTargetTimeLS);	
					
	// Ensure that no other messages were sent to the GPS module
	TESTL(!iGpsModuleObserver->IsMoreActivityObserved());
	
	// Ensure that no other messages were sent to the NG
	TESTL(!iNetworkGatewayChannel->IsMoreActivityObserved());
		
	iNetworkGatewayChannel->SendSessionCompleteToAgpsManager(KErrNone);
	
	}

