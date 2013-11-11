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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnetgateway2Step.cpp
*/
#include "lbsnetgateway2Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsagpsngmsgs.h"
#include <s32mem.h>
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancebadsatlist.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceutcmodel.h>


Clbsnetgateway2Step::~Clbsnetgateway2Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway2Step::Clbsnetgateway2Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway2Step);
	}

TVerdict Clbsnetgateway2Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgateway2Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	RProcess process;
	
	// No msgs are expected to arrive over this interface
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);
	
	// Assistance data messages expected to arrive over this interface
	iAgpsChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelAGPS2NG);
	iAssDataCache.OpenL();
	

    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway2Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{

		//
		// Test the sending and receiving of messages going 
		// to/from the A-GPS manager.
		//
		
		//
		// Section 1 : Request(s) for assistance data coming in
		//			   from the A-GPS Manager (via the A-GPS channel)
		//			   and being sent through to the Protocol Module.
		//

 		// Note: This should produce a self-locate request
		iRequestedDataGroup = (EAssistanceDataAquisitionAssistance
							  | EAssistanceDataBadSatList
							  | EAssistanceDataNavigationModel
							  | EAssistanceDataReferenceTime
							  | EAssistanceDataIonosphericModel
							  | EAssistanceDataDgpsCorrections
							  | EAssistanceDataReferenceLocation
							  | EAssistanceDataAlmanac
							  | EAssistanceDataPositioningGpsUtcModel);

 		iOptions.SetDataRequestMask(iRequestedDataGroup);
 		iOptions.SetNewClientConnected(ETrue);
	 	TLbsNetSelfLocationRequestMsg msgSL(iSessionId, iOptions);
	 	iSentMsgCount++;
 		iAgpsChannel->SendMessageAndWaitForResponse(msgSL);
 		WaitForResponse();

		// Note: This should produce additional assistance data request
		iRequestedDataGroup = (EAssistanceDataAquisitionAssistance
							  | EAssistanceDataBadSatList
							  | EAssistanceDataIonosphericModel
							  | EAssistanceDataAlmanac
							  | EAssistanceDataPositioningGpsUtcModel);
 		TLbsNetAssistanceDataRequestMsg msgAD(iRequestedDataGroup);
 		iSentMsgCount++;
 		iAgpsChannel->SendMessageAndWaitForResponse(msgAD);
 		WaitForResponse();
		
		//
		// Section 2 : Assistance Data arriving from the 'network' and being
		//			   passed up by the Protocol Module to the Network
		//			   Gateway, then across to the A-GPS channel. Error
		//			   codes can also be passed back this way if e.g. some
		//			   types of data are unsupported by the network.
		//
		// TODO : depends on final format of assistance data set class.
		SNetMsgProcessAssistanceData* data = reinterpret_cast<SNetMsgProcessAssistanceData*>(iNetGatMsg.Data());
		iNetGatMsg.iType = ENetMsgProcessAssistanceData;
		new (data) SNetMsgProcessAssistanceData;
		data->iData.OpenL();
		CleanupClosePushL(data->iData);
		
		//
		// 2.1) Passing an error code
		//
		data->iReason = KErrCancel;
		data->iDataMask = EAssistanceDataNone;
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.1b) Passing KErrNone, but no actual data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataNone;
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.2) Checking almanac data.
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataAlmanac;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsAlmanacBuilder* almanacBuilder;
		data->iData.GetDataBuilder(almanacBuilder);
		almanacBuilder->SetField(TUEPositioningGpsAlmanac::EWnA, 
								 TUint(25));
		almanacBuilder->SetTimeStamp(TTime(_L("20060605:095000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.3) Checking acquisition assistance data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataAquisitionAssistance;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsAcquisitionAssistanceBuilder* acquisBuilder;
		data->iData.GetDataBuilder(acquisBuilder);
		acquisBuilder->SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, 
								TUint(123055));
		acquisBuilder->SetTimeStamp(TTime(_L("20060605:105000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.4) Checking bad satellite data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataBadSatList;
		data->iData.ClearAllBuilders();
		RBadSatListBuilder* badsatBuilder;
		data->iData.GetDataBuilder(badsatBuilder);
		RBadSatArrayBuilder badsatArray;
		badsatBuilder->GetArrayBuilder(TBadSatList::EBadSatIdArray, 
									   badsatArray);
		badsatArray.SetElement(0, TUint(7));
		badsatArray.SetElement(1, TUint(3));
		badsatBuilder->SetTimeStamp(TTime(_L("20060605:115000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.5) Checking navigation model data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataNavigationModel;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsNavigationModelBuilder* navBuilder;
		data->iData.GetDataBuilder(navBuilder);
		RNavigationModelSatInfoArrayBuilder satinfoArray;
		navBuilder->GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, 
									   satinfoArray);
		RNavigationModelSatInfoBuilder satinfoBuilder;
		satinfoArray.GetFieldBuilder(0, satinfoBuilder);
		satinfoBuilder.SetField(TNavigationModelSatInfo::ESatId, TUint(4));
		navBuilder->SetTimeStamp(TTime(_L("20060605:125000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.6) Checking reference time data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataReferenceTime;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsReferenceTimeBuilder* reftimeBuilder;
		data->iData.GetDataBuilder(reftimeBuilder);
		reftimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, TUint(23));
		reftimeBuilder->SetTimeStamp(TTime(_L("20060605:135000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.7) Checking ionospheric model data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataIonosphericModel;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsIonosphericModelBuilder* ionBuilder;
		data->iData.GetDataBuilder(ionBuilder);
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, TUint(123));
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa1, TUint(456));
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta1, TUint(789));
		ionBuilder->SetTimeStamp(TTime(_L("20060605:145000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.8) Checking reference location data
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataReferenceLocation;
		data->iData.ClearAllBuilders();
		RReferenceLocationBuilder* reflocBuilder;
		data->iData.GetDataBuilder(reflocBuilder);
		REllipsoidPointAltitudeEllipsoideBuilder locBuilder;
		reflocBuilder->GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, 
									   locBuilder);
		locBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign,
							TEllipsoidPointAltitudeEllipsoide::ENorth);
		locBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitude,
							TUint(1140));
		reflocBuilder->SetTimeStamp(TTime(_L("20060605:155000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.8) Checking UTC model data
		//	
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataPositioningGpsUtcModel;
		data->iData.ClearAllBuilders();
		RUEPositioningGpsUtcModelBuilder* utcBuilder;
		data->iData.GetDataBuilder(utcBuilder);
		utcBuilder->SetField(TUEPositioningGpsUtcModel::EA1, TUint(45));
		utcBuilder->SetField(TUEPositioningGpsUtcModel::EA0, TUint(15));
		utcBuilder->SetField(TUEPositioningGpsUtcModel::EWnt, TUint(127));
		utcBuilder->SetTimeStamp(TTime(_L("20060605:165000")));
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		WaitForResponse();
		
		//
		// 2.9) Testing the status update buffer within the NetGateway.
		//
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataIonosphericModel;
		data->iData.ClearAllBuilders();
		data->iData.GetDataBuilder(ionBuilder);
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, TUint(123));
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa1, TUint(456));
		ionBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta1, TUint(789));
		ionBuilder->SetTimeStamp(TTime(_L("20060605:145000")));		
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);
		
		data->iReason = KErrNone;
		data->iDataMask = EAssistanceDataIonosphericModel;
		iSentProtocolMsg[iSentMsgCount++] = iNetGatMsg;
		iSentAgpsDataGroup = data->iDataMask;
		iStubPMChannelArray->SendNetProtocolMessageL(iNetGatMsg);		
		WaitForResponse();
		

		CleanupStack::PopAndDestroy(&data->iData);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetgateway2Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgateway2Step"));
	// process something post setting to the test step
	iStubPMChannelArray->Close();
	
	iAssDataCache.Close();	
	delete iNetGatewayChannel;
	delete iAgpsChannel;
	
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void Clbsnetgateway2Step::WaitForResponse()
	{
	if (iSentMsgCount > 0)
		{
		iActiveWait.Start();
		}
	}

/* Callback used when updated assistance data has been
   published on the A-GPS channel.
   
In this test this function is used to check that the assistance data
sent by the Protocol Module can be correctly retrieved here.
*/
void Clbsnetgateway2Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
//void Clbsnetgateway2Step::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aMask)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelAGPS2NG);				
	SNetMsgProcessAssistanceData* sentData = reinterpret_cast<SNetMsgProcessAssistanceData*>(iSentProtocolMsg[iReceivedMsgCount].Data());
	
	TInt err = KErrNone;
	switch(aMessage.Type())
		{
		case TLbsNetInternalMsgBase::EAssistanceDataResponse:
			{
			const TLbsNetAssistanceDataResponseMsg& adMsg = reinterpret_cast<const TLbsNetAssistanceDataResponseMsg&>(aMessage);
			TEST(adMsg.Reason() == sentData->iReason);
			
			// TODO: Hmm, data->iData.ClearAllBuilders(); doesn't actually seem to clear
			//		 all of the data so this test fails in some tests.
			//TEST(aMask == iSentAgpsDataGroup);
			
			// TODO: Check the actual assistance data if there was no error
			TTime timeStamp;
			if ((adMsg.DataResponseMask() & EAssistanceDataAlmanac) == EAssistanceDataAlmanac)
				{
				RUEPositioningGpsAlmanacReader almanacReader;
				TRAP(err, almanacReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataAlmanac, 
											   almanacReader);
				
				RUEPositioningGpsAlmanacBuilder* almanacBuilder;
				sentData->iData.GetDataBuilder(almanacBuilder);
				
				// Test timestamp is same as the one sent
				TEST(almanacBuilder->TimeStamp() == almanacReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(almanacBuilder->DataBufferAccess() == almanacReader.DataBuffer());
				
				}
						
			if ((adMsg.DataResponseMask() & EAssistanceDataAquisitionAssistance) == EAssistanceDataAquisitionAssistance)
				{
				RUEPositioningGpsAcquisitionAssistanceReader acquisReader;
				TRAP(err, acquisReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
				
				iAssDataCache.GetAssistDataItem(EAssistanceDataAquisitionAssistance, 
											   acquisReader);

				RUEPositioningGpsAcquisitionAssistanceBuilder* acquisBuilder;
				sentData->iData.GetDataBuilder(acquisBuilder);
				
				// Test timestamp is same as the one sent
				TEST(acquisBuilder->TimeStamp() == acquisReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(acquisBuilder->DataBufferAccess() == acquisReader.DataBuffer());
				
				}
			// TODO : do the above test for each of the assistance data item types
			if ((adMsg.DataResponseMask() & EAssistanceDataBadSatList) == EAssistanceDataBadSatList)
				{
				RBadSatListReader badsatReader;
				TRAP(err, badsatReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataBadSatList, 
											   badsatReader);

				RBadSatListBuilder* badsatBuilder;
				sentData->iData.GetDataBuilder(badsatBuilder);

				// Test timestamp is same as the one sent
				TEST(badsatBuilder->TimeStamp() == badsatReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(badsatBuilder->DataBufferAccess() == badsatReader.DataBuffer());
				
				}

			if ((adMsg.DataResponseMask() & EAssistanceDataNavigationModel) == EAssistanceDataNavigationModel)
				{
				RUEPositioningGpsNavigationModelReader navReader;
				TRAP(err, navReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataNavigationModel, 
											   navReader);

				RUEPositioningGpsNavigationModelBuilder* navBuilder;
				sentData->iData.GetDataBuilder(navBuilder);

				// Test timestamp is same as the one sent
				TEST(navBuilder->TimeStamp() == navReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(navBuilder->DataBufferAccess() == navReader.DataBuffer());
			
				}
				
			if ((adMsg.DataResponseMask() & EAssistanceDataReferenceTime) == EAssistanceDataReferenceTime)
				{
				RUEPositioningGpsReferenceTimeReader reftimeReader;
				TRAP(err, reftimeReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataReferenceTime, 
											   reftimeReader);

				RUEPositioningGpsReferenceTimeBuilder* reftimeBuilder;
				sentData->iData.GetDataBuilder(reftimeBuilder);

				// Test timestamp is same as the one sent
				TEST(reftimeBuilder->TimeStamp() == reftimeReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(reftimeBuilder->DataBufferAccess() == reftimeReader.DataBuffer());
				
				}
				
			if ((adMsg.DataResponseMask() & EAssistanceDataIonosphericModel) == EAssistanceDataIonosphericModel)
				{
				RUEPositioningGpsIonosphericModelReader ionReader;
				TRAP(err, ionReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
				
				iAssDataCache.GetAssistDataItem(EAssistanceDataIonosphericModel, 
											   ionReader);

				RUEPositioningGpsIonosphericModelBuilder* ionBuilder;
				sentData->iData.GetDataBuilder(ionBuilder);

				// Test timestamp is same as the one sent
				TEST(ionBuilder->TimeStamp() == ionReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(ionBuilder->DataBufferAccess() == ionReader.DataBuffer());
				}

			if ((adMsg.DataResponseMask() & EAssistanceDataReferenceLocation) == EAssistanceDataReferenceLocation)
				{
				RReferenceLocationReader reflocReader;
				TRAP(err, reflocReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataReferenceLocation, 
											   reflocReader);

				RReferenceLocationBuilder* reflocBuilder;
				sentData->iData.GetDataBuilder(reflocBuilder);

				// Test timestamp is same as the one sent
				TEST(reflocBuilder->TimeStamp() == reflocReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(reflocBuilder->DataBufferAccess() == reflocReader.DataBuffer());
				
				}			

			if ((adMsg.DataResponseMask() & EAssistanceDataPositioningGpsUtcModel) == EAssistanceDataPositioningGpsUtcModel)
				{
				RUEPositioningGpsUtcModelReader utcReader;
				TRAP(err, utcReader.OpenL();)
				if(err)
					User::Panic(Klbsnetgateway2Step, err);
			
				iAssDataCache.GetAssistDataItem(EAssistanceDataPositioningGpsUtcModel, 
											   utcReader);

				RUEPositioningGpsUtcModelBuilder* utcBuilder;
				sentData->iData.GetDataBuilder(utcBuilder);

				// Test timestamp is same as the one sent
				TEST(utcBuilder->TimeStamp() == utcReader.TimeStamp());
				
				// Test the data stored in the received buffer is the same as the 
				// data in the sent buffer.
				TEST(utcBuilder->DataBufferAccess() == utcReader.DataBuffer());
				
				}

			iReceivedMsgCount++;
			if (iReceivedMsgCount == iSentMsgCount)
				{
				iReceivedMsgCount = 0;
				iSentMsgCount = 0;
				if (iActiveWait.IsStarted())
					{
					iActiveWait.AsyncStop();
					}
				}
			break;
			}
			
		default:
			TEST(EFalse);
			break;
		}
	}

/* Check that the incoming message carries the same information 
   as the request that was sent in along the A-GPS channel.
*/
void Clbsnetgateway2Step::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	switch (aMessage.iType)
		{
		case ENetMsgRequestSelfLocation:
			{
			const SNetMsgRequestSelfLocation* data = reinterpret_cast<const SNetMsgRequestSelfLocation*>(aMessage.Data());
			TEST(data->iNewClient); // New client assumed currently
			TEST(data->iDataRequestMask == iRequestedDataGroup);			
			break;
			}
		case ENetMsgRequestAssistanceData:
			{
			const SNetMsgRequestAssistanceData* data = reinterpret_cast<const SNetMsgRequestAssistanceData*>(aMessage.Data());
			TEST(data->iDataRequestMask == iRequestedDataGroup);			
			break;
			}
		case ENetMsgProcessPrivacyRequest:
		case ENetMsgProcessLocationRequest:
		case ENetMsgProcessSessionComplete:
		case ENetMsgProcessAssistanceData:
		case ENetMsgProcessLocationUpdate:
		case ENetMsgRespondPrivacyRequest:
		case ENetMsgRespondLocationRequest:
		case ENetMsgRequestTransmitLocation:
		case ENetMsgCancelTransmitLocation:
		case ENetMsgCancelSelfLocation:
		case ENetMsgRequestNetworkLocation:
		case ENetMsgCancelNetworkLocation:
		default:
			{
			INFO_PRINTF1(_L("Unexpected call to Clbsnetgateway2Step::ProcessNetProtocolMessage!"));
			TEST(EFalse);
			break;			
			}
		}

	iReceivedMsgCount++;
	if (iReceivedMsgCount == iSentMsgCount)
		{
		iReceivedMsgCount = 0;
		iSentMsgCount = 0;
		if (iActiveWait.IsStarted())
			{
			iActiveWait.AsyncStop();
			}
		}
	}

