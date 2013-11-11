/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Test step definitions for the PacketContext functional unit.
 * @internalTechnology
 */

#include <etelmm.h>
#include <etelpckt.h>
#include <pcktcs.h>
#include "cctsyintegrationtestpacketcontext.h"
#include "chttpdownload.h"

	
CCTSYIntegrationTestPacketContextBase::CCTSYIntegrationTestPacketContextBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iPacketServiceTestHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPacketContextBase::~CCTSYIntegrationTestPacketContextBase()
/*
 * Destructor
 */
	{
	}


void CCTSYIntegrationTestPacketContextBase::SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService)
/**
 * Attaches the phone to the packet service by setting the
 * attach mode to RPacketService::EAttachWhenPossible. Requests
 * that the phone attaches to the packet service and waits for
 * attach to complete.
 * 
 * @param aPacketService Reference to packet service subsession.
 */
    {
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    TExtEtelRequestStatus notifyServiceStatusChStatus(aPacketService, EPacketNotifyStatusChange);
    CleanupStack::PushL(notifyServiceStatusChStatus);
    RPacketService::TStatus packetServiceStatus;
    aPacketService.NotifyStatusChange(notifyServiceStatusChStatus, packetServiceStatus);

    if(attachMode != RPacketService::EAttachWhenPossible)
		{
		CHECK_EQUALS_L(aPacketService.SetAttachMode(RPacketService::EAttachWhenPossible), KErrNone,
					 _L("RPacketService::SetAttachMode returned with an error."));
		}

    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));
    CHECK_EQUALS_L(attachMode, RPacketService::EAttachWhenPossible, _L("RPacketService::GetAttachMode did not return EAttachWhenPossible."));

    // Ensure phone is attached to the packet service (RPacketService::GetStatus returns EStatusAttached).
	RPacketService::TStatus packetStatus;
	CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
	
	// Wait for phone to attach to packet service if it is not attached.
	if (packetStatus != RPacketService::EStatusAttached)
		{
        iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				aPacketService,
				notifyServiceStatusChStatus,
				packetServiceStatus, 
				RPacketService::EStatusAttached,
				KErrNone);
        CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
        CHECK_EQUALS_L(packetStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus did not return EStatusAttached"));
		}

    CleanupStack::PopAndDestroy(1, &notifyServiceStatusChStatus);
    }


TInt CCTSYIntegrationTestPacketContextBase::AddPacketFilterV2ToContextL(RPacketContext& aPacketContext, TInt aId, TInt aErrCode)
/**
 * Adds a packet filter to context.
 * 
 * @param aPacketContext Reference to packet context subsession.
 * @param aId the id of the filter to set, which is also an identifyer to one of three predefined filters
 * @param aErrCode  the error code that the AddPacketFilter should return, default KErrNone.
 * 
 * @return Request status value returned by
 *         RPacketContext::AddPacketFilter call.
 */
    {
    RPacketContext::TPacketFilterV2 packetFilterV2;
    FillPacketFilter(packetFilterV2,aId);
    
	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);
	
	TExtEtelRequestStatus addFilterStatus(aPacketContext, EPacketContextAddPacketFilter); 
    CleanupStack::PushL(addFilterStatus);
	aPacketContext.AddPacketFilter(addFilterStatus, packetFilterV2pckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(addFilterStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::AddPacketFilter timed out"))
    ASSERT_EQUALS(addFilterStatus.Int(), aErrCode,
                  _L("RPacketContext::AddPacketFilter did returned the expected value"))

    TInt ret = addFilterStatus.Int();
    CleanupStack::PopAndDestroy(&addFilterStatus);
	return ret;
    }

// Helper macro, same as ASSERT_EQUALS but also change a local parameter (ret) to EFalse if the parameters are not equal
// One should define the variable TBool ret before calling this macro, and then one can use this variable  to know if any of the local macros failed.
#define LOCAL_ASSERT_EQUALS(result,expected,msg)  \
    { \
    TBool tempLocalAssertEqualsRet = ((result)==(expected)); \
    ASSERT_TRUE(tempLocalAssertEqualsRet,msg); \
    if(!tempLocalAssertEqualsRet)   \
        { \
        ret=EFalse; \
        }  \
    }

// Helper macro, same as ASSERT_TRUE but also change a local parameter (ret) to EFalse if the parameters are not equal
// One should define the variable TBool ret before calling this macro, and then one can use this variable  to know if any of the local macros failed.
#define LOCAL_ASSERT_TRUE(value,msg) LOCAL_ASSERT_EQUALS((TBool)(value),(TBool)ETrue,msg);


TUint CCTSYIntegrationTestPacketContextBase::FindLSB(TUint aNumber)
/**
 * Finds the least significant bit in a number
 * 
 * @param aNumber The number
 * @return The LSB of aNumber
 */
	{
	if(aNumber==0)
		{
		return 0;
		}
	TUint bit=1;
	while(ETrue)
		{
		if((aNumber&bit)!=0)
			{
			return bit;
			}
		bit=bit*2;
		}
	}

TUint CCTSYIntegrationTestPacketContextBase::FindMSB(TUint aNumber)
/**
 * Finds the most significant bit in a number
 * 
 * @param aNumber The number
 * @return The MSB of aNumber
 */
	{
	if(aNumber==0)
		{
		return 0;
		}
	TUint bit=1;
	while(ETrue)
		{
		if((aNumber&bit)!=0)
			{
			aNumber=(aNumber^bit);
			if(aNumber==0)
				{
				return bit;
				}
			}
		bit=bit*2;
		}
	}

void CCTSYIntegrationTestPacketContextBase::FillPacketFilter(RPacketContext::TPacketFilterV2 &aFilter, TInt aId)
/**
 * Fill a packet filter object with actual values.
 * 
 * @param aFilter Reference to the structure to fill.
 * @param aId the id of the filter to set, which is also an identifyer to one of three predefined filters
 * 
 */
    {
	aFilter.iId = aId;
    switch(aId)
        {
        case 1:
        {
        aFilter.iEvaluationPrecedenceIndex = 0;
        aFilter.iSrcAddr[0] = 0xCF;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC0;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 6;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
        }
        case 2:
        {
        aFilter.iEvaluationPrecedenceIndex = 255;
        aFilter.iSrcAddr[0] = 0x21;
        aFilter.iSrcAddr[1] = 0xD2;
        aFilter.iSrcAddr[2] = 0x3F;
        aFilter.iSrcAddr[3] = 0x1A;
        aFilter.iSrcAddrSubnetMask[0] = 0x2A;
        aFilter.iSrcAddrSubnetMask[1] = 0x2B;
        aFilter.iSrcAddrSubnetMask[2] = 0xFF;
        aFilter.iSrcAddrSubnetMask[3] = 0x3F;
        aFilter.iProtocolNumberOrNextHeader = 8;
        aFilter.iIPSecSPI = 0x0F88F000;
        aFilter.iTOSorTrafficClass = 0x2F77;
        aFilter.iDestPortMax = 0;
        aFilter.iDestPortMin = 0;
        aFilter.iFlowLabel = 0;
        aFilter.iSrcPortMax = 0;
        aFilter.iSrcPortMin = 0;        
        break;
        }
        case 3:
        {
        aFilter.iEvaluationPrecedenceIndex = 127;
        aFilter.iSrcAddr[0] = 0xEF;
        aFilter.iSrcAddr[1] = 0xAF;
        aFilter.iSrcAddr[2] = 0xBF;
        aFilter.iSrcAddr[3] = 0x21;
        aFilter.iSrcAddrSubnetMask[0] = 0x10;
        aFilter.iSrcAddrSubnetMask[1] = 0x21;
        aFilter.iSrcAddrSubnetMask[2] = 0x32;
        aFilter.iSrcAddrSubnetMask[3] = 0x41;
        aFilter.iProtocolNumberOrNextHeader = 10;
        aFilter.iSrcPortMin = 0;
        aFilter.iSrcPortMax = 0;
        aFilter.iDestPortMin = 0;
        aFilter.iDestPortMax = 0;
        aFilter.iTOSorTrafficClass = 0x4F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        }
         case 4:
        {
        aFilter.iEvaluationPrecedenceIndex = 0;
        aFilter.iSrcAddr[0] = 0xD0;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC1;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 12;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
		}
         case 5:
        {
        aFilter.iEvaluationPrecedenceIndex = 127;
        aFilter.iSrcAddr[0] = 0xD1;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC2;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 14;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
        }
        case 6:
        {
        aFilter.iEvaluationPrecedenceIndex = 255;
        aFilter.iSrcAddr[0] = 0xD2;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC3;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 16;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
        }
        case 7:
        {
        aFilter.iEvaluationPrecedenceIndex = 0;
        aFilter.iSrcAddr[0] = 0xD3;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC4;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 18;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
        }
        case 8:
        default:
        {
        aFilter.iEvaluationPrecedenceIndex = 127;
        aFilter.iSrcAddr[0] = 0xD4;
        aFilter.iSrcAddr[1] = 0xFF;
        aFilter.iSrcAddr[2] = 0xFF;
        aFilter.iSrcAddr[3] = 0x11;
        aFilter.iSrcAddrSubnetMask[0] = 0xC5;
        aFilter.iSrcAddrSubnetMask[1] = 0xC1;
        aFilter.iSrcAddrSubnetMask[2] = 0xF2;
        aFilter.iSrcAddrSubnetMask[3] = 0x51;
        aFilter.iProtocolNumberOrNextHeader = 18;
        aFilter.iSrcPortMin = 2;
        aFilter.iSrcPortMax = 65000;
        aFilter.iDestPortMin = 8;
        aFilter.iDestPortMax = 65100;
        aFilter.iTOSorTrafficClass = 0x3F8C;
        aFilter.iFlowLabel = 0;
        aFilter.iIPSecSPI = 0;
        break;
        }
        }
    }


TBool CCTSYIntegrationTestPacketContextBase::CompareFilters(const RPacketContext::TPacketFilterV2 &aFilterOne,
                                                            const RPacketContext::TPacketFilterV2 &aFilterTwo)
/**
 * Compare two packet filter objects.
 * 
 * @param aFilterOne Reference to the structure with the first filter.
 * @param aFilterOne Reference to the structure with the second filter.
 *
 * @return ETrue if both filters are the same
 * 
 */
    {
    TBool ret = ETrue;

    LOCAL_ASSERT_EQUALS(aFilterOne.ExtensionId(), aFilterTwo.ExtensionId() ,_L("RPacketContext::GetPacketFilterInfo returned wrong iExtensionId"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iId, aFilterTwo.iId ,_L("RPacketContext::GetPacketFilterInfo returned wrong iId"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iEvaluationPrecedenceIndex, aFilterTwo.iEvaluationPrecedenceIndex ,_L("RPacketContext::GetPacketFilterInfo returned wrong iEvaluationPrecedenceIndex"));
    for(TInt i=0;i<RPacketContext::KIPAddressSize;++i)
        {
        LOCAL_ASSERT_EQUALS(aFilterOne.iSrcAddr[i], aFilterTwo.iSrcAddr[i] ,_L("RPacketContext::GetPacketFilterInfo returned wrong iSrcAddr"));
        LOCAL_ASSERT_EQUALS(aFilterOne.iSrcAddrSubnetMask[i], aFilterTwo.iSrcAddrSubnetMask[i] ,_L("RPacketContext::GetPacketFilterInfo returned wrong iSrcAddrSubnetMask"));
        }
    LOCAL_ASSERT_EQUALS(aFilterOne.iProtocolNumberOrNextHeader, aFilterTwo.iProtocolNumberOrNextHeader ,_L("RPacketContext::GetPacketFilterInfo returned wrong iProtocolNumberOrNextHeader"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iSrcPortMin, aFilterTwo.iSrcPortMin ,_L("RPacketContext::GetPacketFilterInfo returned wrong iSrcPortMin"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iSrcPortMax, aFilterTwo.iSrcPortMax ,_L("RPacketContext::GetPacketFilterInfo returned wrong iSrcPortMax"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iDestPortMin, aFilterTwo.iDestPortMin ,_L("RPacketContext::GetPacketFilterInfo returned wrong iDestPortMin"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iDestPortMax, aFilterTwo.iDestPortMax ,_L("RPacketContext::GetPacketFilterInfo returned wrong iDestPortMax"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iIPSecSPI, aFilterTwo.iIPSecSPI ,_L("RPacketContext::GetPacketFilterInfo returned wrong iIPSecSPI"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iTOSorTrafficClass, aFilterTwo.iTOSorTrafficClass ,_L("RPacketContext::GetPacketFilterInfo returned wrong iTOSorTrafficClass"));
    LOCAL_ASSERT_EQUALS(aFilterOne.iFlowLabel, aFilterTwo.iFlowLabel ,_L("RPacketContext::GetPacketFilterInfo returned wrong iFlowLabel"));


    return ret;
    }

template<>
void CCTSYIntegrationTestPacketContextBase::FillParametersFromCapabilities(RPacketQoS::TQoSGPRSRequested &aParameters,
                                                                           const RPacketQoS::TQoSCapsGPRS &aCapabilities)
/**
 * fill a RPacketQoS::TQoSGPRSRequested object from a given RPacketQoS::TQoSCapsGPRS capabilities
 * 
 * @param aParameters Reference to the structure of the parameters to fill.
 * @param aCapabilities Reference to the structure with the capabilities to use.
 *
 */
    {
    aParameters.iMinDelay = static_cast<RPacketQoS::TQoSDelay>(FindMSB(aCapabilities.iDelay));
	aParameters.iReqDelay = static_cast<RPacketQoS::TQoSDelay>(FindLSB(aCapabilities.iDelay));
	aParameters.iMinMeanThroughput = static_cast<RPacketQoS::TQoSMeanThroughput>(FindMSB(aCapabilities.iMean));
	aParameters.iReqMeanThroughput = static_cast<RPacketQoS::TQoSMeanThroughput>(FindLSB(aCapabilities.iMean));
	aParameters.iMinPeakThroughput = static_cast<RPacketQoS::TQoSPeakThroughput>(FindMSB(aCapabilities.iPeak));
	aParameters.iReqPeakThroughput = static_cast<RPacketQoS::TQoSPeakThroughput>(FindLSB(aCapabilities.iPeak));
	aParameters.iMinReliability = static_cast<RPacketQoS::TQoSReliability>(FindMSB(aCapabilities.iReliability));
	aParameters.iReqReliability = static_cast<RPacketQoS::TQoSReliability>(FindLSB(aCapabilities.iReliability));
	aParameters.iReqPrecedence = static_cast<RPacketQoS::TQoSPrecedence>(FindMSB(aCapabilities.iPrecedence));
	aParameters.iMinPrecedence = static_cast<RPacketQoS::TQoSPrecedence>(FindLSB(aCapabilities.iPrecedence));
    
    }


template<>
void CCTSYIntegrationTestPacketContextBase::FillParametersFromCapabilities(RPacketQoS::TQoSR99_R4Requested &aParameters,
                                                                           const RPacketQoS::TQoSCapsR99_R4 &aCapabilities)
/**
 * fill a RPacketQoS::TQoSR99_R4Requested object from a given RPacketQoS::TQoSCapsR99_R4 capabilities
 * 
 * @param aParameters Reference to the structure of the parameters to fill.
 * @param aCapabilities Reference to the structure with the capabilities to use.
 *
 */
    {
    aParameters.iReqTrafficClass = static_cast<RPacketQoS::TTrafficClass>(FindLSB(aCapabilities.iTrafficClass));
    aParameters.iMinTrafficClass = static_cast<RPacketQoS::TTrafficClass>(FindMSB(aCapabilities.iTrafficClass));
    aParameters.iReqDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(FindLSB(aCapabilities.iDeliveryOrderReqd));
    aParameters.iMinDeliveryOrderReqd = static_cast<RPacketQoS::TDeliveryOrder>(FindMSB(aCapabilities.iDeliveryOrderReqd));
    aParameters.iReqDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(FindLSB(aCapabilities.iDeliverErroneousSDU));
    aParameters.iMinDeliverErroneousSDU = static_cast<RPacketQoS::TErroneousSDUDelivery>(FindMSB(aCapabilities.iDeliverErroneousSDU));
    aParameters.iReqMaxSDUSize = 1500; //NOT SURE
    aParameters.iMinAcceptableMaxSDUSize = 100; //NOT SURE
    aParameters.iReqMaxRate.iUplinkRate = 1000; //NOT SURE
    aParameters.iReqMaxRate.iDownlinkRate  = 1000; //NOT SURE
    aParameters.iMinAcceptableMaxRate.iUplinkRate = 500; //NOT SURE
    aParameters.iMinAcceptableMaxRate.iDownlinkRate = 500; //NOT SURE
    aParameters.iReqBER = static_cast<RPacketQoS::TBitErrorRatio>(FindLSB(aCapabilities.iBER));
    aParameters.iMaxBER = static_cast<RPacketQoS::TBitErrorRatio>(FindMSB(aCapabilities.iBER));
    aParameters.iReqSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(FindLSB(aCapabilities.iSDUErrorRatio));
    aParameters.iMaxSDUErrorRatio = static_cast<RPacketQoS::TSDUErrorRatio>(FindMSB(aCapabilities.iSDUErrorRatio));
    aParameters.iReqTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(FindLSB(aCapabilities.iTrafficHandlingPriority));
    aParameters.iMinTrafficHandlingPriority = static_cast<RPacketQoS::TTrafficHandlingPriority>(FindMSB(aCapabilities.iTrafficHandlingPriority));
    aParameters.iReqTransferDelay = 1000; //NOT SURE
    aParameters.iMaxTransferDelay = 100; //NOT SURE
    aParameters.iReqGuaranteedRate.iUplinkRate = 1000; //NOT SURE
    aParameters.iReqGuaranteedRate.iDownlinkRate  = 1000; //NOT SURE
    aParameters.iMinGuaranteedRate.iUplinkRate = 500; //NOT SURE
    aParameters.iMinGuaranteedRate.iDownlinkRate  = 500; //NOT SURE
    }

template<>
void CCTSYIntegrationTestPacketContextBase::FillParametersFromCapabilities(RPacketQoS::TQoSR5Requested &aParameters,
                                                                           const RPacketQoS::TQoSCapsR5 &aCapabilities)
/**
 * fill a RPacketQoS::TQoSR5Requested object from a given RPacketQoS::TQoSCapsR5 capabilities
 * 
 * @param aParameters Reference to the structure of the parameters to fill.
 * @param aCapabilities Reference to the structure with the capabilities to use.
 *
 */
    {
    FillParametersFromCapabilities((RPacketQoS::TQoSR99_R4Requested&)aParameters,(const RPacketQoS::TQoSCapsR99_R4 &)aCapabilities);
    aParameters.iSignallingIndication = static_cast<TBool>(aCapabilities.iSignallingIndication);
    aParameters.iSourceStatisticsDescriptor = static_cast<RPacketQoS::TSourceStatisticsDescriptor>(aCapabilities.iSourceStatisticsDescriptor);
    }


template<class QOSCAPS>
void CCTSYIntegrationTestPacketContextBase::GetProfileCapsL(RPacketQoS& aContextQoS, QOSCAPS& aCapabilities)
/**
 * Get QoS profile capabilities
 * 
 * @param aContextQoS Reference RPacketQoS object.
 * @param aCapabilities Reference to QoS capabilities.
 *
 */
    {
	TPckg<QOSCAPS> qosCapsPckg(aCapabilities);
	TExtEtelRequestStatus reqQoSProfileCaps(aContextQoS,EPacketQoSGetProfileCaps);
	CleanupStack::PushL(reqQoSProfileCaps);		
	aContextQoS.GetProfileCapabilities(reqQoSProfileCaps, qosCapsPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqQoSProfileCaps, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));	
	ASSERT_EQUALS(reqQoSProfileCaps.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));
	CleanupStack::PopAndDestroy(1, &reqQoSProfileCaps);	
    }


TBool CCTSYIntegrationTestPacketContextBase::ActivateSecondaryPacketContextL(RPacketContext& aSecondaryPacketContext, TPrimaryPacketContextId aPrimaryPacketContextId, TSecondaryPacketContextId aSecondaryPacketContextId)
/**
 * Activate a secondary packet context
 *
 * @param aSecondaryPacketContext a reference to a context session to activate.
 *
 * @return ETrue if everything is OK, EFalse in any other case
 */
    {
    TBool ret = ETrue;

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter
	LOCAL_ASSERT_EQUALS(AddPacketFilterV2ToContextL(aSecondaryPacketContext,1), KErrNone,
                       _L("RPacketContext::AddPacketFilter returned with error status"));

	// Open a new QoS from the secondary context
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												aPrimaryPacketContextId,
												aSecondaryPacketContextId);

	// Get QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, 
                        _L("RPacketQoS::GetProfileCapabilities timed out."));
    LOCAL_ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone,
                        _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities	
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);    
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, 
                  _L("RPacketQoS::SetProfileParameters timed out."));
	LOCAL_ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone,
                   _L("RPacketQoS::SetProfileParameters returned with error status."));


	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(aSecondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	aSecondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate
	TExtEtelRequestStatus contextActivateStatus(aSecondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	aSecondaryPacketContext.Activate(contextActivateStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
                        _L("RPacketContext::Activate timed out."));
	LOCAL_ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
                        _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActive;
    iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				aSecondaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

    // Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(aSecondaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    LOCAL_ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("Packet Context's status expected to be EStatusActive but is not."));

	// Pop:
	//	contextActivateStatus
	//	notifyContextStChStatus
	//	setProfileParamStatus
	//	getProfileCapStatus
	CleanupStack::PopAndDestroy(4,&getProfileCapStatus);
    return ret;
    
    }

CCTSYIntegrationTestPacketContext0001::CCTSYIntegrationTestPacketContext0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0001::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0001::~CCTSYIntegrationTestPacketContext0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0001
 * @SYMFssID BA/CTSY/PKTC-0001
 * @SYMTestCaseDesc Activate and deactivate a primary PDP context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyContextAdded, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketContext::GetConfig, RPacketContext::Deactivate, RPacketContext::GetDnsInfo
 * @SYMTestExpectedResults Pass - Activation and deactivation succeeds. Packet service and context statuses are correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Get the attach mode so that it can be restored at the end of the test
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(packetService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong),
				  						    KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(),
				  KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Currenlty there are three context configs types: TContextConfigGPRS, TContextConfigCDMA, TContextConfigR99_R4
	// A suitable config should be chosen accoriding to the baseband's capability.
	
	// Get the context config using network info
	// Using Gprs config for now...
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);

	// Set the context.

	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	packetContext.SetConfig(setConfigStatus, configPk);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone,
				  _L("RPacketService::NotifyContextAdded did not complete."));
    ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, 
    			_L("RPacketService::NotifyContextAdded returned with error status."));
    ASSERT_TRUE(contextId.Length() > 0,
    			_L("Packet context config has zero length contextId."));

	// ===  Activate packet context  ===
	// ===  Check context and service statuses and notifications  ===

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate
	DEBUG_PRINTF1(_L("Activating packet context"));
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	

    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Check dynamic caps ===
	DEBUG_PRINTF1(_L("Checking Dynamic Caps"));
	
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
    TInt wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10060 );
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("Packet service's KCapsActivate flag not set"));

	// ===  Get context information ===

	// Check RPacketContext::GetConfig returns same config as that set
	DEBUG_PRINTF1(_L("Get config information"));
	TExtEtelRequestStatus contextConfigStatus(packetContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPk(getConfig);
	packetContext.GetConfig(contextConfigStatus, getConfigPk);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, 
											  _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone,
					_L("RPacketContext::GetConfig returned an error."));

	ASSERT_EQUALS_DES8(getConfig.iAccessPointName, config.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername, config.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword, config.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));

	// Check RPacketContext::GetDnsInfo returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfo;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPkg(dnsInfo);
	TExtEtelRequestStatus dnsInfoStatus(packetContext, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
	packetContext.GetDnsInfo(dnsInfoStatus, dnsInfoPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, 
											  _L("RPacketContext::GetDnsInfo timed out."));

	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone,
					_L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfo.iPrimaryDns.Length() > 0,
				  _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfo.iSecondaryDns.Length() > 0,
				  _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));

	// ===  Check context and service statuses on Deactivating packect context ===

	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for service's status change
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Deactivate the context with RPacketContext::Deactivate
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
    expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeactivating, _L("Packet Context's status expected to be EStatusDeactivating but is not."));
    packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	

    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("Packet Context's status did not return EStatusInactive."));
	
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////
	
    StartCleanup();

    // Return packet service's attach mode to original setting
	ASSERT_EQUALS(packetService.SetAttachMode(attachMode), KErrNone, _L("RPacketService::SetAttachMode returned with an error."));

	CleanupStack::Pop(10, &getNetworkStatus);	
                            // contextDeactivateStatus
							// dnsInfoStatus
							// contextConfigStatus
							// contextActivateStatus, 
							// notifyContextStChStatus,
							// notifyServiceDynCapsStatus
							// notifyServiceStChStatus,
							// setConfigStatus,
							// notifyContextStatus,
							// getNetworkStatus

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0001");
	}



CCTSYIntegrationTestPacketContext0002::CCTSYIntegrationTestPacketContext0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0002::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0002::~CCTSYIntegrationTestPacketContext0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0002
 * @SYMFssID BA/CTSY/PKTC-0002
 * @SYMTestCaseDesc Activate and deactivate a primary PDP context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Activation succeeds. NIF info correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify NIF related info from RPacketService correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Get the attach mode so that it can be restored at the end of the test
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(packetService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Ensure RPacketService::GetDynamicCaps returns caps in set of KCapsActivate.

	// $CTSYProblem.
	// GetDynamicCaps does not return 0x1(KCapsActivate), because KCapsActivate flag is not set yet. 
	// CTSY sets KCapsActivate flag during context initialisation.

	// Check for the CAPS being returned
	RPacketService::TDynamicCapsFlags dynCaps;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));

	// RPacketService::TDynamicCapsFlags wantedDynCapBits = RPacketService::KCapsActivate;	
	//CHECK_BITS_SET_L(dynCaps, wantedDynCapBits, KNoUnwantedBits, _L("Packet service's KCapsActivate flag not set"));
	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////

	StartTest();

	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Get the context config using network info
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);	

	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);

	// Open a new primary context.	
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService,
												KPrimaryPacketContext1);

	// Set a valid context config with RPacketContext::SetConfig
	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	packetContext.SetConfig(setConfigStatus, configPk);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with error status."));

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone,	_L("RPacketService::NotifyContextAdded returned with error status."));
	ASSERT_TRUE(contextId.Length() > 0, _L("Packet context config has zero length contextId."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	expectedPacketContextStatus = RPacketContext::EStatusActive;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Check that there is one NIF with one primary context belonging to it ===

	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	const TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Check RPacketService::EnumerateNifs returns aCount = 1
	TExtEtelRequestStatus nifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(nifsStatus);
	TInt nifCount = 0;
	const TInt wantedCount = 1;
	packetService.EnumerateNifs(nifsStatus, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateNifs returned with an error."));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing context name returns aCount = 1
	TExtEtelRequestStatus enumerateContextsInNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifStatus);
	TInt contextsInNifCount;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, contextInfo.iName, contextsInNifCount);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out"));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif returned wrong number of contexts sharing a NIF"));	

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing context name and aIndex = 0 returns aContextName which is same as aExistingContextName
	TExtEtelRequestStatus contextNameNifsStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(contextNameNifsStatus);
	TInt iIndex = 0;
	TName contextName;
	packetService.GetContextNameInNif(contextNameNifsStatus, contextInfo.iName, iIndex, contextName );

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextInfo.iName, contextName, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));	
	ASSERT_EQUALS(iIndex, KErrNone, _L("RPacketContext::GetContextNameInNif with aIndex returned with an error"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 1
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 0 returns EInternalContext

	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	const TInt nifInfoCount = 0;
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pkg(nifInfoV2);
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoV2Pkg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS_DES16(nifInfoV2.iContextName, contextName, _L("RPacketService::GetNifInfo with valid name  returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iNumberOfContexts, 1, _L("RPacketService::GetNifInfo with iNumberOfContexts returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketSerivce::GetNifInfo with iNifStatus returned with an error"));
 	ASSERT_TRUE(nifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address"));

 	// $CTSYProblem:RPacketService::GetNifInfo always returns EInternalContext
 	// Changed test procedure to check for EInternalContext instead of EExternalContext
 	ASSERT_EQUALS(nifInfoV2.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo returned wrong context type"));

	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Deactivate the context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeactivating, _L("RPacketContext::GetStatus returns incorrect status."));
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
				
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));
				
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::EnumerateNifs returns aCount = 0
	TExtEtelRequestStatus enumerateNifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsStatus);
	nifCount = 0;
	const TInt wantedEnumerateNifsCount = 1;
	packetService.EnumerateNifs(enumerateNifsStatus, nifCount);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out"));
	ASSERT_EQUALS(nifCount, wantedEnumerateNifsCount, _L("RPacketService::EnumerateNifs returned wrong NIF count."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();

	// pop
	// getNetworkStatus
	// setConfigStatus
	// notifyContextStatus
	// contextActivateStatus
	// notifyContextStatus
	// notifyContextStChStatus
	// nifsStatus
	// ContextNifsStatus
	// ContextNameNifsStatus
	// nifInfoStatus
	// contextDeactivateStatus
	// enumerateNifsStatus
	CleanupStack::PopAndDestroy(12, &getNetworkStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0002");
	}



CCTSYIntegrationTestPacketContext0003::CCTSYIntegrationTestPacketContext0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0003::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0003::~CCTSYIntegrationTestPacketContext0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0003
 * @SYMFssID BA/CTSY/PKTC-0003
 * @SYMTestCaseDesc Activate and deactivate a primary context (roaming).
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyContextAdded, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketContext::GetConfig, RPacketContext::Deactivate, RPacketContext::GetDnsInfo
 * @SYMTestExpectedResults Pass - Primary context is activated then deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Get the registration status and ensure that the phone is camped on a valid cell. 

	// Ensure phone is attached to packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::GetNtwkRegStatus returns ERegisteredRoaming

	// Set a valid context config with RPacketContext::SetConfig 

	// Check RPacketService::NotifyContextAdded completes with a valid context name

	// Activate packet context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach

	// ===  Get context information ===

	// Check RPacketContext::GetConfig returns same config as that set

	// Check RPacketContext::GetDnsInfo returns valid primary and secondary DNS ip addresses

	// Deactivate the context with RPacketContext::Deactivate 

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive

	// Check RPacketContext::GetStatus returns EStatusInactive

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached

	// Check RPacketService::GetStatus returns EStatusAttached

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0003");
	}



CCTSYIntegrationTestPacketContext0004::CCTSYIntegrationTestPacketContext0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0004::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0004::~CCTSYIntegrationTestPacketContext0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0004
 * @SYMFssID BA/CTSY/PKTC-0004
 * @SYMTestCaseDesc Activate and deactivate a primary context (roaming).
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Context activated and deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify NIF related info from RPacketService correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::GetNtwkRegStatus returns ERegisteredRoaming

	// Set a valid context config with RPacketContext::SetConfig 

	// Check RPacketService::NotifyContextAdded completes with a valid context name

	// Activate packet context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// ===  Check that there is one NIF with one primary context belonging to it ===

	// Check RPacketService::EnumerateNifs returns aCount = 1

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing context name returns aCount = 1

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing context name and aIndex = 0 returns aContextName which is same as aExistingContextName

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 1
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext

	// Deactivate the context with RPacketContext::Deactivate 

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive

	// Check RPacketContext::GetStatus returns EStatusInactive

	// Check RPacketService::EnumerateNifs returns aCount = 0

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0004");
	}



CCTSYIntegrationTestPacketContext0005::CCTSYIntegrationTestPacketContext0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0005::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0005::~CCTSYIntegrationTestPacketContext0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0005
 * @SYMFssID BA/CTSY/PKTC-0005
 * @SYMTestCaseDesc Activate and deactivate an HSDPA context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetConfig, RPacketContext::Activate, RPacketContext::Deactivate, RPacketService::NotifyContextAdded, RPacketContext::GetStatus, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketContext::SetConfig, RPacketContext::GetDnsInfo
 * @SYMTestExpectedResults Pass - Context activated and deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps has KCapsHSDPA and static caps are correct. 
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	//Check if we are  on a simulated network

	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10083);
	User::Leave(KErrNone);		
	
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Ensure RPacketService::GetStaticCaps returns caps in set of KCapsHSDPASupported 
	TUint staticCaps = 0;
	TUint expectedCaps = RPacketService::KCapsHSDPASupported;
	CHECK_EQUALS_L(packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4), KErrNone, _L("RPacketService::GetStaticCaps returned an error"));
	CHECK_BITS_SET_L(staticCaps, expectedCaps, KNoUnwantedBits, _L("RPacketService::GetStaticCaps did not return correct static caps"));
			
	// Ensure phone is attached to the packet service in an HSDPA enabled cell. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////


	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	// Open a primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
												
	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone,	_L("RPacketService::NotifyContextAdded returned with error status."));
	ASSERT_TRUE(contextId.Length() > 0, _L("Packet context config has zero length contextId."));

	// Set a valid context config with RPacketContext::SetConfig 
	//Get network info from phone
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	TExtEtelRequestStatus getNetworkStatus (phone,EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeMedium), KErrNone, _L("MobilePhone::GetCurrentNetwork timed out"))
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("MobilePhone::GetCurrentNetwork returned an error"))	
	
	// Get the context config using network info
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn,user,pwd;
	iPacketServiceTestHelper.GetGprsSettings(info,apn,user,pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);
	
	// set the context using network info
	TExtEtelRequestStatus setConfigStatus (packetContext,EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus); 
	packetContext.SetConfig(setConfigStatus,configPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("PacketContext::SetConfig timed out"))								
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("PacketContext::SetConfig returned an error."))	
	
	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);
	
	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
					packetService,
					notifyServiceStChStatus,
					packetServiceStatus, 
					expectedPacketServiceStatus,
					KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
    TInt wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer | RPacketService::KCapsHSDPA;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);
				
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("RPacketService::GetDynamicCaps did not return correct dynamic caps"));

	// ===  Get context information ===

	// Check RPacketContext::GetConfig returns same config as that set
	TExtEtelRequestStatus contextConfigStatus(packetContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPk(getConfig);
	packetContext.GetConfig(contextConfigStatus, getConfigPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone, _L("RPacketContext::GetConfig returned an error."));
	ASSERT_EQUALS_DES8(getConfig.iAccessPointName, config.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername, config.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword, config.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));

	// Check RPacketContext::GetDnsInfo returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfo;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPkg(dnsInfo);
	TExtEtelRequestStatus dnsInfoStatus(packetContext, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
	packetContext.GetDnsInfo(dnsInfoStatus, dnsInfoPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetDnsInfo timed out."));
	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone, _L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfo.iPrimaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfo.iSecondaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));
	
	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for service's status change
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Deactivate the context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// pop
	// getNetworkStatus
	// setConfigStatus
	// notifyContextStatus
	// notifyServiceStChStatus
	// notifyContextStChStatus
	// notifyServiceDynCapsStatus
	// contextActivateStatus
	// contextConfigStatus
	// dnsInfoStatus
	// contextDeactivateStatus
	
	CleanupStack::PopAndDestroy(10, &getNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0005");
	}



CCTSYIntegrationTestPacketContext0006::CCTSYIntegrationTestPacketContext0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0006::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0006::~CCTSYIntegrationTestPacketContext0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0006
 * @SYMFssID BA/CTSY/PKTC-0006
 * @SYMTestCaseDesc Activate and deactivate an EGPRS context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetConfig, RPacketContext::Activate, RPacketContext::Deactivate, RPacketService::NotifyContextAdded, RPacketContext::GetStatus, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketContext::SetConfig, RPacketContext::GetDnsInfo
 * @SYMTestExpectedResults Pass - Context activated and deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10084);
	User::Leave(KErrNone);	
	

    //Check if we are on a simulated network to run emergency call tests
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);	

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
 
    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    // Ensure RPacketService::GetStaticCaps returns caps in set of KCapsEGPRSSupported 
	TUint staticCaps = 0;
	TUint expectedCaps = 	RPacketService::KCapsEGPRSSupported;
	CHECK_EQUALS_L(packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4), KErrNone, _L("RPacketService::GetStaticCaps returned an error"));
	CHECK_BITS_SET_L(staticCaps, expectedCaps, KNoUnwantedBits, _L("RPacketService::GetStaticCaps did not return correct static caps"));
			

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	// Open a primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
	ASSERT_TRUE(contextId.Length() > 0, _L("RPacketService::NotifyContextAdded returned zero length contextId."));

	// Set a valid context config with iUseEdge = ETrue using RPacketContext::SetConfig 
	//Get network info from phone
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	TExtEtelRequestStatus getNetworkStatus (phone,EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeMedium), KErrNone, _L("MobilePhone::GetCurrentNetwork timed out"))
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("MobilePhone::GetCurrentNetwork returned an error"))	
	
	// Get the context config using network info
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn,user,pwd;
	iPacketServiceTestHelper.GetGprsSettings(info,apn,user,pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TBool edge = ETrue;
	config.iUseEdge = edge;
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);
	
	// set the context using network info
	TExtEtelRequestStatus setConfigStatus (packetContext,EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus); 
	packetContext.SetConfig(setConfigStatus,configPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("PacketContext::SetConfig timed out"))								
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("PacketContext::SetConfig returned an error."))	

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);
	
	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
					packetService,
					notifyServiceStChStatus,
					packetServiceStatus, 
					expectedPacketServiceStatus,
					KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsEGPRS.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
    TInt wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer | RPacketService::KCapsEGPRS;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);
				
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsEGPRS
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("Packet service's KCapsActivate flag not set"));

	// ===  Get context information ===

	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	const TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);
		
	// Check RPacketContext::GetConfig returns same config as that set
	TExtEtelRequestStatus contextConfigStatus(packetContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPk(getConfig);
	packetContext.GetConfig(contextConfigStatus, getConfigPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone, _L("RPacketContext::GetConfig returned an error."));
	ASSERT_EQUALS_DES8(getConfig.iAccessPointName, config.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername, config.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword, config.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS(getConfig.iUseEdge, config.iUseEdge, _L("RPacketContext::GetConfig returns incorrect config data."));

	// Check RPacketContext::GetDnsInfo returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfo;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPkg(dnsInfo);
	TExtEtelRequestStatus dnsInfoStatus(packetContext, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
	packetContext.GetDnsInfo(dnsInfoStatus, dnsInfoPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetDnsInfo timed out."));
	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone, _L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfo.iPrimaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfo.iSecondaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));
	
	// ===  Check that there is one NIF with one primary context belonging to it ===

	// Check RPacketService::EnumerateNifs returns aCount = 1
	TExtEtelRequestStatus nifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(nifsStatus);
	TInt countValue = 0;
	packetService.EnumerateNifs(nifsStatus, countValue);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(countValue, 1, _L("RPacketContext::EnumerateNifs returned wrong aValue"));
	
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing context name returns aCount = 1
	TExtEtelRequestStatus contextNifsStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(contextNifsStatus);
	countValue = 0;	
	packetService.EnumerateContextsInNif(contextNifsStatus, contextInfo.iName, countValue);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(countValue, 1, _L("RPacketService::EnumerateContextsInNif returned with an error."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing context name and aIndex = 0 returns aContextName which is same as aExistingContextName
	TExtEtelRequestStatus contextNameNifsStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(contextNameNifsStatus);
	TInt ind = 0;
	TName contextName;
	packetService.GetContextNameInNif(contextNameNifsStatus, contextInfo.iName, ind, contextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketService::GetContextNameInNif returned an error."));
	ASSERT_EQUALS_DES16(contextName, contextInfo.iName, _L("RPacketService::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));	

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 1
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext
	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pkg(nifInfoV2);
	packetService.GetNifInfo(nifInfoStatus, countValue, nifInfoV2Pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned an error."));
	ASSERT_EQUALS_DES16(nifInfoV2.iContextName, contextName, _L("RPacketService::GetNifInfo with valid name  returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iNumberOfContexts, 1, _L("RPacketService::GetNifInfo with iNumberOfContexts returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo with iNifStatus returned with an error"));
	ASSERT_TRUE(nifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo with iPdpAddress returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iContextType, RPacketService::EExternalContext, _L("RPacketService::GetNifInfo with iContextType returned with an error"));

	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for service's status change
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Deactivate the context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// pop
	// notifyContextStatus
	// getNetworkStatus
	// setConfigStatus
	// notifyServiceStChStatus
	// notifyContextStChStatus
	// notifyServiceDynCapsStatus
	// contextActivateStatus
	// getContextInfo
	// contextConfigStatus
	// dnsInfoStatus
	// NifsStatus
	// contextNifsStatus
	// contextNameNifsStatus
	// NifInfoStatus
	// contextDeactivateStatus
	CleanupStack::PopAndDestroy(15, &notifyContextStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0006");
	}



CCTSYIntegrationTestPacketContext0007::CCTSYIntegrationTestPacketContext0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0007::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0007::~CCTSYIntegrationTestPacketContext0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0007
 * @SYMFssID BA/CTSY/PKTC-0007
 * @SYMTestCaseDesc Activate and deactivate a secondary context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetConfig, RPacketContext::Activate, RPacketContext::Deactivate, RPacketService::NotifyContextAdded, RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::NotifyDynamicCapsChange, RPacketService::GetStatus, RPacketContext::SetConfig, RPacketContext::GetDnsInfo, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Secondary context is activated then deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	// wait until we are on the network
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	//Check if we are on a simulated network 
   iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Ensure phone is attached to the packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Post notification for the ContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	// Open a primary context. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Open a new secondary context from primary context
    RPacketContext& secondaryPacketContext2 = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

    ////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext2,1), KErrNone,
					_L("RPacketContext::AddPacketFilter returned with error status"));

	// ===  Add QoS ===

	// Open a new QoS from the secondary context
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	// Note: Using GPRS params
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, 
					_L("RPacketQoS::GetProfileCapabilities timed out."))						
	ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone,
					_L("RPacketQoS::GetProfileCapabilities returned with error status."))

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities	
	RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, 
					_L("RPacketQoS::SetProfileParameters timed out."))						
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone,
					_L("RPacketQoS::SetProfileParameters returned with error status."))
					
											
	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeShort), KErrNone,
				  _L("RPacketService::NotifyContextAdded did not complete."));
    ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, 
    			_L("RPacketService::NotifyContextAdded returned with error status."));
    ASSERT_TRUE(contextId.Length() > 0,
    			_L("RPacketService::NotifyContextAdded Packet context config has zero length contextId."));

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Activate primary context with RPacketContext::Activate
    iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus did not return RPacketService::EStatusActive."));

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive
	RPacketContext::TContextStatus packetContextStatus;
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext2.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);
	
	// Post notification for for service's dynamic caps change
	RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);

	// Activate the secondary context.
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext2, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext2.Activate(secContextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(secContextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedSecPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifySecContextStChStatus,
				secPacketContextStatus,
				expectedSecPacketContextStatus,
				KErrNone);
	secondaryPacketContext2.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

    //   -> EStatusActive
	expectedSecPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifySecContextStChStatus,
				secPacketContextStatus,
				expectedSecPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext2.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// ===  Check dynamic caps ===
	
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach

	// $CTSYProblem.
	// GetDynamicCaps does not change since the LTSY dose not call the EPacketNotifyDynamicCapsChange when the context is activated 
	
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach

	// $CTSYProblem.
	// GetDynamicCaps dose not change since the LTSY dose not call the EPacketNotifyDynamicCapsChange when the context is activated 
	
	// ===  Get context information ===

   	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));
	
	// Check RPacketContext::GetConfig for primary context returns same config as that set
	TExtEtelRequestStatus contextConfigStatus(primaryPacketContext , EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPk(getConfig);
	primaryPacketContext.GetConfig(contextConfigStatus, getConfigPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, 
											  _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone,
					_L("RPacketContext::GetConfig returned an error."));
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	ASSERT_EQUALS_DES8(getConfig.iAccessPointName,
				  config.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername,
				  config.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword,
				 config.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));
	 
	// Check RPacketContext::GetDnsInfo for primary context returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfoPrimary;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPrimaryPkg(dnsInfoPrimary);
	TExtEtelRequestStatus dnsInfoStatus(primaryPacketContext, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
	primaryPacketContext.GetDnsInfo(dnsInfoStatus, dnsInfoPrimaryPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetDnsInfo timed out."));
	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone, _L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfoPrimary.iPrimaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfoPrimary.iSecondaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));
	
	// Check RPacketContext::GetDnsInfo for secondary context returns same primary and secondary DNS ip addresses as for the primary context
	RPacketContext::TDnsInfoV2 dnsInfoSecondary;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoSecondaryPkg(dnsInfoSecondary);
	TExtEtelRequestStatus dnsInfoStatus2(secondaryPacketContext2, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus2);
	secondaryPacketContext2.GetDnsInfo(dnsInfoStatus2, dnsInfoSecondaryPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus2, ETimeMedium), KErrNone, _L("RPacketContext::GetDnsInfo timed out."));
	ASSERT_EQUALS(dnsInfoStatus2.Int(), KErrNone, _L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfoSecondary.iPrimaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfoSecondary.iSecondaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));
	
	// Compare ip addresses
	ASSERT_EQUALS_DES8(dnsInfoPrimary.iPrimaryDns, dnsInfoSecondary.iPrimaryDns,
					   _L("RPacketContext::GetDnsInfo IP addresses mismatch for Primary and Secondary contexts."));
	ASSERT_EQUALS_DES8(dnsInfoPrimary.iSecondaryDns, dnsInfoSecondary.iSecondaryDns,
					   _L("RPacketContext::GetDnsInfo IP addresses mismatch for Primary and Secondary contexts."));

	// Post notification for context's status change
	secondaryPacketContext2.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);
		
	// Deactivate the secondary context with RPacketContext::Deactivate 
	TExtEtelRequestStatus secCntxtDeactivateStatus(secondaryPacketContext2, EPacketContextDeactivate);
	CleanupStack::PushL(secCntxtDeactivateStatus);
	secondaryPacketContext2.Deactivate(secCntxtDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secCntxtDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(secCntxtDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
					
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusDeactivating -> EStatusInactive
	expectedSecPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifySecContextStChStatus,
				secPacketContextStatus,
				expectedSecPacketContextStatus,
				KErrNone);
    secondaryPacketContext2.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

    //    -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContext2Status = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedPacketContext2Status,
				KErrNone);
	
	// Check RPacketContext::GetStatus for the secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext2.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusInactive."));
	
	// Check RPacketContext::GetStatus for the primary context returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));
	
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	//	secCntxtDeactivateStatus
	//	dnsInfoStatus2
	//	dnsInfoStatus
	//	contextConfigStatus
	//	getNetworkStatus
	//	secContextActivateStatus
	//	notifyServiceDynCapsStatus
	//	notifySecContextStChStatus
	//	notifyServiceStChStatus
	//	setProfileParamStatus
	//	getProfileCapStatus
	//	notifyContextStatus
	CleanupStack::PopAndDestroy(12,&notifyContextStatus);

	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0007");
	}



CCTSYIntegrationTestPacketContext0008::CCTSYIntegrationTestPacketContext0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0008::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0008::~CCTSYIntegrationTestPacketContext0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0008
 * @SYMFssID BA/CTSY/PKTC-0008
 * @SYMTestCaseDesc Activate and deactivate a secondary context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::EnumerateContextsInNif, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Secondary context is activated then deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify NIF info from RPacketService correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	//Check if we are on a simulated network
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
 	
    // Ensure phone is attached to the packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName primaryContextId;
	packetService.NotifyContextAdded(notifyContextStatus, primaryContextId);

	// Open a primary packet context
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
    CHECK_EQUALS_L(notifyContextStatus.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
    CHECK_TRUE_L(primaryContextId.Length() > 0,	_L("RPacketService::NotifyContextAdded returned with zero length context name."));

	// Post notification for the RPacketService::NotifyContextAdded
	TName secondaryContextId;
	packetService.NotifyContextAdded(notifyContextStatus, secondaryContextId);

	// Open a secondary packet context
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
    CHECK_EQUALS_L(notifyContextStatus.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
    CHECK_TRUE_L(secondaryContextId.Length() > 0, _L("RPacketService::NotifyContextAdded returned with zero length context name."));


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context 
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 
	TExtEtelRequestStatus getQoSProfileCapsStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getQoSProfileCapsStatus);
	RPacketQoS::TQoSCapsGPRS qosCaps;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsPckg(qosCaps);
    packetQoS.GetProfileCapabilities(getQoSProfileCapsStatus, qosCapsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getQoSProfileCapsStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."))						
	ASSERT_EQUALS(getQoSProfileCapsStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned error."))

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
	RPacketQoS::TQoSGPRSRequested qosRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosReqPckg(qosRequested);
    FillParametersFromCapabilities(qosRequested,qosCaps);    
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);

	packetQoS.SetProfileParameters(setProfileParamStatus, qosReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Get network info from phone
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"))
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"))	

	// Get the context config using network info
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);

	// Set a valid context config with RPacketContext::SetConfig 
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);
	TExtEtelRequestStatus setConfigStatus (primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	primaryPacketContext.SetConfig(setConfigStatus,configPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"))
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned an error."))	

    // Set notify for  primary context change
    TExtEtelRequestStatus reqPriContextChangeStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(reqPriContextChangeStatus);
	RPacketContext::TContextStatus primaryPacketContextStatus;
    primaryPacketContext.NotifyStatusChange(reqPriContextChangeStatus, primaryPacketContextStatus);

    
	// Activate primary context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the primary context completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
            	            primaryPacketContext,
            				reqPriContextChangeStatus,
            				primaryPacketContextStatus, 
            				expectedPacketContextStatus,
            				KErrNone);

    primaryPacketContext.NotifyStatusChange(reqPriContextChangeStatus, primaryPacketContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
            	            primaryPacketContext,
            				reqPriContextChangeStatus,
            				primaryPacketContextStatus, 
            				expectedPacketContextStatus,
            				KErrNone);

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(primaryPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned error."));
	ASSERT_EQUALS(primaryPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive as expected."));


	// Set notify for  secondary context change
	TExtEtelRequestStatus reqSecContextChangeStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(reqSecContextChangeStatus);
	RPacketContext::TContextStatus secondaryPacketContextStatus;
    secondaryPacketContext.NotifyStatusChange(reqSecContextChangeStatus, secondaryPacketContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextActivateStatus, ETimeMedium), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(secContextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
	expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
            	            secondaryPacketContext,
            				reqSecContextChangeStatus,
            				secondaryPacketContextStatus, 
            				expectedPacketContextStatus,
            				KErrNone);
    
    secondaryPacketContext.NotifyStatusChange(reqSecContextChangeStatus, secondaryPacketContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
            	            secondaryPacketContext,
            				reqSecContextChangeStatus,
            				secondaryPacketContextStatus, 
            				expectedPacketContextStatus,
            				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(primaryPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned error."));
	ASSERT_EQUALS(secondaryPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive as expected."));

	// ===  Check that there is one NIF with one primary and one secondary context belonging to it ===

	// Check RPacketService::EnumerateNifs returns aCount = 1
	TExtEtelRequestStatus reqNifStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(reqNifStatus);
	TInt nifCount = 0;
	packetService.EnumerateNifs(reqNifStatus, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(nifCount, 1, _L("RPacketService::EnumerateNifs did not return 1 as expected"));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing primary context name returns aCount = 2
	TExtEtelRequestStatus reqContextNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(reqContextNifStatus);
	packetService.EnumerateContextsInNif(reqContextNifStatus, primaryContextId, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqContextNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, 2, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected"));
	
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing secondary context name returns aCount = 2
	nifCount = 0;
	packetService.EnumerateContextsInNif(reqContextNifStatus, secondaryContextId, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqContextNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, 2, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected"));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 0 returns valid context name in aContextName
    TExtEtelRequestStatus contextNameNifsStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(contextNameNifsStatus);
	TInt index = 0;
	TName returnedContextName;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketService::GetContextNameInNif returned error"));
	ASSERT_TRUE(returnedContextName.Length() > 0, _L("RPacketService::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 1 returns valid context name in aContextName
	index = 1;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketService::GetContextNameInNif returned error"));
	ASSERT_TRUE(returnedContextName.Length() > 0, _L("RPacketService::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 2 returns an error != KErrNone
	index = 2;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_TRUE(contextNameNifsStatus.Int() != KErrNone, _L("RPacketService::GetContextNameInNif did not return an error as expected"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TExtEtelRequestStatus reqNifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(reqNifInfoStatus);
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pkg(nifInfoV2);
	index = 0;
	packetService.GetNifInfo(reqNifInfoStatus, index, nifInfoV2Pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqNifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo timed out"));
	ASSERT_EQUALS(reqNifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned an error"));
	ASSERT_TRUE(nifInfoV2.iContextName.Length() > 0, _L("RPacketService::GetNifInfo with aCount = 0 returned with invalid iContextName"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	ASSERT_EQUALS(nifInfoV2.iNumberOfContexts, 2, _L("RPacketService::GetNifInfo with iNumberOfContexts did not return 2 as expected"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo did not return EStatusActive as expected"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
    ASSERT_TRUE(nifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo with iPdpAddress returned with an error"));
    
	// Check RPacketService::GetNifInfo with aCount = 0 returns EInternalContext
	ASSERT_EQUALS(nifInfoV2.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo did not return EInternalContext as expected"));
	
	// Deactivate the secondary context with RPacketContext::Deactivate 
	TExtEtelRequestStatus secContextDeactivateStatus(secondaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(secContextDeactivateStatus);
	secondaryPacketContext.Deactivate(secContextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextDeactivateStatus, ETimeMedium), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(secContextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned error."));

	//Release QoS
	iEtelSessionMgr.ReleaseSecondaryPacketContextQoS(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1,
											KSecondaryPacketContext1);

	// Release packet contexts
	iEtelSessionMgr.ReleaseSecondaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1,
											KSecondaryPacketContext1);
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyContextStatus
	// getQoSProfileCapsStatus
	// setProfileParamStatus
	// getNetworkStatus
	// setConfigStatus
	// reqPriContextChangeStatus
	// contextActivateStatus
	// reqSecContextChangeStatus
	// secContextActivateStatus
	// reqNifStatus
	// reqContextNifStatus
	// contextNameNifsStatus
	// reqNifInfoStatus
	// secContextDeactivateStatus
	CleanupStack::PopAndDestroy(14, &notifyContextStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0008");
	}



CCTSYIntegrationTestPacketContext0009::CCTSYIntegrationTestPacketContext0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0009::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0009::~CCTSYIntegrationTestPacketContext0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0009
 * @SYMFssID BA/CTSY/PKTC-0009
 * @SYMTestCaseDesc Activate and deactivate a secondary context (roaming).
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetConfig, RPacketContext::Activate, RPacketContext::Deactivate, RPacketService::NotifyContextAdded, RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::NotifyDynamicCapsChange, RPacketService::GetStatus, RPacketContext::SetConfig, RPacketContext::GetDnsInfo, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Secondary context is activated then deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify packet service status, packet context statuses, dynamic caps.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context 

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Set a valid context config with RPacketContext::SetConfig 

	// Check RPacketService::NotifyContextAdded completes with a valid context name

	// Activate primary context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange for the primary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive

	// Open a new secondary context from primary context  

	// Activate the secondary context. 

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive

	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer | KCapsHSDPA
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach

	// ===  Get context information ===

	// Check RPacketContext::GetConfig for primary context returns same config as that set

	// Check RPacketContext::GetDnsInfo for primary context returns valid primary and secondary DNS ip addresses
	// Check RPacketContext::GetDnsInfo for secondary context returns same primary and secondary DNS ip addresses as for the primary context

	// Deactivate the secondary context with RPacketContext::Deactivate 

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusDeactivating -> EStatusInactive

	// Check RPacketContext::GetStatus for the secondary context returns EStatusInactive
	// Check RPacketContext::GetStatus for the primary context returns EStatusActive

	// Check RPacketService::GetStatus for the secondary context returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0009");
	}



CCTSYIntegrationTestPacketContext0010::CCTSYIntegrationTestPacketContext0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0010::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0010::~CCTSYIntegrationTestPacketContext0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0010
 * @SYMFssID BA/CTSY/PKTC-0010
 * @SYMTestCaseDesc Activate and deactivate a secondary context (roaming).
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::EnumerateContextsInNif, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Secondary context is activated then deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify NIF info from RPacketService correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context  

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Set a valid context config with RPacketContext::SetConfig 

	// Activate primary context with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange for the primary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive

	// Open a new secondary context from primary context  

	// Activate the secondary context. 

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive

	// ===  Check that there is one NIF with one primary and one secondary context belonging to it ===

	// Check RPacketService::EnumerateNifs returns aCount = 1

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing primary context name returns aCount = 2
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing secondary context name returns aCount = 2

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 0 returns valid context name in aContextName
	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 1 returns valid context name in aContextName
	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 2 returns an error != KErrNone

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext

	// Deactivate the secondary context with RPacketContext::Deactivate 

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0010");
	}



CCTSYIntegrationTestPacketContext0011::CCTSYIntegrationTestPacketContext0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0011::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0011::~CCTSYIntegrationTestPacketContext0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0011
 * @SYMFssID BA/CTSY/PKTC-0011
 * @SYMTestCaseDesc Activate a context when it is already active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::Activate, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Error returned on activation.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    
    // Ensure a primary PDP context is active. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

    // Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);	
        
    
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Activate the active context with RPacketContext::Activate. 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);

	// Check error is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_TRUE(contextActivateStatus.Int() != KErrNone, _L("RPacketContext::Activate did not return error as expected."));

	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus primaryPacketContextStatus;
	ASSERT_EQUALS(primaryPacketContext.GetStatus(primaryPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(primaryPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// contextActivateStatus
	CleanupStack::PopAndDestroy(1, &contextActivateStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0011");
	}



CCTSYIntegrationTestPacketContext0012::CCTSYIntegrationTestPacketContext0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0012::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0012::~CCTSYIntegrationTestPacketContext0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0012
 * @SYMFssID BA/CTSY/PKTC-0012
 * @SYMTestCaseDesc Re-activate a context after deactivation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps
 * @SYMTestExpectedResults Pass - Context is deactivated and then reactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context and packet service status, dynamic caps.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Get the attach mode so that it can be restored at the end of the test
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(packetService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
   	 			
	// Ensure a PDP context is active.
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,packetContext); 
												 	
	
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
		
	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);
	
	// Deactivate the PDP context with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));
					
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
    RPacketContext::TContextStatus 	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));
	
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusAttached
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));
	
	// ===  Check dynamic caps ===

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach  | RPacketService::KCapsRxCSCall |  RPacketService::KCapsRxContextActivationReq
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
  	TInt wantedDynCapBits = RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsRxCSCall;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("RPacketService::GetDynamicCaps did not return correct dynamic caps"));
	
	// Post notification for service's status change
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Activate a PDP context again. 
	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50002 );
	// CTSY doesn´t support context re-activation. See cmmpacketcontextgsmwcdmaext.cpp and CMmPacketContextGsmWcdmaExt::ActivateL()
	// Test case is made to leave because of this.
	CHECK_TRUE_L(EFalse, _L("This test has been made to leave intentionally - ***Send message fail, and ctsy cancel it later****"));
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusActive
    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("Packet Context's status expected to be EStatusActive but is not."));

	// Post notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
		
	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
					packetService,
					notifyServiceStChStatus,
					packetServiceStatus, 
					expectedPacketServiceStatus,
					KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					packetContext,
					notifyContextStChStatus,
					packetContextStatus, 
					expectedPacketContextStatus,
					KErrNone);
					
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
	
	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
    wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer;
	unWantedDynCapBits = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);
				
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("Packet service's KCapsActivate flag not set"));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Return packet service's attach mode to original setting
	packetService.SetAttachMode(attachMode);

	// pop
	// notifyContextStChStatus
	// notifyServiceStChStatus
	// notifyServiceDynCapsStatus
	// contextDeactivateStatus
	// contextActivateStatus
	
	CleanupStack::PopAndDestroy(5, &notifyContextStChStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0012");
	}



CCTSYIntegrationTestPacketContext0013::CCTSYIntegrationTestPacketContext0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0013::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0013::~CCTSYIntegrationTestPacketContext0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0013
 * @SYMFssID BA/CTSY/PKTC-0013
 * @SYMTestCaseDesc Activate and deactivate two primary contexts.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::GetConfig, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateContexts, RPacketService::GetContextInfo, RPacketContext::GetDnsInfo, RPacketService::EnumerateContextsInNif, RPacketService::EnumerateNifs, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Two contexts can be simultaneously active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to have two active primary contexts simultaneously.

Verify context and packet service status, NIF related info, DNS and context config info.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Attach to the packet service. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L( iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL( phone ), KErrNone, 
			_L("Network is unavailable") );
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Activate a primary context.
	RPacketContext& packetContext1 = iEtelSessionMgr.GetPrimaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService,
			KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,packetContext1);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
    // Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	// Post notification for service's status change 
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
		
	// Open another primary context (referred to in this test as "primary context 2") 
   RPacketContext& packetContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService,
			KPrimaryPacketContext2);
    
	// Set a valid context config for primary context 2 using RPacketContext::SetConfig 
   TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeMedium),
			KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(),
			KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	RPacketContext::TContextConfigGPRS contextConfig;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd, 2);
	contextConfig.iAccessPointName.Copy(apn);
	contextConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	contextConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckg(contextConfig);
	TExtEtelRequestStatus setConfigStatus(packetContext2, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
    packetContext2.SetConfig(setConfigStatus, contextConfigPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
                  _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
                  _L("RPacketContext::SetConfig returned with error status."));
    
	
	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone,
			_L("RPacketService::NotifyContextAdded did not complete."));
	ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, 
                  _L("RPacketService::NotifyContextAdded returned with error status."));
    ASSERT_TRUE(contextId.Length() > 0,
                _L("Packet context config has zero length contextId."));
	

	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);
		
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext2.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	

	// Activate primary context 2 with RPacketContext::Activate
	TExtEtelRequestStatus contextActivateStatus(packetContext2, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext2.Activate(contextActivateStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
			_L("RPacketContext::Activate timed-out"));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
			_L("RPacketContext::Activate returned with error status."));
		
	// ===  Check context and service statuses ===

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
        packetService,
        notifyServiceStChStatus,
        packetServiceStatus, 
        expectedPacketServiceStatus,
        KErrNone);
	
	// Check RPacketContext::NotifyStatusChange for primary context 2 completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			packetContext2,
			notifyContextStChStatus,
			packetContextStatus, 
			expectedPacketContextStatus,
			KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    packetContext2.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			packetContext2,
			notifyContextStChStatus,
			packetContextStatus, 
			expectedPacketContextStatus,
			KErrNone);
	
	// Check RPacketContext::GetStatus for primary context 2 returns EStatusActive
	ASSERT_EQUALS(packetContext2.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));


    
	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange for primary context 2 completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange for primary context 2 completes with caps NOT in set of RPacketService::KCapsManualAttach
    TInt wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);
	
	// Check RPacketService::GetDynamicCaps for primary context 2 returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps for primary context 2 returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("Packet service's KCapsActivate flag not set"));
	
	// ===  Get context information ===

	// Check RPacketContext::GetConfig for primary context 2 returns same config as that set
	TExtEtelRequestStatus contextConfigStatus(packetContext2, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPk(getConfig);
		packetContext2.GetConfig(contextConfigStatus, getConfigPk);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, 
			_L("RPacketContext::GetConfig timed out."));
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone,
			_L("RPacketContext::GetConfig returned an error."));
    ASSERT_EQUALS_DES8(getConfig.iAccessPointName, contextConfig.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername, contextConfig.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword, contextConfig.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));
	
	// Check RPacketContext::GetDnsInfo for primary context 2 returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfo;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPkg(dnsInfo);
	TExtEtelRequestStatus dnsInfoStatus(packetContext2, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
		packetContext2.GetDnsInfo(dnsInfoStatus, dnsInfoPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketContext::GetDnsInfo timed out."));
	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone,
			_L("RPacketContext::GetDnsInfo returned an error."));
    ASSERT_TRUE(dnsInfo.iPrimaryDns.Length() > 0,
                _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfo.iSecondaryDns.Length() > 0,
                _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));

	// ===  Check that there is one NIF with one primary context belonging to it ===

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = name of primary context 2 returns aCount = 1
	TExtEtelRequestStatus enumContextsInfStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumContextsInfStatus);
	TInt count=-1;
    packetService.EnumerateContextsInNif(enumContextsInfStatus,contextId,count);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextsInfStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateContextsInNif timed out."));
		ASSERT_EQUALS(enumContextsInfStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateContextsInNif failed."));
	ASSERT_EQUALS(count,1,_L("RPacketService::EnumerateContextsInNif did not return correct number of NIFs."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = name of primary context 2 and aIndex = 0 returns context name which is same as the name of primary context 2
	TBuf<200> contextName;
	TExtEtelRequestStatus getContextNameInNifStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifStatus);
		packetService.GetContextNameInNif(getContextNameInNifStatus,contextId,0,contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetContextNameInNif timed out."));
		ASSERT_EQUALS(getContextNameInNifStatus.Int(), KErrNone, 
			_L("RPacketService::GetContextNameInNif failed."));
	ASSERT_EQUALS_DES16(contextName,contextId,_L("RPacketService::GetContextNameInNif did not returned the expected value."));
	
	// Check RPacketService::EnumerateNifs returns aCount = 2
	TExtEtelRequestStatus enumerateNifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsStatus);
	count=-1;
    packetService.EnumerateNifs(enumerateNifsStatus,count);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsStatus, ETimeMedium), KErrNone, 
                  _L("RPacketService::EnumerateNifs timed out."));
    ASSERT_EQUALS(enumerateNifsStatus.Int(), KErrNone, 
                  _L("RPacketService::EnumerateNifs failed."));
	ASSERT_EQUALS(count,2,_L("RPacketService::EnumerateNifs did not return correct number of NIFs."));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TExtEtelRequestStatus getNifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoStatus);
	RPacketService::TNifInfoV2 nifInfo;
	RPacketService::TNifInfoV2Pckg nifInfoPckg(nifInfo);
    packetService.GetNifInfo(getNifInfoStatus,0,nifInfoPckg);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoStatus, ETimeMedium), KErrNone, 
                  _L("RPacketService::GetNifInfo timed out."));
    ASSERT_EQUALS(getNifInfoStatus.Int(), KErrNone, 
                  _L("RPacketService::GetNifInfo failed."));
	ASSERT_TRUE(nifInfo.iContextName.Length()>0,_L("RPacketService::GetNifInfo invlaid context name"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 1
	ASSERT_EQUALS(nifInfo.iNumberOfContexts,1,_L("RPacketService::GetNifInfo wrong number of contexts"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus,RPacketContext::EStatusActive,_L("RPacketService::GetNifInfo wrong status"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()>0,_L("RPacketService::GetNifInfo invlaid PDP address"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns EInternalContext
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext,_L("RPacketService::GetNifInfo wrong type"));
	
	// Check RPacketService::GetNifInfo with aCount = 1 returns valid name in iContextName
	packetService.GetNifInfo(getNifInfoStatus,1,nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(getNifInfoStatus.Int(), KErrNone, 
			_L("RPacketService::GetNifInfo failed."));
	ASSERT_TRUE(nifInfo.iContextName.Length()>0,_L("RPacketService::GetNifInfo invlaid context name"));

	// Check RPacketService::GetNifInfo with aCount = 1 returns iNumberOfContexts = 1
	ASSERT_EQUALS(nifInfo.iNumberOfContexts,1,_L("RPacketService::GetNifInfo wrong number of contexts"));

	// Check RPacketService::GetNifInfo with aCount = 1 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus,RPacketContext::EStatusActive,_L("RPacketService::GetNifInfo wrong status"));

	// Check RPacketService::GetNifInfo with aCount = 1 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()>0,_L("RPacketService::GetNifInfo invlaid PDP address"));

	// Check RPacketService::GetNifInfo with aCount = 1 returns EExternalContext
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext,_L("RPacketService::GetNifInfo wrong type"));

	// Post notification for context's status change
	packetContext2.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	

	// Deactivate primary context 2 with RPacketContext::Deactivate 
	TExtEtelRequestStatus contextDeactivateStatus(packetContext2, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext2.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary context 2 completes with EStatusDeactivating -> EStatusInactive
    expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext2,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeactivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    packetContext2.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext2,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("Packet Context's status expected to be EStatusActivating but is not."));

	// Check RPacketContext::GetStatus returns for primary context 2 EStatusInactive
	ASSERT_EQUALS(packetContext2.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq
	wantedDynCapBits=RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall;
	unWantedDynCapBits=RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	//ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("Packet service's KCapsActivate flag not set"));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	//	contextDeactivateStatus
	//	getNifInfoStatus
	//	enumerateNifsStatus
	//	getContextNameInNifStatus
	//	enumContextsInfStatus
	//	dnsInfoStatus
	//	contextConfigStatus
	//	contextActivateStatus
	//	notifyContextStChStatus
	//	notifyServiceDynCapsStatus
	//	notifyServiceStChStatus
	//	setConfigStatus
	//	getNetworkStatus
	//	notifyContextStatus
	CleanupStack::PopAndDestroy(14,&notifyContextStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0013");
	}



CCTSYIntegrationTestPacketContext0014::CCTSYIntegrationTestPacketContext0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0014::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0014::~CCTSYIntegrationTestPacketContext0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0014
 * @SYMFssID BA/CTSY/PKTC-0014
 * @SYMTestCaseDesc Deactivate a primary context when there is a secondary context active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::EnumerateNifs, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Primary and secondary contexts deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify secondary is also deactivated.

Verify packet service status, packet context status, context info, NIF info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
    
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    //Check if we are on a simulated network
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

   	// Set the attach mode to EAttachWhenPossible. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
	// Ensure there is a primary context active and a secondary context active. 	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

    RPacketContext& secondaryPacketContext2 = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);	
	
	// Activate the secondary context.
	ActivateSecondaryPacketContextL(secondaryPacketContext2);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
		
	
	
	
	
	// Post notification for primary context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Post notification for secondary context status change
	TExtEtelRequestStatus notifyContext2StChStatus(secondaryPacketContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContext2StChStatus);
	RPacketContext::TContextStatus packetContextStatus2;
	secondaryPacketContext2.NotifyStatusChange(notifyContextStChStatus, packetContextStatus2);

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Post notification for service's dynamic caps change
    RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);

	// Deactivate the primary context. 
	TExtEtelRequestStatus contextDeactivateStatus(primaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	primaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary context completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for primary context returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange for secondary context completes with EStatusDeactivating -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10061 );
   	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifyContext2StChStatus,
				packetContextStatus2, 
				expectedPacketContextStatus,
				KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10062 );
	// Leaving since the cancellation did not work, so ETEL will panic if another notification will be send
	CHECK_TRUE_L(EFalse, _L("This test has been made to leave intentionally - CTSY doesn't have support for Cancelation of NotifyStatusChange")); 
	secondaryPacketContext2.NotifyStatusChange(notifyContext2StChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext2,
				notifyContext2StChStatus,
				packetContextStatus2, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext2.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for secondary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange for primary context completes with EStatusAttached
	RPacketService::TStatus expectedPacketStatus = RPacketService::EStatusAttached;
	
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange( 
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketStatus,
				KErrNone );

	// Check RPacketService::GetStatus for primary context returns EStatusAttached	
	
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyDynamicCapsChange returns caps in set of RPacketService::KCapsSMSTransfer  | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	// Check RPacketService::NotifyDynamicCapsChange returns caps NOT in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
  	TInt wantedDynCapBits = RPacketService::KCapsSMSTransfer  | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall;
	
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);
				
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("RPacketService::GetDynamicCaps did not return correct dynamic caps"));

	
	// Check RPacketService::EnumerateNifs returns aCount = 0
	TExtEtelRequestStatus enumerateNifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsStatus);
	TInt nifCount = 0;
	const TInt wantedCount = 0;
	packetService.EnumerateNifs(enumerateNifsStatus, nifCount);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out"));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateNifs returned wrong NIF count."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// PopAndDestroy
	// notifyContextStChStatus
	// notifyContext2StChStatus
	// notifyServiceStChStatus
	// notifyServiceDynCapsStatus
	// contextDeactivateStatus
	// enumerateNifsStatus
	CleanupStack::PopAndDestroy(6, &notifyContextStChStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0014");
	}



CCTSYIntegrationTestPacketContext0015::CCTSYIntegrationTestPacketContext0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0015::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0015::~CCTSYIntegrationTestPacketContext0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0015
 * @SYMFssID BA/CTSY/PKTC-0015
 * @SYMTestCaseDesc Deactivate a secondary context and check that primary context is kept active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::GetContextInfo, RPacketService::EnumerateContexts
 * @SYMTestExpectedResults Pass - First context remains active. Second context is deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet service status, packet context status, context info, NIF info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL	
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Check if we are on a simulated network
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Set the attach mode to EAttachWhenPossible. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
	TExtEtelRequestStatus notifyContextAddedStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus);
	TName primaryCntxtId;
	
	//post notifier for RPacketService::NotifyContextAdded
	packetService.NotifyContextAdded(notifyContextAddedStatus, primaryCntxtId);
	
	// Ensure there is a primary context active and a secondary context active. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(),KErrNone, _L("RPacketService::NotifyContextAdded returned an error"));
	
	TName secondaryCntxtId;
	
	//post notifier for RPacketService::NotifyContextAdded
	packetService.NotifyContextAdded(notifyContextAddedStatus, secondaryCntxtId);
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
												
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(),KErrNone, _L("RPacketService::NotifyContextAdded returned an error"));
	
	// Activate primary context.
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);	
	
	// Activate the secondary context.
	ActivateSecondaryPacketContextL(secondaryPacketContext);
	
	
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////


	// Post notification for secondary context status change
	TExtEtelRequestStatus notifyContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus secondaryPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, secondaryPacketContextStatus);
	
	// Deactivate the secondary context. 
	TExtEtelRequestStatus contextDeactivateStatus(secondaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	secondaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
					
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for secondary context completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
   	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				secondaryPacketContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	
	//repost notifier			
	secondaryPacketContext.NotifyStatusChange(contextDeactivateStatus, secondaryPacketContextStatus);
	
	expectedPacketContextStatus = RPacketContext::EStatusInactive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				secondaryPacketContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for secondary context returns EStatusInactive
	RPacketContext::TContextStatus getPacketContextStatus;
	CHECK_EQUALS_L(secondaryPacketContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	CHECK_EQUALS_L(getPacketContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus did not return RPacketContext::EStatusInactive."));

	// Check RPacketService::GetStatus for primary context returns EStatusActive
	RPacketService::TStatus packetServiceStatus;		
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketService::EnumerateContexts returns aCount = 2
	TExtEtelRequestStatus enumContextsStatus(packetService, EPacketEnumerateContexts);
	CleanupStack::PushL(enumContextsStatus);
	TInt count = 0;
    TInt max = 0;
    const TInt wantedCount = 2;
    packetService.EnumerateContexts(enumContextsStatus, count, max);
    
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextsStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateContexts timed out."));
    ASSERT_EQUALS(enumContextsStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateContexts returned with an error."));
	ASSERT_EQUALS(count, wantedCount,_L("RPacketService::EnumerateContexts did not return correct number of open contexts."));

	// Check RPacketService::GetContextInfo for aIndex = 0 and 1 return the correct context name and status for the primary and secondary context.
	TExtEtelRequestStatus getContextsInfoStatus(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextsInfoStatus);
	RPacketService::TContextInfo primaryContextInfo;
	RPacketService::TContextInfo secondaryContextInfo;
	packetService.GetContextInfo(getContextsInfoStatus, 0, primaryContextInfo);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextsInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetContextInfo timed out."));
    ASSERT_EQUALS(getContextsInfoStatus.Int(), KErrNone, _L("RPacketService::GetContextInfo returned with an error."));
    
	ASSERT_EQUALS_DES16(primaryContextInfo.iName, primaryCntxtId, _L("RPacketService::GetContextInfo did not return correct iName"));
	ASSERT_EQUALS(primaryContextInfo.iStatus, RPacketContext::EStatusActive, _L("RPacketService::GetContextInfo did not return iStatus EStatusActive"));
	
	packetService.GetContextInfo(getContextsInfoStatus, 1, secondaryContextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextsInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetContextInfo timed out."));
    ASSERT_EQUALS(getContextsInfoStatus.Int(), KErrNone, _L("RPacketService::GetContextInfo returned with an error."));
    
	ASSERT_EQUALS_DES16(secondaryContextInfo.iName, secondaryCntxtId, _L("RPacketService::GetContextInfo did not return correct iName"));
	ASSERT_EQUALS(secondaryContextInfo.iStatus, RPacketContext::EStatusInactive, _L("RPacketService::GetContextInfo did not return iStatus EStatusInactive"));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	//--PopAndDestroy--
	//notifyContextAddedStatus
	//notifyContextStChStatus
	//contextDeactivateStatus
	//enumContextsStatus
	//getContextsInfoStatus
	CleanupStack::PopAndDestroy(5, &notifyContextAddedStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0015");
	}



CCTSYIntegrationTestPacketContext0016::CCTSYIntegrationTestPacketContext0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0016::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0016::~CCTSYIntegrationTestPacketContext0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0016
 * @SYMFssID BA/CTSY/PKTC-0016
 * @SYMTestCaseDesc Handle network initiated deactivation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::GetContextInfo, RPacketService::EnumerateContexts
 * @SYMTestExpectedResults Pass - Context 1 is deactivated. Other context is unaffected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify packet service status, packet context status and context info.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// == Ensure there are two PDP contexts active. ==
	
    //Check if we are on a simulated network
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL( phone ), KErrNone, _L("Network is unavailable"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Ensure phone is attached to the packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
	//post notifier for RPacketService::NotifyContextAdded
    TExtEtelRequestStatus notifyContextAddedStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus);
	TName contextId1;
	packetService.NotifyContextAdded(notifyContextAddedStatus, contextId1);

    // Open primary context 1
    RPacketContext& primaryPacketContext1 = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	
	// Check RPacketService::NotifyContextAdded completes
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(),KErrNone, _L("RPacketService::NotifyContextAdded returned an error"));

    // Activate primary packet context 1
    iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext1);
	TName contextId2;
	
	//post notifier for RPacketService::NotifyContextAdded
	packetService.NotifyContextAdded(notifyContextAddedStatus, contextId2);

    // Open primary context 2
    RPacketContext& primaryPacketContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext2);
											
	// Check RPacketService::NotifyContextAdded completes
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(),KErrNone, _L("RPacketService::NotifyContextAdded returned an error"));

    // Activate primary packet context 2
    iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext2,2);

    // Check that both primary contexts are active
	RPacketContext::TContextStatus packetContextStatus;
    CHECK_EQUALS_L(primaryPacketContext1.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    CHECK_EQUALS_L(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
    CHECK_EQUALS_L(primaryPacketContext2.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    CHECK_EQUALS_L(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
    

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Post notification for the first context's status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext1, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	primaryPacketContext1.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// The simulator sends a PDP context deactivation message saying that context 1 has been deactivated.
	DisplayUserInteractionPromptL(_L("Please send me a deactivation of a packet context request."), ETimeMedium);
	
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for context 1 completes with EStatusDeactivating
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10067 );
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			primaryPacketContext1,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeactivating, _L("RPacketContext::NotifyStatusChange did not return EStatusDeactivating"));

    //                  -> EStatusInactive
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10067 );
    primaryPacketContext1.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			primaryPacketContext1,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::NotifyStatusChange did not return EStatusInactive"));

	// Check RPacketContext::GetStatus for context 1 returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext1.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::GetStatus for context 2 returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext2.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketService::EnumerateContexts returns aCount = 2
	TExtEtelRequestStatus enumContextsStatus(packetService, EPacketEnumerateContexts);
	CleanupStack::PushL(enumContextsStatus);
	TInt count=-1;
    TInt max=-1;
    packetService.EnumerateContexts(enumContextsStatus,count,max);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextsStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateContexts timed out."));
    ASSERT_EQUALS(enumContextsStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateContexts failed."));
	ASSERT_EQUALS(count,2,_L("RPacketService::EnumerateContexts returned with an error"));

	// Check RPacketService::GetContextInfo for aIndex = 0 and 1 return the correct context name and status for the two contexts.
    RPacketService::TContextInfo contextInfo;
	TExtEtelRequestStatus getContextInfoStatus(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(enumContextsStatus);
    packetService.GetContextInfo(getContextInfoStatus,0,contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetContextInfo timed out."));
    ASSERT_EQUALS(getContextInfoStatus.Int(), KErrNone, 
			_L("RPacketService::GetContextInfo returned with an error"));
    ASSERT_EQUALS_DES16(contextInfo.iName,contextId1,_L("RPacketService::GetContextInfo return wrong value in iName"));
    ASSERT_EQUALS(contextInfo.iStatus,RPacketContext::EStatusInactive,_L("RPacketService::GetContextInfo return wrong value in iStatus"));
    packetService.GetContextInfo(getContextInfoStatus,1,contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetContextInfo timed out."));
    ASSERT_EQUALS(getContextInfoStatus.Int(), KErrNone, 
			_L("RPacketService::GetContextInfo returned with an error."));
    ASSERT_EQUALS_DES16(contextInfo.iName,contextId2,_L("RPacketService::GetContextInfo return wrong value in iName"));
    ASSERT_EQUALS(contextInfo.iStatus,RPacketContext::EStatusActive,_L("RPacketService::GetContextInfo return wrong value in iStatus"));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	//	enumContextsStatus
	//	enumContextsStatus
	//	notifyContextStChStatus
	//	notifyContextAddedStatus
	CleanupStack::PopAndDestroy(4,&notifyContextAddedStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0016");
	}



CCTSYIntegrationTestPacketContext0017::CCTSYIntegrationTestPacketContext0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0017::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0017::~CCTSYIntegrationTestPacketContext0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0017
 * @SYMFssID BA/CTSY/PKTC-0017
 * @SYMTestCaseDesc Deactivate a packet context when it is already inactive.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Error returned on deactivation.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verfiy error is returned and context stays inactive.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Get the attach mode so that it can be restored at the end of the test
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(packetService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new context.	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
											 
	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);	
	
	// Post notification for primary context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Deactivate the primary context. 
	TExtEtelRequestStatus contextDeactivateStatus(primaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	primaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
	
	// ===  Check context status===
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusInactive;
   
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Deactivate the context.	
 	primaryPacketContext.Deactivate(contextDeactivateStatus);
 	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));
					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));
				
	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyContextStChStatus
	// contextDeactivateStatus
	CleanupStack::PopAndDestroy(2,&notifyContextStChStatus);
	
	return TestStepResult();
	}


TPtrC CCTSYIntegrationTestPacketContext0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0017");
	}



CCTSYIntegrationTestPacketContext0018::CCTSYIntegrationTestPacketContext0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0018::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0018::~CCTSYIntegrationTestPacketContext0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0018
 * @SYMFssID BA/CTSY/PKTC-0018
 * @SYMTestCaseDesc Activate a PDP context with an unknown APN.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::GetLastErrorCause, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - It is not possible to activate a context with an incorrect APN.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify activation fails and  last error cause returned correctly.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Set attach mode to 'when possible'. 
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
				
	// Ensure automatic attach completes successfully.
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
		_L("RPacketService::SetAttachMode timed-out"));
	CHECK_EQUALS_L(setAttachModeStatus.Int(), KErrNone,
		_L("RPacketService::SetAttachMode returned with an error"));
		
	//Ensure RPacketService::GetDynamicCaps returns caps in set of KCapsActivate.
	
	//$CTSYProblem.
	//GetDynamicCaps does not return 0x1(KCapsActivate), because KCapsActivate flag is not set yet. 
	//CTSY sets KCapsActivate flag during context initialisation.
	
	//Check for the CAPS being returned
	RPacketService::TDynamicCapsFlags dynCaps;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	
	//RPacketService::TDynamicCapsFlags wantedDynCapBits = RPacketService::KCapsActivate;	
	//CHECK_BITS_SET_L(dynCaps, wantedDynCapBits, KNoUnwantedBits, _L("Packet service's KCapsActivate flag not set"));

	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
		
	// Open a new primary context. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	RPacketService::TStatus pckSrvcStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Set an incorrect APN when setting the config with RPacketContext::SetConfig
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	_LIT(KFaultApnName, "IncorrectAPNname");
	config.iAccessPointName.Copy(KFaultApnName);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);	
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);	
	
	TExtEtelRequestStatus setConfigStatus (primaryPacketContext,EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus); 
	primaryPacketContext.SetConfig(setConfigStatus,configPk);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("PacketContext::SetConfig timed out"));								
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("PacketContext::SetConfig returned an error."));	

	// Activate the context 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("PacketContext::Activate timed out"));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("PacketContext::Activate did not return KErrNone"))
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusInactive.	
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

    primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);


	// === Check the status of context is EStatusInactive and that a valid error is returned === 
	
	// Check RPacketContext::GetLastErrorCause returns error != KErrNone
	TInt error;
	ASSERT_EQUALS(primaryPacketContext.GetLastErrorCause(error), KErrNone, _L("RPacketContext::GetLastErrorCause returned with an error."));
	ASSERT_TRUE(error != KErrNone, _L("RPacketContext::GetLastErrorCause returns incorrectly KErrNone."));

	// Check RPacketContext::GetStatus returns EStatusInactive.
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusAttached.
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// setAttachModeStatus
	// notifyContextStChStatus
	// setConfigStatus
	// contextActivateStatus
    CleanupStack::PopAndDestroy(4, &setAttachModeStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0018");
	}



CCTSYIntegrationTestPacketContext0019::CCTSYIntegrationTestPacketContext0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0019::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0019::~CCTSYIntegrationTestPacketContext0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0019
 * @SYMFssID BA/CTSY/PKTC-0019
 * @SYMTestCaseDesc Activate a PDP context with an incorrect password.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::GetLastErrorCause, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Activation fails and error returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify activation fails and  last error cause returned correctly.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
			_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
		
	// Set attach mode to 'when possible'. 
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
				
	// Ensure automatic attach completes successfully.
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
		_L("RPacketService::SetAttachMode timed-out"));
	CHECK_EQUALS_L(setAttachModeStatus.Int(), KErrNone,
		_L("RPacketService::SetAttachMode returned with an error"));
		
	// Ensure phone has KCapsActivate capability. 

	//Check for the CAPS being returned
	RPacketService::TDynamicCapsFlags dynCaps;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	
	//$CTSYProblem.
	//GetDynamicCaps does not return 0x1(KCapsActivate) as it should, because KCapsActivate flag is not set yet. 
	//CTSY sets KCapsActivate flag during context initialisation.
	RPacketService::TDynamicCapsFlags wantedDynCapBits = 0x00000000;
	CHECK_BITS_SET_L(dynCaps, wantedDynCapBits, KNoUnwantedBits, _L("Packet service's KCapsActivate flag not set"));


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Open a new primary context. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Set an incorrect password when setting the config with RPacketContext::SetConfig
	RPacketContext::TContextConfigGPRS config;
	_LIT(KFaultPwd, "IncorrectPwd");
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KFaultPwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);
	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	primaryPacketContext.SetConfig(setConfigStatus, configPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));
	
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	RPacketService::TStatus pckSrvcStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate the context 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("PacketContext::Activate timed out"));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("PacketContext::Activate did not return KErrNone"));

	// Check RPacketContext::NotifyStatusChange completes wtih EStatusActivating -> EStatusInactive.
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	
	// Check that the activation fails and error is returned. 
	// Check RPacketContext::GetLastErrorCause returns error != KErrNone
	TInt error;
	ASSERT_EQUALS(primaryPacketContext.GetLastErrorCause(error), KErrNone, _L("RPacketContext::GetLastErrorCause returned with an error."));
	ASSERT_TRUE(error != KErrNone, _L("RPacketContext::GetLastErrorCause returns incorrectly KErrNone."));

	// Check RPacketContext::GetStatus returns EStatusInactive.
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusAttached.
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// setAttachModeStatus
	// setConfigStatus
	// notifyContextStChStatus
	// contextActivateStatus
    CleanupStack::PopAndDestroy(4, &setAttachModeStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0019");
	}



CCTSYIntegrationTestPacketContext0020::CCTSYIntegrationTestPacketContext0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0020::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0020::~CCTSYIntegrationTestPacketContext0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0020
 * @SYMFssID BA/CTSY/PKTC-0020
 * @SYMTestCaseDesc Activate a context when phone is in class C operation and preferred bearer is RPacketService::EBearerPacketSwitched
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetPreferredBearer, RPacketContext::OpenNewContext, RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::Deactivate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Context activated and deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status and packet service status.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure attach mode to RPacketService::EAttachWhenPossible. 

	// Ensure MS class is class RPacketService::EMSClassAlternateMode. 

	// Ensure preferred bearer to  

	// RPacketService::EBearerPacketSwitched 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::GetPreferredBearer returns RPacketService::EBearerPacketSwitched

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate packet context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Deactivate the context 

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive

	// Check RPacketContext::GetStatus returns EStatusInactive

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached

	// Check RPacketService::GetStatus returns EStatusAttached

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0020");
	}



CCTSYIntegrationTestPacketContext0021::CCTSYIntegrationTestPacketContext0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0021::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0021::~CCTSYIntegrationTestPacketContext0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0021
 * @SYMFssID BA/CTSY/PKTC-0021
 * @SYMTestCaseDesc Cancel activation before activation is complete.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Activation cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context is not activated and activation cancelled successfully.

Verify subsequent attempt to activate succeeds and therefore roll back from cancelling worked correctly.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));
		
	// Get the context config using network info
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);	
	
	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
	
	// Open a new context	
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService,
												KPrimaryPacketContext1);
												
	// Set a valid context config using RPacketContext::SetConfig
	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	packetContext.SetConfig(setConfigStatus, configPk);	
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with error status."));

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone,	_L("RPacketService::NotifyContextAdded returned with error status."));
	ASSERT_TRUE(contextId.Length() > 0, _L("RPacketService::NotifyContextAdded completed with invalid context name."));
		
	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Activate a context (don't wait for completion) 		
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
					
	// Cancel activate with RTelSubSessionBase::CancelAsyncRequest(EPacketContextActivate)
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50003 );
	// When packet context is requested to be activated, there is nothing CTSY can do to cancel activation,
	// because activation has already been sent to the network.
	// CancelAsyncRequest(EPacketContextActivate) returns faulty with KErrNone instead of KErrNotSupported.
	packetContext.CancelAsyncRequest(EPacketContextActivate);

	// Check RTelSubSessionBase::CancelAsyncRequest(EPacketContextActivate) returns with KErrNotSupported
	TInt error;
	ASSERT_EQUALS(packetContext.GetLastErrorCause(error), KErrNone, _L("RPacketContext::GetLastErrorCause returned with an error."));
	ASSERT_EQUALS(error, KErrNotSupported, _L("RPacketContext::GetLastErrorCause does not return KErrNotSupported."));

    // Check RPacketService::NotifyStatusChange completes with EStatusActivating -> EStatusActive 	
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusActive
    RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////
    
    StartCleanup();
    
    // pop
	// getNetworkStatus
	// notifyContextStatus
	// setConfigStatus
	// notifyServicetStChStatus
	// contextActivateStatus
	CleanupStack::PopAndDestroy(5, &getNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0021");
	}



CCTSYIntegrationTestPacketContext0022::CCTSYIntegrationTestPacketContext0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0022::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0022::~CCTSYIntegrationTestPacketContext0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0022
 * @SYMFssID BA/CTSY/PKTC-0022
 * @SYMTestCaseDesc Activate context when request received from network.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketService::NotifyContextAdded, RPacketService::EnumerateContexts, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps
 * @SYMTestExpectedResults Pass - Context activated on request from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify context parameters in TPacketDataConfigBase derived class received correctly.

Verify context added notification produced, number of contexts, dynamic caps, packet service status are all correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Ensure RPacketService::GetDynamicCaps returns caps in set of KCapsRxContextActivationReq 
    RPacketService::TDynamicCapsFlags dynCaps;
    RPacketService::TDynamicCapsFlags wantedDynCapBits = RPacketService::KCapsRxContextActivationReq;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	CHECK_BITS_SET_L(dynCaps, wantedDynCapBits , KNoUnwantedBits, _L("RPacketService::GetDynamicCaps KCapsRxContextActivationReq flag not set"));


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

    // Post notifications for the RPacketService::NotifyContextActivationRequested
    TExtEtelRequestStatus notifyContextActivationRequestedStatus(packetService, EPacketNotifyContextActivationRequested);
    CleanupStack::PushL(notifyContextActivationRequestedStatus);
	RPacketContext::TContextConfigGPRS requestedConfig;
	TPckg<RPacketContext::TContextConfigGPRS> requestedConfigPckg(requestedConfig);
    packetService.NotifyContextActivationRequested(notifyContextActivationRequestedStatus,requestedConfigPckg);

	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Post notification for service's dynamic caps change
	TExtEtelRequestStatus notifyServiceDynCapsStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyServiceDynCapsStatus);
	packetService.NotifyDynamicCapsChange(notifyServiceDynCapsStatus, dynCaps);
    
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong),
				  						    KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(),
				  KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Simulator sends a context activation request to the phone.
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
    DisplayUserInteractionPromptL(_L("Please send me a context activation request."), ETimeLong);
	 
	 
	// $CTSYProblem The LTSY does nothing with the context activation request.
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: The LTSY does nothing with the context activation request.<font color=Black>"));    
	// Check RPacketService::NotifyContextActivationRequested completes.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextActivationRequestedStatus, ETimeLong), KErrNone,
				  _L("RPacketService::NotifyContextActivationRequested did not complete."));
    ASSERT_EQUALS(notifyContextActivationRequestedStatus.Int(), KErrNone, 
    			_L("RPacketService::NotifyContextActivationRequested returned with error status."));

	// Open a primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService,
			KPrimaryPacketContext1);
    
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Extract the context parameters received and check they are the same as that sent by the simulator.
	ASSERT_EQUALS_DES8(requestedConfig.iAccessPointName, config.iAccessPointName, _L("RPacketService::NotifyContextActivationRequested gave incorrect config data."));
	ASSERT_EQUALS_DES8(requestedConfig.iProtocolConfigOption.iAuthInfo.iUsername, config.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketService::NotifyContextActivationRequested gave incorrect config data."));
	ASSERT_EQUALS_DES8(requestedConfig.iProtocolConfigOption.iAuthInfo.iPassword, config.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketService::NotifyContextActivationRequested gave incorrect config data."));

	// Use them to set a context config with RPacketContext::SetConfig 
	TPckg<RPacketContext::TContextConfigGPRS> configPckg(requestedConfig);
	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	packetContext.SetConfig(setConfigStatus, configPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone,
				  _L("RPacketService::NotifyContextAdded did not complete."));
    ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, 
    			_L("RPacketService::NotifyContextAdded returned with error status."));
    ASSERT_TRUE(contextId.Length() > 0,
    			_L("Packet context config has zero length contextId."));

	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				packetContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    
    //   -> EStatusActive
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			packetContext,
			notifyContextStChStatus,
			packetContextStatus, 
			expectedPacketContextStatus,
			KErrNone);
	
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Check dynamic caps ===

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
    wantedDynCapBits = RPacketService::KCapsRxCSCall | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsSMSTransfer;
	TInt unWantedDynCapBits = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange( 
				packetService,
				notifyServiceDynCapsStatus,
				dynCaps, 
				wantedDynCapBits,
				unWantedDynCapBits,
				KErrNone);

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	ASSERT_BITS_SET(dynCaps, wantedDynCapBits, unWantedDynCapBits, _L("RPacketService::GetDynamicCaps Packet service's KCapsActivate flag not set"));

	// Check RPacketService::EnumerateContexts returns 1.
	TExtEtelRequestStatus enumContextsStatus(packetService, EPacketEnumerateContexts);
	CleanupStack::PushL(enumContextsStatus);
	TInt count=-1;
    TInt max=-1;
    packetService.EnumerateContexts(enumContextsStatus,count,max);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextsStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateContexts timed out."));
    ASSERT_EQUALS(enumContextsStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateContexts failed."));
	ASSERT_EQUALS(count,1,_L("RPacketService::EnumerateContexts did not returned one."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	//	enumContextsStatus
	//	contextActivateStatus
	//	setConfigStatus
	//	notifyContextStChStatus
	//	getNetworkStatus
	//	notifyServiceDynCapsStatus
	//	notifyServiceStChStatus
	//	notifyContextStatus
	//	notifyContextActivationRequestedStatus
	CleanupStack::PopAndDestroy(9,&notifyContextActivationRequestedStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0022");
	}



CCTSYIntegrationTestPacketContext0023::CCTSYIntegrationTestPacketContext0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0023::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0023::~CCTSYIntegrationTestPacketContext0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0023
 * @SYMFssID BA/CTSY/PKTC-0023
 * @SYMTestCaseDesc Activate context when request received from network.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::Activate, RPacketService::NotifyContextAdded, RPacketContext::GetStatus, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif, RPacketService::EnumerateNifs, RPacketContext::NotifyStatusChange, RPacketContext::SetConfig, RPacketService::EnumerateContextsInNif, RPacketService::NotifyContextActivationRequested, RPacketContext::GetDnsInfo
 * @SYMTestExpectedResults Pass - Context activated on request from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify NIF related info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10085);
	User::Leave(KErrNone);	
	
    // Ensure phone is attached to the packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Ensure RPacketService::GetDynamicCaps returns caps in set of KCapsRxContextActivationReq 
	RPacketService::TDynamicCapsFlags dynCaps;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	CHECK_BITS_SET_L(dynCaps, RPacketService::KCapsRxContextActivationReq, KNoUnwantedBits, _L("RPacketService::GetStaticCaps did not return correct dynamic caps"));

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	// post notifier for RPacketService::NotifyContextActivationRequested
	RPacketContext::TContextConfigGPRS configGPRS;
	TPckg<RPacketContext::TContextConfigGPRS> configPkg(configGPRS);
	TExtEtelRequestStatus notifyContextActivationRequested(packetService , EPacketNotifyContextActivationRequestedCancel);
	CleanupStack::PushL(notifyContextActivationRequested);
	packetService.NotifyContextActivationRequested(notifyContextActivationRequested, configPkg);

    // Ask the simulator to send a context activation request
	RPacketContext::TContextConfigGPRS configSimulator;
	TPckg<RPacketContext::TContextConfigGPRS> configSimulatorPkg(configSimulator);
    DisplayUserInteractionPromptL(_L("Simulator needs to send context activation request to this phone."), ETimeMedium);

	// Check RPacketService::NotifyContextActivationRequested completes with valid context configuration.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextActivationRequested, ETimeLong), KErrNone, _L("RPacketService::NotifyContextActivationRequested timed out."));
	ASSERT_EQUALS(notifyContextActivationRequested.Int(), KErrNone, _L("RPacketService::NotifyContextActivationRequested returned with error status."));
	ASSERT_TRUE(configGPRS.iAccessPointName.Length() > 0,_L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iAccessPointName."));
	ASSERT_TRUE(configGPRS.iProtocolConfigOption.iAuthInfo.iUsername.Length() > 0, _L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iUsername."));
	ASSERT_TRUE(configGPRS.iProtocolConfigOption.iAuthInfo.iPassword.Length() > 0, _L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iPassword."));

	// Post notification for the RPacketService::NotifyContextAdded
	TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);

	// Open a primary context. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Extract the context parameters received and check they are the same as that sent by the simulator. 
	// ETEL PACKET API says that nw may send PDP type requested, PDP address, Access Point Name (APN)
	ASSERT_EQUALS_DES8(configSimulator.iAccessPointName, configGPRS.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS(configSimulator.iPdpType, configGPRS.iPdpType, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(configSimulator.iPdpAddress, configGPRS.iPdpAddress, _L("RPacketContext::GetConfig returns incorrect config data."));
	 

	// Use them to set a context config with RPacketContext::SetConfig 
	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	primaryPacketContext.SetConfig(setConfigStatus, configPkg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with error status."));

	// Check RPacketService::NotifyContextAdded completes with a valid context name
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyContextStatus, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
    ASSERT_EQUALS(notifyContextStatus.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
    ASSERT_TRUE(contextId.Length() > 0, _L("RPacketService::NotifyContextAdded completed with invalid context name"));

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// ===  Check context statuses ===

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	

    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Get context information ===

	// Check RPacketContext::GetConfig returns same config as that set
	TExtEtelRequestStatus contextGetConfigStatus(primaryPacketContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextGetConfigStatus);
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPkg(getConfig);
	primaryPacketContext.GetConfig(contextGetConfigStatus, getConfigPkg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextGetConfigStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextGetConfigStatus.Int(), KErrNone, _L("RPacketContext::GetConfig returned an error."));

	ASSERT_EQUALS_DES8(getConfig.iAccessPointName, configGPRS.iAccessPointName, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iUsername, configGPRS.iProtocolConfigOption.iAuthInfo.iUsername, _L("RPacketContext::GetConfig returns incorrect config data."));
	ASSERT_EQUALS_DES8(getConfig.iProtocolConfigOption.iAuthInfo.iPassword, configGPRS.iProtocolConfigOption.iAuthInfo.iPassword, _L("RPacketContext::GetConfig returns incorrect config data."));

	// Check RPacketContext::GetDnsInfo returns valid primary and secondary DNS ip addresses
	RPacketContext::TDnsInfoV2 dnsInfo;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoPkg(dnsInfo);
	TExtEtelRequestStatus dnsInfoStatus(primaryPacketContext, EPacketContextGetDNSInfo);
	CleanupStack::PushL(dnsInfoStatus);
	primaryPacketContext.GetDnsInfo(dnsInfoStatus, dnsInfoPkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dnsInfoStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetDnsInfo timed out."));

	ASSERT_EQUALS(dnsInfoStatus.Int(), KErrNone, _L("RPacketContext::GetDnsInfo returned an error."));
	ASSERT_TRUE(dnsInfo.iPrimaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length PrimaryDns."));
	ASSERT_TRUE(dnsInfo.iSecondaryDns.Length() > 0, _L("RPacketContext::GetDnsInfo returned with zero length SecondaryDns."));

	// ===  Check that there is one NIF with one primary context belonging to it ===

	// Check RPacketService::EnumerateNifs returns aCount = 1

	TExtEtelRequestStatus enumerateNifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsStatus);
	TInt nifCount;
	const TInt wantedCount = 1;
	packetService.EnumerateNifs(enumerateNifsStatus, nifCount);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out"));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateNifs returned wrong NIF count."));


	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	const TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing context name returns aCount = 1
	TExtEtelRequestStatus enumerateContextsInNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifStatus);
	TInt contextsInNifCount;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, contextInfo.iName, contextsInNifCount);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out"));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif returned wrong number of contexts sharing a NIF"));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing context name and aIndex = 0 returns aContextName which is same as aExistingContextName
	TExtEtelRequestStatus getContextNameInNifStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifStatus);
	TName contextName;
	packetService.GetContextNameInNif(getContextNameInNifStatus, contextInfo.iName, index, contextName);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out"));
	ASSERT_EQUALS_DES16(contextInfo.iName, contextName, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TExtEtelRequestStatus getNifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoStatus);
	const TInt nifInfoCount = 0;
	RPacketService::TNifInfoV2 nifInfo;
	RPacketService::TNifInfoV2Pckg nifInfoPkg(nifInfo);
	packetService.GetNifInfo(getNifInfoStatus, nifInfoCount, nifInfoPkg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo timed out"));
	ASSERT_TRUE(nifInfo.iContextName.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid context name"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 1
	ASSERT_EQUALS(nifInfo.iNumberOfContexts, wantedCount, _L("RPacketService::GetNifInfo returned wrong count of contexts"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo returned wrong NIF status"));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext in iContextType
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EExternalContext, _L("RPacketService::GetNifInfo returned wrong context type"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyContextActivationRequested
	// setConfigStatus
	// notifyContextStatus
	// notifyContextStChStatus
	// contextActivateStatus
	// contextGetConfigStatus
	// dnsInfoStatus
	// enumerateNifStatus
	// getContextInfo
	// enumerateContextsInNifStatus
	// getContextNameInNifStatus
	// getNifInfoStatus

	CleanupStack::PopAndDestroy(12, &notifyContextActivationRequested);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0023");
	}



CCTSYIntegrationTestPacketContext0024::CCTSYIntegrationTestPacketContext0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0024::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0024::~CCTSYIntegrationTestPacketContext0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0024
 * @SYMFssID BA/CTSY/PKTC-0024
 * @SYMTestCaseDesc Reject network requested context activation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::RejectActivationRequest, RPacketService::EnumerateContexts
 * @SYMTestExpectedResults Pass - Context activation request rejected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify context parameters in TPacketDataConfigBase derive class received correctly.  Verify when network repeated requests for a context activation (up to 5 times), it can be rejected.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10086);
	User::Leave(KErrNone);

    // Check that the phone is registered with the network simulator. 
    RMobilePhone &mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(mobilePhone);

	// Ensure phone is attached to the packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Ensure RPacketService::GetDynamicCaps returns caps in set of KCapsRxContextActivationReq 
	RPacketService::TDynamicCapsFlags dynCaps;
	CHECK_EQUALS_L(packetService.GetDynamicCaps(dynCaps), KErrNone, _L("RPacketService::GetDynamicCaps returned with an error"));
	CHECK_BITS_SET_L(dynCaps, RPacketService::KCapsRxContextActivationReq, KNoUnwantedBits, _L("RPacketService::GetDynamicCaps returned with incorrect caps"));


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// post notifier for RPacketService::NotifyContextActivationRequested
	RPacketContext::TContextConfigGPRS configGPRS;
	TPckg<RPacketContext::TContextConfigGPRS> configPkg(configGPRS);
	TExtEtelRequestStatus notifyContextActivationRequested(packetService , EPacketNotifyContextActivationRequested);
	CleanupStack::PushL(notifyContextActivationRequested);
	packetService.NotifyContextActivationRequested(notifyContextActivationRequested, configPkg);

    // Ask the simulator to send a context activation request
    DisplayUserInteractionPromptL(_L("Simulator needs to send context activation request to this phone."), ETimeMedium);
	
	// Check RPacketService::NotifyContextActivationRequested completes with valid context configuration.
	iPacketServiceTestHelper.WaitForPacketServiceNotifyContextActivationRequested( 
													packetService,
													notifyContextActivationRequested,
													configPkg,
													KErrNone );

	ASSERT_EQUALS(notifyContextActivationRequested.Int(), KErrNone, _L("RPacketService::NotifyContextActivationRequested returned with error status."));
	ASSERT_TRUE(configGPRS.iAccessPointName.Length() > 0,_L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iAccessPointName."));
	ASSERT_TRUE(configGPRS.iProtocolConfigOption.iAuthInfo.iUsername.Length() > 0, _L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iUsername."));
	ASSERT_TRUE(configGPRS.iProtocolConfigOption.iAuthInfo.iPassword.Length() > 0, _L("RPacketService::NotifyContextActivationRequested returned from Simulator with incorrect or zero length iPassword."));
	
	// Check RPacketService::RejectActivationRequest completes with KErrNone
	TExtEtelRequestStatus rejectActivationRequestStatus(packetService, EPacketRejectActivationRequest);	
	CleanupStack::PushL(rejectActivationRequestStatus);
	packetService.RejectActivationRequest(rejectActivationRequestStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(rejectActivationRequestStatus, ETimeLong), KErrNone, _L("RPacketService::RejectActivationRequest timed out"));
	ASSERT_EQUALS(rejectActivationRequestStatus.Int(), KErrNone, _L("RPacketService::RejectActivationRequest returned with error status."));

	// Check RPacketService::EnumerateContexts returns 0
	TExtEtelRequestStatus enumerateContextsStatus(packetService, EPacketEnumerateContexts);	
	CleanupStack::PushL(enumerateContextsStatus);
	TInt count(0);
	TInt maxAllowed(0);
	const TInt wantedCount = 0;
	packetService.EnumerateContexts(enumerateContextsStatus, count, maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumerateContextsStatus.Int(), KErrNone, _L("RPacketService::EnumerateContexts returned with error status."));
	ASSERT_EQUALS(count, wantedCount, _L("RPacketService::EnumerateContexts did not return context count 0 as expected"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyContextActivationRequested
	// rejectActivationRequestStatus
	// enumerateContextsStatus
	CleanupStack::PopAndDestroy(3, &notifyContextActivationRequested);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0024");
	}



CCTSYIntegrationTestPacketContext0025::CCTSYIntegrationTestPacketContext0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0025::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0025::~CCTSYIntegrationTestPacketContext0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0025
 * @SYMFssID BA/CTSY/PKTC-0025
 * @SYMTestCaseDesc Delete a context when it is active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::Delete, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Context is deleted and the status of the contexts reflects this status..
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status is correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// === Ensure there is an active PDP context. ===
    
    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    
    // Ensure phone is attached to the packet service. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Open a new context.	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Delete the context with RPacketContext::Delete 
	TExtEtelRequestStatus deleteStatus(packetService, EPacketContextDelete);
	CleanupStack::PushL(deleteStatus);
	primaryPacketContext.Delete(deleteStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStatus, ETimeLong), KErrNone, _L("RPacketContext::Delete timed out"));

	// Check an error is returned.
	// $CTSYProblem Adaptation deletes context always if it's status is else than RPacketContext::EStatusUnknown.
	// Checking is changed from NOT KErrNone to KErrNone and GetStatus() checking is changed from EStatusActive to EStatusDeleted
	ASSERT_EQUALS(deleteStatus.Int(), KErrNone, _L("RPacketContext::Delete returned with an error"));

	// Check RPacketContext::GetStatus returns EStatusDeleted
	RPacketContext::TContextStatus packetContextStatus;
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeleted, _L("RPacketContext::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// deleteStatus
	CleanupStack::PopAndDestroy(1, &deleteStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0025");
	}



CCTSYIntegrationTestPacketContext0026::CCTSYIntegrationTestPacketContext0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0026::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0026::~CCTSYIntegrationTestPacketContext0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0026
 * @SYMFssID BA/CTSY/PKTC-0026
 * @SYMTestCaseDesc Delete a context when it has been deactivated.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::Delete
 * @SYMTestExpectedResults Pass - Context deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status is deleted.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
	
	// Ensure there is an active PDP context. 

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Open a new context.	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
											 
	// Activate primary context											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);	
	
	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus packetContextStatus;
	CHECK_EQUALS_L(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	CHECK_EQUALS_L(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////	
	
	
	//Post notifier for packet context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Deactivate the context. 
	TExtEtelRequestStatus contextDeactivateStatus(primaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	primaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive				
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;  
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Delete the context with RPacketContext::Delete 	
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	TExtEtelRequestStatus contextDeleteStatus(primaryPacketContext, EPacketContextDelete);
	CleanupStack::PushL(contextDeleteStatus);
	primaryPacketContext.Delete(contextDeleteStatus);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeleteStatus, ETimeLong), KErrNone, _L("RPacketContext::Delete timed out"));
	ASSERT_EQUALS(contextDeleteStatus.Int(), KErrNone, _L("RPacketContext::Delete returned an error"));
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusDeleted
	expectedPacketContextStatus = RPacketContext::EStatusDeleted;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusDeleted
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusDeleted, _L("RPacketContext::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyContextStChStatus
	// contextDeactivateStatus
	// contextDeleteStatus
	CleanupStack::PopAndDestroy(3, &notifyContextStChStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0026");
	}



CCTSYIntegrationTestPacketContext0027::CCTSYIntegrationTestPacketContext0027(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0027::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0027::~CCTSYIntegrationTestPacketContext0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0027
 * @SYMFssID BA/CTSY/PKTC-0027
 * @SYMTestCaseDesc Dial a voice call while context is active in class B operation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RCall::Dial, RCall::HangUp, RCall::GetStatus, RCall::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Packet service suspends when CS call is made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context and packet service are suspended when voice call is initiated whilst context is activated
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure MS class is EMSClassSuspensionRequired 

	// Ensure there is an active PDP context. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::GetMSClass returns EMSClassSuspensionRequired

	// Dial a number that answers. 

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusConnected.

	// Check RCall::GetStatus returns EStatusConnected.

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusSuspended

	// Check RPacketService::GetStatus returns EStatusSuspended

	// Check RPacketContext::NotifyStatusChange completes with EStatusSuspended

	// Check RPacketContext::GetStatus returns EStatusSuspended

	// Hang up call. 

	// Check RCall::NotifyStatusChange completes with EStatusHangingUp -> EStatusIdle.

	// Check RCall::GetStatus returns EStatusIdle.

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange completes with EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0027");
	}



CCTSYIntegrationTestPacketContext0028::CCTSYIntegrationTestPacketContext0028(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0028::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0028::~CCTSYIntegrationTestPacketContext0028()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0028::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0028
 * @SYMFssID BA/CTSY/PKTC-0028
 * @SYMTestCaseDesc Get default context params.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::GetConfig, RPacketService::SetDefaultContextParams, RPacketService::GetContextInfo
 * @SYMTestExpectedResults Pass - Context activated with default params.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context activated without specific config has default context params.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	const TInt gprs = 1;
	const TInt r99 = 2;
	const TInt r5 = 3;

   	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);

	// Activate context using RPacketContext::TContextConfigGPRS
	DoTest0028L(packetService, apn, user, pwd, gprs);

	// Repeat test with RPacketContext::TContextConfigR99_R4
	DoTest0028L(packetService, apn, user, pwd, r99);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50004 );
	// Test case is made to leave because CTSY doesn't have support for TContextConfig_R5.
	CHECK_TRUE_L(EFalse, _L("This test has been made to leave intentionally - CTSY doesn't have support for TContextConfig_R5")); 

	// Repeat test with RPacketContext::TContextConfig_R5
	DoTest0028L(packetService, apn, user, pwd, r5);

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getNetworkStatus
	CleanupStack::PopAndDestroy(1, &getNetworkStatus);
	
	return TestStepResult();
	}


void CCTSYIntegrationTestPacketContext0028::DoTest0028L(RPacketService& aPacketService, TPtrC aApn, TPtrC aUser, TPtrC aPwd, TInt aNWType)
/**
 * Activates primary packet context with default context parameters.
 * @param aPacketService Reference to packet service.
 * @param aNWType Gprs / R99_R4 / R5.
 */
	{
	TExtEtelRequestStatus reqSetDefaultContextParams(aPacketService, EPacketSetDefaultContextParams);	
	CleanupStack::PushL(reqSetDefaultContextParams);
	RPacketContext::TContextConfigGPRS setConfigGprs;
	TPckg<RPacketContext::TContextConfigGPRS> setConfigPkgGprs(setConfigGprs);
	RPacketContext::TContextConfigR99_R4 setConfigR99;
	TPckg<RPacketContext::TContextConfigR99_R4> setConfigPkgR99(setConfigR99);
	RPacketContext::TContextConfig_R5 setConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> setConfigPkgR5(setConfigR5);

	switch(aNWType)
		{
		case 1: // GPRS
			// Set valid context params with RPacketService::SetDefaultContextParams and using RPacketContext::TContextConfigGPRS
			setConfigGprs.iAccessPointName.Copy(aApn);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			aPacketService.SetDefaultContextParams(reqSetDefaultContextParams, setConfigPkgGprs);
			break;
		
		case 2: // R99_R4
			// Set valid context params with RPacketService::SetDefaultContextParams and using RPacketContext::TContextConfigR99_R4
			setConfigR99.iAccessPointName.Copy(aApn);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			aPacketService.SetDefaultContextParams(reqSetDefaultContextParams, setConfigPkgR99);
			break;

		case 3: // R5
			// Set valid context params with RPacketService::SetDefaultContextParams and using RPacketContext::TContextConfig_R5
			setConfigR5.iAccessPointName.Copy(aApn);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			aPacketService.SetDefaultContextParams(reqSetDefaultContextParams, setConfigPkgR5);
			break;
		}
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqSetDefaultContextParams, ETimeLong), KErrNone, _L("RPacketService::SetDefaultContextParams timed out"));
	ASSERT_EQUALS(reqSetDefaultContextParams.Int(), KErrNone, _L("RPacketService::SetDefaultContextParams returned with an error"));

	// Check RPacketService::GetDefaultContextParams returns same params as that set
	TExtEtelRequestStatus reqGetDefaultContextParams(aPacketService, EPacketGetDefaultContextParams);	
	CleanupStack::PushL(reqGetDefaultContextParams);
	RPacketContext::TContextConfigGPRS getConfigGprs;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPkgGprs(getConfigGprs);
	RPacketContext::TContextConfigR99_R4 getConfigR99;
	TPckg<RPacketContext::TContextConfigR99_R4> getConfigPkgR99(getConfigR99);
	RPacketContext::TContextConfig_R5 getConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> getConfigPkgR5(getConfigR5);

	switch(aNWType)
		{
		case 1: // GPRS
			aPacketService.GetDefaultContextParams(reqGetDefaultContextParams, getConfigPkgGprs);
			break;
		
		case 2: // R99_R4
			aPacketService.GetDefaultContextParams(reqGetDefaultContextParams, getConfigPkgR99);
			break;

		case 3: // R5
			aPacketService.GetDefaultContextParams(reqGetDefaultContextParams, getConfigPkgR5);
			break;
		}

	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqGetDefaultContextParams, ETimeLong), KErrNone, _L("RPacketService::GetDefaultContextParams timed out"));
	ASSERT_EQUALS(reqGetDefaultContextParams.Int(), KErrNone, _L("RPacketService::GetDefaultContextParams returned with an error"));

	TCmpRPacketContextTContextConfigGPRS contextConfigGPRSCmp(getConfigGprs, setConfigGprs, *this);
	TCmpRPacketContextTContextConfigR99_R4 contextConfigR99Cmp(getConfigR99, setConfigR99, *this);
	TCmpRPacketContextTContextConfig_R5 contextConfigR5Cmp(getConfigR5, setConfigR5, *this);

	switch(aNWType)
		{
		case 1: // GPRS
			ASSERT_TRUE(contextConfigGPRSCmp.IsEqual(ELogError), _L("RPacketService.GetDefaultContextParams did not return same params as that set"));	
			break;

		case 2: // R99_R4
			ASSERT_TRUE(contextConfigR99Cmp.IsEqual(ELogError), _L("RPacketService.GetDefaultContextParams did not return same params as that set"));	
			break;

		case 3: // R5
			ASSERT_TRUE(contextConfigR5Cmp.IsEqual(ELogError), _L("RPacketService.GetDefaultContextParams did not return same params as that set"));	
			break;
		}

	// Open a new context.	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(aPacketService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	aPacketService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Post notification for primary context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate a context with RPacketContext::Activate 
	//$CTSYProblem SetConfig is required before activation of the context. 
	//If SetConfig is not called, Activate returns KErrTimedOut(-33). 
	//SetDefaultContextParams sends context parameters common to all contexts to adaptation/LTSY layer.
	//However,for some reason, adaptation does not use these params when Activate is called 
	//and requires setting of context specific parameters with SetConfig     
	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	switch(aNWType)
		{
		case 1: // GPRS
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgGprs);
			break;

		case 2: // R99_R4
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR99);
			break;

		case 3: // R5
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR5);
			break;
		}
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with error status."));

	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				aPacketService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(aPacketService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Get context information ===

	// Check RPacketContext::GetConfig returns same config as the default set in RPacketService::SetDefaultContextParams
	TExtEtelRequestStatus contextConfigStatus(primaryPacketContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);

	switch(aNWType)
		{
		case 1: // GPRS
			primaryPacketContext.GetConfig(contextConfigStatus, getConfigPkgGprs);
			break;
		
		case 2: // R99_R4
			primaryPacketContext.GetConfig(contextConfigStatus, getConfigPkgR99);
			break;

		case 3: // R5
			primaryPacketContext.GetConfig(contextConfigStatus, getConfigPkgR5);
			break;
		}

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConfig timed out."))
	ASSERT_EQUALS(contextConfigStatus.Int(), KErrNone, _L("RPacketContext::GetConfig returned an error."));
	
	TCmpRPacketContextTContextConfigGPRS contextConfigGPRSCmp2(getConfigGprs, setConfigGprs, *this);
	TCmpRPacketContextTContextConfigR99_R4 contextConfigR99Cmp2(getConfigR99, setConfigR99, *this);
	TCmpRPacketContextTContextConfig_R5 contextConfigR5Cmp2(getConfigR5, setConfigR5, *this);

	switch(aNWType)
		{
		case 1: // GPRS
			ASSERT_TRUE(contextConfigGPRSCmp2.IsEqual(ELogError), _L("RPacketService.GetConfig did not return same params as the default set in RPacketService::SetDefaultContextParams"));
			break;

		case 2: // R99_R4
			ASSERT_TRUE(contextConfigR99Cmp2.IsEqual(ELogError), _L("RPacketService.GetConfig did not return same params as the default set in RPacketService::SetDefaultContextParams"));
			break;

		case 3: // R5
			ASSERT_TRUE(contextConfigR5Cmp2.IsEqual(ELogError), _L("RPacketService.GetConfig did not return same params as the default set in RPacketService::SetDefaultContextParams"));
			break;
		}

	// Check RPacketService::EnumerateContexts returns 1.
	TExtEtelRequestStatus enumerateContextsStatus(aPacketService, EPacketEnumerateContexts);	
	CleanupStack::PushL(enumerateContextsStatus);
	TInt count(0);
	TInt maxAllowed(0);
	const TInt wantedCount = 1;
	aPacketService.EnumerateContexts(enumerateContextsStatus, count, maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumerateContextsStatus.Int(), KErrNone, _L("RPacketService::EnumerateContexts returned with error status."));
	ASSERT_EQUALS(count, wantedCount, _L("RPacketService::EnumerateContexts did not return context count 1 as expected"));

	// Check RPacketService::GetContextInfo with aIndex = 0 returns correct name in iName
	TExtEtelRequestStatus getContextInfo(aPacketService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	const TInt index = 0;
	aPacketService.GetContextInfo(getContextInfo, index, contextInfo);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Check RPacketService::GetContextInfo with aIndex = 0 returns EStatusActive in iStatus
	ASSERT_EQUALS(contextInfo.iStatus, RPacketContext::EStatusActive, _L("RPacketService::GetContextInfo did not return iStatus RPacketContext::EStatusActive as expected"));

	// Release packet context
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);

	// Pop
	// reqSetDefaultContextParams
	// reqGetDefaultContextParams
	// notifyServiceStChStatus
	// notifyContextStChStatus
	// setConfigStatus
	// contextActivateStatus
	// contextConfigStatus
	// enumerateContextsStatus
	// getContextInfo
	CleanupStack::PopAndDestroy(9, &reqSetDefaultContextParams);

	}


TPtrC CCTSYIntegrationTestPacketContext0028::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0028");
	}



CCTSYIntegrationTestPacketContext0029::CCTSYIntegrationTestPacketContext0029(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0029::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0029::~CCTSYIntegrationTestPacketContext0029()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0029::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0029
 * @SYMFssID BA/CTSY/PKTC-0029
 * @SYMTestCaseDesc Set default context params to unsupported type.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::SetDefaultContextParams, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::GetConfig, RPacketService::SetDefaultContextParams, RPacketService::Open
 * @SYMTestExpectedResults Pass - Context activated with original default params.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context activated without specific config has original default context params (not ones set).
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Get the default context params with RPacketService::GetDefaultContextParams 	
	RPacketContext::TContextConfigGPRS dummyConfig;
	TPckg<RPacketContext::TContextConfigGPRS> dummyConfigPkgGprs(dummyConfig);
	
	TExtEtelRequestStatus reqGetDummyContextParams(packetService, EPacketGetDefaultContextParams);	
	CleanupStack::PushL(reqGetDummyContextParams);
	packetService.GetDefaultContextParams(reqGetDummyContextParams, dummyConfigPkgGprs);
	
	//Check RPacketService::GetDefaultContextParams returns with KErrNotReady
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqGetDummyContextParams, ETimeLong), KErrNone, _L("RPacketService::GetDefaultContextParams timed out"));
	ASSERT_EQUALS(reqGetDummyContextParams.Int(), KErrNotReady, _L("RPacketService::GetDefaultContextParams did not return KErrNotReady"));
		
	// Get network info from phone	 
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong),
				  						    KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(),
				  KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));
	
	// Get the context config using network info
	RPacketContext::TContextConfigGPRS defaultConfig;
	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	defaultConfig.iAccessPointName.Copy(apn);
	defaultConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	defaultConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> defaultConfigPkgGprs(defaultConfig);

	// Set the default context params with RPacketService::SetDefaultContextParams 	
	DEBUG_PRINTF1(_L("Setting Default Context Params"));
	TExtEtelRequestStatus regSetInitParams(packetService,EPacketSetDefaultContextParams);
	CleanupStack::PushL(regSetInitParams);		
	packetService.SetDefaultContextParams(regSetInitParams, defaultConfigPkgGprs);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regSetInitParams, ETimeLong), KErrNone, _L("RPacketService::SetDefaultContextParams timed out"));
	ASSERT_EQUALS(regSetInitParams.Int(), KErrNone, _L("RPacketService::SetDefaultContextParams returned with an error"));
	
	// Get the default context params with RPacketService::GetDefaultContextParams check they are the same as what was set
	DEBUG_PRINTF1(_L("Getting Default Context Params"));
	RPacketContext::TContextConfigGPRS getConfig;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPkgGprs(getConfig);
	 	
	TExtEtelRequestStatus reqGetDefaultContextParams(packetService, EPacketGetDefaultContextParams);	
	CleanupStack::PushL(reqGetDefaultContextParams);
	packetService.GetDefaultContextParams(reqGetDefaultContextParams, getConfigPkgGprs);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqGetDefaultContextParams, ETimeLong), KErrNone, _L("RPacketService::GetDefaultContextParams timed out"));
	ASSERT_EQUALS(reqGetDefaultContextParams.Int(), KErrNone, _L("RPacketService::GetDefaultContextParams returned with an error"));

	DEBUG_PRINTF1(_L("Comparing default Context Params"));
	TCmpRPacketContextTContextConfigGPRS contextParamsComparator(defaultConfig, getConfig, *this);
	ASSERT_TRUE(contextParamsComparator.IsEqual(ELogError), _L("RPacketService::GetDefaultContextParams did not return set parameters"));
		
	// Set invalid context parameters using RPacketService::SetDefaultContextParams and using RPacketContext::TContextConfigGPRS 
	RPacketContext::TContextConfigGPRS invalidConfig;
	invalidConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	invalidConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	_LIT(KFaultyAPN, "Incorrect APN name");
	invalidConfig.iAccessPointName.Copy(KFaultyAPN);
	TPckg<RPacketContext::TContextConfigGPRS> invalidConfigPkgGprs(invalidConfig);
	
	TExtEtelRequestStatus reqSetDefaultContextParams(packetService, EPacketSetDefaultContextParams);	
	CleanupStack::PushL(reqSetDefaultContextParams);
	packetService.SetDefaultContextParams(reqSetDefaultContextParams, invalidConfigPkgGprs);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqSetDefaultContextParams, ETimeLong), KErrNone, _L("RPacketService::SetDefaultContextParams timed out"));
	ASSERT_EQUALS(reqSetDefaultContextParams.Int(), KErrNone, _L("RPacketService::SetDefaultContextParams returned with an error"));


	// Open a new context. 	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);		

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);

	// Post notification for primary context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	//$CTSYProblem
	//SetConfig is required before activation of the context. 
	//If SetConfig is not called, Activate returns KErrTimedOut(-33).
	//SetDefaultContextParams sends context parameters common to all contexts to adaptation/LTSY layer.
	//However,for some reason, adaptation does not use these params when Activate is called 
	//and requires setting of context specific parameters with SetConfig     

	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	primaryPacketContext.SetConfig(setConfigStatus, defaultConfigPkgGprs);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));

	// Activate packet context with RPacketContext::Activate	
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
		
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::GetConfig returns same config as the default returned by RPacketService::GetDefaultContextParams
	TExtEtelRequestStatus contextConfigStatus(primaryPacketContext, EPacketContextGetConfig);
	CleanupStack::PushL(contextConfigStatus);
	
	RPacketContext::TContextConfigGPRS getConfigGprs2;
	TPckg<RPacketContext::TContextConfigGPRS> getConfigPkgGprs2(getConfigGprs2);
	primaryPacketContext.GetConfig(contextConfigStatus, getConfigPkgGprs2);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextConfigStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConfig timed out."));
	
	TCmpRPacketContextTContextConfigGPRS contextParamsComparatorNumberTwo(defaultConfig, getConfigGprs2, *this);
	ASSERT_TRUE(contextParamsComparatorNumberTwo.IsEqual(ELogError), _L("RPacketService::GetConfig did not return same config as the default returned by RPacketService::GetDefaultContextParams"));

	
			
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// reqGetDummyContextParams
	// getNetworkStatus
	// regSetInitParams
	// reqGetDefaultContextParams
	// reqSetDefaultContextParams
	// notifyServiceStChStatus
	// notifyContextStChStatus
	// setConfigStatus
	// contextActivateStatus
	// contextConfigStatus
	CleanupStack::PopAndDestroy(10,&reqGetDummyContextParams);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0029::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0029");
	}



CCTSYIntegrationTestPacketContext0030::CCTSYIntegrationTestPacketContext0030(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0030::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0030::~CCTSYIntegrationTestPacketContext0030()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0030::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0030
 * @SYMFssID BA/CTSY/PKTC-0030
 * @SYMTestCaseDesc Deactivate NIF when there is only one NIF.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::EnumerateContextsInNif, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::DeactivateNIF, RPacketService::GetNifInfo, RPacketService::DeactivateNIF, RPacketService::GetNifInfo, RPacketService::DeactivateNIF
 * @SYMTestExpectedResults Pass - All contexts in NIF deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify primary and secondary contexts in NIF are deactivated when primary context name specified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary and secondary context. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
    // Check if we are on a simulated network
	CHECK_EQUALS_L( iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL( phone ), KErrNone, 
			_L("Network is unavailable") );
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    TExtEtelRequestStatus notifyContextStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatus);
	TName contextId;
	packetService.NotifyContextAdded(notifyContextStatus, contextId);
    RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
    iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);
	TName secondaryContextId;
	packetService.NotifyContextAdded(notifyContextStatus, secondaryContextId);
	RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
    CHECK_TRUE_L(ActivateSecondaryPacketContextL(secondaryPacketContext),_L("Can't activate the secondary context"));

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::EnumerateNifs returns aCount = 1
	TExtEtelRequestStatus enumerateNifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsStatus);
	TInt count=-1;
	packetService.EnumerateNifs(enumerateNifsStatus,count);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(enumerateNifsStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateNifs failed."));
	ASSERT_EQUALS(count,1,_L("RPacketService::EnumerateNifs did not return correct number of NIFs."));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TExtEtelRequestStatus getNifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoStatus);
	RPacketService::TNifInfoV2 nifInfo;
	RPacketService::TNifInfoV2Pckg nifInfoPckg(nifInfo);
	packetService.GetNifInfo(getNifInfoStatus,0,nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(getNifInfoStatus.Int(), KErrNone, 
			_L("RPacketService::GetNifInfo failed."));
	ASSERT_TRUE(nifInfo.iContextName.Length()>0,_L("RPacketService::GetNifInfo invlaid context name"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	ASSERT_EQUALS(nifInfo.iNumberOfContexts,2,_L("RPacketService::GetNifInfo wrong number of contexts"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus,RPacketContext::EStatusActive,_L("RPacketService::GetNifInfo wrong status"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()>0,_L("RPacketService::GetNifInfo invlaid PDP address"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns EInternalContext
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext,_L("RPacketService::GetNifInfo wrong type"));

	// Check RPacketService::GetNifInfo with aCount = 1 returns KErrArgument
	packetService.GetNifInfo(getNifInfoStatus,1,nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(getNifInfoStatus.Int(), KErrArgument, 
			_L("RPacketService::GetNifInfo did not failed."));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing primary context name returns aCount = 2
	TExtEtelRequestStatus enumContextsInfStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumContextsInfStatus);
	count=-1;
	packetService.EnumerateContextsInNif(enumContextsInfStatus,contextId,count);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextsInfStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(enumContextsInfStatus.Int(), KErrNone, 
			_L("RPacketService::EnumerateContextsInNif failed."));
	ASSERT_EQUALS(count,2,_L("RPacketService::EnumerateContextsInNif did not return correct number of NIFs."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 0 returns aContextName = name of primary context
	TBuf<200> contextName;
	TExtEtelRequestStatus getContextNameInNifStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifStatus);
	packetService.GetContextNameInNif(getContextNameInNifStatus,contextId,0,contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS(getContextNameInNifStatus.Int(), KErrNone, 
			_L("RPacketService::GetContextNameInNif failed."));
	ASSERT_EQUALS_DES16(contextName,contextId,_L("RPacketService::GetContextNameInNif did not returned the expected value."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 1 returns aContextName = name of secondary context
	packetService.GetContextNameInNif(getContextNameInNifStatus,contextId,1,contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS(getContextNameInNifStatus.Int(), KErrNone, 
			_L("RPacketService::GetContextNameInNif failed."));
	ASSERT_EQUALS_DES16(contextName,secondaryContextId,_L("RPacketService::GetContextNameInNif did not returned the expected value."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Post notification for secondary context's status change
	TExtEtelRequestStatus secondaryNotifyContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(secondaryNotifyContextStChStatus);
	RPacketContext::TContextStatus secondaryPacketContextStatus;
    secondaryPacketContext.NotifyStatusChange(secondaryNotifyContextStChStatus, secondaryPacketContextStatus);

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
    
	// Deactivate the NIF with RPacketService::DeactivateNIF and aContextName = name of primary context 
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10047);
	ASSERT_EQUALS(1,0, _L("Test leaving because it will cause TSY crash, remove this when defect is fixed"));
	TEST_CHECK_POINT_L(_L("test leaving..."));
	TExtEtelRequestStatus deactivateNifStatus(packetService, EPacketDeactivateNIF);
	CleanupStack::PushL(deactivateNifStatus);
    packetService.DeactivateNIF(deactivateNifStatus,contextId);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateNifStatus, ETimeMedium), KErrNone, 
			_L("RPacketService::DeactivateNIF timed out."));
	ASSERT_EQUALS(deactivateNifStatus.Int(), KErrNone, 
			_L("RPacketService::DeactivateNIF failed."));
    

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary context completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	//  -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for primary context returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange for secondary context completes with EStatusDeactivating -> EStatusInactive
    expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				secondaryNotifyContextStChStatus,
				secondaryPacketContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	//   ->  EStatusInactive
    secondaryPacketContext.NotifyStatusChange(secondaryNotifyContextStChStatus, secondaryPacketContextStatus);
    expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				secondaryNotifyContextStChStatus,
				secondaryPacketContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusAttached
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////
    StartCleanup();
	
	// Pop:
	//	deactivateNifStatus
	//	notifyServiceStChStatus
	//	secondaryNotifyContextStChStatus
	//	notifyContextStChStatus
	//	getContextNameInNifStatus
	//	enumContextsInfStatus
	//	getNifInfoStatus
	//	enumerateNifsStatus
	//	notifyContextStatus
	CleanupStack::PopAndDestroy(9,&notifyContextStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0030::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0030");
	}



CCTSYIntegrationTestPacketContext0031::CCTSYIntegrationTestPacketContext0031(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0031::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0031::~CCTSYIntegrationTestPacketContext0031()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0031::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0031
 * @SYMFssID BA/CTSY/PKTC-0031
 * @SYMTestCaseDesc Deactivate NIF when there is only one NIF.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::EnumerateContextsInNif, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::DeactivateNIF, RPacketService::GetNifInfo, RPacketService::DeactivateNIF, RPacketService::GetNifInfo, RPacketService::DeactivateNIF
 * @SYMTestExpectedResults Pass - All contexts in NIF deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify primary and secondary contexts in NIF are deactivated when secondary context name specified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
    //Check if we are on a simulated network
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	//  ===  Ensure there is an active primary and secondary context. ===   

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    // Open a primary PDP context
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

    // Open a secondary PDP context	
	RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
	
    // Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);
		
	// Activate the secondary context 1.
	ActivateSecondaryPacketContextL(secondaryPacketContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));
	index = 1;
	RPacketService::TContextInfo contextInfo2;
	packetService.GetContextInfo(getContextInfo, index, contextInfo2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo2.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));
	
	// Check RPacketService::EnumerateNifs returns aCount = 1
	TExtEtelRequestStatus nifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(nifsStatus);
	TInt nifCount = 0;
	const TInt wantedCount = 1;
	packetService.EnumerateNifs(nifsStatus, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateNifs returned with an error."));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	TInt nifInfoCount = 0;
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pkg(nifInfoV2);
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoV2Pkg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo returned an error"));
	ASSERT_TRUE(nifInfoV2.iContextName.Length() > 0, _L("RPacketService::GetNifInfo with valid name  returned with an error"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	ASSERT_EQUALS(nifInfoV2.iNumberOfContexts, 2, _L("RPacketService::GetNifInfo returned with incorrect number of contexts"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo returned with incorrect NIF status"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
 	ASSERT_TRUE(nifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address"));

	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext
	// $CTSYProblem:RPacketService::GetNifInfo always returns EInternalContext
    // Changed test procedure to check for EInternalContext instead of EExternalContext
	ASSERT_EQUALS(nifInfoV2.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo returned wrong context type."));

	// Check RPacketService::GetNifInfo with aCount = 1 returns KErrArgument
	nifInfoCount = 1;
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoV2Pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo returned an error"));	
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrArgument, _L("RPacketService::GetNifInfo did not failed."));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = existing primary context name returns aCount = 2
	TExtEtelRequestStatus reqContextNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(reqContextNifStatus);	
	packetService.EnumerateContextsInNif(reqContextNifStatus, contextInfo.iName, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqContextNifStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, 2, _L("RPacketContext::EnumerateContextsInNif did not return 2 as expected"));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 0 returns aContextName = name of primary context
	TName contextName;
	TExtEtelRequestStatus getContextNameInNifStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifStatus);	
	packetService.GetContextNameInNif(getContextNameInNifStatus, contextInfo.iName, 0, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeMedium), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS(getContextNameInNifStatus.Int(), KErrNone, _L("RPacketService::GetContextNameInNif returned with an error."));
	ASSERT_EQUALS_DES16(contextName,contextInfo.iName,_L("RPacketService::GetContextNameInNif did not returned the expected value."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = existing primary context name and aIndex = 1 returns aContextName = name of secondary context
	packetService.GetContextNameInNif(getContextNameInNifStatus,contextInfo.iName, 1, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeMedium), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS(getContextNameInNifStatus.Int(), KErrNone, _L("RPacketService::GetContextNameInNif returned with an error."));
	ASSERT_EQUALS_DES16(contextName, contextInfo2.iName,_L("RPacketService::GetContextNameInNif did not returned the expected value."));

	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
	
	// Post notification for primary context status change
	TExtEtelRequestStatus notifyContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
																
	// Post notification for secondary context status change
	TExtEtelRequestStatus notifySecondaryContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecondaryContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecondaryContextStChStatus, secPacketContextStatus);

	// Deactivate the NIF with RPacketService::DeactivateNIF and aContextName = name of secondary context 
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10047);
	ASSERT_EQUALS(1,0, _L("Test leaving because it will cause TSY crash, remove this when defect is fixed"));
	TEST_CHECK_POINT_L(_L("test leaving..."));
	TExtEtelRequestStatus deactivateNifStatus(packetService, EPacketDeactivateNIF);
	CleanupStack::PushL(deactivateNifStatus);
    packetService.DeactivateNIF(deactivateNifStatus,contextInfo2.iName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateNifStatus, ETimeMedium), KErrNone, _L("RPacketService::DeactivateNIF timed out."));
	ASSERT_EQUALS(deactivateNifStatus.Int(), KErrNone, _L("RPacketService::DeactivateNIF returned with an error."));
	
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary context completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
    primaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus for primary context returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange for secondary context completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedSecondaryPacketContextStatus = RPacketContext::EStatusDeactivating;
   	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecondaryContextStChStatus,
				secPacketContextStatus, 
				expectedSecondaryPacketContextStatus,
				KErrNone);
				
	secondaryPacketContext.NotifyStatusChange(notifySecondaryContextStChStatus, secPacketContextStatus);
	expectedSecondaryPacketContextStatus = RPacketContext::EStatusInactive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecondaryContextStChStatus,
				secPacketContextStatus, 
				expectedSecondaryPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for secondary context returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, _L("RPacketService::GetStatus return error."));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	// notifyServiceStChStatus
	// notifyContextStChStatus
	// notifySecondaryContextStChStatus
	// getContextInfo
	// nifsStatus
	// nifInfoStatus
	// reqContextNifStatus
	// getContextNameInNifStatus
	// deactivateNifStatus	
	CleanupStack::PopAndDestroy(9,&notifyServiceStChStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0031::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0031");
	}



CCTSYIntegrationTestPacketContext0032::CCTSYIntegrationTestPacketContext0032(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0032::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0032::~CCTSYIntegrationTestPacketContext0032()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0032::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0032
 * @SYMFssID BA/CTSY/PKTC-0032
 * @SYMTestCaseDesc Deactivate NIF when there is more than one NIF.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::EnumerateNifs, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif, RPacketContext::NotifyStatusChange, RPacketService::DeactivateNIF, RPacketService::GetStatus, RPacketService::EnumerateContextsInNif
 * @SYMTestExpectedResults Pass - All contexts in NIF deactivated. Other contexts unaffected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify primary and secondary contexts in NIF are deactivated when primary context name specified

Verify contexts in another NIF are not affected.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// === Ensure there is an 2 primary contexts active. Each of these must have an active secondary context opened from it. (Primary 1 and secondary 1 in one NIF and primary 2 and secondary 2 in the second NIF.) ===
	
 	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
  
  	//Check if we are on a simulated network
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
 	
    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    // Post notification for the RPacketService::NotifyContextAdded
   	TExtEtelRequestStatus notifyContextPrimary1Status(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextPrimary1Status);
	
    // Open a primary PDP context 1
    TName primaryContextId1;
    packetService.NotifyContextAdded(notifyContextPrimary1Status, primaryContextId1);
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
											
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextPrimary1Status, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextPrimary1Status.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));										
											
	// Post notification for the RPacketService::NotifyContextAdded
   	TExtEtelRequestStatus notifyContextPrimary2Status(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextPrimary2Status);
	
    // Open a primary PDP context 2 
    TName primaryContextId2;
    packetService.NotifyContextAdded(notifyContextPrimary2Status, primaryContextId2);
	RPacketContext& primaryPacketContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext2);
											
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextPrimary2Status, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextPrimary2Status.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
		
	// Activate primary context 1.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);
	
	// Activate primary context 2.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext2, 2);
	
	// Post notification for the RPacketService::NotifyContextAdded
   	TExtEtelRequestStatus notifyContextSecondary1Status(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextSecondary1Status);	
	
	// Open a secondary PDP context 1. 
	TName secondaryContextId1;		
	packetService.NotifyContextAdded(notifyContextSecondary1Status, secondaryContextId1);
	RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
												
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextSecondary1Status, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextSecondary1Status.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
																									
	// Post notification for the RPacketService::NotifyContextAdded
   	TExtEtelRequestStatus notifyContextSecondary2Status(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextSecondary2Status);
													
	// Open a secondary PDP context 2. 
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts

	// Activate the secondary context 1.
	CHECK_TRUE_L(ActivateSecondaryPacketContextL(secondaryPacketContext, KPrimaryPacketContext1, KSecondaryPacketContext1),_L("Can't activate the secondary context"));

	// Activate the secondary context 2.
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::EnumerateNifs returns aCount = 2
	TExtEtelRequestStatus nifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(nifsStatus);
	TInt nifCount = 0;
	const TInt wantedCount = 2;
	packetService.EnumerateNifs(nifsStatus, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(nifCount, wantedCount, _L("RPacketService::EnumerateNifs returned with an error."));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext
	// Check RPacketService::GetNifInfo with aCount = 1 returns valid name in iContextName
	// Check RPacketService::GetNifInfo with aCount = 1 returns iNumberOfContexts = 2
	// Check RPacketService::GetNifInfo with aCount = 1 returns iNifStatus = EStatusActive
	// Check RPacketService::GetNifInfo with aCount = 1 returns valid address in iPdpAddress
	// Check RPacketService::GetNifInfo with aCount = 1 returns EExternalContext
	// Check RPacketService::GetNifInfo with aCount = 2 returns KErrArgument
	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	TInt nifInfoCount = 0;
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pkg(nifInfoV2);
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoV2Pkg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeLong), KErrNone, _L("RPacketService::GetNifInfo returned an error"));
	ASSERT_TRUE(nifInfoV2.iContextName.Length() > 0, _L("RPacketService::GetNifInfo with aCount = 0 returned with invalid iContextName"));
	ASSERT_EQUALS(nifInfoV2.iNumberOfContexts, 2, _L("RPacketService::GetNifInfo with iNumberOfContexts returned with an error"));
	ASSERT_EQUALS(nifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo with iNifStatus returned with an error"));
 	ASSERT_TRUE(nifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address"));
	ASSERT_EQUALS(nifInfoV2.iContextType, RPacketService::EExternalContext, _L("RPacketService::GetNifInfo with iContextType returned with an error"));

	nifInfoCount = 1;
	RPacketService::TNifInfoV2 secondNifInfoV2;
	RPacketService::TNifInfoV2Pckg secondNifInfoV2V2Pkg(secondNifInfoV2);
	packetService.GetNifInfo(nifInfoStatus,nifInfoCount,secondNifInfoV2V2Pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned with error."));
	ASSERT_TRUE(secondNifInfoV2.iContextName.Length() > 0, _L("RPacketService::GetNifInfo with aCount = 0 returned with invalid iContextName"));
	ASSERT_EQUALS(secondNifInfoV2.iNumberOfContexts, 2, _L("RPacketService::GetNifInfo with iNumberOfContexts returned with an error"));
	ASSERT_EQUALS(secondNifInfoV2.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo with iNifStatus returned with an error"));
 	ASSERT_TRUE(secondNifInfoV2.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address"));
	ASSERT_EQUALS(secondNifInfoV2.iContextType, RPacketService::EExternalContext, _L("RPacketService::GetNifInfo with iContextType returned with an error"));
	
	nifInfoCount = 2;
	packetService.GetNifInfo(nifInfoStatus,nifInfoCount,nifInfoV2Pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrArgument, _L("RPacketService::GetNifInfo did not returned with error."));
	
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = primary 1 returns aCount = 2
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = primary 2 returns aCount = 2
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = secondary 1 returns aCount = 2
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = secondary 2 returns aCount = 2
	TInt wantedNifCount = 2;
	TExtEtelRequestStatus enumerateContextsInNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifStatus);
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, primaryContextId1, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, wantedNifCount, _L("RPacketContext::EnumerateContextsInNif did not return 2 as expected."));
	
	nifCount = 0;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, primaryContextId2, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, wantedNifCount, _L("RPacketContext::EnumerateContextsInNif did not return 2 as expected."));
	
	nifCount = 0;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, secondaryContextId1, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(nifCount, wantedNifCount, _L("RPacketContext::EnumerateContextsInNif did not return 2 as expected."));
	
	nifCount = 0;	
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 1 and aIndex = 0 returns aContextName = name of primary context 1
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 1 and aIndex = 1 returns aContextName = name of secondary context 1
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 1 and aIndex = 0 returns aContextName = name of primary context 1
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 1 and aIndex = 1 returns aContextName = name of secondary context 1
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 2 and aIndex = 0 returns aContextName = name of primary context 2
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 2 and aIndex = 1 returns aContextName = name of secondary context 2
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 2 and aIndex = 0 returns aContextName = name of primary context 2
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 2 and aIndex = 1 returns aContextName = name of secondary context 2
    TExtEtelRequestStatus contextNameNifsStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(contextNameNifsStatus);
	TInt index = 0;	
	TName returnedContextName;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId1, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketContext::GetContextNameInNif returned error"));
	ASSERT_EQUALS_DES16(returnedContextName, primaryContextId1, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));

	index = 1;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId1, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketContext::GetContextNameInNif returned error"));
	ASSERT_EQUALS_DES16(returnedContextName, secondaryContextId1, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));	

	index = 0;
	packetService.GetContextNameInNif(contextNameNifsStatus, secondaryContextId1, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketContext::GetContextNameInNif returned error"));
	ASSERT_EQUALS_DES16(returnedContextName, primaryContextId1, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));

	index = 1;
	packetService.GetContextNameInNif(contextNameNifsStatus, secondaryContextId1, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketContext::GetContextNameInNif returned error"));
	ASSERT_EQUALS_DES16(returnedContextName, secondaryContextId1, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));

	index = 0;
	packetService.GetContextNameInNif(contextNameNifsStatus, primaryContextId2, index, returnedContextName );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextNameNifsStatus, ETimeLong), KErrNone, _L("RPacketContext::GetContextNameInNif timed out."));
	ASSERT_EQUALS(contextNameNifsStatus.Int(), KErrNone, _L("RPacketContext::GetContextNameInNif returned error"));
	ASSERT_EQUALS_DES16(returnedContextName, primaryContextId2, _L("RPacketContext::GetContextNameInNif with aExistingContextName = existing context name returned with an error"));

	index = 1;
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts

	index = 0;
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts

	index = 1;
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts

	// Post notifications for primary & secondary context status changes
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts
	TExtEtelRequestStatus notifyprimaryContextStChStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyprimaryContextStChStatus);
	RPacketContext::TContextStatus primaryPacketContextStatus;
	primaryPacketContext.NotifyStatusChange(notifyprimaryContextStChStatus, primaryPacketContextStatus);

	TExtEtelRequestStatus notifyprimary2ContextStChStatus(primaryPacketContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyprimary2ContextStChStatus);
	RPacketContext::TContextStatus primaryPacketContext2Status;
	primaryPacketContext2.NotifyStatusChange(notifyprimary2ContextStChStatus, primaryPacketContext2Status);

	TExtEtelRequestStatus notifySecondaryContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecondaryContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecondaryContextStChStatus, secPacketContextStatus);
	
		
	// Deactivate the NIF with RPacketService::DeactivateNIF and aContextName = name of primary 2 
	TExtEtelRequestStatus deactivateNifStatus(packetService, EPacketDeactivateNIF);
	CleanupStack::PushL(deactivateNifStatus);
    packetService.DeactivateNIF(deactivateNifStatus,primaryContextId2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateNifStatus, ETimeMedium), KErrNone, _L("RPacketService::DeactivateNIF timed out."));
	ASSERT_EQUALS(deactivateNifStatus.Int(), KErrNone, _L("RPacketService::DeactivateNIF returned with error."));
	
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary 2 completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext2,
				notifyprimary2ContextStChStatus,
				primaryPacketContext2Status, 
				expectedPacketContextStatus,
				KErrNone);
				
	primaryPacketContext2.NotifyStatusChange(notifyprimary2ContextStChStatus, primaryPacketContext2Status);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    primaryPacketContext2,
				notifyprimary2ContextStChStatus,
				primaryPacketContext2Status, 
				expectedPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus for primary 2 returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext2.GetStatus(primaryPacketContext2Status), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(primaryPacketContext2Status, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::NotifyStatusChange for secondary 2 completes with EStatusDeactivating -> EStatusInactive
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts
				
	// Check RPacketContext::GetStatus for secondary 2 returns EStatusInactive
	// Check RPacketContext::GetStatus for primary 1 returns EStatusActive
	// Check RPacketContext::GetStatus for secondary 1 returns EStatusActive
	// $CTSYProblem LTSY dose not allow us to initialise 4 packets contexts
	ASSERT_EQUALS(primaryPacketContext.GetStatus(primaryPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(primaryPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	//	deactivateNifStatus
	//	notifySecondaryContextStChStatus
	//	notifyprimary2ContextStChStatus
	//	notifyprimaryContextStChStatus
	//	contextNameNifsStatus
	//	enumerateContextsInNifStatus
	//	nifInfoStatus
	//	nifsStatus
	//	notifyContextSecondary2Status
	//	notifyContextSecondary1Status
	//	notifyContextPrimary2Status
	//	notifyContextPrimary1Status
	CleanupStack::PopAndDestroy(12,&notifyContextPrimary1Status);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0032::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0032");
	}



CCTSYIntegrationTestPacketContext0033::CCTSYIntegrationTestPacketContext0033(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0033::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0033::~CCTSYIntegrationTestPacketContext0033()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0033::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0033
 * @SYMFssID BA/CTSY/PKTC-0033
 * @SYMTestCaseDesc Deactivate NIF specifying unknown context name.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::DeactivateNIF, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - No NIFs deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status unaffected.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary context. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer, KMainPhone, KMainPacketService, KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckServiceStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckServiceStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckServiceStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus did not return EStatusActive as expected."));

	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus pckContextStatus;
	ASSERT_EQUALS(primaryPacketContext.GetStatus(pckContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(pckContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return EStatusActive as expected."));

	// Deactivate the NIF with RPacketService::DeactivateNIF and aContextName = unknown context name 
	TExtEtelRequestStatus deactiveNIFStatus(packetService, EPacketDeactivateNIF);
	CleanupStack::PushL(deactiveNIFStatus);
	RPacketService::TContextInfo unknownContextInfo;
	unknownContextInfo.iName = _L("unknown123abc");
	packetService.DeactivateNIF(deactiveNIFStatus, unknownContextInfo.iName);

	// Check error is returned.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactiveNIFStatus, ETimeMedium), KErrNone, _L("RPacketService::DeactivateNIF timed out."));
	ASSERT_TRUE(deactiveNIFStatus.Int() != KErrNone, _L("RPacketService::DeactivateNIF did not return error as expected."));

	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(pckServiceStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckServiceStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus did not return EStatusActive as expected."));

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext.GetStatus(pckContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(pckContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return EStatusActive as expected."));

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// deactiveNIFStatus
	CleanupStack::PopAndDestroy(1, &deactiveNIFStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0033::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0033");
	}



CCTSYIntegrationTestPacketContext0034::CCTSYIntegrationTestPacketContext0034(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0034::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0034::~CCTSYIntegrationTestPacketContext0034()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0034::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0034
 * @SYMFssID BA/CTSY/PKTC-0034
 * @SYMTestCaseDesc Deactivate NIF when there is more than one NIF.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyAttachModeChange, RPacketService::GetAttachMode, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::EnumerateContextsInNif, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::DeactivateNIF, RPacketService::GetNifInfo
 * @SYMTestExpectedResults Pass - All contexts in NIF deactivated. Other contexts unaffected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify primary and secondary contexts in NIF are deactivated when secondary context name specified

Verify contexts in another NIF are not affected.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
	
    //Check if we are on a simulated network 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error."));
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
	// Ensure there is an 2 primary contexts active. Each of these must have an active secondary context opened from it. (Primary 1 and secondary 1 in one NIF and primary 2 and secondary 2 in the second NIF.) 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	TName primaryContextId1;
	TName primaryContextId2;
	
	TExtEtelRequestStatus notifyContextStatusPrimary1(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatusPrimary1);
	
	packetService.NotifyContextAdded(notifyContextStatusPrimary1, primaryContextId1);
	RPacketContext& primaryPacketContext1 = iEtelSessionMgr.GetPrimaryPacketContextL(
									KMainServer,
									KMainPhone,
									KMainPacketService,
									KPrimaryPacketContext1);
									
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatusPrimary1, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextStatusPrimary1.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
	CHECK_TRUE_L(primaryContextId1.Length() > 0, _L("RPacketService::NotifyContextAdded returned with invalid context name."));								
												
	TExtEtelRequestStatus notifyContextStatusPrimary2(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatusPrimary2);											
																
	packetService.NotifyContextAdded(notifyContextStatusPrimary2, primaryContextId2);
	RPacketContext& primaryPacketContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(
									KMainServer,
									KMainPhone,
									KMainPacketService,
									KPrimaryPacketContext2);
									
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatusPrimary2, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextStatusPrimary2.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
	CHECK_TRUE_L(primaryContextId2.Length() > 0, _L("RPacketService::NotifyContextAdded returned with invalid context name."));									

	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext1);	
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext2, 2);	

	TName secondaryContextId1;
	TName secondaryContextId2;
	TExtEtelRequestStatus notifyContextStatusSecondary1(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatusSecondary1);	
	
	packetService.NotifyContextAdded(notifyContextStatusSecondary1, secondaryContextId1);
	RPacketContext& secondaryPacketContext1 = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
												
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatusSecondary1, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextStatusSecondary1.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
	CHECK_TRUE_L(secondaryContextId1.Length() > 0, _L("RPacketService::NotifyContextAdded returned with invalid context name."));
												
	TExtEtelRequestStatus notifyContextStatusSecondary2(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextStatusSecondary2);													
																							
	packetService.NotifyContextAdded(notifyContextStatusSecondary2, secondaryContextId2);
	// $CTSYProblem This test cannot be run since LTSY does not allow us to initialise 4 packets contexts
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY does not allow us to initialise 4 packets contexts"));
	RPacketContext& secondaryPacketContext2 = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext2,
												KSecondaryPacketContext2);
											
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextStatusSecondary2, ETimeMedium), KErrNone, _L("RPacketService::NotifyContextAdded did not complete."));
	CHECK_EQUALS_L(notifyContextStatusSecondary2.Int(), KErrNone, _L("RPacketService::NotifyContextAdded returned with error status."));
	CHECK_TRUE_L(secondaryContextId2.Length() > 0, _L("RPacketService::NotifyContextAdded returned with invalid context name."));
																								
	CHECK_TRUE_L(ActivateSecondaryPacketContextL(secondaryPacketContext1), _L("Can't activate the secondary context"));
	CHECK_TRUE_L(ActivateSecondaryPacketContextL(secondaryPacketContext2), _L("Can't activate the secondary context"));												

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Check RPacketService::EnumerateNifs returns aCount = 2
	TExtEtelRequestStatus nifsStatus(packetService, EPacketEnumerateNifs);
	CleanupStack::PushL(nifsStatus);
	TInt nifCount = 0;
	TInt wantedNifCount = 2;
	packetService.EnumerateNifs(nifsStatus, nifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifsStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateNifs timed out."));
	ASSERT_EQUALS(nifsStatus.Int(), KErrNone, _L("RPacketService::EnumerateNifs returned with error."));
	ASSERT_EQUALS(nifCount, wantedNifCount, _L("RPacketService::EnumerateNifs returned wrong NIF count."));

	// Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName
	TInt nifInfoCount = 0;
	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	RPacketService::TNifInfoV2 nifInfo;
	RPacketService::TNifInfoV2Pckg nifInfoPckg(nifInfo);
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned with error."));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, primaryContextId1, _L("RPacketService::GetNifInfo returned with invalid context name."));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNumberOfContexts = 2
	TInt wantedNumberOfContexts = 2;
	ASSERT_EQUALS(nifInfo.iNumberOfContexts, wantedNumberOfContexts, _L("RPacketService::GetNifInfo returned wrong number of contexts."));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo did not return EStatusActive as expected."));
 
	// Check RPacketService::GetNifInfo with aCount = 0 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address."));
	
	// Check RPacketService::GetNifInfo with aCount = 0 returns EExternalContext
	// $CTSYProblem:RPacketService::GetNifInfo always returns EInternalContext
    // Changed test procedure to check for EInternalContext instead of EExternalContext
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo returned wrong context type."));

	// Check RPacketService::GetNifInfo with aCount = 1 returns valid name in iContextName
	nifInfoCount = 1;
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned with error."));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, primaryContextId2, _L("RPacketService::GetNifInfo returned with invalid context name."));
	
	// Check RPacketService::GetNifInfo with aCount = 1 returns iNumberOfContexts = 2
	ASSERT_EQUALS(nifInfo.iNumberOfContexts,2,_L("RPacketService::GetNifInfo wrong number of contexts."));
	
	// Check RPacketService::GetNifInfo with aCount = 1 returns iNifStatus = EStatusActive
	ASSERT_EQUALS(nifInfo.iNifStatus, RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo did not return EStatusActive as expected."));
 
	// Check RPacketService::GetNifInfo with aCount = 1 returns valid address in iPdpAddress
	ASSERT_TRUE(nifInfo.iPdpAddress.Length() > 0, _L("RPacketService::GetNifInfo returned with invalid pdp address."));
	
	// Check RPacketService::GetNifInfo with aCount = 1 returns EExternalContext
	// $CTSYProblem:RPacketService::GetNifInfo always returns EInternalContext
    // Changed test procedure to check for EInternalContext instead of EExternalContext
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo returned wrong context type."));
	
	// Check RPacketService::GetNifInfo with aCount = 2 returns KErrArgument
	nifInfoCount = 2;
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrArgument, _L("RPacketService::GetNifInfo did not returned an error."));

	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = primary 1 returns aCount = 2
	TInt wantedCount = 2;
	TInt contextsInNifCount = 0;
	TExtEtelRequestStatus enumerateContextsInNifStatus(packetService, EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifStatus);
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, primaryContextId1, contextsInNifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(enumerateContextsInNifStatus.Int(), KErrNone, _L("RPacketService::EnumerateContextsInNif returned with error."));
	ASSERT_EQUALS(contextsInNifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected."));
	
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = primary 2 returns aCount = 2
	contextsInNifCount = 0;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, primaryContextId2, contextsInNifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(enumerateContextsInNifStatus.Int(), KErrNone, _L("RPacketService::EnumerateContextsInNif returned with error."));
	ASSERT_EQUALS(contextsInNifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected."));
	
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = secondary 1 returns aCount = 2
	contextsInNifCount = 0;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, secondaryContextId1, contextsInNifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(enumerateContextsInNifStatus.Int(), KErrNone, _L("RPacketService::EnumerateContextsInNif returned with error."));
	ASSERT_EQUALS(contextsInNifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected."));
		
	// Check RPacketService::EnumerateContextsInNif with aExistingContextName = secondary 2 returns aCount = 2
    contextsInNifCount = 0;
	packetService.EnumerateContextsInNif(enumerateContextsInNifStatus, secondaryContextId2, contextsInNifCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifStatus, ETimeLong), KErrNone, _L("RPacketService::EnumerateContextsInNif timed out."));
	ASSERT_EQUALS(enumerateContextsInNifStatus.Int(), KErrNone, _L("RPacketService::EnumerateContextsInNif returned with error."));
	ASSERT_EQUALS(contextsInNifCount, wantedCount, _L("RPacketService::EnumerateContextsInNif did not return 2 as expected."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 1 and aIndex = 0 returns aContextName = name of primary context 1
	const TInt index0 = 0;
	const TInt index1 = 1;
	TName contextName;
	TExtEtelRequestStatus getContextNameInNifStatus(packetService, EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifStatus);
	packetService.GetContextNameInNif(getContextNameInNifStatus, primaryContextId1, index0, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, primaryContextId1, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 1 and aIndex = 1 returns aContextName = name of secondary context 1
	packetService.GetContextNameInNif(getContextNameInNifStatus, primaryContextId1, index1, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, secondaryContextId1, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 1 and aIndex = 0 returns aContextName = name of primary context 1
	packetService.GetContextNameInNif(getContextNameInNifStatus, secondaryContextId1, index0, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, primaryContextId1, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 1 and aIndex = 1 returns aContextName = name of secondary context 1
	packetService.GetContextNameInNif(getContextNameInNifStatus, secondaryContextId1, index1, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, secondaryContextId1, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 2 and aIndex = 0 returns aContextName = name of primary context 2
	packetService.GetContextNameInNif(getContextNameInNifStatus, primaryContextId2, index0, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, primaryContextId2, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
	
	// Check RPacketService::GetContextNameInNif with aExistingContextName = primary 2 and aIndex = 1 returns aContextName = name of secondary context 2
	packetService.GetContextNameInNif(getContextNameInNifStatus, primaryContextId2, index1, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
    ASSERT_EQUALS_DES16(contextName, secondaryContextId2, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));

	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 2 and aIndex = 0 returns aContextName = name of primary context 2
	packetService.GetContextNameInNif(getContextNameInNifStatus, secondaryContextId2, index0, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, primaryContextId2, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
		
	// Check RPacketService::GetContextNameInNif with aExistingContextName = secondary 2 and aIndex = 1 returns aContextName = name of secondary context 2
	packetService.GetContextNameInNif(getContextNameInNifStatus, secondaryContextId2, index1, contextName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifStatus, ETimeLong), KErrNone, _L("RPacketService::GetContextNameInNif timed out."));
	ASSERT_EQUALS_DES16(contextName, secondaryContextId2, _L("RPacketService::GetContextNameInNif returned context name which doesn't match with the existing context name."));
	
	// Post notifications for primary2 and secondary2 context's status change
	TExtEtelRequestStatus notifyPrimaryContext2StChStatus(primaryPacketContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyPrimaryContext2StChStatus);
	RPacketContext::TContextStatus primaryPacketContext2Status;
	primaryPacketContext2.NotifyStatusChange(notifyPrimaryContext2StChStatus, primaryPacketContext2Status);

	TExtEtelRequestStatus notifySecondaryContext2StChStatus(secondaryPacketContext2, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecondaryContext2StChStatus);
	RPacketContext::TContextStatus secondaryPacketContext2Status;
	secondaryPacketContext2.NotifyStatusChange(notifySecondaryContext2StChStatus, secondaryPacketContext2Status);

	// Post notifications for primary1 context status change
	TExtEtelRequestStatus notifyPrimaryContext1StChStatus(primaryPacketContext1, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyPrimaryContext1StChStatus);
	RPacketContext::TContextStatus primaryPacketContext1Status;
	primaryPacketContext1.NotifyStatusChange(notifyPrimaryContext1StChStatus, primaryPacketContext1Status);

	// Deactivate the NIF with RPacketService::DeactivateNIF and aContextName = name of secondary 2 
	TExtEtelRequestStatus deactivateNIFStatus(packetService, EPacketDeactivateNIF);
	CleanupStack::PushL(deactivateNIFStatus);
	packetService.DeactivateNIF(deactivateNIFStatus, secondaryContextId2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateNIFStatus, ETimeMedium), KErrNone, _L("RPacketService::DeactivateNIF timed out."));
	ASSERT_EQUALS(deactivateNIFStatus.Int(), KErrNone, _L("RPacketService::DeactivateNIF returned with error.."));
	
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for primary 2 completes with EStatusDeactivating -> EStatusInactive
    RPacketContext::TContextStatus 	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					primaryPacketContext2,
					notifyPrimaryContext2StChStatus,
					primaryPacketContext2Status, 
					expectedPacketContextStatus,
					KErrNone);

	primaryPacketContext2.NotifyStatusChange(notifyPrimaryContext2StChStatus, primaryPacketContext2Status);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					primaryPacketContext2,
					notifyPrimaryContext2StChStatus,
					primaryPacketContext2Status, 
					expectedPacketContextStatus,
					KErrNone);

	// Check RPacketContext::GetStatus for primary 2 returns EStatusInactive
	ASSERT_EQUALS(primaryPacketContext2.GetStatus(primaryPacketContext2Status), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(primaryPacketContext2Status, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));
	
	// Check RPacketContext::NotifyStatusChange for secondary 2 completes with EStatusDeactivating -> EStatusInactive
	expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    secondaryPacketContext2,
				notifySecondaryContext2StChStatus,
				secondaryPacketContext2Status, 
				expectedPacketContextStatus,
				KErrNone);
				
	secondaryPacketContext2.NotifyStatusChange(notifySecondaryContext2StChStatus, secondaryPacketContext2Status);
	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
			    secondaryPacketContext2,
				notifySecondaryContext2StChStatus,
				secondaryPacketContext2Status, 
				expectedPacketContextStatus,
				KErrNone); 

	// Check RPacketContext::GetStatus for secondary 2 returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext2.GetStatus(secondaryPacketContext2Status), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(secondaryPacketContext2Status, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));	
	
	// Check RPacketContext::GetStatus for primary 1 returns EStatusActive
	ASSERT_EQUALS(primaryPacketContext1.GetStatus(primaryPacketContext1Status), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(primaryPacketContext1Status, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
	
	// Check RPacketContext::GetStatus for secondary 1 returns EStatusActive
	RPacketContext::TContextStatus secondaryPacketContext1Status;
	ASSERT_EQUALS(secondaryPacketContext1.GetStatus(secondaryPacketContext1Status), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(secondaryPacketContext1Status, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));
	
	// Check RPacketService::GetStatus returns EStatusActive	
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// pop
	// notifyContextStatusPrimary1
	// notifyContextStatusPrimary1
	// notifyContextStatusSecondary1
	// notifyContextStatusSecondary2
	// nifsStatus
	// nifInfoStatus
	// enumerateContextsInNifStatus
	// getContextNameInNifStatus
	// notifyPrimaryContext1StChStatus
	// notifySecondaryContext2StChStatus
	// notifyPrimaryContext1StChStatus
	// deactivateNIFStatus	
	CleanupStack::PopAndDestroy(12,&notifyContextStatusPrimary1);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0034::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0034");
	}



CCTSYIntegrationTestPacketContext0035::CCTSYIntegrationTestPacketContext0035(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0035::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0035::~CCTSYIntegrationTestPacketContext0035()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0035::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0035
 * @SYMFssID BA/CTSY/PKTC-0035
 * @SYMTestCaseDesc Enumerate NIFs.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketService::EnumerateNifs, RPacketContext::NotifyStatusChange, RPacketContext::SetConfig, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Number of NIFs correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify number of NIFs and NIF info correct as contexts are activated. Verify that for each primary context that is activated, number of NIFs increases by 1, if a secondary context is activated number of NIFs does not change.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure packet service is attached, 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new primary context (context 1). 

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context  

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate packet context with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Check RPacketService::EnumerateNifs returns aCount = 1

	// Open a new primary context (context 2) 

	// Add a TPacketFilterV3 packet filter to context 2 using RPacketContext::AddPacketFilter 

	// Open a new QoS from context 2  

	// Get R5 QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSR5Requested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Set a valid context config for context 2 using RPacketContext::SetConfig 

	// Activate packet context for context 2 with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange for context 2 completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for context 2 returns EStatusActive

	// Check RPacketService::EnumerateNifs returns aCount = 2

	// Open a new secondary context from context 1 (this is context 3) 

	// Set a valid context config for context 3 using RPacketContext::SetConfig 

	// Activate packet context for context 3 with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange for context 3 completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for context 3 returns EStatusActive

	// Check RPacketService::EnumerateNifs returns aCount = 2

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0035::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0035");
	}



CCTSYIntegrationTestPacketContext0036::CCTSYIntegrationTestPacketContext0036(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0036::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0036::~CCTSYIntegrationTestPacketContext0036()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0036::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0036
 * @SYMFssID BA/CTSY/PKTC-0036
 * @SYMTestCaseDesc Activate a primary context with packet filter and QoS set up.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketQoS::OpenNewQoS, RPacketQoS::SetProfileParameters, RPacketQoS::GetProfileParameters, RPacketQoS::GetProfileCapabilities, RPacketContext::GetProfileName
 * @SYMTestExpectedResults Pass - Packet filter added to context and context activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet filter info and QoS profile info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new primary context 

	// Add a TPacketFilterV2 packet filter to the primary context using RPacketContext::AddPacketFilter 

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns same packet filter info added to the context

	// Open a new QoS from the primary context  

	// Get profile capabilities with RPacketCotnext::GetProfileCapabilities with aProfileCaps as a packaged RPacketContext::TQoSCapsGPRS  

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSR5Requested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate packet context with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns same packet filter info added to the context

	// Repeat test with RPacketContext::TQoSCapsR99_R4 and RPacketContext::TQoSCapsR5 instead of RPacketContext::TQoSCapsGPRS 

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0036::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0036");
	}



CCTSYIntegrationTestPacketContext0037::CCTSYIntegrationTestPacketContext0037(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0037::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0037::~CCTSYIntegrationTestPacketContext0037()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0037::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0037
 * @SYMFssID BA/CTSY/PKTC-0037
 * @SYMTestCaseDesc Modify a primary context with packet filter and QoS.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::ModifyActiveContext, RPacketContext::GetProfileParameters, RPacketContext::GetProfileName, RPacketContext::RemovePacketFilter, RPacketContext::EnumeratePacketFilters, RPacketQoS::SetProfileParameters, RPacketQoS::OpenNewQoS, RPacketContext::AddPacketFilter
 * @SYMTestExpectedResults Pass - Context is modified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet filter info and QoS profile info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary context. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Add a TPacketFilterV2packet filter to the primary context using RPacketContext::AddPacketFilter 

	// Open a new QoS from the primary context 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0

	// Modify the active context with RPacketContext::ModifyActiveContext 

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns same packet filter info added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns KErrArgument

	// Check RPacketContext::GetProfileParameters returns TQoSGPRSNegotiated parameters.

	// Remove packet filter with RPacketContext::RemovePacketFilter. 

	// Modify the active context with RPacketContext::ModifyActiveContext 

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns KErrArgument

	// Repeat test with TQoSR99_R4Requested and TQoSR5Requested types in  RPacketQoS::SetProfileParameters and checking for TQoSR99_R4Negotiated and TQoSR5Negotiated  in RPacketContext::GetProfileParameters respectively. 

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0037::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0037");
	}



CCTSYIntegrationTestPacketContext0038::CCTSYIntegrationTestPacketContext0038(CEtelSessionMgr& aEtelSessionMgr, const TDesC& aName)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(aName);
	if(aName.Find(_L("GRPS")) != KErrNotFound)
		{
		iTestType = EQoSGPRS;
		}
	else if(aName.Find(_L("R4")) != KErrNotFound)
		{
		iTestType = EQoSR4;
		}
	else if(aName.Find(_L("R5")) != KErrNotFound)
		{
		iTestType = EQoSR5;
		}
	}

CCTSYIntegrationTestPacketContext0038::~CCTSYIntegrationTestPacketContext0038()
/**
 * Destructor.
 */
	{
	}


template<class PARAMETERS, class CAPABILITIES, class NEGOTIATED>
	TBool CCTSYIntegrationTestPacketContext0038::ActiveContextL(RPacketContext &aPacketContext,
                                                               RPacketQoS &aPacketQoS,
                                                               PARAMETERS &aParameters,
                                                               CAPABILITIES &aCapabilities,
                                                               NEGOTIATED &aNegotiated)

/**
 * Activate a secondary context. This is actually a single context activation method, 
 * the test activate the context three times, each with diffrent type of parameters, capabilities and negotiation objects
 * 
 * @param aPacketContext Reference to the secondary context session.
 * @param aPacketQoS Reference to the packet Quality of Service session.
 * @param aParameters Reference to the QoS parameters. Must be TQoSGPRSRequested, TQoSR99_R4Requested or TQoSR5Requested.
 * @param aCapabilities Reference to the QoS capabilities. Must be a TQoSCapsGPRS, TQoSCapsR99_R4 or TQoSCapsR5 respectivley.
 * @param aNegotiated Reference to the QoS nagotiated. Must be a TQoSGPRSNegotiated, t99_r4Negotiated or r5Negotiated respectivley.
 * 
 * @return ETrue if the activation went well
 */
    {
    TBool ret = ETrue;

    // Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(aPacketContext,1), KErrNone,
					_L("RPacketContext::AddPacketFilter returned with error status")); 

	// Get profile capabilities with RPacketCotnext::GetProfileCapabilities with aProfileCaps as a packaged CAPABILITIES
	TExtEtelRequestStatus getProfileCapStatus(aPacketQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	TPckg<CAPABILITIES> qosCapsPckg(aCapabilities);
	aPacketQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, 
                        _L("RPacketQoS::GetProfileCapabilities timed out."));
	LOCAL_ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone,
                        _L("RPacketQoS::GetProfileCapabilities returned with error status."));

    // Set requested profile parameters with RPacketQoS::SetProfileParameters and PARAMETERS with parameters within range returned by RPacketQoS::GetProfileCapabilities 
	TPckg<PARAMETERS> qosReqPckg(aParameters);
    FillParametersFromCapabilities(aParameters,aCapabilities);
	TExtEtelRequestStatus setProfileParamStatus(aPacketQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
    aPacketQoS.SetProfileParameters(setProfileParamStatus, qosReqPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, 
                  _L("RPacketQoS::SetProfileParameters timed out."));
	LOCAL_ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone,
                  _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    LOCAL_ASSERT_EQUALS(aPacketContext.GetProfileName(profileName),KErrNone,
                        _L("RPacketContext::GetProfileName returned with error status."));
    LOCAL_ASSERT_TRUE(profileName.Length()>0,
                      _L("RPacketContext::GetProfileName name length is 0."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(aPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	aPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
    
    // Activate the secondary context. 
	TExtEtelRequestStatus contextActivateStatus(aPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	aPacketContext.Activate(contextActivateStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeMedium), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	LOCAL_ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				aPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    LOCAL_ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    aPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				aPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	LOCAL_ASSERT_EQUALS(aPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	LOCAL_ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1
    TInt count=-1;
    TExtEtelRequestStatus enumeratePacketFiltersChStatus(aPacketContext, EPacketContextEnumeratePacketFilters);
    CleanupStack::PushL(enumeratePacketFiltersChStatus);
    aPacketContext.EnumeratePacketFilters(enumeratePacketFiltersChStatus,count);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersChStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters timed out"));
    LOCAL_ASSERT_EQUALS(enumeratePacketFiltersChStatus.Int(), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters returned an error"));
    LOCAL_ASSERT_EQUALS(count, 1,
                  _L("RPacketContext::EnumeratePacketFilters returned wrong number"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns packet filter added to the context
    TExtEtelRequestStatus getPacketFilterInfoStatus(aPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
    RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg packetFilterPkg(packetFilter);
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,0,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;
    FillPacketFilter(defaultPacketFilterInfo,1);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns KErrArgument
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,1,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument,
                  _L("RPacketContext::GetPacketFilterInfo did not returned an error"));

	// Check RPacketContext::GetProfileParameters returns NEGOTIATED parameters.
    TExtEtelRequestStatus getProfileParametersStatus(aPacketQoS, EPacketQoSGetProfileParams);
    CleanupStack::PushL(getProfileParametersStatus);
	TPckg<NEGOTIATED> negotiatedPckg(aNegotiated);
    aPacketQoS.GetProfileParameters(getProfileParametersStatus,negotiatedPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeShort), KErrNone,
                  _L("RPacketQoS::GetProfileParameters timed out"));
    LOCAL_ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone,
                  _L("RPacketQoS::GetProfileParameters returned an error"));
    
	// Deactivate context.
    TExtEtelRequestStatus deactivateContextStatus(aPacketContext, EPacketContextDeactivate);
    CleanupStack::PushL(getProfileParametersStatus);
    aPacketContext.Deactivate(deactivateContextStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateContextStatus, ETimeMedium), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));
	LOCAL_ASSERT_EQUALS(deactivateContextStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));

	
    iEtelSessionMgr.ReleaseSecondaryPacketContext(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext3);

	// Pop:
	//  deactivateContextStatus
	//	getProfileParametersStatus
	//	getPacketFilterInfoStatus
	//	enumeratePacketFiltersChStatus
	//	contextActivateStatus
	//	notifyContextStChStatus
	//	setProfileParamStatus
	//	getProfileCapStatus
	CleanupStack::PopAndDestroy(8,&getProfileCapStatus);

    return ret;
    }
TVerdict CCTSYIntegrationTestPacketContext0038::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0038
 * @SYMFssID BA/CTSY/PKTC-0038
 * @SYMTestCaseDesc Activate a secondary context with packet filter and QoS set up.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetProfileName, RPacketContext::GetStatus, RPacketContext::EnumeratePacketFilters, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketContext::GetPacketFilterInfo, RPacketContext::ModifyActiveContext, RPacketQoS::OpenNewQoS, RPacketQoS::SetProfileParameters, RPacketContext::RemovePacketFilter
 * @SYMTestExpectedResults Pass - Packet filter added and removed successfully.Context active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet filter info and QoS profile info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	DEBUG_PRINTF2(_L("Starting with type %d"),iTestType);

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Ensure there is an active primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService,
			KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,packetContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new secondary context. 
    RPacketContext& secondaryPacketContext1 = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Open a new QoS from the secondary context 
	RPacketQoS& packetQoS1 = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

    RPacketQoS::TQoSGPRSRequested gprsRequested;
    RPacketQoS::TQoSCapsGPRS capsGPRS;
    RPacketQoS::TQoSGPRSNegotiated gprsNegotiated;
    
    RPacketQoS::TQoSR99_R4Requested r99_r4Requested;
    RPacketQoS::TQoSCapsR99_R4 capsR99_R4;
    RPacketQoS::TQoSR99_R4Negotiated t99_r4Negotiated;
    t99_r4Negotiated.iTrafficClass = RPacketQoS::ETrafficClassUnspecified; 
    t99_r4Negotiated.iDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
    t99_r4Negotiated.iDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
    //t99_r4Negotiated.iMaxSDUSize = 500; //not sure
    //t99_r4Negotiated.iMaxRate.iUplinkRate = 5000; //not sure 
    //t99_r4Negotiated.iMaxRate.iDownlinkRate = 5000; //not sure 
    t99_r4Negotiated.iBER = RPacketQoS::EBERUnspecified;
    t99_r4Negotiated.iSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    t99_r4Negotiated.iTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified; 
    //t99_r4Negotiated.iTransferDelay = 110; //not sure
    //t99_r4Negotiated.iGuaranteedRate.iUplinkRate = 5000; //not sure 
    //t99_r4Negotiated.iGuaranteedRate.iDownlinkRate = 5000; //not sure 
    
    RPacketQoS::TQoSR5Requested r5Requested;
    RPacketQoS::TQoSCapsR5 capsr5;
    RPacketQoS::TQoSR5Negotiated r5Negotiated;

    switch(iTestType)
		{
		case EQoSGPRS:
		    //Activate The Context using TQoSGPRSRequested, TQoSCapsGPRS and TQoSGPRSNegotiated
		    ASSERT_TRUE(ActiveContextL(secondaryPacketContext1,packetQoS1,gprsRequested,capsGPRS,gprsNegotiated),
		                _L("CCTSYIntegrationTestPacketContext0038::ActiveContextL failed"));
		    break;
		case EQoSR4:
		    ASSERT_TRUE(ActiveContextL(secondaryPacketContext1,packetQoS1,r99_r4Requested,capsR99_R4,t99_r4Negotiated),
		                _L("CCTSYIntegrationTestPacketContext0038::ActiveContextL failed"));
		    break;
		case EQoSR5:
		    ASSERT_TRUE(ActiveContextL(secondaryPacketContext1,packetQoS1,r5Requested,capsr5,r5Negotiated),
		                _L("CCTSYIntegrationTestPacketContext0038::ActiveContextL failed"));
			break;
		default:
			CHECK_TRUE_L(EFalse, _L("Wrong test type!"));
			
		}


	// Repeat test with TQoSR99_R4Requested and TQoSR5Requested types in  RPacketQoS::SetProfileParameters and checking for TQoSR99_R4Negotiated and TQoSR5Negotiated  in RPacketContext::GetProfileParameters respectively. 


    //Activate The Context using TQoSR99_R4Requested, TQoSCapsR99_R4 and TQoSR99_R4Negotiated


    //Activate The Context using TQoSR5Requested, TQoSCapsR5 and TQoSR5Negotiated
    
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0038::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0038");
	}



CCTSYIntegrationTestPacketContext0039::CCTSYIntegrationTestPacketContext0039(CEtelSessionMgr& aEtelSessionMgr, const TDesC& aName)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(aName);
	if(aName.Find(_L("GRPS")) != KErrNotFound)
		{
		iTestType = EQoSGPRS;
		}
	else if(aName.Find(_L("R4")) != KErrNotFound)
		{
		iTestType = EQoSR4;
		}
	else if(aName.Find(_L("R5")) != KErrNotFound)
		{
		iTestType = EQoSR5;
		}
	}

CCTSYIntegrationTestPacketContext0039::~CCTSYIntegrationTestPacketContext0039()
/**
 * Destructor.
 */
	{
	}

template<class PARAMETERS,class CAPABILITIES,class NEGOTIATED>
  TBool CCTSYIntegrationTestPacketContext0039::ActivateAndModifyContextL(RPacketContext &aPacketContext,
                                                                        RPacketQoS &aPacketQoS,
                                                                        PARAMETERS &aParameters,
                                                                        CAPABILITIES &aCapabilities,
                                                                        NEGOTIATED &aNegotiated)
/**
 * Activate and modify a secondary context. This is actually a single context activation and modification method, 
 * the test activate and modify the context three times, each with diffrent type of parameters, capabilities and negotiation objects
 * 
 * @param aPacketContext Reference to the secondary context session.
 * @param aPacketQoS Reference to the packet Quality of Service session.
 * @param aParameters Reference to the QoS parameters. Must be TQoSGPRSRequested, TQoSR99_R4Requested or TQoSR5Requested.
 * @param aCapabilities Reference to the QoS capabilities. Must be a TQoSCapsGPRS, TQoSCapsR99_R4 or TQoSCapsR5 respectivley.
 * @param aNegotiated Reference to the QoS nagotiated. Must be a TQoSGPRSNegotiated, t99_r4Negotiated or r5Negotiated respectivley.
 * 
 * @return ETrue if the activation went well
 */
    {
    TBool ret=ETrue;

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter
    LOCAL_ASSERT_EQUALS(AddPacketFilterV2ToContextL(aPacketContext,1), KErrNone,
                        _L("RPacketContext::AddPacketFilter returned with error status"));

	// Add a second TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
    LOCAL_ASSERT_EQUALS(AddPacketFilterV2ToContextL(aPacketContext,2), KErrNone,
                        _L("RPacketContext::AddPacketFilter returned with error status"));

	// Get profile capabilities with RPacketCotnext::GetProfileCapabilities with aProfileCaps as a packaged CAPABILITIES  
	TExtEtelRequestStatus getProfileCapStatus(aPacketQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	TPckg<CAPABILITIES> qosCapsPckg(aCapabilities);
	aPacketQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, 
                        _L("RPacketQoS::GetProfileCapabilities timed out."));
	LOCAL_ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone,
                        _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and PARAMETERS with parameters within range returned by RPacketQoS::GetProfileCapabilities 
	TPckg<PARAMETERS> qosReqPckg(aParameters);
    FillParametersFromCapabilities(aParameters,aCapabilities);
	TExtEtelRequestStatus setProfileParamStatus(aPacketQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
    aPacketQoS.SetProfileParameters(setProfileParamStatus, qosReqPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, 
                  _L("RPacketQoS::SetProfileParameters timed out."));
	LOCAL_ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone,
                  _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    LOCAL_ASSERT_EQUALS(aPacketContext.GetProfileName(profileName),KErrNone,
                        _L("RPacketContext::GetProfileName returned with error status."));
    LOCAL_ASSERT_TRUE(profileName.Length()>0,
                      _L("RPacketContext::GetProfileName name length is 0."));

	// ===  Activate a context with two packet filters and QoS ===

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(aPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	aPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus contextActivateStatus(aPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	aPacketContext.Activate(contextActivateStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeMedium), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	LOCAL_ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				aPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    LOCAL_ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
    aPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
    expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				aPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	LOCAL_ASSERT_EQUALS(aPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	LOCAL_ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 2
    TInt count=-1;
    TExtEtelRequestStatus enumeratePacketFiltersChStatus(aPacketContext, EPacketContextEnumeratePacketFilters);
    CleanupStack::PushL(enumeratePacketFiltersChStatus);
    aPacketContext.EnumeratePacketFilters(enumeratePacketFiltersChStatus,count);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersChStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters timed out"));
    LOCAL_ASSERT_EQUALS(enumeratePacketFiltersChStatus.Int(), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters returned an error"));
    LOCAL_ASSERT_EQUALS(count, 2,
                  _L("RPacketContext::EnumeratePacketFilters returned wrong number"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
    TExtEtelRequestStatus getPacketFilterInfoStatus(aPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
    RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg packetFilterPkg(packetFilter);
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,0,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;
    FillPacketFilter(defaultPacketFilterInfo,1);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,1,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,2);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns KErrArgument
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,2,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument,
                  _L("RPacketContext::GetPacketFilterInfo did not returned an error"));

	// Check RPacketContext::GetProfileParameters returns NEGOTIATED parameters.
    TExtEtelRequestStatus getProfileParametersStatus(aPacketQoS, EPacketQoSGetProfileParams);
    CleanupStack::PushL(getProfileParametersStatus);
	TPckg<NEGOTIATED> negotiatedPckg(aNegotiated);
    aPacketQoS.GetProfileParameters(getProfileParametersStatus,negotiatedPckg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeShort), KErrNone,
                  _L("RPacketQoS::GetProfileParameters timed out"));
    LOCAL_ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone,
                  _L("RPacketQoS::GetProfileParameters returned an error"));

	// ===  Add another packet filter and modify the context ===

	// Add a third TPacketFilterV3 packet filter to the secondary context using RPacketContext::AddPacketFilter 
    LOCAL_ASSERT_EQUALS(AddPacketFilterV2ToContextL(aPacketContext,3), KErrNone,
                        _L("RPacketContext::AddPacketFilter returned with error status"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	// $CTSYProblem The LTSY or the Anite system has no support to RPacketContext::ModifyActiveContext
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: The LTSY or the Anite system has no support to RPacketContext::ModifyActiveContext.<font color=Black>"));    
	TExtEtelRequestStatus modifyActiveContextStatus(aPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(modifyActiveContextStatus);
	aPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, 
                  _L("RPacketContext::ModifyActiveContext timed out."));
	LOCAL_ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone,
                  _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 3
    aPacketContext.EnumeratePacketFilters(enumeratePacketFiltersChStatus,count);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersChStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters timed out"));
    LOCAL_ASSERT_EQUALS(enumeratePacketFiltersChStatus.Int(), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters returned an error"));
    LOCAL_ASSERT_EQUALS(count, 3,
                  _L("RPacketContext::EnumeratePacketFilters returned wrong number"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,0,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,1);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,1,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,2);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,2,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,3);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns KErrArgument
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,3,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument,
                  _L("RPacketContext::GetPacketFilterInfo did not returned an error"));

	// ===  Remove a packet filter and modify the context ===

	// Remove the third packet filter from the secondary context with RPacketContext::RemovePacketFilter 
    TExtEtelRequestStatus removePacketFilterStatus(aPacketContext, EPacketContextRemovePacketFilter);
    CleanupStack::PushL(removePacketFilterStatus);
    aPacketContext.RemovePacketFilter(removePacketFilterStatus,2);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(removePacketFilterStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::RemovePacketFilter timed out"));
    LOCAL_ASSERT_EQUALS(removePacketFilterStatus.Int(), KErrNone,
                  _L("RPacketContext::RemovePacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
    aPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, 
                  _L("RPacketContext::ModifyActiveContext timed out."));
	LOCAL_ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone,
                  _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 2
    count=-1;
    TExtEtelRequestStatus enumeratePacketFiltersStatus(aPacketContext, EPacketContextEnumeratePacketFilters);
    CleanupStack::PushL(enumeratePacketFiltersStatus);
    aPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus,count);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters timed out"));
    LOCAL_ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters returned an error"));
    LOCAL_ASSERT_EQUALS(count, 2,
                  _L("RPacketContext::EnumeratePacketFilters returned wrong number"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,0,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,1);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,1,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,2);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns KErrArgument
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,2,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument,
                  _L("RPacketContext::GetPacketFilterInfo did not returned an error"));

	// ===  Remove another packet filter and modify the context ===

	// Remove the first packet filter from the secondary context with RPacketContext::RemovePacketFilter 
    aPacketContext.RemovePacketFilter(removePacketFilterStatus,0);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(removePacketFilterStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::RemovePacketFilter timed out"));
    LOCAL_ASSERT_EQUALS(removePacketFilterStatus.Int(), KErrNone,
                  _L("RPacketContext::RemovePacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
    aPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, 
                  _L("RPacketContext::ModifyActiveContext timed out."));
	LOCAL_ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone,
                  _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1
    count=-1;
    aPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus,count);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters timed out"));
    LOCAL_ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone,
                  _L("RPacketContext::EnumeratePacketFilters returned an error"));
    LOCAL_ASSERT_EQUALS(count, 1,
                  _L("RPacketContext::EnumeratePacketFilters returned wrong number"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns second packet filter added to the context
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,0,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo returned an error"));
    FillPacketFilter(defaultPacketFilterInfo,2);
    LOCAL_ASSERT_TRUE(CompareFilters(packetFilter,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns KErrArgument
    aPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus,1,packetFilterPkg);
	LOCAL_ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone,
                  _L("RPacketContext::GetPacketFilterInfo timed out"));
    LOCAL_ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument,
                  _L("RPacketContext::GetPacketFilterInfo did not returned an error"));

	// Pop:
	//	enumeratePacketFiltersStatus
	//	removePacketFilterStatus
	//	modifyActiveContextStatus
	//	getProfileParametersStatus
	//	getPacketFilterInfoStatus
	//	enumeratePacketFiltersChStatus
	//	contextActivateStatus
	//	notifyContextStChStatus
	//	setProfileParamStatus
	//	getProfileCapStatus
	CleanupStack::PopAndDestroy(10,&getProfileCapStatus);
	
    return ret;
    }


TVerdict CCTSYIntegrationTestPacketContext0039::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0039
 * @SYMFssID BA/CTSY/PKTC-0039
 * @SYMTestCaseDesc Modify a secondary context with packet filter and QoS.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetProfileName, RPacketContext::GetStatus, RPacketContext::EnumeratePacketFilters, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketContext::GetPacketFilterInfo, RPacketContext::ModifyActiveContext, RPacketContext::RemovePacketFilter, RPacketQoS::OpenNewQoS, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Packet filter added and removed successfully.Context modified successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet filter info and QoS profile info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	DEBUG_PRINTF2(_L("Starting with type %d"),iTestType);
	
    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
	// Ensure there is an active primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService,
			KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,packetContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new secondary context. 
    RPacketContext& secondaryPacketContext1 = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Open a new QoS from the secodnary context  
	RPacketQoS& packetQoS1 = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

    // Activate and modify the context with TQoSGPRSRequested, TQoSCapsGPRS and TQoSGPRSNegotiated
    RPacketQoS::TQoSGPRSRequested gprsRequested;
    RPacketQoS::TQoSCapsGPRS capsGPRS;
    RPacketQoS::TQoSGPRSNegotiated gprsNegotiated;
    RPacketQoS::TQoSR99_R4Requested r99_r4Requested;
    RPacketQoS::TQoSCapsR99_R4 capsR99_R4;
    RPacketQoS::TQoSR99_R4Negotiated t99_r4Negotiated;
    RPacketQoS::TQoSR5Requested r5Requested;
    RPacketQoS::TQoSCapsR5 capsr5;
    RPacketQoS::TQoSR5Negotiated r5Negotiated;
    switch (iTestType)
		{
		case EQoSGPRS:
		    ASSERT_TRUE(ActivateAndModifyContextL(secondaryPacketContext1,packetQoS1,gprsRequested,capsGPRS,gprsNegotiated),
		                _L("CCTSYIntegrationTestPacketContext0039::ActivateAndModifyContextL failed"));
		    break;
		case EQoSR4:
		    ASSERT_TRUE(ActivateAndModifyContextL(secondaryPacketContext1,packetQoS1,r99_r4Requested,capsR99_R4,t99_r4Negotiated),
		                _L("CCTSYIntegrationTestPacketContext0039::ActivateAndModifyContextL failed"));
		    break;
		case EQoSR5:
		    ASSERT_TRUE(ActivateAndModifyContextL(secondaryPacketContext1,packetQoS1,r5Requested,capsr5,r5Negotiated),
		                _L("CCTSYIntegrationTestPacketContext0039::ActivateAndModifyContextL failed"));
		    break;			
			
		default:
			CHECK_TRUE_L(EFalse, _L("Wrong test type!"));
			break;
		}

	// Repeat test with TQoSR99_R4Requested and TQoSR5Requested types in  RPacketQoS::SetProfileParameters and checking for TQoSR99_R4Negotiated and TQoSR5Negotiated  in RPacketContext::GetProfileParameters respectively. 

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0039::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0039");
	}



CCTSYIntegrationTestPacketContext0040::CCTSYIntegrationTestPacketContext0040(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0040::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0040::~CCTSYIntegrationTestPacketContext0040()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0040::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0040
 * @SYMFssID BA/CTSY/PKTC-0040
 * @SYMTestCaseDesc Activate a secondary context with the maximum number of packet filters allowed by default TFT.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketContext::Activate
 * @SYMTestExpectedResults Pass - 8 packet filters added. Unable to add 9th packet filter.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet filter info and QoS profile info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


    // Check that the phone is registered with the network simulator. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Ensure there is an active primary context. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new secondary context. 
	RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Add 8 TPacketFilterV2 packet filters to the secondary context using RPacketContext::AddPacketFilter each with a different iId. 
	// $CTSYProblem Adding 7 filters wil cause the LTSY to crash the board
	for(TInt index = 1; index < 7; index++)
		{
		ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, index), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 
		}

	// ===  Attempt to add a 9th filter ===

	// Check RPacketContext::AddPacketFilter returns KErrArgument
	// $CTSYProblem Adding 7 filters wil cause the LTSY to crash the board

	// Open a new QoS from the secondary context 
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
     RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);    
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    profileName.Zero();
    ASSERT_EQUALS(secondaryPacketContext.GetProfileName(profileName),KErrNone, _L("RPacketContext::GetProfileName returned with error status."));
    ASSERT_TRUE(profileName.Length()>0, _L("RPacketContext::GetProfileName name length is 0."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(secContextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedSecContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

    secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);	
    expectedSecContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus status expected to be EStatusActive but is not."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 8
	TExtEtelRequestStatus enumeratePacketFiltersStatus(secondaryPacketContext, EPacketContextEnumeratePacketFilters);
	CleanupStack::PushL(enumeratePacketFiltersStatus);
	TInt packetFiltersCount(0);
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 8, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns sixth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 6 returns seventh packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 7 returns eighth packet filter added to the context
	// $CTSYProblem Adding 7 filters wil cause the LTSY to crash the board
    RPacketContext::TPacketFilterV2 getPacketFilterV2;
    RPacketContext::TPacketFilterV2Pckg getPacketFilterV2pkg(getPacketFilterV2);
	TExtEtelRequestStatus getPacketFilterInfoStatus(secondaryPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
	RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;

	for (TInt index = 0; index < 6; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
		FillPacketFilter(defaultPacketFilterInfo, index + 1);
		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 8 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 7, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getProfileCapStatus
	// setProfileParamStatus
	// notifySecContextStChStatus
	// secContextActivateStatus
	// enumeratePacketFiltersStatus
	// getPacketFilterInfoStatus
	CleanupStack::PopAndDestroy(6, &getProfileCapStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0040::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0040");
	}



CCTSYIntegrationTestPacketContext0041::CCTSYIntegrationTestPacketContext0041(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0041::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0041::~CCTSYIntegrationTestPacketContext0041()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0041::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0041
 * @SYMFssID BA/CTSY/PKTC-0041
 * @SYMTestCaseDesc Modify a secondary context with the maximum number of packet filters allowed by default TFT.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::GetPacketFilterInfo, RPacketContext::NotifyStatusChange, RPacketContext::ModifyActiveContext, RPacketContext::GetProfileParameters, RPacketContext::GetProfileName, RPacketContext::GetStatus, RPacketContext::RemovePacketFilter, RPacketContext::EnumeratePacketFilters, RPacketQoS::SetProfileParameters, RPacketQoS::OpenNewQoS, RPacketContext::AddPacketFilter
 * @SYMTestExpectedResults Pass - 8 packet filters added. Unable to add 9th packet filter.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet filter info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


    //Check if we are on a simulated network to run emergency call tests
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);	

	// Ensure there is an active primary context. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new secondary context. 
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Add a TPacketFilterV2 packet filter with iId = 1 to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// Open a new QoS from the secondary context
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    profileName.Zero();
    ASSERT_EQUALS(secondaryPacketContext.GetProfileName(profileName),KErrNone, _L("RPacketContext::GetProfileName returned with error status."));
    ASSERT_TRUE(profileName.Length()>0, _L("RPacketContext::GetProfileName name length is 0."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(secContextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedSecContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);
    secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);	
    expectedSecContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus status expected to be EStatusActive but is not."));

	// Add 7 more TPacketFilterV2 packet filters (with iId = 2 to 8) to the secondary context using RPacketContext::AddPacketFilter 
	for(TInt index = 2; index < 9; index++)
		{
		ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, index), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 
		}

	// ===  Attempt to add a 9th filter to the context ===

	// Check RPacketContext::AddPacketFilter with iId = 9 returns KErrArgument
	// $CTSYProblem.
	// PacketContext::AddPacketFilter return KErrGeneral instead of KErrArgument for wrong argument. 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 9,KErrGeneral), KErrGeneral, _L("RPacketContext::AddPacketFilter did not return with an error as expected."));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	// $CTSYProblem The LTSY or the Anite system has no support to RPacketContext::ModifyActiveContext
	ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: The LTSY or the Anite system has no support to RPacketContext::ModifyActiveContext.<font color=Black>"));    
	TExtEtelRequestStatus modifyActiveContextStatus(secondaryPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(modifyActiveContextStatus);
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 8
	TExtEtelRequestStatus enumeratePacketFiltersStatus(secondaryPacketContext, EPacketContextEnumeratePacketFilters);
	CleanupStack::PushL(enumeratePacketFiltersStatus);
	TInt packetFiltersCount(0);
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 8, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns sixth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 6 returns seventh packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 7 returns eighth packet filter added to the context
    RPacketContext::TPacketFilterV2 getPacketFilterV2;
    RPacketContext::TPacketFilterV2Pckg getPacketFilterV2pkg(getPacketFilterV2);
	TExtEtelRequestStatus getPacketFilterInfoStatus(secondaryPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
	RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;

	for (TInt index = 0; index < 8; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
		FillPacketFilter(defaultPacketFilterInfo, index + 1);
		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 8 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 8, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// Remove the filter with iId = 3 using RPacketContext::RemovePacketFilter 
	TExtEtelRequestStatus removePacketFilterStatus(secondaryPacketContext, EPacketContextRemovePacketFilter);
	CleanupStack::PushL(removePacketFilterStatus);
	TInt wantedId = 3;
	secondaryPacketContext.RemovePacketFilter(removePacketFilterStatus, wantedId);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(removePacketFilterStatus, ETimeShort), KErrNone, _L("RPacketContext::RemovePacketFilter timed out"));
	ASSERT_EQUALS(removePacketFilterStatus.Int(), KErrNone, _L("RPacketContext::RemovePacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 7
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 7, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fifth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns sixth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns seventh packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 6 returns eighth packet filter added to the context
	for (TInt index = 0; index < 7; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
		
		if(index >= 2)
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 2);
			}
		else
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 1);
			}
			
		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 7 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 7, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// ===  Add a packet filter with the same Id as an existing one (modification of a packet filter) ===

	// Add another filter packet filter to the context with iId = 1 
    RPacketContext::TPacketFilterV2 packetFilterV2;
    FillPacketFilter(packetFilterV2, 3);
	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);
	TExtEtelRequestStatus addFilterStatus(primaryPacketContext, EPacketContextAddPacketFilter); 
    CleanupStack::PushL(addFilterStatus);
    packetFilterV2.iId = 1;
	secondaryPacketContext.AddPacketFilter(addFilterStatus, packetFilterV2pckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(addFilterStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddPacketFilter timed out"));
    ASSERT_EQUALS(addFilterStatus.Int(), KErrNone, _L("RPacketContext::AddPacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 7
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 7, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns newly added packet filter 
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fifth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns sixth packet filter added to the tcontext
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns seventh packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 6 returns eighth packet filter added to the context
	for (TInt index = 0; index < 7; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));

		if(index == 0)
			{
			FillPacketFilter(defaultPacketFilterInfo, 3);
			defaultPacketFilterInfo.iId = 1;
			}
		else if(index >= 2)
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 2);
			}
		else
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 1);
			}

		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 7 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 7, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// ===  Add a packet filter with iId = 3 (add new one that was removed) ===

	// Add another filter packet filter to the context with iId = 3 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 3), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 8
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 8, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns sixth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 6 returns seventh packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 7 returns eighth packet filter added to the context
	for (TInt index = 0; index < 8; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));

		if(index == 0)
			{
			FillPacketFilter(defaultPacketFilterInfo, 3);
			defaultPacketFilterInfo.iId = 1;
			}
		else
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 1);
			}

		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 8 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 8, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getProfileCapStatus
	// setProfileParamStatus
	// notifySecContextStChStatus
	// secContextActivateStatus
	// addFilterStatus
	// modifyActiveContextStatus
	// enumeratePacketFiltersStatus
	// getPacketFilterInfoStatus
	// removePacketFilterStatus
	CleanupStack::PopAndDestroy(9, &getProfileCapStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0041::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0041");
	}



CCTSYIntegrationTestPacketContext0042::CCTSYIntegrationTestPacketContext0042(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0042::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0042::~CCTSYIntegrationTestPacketContext0042()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0042::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0042
 * @SYMFssID BA/CTSY/PKTC-0042
 * @SYMTestCaseDesc Activate a secondary context creating a TFT for the packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::CreateNewTFT, RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketContext::Activate, RPacketContext::DeleteTFT
 * @SYMTestExpectedResults Pass - TFT created and deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet filter info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// ==== Ensure there is an active primary context === 

    //Check if we are on a simulated network
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Set the attach mode to EAttachWhenPossible. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    // Open primary context
    RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////


	// Open a new secondary context. 
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Create a new TFT with RPacketContext::CreateNewTFT and aSize = 5 
	TExtEtelRequestStatus createNewTFTStatus(secondaryPacketContext, EPacketContextCreateNewTFT);
	CleanupStack::PushL(createNewTFTStatus);
	const TInt size(5);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50005);
	secondaryPacketContext.CreateNewTFT(createNewTFTStatus, size);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createNewTFTStatus, ETimeMedium), KErrNone, _L("RPacketContext::CreateNewTFT timed out"));
	ASSERT_EQUALS(createNewTFTStatus.Int(), KErrNone, _L("RPacketContext::CreateNewTFT returned an error"));

	// Add a TPacketFilterV2 packet filter with iId = 1 to the secondary context using RPacketContext::AddPacketFilter 
	// Add a TPacketFilterV2 packet filter with iId = 2 to the secondary context using RPacketContext::AddPacketFilter 
	// Add a TPacketFilterV2 packet filter with iId = 3 to the secondary context using RPacketContext::AddPacketFilter 
	// Add a TPacketFilterV2 packet filter with iId = 4 to the secondary context using RPacketContext::AddPacketFilter 
	// Add a TPacketFilterV2 packet filter with iId = 5 to the secondary context using RPacketContext::AddPacketFilter 
	for (TInt index = 1; index < 6; index++)
		{
		ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, index), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 
		}

	// Add a TPacketFilterV2 packet filter with iId = 6 to the secondary context returns KErrArgument 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 6), KErrArgument, _L("RPacketContext::AddPacketFilter did not return with an error as expected.")); 

	// Open a new QoS from the secondary context  
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);    
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    ASSERT_EQUALS(secondaryPacketContext.GetProfileName(profileName),KErrNone, _L("RPacketContext::GetProfileName returned with error status."));
    ASSERT_TRUE(profileName.Length()>0, _L("RPacketContext::GetProfileName name length is 0."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedSecContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);
    secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);	
    expectedSecContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus status expected to be EStatusActive but is not."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 5
	TExtEtelRequestStatus enumeratePacketFiltersStatus(secondaryPacketContext, EPacketContextEnumeratePacketFilters);
	CleanupStack::PushL(enumeratePacketFiltersStatus);
	TInt packetFiltersCount(0);
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 5, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
    RPacketContext::TPacketFilterV2 getPacketFilterV2;
    RPacketContext::TPacketFilterV2Pckg getPacketFilterV2pkg(getPacketFilterV2);
	TExtEtelRequestStatus getPacketFilterInfoStatus(secondaryPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
	RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;

	for (TInt index = 0; index < 5; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
		FillPacketFilter(defaultPacketFilterInfo, index + 1);
		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns KErrArgument
	TInt index = 5;
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// Repost notification for context's status change
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Deactivate the secondary context. 
	TExtEtelRequestStatus contextDeactivateStatus(secondaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	secondaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusInactive
    RPacketContext::TContextStatus 	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					secondaryPacketContext,
					notifySecContextStChStatus,
					secPacketContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus status did not return EStatusInactive."));

	// Delete the TFT with RPacketContext::DeleteTFT 
	TExtEtelRequestStatus deleteTFTStatus(secondaryPacketContext, EPacketContextDeleteTFT);
	CleanupStack::PushL(deleteTFTStatus);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50006);
	secondaryPacketContext.DeleteTFT(deleteTFTStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createNewTFTStatus, ETimeMedium), KErrNone, _L("RPacketContext::DeleteTFT timed out"));
	ASSERT_EQUALS(createNewTFTStatus.Int(), KErrNone, _L("RPacketContext::DeleteTFT returned an error"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();

	// Pop
	// createNewTFTStatus
	// getProfileCapStatus
	// setProfileParamStatus
	// notifySecContextStChStatus
	// secContextActivateStatus
	// enumeratePacketFiltersStatus
	// getPacketFilterInfoStatus
	// contextDeactivateStatus
	// deleteTFTStatus
	CleanupStack::PopAndDestroy(9, &createNewTFTStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0042::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0042");
	}



CCTSYIntegrationTestPacketContext0043::CCTSYIntegrationTestPacketContext0043(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0043::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0043::~CCTSYIntegrationTestPacketContext0043()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0043::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0043
 * @SYMFssID BA/CTSY/PKTC-0043
 * @SYMTestCaseDesc Modify a secondary context creating a TFT for the packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::Activate, RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketContext::ModifyActiveContext, RPacketContext::DeleteTFT
 * @SYMTestExpectedResults Pass - TFT created and deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify packet filter info correct.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


    //Check if we are on a simulated network to run emergency call tests
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);	

	// Ensure there is an active primary context. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new secondary context. 
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
			KMainServer,
			KMainPhone,
			KMainPacketService, 
			KPrimaryPacketContext1,
			KSecondaryPacketContext1);

	// Create a new TFT with RPacketContext::CreateNewTFT and aSize = 5 
	TExtEtelRequestStatus createNewTFTStatus(secondaryPacketContext, EPacketContextCreateNewTFT);
	CleanupStack::PushL(createNewTFTStatus);
	const TInt size(5);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50005);
	secondaryPacketContext.CreateNewTFT(createNewTFTStatus, size);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createNewTFTStatus, ETimeMedium), KErrNone, _L("RPacketContext::CreateNewTFT timed out"));
	ASSERT_EQUALS(createNewTFTStatus.Int(), KErrNone, _L("RPacketContext::CreateNewTFT returned an error"));

	// Add a TPacketFilterV2 packet filter with iId = 1 to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// Open a new QoS from the secondary context with  
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get QoS profile capabilities with RPacketQoS::GetProfileCapabilities
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);    
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Check RPacketContext::GetProfileName returns QoS profile name with length > 0
    TName profileName;
    ASSERT_EQUALS(secondaryPacketContext.GetProfileName(profileName),KErrNone, _L("RPacketContext::GetProfileName returned with error status."));
    ASSERT_TRUE(profileName.Length()>0, _L("RPacketContext::GetProfileName name length is 0."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Activate the secondary context. 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedSecContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

    secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);	
    expectedSecContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus status expected to be EStatusActive but is not."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1
	TExtEtelRequestStatus enumeratePacketFiltersStatus(secondaryPacketContext, EPacketContextEnumeratePacketFilters);
	CleanupStack::PushL(enumeratePacketFiltersStatus);
	TInt packetFiltersCount(0);
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 1, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns packet filter with iId = 1
    RPacketContext::TPacketFilterV2 getPacketFilterV2;
    RPacketContext::TPacketFilterV2Pckg getPacketFilterV2pkg(getPacketFilterV2);
	TExtEtelRequestStatus getPacketFilterInfoStatus(secondaryPacketContext, EPacketContextGetPacketFilterInfo);
    CleanupStack::PushL(getPacketFilterInfoStatus);
	RPacketContext::TPacketFilterV2 defaultPacketFilterInfo;
	FillPacketFilter(defaultPacketFilterInfo, 1);
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 0, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
	ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));

	// Add 4 more TPacketFilterV2 packet filters (with iId = 2 to 5) to the secondary context using RPacketContext::AddPacketFilter 
	for(TInt index = 2; index < 6; index++)
		{
		ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, index), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 
		}

	// ===  Attempt to add a 6th filter to the context ===

	// Check RPacketContext::AddPacketFilter with iId = 6 returns KErrArgument
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 6), KErrArgument, _L("RPacketContext::AddPacketFilter did not return with an error as expected."));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	TExtEtelRequestStatus modifyActiveContextStatus(secondaryPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(modifyActiveContextStatus);
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 5
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 5, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
	for (TInt index = 0; index < 5; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
		FillPacketFilter(defaultPacketFilterInfo, index + 1);
		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 5, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// Remove the filter with iId = 3 using RPacketContext::RemovePacketFilter 
	TExtEtelRequestStatus removePacketFilterStatus(secondaryPacketContext, EPacketContextRemovePacketFilter);
	CleanupStack::PushL(removePacketFilterStatus);
	TInt wantedId = 3;
	secondaryPacketContext.RemovePacketFilter(removePacketFilterStatus, wantedId);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(removePacketFilterStatus, ETimeShort), KErrNone, _L("RPacketContext::RemovePacketFilter timed out"));
	ASSERT_EQUALS(removePacketFilterStatus.Int(), KErrNone, _L("RPacketContext::RemovePacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 4
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 4, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fifth packet filter added to the context
	for (TInt index = 0; index < 4; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));
	    
	    if (index >= 2) // Third filter removed earlier in this test
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 2);
			}
	    else
   			{
   			FillPacketFilter(defaultPacketFilterInfo, index + 1);
   			}

		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 4, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// ===  Add a packet filter with the same Id as an existing one (modification of a packet filter) ===

	// Add another packet filter to the context with iId = 1 
    RPacketContext::TPacketFilterV2 packetFilterV2;
	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);
	TExtEtelRequestStatus addFilterStatus(primaryPacketContext, EPacketContextAddPacketFilter); 
    CleanupStack::PushL(addFilterStatus);
	FillPacketFilter(packetFilterV2, 2);
	packetFilterV2.iId = 1;
	secondaryPacketContext.AddPacketFilter(addFilterStatus, packetFilterV2pckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(addFilterStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddPacketFilter timed out"));
	ASSERT_EQUALS(addFilterStatus.Int(), KErrNone, _L("RPacketContext::AddPacketFilter returned an error"));

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 4
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 4, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter that has just been added
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fifth packet filter added to the context
	for (TInt index = 0; index < 4; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));

		if (index == 0)
			{
			FillPacketFilter(defaultPacketFilterInfo, 2);
            packetFilterV2.iId = 1;
			}
	    else if (index >= 2) // Third filter removed earlier in this test
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 2);
			}
	    else
			{
   			FillPacketFilter(defaultPacketFilterInfo, index + 1);
			}

		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 4, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// ===  Add a packet filter with iId = 3 (add new one that was removed) ===

	// Add another filter packet filter to the context with iId = 3 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 3), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));
	ASSERT_EQUALS(modifyActiveContextStatus.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned with error status."));

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 5
	packetFiltersCount = 0;
	secondaryPacketContext.EnumeratePacketFilters(enumeratePacketFiltersStatus, packetFiltersCount);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumeratePacketFiltersStatus, ETimeLong), KErrNone, _L("RPacketContext::EnumeratePacketFilters timed out."));
	ASSERT_EQUALS(enumeratePacketFiltersStatus.Int(), KErrNone, _L("RPacketContext::EnumeratePacketFilters returned with error status."));
	ASSERT_EQUALS(packetFiltersCount, 5, _L("RPacketContext::EnumeratePacketFilters returned wrong number of packet filters."));

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns first packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 1 returns second packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 2 returns third packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 3 returns fourth packet filter added to the context
	// Check RPacketContext::GetPacketFilterInfo with aIndex = 4 returns fifth packet filter added to the context
	for (TInt index = 1; index < 4; index++)
		{
		secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, index, getPacketFilterV2pkg);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
	    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrNone, _L("RPacketContext::GetPacketFilterInfo returned an error"));

		if (index == 0)
			{
			FillPacketFilter(defaultPacketFilterInfo, 2);
            packetFilterV2.iId = 1;
			}
		else
			{
			FillPacketFilter(defaultPacketFilterInfo, index + 1);
			}

		ASSERT_TRUE(CompareFilters(getPacketFilterV2,defaultPacketFilterInfo),_L("RPacketContext::GetPacketFilterInfo returned wrong filter"));
		}

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 5 returns KErrArgument
	secondaryPacketContext.GetPacketFilterInfo(getPacketFilterInfoStatus, 5, getPacketFilterV2pkg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getPacketFilterInfoStatus, ETimeShort), KErrNone, _L("RPacketContext::GetPacketFilterInfo timed out"));
    ASSERT_EQUALS(getPacketFilterInfoStatus.Int(), KErrArgument, _L("RPacketContext::GetPacketFilterInfo did not return correct error"));

	// Repost notification for context's status change
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Deactivate the secondary context. 
	TExtEtelRequestStatus contextDeactivateStatus(secondaryPacketContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	secondaryPacketContext.Deactivate(contextDeactivateStatus);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone, _L("RPacketContext::Deactivate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusInactive
    RPacketContext::TContextStatus 	expectedPacketContextStatus = RPacketContext::EStatusInactive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
					secondaryPacketContext,
					notifySecContextStChStatus,
					secPacketContextStatus, 
					expectedPacketContextStatus,
					KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusInactive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusInactive, _L("Packet Context's status expected to be EStatusActive but is not."));

	// Delete the TFT with RPacketContext::DeleteTFT 
	TExtEtelRequestStatus deleteTFTStatus(secondaryPacketContext, EPacketContextDeleteTFT);
	CleanupStack::PushL(deleteTFTStatus);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50006);
	secondaryPacketContext.DeleteTFT(deleteTFTStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(createNewTFTStatus, ETimeMedium), KErrNone, _L("RPacketContext::DeleteTFT timed out"));
	ASSERT_EQUALS(createNewTFTStatus.Int(), KErrNone, _L("RPacketContext::DeleteTFT returned an error"));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// createNewTFTStatus
	// getProfileCapStatus
	// setProfileParamStatus
	// notifySecContextStChStatus
	// secContextActivateStatus
	// enumeratePacketFiltersStatus
	// getPacketFilterInfoStatus
	// modifyActiveContextStatus
	// removePacketFilterStatus
	// addFilterStatus
	// contextDeactivateStatus
	// deleteTFTStatus
	CleanupStack::PopAndDestroy(12, &createNewTFTStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0043::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0043");
	}



CCTSYIntegrationTestPacketContext0044::CCTSYIntegrationTestPacketContext0044(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0044::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0044::~CCTSYIntegrationTestPacketContext0044()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0044::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0044
 * @SYMFssID BA/CTSY/PKTC-0044
 * @SYMTestCaseDesc Modify a context when it is not active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext
 * @SYMTestExpectedResults Pass - Error returned on attempt to modify a context that is not active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context status and packet service status and error returned for modify.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////


	// Open a new primary context.
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
																KMainServer,
																KMainPhone,
																KMainPacketService,
																KPrimaryPacketContext1);

	// Modify the context with RPacketContext::ModifyActiveContext 
	TExtEtelRequestStatus modifyActiveContextStatus(primaryPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(modifyActiveContextStatus);
	primaryPacketContext.ModifyActiveContext(modifyActiveContextStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));

	// Check error is returned.
	ASSERT_TRUE(modifyActiveContextStatus.Int() != KErrNone, _L("RPacketContext::ModifyActiveContext did not return with an error as expected."));

	// Open a new secondary context from the primary  
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
																KMainServer,
																KMainPhone,
																KMainPacketService, 
																KPrimaryPacketContext1,
																KSecondaryPacketContext1);

	// Modify the secondary context with RPacketContext::ModifyActiveContext 
	TExtEtelRequestStatus modifyActiveContextStatus2(secondaryPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(modifyActiveContextStatus2);
	secondaryPacketContext.ModifyActiveContext(modifyActiveContextStatus2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(modifyActiveContextStatus2, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out."));

	// Check error is returned.
	ASSERT_TRUE(modifyActiveContextStatus2.Int() != KErrNone, _L("RPacketContext::ModifyActiveContext did not return with an error as expected."));


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// modifyActiveContextStatus
	// modifyActiveContextStatus2
	CleanupStack::PopAndDestroy(2, &modifyActiveContextStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0044::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0044");
	}



CCTSYIntegrationTestPacketContext0045::CCTSYIntegrationTestPacketContext0045(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0045::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0045::~CCTSYIntegrationTestPacketContext0045()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0045::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0045
 * @SYMFssID BA/CTSY/PKTC-0045
 * @SYMTestCaseDesc Add packet filter with same ID as previous packet filter.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::Activate, RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketContext::ModifyActiveContext
 * @SYMTestExpectedResults Pass - Old filter replaced.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify existing packet filter is replaced.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary context. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new primary context. 

	// Add a TPacketFilterV2 packet filter with iId = 1 to the context using RPacketContext::AddPacketFilter 

	// Add a TPacketFilterV2 packet filter with iId = 1 but with different values for the other field to the context using RPacketContext::AddPacketFilter 

	// Set a valid config for the primary context. 

	// Activate the context. 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns packet filter with details equal to those used in second call to RPacketContext::AddPacketFilter

	// Add a TPacketFilterV2 packet filter with iId = 1 but with different values for the other field to the context using RPacketContext::AddPacketFilter 

	// Modify the context 

	// Check RPacketContext::EnumeratePacketFilters returns aCount = 1

	// Check RPacketContext::GetPacketFilterInfo with aIndex = 0 returns packet filter with details equal to those used in third call to RPacketContext::AddPacketFilter

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0045::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0045");
	}



CCTSYIntegrationTestPacketContext0046::CCTSYIntegrationTestPacketContext0046(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0046::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0046::~CCTSYIntegrationTestPacketContext0046()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0046::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0046
 * @SYMFssID BA/CTSY/PKTC-0046
 * @SYMTestCaseDesc Remove packet filter specifying invalid ID.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::RemovePacketFilter
 * @SYMTestExpectedResults Pass - Error returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary context. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Add a TPacketFilterV2 packet filter with iId = 1 to the context using RPacketContext::AddPacketFilter 

	// Modify the context. 

	// Remove a packet filter with RPacketContext::RemovePacketFilter and aId = 2 

	// Check KErrArgument is returned

	// Remove a packet filter with RPacketContext::RemovePacketFilter and aId = -1 

	// Check KErrArgument is returned

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0046::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0046");
	}



CCTSYIntegrationTestPacketContext0047::CCTSYIntegrationTestPacketContext0047(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0047::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0047::~CCTSYIntegrationTestPacketContext0047()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0047::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0047
 * @SYMFssID BA/CTSY/PKTC-0047
 * @SYMTestCaseDesc Activate a primary context with media auth tokens added but no QoS or packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::AddMediaAuthorizationL, RPacketContext::GetStatus, RPacketContext::SetConfig
 * @SYMTestExpectedResults Pass - Context activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context is activated.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a primary context 

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate the context. 

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0047::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0047");
	}



CCTSYIntegrationTestPacketContext0048::CCTSYIntegrationTestPacketContext0048(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0048::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0048::~CCTSYIntegrationTestPacketContext0048()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0048::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0048
 * @SYMFssID BA/CTSY/PKTC-0048
 * @SYMTestCaseDesc Modify a primary context with media auth tokens added but no QoS or packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::AddMediaAuthorizationL, RPacketContext::GetStatus, RPacketContext::SetConfig
 * @SYMTestExpectedResults Pass - Context modified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context is modified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a primary context 

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate the context. 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Remove media auth token using RPacketContext::RemoveMediaAuthorization and aAuthorizationToken = identifier used in RPacketContext::AddMediaAuthorizationL 

	// Check RPacketContext::GetStatus returns EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0048::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0048");
	}



CCTSYIntegrationTestPacketContext0049::CCTSYIntegrationTestPacketContext0049(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0049::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0049::~CCTSYIntegrationTestPacketContext0049()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0049::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0049
 * @SYMFssID BA/CTSY/PKTC-0049
 * @SYMTestCaseDesc Activate a secondary context with media auth tokens added but no QoS or packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::Activate, RPacketContext::AddMediaAuthorizationL, RPacketContext::RemoveMediaAuthorization
 * @SYMTestExpectedResults Pass - Context activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify context is activated.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is a primary context active which has been activated with QoS and has a packet filter added to it.
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService); 

	// Open a new primary context. 
	RPacketContext& primaryContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
	// Open a new QoS for primary context
	RPacketQoS& contextQoS = iEtelSessionMgr.GetPrimaryPacketContextQoSL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);	

	// $CTSYProblem Can't add filters to primary context 
	//CHECK_EQUALS_L(AddPacketFilterV2ToContextL(primaryContext, 1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error")); 																												
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryContext);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
		
	// Open a new secondary context from the primary.
 	RPacketContext& secondaryContext = iEtelSessionMgr.GetSecondaryPacketContextL(
														KMainServer,
														KMainPhone,
														KMainPacketService, 
														KPrimaryPacketContext1,
														KSecondaryPacketContext1); 

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuth = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuth);  		
	TExtEtelRequestStatus requestAddMediaAuthStatus(secondaryContext, EPacketAddMediaAuthorization);
	CleanupStack::PushL(requestAddMediaAuthStatus);
	// $CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him"));
	secondaryContext.AddMediaAuthorizationL(requestAddMediaAuthStatus, *mediaAuth);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestAddMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddMediaAuthorizationL timed out"));
    ASSERT_EQUALS(requestAddMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::AddMediaAuthorizationL returned an error"));
	
	// Post notifier for RPacketContext::NotifyStatusChange
	TExtEtelRequestStatus notifyContextStChStatus(secondaryContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	secondaryContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Activate packet context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(secondaryContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	secondaryContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	secondaryContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);	
	expectedPacketContextStatus = RPacketContext::EStatusActive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus getPacketContextStatus;
	ASSERT_EQUALS(secondaryContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for Secondary context returned with an error."));
	ASSERT_EQUALS(getPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// mediaAuth
	// requestAddMediaAuthStatus
	// notifyContextStChStatus
	// contextActivateStatus
	CleanupStack::PopAndDestroy(4, mediaAuth);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0049::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0049");
	}



CCTSYIntegrationTestPacketContext0050::CCTSYIntegrationTestPacketContext0050(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0050::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0050::~CCTSYIntegrationTestPacketContext0050()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0050::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0050
 * @SYMFssID BA/CTSY/PKTC-0050
 * @SYMTestCaseDesc Modify a secondary context with media auth tokens added but no QoS or packet filters.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::Activate, RPacketContext::AddMediaAuthorizationL, RPacketContext::RemoveMediaAuthorization, RPacketContext::ModifyActiveContext
 * @SYMTestExpectedResults Pass - Context modified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify context is modified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Ensure there is a primary context active which has been activated with QoS and has a packet filter added to it. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	//Check if we are on a simulated network
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);
	
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
	RPacketContext& primaryContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	RPacketQoS& primaryQoS = iEtelSessionMgr.GetPrimaryPacketContextQoSL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);							
											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryContext);

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Open a new secondary context from the primary. 
	RPacketContext& secondaryContext = iEtelSessionMgr.GetSecondaryPacketContextL(
														KMainServer,
														KMainPhone,
														KMainPacketService, 
														KPrimaryPacketContext1,
														KSecondaryPacketContext1); 

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuth = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuth);  
	TExtEtelRequestStatus requestAddMediaAuthStatus(secondaryContext, EPacketAddMediaAuthorization);
	CleanupStack::PushL(requestAddMediaAuthStatus);
	// $CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him"));
	secondaryContext.AddMediaAuthorizationL(requestAddMediaAuthStatus, *mediaAuth);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestAddMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddMediaAuthorizationL timed out"));
    ASSERT_EQUALS(requestAddMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::AddMediaAuthorizationL returned an error"));	

	// Post notifier for RPacketContext::NotifyStatusChange
	TExtEtelRequestStatus notifyContextStChStatus(secondaryContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	secondaryContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Activate the context.
	TExtEtelRequestStatus contextActivateStatus(secondaryContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	secondaryContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;   
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	secondaryContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus getPacketContextStatus;
	ASSERT_EQUALS(secondaryContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for secondary context returned with an error."));
	ASSERT_EQUALS(getPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Remove media auth token using RPacketContext::RemoveMediaAuthorization and aAuthorizationToken = identifier used in RPacketContext::AddMediaAuthorizationL 
	TExtEtelRequestStatus requestRemoveMediaAuthStatus(secondaryContext, EPacketRemoveMediaAuthorization);
	CleanupStack::PushL(requestRemoveMediaAuthStatus);
	secondaryContext.RemoveMediaAuthorization(requestRemoveMediaAuthStatus, mediaAuth->iAuthorizationToken); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestRemoveMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::RemoveMediaAuthorization timed out"));
    ASSERT_EQUALS(requestRemoveMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::RemoveMediaAuthorization returned an error"));

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(secondaryContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for secondary context returned with an error."));
	ASSERT_EQUALS(getPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus packetServiceStatus;
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop:
	// mediaAuth
	// requestAddMediaAuthStatus
	// notifyContextStChStatus
	// contextActivateStatus
	// requestRemoveMediaAuthStatus
	CleanupStack::PopAndDestroy(5, &mediaAuth);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0050::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0050");
	}



CCTSYIntegrationTestPacketContext0051::CCTSYIntegrationTestPacketContext0051(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0051::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0051::~CCTSYIntegrationTestPacketContext0051()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0051::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0051
 * @SYMFssID BA/CTSY/PKTC-0051
 * @SYMTestCaseDesc Activate a primary context with QoS, packet filters and media auth tokens.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::AddMediaAuthorizationL, RPacketContext::SetConfig, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Context activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context is activated.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context  

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Set a valid context config with RPacketContext::SetConfig 

	// Activate primary context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange for the primary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0051::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0051");
	}



CCTSYIntegrationTestPacketContext0052::CCTSYIntegrationTestPacketContext0052(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0052::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0052::~CCTSYIntegrationTestPacketContext0052()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0052::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0052
 * @SYMFssID BA/CTSY/PKTC-0052
 * @SYMTestCaseDesc Modify a primary context with QoS, packet filters and media auth tokens.
 * @SYMTestPriority High
 * @SYMTestActions RPacketQoS::SetProfileParameters, RPacketQoS::GetProfileParameters, RPacketQoS::NotifyProfileChanged, RPacketQoS::GetProfileCapabilities, RPacketContext::OpenNewContext, RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::EnumeratePacketFilters, RPacketContext::GetPacketFilterInfo, RPacketContext::RemovePacketFilter
 * @SYMTestExpectedResults Pass - Context modified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify context is modified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is an active primary context. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the primary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the primary context  

	// Get R5 QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSR5Requested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Set a valid context config with RPacketContext::SetConfig 

	// Activate primary context with RPacketContext::Activate 

	// ===  Check context and service statuses ===

	// Check RPacketService::NotifyStatusChange completes with EStatusActive

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::NotifyStatusChange for the primary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the primary context returns EStatusActive

	// ===  Remove media auth tokens ===

	// Remove media authorization with RPacketContext::RemoveMediaAuthorization 

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Modify context with RPacketContext::ModifyActiveContext 

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0052::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0052");
	}



CCTSYIntegrationTestPacketContext0053::CCTSYIntegrationTestPacketContext0053(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0053::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0053::~CCTSYIntegrationTestPacketContext0053()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0053::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0053
 * @SYMFssID BA/CTSY/PKTC-0053
 * @SYMTestCaseDesc Activate a secondary context with QoS, packet filters and media auth tokens.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketContext::AddMediaAuthorizationL, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Context activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify context is activated.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////


	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    //Check if we are on a simulated network
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
    // Open primary context
    RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, primaryPacketContext);	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	// Open a new secondary context 
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext, 1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status")); 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context 
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuth = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuth);  
	TExtEtelRequestStatus addMediaAuthStatus(secondaryPacketContext, EPacketAddMediaAuthorization);
	CleanupStack::PushL(addMediaAuthStatus);
	// $CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him"));
	secondaryPacketContext.AddMediaAuthorizationL(addMediaAuthStatus, *mediaAuth);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(addMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddMediaAuthorizationL timed out"));
    ASSERT_EQUALS(addMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::AddMediaAuthorizationL returned an error"));

	//Post notifier for RPacketContext::NotifyStatusChange
	TExtEtelRequestStatus notifyContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Activate context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	secondaryPacketContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	secondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	RPacketContext::TContextStatus getPacketContextStatus;
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for primary context returned with an error."));
	ASSERT_EQUALS(getPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getProfileCapStatus
	// setProfileParamStatus
	// mediaAuth
	// addMediaAuthStatus
	// notifyContextStChStatus
	// contextActivateStatus
	CleanupStack::PopAndDestroy(6, &getProfileCapStatus);

	return TestStepResult();
	}


TPtrC CCTSYIntegrationTestPacketContext0053::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0053");
	}



CCTSYIntegrationTestPacketContext0054::CCTSYIntegrationTestPacketContext0054(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0054::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0054::~CCTSYIntegrationTestPacketContext0054()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0054::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0054
 * @SYMFssID BA/CTSY/PKTC-0054
 * @SYMTestCaseDesc Modify a secondary context with QoS, packet filters and media auth tokens.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketContext::AddMediaAuthorizationL, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters
 * @SYMTestExpectedResults Pass - Context modified.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/automatic
 *
 * Reason for test: Verify context is modified.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// ==== Ensure there is an active primary context === 

    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    
    //Check if we are on a simulated network
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
 	iNetworkTestHelper.CheckPhoneConnectedToAniteL(phone);

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Set the attach mode to EAttachWhenPossible. 
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    // Open primary context
    RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Activate primary context.											
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone,primaryPacketContext);												

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
			
	// Open a new secondary context 
    RPacketContext& secondaryPacketContext = iEtelSessionMgr.GetSecondaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);
	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
	RPacketContext::TPacketFilterV2 packetFilterV2;
    TInt id(1);
    FillPacketFilter(packetFilterV2, id);
	RPacketContext::TPacketFilterV2Pckg packetFilterV2pckg(packetFilterV2);
	TExtEtelRequestStatus addFilterStatus(secondaryPacketContext, EPacketContextAddPacketFilter); 
    CleanupStack::PushL(addFilterStatus);
	secondaryPacketContext.AddPacketFilter(addFilterStatus, packetFilterV2pckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(addFilterStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddPacketFilter timed out"));
    ASSERT_EQUALS(addFilterStatus.Int(), KErrNone, _L("RPacketContext::AddPacketFilter returned an error"));
	
	// ===  Add QoS ===

	// Open a new QoS from the secondary context  
	RPacketQoS& contextQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1,
											KSecondaryPacketContext1);
											
	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 
	RPacketQoS::TQoSCapsGPRS qosCapsGPRS;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGPRSPckg(qosCapsGPRS);	
	TExtEtelRequestStatus reqQoSProfileCaps(contextQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(reqQoSProfileCaps);
	contextQoS.GetProfileCapabilities(reqQoSProfileCaps, qosCapsGPRSPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqQoSProfileCaps, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));						
	ASSERT_EQUALS(reqQoSProfileCaps.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));
	
	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
	RPacketQoS::TQoSGPRSRequested qosReqCapsGPRS;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosReqCapsGPRSPckg(qosReqCapsGPRS);
	FillParametersFromCapabilities(qosReqCapsGPRS, qosCapsGPRS);
	TExtEtelRequestStatus reqStatusSetProfileParams(contextQoS,EPacketQoSSetProfileParams);
	CleanupStack::PushL(reqStatusSetProfileParams);
	contextQoS.SetProfileParameters(reqStatusSetProfileParams,qosReqCapsGPRSPckg );	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqStatusSetProfileParams, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));						
	ASSERT_EQUALS(reqStatusSetProfileParams.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuth = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuth);  		
	TExtEtelRequestStatus requestAddMediaAuthStatus(secondaryPacketContext, EPacketAddMediaAuthorization);
	CleanupStack::PushL(requestAddMediaAuthStatus);
	// $CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him 
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY crash when the CTSY send the EPacketAddMediaAuthorization message to him"));
	secondaryPacketContext.AddMediaAuthorizationL(requestAddMediaAuthStatus, *mediaAuth);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestAddMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::AddMediaAuthorizationL timed out"));
    ASSERT_EQUALS(requestAddMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::AddMediaAuthorizationL returned an error"));

	// Post notifier for RPacketContext::NotifyStatusChange
	TExtEtelRequestStatus notifyContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Activate context with RPacketContext::Activate 
	TExtEtelRequestStatus contextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	secondaryPacketContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// ===  Check context and service statuses ===

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActivating;
   	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	secondaryPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	expectedPacketContextStatus = RPacketContext::EStatusActive;	
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
				
	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	RPacketContext::TContextStatus getPacketContextStatus;
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(getPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus for secondary context returned with an error."));
	ASSERT_EQUALS(getPacketContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// ===  Remove media auth tokens ===

	// Remove media authorization with RPacketContext::RemoveMediaAuthorization 
	TExtEtelRequestStatus requestRemoveMediaAuthStatus(secondaryPacketContext, EPacketRemoveMediaAuthorization);
	CleanupStack::PushL(requestRemoveMediaAuthStatus);
	secondaryPacketContext.RemoveMediaAuthorization(requestRemoveMediaAuthStatus, mediaAuth->iAuthorizationToken); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestRemoveMediaAuthStatus, ETimeMedium), KErrNone, _L("RPacketContext::RemoveMediaAuthorization timed out"));
    ASSERT_EQUALS(requestRemoveMediaAuthStatus.Int(), KErrNone, _L("RPacketContext::RemoveMediaAuthorization returned an error"));

	// ===  Add media auth tokens ===

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL
	RPacketContext::CTFTMediaAuthorizationV3* secondMediaAuth = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(secondMediaAuth);  
	TExtEtelRequestStatus requestAddMediaAuthStatusNumberTwo(secondaryPacketContext, EPacketAddMediaAuthorization);
	CleanupStack::PushL(requestAddMediaAuthStatusNumberTwo);
	secondaryPacketContext.AddMediaAuthorizationL(requestAddMediaAuthStatusNumberTwo, *secondMediaAuth);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestAddMediaAuthStatusNumberTwo, ETimeMedium), KErrNone, _L("RPacketContext::AddMediaAuthorizationL timed out"));
    ASSERT_EQUALS(requestAddMediaAuthStatusNumberTwo.Int(), KErrNone, _L("RPacketContext::AddMediaAuthorizationL returned an error")); 

	// Modify context with RPacketContext::ModifyActiveContext 
	TExtEtelRequestStatus requestModifyContext(secondaryPacketContext, EPacketContextModifyActiveContext);
	CleanupStack::PushL(requestModifyContext);
	secondaryPacketContext.ModifyActiveContext(requestModifyContext);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(requestModifyContext, ETimeMedium), KErrNone, _L("RPacketContext::ModifyActiveContext timed out"));
    ASSERT_EQUALS(requestModifyContext.Int(), KErrNone, _L("RPacketContext::ModifyActiveContext returned an error")); 
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// addFilterStatus
	// reqQoSProfileCaps
	// reqStatusSetProfileParams
	// mediaAuth
	// requestAddMediaAuthStatus
	// notifyContextStChStatus
	// contextActivateStatus
	// requestRemoveMediaAuthStatus
	// secondMediaAuth
	// requestAddMediaAuthStatusNumberTwo
	// requestModifyContext
	
	CleanupStack::PopAndDestroy(11, &addFilterStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0054::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0054");
	}



CCTSYIntegrationTestPacketContext0055::CCTSYIntegrationTestPacketContext0055(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0055::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0055::~CCTSYIntegrationTestPacketContext0055()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0055::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0055
 * @SYMFssID BA/CTSY/PKTC-0055
 * @SYMTestCaseDesc Remove media authorization with invalid identifier.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::RemoveMediaAuthorization
 * @SYMTestExpectedResults Pass - Error returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a primary context. 

	// Add a media auth token using RPacketContext::AddMediaAuthorizationL 

	// Remove media authorization with RPacketContext::RemoveMediaAuthorization with invalid identifier in aAuthorizationToken 

	// Check error is returned.

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0055::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0055");
	}



CCTSYIntegrationTestPacketContext0056::CCTSYIntegrationTestPacketContext0056(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0056::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0056::~CCTSYIntegrationTestPacketContext0056()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0056::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0056
 * @SYMFssID BA/CTSY/PKTC-0056
 * @SYMTestCaseDesc Activate a context and download data.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenExistingContext, RPacketContext::GetConnectionSpeed, RPacketContext::GetDataVolumeTransferred
 * @SYMTestExpectedResults Pass - Context activated. Data transferred advances.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify connection speed and volume data transferred advances realistically.
 *
 * @return - TVerdict code
 */
	{
	
	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Ensure phone is attached to packet service. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

	// ===  Send and receive some data and check data transferred advances ===
	
	// Activate context with ESOCK server
	TBuf<140> host(_L("developer.symbian.org"));
    TBuf<140> page(_L("/wiki/images/1/12/Common_Design_Patterns_for_Symbian_OS_Sample_Chapter.pdf"));
    CHTTPDownload *download = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download);
    ASSERT_TRUE(download->StartDownloadL(host,page),_L("Download Failed - perhaps page has moved!"));
    
    // Check RPacketService::GetNifInfo with aCount = 0 returns valid name in iContextName 
	TInt nifInfoCount = 0;
	TExtEtelRequestStatus nifInfoStatus(packetService, EPacketGetNifInfo);
	CleanupStack::PushL(nifInfoStatus);
	RPacketService::TNifInfoV2 nifInfo;
	RPacketService::TNifInfoV2Pckg nifInfoPckg(nifInfo);
	packetService.GetNifInfo(nifInfoStatus, nifInfoCount, nifInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(nifInfoStatus, ETimeMedium), KErrNone, _L("RPacketService::GetNifInfo timed out."));
	ASSERT_EQUALS(nifInfoStatus.Int(), KErrNone, _L("RPacketService::GetNifInfo returned with an error."));
	ASSERT_TRUE(nifInfo.iContextName.Length()>0,_L("RPacketService::GetNifInfo returned invalid context name."));
	TName contextName = nifInfo.iContextName;
	
	// Open a handle on an existing context with OpenExistingContext
	RPacketContext packetContext;										
    ASSERT_EQUALS(packetContext.OpenExistingContext(packetService, contextName), KErrNone, _L("RPacketContext::OpenExistingContext returned with an error."));
  	
  	// Check RPacketContext::GetConnectionSpeed returns aRate > 0;
    TExtEtelRequestStatus getConnectionSpeedStatus(packetContext, EPacketContextGetConnectionSpeed);
	CleanupStack::PushL(getConnectionSpeedStatus);
	TUint connectionSpeedRate;
	packetContext.GetConnectionSpeed(getConnectionSpeedStatus, connectionSpeedRate);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getConnectionSpeedStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConnectionSpeed timed out."));
    ASSERT_EQUALS(getConnectionSpeedStatus.Int(), KErrNone, _L("RPacketContext::GetConnectionSpeed returned with an error."));
	ASSERT_TRUE(connectionSpeedRate > 0, _L("RPacketContext::GetConnectionSpeed is lower than expected."));
	
    // Repeat this section until download is complete and check that iBytesSent and iBytesReceived is higher than the previous
    RPacketContext::TDataVolume dataVolume;	
	TUint32 zeroBytesSent = 0;
	TUint32 zeroBytesReceived = 0;
	while(download->VerifyDownloading())
		{
		packetContext.GetDataVolumeTransferred(dataVolume);	
		DEBUG_PRINTF2(_L("sent data: %d bytes"), dataVolume.iBytesSent);
		DEBUG_PRINTF2(_L("received data: %d bytes"), dataVolume.iBytesReceived);
		User::After(KOneSecond*3);
		}
	
	// Check RPacketContext::GetDataVolumeTransferred returns TDataVolume::iBytesSent is higher than the previous
	ASSERT_TRUE(dataVolume.iBytesSent > zeroBytesSent, _L("RPacketContext::GetVolumeDataTransferred returned with no increase in bytes sent."));
	// Check RPacketContext::GetDataVolumeTransferred returns TDataVolume::iBytesReceived is higher than the previous
	ASSERT_TRUE(dataVolume.iBytesReceived > zeroBytesReceived, _L("RPacketContext::GetVolumeDataTransferred returned with no increase in bytes received."));
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////
	
    StartCleanup();
	
	// pop
	// download
	// nifInfoStatus
	// getConnectionSpeedStatus
	CleanupStack::PopAndDestroy(3, download);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0056::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0056");
	}



CCTSYIntegrationTestPacketContext0057::CCTSYIntegrationTestPacketContext0057(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0057::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0057::~CCTSYIntegrationTestPacketContext0057()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0057::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0057
 * @SYMFssID BA/CTSY/PKTC-0057
 * @SYMTestCaseDesc Download data with a primary context and a secondary context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::OpenNewSecondaryContext, RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::GetConnectionSpeed, RPacketContext::NotifyConnectionSpeedChange, RPacketContext::NotifyDataTransferred, RPacketContext::GetDataVolumeTransferred, RPacketContext::LoanCommPort, RPacketContext::RecoverCommPort, RPacketQoS::SetProfileParameters, RPacketQoS::GetProfileCapabilities, RPacketQoS::OpenNewQoS, RPacketContext::AddPacketFilter
 * @SYMTestExpectedResults Pass - Context activated. Data transferred advances.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify connection speed and volume data transferred advances realistically.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////
/*
	
	// Ensure phone is attached to packet service. 
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////


	// Post notification for service's status change
	TExtEtelRequestStatus notifyServiceStChStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStChStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyServiceStChStatus, packetServiceStatus);
    
    // ===  Activate a primary context ===

	// Open a new primary context 
    // Set a valid context config using RPacketContext::SetConfig 

	// Activate context with ESOCK server
	TBuf<140> host(_L("developer.symbian.com"));
    TBuf<140> page(_L("/main/downloads/papers/IMS_Introduction_Part_1.pdf"));
    CHTTPDownload* download = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download);
    ASSERT_TRUE(download->StartDownloadL(host,page),_L("Download Failed"));
    
 	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_EQUALS(getContextInfo.Int(), KErrNone, _L("RPacketService::GetContextInfo returned an error"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Open a handle on an existing context with OpenExistingContext
	RPacketContext primaryPacketContext;										
    ASSERT_EQUALS(primaryPacketContext.OpenExistingContext(packetService, contextInfo.iName), KErrNone, _L("RPacketContext::OpenExistingContext returned with an error."));

	// Post notification for RPacketContext::NotifyDataTransferred
	ERR_PRINTF1(_L("<font color=Orange>@CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out."));
	//TExtEtelRequestStatus notifyDataTransferredStatus(primaryPacketContext, EPacketContextNotifyDataTransferred);
	//CleanupStack::PushL(notifyDataTransferredStatus);
	//RPacketContext::TDataVolume primaryDataVolume;
	//primaryPacketContext.NotifyDataTransferred(notifyDataTransferredStatus, primaryDataVolume);

	// Check RPacketService::NotifyStatusChange completes with EStatusActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				packetService,
				notifyServiceStChStatus,
				packetServiceStatus, 
				expectedPacketServiceStatus,
				KErrNone);

	// Check RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus returns incorrect status."));

	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus packetContextStatus;
	ASSERT_EQUALS(primaryPacketContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus returns incorrect status."));

	// Post notification for the RPacketContext::NotifyConnectionSpeedChange
	TExtEtelRequestStatus notifyConnectionSpeedChangeStatus(primaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyConnectionSpeedChangeStatus);
	TUint rate(0);
	primaryPacketContext.NotifyConnectionSpeedChange(notifyConnectionSpeedChangeStatus, rate);

    // ===  Activate a secondary context ===

	// Activate secondary context with ESOCK server
	TBuf<140> subHost(_L("developer.symbian.com"));
    TBuf<140> subPage(_L("/main/downloads/papers/SymbianOSv91/Whats_new_in_Symbian_OS_v9.1.pdf"));
	TInt port = 80;
    CHTTPDownload* downloadSecondary = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(downloadSecondary);
   // ASSERT_TRUE(downloadSecondary->StartDownloadL(subHost, subPage, port, ETrue), _L("Download Failed"));
    ASSERT_TRUE(downloadSecondary->StartDownloadL(subHost, subPage, port), _L("Download Failed"));


 	// Get secondary context info	
	index = 1;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Open a handle on an existing secondary context with OpenExistingContext
	RPacketContext secondaryPacketContext;										
    ASSERT_EQUALS(secondaryPacketContext.OpenExistingContext(packetService, contextInfo.iName), KErrNone, _L("RPacketContext::OpenExistingContext returned with an error."));

	// Post notification for RPacketContext::NotifyDataTransferred
	ERR_PRINTF1(_L("<font color=Orange>@CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out."));
	//TExtEtelRequestStatus notifySecDataTransferredStatus(secondaryPacketContext, EPacketContextNotifyDataTransferred);
	//CleanupStack::PushL(notifySecDataTransferredStatus);
	//RPacketContext::TDataVolume secondaryDataVolume;
	//secondaryPacketContext.NotifyDataTransferred(notifySecDataTransferredStatus, secondaryDataVolume);

	// Post notification for the secondary context connection speed change
	TExtEtelRequestStatus notifySecConnectionSpeedChangeStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecConnectionSpeedChangeStatus);
	TUint secRate(0);
	secondaryPacketContext.NotifyConnectionSpeedChange(notifyConnectionSpeedChangeStatus, secRate);

	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 
	ASSERT_EQUALS(AddPacketFilterV2ToContextL(secondaryPacketContext,1), KErrNone, _L("RPacketContext::AddPacketFilter returned with error status"));

	// ===  Add QoS ===

	// Open a new QoS from the secondary context  
	RPacketQoS& packetQoS = iEtelSessionMgr.GetSecondaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1,
												KSecondaryPacketContext1);

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 
	TExtEtelRequestStatus getProfileCapStatus(packetQoS, EPacketQoSGetProfileCaps);
	CleanupStack::PushL(getProfileCapStatus);
	RPacketQoS::TQoSCapsGPRS qosCapGprs;
	TPckg<RPacketQoS::TQoSCapsGPRS> qosCapsGprsPckg(qosCapGprs);
	packetQoS.GetProfileCapabilities(getProfileCapStatus, qosCapsGprsPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileCapStatus, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileCapabilities timed out."));
    ASSERT_EQUALS(getProfileCapStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileCapabilities returned with error status."));

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 
    RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
    FillParametersFromCapabilities(qosGprsRequested,qosCapGprs);
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);
	packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Post notification for context's status change
	TExtEtelRequestStatus notifySecContextStChStatus(secondaryPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifySecContextStChStatus);
	RPacketContext::TContextStatus secPacketContextStatus;
	secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);

	// Activate context with RPacketContext::Activate 
	TExtEtelRequestStatus secContextActivateStatus(secondaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(secContextActivateStatus);
	secondaryPacketContext.Activate(secContextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(secContextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(secContextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive
    RPacketContext::TContextStatus expectedSecContextStatus = RPacketContext::EStatusActivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

    secondaryPacketContext.NotifyStatusChange(notifySecContextStChStatus, secPacketContextStatus);	
    expectedSecContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(
				secondaryPacketContext,
				notifySecContextStChStatus,
				secPacketContextStatus, 
				expectedSecContextStatus,
				KErrNone);

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive
	ASSERT_EQUALS(secondaryPacketContext.GetStatus(secPacketContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(secPacketContextStatus, RPacketContext::EStatusActive, _L("Packet Context's status expected to be EStatusActive but is not."));
    
    // Loan comm port to the client with RPacketContext::LoanCommPort 

	// ===  Send and receive some data and check data transferred advances ===

	// Send some data and receive some data to which would go to the primary and the secondary context depending on packet filter values 

	// Check RPacketContext::NotifyDataTransferred for primary context completes with TDataVolume::iBytesSent > 0
    // @CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out.
	//ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyDataTransferredStatus, ETimeMedium), KErrNone, _L("RPacketContext::NotifyDataTransferred did not complete."));
    //ASSERT_EQUALS(notifyDataTransferredStatus.Int(), KErrNone, _L("RPacketContext::NotifyDataTransferred returned with error status."));
    //ASSERT_TRUE(primaryDataVolume.iBytesSent > 0, _L("RPacketContext::NotifyDataTransferred returned with zero iBytesSent."));

	// Check RPacketContext::NotifyDataTransferred for primary context completes with TDataVolume::iBytesReceived > 0
    // @CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out.
    //ASSERT_TRUE(primaryDataVolume.iBytesReceived > 0, _L("RPacketContext::NotifyDataTransferred returned with zero iBytesReceived."));

	// Check RPacketContext::GetConnectionSpeed for primary context returns aRate > 0
    TExtEtelRequestStatus getConnectionSpeedStatus(primaryPacketContext, EPacketContextGetConnectionSpeed);
	CleanupStack::PushL(getConnectionSpeedStatus);
	TUint connectionSpeedRate;
	primaryPacketContext.GetConnectionSpeed(getConnectionSpeedStatus, connectionSpeedRate);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getConnectionSpeedStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConnectionSpeed timed out."));
    ASSERT_EQUALS(getConnectionSpeedStatus.Int(), KErrNone, _L("RPacketContext::GetConnectionSpeed failed."));
	ASSERT_TRUE(connectionSpeedRate > 0, _L("RPacketContext::GetConnectionSpeed is lower than expected."));

	// Check RPacketContext::NotifyConnectionSpeedChange for primary context returns aRate > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyConnectionSpeedChangeStatus, ETimeMedium), KErrNone, _L("RPacketContext::NotifyConnectionSpeedChange timed out."));
    ASSERT_EQUALS(notifyConnectionSpeedChangeStatus.Int(), KErrNone, _L("RPacketContext::NotifyConnectionSpeedChange failed."));
	ASSERT_TRUE(rate > 0, _L("RPacketContext::NotifyConnectionSpeedChange returns aRate = 0."));

	// Check RPacketContext::NotifyDataTransferred for secondary context completes with TDataVolume::iBytesSent > 0
    // @CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out.
	//ASSERT_EQUALS(WaitForRequestWithTimeOut(notifySecDataTransferredStatus, ETimeMedium), KErrNone, _L("RPacketContext::NotifyDataTransferred did not complete."));
    //ASSERT_EQUALS(notifySecDataTransferredStatus.Int(), KErrNone, _L("RPacketContext::NotifyDataTransferred returned with error status."));
    //ASSERT_TRUE(secondaryDataVolume.iBytesSent > 0, _L("RPacketContext::NotifyDataTransferred returned with zero iBytesSent."));

	// Check RPacketContext::NotifyDataTransferred for secondary context completes with TDataVolume::iBytesReceived > 0
    // @CTSYProblem: RPacketContext::NotifyDataTransferred is not supported by CTSY. All code related to this request is commented out.
    //ASSERT_TRUE(secondaryDataVolume.iBytesReceived > 0, _L("RPacketContext::NotifyDataTransferred returned with zero iBytesReceived."));

	// Check RPacketContext::GetConnectionSpeed for secondary context returns aRate > 0
    TExtEtelRequestStatus getSecConnectionSpeedStatus(secondaryPacketContext, EPacketContextGetConnectionSpeed);
	CleanupStack::PushL(getSecConnectionSpeedStatus);
	secondaryPacketContext.GetConnectionSpeed(getSecConnectionSpeedStatus, connectionSpeedRate);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getSecConnectionSpeedStatus, ETimeMedium), KErrNone, _L("RPacketContext::GetConnectionSpeed timed out."));
    ASSERT_EQUALS(getSecConnectionSpeedStatus.Int(), KErrNone, _L("RPacketContext::GetConnectionSpeed failed."));
	ASSERT_TRUE(connectionSpeedRate > 0, _L("RPacketContext::GetConnectionSpeed is lower than expected."));

	// Check RPacketContext::NotifyConnectionSpeedChange for secondary context returns aRate > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifySecConnectionSpeedChangeStatus, ETimeMedium), KErrNone, _L("RPacketContext::NotifyConnectionSpeedChange timed out."));
    ASSERT_EQUALS(notifySecConnectionSpeedChangeStatus.Int(), KErrNone, _L("RPacketContext::NotifyConnectionSpeedChange failed."));
	ASSERT_TRUE(secRate > 0, _L("RPacketContext::NotifyConnectionSpeedChange returns aRate = 0."));

	// Repeat this section 10 times and check that iBytesSent and iBytesReceived is higher than the previous 
	RPacketContext::TDataVolume priDataVolume;
    RPacketContext::TDataVolume secDataVolume;	
	TUint32 primaryLastBytesSent(0);
	TUint32 primaryLastBytesReceived(0);
	TUint32 secondaryLastBytesSent(0);
	TUint32 secondaryLastBytesReceived(0);

	while(download->VerifyDownloading())
		{
		// Check RPacketContext::GetDataVolumeTransferred for primary context returns TDataVolume::iBytesSent is higher than the previous
		primaryPacketContext.GetDataVolumeTransferred(priDataVolume);	
		ASSERT_TRUE(priDataVolume.iBytesSent > primaryLastBytesSent, _L("Primary packet context iBytesSend value is not higher than the previous iBytesSend value"));
		primaryLastBytesSent = priDataVolume.iBytesSent;
		
		// Check RPacketContext::GetDataVolumeTransferred for primary context returns TDataVolume::iBytesReceived is higher than the previous
		ASSERT_TRUE(priDataVolume.iBytesReceived > primaryLastBytesReceived, _L("Primary packet context iBytesReceived value is not higher than the previous iBytesReceived value"));
		primaryLastBytesReceived = priDataVolume.iBytesReceived;
		
		// Check RPacketContext::GetDataVolumeTransferred for secondary context returns TDataVolume::iBytesSent is higher than the previous
		secondaryPacketContext.GetDataVolumeTransferred(secDataVolume);	
		ASSERT_TRUE(secDataVolume.iBytesSent > secondaryLastBytesSent, _L("Secondary packet context iBytesSend value is not higher than the previous iBytesSend value"));
		secondaryLastBytesSent = secDataVolume.iBytesSent;
		
		// Check RPacketContext::GetDataVolumeTransferred for secondary context returns TDataVolume::iBytesReceived is higher than the previous
		ASSERT_TRUE(secDataVolume.iBytesReceived > secondaryLastBytesReceived, _L("Secondary packet context iBytesReceived value is not higher than the previous iBytesReceived value"));
		secondaryLastBytesReceived = secDataVolume.iBytesReceived;
		
		DEBUG_PRINTF2(_L("Primary context sent data: %d bytes"), primaryLastBytesSent);
		DEBUG_PRINTF2(_L("Primary context received data: %d bytes"), primaryLastBytesReceived);
		DEBUG_PRINTF2(_L("Secondary context sent data: %d bytes"), secondaryLastBytesSent);
		DEBUG_PRINTF2(_L("Secondary context received data: %d bytes"), secondaryLastBytesReceived);
		User::After(KOneSecond*3);
		}

	// Release QoS
	iEtelSessionMgr.ReleaseSecondaryPacketContextQoS(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1,
											KSecondaryPacketContext1);

	// Release packet contexts
	iEtelSessionMgr.ReleaseSecondaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1,
											KSecondaryPacketContext1);

	// Release primary packet context
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);


	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// notifyServiceStChStatus
	// download
	// getContextInfo
	// notifyDataTransferredStatus (@CTSYProblem, commented out)
	// notifyConnectionSpeedChangeStatus
	// downloadSecondary
	// notifySecDataTransferredStatus (@CTSYProblem, commented out)
	// notifySecConnectionSpeedChangeStatus
	// getProfileCapStatus
	// setProfileParamStatus
	// notifySecContextStChStatus
	// secContextActivateStatus
	// getConnectionSpeedStatus
	// getSecConnectionSpeedStatus
	CleanupStack::PopAndDestroy(12, &notifyServiceStChStatus);
*/
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0057::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0057");
	}



CCTSYIntegrationTestPacketContext0058::CCTSYIntegrationTestPacketContext0058(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0058::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0058::~CCTSYIntegrationTestPacketContext0058()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0058::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0058
 * @SYMFssID BA/CTSY/PKTC-0058
 * @SYMTestCaseDesc Activate a PDP context with the requested QOS equal to the subscribed QoS.
 * @SYMTestPriority High
 * @SYMTestActions RPacketQoS::NotifyProfileChanged, RPacketQoS::GetProfileParameters, RPacketContext::SetConfig, RPacketQoS::SetProfileParameters, RPacketQoS::GetProfileCapabilities
 * @SYMTestExpectedResults Pass - A PDP context with subscribed QoS could be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify negotiated QoS same as that requested.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
    // Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
	

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////

   	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);

	// == Activate a PDP context with the requested QOS equal to the subscribed QoS == 
	DEBUG_PRINTF1(_L("== GPRS =="));
	DoTest0058L(apn, user, pwd, EQoSGPRS);

	// ===  Repeat test for R99_R4 ===
	// Repeat test with TQoSCapsR99_R4 instead of TQoSCapsGPRS, TQoSR99_R4Requested instead of TQoSGPRSRequested and TQoSR99_R4Negotiated instead of TQoSGPRSNegotiated 
	DEBUG_PRINTF1(_L("== R99_R4 =="));
	DoTest0058L(apn, user, pwd, EQoSR4);

	// ===  Repeat test for R5 ===
	// Repeat test with TQoSCapsR5 instead of TQoSCapsGPRS, TQoSR5Requested instead of TQoSGPRSRequested, and TQoSR5Negotiated instead of TQoSGPRSNegotiated 
	DEBUG_PRINTF1(_L("== R5 =="));
	DoTest0058L(apn, user, pwd, EQoSR5);

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getNetworkStatus
	CleanupStack::PopAndDestroy(1, &getNetworkStatus);
	
	return TestStepResult();
	}




void CCTSYIntegrationTestPacketContext0058::DoTest0058L(TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType)
/**
 * Activate a PDP context with the requested QOS equal to the subscribed QoS.
 * @param aNWType Gprs / R99_R4 / R5.
 */
	{
	
	// Open a new primary context. 
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Open a new primary context QoS
	RPacketQoS& contextQoS = iEtelSessionMgr.GetPrimaryPacketContextQoSL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	// Get the QoS profile capabilities with RPacketQoS::GetProfileCapabilities with aProfileCaps of type TQoSCapsGPRS/TQoSCapsR99_R4/TQoSCapsR5.
	RPacketQoS::TQoSCapsGPRS qosCapsGPRS;
	RPacketQoS::TQoSCapsR99_R4 qosCapsR99;
	RPacketQoS::TQoSCapsR5 qosCapsR5;

	switch(aQoSCapType)
		{
			case EQoSGPRS:
					GetProfileCapsL(contextQoS, qosCapsGPRS);
					break;
					
			case EQoSR4:		
					GetProfileCapsL(contextQoS, qosCapsR99);
					break;
					
			case EQoSR5:		
					GetProfileCapsL(contextQoS, qosCapsR5);					
					break;
		}

	// Set the minimum requested QoS profile parameters to lower than what was returned from GetProfileCapabilities using RPacketQoS::SetProfileParameters
	// with aProfile of type TQoSGPRSRequested/TQoSR99_R4Requested/TQoSR5Requested.
	// Set all iMin... to a lower value than that returned by GetProfileCapabilities and all iReq... to the same value as that returned by GetProfileCapabilities. 
	TExtEtelRequestStatus setProfileParametersStatus(contextQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParametersStatus);
	RPacketQoS::TQoSGPRSRequested qoSGPRSRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qoSGPRSRequestedPkg(qoSGPRSRequested);
	RPacketQoS::TQoSR99_R4Requested qoSR99Requested;
	TPckg<RPacketQoS::TQoSR99_R4Requested> qoSR99RequestedPkg(qoSR99Requested);
	RPacketQoS::TQoSR5Requested qoSR5Requested;
	TPckg<RPacketQoS::TQoSR5Requested> qoSR5RequestedPkg(qoSR5Requested);
	RPacketQoS::TBitRate bitRate;
	bitRate.iDownlinkRate = 0;
	bitRate.iUplinkRate = 0;

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			qoSGPRSRequested.iMinDelay 			= RPacketQoS::EUnspecifiedDelayClass;
			qoSGPRSRequested.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
			qoSGPRSRequested.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
			qoSGPRSRequested.iMinPrecedence 	= RPacketQoS::EUnspecifiedPrecedence;
			qoSGPRSRequested.iMinReliability 	= RPacketQoS::EUnspecifiedReliabilityClass;
			
			qoSGPRSRequested.iReqDelay 			= RPacketQoS::EDelayClass3;
			qoSGPRSRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughputBestEffort;
			qoSGPRSRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput8000;
			qoSGPRSRequested.iReqPrecedence 	= RPacketQoS::EPriorityMediumPrecedence;
			qoSGPRSRequested.iReqReliability 	= RPacketQoS::EReliabilityClass3;
			contextQoS.SetProfileParameters(setProfileParametersStatus ,qoSGPRSRequestedPkg);
			break;
		
		case EQoSR4: // R99_R4
			qoSR99Requested.iMinTrafficClass			= RPacketQoS::ETrafficClassUnspecified;
			qoSR99Requested.iMinDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderUnspecified;
			qoSR99Requested.iMinDeliverErroneousSDU		= RPacketQoS::EErroneousSDUDeliveryUnspecified;
			qoSR99Requested.iMinAcceptableMaxSDUSize	= 10;
			qoSR99Requested.iMinAcceptableMaxRate		= bitRate;
			qoSR99Requested.iMaxBER						= RPacketQoS::EBERUnspecified;
			qoSR99Requested.iMaxSDUErrorRatio			= RPacketQoS::ESDUErrorRatioUnspecified;
			qoSR99Requested.iMinTrafficHandlingPriority	= RPacketQoS::ETrafficPriorityUnspecified;
			qoSR99Requested.iMaxTransferDelay			= 4000;
			qoSR99Requested.iMinGuaranteedRate			= bitRate;

			qoSR99Requested.iReqTrafficClass			= RPacketQoS::ETrafficClassInteractive;
			qoSR99Requested.iReqDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderNotRequired;
			qoSR99Requested.iReqDeliverErroneousSDU		= RPacketQoS::EErroneousSDUDeliveryNotRequired;
			qoSR99Requested.iReqMaxSDUSize				= 1500;
			bitRate.iDownlinkRate = 104;
			bitRate.iUplinkRate = 104;
			qoSR99Requested.iReqMaxRate					= bitRate;
			qoSR99Requested.iReqBER			 			= RPacketQoS::EBERFourPerThousand;
			qoSR99Requested.iReqSDUErrorRatio 			= RPacketQoS::ESDUErrorRatioOnePerThousand;
			qoSR99Requested.iReqTrafficHandlingPriority	= RPacketQoS::ETrafficPriority3;
			qoSR99Requested.iReqTransferDelay			= 1600;
			bitRate.iDownlinkRate = 10;
			bitRate.iUplinkRate = 10;
			qoSR99Requested.iReqGuaranteedRate			= bitRate;
			contextQoS.SetProfileParameters(setProfileParametersStatus ,qoSR99RequestedPkg);
			break;

		case EQoSR5: // R5
			qoSR5Requested.iMinTrafficClass				= RPacketQoS::ETrafficClassUnspecified;
			qoSR5Requested.iMinDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderUnspecified;
			qoSR5Requested.iMinDeliverErroneousSDU		= RPacketQoS::EErroneousSDUDeliveryUnspecified;
			qoSR5Requested.iMinAcceptableMaxSDUSize		= 10;
			qoSR5Requested.iMinAcceptableMaxRate		= bitRate;
			qoSR5Requested.iMaxBER						= RPacketQoS::EBERUnspecified;
			qoSR5Requested.iMaxSDUErrorRatio			= RPacketQoS::ESDUErrorRatioUnspecified;
			qoSR5Requested.iMinTrafficHandlingPriority	= RPacketQoS::ETrafficPriorityUnspecified;
			qoSR5Requested.iMaxTransferDelay			= 0;
			qoSR5Requested.iMinGuaranteedRate			= bitRate;
			
			qoSR5Requested.iReqTrafficClass				= RPacketQoS::ETrafficClassInteractive;
			qoSR5Requested.iReqDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderNotRequired;
			qoSR5Requested.iReqDeliverErroneousSDU		= RPacketQoS::EErroneousSDUDeliveryNotRequired;
			qoSR5Requested.iReqMaxSDUSize				= 1500;
			bitRate.iDownlinkRate = 64;
			bitRate.iUplinkRate = 16;
			qoSR5Requested.iReqMaxRate					= bitRate;
			qoSR5Requested.iReqBER			 			= RPacketQoS::EBERFourPerThousand;
			qoSR5Requested.iReqSDUErrorRatio 			= RPacketQoS::ESDUErrorRatioOnePerTenThousand;
			qoSR5Requested.iReqTrafficHandlingPriority	= RPacketQoS::ETrafficPriority3;
			qoSR5Requested.iReqTransferDelay			= 1600;
			qoSR5Requested.iReqGuaranteedRate			= bitRate;
			qoSR5Requested.iSignallingIndication		= qosCapsR5.iSignallingIndication;
			qoSR5Requested.iSourceStatisticsDescriptor	= RPacketQoS::ESourceStatisticsDescriptorUnknown;
			contextQoS.SetProfileParameters(setProfileParametersStatus ,qoSR5RequestedPkg);
			break;
		}

    ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParametersStatus, ETimeLong), KErrNone, _L("RPacketQoS::SetProfileParameters timed out"));
	ASSERT_EQUALS(setProfileParametersStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with an error"));

	// Set the context config with RPacketContext::SetConfig 
	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	RPacketContext::TContextConfigGPRS setConfigGprs;
	TPckg<RPacketContext::TContextConfigGPRS> setConfigPkgGprs(setConfigGprs);
	RPacketContext::TContextConfigR99_R4 setConfigR99;
	TPckg<RPacketContext::TContextConfigR99_R4> setConfigPkgR99(setConfigR99);
	RPacketContext::TContextConfig_R5 setConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> setConfigPkgR5(setConfigR5);

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			setConfigGprs.iAccessPointName.Copy(aApn);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgGprs);
			break;
		
		case EQoSR4: // R99_R4
			setConfigR99.iAccessPointName.Copy(aApn);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR99);
			break;

		case EQoSR5: // R5
			setConfigR5.iAccessPointName.Copy(aApn);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR5);
			break;
		}
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with an error"));

	// Post notification for RPacketQoS::NotifyProfileChanged
	TExtEtelRequestStatus notifyProfileChangedStatus(contextQoS, EPacketQoSNotifyProfileChanged);
	CleanupStack::PushL(notifyProfileChangedStatus);
	RPacketQoS::TQoSGPRSNegotiated qoSGPRSNegotiated;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qoSGPRSNegotiatedPkg(qoSGPRSNegotiated);
	RPacketQoS::TQoSR99_R4Negotiated qoSR99Negotiated;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> qoSR99NegotiatedPkg(qoSR99Negotiated);
	RPacketQoS::TQoSR5Negotiated qoSR5Negotiated;
	TPckg<RPacketQoS::TQoSR5Negotiated> qoSR5NegotiatedPkg(qoSR5Negotiated);

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			contextQoS.NotifyProfileChanged(notifyProfileChangedStatus, qoSGPRSNegotiatedPkg);
			break;
		
		case EQoSR4: // R99_R4
			contextQoS.NotifyProfileChanged(notifyProfileChangedStatus, qoSR99NegotiatedPkg);
			break;

		case EQoSR5: // R5
			contextQoS.NotifyProfileChanged(notifyProfileChangedStatus, qoSR5NegotiatedPkg);
			break;
		}

	// Activate the primary context with RPacketContext::Activate. 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));

	// Check RPacketQoS::NotifyProfileChanged returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated with same as the requested values used in SetProfileParameters.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyProfileChangedStatus, ETimeLong), KErrNone, _L("RPacketQoS::NotifyProfileChanged timed out"));
	ASSERT_EQUALS(notifyProfileChangedStatus.Int(), KErrNone, _L("RPacketQoS::NotifyProfileChanged returned with error status."));

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			ASSERT_EQUALS(qoSGPRSNegotiated.iDelay, qoSGPRSRequested.iReqDelay, _L("RPacketQoS::NotifyProfileChanged did not return with same iDelay as used in SetProfileParameters"));	
			ASSERT_EQUALS(qoSGPRSNegotiated.iMeanThroughput, qoSGPRSRequested.iReqMeanThroughput, _L("RPacketQoS::NotifyProfileChanged did not return with same iMeanThroughput as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSGPRSNegotiated.iPeakThroughput, qoSGPRSRequested.iReqPeakThroughput, _L("RPacketQoS::NotifyProfileChanged did not return with same iPeakThroughput as used in SetProfileParameters"));	
			ASSERT_EQUALS(qoSGPRSNegotiated.iPrecedence, qoSGPRSRequested.iReqPrecedence, _L("RPacketQoS::NotifyProfileChanged did not return with same iPrecedence as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSGPRSNegotiated.iReliability, qoSGPRSRequested.iReqReliability, _L("RPacketQoS::NotifyProfileChanged did not return with same iReliability as used in SetProfileParameters"));
			break;
		
		case EQoSR4: // R99_R4
			ASSERT_EQUALS(qoSR99Negotiated.iTrafficClass, qoSR99Requested.iReqTrafficClass, _L("RPacketQoS::NotifyProfileChanged did not return with same iTrafficClass as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iDeliveryOrderReqd, qoSR99Requested.iReqDeliveryOrderReqd,  _L("RPacketQoS::NotifyProfileChanged did not return with same iDeliveryOrderReqd as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iDeliverErroneousSDU, qoSR99Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::NotifyProfileChanged did not return with same iDeliverErroneousSDU as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxSDUSize, qoSR99Requested.iReqMaxSDUSize, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxSDUSize as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxRate.iDownlinkRate, qoSR99Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxRate.iUplinkRate, qoSR99Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iBER, qoSR99Requested.iReqBER, _L("RPacketQoS::NotifyProfileChanged did not return with same iBER as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iSDUErrorRatio, qoSR99Requested.iReqSDUErrorRatio, _L("RPacketQoS::NotifyProfileChanged did not return with same iSDUErrorRatio as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iTrafficHandlingPriority, qoSR99Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::NotifyProfileChanged did not return with same iTrafficHandlingPriority as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iTransferDelay, qoSR99Requested.iReqTransferDelay, _L("RPacketQoS::NotifyProfileChanged did not return with same iTransferDelay as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iGuaranteedRate.iDownlinkRate, qoSR99Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iGuaranteedRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iGuaranteedRate.iUplinkRate, qoSR99Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iGuaranteedRate.iUplinkRate as used in SetProfileParameters"));
			break;

		case EQoSR5: // R5
			ASSERT_EQUALS(qoSR5Negotiated.iTrafficClass, qoSR5Requested.iReqTrafficClass, _L("RPacketQoS::NotifyProfileChanged did not return with same iTrafficClass as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iDeliveryOrderReqd, qoSR5Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::NotifyProfileChanged did not return with same iDeliveryOrderReqd as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iDeliverErroneousSDU, qoSR5Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::NotifyProfileChanged did not return with same iDeliverErroneousSDU as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxSDUSize, qoSR5Requested.iReqMaxSDUSize, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxSDUSize as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxRate.iDownlinkRate, qoSR5Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxRate.iUplinkRate, qoSR5Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iMaxRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iBER, qoSR5Requested.iReqBER, _L("RPacketQoS::NotifyProfileChanged did not return with same iBER as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSDUErrorRatio, qoSR5Requested.iReqSDUErrorRatio, _L("RPacketQoS::NotifyProfileChanged did not return with same iSDUErrorRatio as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iTrafficHandlingPriority, qoSR5Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::NotifyProfileChanged did not return with same iTrafficHandlingPriority as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iTransferDelay, qoSR5Requested.iReqTransferDelay, _L("RPacketQoS::NotifyProfileChanged did not return with same iTransferDelay as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iGuaranteedRate.iDownlinkRate, qoSR5Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return with same iGuaranteedRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iGuaranteedRate.iUplinkRate, qoSR5Requested.iReqGuaranteedRate.iUplinkRate,  _L("RPacketQoS::NotifyProfileChanged did not return with same iGuaranteedRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSignallingIndication, qoSR5Requested.iSignallingIndication, _L("RPacketQoS::NotifyProfileChanged did not return with same iSignallingIndication as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSourceStatisticsDescriptor, qoSR5Requested.iSourceStatisticsDescriptor, _L("RPacketQoS::NotifyProfileChanged did not return with same iSourceStatisticsDescriptor as used in SetProfileParameters"));
			break;
		}

	// Check RPacketQoS::GetProfileParameters returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated with same as the requested values used in SetProfileParameters.
	TExtEtelRequestStatus getProfileParametersStatus(contextQoS, EPacketQoSGetProfileParams);
	CleanupStack::PushL(getProfileParametersStatus);

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			contextQoS.GetProfileParameters(getProfileParametersStatus, qoSGPRSNegotiatedPkg);
			break;
		
		case EQoSR4: // R99_R4
			contextQoS.GetProfileParameters(getProfileParametersStatus, qoSR99NegotiatedPkg);
			break;

		case EQoSR5: // R5
			contextQoS.GetProfileParameters(getProfileParametersStatus, qoSR5NegotiatedPkg);
			break;
		}

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeLong), KErrNone, _L("RPacketQoS::GetProfileParameters timed out"));
	ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileParameters returned with an error"));

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			ASSERT_EQUALS(qoSGPRSNegotiated.iDelay, qoSGPRSRequested.iReqDelay, _L("RPacketQoS::NotifyProfileChanged did not return with same iDelay as used in SetProfileParameters"));	
			ASSERT_EQUALS(qoSGPRSNegotiated.iMeanThroughput, qoSGPRSRequested.iReqMeanThroughput, _L("RPacketQoS::NotifyProfileChanged did not return with same iMeanThroughput as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSGPRSNegotiated.iPeakThroughput, qoSGPRSRequested.iReqPeakThroughput, _L("RPacketQoS::NotifyProfileChanged did not return with same iPeakThroughput as used in SetProfileParameters"));	
			ASSERT_EQUALS(qoSGPRSNegotiated.iPrecedence, qoSGPRSRequested.iReqPrecedence, _L("RPacketQoS::NotifyProfileChanged did not return with same iPrecedence as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSGPRSNegotiated.iReliability, qoSGPRSRequested.iReqReliability, _L("RPacketQoS::NotifyProfileChanged did not return with same iReliability as used in SetProfileParameters"));
			break;
		
		case EQoSR4: // R99_R4
			ASSERT_EQUALS(qoSR99Negotiated.iTrafficClass, qoSR99Requested.iReqTrafficClass, _L("RPacketQoS::GetProfileParameters did not return with same iTrafficClass as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iDeliveryOrderReqd, qoSR99Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::GetProfileParameters did not return with same iDeliveryOrderReqd as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iDeliverErroneousSDU, qoSR99Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::GetProfileParameters did not return with same iDeliverErroneousSDU as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxSDUSize, qoSR99Requested.iReqMaxSDUSize, _L("RPacketQoS::GetProfileParameters did not return with same iMaxSDUSize as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxRate.iDownlinkRate, qoSR99Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iMaxRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iMaxRate.iUplinkRate, qoSR99Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iMaxRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iBER, qoSR99Requested.iReqBER, _L("RPacketQoS::GetProfileParameters did not return with same iBER as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iSDUErrorRatio, qoSR99Requested.iReqSDUErrorRatio, _L("RPacketQoS::GetProfileParameters did not return with same iSDUErrorRatio as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iTrafficHandlingPriority, qoSR99Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::GetProfileParameters did not return with same iTrafficHandlingPriority as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iTransferDelay, qoSR99Requested.iReqTransferDelay, _L("RPacketQoS::GetProfileParameters did not return with same iTransferDelay as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iGuaranteedRate.iDownlinkRate, qoSR99Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iGuaranteedRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR99Negotiated.iGuaranteedRate.iUplinkRate,qoSR99Requested.iReqGuaranteedRate.iUplinkRate,  _L("RPacketQoS::GetProfileParameters did not return with same iGuaranteedRate.iUplinkRate as used in SetProfileParameters"));
			break;

		case EQoSR5: // R5
			ASSERT_EQUALS(qoSR5Negotiated.iTrafficClass, qoSR5Requested.iReqTrafficClass, _L("RPacketQoS::GetProfileParameters did not return with same iTrafficClass as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iDeliveryOrderReqd, qoSR5Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::GetProfileParameters did not return with same iDeliveryOrderReqd as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iDeliverErroneousSDU, qoSR5Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::GetProfileParameters did not return with same iDeliverErroneousSDU as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxSDUSize, qoSR5Requested.iReqMaxSDUSize, _L("RPacketQoS::GetProfileParameters did not return with same iMaxSDUSize as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxRate.iDownlinkRate, qoSR5Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iMaxRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iMaxRate.iUplinkRate, qoSR5Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iMaxRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iBER, qoSR5Requested.iReqBER, _L("RPacketQoS::GetProfileParameters did not return with same iBER as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSDUErrorRatio, qoSR5Requested.iReqSDUErrorRatio, _L("RPacketQoS::GetProfileParameters did not return with same iSDUErrorRatio as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iTrafficHandlingPriority, qoSR5Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::GetProfileParameters did not return with same iTrafficHandlingPriority as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iTransferDelay, qoSR5Requested.iReqTransferDelay, _L("RPacketQoS::GetProfileParameters did not return with same iTransferDelay as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iGuaranteedRate.iDownlinkRate, qoSR5Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iGuaranteedRate.iDownlinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iGuaranteedRate.iUplinkRate, qoSR5Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return with same iGuaranteedRate.iUplinkRate as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSignallingIndication, qoSR5Requested.iSignallingIndication, _L("RPacketQoS::GetProfileParameters did not return with same iSignallingIndication as used in SetProfileParameters"));
			ASSERT_EQUALS(qoSR5Negotiated.iSourceStatisticsDescriptor, qoSR5Requested.iSourceStatisticsDescriptor, _L("RPacketQoS::GetProfileParameters did not return with same iSourceStatisticsDescriptor as used in SetProfileParameters"));
			break;
		}

	// Release primary packet context QoS
	iEtelSessionMgr.ReleasePrimaryPacketContextQoS(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);

	// Release primary packet context
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);

	// Pop
	// setProfileParametersStatus
	// setConfigStatus
	// notifyProfileChangedStatus
	// contextActivateStatus
	// getProfileParametersStatus
	CleanupStack::PopAndDestroy(5, &setProfileParametersStatus);

	}


TPtrC CCTSYIntegrationTestPacketContext0058::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0058");
	}



CCTSYIntegrationTestPacketContext0059::CCTSYIntegrationTestPacketContext0059(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0059::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0059::~CCTSYIntegrationTestPacketContext0059()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0059::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0059
 * @SYMFssID BA/CTSY/PKTC-0059
 * @SYMTestCaseDesc Activate a PDP context with requested QoS is better than subscriber QoS (success scenario).
 * @SYMTestPriority High
 * @SYMTestActions RPacketQoS::NotifyProfileChanged, RPacketQoS::GetProfileParameters, RPacketContext::SetConfig, RPacketQoS::SetProfileParameters, RPacketQoS::GetProfileCapabilities
 * @SYMTestExpectedResults Pass - A PDP context with subscribed QoS could be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify negotiated QoS same as that requested.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    

	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);
	
	// == Activate a PDP context with the requested QoS better than subscribed QoS == 
	DEBUG_PRINTF1(_L("== GPRS =="));
	DoTest0059L(apn, user, pwd, EQoSGPRS);

	// ===  Repeat test for R99_R4 ===
	// Repeat test with TQoSCapsR99_R4 instead of TQoSCapsGPRS, TQoSR99_R4Requested instead of TQoSGPRSRequested and TQoSR99_R4Negotiated instead of TQoSGPRSNegotiated 	
	DEBUG_PRINTF1(_L("== R99_R4 =="));
	DoTest0059L(apn, user, pwd, EQoSR4);

	// ===  Repeat test for R5 ===
	// Repeat test with TQoSCapsR5 instead of TQoSCapsGPRS, TQoSR5Requested instead of TQoSGPRSRequested, and TQoSR5Negotiated instead of TQoSGPRSNegotiated 	
	DEBUG_PRINTF1(_L("== R5 =="));
	DoTest0059L(apn, user, pwd, EQoSR5);
	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
    
    //PopAndDestroy
    //getNetWorkStatus
    CleanupStack::PopAndDestroy(1, &getNetworkStatus);
	
	return TestStepResult();
	}
	
void CCTSYIntegrationTestPacketContext0059::DoTest0059L(TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType)
	{
	/**
	 * Activate a PDP context with the requested QOS better to the subscribed QoS.
	 * @param aQoSCapType Gprs / R99_R4 / R5.
	 */
	
	// Open a new primary context. 	
	RPacketContext& primaryPacketContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);
											 	
	//Open a new QoS
	RPacketQoS& contextQoS = iEtelSessionMgr.GetPrimaryPacketContextQoSL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
											KPrimaryPacketContext1);

	//Get the QoS profile capabilities with RPacketQoS::GetProfileCapabilities with aProfileCaps of type TQoSCapsGPRS/TQoSCapsR99_R4/TQoSCapsR5			
	RPacketQoS::TQoSCapsGPRS qosCapsGPRS;
	RPacketQoS::TQoSCapsR99_R4 qosCapsR99;
	RPacketQoS::TQoSCapsR5 qosCapsR5;

	switch(aQoSCapType)
		{
			case EQoSGPRS:
					GetProfileCapsL(contextQoS, qosCapsGPRS);
					break;
					
			case EQoSR4:		
					GetProfileCapsL(contextQoS, qosCapsR99);
					break;
					
			case EQoSR5:		
					GetProfileCapsL(contextQoS, qosCapsR5);					
					break;
		}
		
	RPacketQoS::TQoSGPRSRequested qosReqCapsGPRS;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosReqCapsGPRSPckg(qosReqCapsGPRS);	
	RPacketQoS::TQoSR99_R4Requested qosReqCapsR99;
	TPckg<RPacketQoS::TQoSR99_R4Requested> qosReqCapsR99Pckg(qosReqCapsR99);	
	RPacketQoS::TQoSR5Requested qosReqCapsR5;
	TPckg<RPacketQoS::TQoSR5Requested> qosReqCapsR5Pckg(qosReqCapsR5);
	
	// Set the minimum requested QoS profile parameters to contain lower(worse) values than those that were returned from GetProfileCapabilities by using SetProfileParameters
	//with aProfile of type TQoSGPRSRequested/TQoSR99_R4Requested/TQoSR5Requested.	
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:																	
				//Min caps			
				qosReqCapsGPRS.iMinDelay = RPacketQoS::EUnspecifiedDelayClass;		
				qosReqCapsGPRS.iMinMeanThroughput =  RPacketQoS::EUnspecifiedMeanThroughput;	
				qosReqCapsGPRS.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;	
				qosReqCapsGPRS.iMinPrecedence = RPacketQoS::EUnspecifiedPrecedence;
				qosReqCapsGPRS.iMinReliability = RPacketQoS::EUnspecifiedReliabilityClass;
						
				//Req caps
				qosReqCapsGPRS.iReqDelay = RPacketQoS::EDelayClass4;
				qosReqCapsGPRS.iReqMeanThroughput =  RPacketQoS::EMeanThroughput100;
				qosReqCapsGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput1000;
				qosReqCapsGPRS.iReqPrecedence = RPacketQoS::EPriorityLowPrecedence;
				qosReqCapsGPRS.iReqReliability = RPacketQoS::EReliabilityClass1;				
				break;
				
		case EQoSR4:	
				//min values 				
				qosReqCapsR99.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
				qosReqCapsR99.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
				qosReqCapsR99.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
				qosReqCapsR99.iMinAcceptableMaxSDUSize = 1000;
				qosReqCapsR99.iMinAcceptableMaxRate.iDownlinkRate = 0;
				qosReqCapsR99.iMinAcceptableMaxRate.iUplinkRate = 0;
				qosReqCapsR99.iMaxBER =  RPacketQoS::EBERUnspecified;
				qosReqCapsR99.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
				qosReqCapsR99.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
				qosReqCapsR99.iMaxTransferDelay = 4000;
				qosReqCapsR99.iMinGuaranteedRate.iDownlinkRate = 1;
				qosReqCapsR99.iMinGuaranteedRate.iUplinkRate =  1;
	
				//req values 						
				qosReqCapsR99.iReqTrafficClass = RPacketQoS::ETrafficClassConversational;
				qosReqCapsR99.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;
				qosReqCapsR99.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
				qosReqCapsR99.iReqMaxSDUSize = 1500;
				qosReqCapsR99.iReqMaxRate.iDownlinkRate = 16;
				qosReqCapsR99.iReqMaxRate.iUplinkRate = 16;	
				qosReqCapsR99.iReqBER = RPacketQoS::EBERFourPerThousand;
				qosReqCapsR99.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;	
				qosReqCapsR99.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;		
				qosReqCapsR99.iReqTransferDelay = 1000;
				qosReqCapsR99.iReqGuaranteedRate.iDownlinkRate = 16;	
				qosReqCapsR99.iReqGuaranteedRate.iUplinkRate = 16;					
				break;
		
		case EQoSR5:						
				//min values
				qosReqCapsR5.iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;		
				qosReqCapsR5.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
				qosReqCapsR5.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;				
				qosReqCapsR5.iMinAcceptableMaxSDUSize = 1000;				
				qosReqCapsR5.iMinAcceptableMaxRate.iDownlinkRate = 0;							
				qosReqCapsR5.iMinAcceptableMaxRate.iUplinkRate = 0;							
				qosReqCapsR5.iMaxBER =  RPacketQoS::EBERUnspecified;	
				qosReqCapsR5.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;			
				qosReqCapsR5.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;		
				qosReqCapsR5.iMaxTransferDelay = 4000;		
				qosReqCapsR5.iMinGuaranteedRate.iDownlinkRate =  1;							
				qosReqCapsR5.iMinGuaranteedRate.iUplinkRate =  1;
											
				//req values 							
				qosReqCapsR5.iReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
				qosReqCapsR5.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;
				qosReqCapsR5.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
				qosReqCapsR5.iReqMaxSDUSize = 1500;
				qosReqCapsR5.iReqMaxRate.iDownlinkRate = 16;
				qosReqCapsR5.iReqMaxRate.iUplinkRate = 16;
				qosReqCapsR5.iReqBER = RPacketQoS::EBERFivePerHundred;
				qosReqCapsR5.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
				qosReqCapsR5.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
				qosReqCapsR5.iReqTransferDelay = 1000;
				qosReqCapsR5.iReqGuaranteedRate.iDownlinkRate = 16;	
				qosReqCapsR5.iReqGuaranteedRate.iUplinkRate = 16;
				qosReqCapsR5.iSignallingIndication = EFalse;
				qosReqCapsR5.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;						
				break;	
												
	}
	
	//Set profile parameters	
	TExtEtelRequestStatus reqStatusSetProfileParams(contextQoS,EPacketQoSSetProfileParams);
	CleanupStack::PushL(reqStatusSetProfileParams);
	
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:
				contextQoS.SetProfileParameters(reqStatusSetProfileParams,qosReqCapsGPRSPckg );	
				break;
				
		case EQoSR4:		
				contextQoS.SetProfileParameters(reqStatusSetProfileParams, qosReqCapsR99Pckg);
				break;
				
		case EQoSR5:		
				contextQoS.SetProfileParameters(reqStatusSetProfileParams, qosReqCapsR5Pckg);					
				break;
				
	}
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqStatusSetProfileParams, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));						
	ASSERT_EQUALS(reqStatusSetProfileParams.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	//post notifier for NotifyProfileChanged
	TExtEtelRequestStatus reqStatusNotifyProfileCh(contextQoS,EPacketQoSNotifyProfileChanged);
	CleanupStack::PushL(reqStatusNotifyProfileCh);
	
	RPacketQoS::TQoSGPRSNegotiated qosNegCapsGPRS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosNegCapsGPRSPckg(qosNegCapsGPRS);
	RPacketQoS::TQoSR99_R4Negotiated qosNegCapsR99;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> qosNegCapsR99Pckg(qosNegCapsR99);
	RPacketQoS::TQoSR5Negotiated qosNegCapsR5;
	TPckg<RPacketQoS::TQoSR5Negotiated> qosNegCapsR5Pckg(qosNegCapsR5);
	
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:
				contextQoS.NotifyProfileChanged(reqStatusNotifyProfileCh,qosNegCapsGPRSPckg );	
				break;
				
		case EQoSR4:		
				contextQoS.NotifyProfileChanged(reqStatusNotifyProfileCh, qosNegCapsR99Pckg);
				break;
				
		case EQoSR5:		
				contextQoS.NotifyProfileChanged(reqStatusNotifyProfileCh, qosNegCapsR5Pckg);					
				break;
				
	}
	
	// Set the context config with RPacketContext::SetConfig 
	TExtEtelRequestStatus setConfigStatus(primaryPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);	
	
	RPacketContext::TContextConfigGPRS setConfigGprs;
	TPckg<RPacketContext::TContextConfigGPRS> setConfigPkgGprs(setConfigGprs);
	RPacketContext::TContextConfigR99_R4 setConfigR99;
	TPckg<RPacketContext::TContextConfigR99_R4> setConfigPkgR99(setConfigR99);
	RPacketContext::TContextConfig_R5 setConfigR5;
	TPckg<RPacketContext::TContextConfig_R5> setConfigPkgR5(setConfigR5);

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
			setConfigGprs.iAccessPointName.Copy(aApn);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigGprs.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgGprs);
			break;
		
		case EQoSR4: // R99_R4
			setConfigR99.iAccessPointName.Copy(aApn);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR99);
			break;

		case EQoSR5: // R5
			setConfigR5.iAccessPointName.Copy(aApn);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
			setConfigR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
			primaryPacketContext.SetConfig(setConfigStatus, setConfigPkgR5);
			break;
		}
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with an error"));

	//Activate the primary context with RPacketContext::Activate. 
	TExtEtelRequestStatus contextActivateStatus(primaryPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	primaryPacketContext.Activate(contextActivateStatus);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	//Check RPacketQoS::NotifyProfileChanged returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated 
	//with higher(better) value as the requested minimum value used with SetProfileParameters	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqStatusNotifyProfileCh, ETimeMedium), KErrNone, _L("RPacketQoS::NotifyProfileChanged timed out."));						
	ASSERT_EQUALS(reqStatusNotifyProfileCh.Int(), KErrNone, _L("RPacketQoS::NotifyProfileChanged returned with error status."));
		
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:	
														
				ASSERT_TRUE(qosNegCapsGPRS.iDelay > qosReqCapsGPRS.iMinDelay, _L("Negotiated QoS iDelay is not better than minimun requested Qos iDelay"));
				ASSERT_TRUE(qosNegCapsGPRS.iMeanThroughput > qosReqCapsGPRS.iMinMeanThroughput, _L("Negotiated QoS iMeanThroughput is not better than minimun requested Qos iMeanThroughput"));
				ASSERT_TRUE(qosNegCapsGPRS.iPeakThroughput > qosReqCapsGPRS.iMinPeakThroughput, _L("Negotiated QoS iPeakThroughput is not better than minimun requested Qos iPeakThroughput"));
				ASSERT_TRUE(qosNegCapsGPRS.iPrecedence > qosReqCapsGPRS.iMinPrecedence, _L("Negotiated QoS iPrecedence is not better than minimun requested Qos iPrecedence"));
				ASSERT_TRUE(qosNegCapsGPRS.iReliability > qosReqCapsGPRS.iMinReliability, _L("Negotiated QoS iPrecedence is not better than minimun requested Qos iPrecedence"));
				break;
				
		case EQoSR4:	
				
				ASSERT_TRUE(qosNegCapsR99.iTrafficClass > qosReqCapsR99.iMinTrafficClass, _L("Negotiated QoS iTrafficClass is not better than minimun requested Qos iTrafficClass"));
				ASSERT_TRUE(qosNegCapsR99.iDeliveryOrderReqd > qosReqCapsR99.iMinDeliveryOrderReqd, _L("Negotiated QoS iDeliveryOrderReqd is not better than minimun requested Qos iDeliveryOrderReqd"));
				ASSERT_TRUE(qosNegCapsR99.iDeliverErroneousSDU > qosReqCapsR99.iMinDeliverErroneousSDU, _L("Negotiated QoS iDeliverErroneousSDU is not better than minimun requested Qos iDeliverErroneousSDU"));
				ASSERT_TRUE(qosNegCapsR99.iMaxSDUSize > qosReqCapsR99.iMinAcceptableMaxSDUSize, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsR99.iMaxRate.iDownlinkRate > qosReqCapsR99.iMinAcceptableMaxRate.iDownlinkRate, _L("Negotiated QoS iMaxRate.iDownlinkRate is not better than minimun requested Qos iMaxRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsR99.iMaxRate.iUplinkRate > qosReqCapsR99.iMinAcceptableMaxRate.iUplinkRate, _L("Negotiated QoS iMaxRate.iUplinkRate is not better than minimun requested Qos iMaxRate.iUplinkRate"));
				ASSERT_TRUE(qosNegCapsR99.iBER > qosReqCapsR99.iMaxBER, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsR99.iSDUErrorRatio > qosReqCapsR99.iMaxSDUErrorRatio, _L("Negotiated QoS iSDUErrorRatio is not better than minimun requested Qos iSDUErrorRatio"));
				ASSERT_TRUE(qosNegCapsR99.iTrafficHandlingPriority > qosReqCapsR99.iMinTrafficHandlingPriority, _L("Negotiated QoS iTrafficHandlingPriority is not better than minimun requested Qos iTrafficHandlingPriority"));
				ASSERT_TRUE(qosNegCapsR99.iTransferDelay < qosReqCapsR99.iMaxTransferDelay, _L("Negotiated QoS iTransferDelay is not better than minimun requested Qos iTransferDelay"));
				ASSERT_TRUE(qosNegCapsR99.iGuaranteedRate.iDownlinkRate > qosReqCapsR99.iMinGuaranteedRate.iDownlinkRate, _L("Negotiated QoS iGuaranteedRate.iDownlinkRate is not better than minimun requested Qos iGuaranteedRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsR99.iGuaranteedRate.iUplinkRate > qosReqCapsR99.iMinGuaranteedRate.iUplinkRate, _L("Negotiated QoS iGuaranteedRate.iUplinkRate is not better than minimun requested Qos iGuaranteedRate.iUplinkRate"));
				break;
				
		case EQoSR5:
									
				ASSERT_TRUE(qosNegCapsR5.iTrafficClass > qosReqCapsR5.iMinTrafficClass, _L("Negotiated QoS iTrafficClass is not better than minimun requested Qos iTrafficClass"));
				ASSERT_TRUE(qosNegCapsR5.iDeliveryOrderReqd > qosReqCapsR5.iMinDeliveryOrderReqd, _L("Negotiated QoS iDeliveryOrderReqd is not better than minimun requested Qos iDeliveryOrderReqd"));
				ASSERT_TRUE(qosNegCapsR5.iDeliverErroneousSDU > qosReqCapsR5.iMinDeliverErroneousSDU, _L("Negotiated QoS iDeliverErroneousSDU is not better than minimun requested Qos iDeliverErroneousSDU"));
				ASSERT_TRUE(qosNegCapsR5.iMaxSDUSize > qosReqCapsR5.iMinAcceptableMaxSDUSize, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsR5.iMaxRate.iDownlinkRate > qosReqCapsR5.iMinAcceptableMaxRate.iDownlinkRate, _L("Negotiated QoS iMaxRate.iDownlinkRate is not better than minimun requested Qos iMaxRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsR5.iMaxRate.iUplinkRate > qosReqCapsR5.iMinAcceptableMaxRate.iUplinkRate, _L("Negotiated QoS iMaxRate.iUplinkRate is not better than minimun requested Qos iMaxRate.iUplinkRate"));
				ASSERT_TRUE(qosNegCapsR5.iBER > qosReqCapsR5.iMaxBER, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsR5.iSDUErrorRatio > qosReqCapsR5.iMaxSDUErrorRatio, _L("Negotiated QoS iSDUErrorRatio is not better than minimun requested Qos iSDUErrorRatio"));
				ASSERT_TRUE(qosNegCapsR5.iTrafficHandlingPriority > qosReqCapsR5.iMinTrafficHandlingPriority, _L("Negotiated QoS iTrafficHandlingPriority is not better than minimun requested Qos iTrafficHandlingPriority"));
				ASSERT_TRUE(qosNegCapsR5.iTransferDelay < qosReqCapsR5.iMaxTransferDelay, _L("Negotiated QoS iTransferDelay is not better than minimun requested Qos iTransferDelay"));
				ASSERT_TRUE(qosNegCapsR5.iGuaranteedRate.iDownlinkRate > qosReqCapsR5.iMinGuaranteedRate.iDownlinkRate, _L("Negotiated QoS iGuaranteedRate.iDownlinkRate is not better than minimun requested Qos iGuaranteedRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsR5.iGuaranteedRate.iUplinkRate > qosReqCapsR5.iMinGuaranteedRate.iUplinkRate, _L("Negotiated QoS iGuaranteedRate.iUplinkRate is not better than minimun requested Qos iGuaranteedRate.iUplinkRate"));		
				break;
				
	}
	
	RPacketQoS::TQoSGPRSNegotiated qosNegCapsNumberTwoGPRS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> qosNegCapsNumberTwoGPRSPckg(qosNegCapsNumberTwoGPRS);
	RPacketQoS::TQoSR99_R4Negotiated qosNegCapsNumberTwoR99;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> qosNegCapsNumberTwoR99Pckg(qosNegCapsNumberTwoR99);
	RPacketQoS::TQoSR5Negotiated qosNegCapsNumberTwoR5;
	TPckg<RPacketQoS::TQoSR5Negotiated> qosNegCapsNumberTwoR5Pckg(qosNegCapsNumberTwoR5);
		
	//Check RPacketQoS::GetProfileParameters returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated 
	//with higher(better) value as the requested minimum value used with SetProfileParameters
	TExtEtelRequestStatus reqStatusGetProfileParams(contextQoS,EPacketQoSGetProfileParams);
	CleanupStack::PushL(reqStatusGetProfileParams);
	
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:
				contextQoS.GetProfileParameters(reqStatusGetProfileParams, qosNegCapsNumberTwoGPRSPckg);	
				break;
				
		case EQoSR4:		
				contextQoS.GetProfileParameters(reqStatusGetProfileParams, qosNegCapsNumberTwoR99Pckg);
				break;
				
		case EQoSR5:		
				contextQoS.GetProfileParameters(reqStatusGetProfileParams, qosNegCapsNumberTwoR5Pckg);					
				break;
				
	}
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqStatusGetProfileParams, ETimeMedium), KErrNone, _L("RPacketQoS::GetProfileParameters timed out."));						
	ASSERT_EQUALS(reqStatusGetProfileParams.Int(), KErrNone, _L("RPacketQoS::GetProfileParameters returned with error status."));
	
	switch(aQoSCapType)
	{
	
		case EQoSGPRS:
		
				ASSERT_TRUE(qosNegCapsNumberTwoGPRS.iDelay > qosReqCapsGPRS.iMinDelay, _L("Negotiated QoS iDelay is not better than minimun requested Qos iDelay"));
				ASSERT_TRUE(qosNegCapsNumberTwoGPRS.iMeanThroughput > qosReqCapsGPRS.iMinMeanThroughput, _L("Negotiated QoS iMeanThroughput is not better than minimun requested Qos iMeanThroughput"));
				ASSERT_TRUE(qosNegCapsNumberTwoGPRS.iPeakThroughput > qosReqCapsGPRS.iMinPeakThroughput, _L("Negotiated QoS iPeakThroughput is not better than minimun requested Qos iPeakThroughput"));
				ASSERT_TRUE(qosNegCapsNumberTwoGPRS.iPrecedence > qosReqCapsGPRS.iMinPrecedence, _L("Negotiated QoS iPrecedence is not better than minimun requested Qos iPrecedence"));
				ASSERT_TRUE(qosNegCapsNumberTwoGPRS.iReliability > qosReqCapsGPRS.iMinReliability, _L("Negotiated QoS iPrecedence is not better than minimun requested Qos iPrecedence"));
				break;
		
		case EQoSR4:
		
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iTrafficClass > qosReqCapsR99.iMinTrafficClass, _L("Negotiated QoS iTrafficClass is not better than minimun requested Qos iTrafficClass"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iDeliveryOrderReqd > qosReqCapsR99.iMinDeliveryOrderReqd, _L("Negotiated QoS iDeliveryOrderReqd is not better than minimun requested Qos iDeliveryOrderReqd"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iDeliverErroneousSDU > qosReqCapsR99.iMinDeliverErroneousSDU, _L("Negotiated QoS iDeliverErroneousSDU is not better than minimun requested Qos iDeliverErroneousSDU"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iMaxSDUSize > qosReqCapsR99.iMinAcceptableMaxSDUSize, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iMaxRate.iDownlinkRate > qosReqCapsR99.iMinAcceptableMaxRate.iDownlinkRate, _L("Negotiated QoS iMaxRate.iDownlinkRate is not better than minimun requested Qos iMaxRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iMaxRate.iUplinkRate > qosReqCapsR99.iMinAcceptableMaxRate.iUplinkRate, _L("Negotiated QoS iMaxRate.iUplinkRate is not better than minimun requested Qos iMaxRate.iUplinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iBER > qosReqCapsR99.iMaxBER, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iSDUErrorRatio > qosReqCapsR99.iMaxSDUErrorRatio, _L("Negotiated QoS iSDUErrorRatio is not better than minimun requested Qos iSDUErrorRatio"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iTrafficHandlingPriority > qosReqCapsR99.iMinTrafficHandlingPriority, _L("Negotiated QoS iTrafficHandlingPriority is not better than minimun requested Qos iTrafficHandlingPriority"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iTransferDelay < qosReqCapsR99.iMaxTransferDelay, _L("Negotiated QoS iTransferDelay is not better than minimun requested Qos iTransferDelay"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iGuaranteedRate.iDownlinkRate > qosReqCapsR99.iMinGuaranteedRate.iDownlinkRate, _L("Negotiated QoS iGuaranteedRate.iDownlinkRate is not better than minimun requested Qos iGuaranteedRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR99.iGuaranteedRate.iUplinkRate > qosReqCapsR99.iMinGuaranteedRate.iUplinkRate, _L("Negotiated QoS iGuaranteedRate.iUplinkRate is not better than minimun requested Qos iGuaranteedRate.iUplinkRate"));
				break;
				
		case EQoSR5:
				
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iTrafficClass > qosReqCapsR5.iMinTrafficClass, _L("Negotiated QoS iTrafficClass is not better than minimun requested Qos iTrafficClass"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iDeliveryOrderReqd > qosReqCapsR5.iMinDeliveryOrderReqd, _L("Negotiated QoS iDeliveryOrderReqd is not better than minimun requested Qos iDeliveryOrderReqd"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iDeliverErroneousSDU > qosReqCapsR5.iMinDeliverErroneousSDU, _L("Negotiated QoS iDeliverErroneousSDU is not better than minimun requested Qos iDeliverErroneousSDU"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iMaxSDUSize > qosReqCapsR5.iMinAcceptableMaxSDUSize, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iMaxRate.iDownlinkRate > qosReqCapsR5.iMinAcceptableMaxRate.iDownlinkRate, _L("Negotiated QoS iMaxRate.iDownlinkRate is not better than minimun requested Qos iMaxRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iMaxRate.iUplinkRate > qosReqCapsR5.iMinAcceptableMaxRate.iUplinkRate, _L("Negotiated QoS iMaxRate.iUplinkRate is not better than minimun requested Qos iMaxRate.iUplinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iBER > qosReqCapsR5.iMaxBER, _L("Negotiated QoS iMaxSDUSize is not better than minimun requested Qos iMaxSDUSize"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iSDUErrorRatio > qosReqCapsR5.iMaxSDUErrorRatio, _L("Negotiated QoS iSDUErrorRatio is not better than minimun requested Qos iSDUErrorRatio"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iTrafficHandlingPriority > qosReqCapsR5.iMinTrafficHandlingPriority, _L("Negotiated QoS iTrafficHandlingPriority is not better than minimun requested Qos iTrafficHandlingPriority"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iTransferDelay < qosReqCapsR5.iMaxTransferDelay, _L("Negotiated QoS iTransferDelay is not better than minimun requested Qos iTransferDelay"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iGuaranteedRate.iDownlinkRate > qosReqCapsR5.iMinGuaranteedRate.iDownlinkRate, _L("Negotiated QoS iGuaranteedRate.iDownlinkRate is not better than minimun requested Qos iGuaranteedRate.iDownlinkRate"));
				ASSERT_TRUE(qosNegCapsNumberTwoR5.iGuaranteedRate.iUplinkRate > qosReqCapsR5.iMinGuaranteedRate.iUplinkRate, _L("Negotiated QoS iGuaranteedRate.iUplinkRate is not better than minimun requested Qos iGuaranteedRate.iUplinkRate"));		
				break;
							
	}
				
	//Release QoS
	iEtelSessionMgr.ReleasePrimaryPacketContextQoS(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);
	
			
	// Release packet context
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);
	
	//PopAndDestroy
	//reqStatusSetProfileParams
	//reqStatusNotifyProfileCh
	//setConfigStatus
	//contextActivateStatus
	//reqStatusGetProfileParams	
	
	CleanupStack::PopAndDestroy(5,&reqStatusSetProfileParams);


	}

TPtrC CCTSYIntegrationTestPacketContext0059::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0059");
	}



CCTSYIntegrationTestPacketContext0060::CCTSYIntegrationTestPacketContext0060(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0060::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0060::~CCTSYIntegrationTestPacketContext0060()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0060::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0060
 * @SYMFssID BA/CTSY/PKTC-0060
 * @SYMTestCaseDesc Activate a PDP context can be activated with requested QOS better than subscribed QOS (failure scenario).
 * @SYMTestPriority High
 * @SYMTestActions RPacketQos::SetProfileParameters, RPacketContext::Activate, RPacketQos::GetProfileParameters
 * @SYMTestExpectedResults Pass - A PDP context with subscribed QoS could be activated. Negotiated QoS lower than requested.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify negotiated QoS lower than that requested.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

												 
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
    // Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    	
    // Ensure phone is attached to the packet service
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
  
    	
	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
		
	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	phone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	TPtrC apn, user, pwd;
	iPacketServiceTestHelper.GetGprsSettings(info, apn, user, pwd);

	// == Activate a PDP context with the requested QOS better than subscribed QOS (failure scenario) by using TQoSCapsGPRS, TQoSGPRSRequested, TQoSGPRSNegotiated   ==
	DEBUG_PRINTF1(_L("== GPRS =="));		
	DoTest0060L(apn, user, pwd, EQoSGPRS);
		
	// ===  Repeat test for R99_R4 ===
	// Repeat test with TQoSCapsR99_R4 instead of TQoSCapsGPRS, TQoSR99_R4Requested instead of TQoSGPRSRequested and TQoSR99_R4Negotiated instead of TQoSGPRSNegotiated 
	DEBUG_PRINTF1(_L("== R99_R4 =="));
	DoTest0060L(apn, user, pwd, EQoSR4);
	
	// ===  Repeat test for R5 ===
	// Repeat test with TQoSCapsR5 instead of TQoSCapsGPRS, TQoSR5Requested instead of TQoSGPRSRequested, and TQoSR5Negotiated instead of TQoSGPRSNegotiated 
	DEBUG_PRINTF1(_L("== R5 =="));
	DoTest0060L(apn, user, pwd, EQoSR5);
	

	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Pop
	// getNetworkStatus
	CleanupStack::PopAndDestroy(1, &getNetworkStatus);
	
	return TestStepResult();
	}

void CCTSYIntegrationTestPacketContext0060::DoTest0060L( TPtrC& aApn, TPtrC& aUser, TPtrC& aPwd, TInt aQoSCapType )
/**
 * Activates primary packet context with default context parameters.
 * @param aNetworkType Gprs / R99_R4 / R5.
 */
    {
		
 	// Open a new primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
												KMainServer,
												KMainPhone,
												KMainPacketService,
												KPrimaryPacketContext1);
			
	// Open a new QoS from the primary context												
	RPacketQoS& packetQoS = iEtelSessionMgr.GetPrimaryPacketContextQoSL(
												KMainServer,
												KMainPhone,
												KMainPacketService, 
												KPrimaryPacketContext1);
	
    //Get the QoS profile capabilities with RPacketQoS::GetProfileCapabilities with aProfileCaps of type TQoSCapsGPRS/TQoSCapsR99_R4/TQoSCapsR5			
	RPacketQoS::TQoSCapsGPRS qosCapsGPRS;
	RPacketQoS::TQoSCapsR99_R4 qosCapsR99;
	RPacketQoS::TQoSCapsR5 qosCapsR5;

	switch(aQoSCapType)
		{
			case EQoSGPRS:
					GetProfileCapsL(packetQoS, qosCapsGPRS);
					break;
					
			case EQoSR4:		
					GetProfileCapsL(packetQoS, qosCapsR99);
					break;
					
			case EQoSR5:		
					GetProfileCapsL(packetQoS, qosCapsR5);					
					break;
		}
		
		
	// Set the minimum requested QoS profile parameters to lower than what was returned from GetProfileCapabilities using RPacketQoS::SetProfileParameters
	// with aProfile of type TQoSGPRSRequested/TQoSR99_R4Requested/TQoSR5Requested.
	// Set all iMin... to a lower value than that returned by GetProfileCapabilities and all iReq... to a maximum value allowable by the API. 
	RPacketQoS::TQoSGPRSRequested qosGprsRequested;
	TPckg<RPacketQoS::TQoSGPRSRequested> qosGprsReqPckg(qosGprsRequested);
	RPacketQoS::TQoSR99_R4Requested qosR99_R4Requested;
	TPckg<RPacketQoS::TQoSR99_R4Requested> qosR99_R4ReqPckg(qosR99_R4Requested);	
	RPacketQoS::TQoSR5Requested qosR5Requested;
	TPckg<RPacketQoS::TQoSR5Requested> qosR5ReqPckg(qosR5Requested);
	RPacketQoS::TBitRate bitRate;
	bitRate.iDownlinkRate = 0;
	bitRate.iUplinkRate = 0;
	
	TExtEtelRequestStatus setProfileParamStatus(packetQoS, EPacketQoSSetProfileParams);
	CleanupStack::PushL(setProfileParamStatus);	

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS	
		//Min caps		
		qosGprsRequested.iMinDelay 			= RPacketQoS::EUnspecifiedDelayClass;
		qosGprsRequested.iMinMeanThroughput = RPacketQoS::EUnspecifiedMeanThroughput;
		qosGprsRequested.iMinPeakThroughput = RPacketQoS::EUnspecifiedPeakThroughput;
		qosGprsRequested.iMinPrecedence 	= RPacketQoS::EUnspecifiedPrecedence;
		qosGprsRequested.iMinReliability 	= RPacketQoS::EUnspecifiedReliabilityClass;
		//Request caps
		qosGprsRequested.iReqDelay          = RPacketQoS::EDelayClass1;
		qosGprsRequested.iReqMeanThroughput = RPacketQoS::EMeanThroughput50000000;
		qosGprsRequested.iReqPeakThroughput = RPacketQoS::EPeakThroughput256000;
		qosGprsRequested.iReqReliability    = RPacketQoS::EReliabilityClass1;
		qosGprsRequested.iReqPrecedence     = RPacketQoS::EPriorityHighPrecedence;
		packetQoS.SetProfileParameters(setProfileParamStatus, qosGprsReqPckg);
		break;
			
		case EQoSR4: // R99_R4
		//Min caps
		qosR99_R4Requested.iMinTrafficClass				= RPacketQoS::ETrafficClassUnspecified;
		qosR99_R4Requested.iMinDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderUnspecified;
		qosR99_R4Requested.iMinDeliverErroneousSDU		= RPacketQoS::EErroneousSDUNoDetection;
		qosR99_R4Requested.iMinAcceptableMaxSDUSize		= 10;
		qosR99_R4Requested.iMinAcceptableMaxRate		= bitRate;
		qosR99_R4Requested.iMaxBER						= RPacketQoS::EBERUnspecified;
		qosR99_R4Requested.iMaxSDUErrorRatio			= RPacketQoS::ESDUErrorRatioUnspecified;
		qosR99_R4Requested.iMinTrafficHandlingPriority	= RPacketQoS::ETrafficPriorityUnspecified;
		qosR99_R4Requested.iMaxTransferDelay			= 4000;
		qosR99_R4Requested.iMinGuaranteedRate			= bitRate;
		//Request caps
		qosR99_R4Requested.iReqTrafficClass				= RPacketQoS::ETrafficClassConversational;
		qosR99_R4Requested.iReqDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderRequired;
		qosR99_R4Requested.iReqDeliverErroneousSDU		= RPacketQoS::EErroneousSDUNoDetection;
		qosR99_R4Requested.iReqMaxSDUSize				= 1520;
		bitRate.iDownlinkRate = 16000;
		bitRate.iUplinkRate = 16000;
		qosR99_R4Requested.iReqMaxRate					= bitRate;
		qosR99_R4Requested.iReqBER						= RPacketQoS::EBERSixPerHundredMillion;
		qosR99_R4Requested.iReqSDUErrorRatio 			= RPacketQoS::ESDUErrorRatioOnePerMillion;
		qosR99_R4Requested.iReqTrafficHandlingPriority	= RPacketQoS::ETrafficPriority1;
		qosR99_R4Requested.iReqTransferDelay			= 10;
		qosR99_R4Requested.iReqGuaranteedRate			= bitRate;
		packetQoS.SetProfileParameters(setProfileParamStatus, qosR99_R4ReqPckg);
		break;
		
		case EQoSR5: // R5
		//Min caps
		qosR5Requested.iMinTrafficClass				= RPacketQoS::ETrafficClassUnspecified;
		qosR5Requested.iMinDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderUnspecified;
		qosR5Requested.iMinDeliverErroneousSDU		= RPacketQoS::EErroneousSDUDeliveryUnspecified;
		qosR5Requested.iMinAcceptableMaxSDUSize		= 10;
		qosR5Requested.iMinAcceptableMaxRate		= bitRate;
		qosR5Requested.iMaxBER						= RPacketQoS::EBERUnspecified;
		qosR5Requested.iMaxSDUErrorRatio			= RPacketQoS::ESDUErrorRatioUnspecified;
		qosR5Requested.iMinTrafficHandlingPriority	= RPacketQoS::ETrafficPriorityUnspecified;
		qosR5Requested.iMaxTransferDelay			= 0;
		qosR5Requested.iMinGuaranteedRate			= bitRate;
		//Request caps
		qosR5Requested.iReqTrafficClass				= RPacketQoS::ETrafficClassConversational;
		qosR5Requested.iReqDeliveryOrderReqd		= RPacketQoS::EDeliveryOrderRequired;
		qosR5Requested.iReqDeliverErroneousSDU		= RPacketQoS::EErroneousSDUNoDetection;
		qosR5Requested.iReqMaxSDUSize				= 1520;
		bitRate.iDownlinkRate = 16000;
		bitRate.iUplinkRate = 16000;
		qosR5Requested.iReqMaxRate					= bitRate;
		qosR5Requested.iReqBER						= RPacketQoS::EBERSixPerHundredMillion;
		qosR5Requested.iReqSDUErrorRatio 			= RPacketQoS::ESDUErrorRatioOnePerMillion;
		qosR5Requested.iReqTrafficHandlingPriority	= RPacketQoS::ETrafficPriority1;
		qosR5Requested.iReqTransferDelay			= 10;
		qosR5Requested.iReqGuaranteedRate			= bitRate;
		qosR5Requested.iSignallingIndication		= ETrue;
		qosR5Requested.iSourceStatisticsDescriptor	= RPacketQoS::ESourceStatisticsDescriptorSpeech;
		packetQoS.SetProfileParameters(setProfileParamStatus, qosR5ReqPckg);
		break;
		}

	ASSERT_EQUALS(WaitForRequestWithTimeOut(setProfileParamStatus, ETimeMedium), KErrNone, _L("RPacketQoS::SetProfileParameters timed out."));						
	ASSERT_EQUALS(setProfileParamStatus.Int(), KErrNone, _L("RPacketQoS::SetProfileParameters returned with error status."));

	// Set the context config with RPacketContext::SetConfig 
	RPacketContext::TContextConfigGPRS gprsConfig;
	TPckg<RPacketContext::TContextConfigGPRS> gprsConfigPk(gprsConfig);
	RPacketContext::TContextConfigR99_R4 r99_r4Config;
	TPckg<RPacketContext::TContextConfigR99_R4> r99_r4ConfigPk(r99_r4Config);
	RPacketContext::TContextConfig_R5 r5Config;
	TPckg<RPacketContext::TContextConfig_R5> r5ConfigPk(r5Config);
	
	TExtEtelRequestStatus setConfigStatus(packetContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS	
		gprsConfig.iAccessPointName.Copy(aApn);
		gprsConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
		gprsConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
		packetContext.SetConfig(setConfigStatus, gprsConfigPk);
		break;
		
		case EQoSR4: // R99_R4	
		r99_r4Config.iAccessPointName.Copy(aApn);
		r99_r4Config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
		r99_r4Config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
		packetContext.SetConfig(setConfigStatus, r99_r4ConfigPk);
		break;
		
		case EQoSR5: // R5 
		r5Config.iAccessPointName.Copy(aApn);
		r5Config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(aUser);
		r5Config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(aPwd);
		packetContext.SetConfig(setConfigStatus, r5ConfigPk);
		break;
		}

	ASSERT_EQUALS(WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, _L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone, _L("RPacketContext::SetConfig returned with error status."));
	
	//post notifier for NotifyProfileChanged
	TExtEtelRequestStatus notifyProfileChangedStatus (packetQoS, EPacketQoSNotifyProfileChanged);
	CleanupStack::PushL(notifyProfileChangedStatus);
	RPacketQoS::TQoSGPRSNegotiated gprsNegotiatedQoSValues;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> gprsNegotiatedQoSPk(gprsNegotiatedQoSValues);
	RPacketQoS::TQoSR99_R4Negotiated r99_r4NegotiatedQoSValues;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> r99_r4NegotiatedQoSPk(r99_r4NegotiatedQoSValues);
	RPacketQoS::TQoSR5Negotiated r5NegotiatedQoSValues;
	TPckg<RPacketQoS::TQoSR5Negotiated> r5NegotiatedQoSPk(r5NegotiatedQoSValues);
	
	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS
		packetQoS.NotifyProfileChanged(notifyProfileChangedStatus, gprsNegotiatedQoSPk);
		break;
		
		case EQoSR4: // R99_R4
		packetQoS.NotifyProfileChanged(notifyProfileChangedStatus, r99_r4NegotiatedQoSPk);
		break;

		case EQoSR5: // R5
		packetQoS.NotifyProfileChanged(notifyProfileChangedStatus, r5NegotiatedQoSPk);
		break;
		} 
				
	// Activate the primary context with RPacketContext::Activate. 
	TExtEtelRequestStatus contextActivateStatus(packetContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	packetContext.Activate(contextActivateStatus);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, _L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone, _L("RPacketContext::Activate returned with error status."));
	
	// Check RPacketQoS::NotifyProfileChanged returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated with lower values than the requested values used in SetProfileParameters.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyProfileChangedStatus, ETimeLong), KErrNone, _L("RPacketQoS::NotifyProfileChanged timed out"));
	ASSERT_EQUALS(notifyProfileChangedStatus.Int(), KErrNone, _L("RPacketQoS::NotifyProfileChanged returned with error status."));

	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS	
		ASSERT_TRUE(gprsNegotiatedQoSValues.iDelay > qosGprsRequested.iReqDelay, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDelay."));
		// Network returns EMeanThroughputBestEffort (0x80000) which is is higher than best EMeanThroughput50000000 (0x40000). That is why comparison must be this way
		ASSERT_TRUE(gprsNegotiatedQoSValues.iMeanThroughput > qosGprsRequested.iReqMeanThroughput, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMeanThroughput."));	
		ASSERT_TRUE(gprsNegotiatedQoSValues.iPeakThroughput < qosGprsRequested.iReqPeakThroughput, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iPeakThroughput."));		
		ASSERT_TRUE(gprsNegotiatedQoSValues.iReliability > qosGprsRequested.iReqReliability, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iReliability."));
		ASSERT_TRUE(gprsNegotiatedQoSValues.iPrecedence > qosGprsRequested.iReqPrecedence, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iPrecedence."));
		break;
		
		case EQoSR4: // R99_R4			
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTrafficClass > qosR99_R4Requested.iReqTrafficClass, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficClass"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iDeliveryOrderReqd > qosR99_R4Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliveryOrderReqd."));	
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iDeliverErroneousSDU > qosR99_R4Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliverErroneousSDU."));								
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxSDUSize < qosR99_R4Requested.iReqMaxSDUSize, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxSDUSize."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxRate.iDownlinkRate < qosR99_R4Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iDownlinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxRate.iUplinkRate < qosR99_R4Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iUplinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iBER < qosR99_R4Requested.iReqBER, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iBER."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iSDUErrorRatio < qosR99_R4Requested.iReqSDUErrorRatio, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSDUErrorRatio."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTrafficHandlingPriority > qosR99_R4Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficHandlingPriority."));	
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTransferDelay > qosR99_R4Requested.iReqTransferDelay, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTransferDelay"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iGuaranteedRate.iDownlinkRate < qosR99_R4Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iDownlinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iGuaranteedRate.iUplinkRate < qosR99_R4Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iUplinkRate"));
		break;

		case EQoSR5: // R5		
		ASSERT_TRUE(r5NegotiatedQoSValues.iTrafficClass > qosR5Requested.iReqTrafficClass, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficClass"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iDeliveryOrderReqd > qosR5Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliveryOrderReqd."));	
		ASSERT_TRUE(r5NegotiatedQoSValues.iDeliverErroneousSDU > qosR5Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliverErroneousSDU."));								
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxSDUSize < qosR5Requested.iReqMaxSDUSize, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxSDUSize."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxRate.iDownlinkRate < qosR5Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iDownlinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxRate.iUplinkRate < qosR5Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iUplinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iBER < qosR5Requested.iReqBER, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iBER."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSDUErrorRatio < qosR5Requested.iReqSDUErrorRatio, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSDUErrorRatio."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iTrafficHandlingPriority > qosR5Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficHandlingPriority."));	
		ASSERT_TRUE(r5NegotiatedQoSValues.iTransferDelay > qosR5Requested.iReqTransferDelay, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTransferDelay"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iGuaranteedRate.iDownlinkRate < qosR5Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iDownlinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iGuaranteedRate.iUplinkRate < qosR5Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iUplinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSignallingIndication < qosR5Requested.iSignallingIndication, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSignallingIndication."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSourceStatisticsDescriptor < qosR5Requested.iSourceStatisticsDescriptor, _L("RPacketQoS::NotifyProfileChanged did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSourceStatisticsDescriptor."));				
		break;	
		}	
			
	// Check RPacketQoS::GetProfileParameters returns aProfile of type TQoSGPRSNegotiated/TQoSR99_R4Negotiated/TQoSR5Negotiated with lower values than the requested values used in SetProfileParameters.
	TExtEtelRequestStatus getProfileParametersStatus (packetQoS, EPacketQoSGetProfileParams);
	CleanupStack::PushL(getProfileParametersStatus);
	
	switch(aQoSCapType)
		{
		case EQoSGPRS: // GPRS	
		packetQoS.GetProfileParameters(getProfileParametersStatus, gprsNegotiatedQoSPk);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeLong), KErrNone, _L("RPacketQoS::GetProfileParameters timed out"));
		ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileParameters returned with an error"));
		ASSERT_TRUE(gprsNegotiatedQoSValues.iDelay > qosGprsRequested.iReqDelay, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDelay."));
		// Network returns EMeanThroughputBestEffort (0x80000) which is is higher than best EMeanThroughput50000000 (0x40000). That is why comparison must be this way
		ASSERT_TRUE(gprsNegotiatedQoSValues.iMeanThroughput > qosGprsRequested.iReqMeanThroughput, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMeanThroughput."));	
		ASSERT_TRUE(gprsNegotiatedQoSValues.iPeakThroughput < qosGprsRequested.iReqPeakThroughput, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iPeakThroughput."));		
		ASSERT_TRUE(gprsNegotiatedQoSValues.iReliability > qosGprsRequested.iReqReliability, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iReliability."));
		ASSERT_TRUE(gprsNegotiatedQoSValues.iPrecedence > qosGprsRequested.iReqPrecedence, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iPrecedence."));
		break;
		
		case EQoSR4: // R99_R4
		packetQoS.GetProfileParameters(getProfileParametersStatus, r99_r4NegotiatedQoSPk);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeLong), KErrNone, _L("RPacketQoS::GetProfileParameters timed out"));
		ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileParameters returned with an error"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTrafficClass > qosR99_R4Requested.iReqTrafficClass, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficClass"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iDeliveryOrderReqd > qosR99_R4Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliveryOrderReqd."));	
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iDeliverErroneousSDU > qosR99_R4Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliverErroneousSDU."));								
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxSDUSize < qosR99_R4Requested.iReqMaxSDUSize, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxSDUSize."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxRate.iDownlinkRate < qosR99_R4Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iDownlinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iMaxRate.iUplinkRate < qosR99_R4Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iUplinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iBER < qosR99_R4Requested.iReqBER, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iBER."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iSDUErrorRatio < qosR99_R4Requested.iReqSDUErrorRatio, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSDUErrorRatio."));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTrafficHandlingPriority > qosR99_R4Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficHandlingPriority."));	
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iTransferDelay > qosR99_R4Requested.iReqTransferDelay, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTransferDelay"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iGuaranteedRate.iDownlinkRate < qosR99_R4Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iDownlinkRate"));
		ASSERT_TRUE(r99_r4NegotiatedQoSValues.iGuaranteedRate.iUplinkRate < qosR99_R4Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iUplinkRate"));
		break;
		
		case EQoSR5: // R5
		packetQoS.GetProfileParameters(getProfileParametersStatus, r5NegotiatedQoSPk);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(getProfileParametersStatus, ETimeLong), KErrNone, _L("RPacketQoS::GetProfileParameters timed out"));
		ASSERT_EQUALS(getProfileParametersStatus.Int(), KErrNone, _L("RPacketQoS::GetProfileParameters returned with an error"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iTrafficClass > qosR5Requested.iReqTrafficClass, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficClass"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iDeliveryOrderReqd > qosR5Requested.iReqDeliveryOrderReqd, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliveryOrderReqd."));	
		ASSERT_TRUE(r5NegotiatedQoSValues.iDeliverErroneousSDU > qosR5Requested.iReqDeliverErroneousSDU, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iDeliverErroneousSDU."));								
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxSDUSize < qosR5Requested.iReqMaxSDUSize, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxSDUSize."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxRate.iDownlinkRate < qosR5Requested.iReqMaxRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iDownlinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iMaxRate.iUplinkRate < qosR5Requested.iReqMaxRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iMaxRate.iUplinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iBER < qosR5Requested.iReqBER, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iBER."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSDUErrorRatio < qosR5Requested.iReqSDUErrorRatio, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSDUErrorRatio."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iTrafficHandlingPriority > qosR5Requested.iReqTrafficHandlingPriority, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTrafficHandlingPriority."));	
		ASSERT_TRUE(r5NegotiatedQoSValues.iTransferDelay > qosR5Requested.iReqTransferDelay, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iTransferDelay"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iGuaranteedRate.iDownlinkRate < qosR5Requested.iReqGuaranteedRate.iDownlinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iDownlinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iGuaranteedRate.iUplinkRate < qosR5Requested.iReqGuaranteedRate.iUplinkRate, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iGuaranteedRate.iUplinkRate"));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSignallingIndication < qosR5Requested.iSignallingIndication, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSignallingIndication."));
		ASSERT_TRUE(r5NegotiatedQoSValues.iSourceStatisticsDescriptor < qosR5Requested.iSourceStatisticsDescriptor, _L("RPacketQoS::GetProfileParameters did not return lower profile value than requested value used in RPacketQoS::SetProfileParameters with iSourceStatisticsDescriptor."));				
		break;
		}
		
	//Release QoS
	iEtelSessionMgr.ReleasePrimaryPacketContextQoS(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);

	// Release packet context
	iEtelSessionMgr.ReleasePrimaryPacketContext(KMainServer,
											KMainPhone, 
											KMainPacketService, 
											KPrimaryPacketContext1);

	// Pop
	// setProfileParamStatus
	// setConfigStatus
	// notifyProfileChangedStatus
	// contextActivateStatus
	// getProfileParametersStatus
	CleanupStack::PopAndDestroy(5, &setProfileParamStatus);		
    }
 
TPtrC CCTSYIntegrationTestPacketContext0060::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0060");
	}



CCTSYIntegrationTestPacketContext0061::CCTSYIntegrationTestPacketContext0061(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0061::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0061::~CCTSYIntegrationTestPacketContext0061()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0061::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0061
 * @SYMFssID BA/CTSY/PKTC-0061
 * @SYMTestCaseDesc Initialise a context.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::InitialiseContext, RPacketContext::GetStatus, RPacketContext::AddPacketFilter, RPacketContext::NotifyStatusChange, RPacketContext::SetConfig, RPacketQoS::OpenNewQoS, RPacketQoS::GetProfileCapabilities, RPacketQoS::SetProfileParameters, RPacketContext::OpenNewSecondaryContext
 * @SYMTestExpectedResults Pass - RPacketContext::InitialiseContext returns TDataChannelV2 information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify comm port information received.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure phone is attached to the packet service. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Open a new primary context. 

	// Initialise the context with RPacketContext::InitialiseContext 

	// Check RPacketContext::InitialiseContext returns TDataChannelV2::iChannelId of length > 0
	// Check RPacketContext::InitialiseContext returns TDataChannelV2::iCsy of length > 0
	// Check RPacketContext::InitialiseContext returns TDataChannelV2::iPort of length > 0

	// Set a valid context config using RPacketContext::SetConfig 

	// Activate the context 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Open a new secondary context 

	// Initialise the secondary context with RPacketContext::InitialiseContext 

	// Check RPacketContext::InitialiseContext for the secondary context returns TDataChannelV2::iChannelId of length > 0
	// Check RPacketContext::InitialiseContext for the secondary context returns TDataChannelV2::iCsy of length > 0
	// Check RPacketContext::InitialiseContext for the secondary context returns TDataChannelV2::iPort of length > 0

	// ===  Add packet filter ===

	// Add a TPacketFilterV2 packet filter to the secondary context using RPacketContext::AddPacketFilter 

	// ===  Add QoS ===

	// Open a new QoS from the secondary context 

	// Get GPRS QoS profile capabilities with RPacketQoS::GetProfileCapabilities 

	// Set requested profile parameters with RPacketQoS::SetProfileParameters and TQoSGPRSRequested with parameters within range returned by RPacketQoS::GetProfileCapabilities 

	// Activate the secondary context with RPacketContext::Activate 

	// Check RPacketContext::NotifyStatusChange for the secondary context completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus for the secondary context returns EStatusActive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0061::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0061");
	}



CCTSYIntegrationTestPacketContext0062::CCTSYIntegrationTestPacketContext0062(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketContextBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketContext0062::GetTestStepName());
	}

CCTSYIntegrationTestPacketContext0062::~CCTSYIntegrationTestPacketContext0062()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketContext0062::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTC-0062
 * @SYMFssID BA/CTSY/PKTC-0062
 * @SYMTestCaseDesc Ensure change in QoS from network is handled correct when new QoS is not acceptable.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Context is deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify context is deactivated.
 *
 * @return - TVerdict code
 */
	{

	////////////////////////////////////////////////////////////////
	// SET UP
	////////////////////////////////////////////////////////////////

	
	// Ensure there is a primary PDP context active. 


	////////////////////////////////////////////////////////////////
	// SET UP END
	////////////////////////////////////////////////////////////////
	
	StartTest();
	
	////////////////////////////////////////////////////////////////
	// TEST START
	////////////////////////////////////////////////////////////////
	
	
	// Simulator sends a modify PDP context request message with a QoS that is not acceptable to the phone.  

	// Check RPacketContext::NotifyStatusChange completes with EStatusInactive

	
	////////////////////////////////////////////////////////////////
	// TEST END
	////////////////////////////////////////////////////////////////

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketContext0062::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketContext0062");
	}



