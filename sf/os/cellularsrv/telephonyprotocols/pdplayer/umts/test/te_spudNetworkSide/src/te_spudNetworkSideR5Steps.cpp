// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the integration tests for R5 Qos parameters.
// 
//

/**
 @file
 @internalComponent
*/


#include "te_spudNetworkSideR5Steps.h"
#include <in_sock.h>

using namespace te_spudNetworkSide; 

/**
Initiates the start (creation & activation) of a secondary PDP context, but does not wait/verify that the context
was actually created / activated 

Set Qos R5 parameters on secondary pdp context.
@leave if the start cannot be initiated: a socket cannot be opened, etc
*/
void CSpudQosR5TestBase::InitiateSecondaryStartL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	ReadQosParameterSettingsFromConfigL();

	UpdatePolicySelector2QosParametersTableL(iPolicySelector, iQosParametersRecord);
#endif

	TestL(iSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp), _L("RSocket::Open"));
	TInetAddr localAddr;
	localAddr.SetPort(KConfiguredTftFilter1DestPort);
	TestL(iSocket.Bind(localAddr), _L("Binding the local Socket"));
	
	TInetAddr dstAddr;
	dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
	dstAddr.Input(KConfiguredTftFilter1SrcAddr);
	
	TRequestStatus status;
	iSocket.Connect(dstAddr, status);
	User::WaitForRequest(status);
	TestL(status.Int(), _L("Connecting to local socket"));

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TQoSSelector selector;
	selector.SetAddr(iSocket);

	TestL(iQoSPolicy_1.Open(selector), _L("Opening Policy"));
	TestL(iQoSPolicy_1.NotifyEvent(*this), _L("Requesting Event notification on QosChannel"));

	CQoSParameters *parameters = new(ELeave) CQoSParameters;
	CleanupStack::PushL(parameters);
	
	SetQoSParameters(*parameters);
	
	TUmtsR5QoSParameters* umtsParameters = new(ELeave) TUmtsR5QoSParameters;
	CleanupStack::PushL(umtsParameters);
	    	    
	SetQoSR5Parameters(*umtsParameters);
	    
	CUmtsR5QoSPolicy *umtsR5Policy = CUmtsR5QoSPolicy::NewL();
	CleanupStack::PushL(umtsR5Policy);
	
    umtsR5Policy->SetQoSRequested(*umtsParameters);
    
    TEST(parameters->AddExtensionL(*umtsR5Policy) == KErrNone);
	    
	TImsParameter* imsParameters = new(ELeave) TImsParameter;
	CleanupStack::PushL(imsParameters);
	    
	SetIMSParameters(*imsParameters);
	    
	CImsPolicy *imsPolicy = CImsPolicy::NewL();
	CleanupStack::PushL(imsPolicy);
	
    imsPolicy->SetImsParameter(*imsParameters);
    
    TEST(parameters->AddExtensionL(*imsPolicy) == KErrNone);
      	 	
	TestL(iQoSPolicy_1.SetQoS(*parameters), _L("Setting R5 Qos Parameters"));
	
	WaitForQoSEventL(_L("SecondaryActivationEvent1"),KErrNone);
		
    CleanupStack::PopAndDestroy(5);
#endif
	}
	
/**
Test operation of the secondary PDP context when there are no network-side events.
Set R5 Qos parameters for secondary PDP context.
@leave if the test fails.
*/	
enum TVerdict CSpudQosR5Secondary::RunTestStepL()
	{
	//start secondary pdp setting  R5 Qos parameters
	StartSecondaryL();
	StopSecondaryL();
	
	return EPass;	
	};

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Test operation of the secondary PDP context when there is a network side event

@leave if the test fails.
*/	
enum TVerdict CSpudSecondaryQosR5Event::RunTestStepL()
	{
	
	TInt idx				 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	//start secondary pdp setting  R5 Qos parameters
	StartSecondaryL();  
	
	EtelRequestL(request, idx);

	WaitForOptionalQoSEventL(_L("ExpQoSEvent1"),_L("ExpQoSEvent1Reason"), _L("OptionalQoSEvent1"), _L("OptionalQoSEvent1Reason"));

	StopSecondaryL();	
		
	return EPass;
	}

/**
Test operation of the secondary PDP context when R5 QoS parameter is changed after the context was activated

@leave if the test fails.
*/		
enum TVerdict CSpudSecondaryChangeR5QoS::RunTestStepL()
	{
	INFO_PRINTF1(_L("CSpudSecondaryChangeQoS R5 test start"));
	
	StartSecondaryL();
	ChangeQoSL();
	StopSecondaryL();
	return EPass;	
	};
#endif

//delete primary pdp context where R5 qos parameters are set on sec pdp context.	
TVerdict CSpudDeletePrimaryR5Qos::RunTestStepL()
{
	 if(SpudDeletePrimaryPdpL())
	   {
	 	return EPass;
	   }
	 
	 return EFail;

}

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/** 
This method must be called after StartSecondaryL() is called.
Change R5 Qos parameters
*/
void CSpudQosR5TestBase::ChangeQoSL()
	{
	CQoSParameters *parameters = new(ELeave) CQoSParameters;
	CleanupStack::PushL(parameters);

	SetQoSParameters1(*parameters);
	 
	TUmtsR5QoSParameters* umtsParameters = new(ELeave) TUmtsR5QoSParameters;
	CleanupStack::PushL(umtsParameters);
	    	    
	ChangeQoSR5Parameters(*umtsParameters);
	    
	CUmtsR5QoSPolicy *umtsR5Policy = CUmtsR5QoSPolicy::NewL();
    umtsR5Policy->SetQoSRequested(*umtsParameters);
    
    TEST(parameters->AddExtensionL(*umtsR5Policy) == KErrNone);
    	
	TestL(iQoSPolicy_1.SetQoS(*parameters), _L("Setting Qos Parameters"));

	WaitForOptionalQoSEventL(_L("ExpQoSEvent1"), _L("ExpQoSEvent1Reason"), _L("OptionalQoSEvent1"), _L("OptionalQoSEvent1Reason"));
	
	WaitForQoSEventL(_L("ExpQoSEvent2"),_L("ExpQoSEvent2Reason"));

	CleanupStack::PopAndDestroy(umtsParameters);
	CleanupStack::PopAndDestroy(parameters);
	}

/**
Set the IMS SI parameters to ETrue
*/
void CSpudQosR5TestBase::SetIMSParameters(TImsParameter& imsParameters)
{
    imsParameters.SetIMSSigallingIndicator(ETrue); 
}
/**
Set the QoS R5 parameters
*/
void CSpudQosR5TestBase::SetQoSR5Parameters(TUmtsR5QoSParameters& umtsParameters)
{
     
    umtsParameters.SetTrafficClass(ETrafficClassStreaming);
    umtsParameters.SetDeliveryOrder(EDeliveryOrderNotRequired);
    umtsParameters.SetDeliveryOfErroneusSdu(EErroneousSDUDeliveryNotRequired);
    umtsParameters.SetResidualBer(EBEROnePerThousand);
    umtsParameters.SetErrorRatio(ESDUErrorRatioOnePerThousand);
    umtsParameters.SetPriority(ETrafficPriority1);
    umtsParameters.SetTransferDelay(1);
    umtsParameters.SetMaxSduSize(0);
    umtsParameters.SetMaxBitrateUplink(2048);
    umtsParameters.SetMaxBitrateDownlink(2048);
    umtsParameters.SetGuaranteedBitrateUplink(128);
    umtsParameters.SetGuaranteedBitrateDownlink(128);
    
    umtsParameters.SetSignallingIndicator(ETrue);
    umtsParameters.SetSourceStatisticsDescriptor(ESourceStatisticsDescriptorSpeech);
      
}
/**
Resets Qos R5 parameters.
*/
void CSpudQosR5TestBase::ChangeQoSR5Parameters(TUmtsR5QoSParameters& umtsParameters)
{
         
    umtsParameters.SetTrafficClass(ETrafficClassStreaming);
    umtsParameters.SetDeliveryOrder(EDeliveryOrderNotRequired);
    umtsParameters.SetDeliveryOfErroneusSdu(EErroneousSDUDeliveryNotRequired);
    umtsParameters.SetResidualBer(EBEROnePerThousand);
    umtsParameters.SetErrorRatio(ESDUErrorRatioOnePerThousand);
    umtsParameters.SetPriority(ETrafficPriority1);
    umtsParameters.SetTransferDelay(1);
    umtsParameters.SetMaxSduSize(0);
    umtsParameters.SetMaxBitrateUplink(2048);
    umtsParameters.SetMaxBitrateDownlink(2048);
    umtsParameters.SetGuaranteedBitrateUplink(128);
    umtsParameters.SetGuaranteedBitrateDownlink(128);
        
    umtsParameters.SetSignallingIndicator(EFalse);
    umtsParameters.SetSourceStatisticsDescriptor(ESourceStatisticsDescriptorUnknown);
     
}
#endif
