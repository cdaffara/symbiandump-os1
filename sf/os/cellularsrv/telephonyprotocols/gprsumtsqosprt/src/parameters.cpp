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
//

#include "parameters.h"
#include "umts_qos_variables.h"
#include "guqos_log.h"
#include "guqos.h"


TQoSRequested::TQoSRequested() : iHeaderCompression(0)
	{
	Reset();
	}

void TQoSRequested::Reset()
	{
	iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	iReqMaxSDUSize = 0;   // subscribe value
	iMinAcceptableMaxSDUSize = 0;	// subscribe value
	iReqMaxRate.iUplinkRate = 0;
	iReqMaxRate.iDownlinkRate = 0;
	iMinAcceptableMaxRate.iUplinkRate = 0;
	iMinAcceptableMaxRate.iDownlinkRate = 0;
	iReqBER = RPacketQoS::EBERUnspecified;
	iMaxBER = RPacketQoS::EBERUnspecified;
	iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	iReqTransferDelay = 0;
	iMaxTransferDelay = 0;
	iReqGuaranteedRate.iUplinkRate = 0;
	iReqGuaranteedRate.iDownlinkRate = 0;
	iMinGuaranteedRate.iUplinkRate = 0;
	iMinGuaranteedRate.iDownlinkRate = 0;
	}

void TQoSRequested::ClearMinimumParameters()
	{
	iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
	iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	iMinAcceptableMaxSDUSize = 0;
	iMinAcceptableMaxRate.iUplinkRate = 0;
	iMinAcceptableMaxRate.iDownlinkRate = 0;
	iMaxBER = RPacketQoS::EBERUnspecified;
	iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
	iMaxTransferDelay = 0;
	iMinGuaranteedRate.iUplinkRate = 0;
	iMinGuaranteedRate.iDownlinkRate = 0;
	}

void TQoSRequested::ParsePolicyData(CExtensionPolicy* extensionPolicy)
	{
	LOG(Log::Printf(_L("<-------------------------------------------------")));
	LOG(Log::Printf(_L("TQoSRequested::ParsePolicyData - Start Parsing Extensions")));
	
	TExtensionQueueIter i(extensionPolicy->Extensions());
	CExtension* extension;

	while ((extension = i++) != NULL)
		{
		if( (extension->Type() != STATIC_CAST(TUint,KPfqosR5ExtensionUmts))
		&&  (extension->Type() != STATIC_CAST(TUint,KPfqosExtensionUmts))
		  )
			{
			LOG(Log::Printf(_L("\tKPfqosExtensionUmts - Extension NOT found")));
			continue;
			}
		
		LOG(Log::Printf(_L("\tKPfqosExtensionUmts - Extension found")));
		

		TVariableQueueIter iter(extension->Queue());
		TVariableBase *var;

		while ((var = iter++) != NULL)
			{
			switch (var->Type())
				{

			case KPfqosTypeInteger:
				{
				TIntVariable* tmp = (TIntVariable*) var;
				LOG(Log::Printf(_L("\tKPfqosExtensionUmts '%S': %d"), &tmp->Name(), tmp->Value()));
				
				//lint -e{961} would want terminating 'else' (but we don't)
				if (tmp->Name().Compare(KDescDeliveryOfErroneusSduRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EErroneousSDUDeliveryUnspecified:
						iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
						break;
					case RPacketQoS::EErroneousSDUDeliveryRequired:
						iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
						break;
					case RPacketQoS::EErroneousSDUDeliveryNotRequired:
						iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescDeliveryOrderRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EDeliveryOrderUnspecified:
						iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
						break;
					case RPacketQoS::EDeliveryOrderRequired:
						iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;
						break;
					case RPacketQoS::EDeliveryOrderNotRequired:
						iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescErrorRatioRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ESDUErrorRatioUnspecified:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerTen:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTen;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerHundred:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;
						break;
					case RPacketQoS::ESDUErrorRatioSevenPerThousand:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerThousand:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerTenThousand:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerHundredThousand:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerMillion:
						iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescGuaBitrateDownlinkRequested)==0)
					{
					iReqGuaranteedRate.iDownlinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescGuaBitrateUplinkRequested)==0)
					{
					iReqGuaranteedRate.iUplinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxBitrateDownlinkRequested)==0)
					{
					iReqMaxRate.iDownlinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxBitrateUplinkRequested)==0)
					{
					iReqMaxRate.iUplinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxSduSizeRequested)==0)
					{
					iReqMaxSDUSize = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescPriorityRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ETrafficPriorityUnspecified:
						iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
						break;
					case RPacketQoS::ETrafficPriority1:
						iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
						break;
					case RPacketQoS::ETrafficPriority2:
						iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
						break;
					case RPacketQoS::ETrafficPriority3:
						iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescResidualBerRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EBERUnspecified:
						iReqBER = RPacketQoS::EBERUnspecified;
						break;
					case RPacketQoS::EBERFivePerHundred:
						iReqBER = RPacketQoS::EBERFivePerHundred;
						break;
					case RPacketQoS::EBEROnePerHundred:
						iReqBER = RPacketQoS::EBEROnePerHundred;
						break;
					case RPacketQoS::EBERFivePerThousand:
						iReqBER = RPacketQoS::EBERFivePerThousand;
						break;
					case RPacketQoS::EBERFourPerThousand:
						iReqBER = RPacketQoS::EBERFourPerThousand;
						break;
					case RPacketQoS::EBEROnePerThousand:
						iReqBER = RPacketQoS::EBEROnePerThousand;
						break;
					case RPacketQoS::EBEROnePerTenThousand:
						iReqBER = RPacketQoS::EBEROnePerTenThousand;
						break;
					case RPacketQoS::EBEROnePerHundredThousand:
						iReqBER = RPacketQoS::EBEROnePerHundredThousand;
						break;
					case RPacketQoS::EBEROnePerMillion:
						iReqBER = RPacketQoS::EBEROnePerMillion;
						break;
					case RPacketQoS::EBERSixPerHundredMillion:
						iReqBER = RPacketQoS::EBERSixPerHundredMillion;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescTrafficClassRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ETrafficClassUnspecified:
						iReqTrafficClass = RPacketQoS::ETrafficClassUnspecified;
						break;
					case RPacketQoS::ETrafficClassConversational:
						iReqTrafficClass = RPacketQoS::ETrafficClassConversational;
						break;
					case RPacketQoS::ETrafficClassStreaming:
						iReqTrafficClass = RPacketQoS::ETrafficClassStreaming;
						break;
					case RPacketQoS::ETrafficClassInteractive:
						iReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
						break;
					case RPacketQoS::ETrafficClassBackground:
						iReqTrafficClass = RPacketQoS::ETrafficClassBackground;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescTransferDelayRequested)==0)
					{
					iReqTransferDelay = tmp->Value();
					}
				// Minimum
				else if (tmp->Name().Compare(KDescDeliveryOfErroneusSduMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EErroneousSDUDeliveryUnspecified:
						iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified;
						break;
					case RPacketQoS::EErroneousSDUDeliveryRequired:
						iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
						break;
					case RPacketQoS::EErroneousSDUDeliveryNotRequired:
						iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescDeliveryOrderMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EDeliveryOrderUnspecified:
						iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
						break;
					case RPacketQoS::EDeliveryOrderRequired:
						iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;
						break;
					case RPacketQoS::EDeliveryOrderNotRequired:
						iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescErrorRatioMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ESDUErrorRatioUnspecified:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerTen:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTen;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerHundred:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;
						break;
					case RPacketQoS::ESDUErrorRatioSevenPerThousand:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerThousand:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerTenThousand:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerHundredThousand:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundredThousand;
						break;
					case RPacketQoS::ESDUErrorRatioOnePerMillion:
						iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescGuaBitrateDownlinkMinimum)==0)
					{
					iMinGuaranteedRate.iDownlinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescGuaBitrateUplinkMinimum)==0)
					{
					iMinGuaranteedRate.iUplinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxBitrateDownlinkMinimum)==0)
					{
					iMinAcceptableMaxRate.iDownlinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxBitrateUplinkMinimum)==0)
					{
					iMinAcceptableMaxRate.iUplinkRate = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescMaxSduSizeMinimum)==0)
					{
					iMinAcceptableMaxSDUSize = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescPriorityMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ETrafficPriorityUnspecified:
						iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
						break;
					case RPacketQoS::ETrafficPriority1:
						iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
						break;
					case RPacketQoS::ETrafficPriority2:
						iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
						break;
					case RPacketQoS::ETrafficPriority3:
						iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescResidualBerMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::EBERUnspecified:
						iMaxBER = RPacketQoS::EBERUnspecified;
						break;
					case RPacketQoS::EBERFivePerHundred:
						iMaxBER = RPacketQoS::EBERFivePerHundred;
						break;
					case RPacketQoS::EBEROnePerHundred:
						iMaxBER = RPacketQoS::EBEROnePerHundred;
						break;
					case RPacketQoS::EBERFivePerThousand:
						iMaxBER = RPacketQoS::EBERFivePerThousand;
						break;
					case RPacketQoS::EBERFourPerThousand:
						iMaxBER = RPacketQoS::EBERFourPerThousand;
						break;
					case RPacketQoS::EBEROnePerThousand:
						iMaxBER = RPacketQoS::EBEROnePerThousand;
						break;
					case RPacketQoS::EBEROnePerTenThousand:
						iMaxBER = RPacketQoS::EBEROnePerTenThousand;
						break;
					case RPacketQoS::EBEROnePerHundredThousand:
						iMaxBER = RPacketQoS::EBEROnePerHundredThousand;
						break;
					case RPacketQoS::EBEROnePerMillion:
						iMaxBER = RPacketQoS::EBEROnePerMillion;
						break;
					case RPacketQoS::EBERSixPerHundredMillion:
						iMaxBER = RPacketQoS::EBERSixPerHundredMillion;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescTrafficClassMinimum)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{
					case RPacketQoS::ETrafficClassUnspecified:
						iMinTrafficClass = RPacketQoS::ETrafficClassUnspecified;
						break;
					case RPacketQoS::ETrafficClassConversational:
						iMinTrafficClass = RPacketQoS::ETrafficClassConversational;
						break;
					case RPacketQoS::ETrafficClassStreaming:
						iMinTrafficClass = RPacketQoS::ETrafficClassStreaming;
						break;
					case RPacketQoS::ETrafficClassInteractive:
						iMinTrafficClass = RPacketQoS::ETrafficClassInteractive;
						break;
					case RPacketQoS::ETrafficClassBackground:
						iMinTrafficClass = RPacketQoS::ETrafficClassBackground;
						break;
					default:
						break;
						}
					}
				else if (tmp->Name().Compare(KDescTransferDelayMinimum)==0)
					{
					iMaxTransferDelay = tmp->Value();
					}
#ifdef SYMBIAN_NETWORKING_UMTSR5
				else if (tmp->Name().Compare(KDescSignallingIndicatorRequested)==0)
					{
					iSignallingIndication = tmp->Value();
					}
				else if (tmp->Name().Compare(KDescSrcStatDescRequested)==0)
					{
					TInt value = tmp->Value();
					switch(value)
						{  
					case RPacketQoS::ESourceStatisticsDescriptorUnknown:
						iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
						break;
					case RPacketQoS::ESourceStatisticsDescriptorSpeech:
						iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorSpeech;
						break;
					default:
						break;
						}
					}
#endif // SYMBIAN_NETWORKING_UMTSR5
				else if (tmp->Name().Compare(KDescHeaderCompression)==0)
					{
					iHeaderCompression = tmp->Value();
					}
				}
				break;

			default:
				LOG(Log::Printf(_L("\tKPfqosExtensionUmts type=%d '%S': ignored"), (TInt)var->Type(), &var->Name()));
				break;
				}
			}
		}
	
	LOG(Log::Printf(_L("TQoSRequested::ParsePolicyData - End Parsing Extensions")));
	LOG(Log::Printf(_L("PARSED VALUES:")));
	LOG(Log::Printf(_L("\tiReqTrafficClass = %d"),			iReqTrafficClass));
	LOG(Log::Printf(_L("\tiReqDeliveryOrderReqd = %d"),		iReqDeliveryOrderReqd));
	LOG(Log::Printf(_L("\tiReqDeliverErroneousSDU = %d"),	iReqDeliverErroneousSDU));
	LOG(Log::Printf(_L("\tiReqMaxSDUSize = %d"),			iReqMaxSDUSize));
	LOG(Log::Printf(_L("\tiReqMaxRate.iUplinkRate = %d"),	iReqMaxRate.iUplinkRate));
	LOG(Log::Printf(_L("\tiReqMaxRate.iDownlinkRate = %d"), iReqMaxRate.iDownlinkRate));
	LOG(Log::Printf(_L("\tiReqBER = %d"),					iReqBER));
	LOG(Log::Printf(_L("\tiReqSDUErrorRatio = %d"),			iReqSDUErrorRatio));
	LOG(Log::Printf(_L("\tiReqTrafficHandlingPriority = %d"),iReqTrafficHandlingPriority));
	LOG(Log::Printf(_L("\tiReqTransferDelay = %d"),			iReqTransferDelay));
	LOG(Log::Printf(_L("\tiReqGuaranteedRate.iUplinkRate = %d"),iReqGuaranteedRate.iUplinkRate));
	LOG(Log::Printf(_L("\tiReqGuaranteedRate.iDownlinkRate = %d"),iReqGuaranteedRate.iDownlinkRate));
	LOG(Log::Printf(_L("")));
	LOG(Log::Printf(_L("\tiMinTrafficClass = %d"),			iMinTrafficClass));
	LOG(Log::Printf(_L("\tiMinDeliveryOrderReqd = %d"),		iMinDeliveryOrderReqd));
	LOG(Log::Printf(_L("\tiMinDeliverErroneousSDU = %d")	,iMinDeliverErroneousSDU));
	LOG(Log::Printf(_L("\tiMinAcceptableMaxSDUSize = %d")	,iMinAcceptableMaxSDUSize));
	LOG(Log::Printf(_L("\tiMinAcceptableMaxRate.iUplinkRate = %d"),iMinAcceptableMaxRate.iUplinkRate));
	LOG(Log::Printf(_L("\tiMinAcceptableMaxRate.iDownlinkRate= %d"),iMinAcceptableMaxRate.iDownlinkRate));
	LOG(Log::Printf(_L("\tiMaxBER = %d"),					iMaxBER));
	LOG(Log::Printf(_L("\tiMaxSDUErrorRatio = %d"),			iMaxSDUErrorRatio));
	LOG(Log::Printf(_L("\tiMinTrafficHandlingPriority = %d"),iMinTrafficHandlingPriority));
	LOG(Log::Printf(_L("\tiMaxTransferDelay = %d"),			iMaxTransferDelay));
	LOG(Log::Printf(_L("\tiMinGuaranteedRate.iUplinkRate = %d"),iMinGuaranteedRate.iUplinkRate));
	LOG(Log::Printf(_L("\tiMinGuaranteedRate.iDownlinkRate = %d"),iMinGuaranteedRate.iDownlinkRate));
#ifdef SYMBIAN_NETWORKING_UMTSR5
	LOG(Log::Printf(_L("\tiSignallingIndication = %d"),iSignallingIndication));
	LOG(Log::Printf(_L("\tiSourceStatisticsDescriptor = %d"),iSourceStatisticsDescriptor));
#endif // SYMBIAN_NETWORKING_UMTSR5

	}

void ParameterMapper::MapGenericToRel99(const TQoSParameters& aSpec, RPacketQoS::TQoSR99_R4Requested& aGprs)
	{
	//
	// Traffic class
	//
	if ((aSpec.GetUpLinkDelay() < 250 && aSpec.GetUpLinkDelay() > 0) || 
		(aSpec.GetDownLinkDelay() < 250 && aSpec.GetDownLinkDelay() > 0))
		aGprs.iReqTrafficClass = RPacketQoS::ETrafficClassConversational;
	else if (aSpec.GetUpLinkDelay() >= 250 || aSpec.GetDownLinkDelay() >= 250)
		aGprs.iReqTrafficClass = RPacketQoS::ETrafficClassStreaming;
	else if ((aSpec.GetUpLinkPriority() >= 0 && aSpec.GetUpLinkPriority() <= 2) || 
		(aSpec.GetDownLinkPriority() >= 0 && aSpec.GetDownLinkPriority() <= 2))
		aGprs.iReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
	else
		aGprs.iReqTrafficClass = RPacketQoS::ETrafficClassBackground;

	aGprs.iMinTrafficClass = aGprs.iReqTrafficClass;

	//
	// Delivery order
	//
	aGprs.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	aGprs.iMinDeliveryOrderReqd = aGprs.iReqDeliveryOrderReqd;

	//
	// Delivery of erroneous SDUs
	//
	aGprs.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
	aGprs.iMinDeliverErroneousSDU = aGprs.iReqDeliverErroneousSDU;

	//
	// Residual BER
	//
	aGprs.iReqBER = RPacketQoS::EBERUnspecified;
	aGprs.iMaxBER = aGprs.iReqBER;

	//
	// SDU error ratio
	//
	aGprs.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
	aGprs.iMaxSDUErrorRatio = aGprs.iReqSDUErrorRatio;

	//
	// Traffic handling priority
	//
	if (aGprs.iReqTrafficClass == RPacketQoS::ETrafficClassInteractive)
		{   
		if (aSpec.GetUpLinkPriority() == 0 || aSpec.GetDownLinkPriority() == 0)
			aGprs.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
		else if (aSpec.GetUpLinkPriority() == 1 || aSpec.GetDownLinkPriority() == 1)
			aGprs.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority2;
		else
			aGprs.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority3;
		}
	else
		{
		aGprs.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriorityUnspecified;
		}
	
	aGprs.iMinTrafficHandlingPriority = aGprs.iReqTrafficHandlingPriority;

	//
	// Transfer delay
	//
	aGprs.iReqTransferDelay = ParameterMapper::Min(aSpec.GetUpLinkDelay(), aSpec.GetDownLinkDelay());
	aGprs.iMaxTransferDelay = aGprs.iReqTransferDelay;

	//
	// Maximum SDU size
	//
	aGprs.iReqMaxSDUSize = aSpec.GetUpLinkMaximumPacketSize();
	aGprs.iMinAcceptableMaxSDUSize = aGprs.iReqMaxSDUSize;

	//
	// Maximum bitrate
	//
	aGprs.iReqMaxRate.iUplinkRate = aSpec.GetUplinkBandwidth();
	aGprs.iReqMaxRate.iDownlinkRate = aSpec.GetDownlinkBandwidth();

	//
	// Guaranteed bitrate
	//
	aGprs.iReqGuaranteedRate.iUplinkRate = aSpec.GetUplinkBandwidth();
	aGprs.iReqGuaranteedRate.iDownlinkRate = aSpec.GetDownlinkBandwidth();
	}


TInt ParameterMapper::CombineParameters(const RPacketQoS::TQoSR99_R4Requested& aGprs, TQoSParameters& aFlowSpec)
	{
	if (aGprs.iReqGuaranteedRate.iUplinkRate > 0)
		aFlowSpec.SetUplinkBandwidth(aGprs.iReqGuaranteedRate.iUplinkRate);
	if (aGprs.iReqGuaranteedRate.iDownlinkRate > 0)
		aFlowSpec.SetDownlinkBandwidth(aGprs.iReqGuaranteedRate.iDownlinkRate);
	
	return KErrNone;
	}


void ParameterMapper::CombineDefault(const TQoSRequested& aDefault, TQoSRequested& aPolicy)
	{
	if (aPolicy.iReqTrafficClass == RPacketQoS::ETrafficClassUnspecified)
		aPolicy.iReqTrafficClass = aDefault.iReqTrafficClass;
	if (aPolicy.iMinTrafficClass == RPacketQoS::ETrafficClassUnspecified)
		aPolicy.iMinTrafficClass = aDefault.iMinTrafficClass;
	if (aPolicy.iReqDeliveryOrderReqd == RPacketQoS::EDeliveryOrderUnspecified)
		aPolicy.iReqDeliveryOrderReqd = aDefault.iReqDeliveryOrderReqd;
	if (aPolicy.iMinDeliveryOrderReqd == RPacketQoS::EDeliveryOrderUnspecified)
		aPolicy.iMinDeliveryOrderReqd = aDefault.iMinDeliveryOrderReqd;
	if (aPolicy.iReqDeliverErroneousSDU == RPacketQoS::EErroneousSDUDeliveryUnspecified)
		aPolicy.iReqDeliverErroneousSDU = aDefault.iReqDeliverErroneousSDU;
	if (aPolicy.iMinDeliverErroneousSDU == RPacketQoS::EErroneousSDUDeliveryUnspecified)
		aPolicy.iMinDeliverErroneousSDU = aDefault.iMinDeliverErroneousSDU;
	if (aPolicy.iReqMaxSDUSize == 0)
		aPolicy.iReqMaxSDUSize = aDefault.iReqMaxSDUSize;
	if (aPolicy.iMinAcceptableMaxSDUSize == 0)
		aPolicy.iMinAcceptableMaxSDUSize = aDefault.iMinAcceptableMaxSDUSize;
	if (aPolicy.iReqMaxRate.iUplinkRate == 0)
		aPolicy.iReqMaxRate.iUplinkRate = aDefault.iReqMaxRate.iUplinkRate;
	if (aPolicy.iReqMaxRate.iDownlinkRate == 0)
		aPolicy.iReqMaxRate.iDownlinkRate = aDefault.iReqMaxRate.iDownlinkRate;
	if (aPolicy.iMinAcceptableMaxRate.iUplinkRate == 0)
		aPolicy.iMinAcceptableMaxRate.iUplinkRate = aDefault.iMinAcceptableMaxRate.iUplinkRate;
	if (aPolicy.iMinAcceptableMaxRate.iDownlinkRate == 0)
		aPolicy.iMinAcceptableMaxRate.iDownlinkRate = aDefault.iMinAcceptableMaxRate.iDownlinkRate;
	if (aPolicy.iReqBER == RPacketQoS::EBERUnspecified)
		aPolicy.iReqBER = aDefault.iReqBER;
	if (aPolicy.iMaxBER == RPacketQoS::EBERUnspecified)
		aPolicy.iMaxBER = aDefault.iMaxBER;
	if (aPolicy.iReqSDUErrorRatio == RPacketQoS::ESDUErrorRatioUnspecified)
		aPolicy.iReqSDUErrorRatio = aDefault.iReqSDUErrorRatio;
	if (aPolicy.iMaxSDUErrorRatio == RPacketQoS::ESDUErrorRatioUnspecified)
		aPolicy.iMaxSDUErrorRatio = aDefault.iMaxSDUErrorRatio;
	if (aPolicy.iReqTrafficHandlingPriority == RPacketQoS::ETrafficPriorityUnspecified)
		aPolicy.iReqTrafficHandlingPriority = aDefault.iReqTrafficHandlingPriority;
	if (aPolicy.iMinTrafficHandlingPriority == RPacketQoS::ETrafficPriorityUnspecified)
		aPolicy.iMinTrafficHandlingPriority = aDefault.iMinTrafficHandlingPriority;
	if (aPolicy.iReqTransferDelay == 0)
		aPolicy.iReqTransferDelay = aDefault.iReqTransferDelay;
	if (aPolicy.iMaxTransferDelay == 0)
		aPolicy.iMaxTransferDelay = aDefault.iMaxTransferDelay;
	if (aPolicy.iReqGuaranteedRate.iUplinkRate == 0)
		aPolicy.iReqGuaranteedRate.iUplinkRate = aDefault.iReqGuaranteedRate.iUplinkRate;
	if (aPolicy.iReqGuaranteedRate.iDownlinkRate == 0)
		aPolicy.iReqGuaranteedRate.iDownlinkRate = aDefault.iReqGuaranteedRate.iDownlinkRate;
	if (aPolicy.iMinGuaranteedRate.iUplinkRate == 0)
		aPolicy.iMinGuaranteedRate.iUplinkRate = aDefault.iMinGuaranteedRate.iUplinkRate;
	if (aPolicy.iMinGuaranteedRate.iDownlinkRate == 0)
		aPolicy.iMinGuaranteedRate.iDownlinkRate = aDefault.iMinGuaranteedRate.iDownlinkRate;
	}


void ParameterMapper::CombineOverride(const TQoSRequested& aOverride, TQoSRequested& aPolicy)
	{
	if (aOverride.iReqTrafficClass != RPacketQoS::ETrafficClassUnspecified)
		aPolicy.iReqTrafficClass = aOverride.iReqTrafficClass;
	if (aOverride.iMinTrafficClass != RPacketQoS::ETrafficClassUnspecified)
		aPolicy.iMinTrafficClass = aOverride.iMinTrafficClass;
	if (aOverride.iReqDeliveryOrderReqd != RPacketQoS::EDeliveryOrderUnspecified)
		aPolicy.iReqDeliveryOrderReqd = aOverride.iReqDeliveryOrderReqd;
	if (aOverride.iMinDeliveryOrderReqd != RPacketQoS::EDeliveryOrderUnspecified)
		aPolicy.iMinDeliveryOrderReqd = aOverride.iMinDeliveryOrderReqd;
	if (aOverride.iReqDeliverErroneousSDU != RPacketQoS::EErroneousSDUDeliveryUnspecified)
		aPolicy.iReqDeliverErroneousSDU = aOverride.iReqDeliverErroneousSDU;
	if (aOverride.iMinDeliverErroneousSDU != RPacketQoS::EErroneousSDUDeliveryUnspecified)
		aPolicy.iMinDeliverErroneousSDU = aOverride.iMinDeliverErroneousSDU;
	if (aOverride.iReqMaxSDUSize > 0)
		aPolicy.iReqMaxSDUSize = aOverride.iReqMaxSDUSize;
	if (aOverride.iMinAcceptableMaxSDUSize > 0)
		aPolicy.iMinAcceptableMaxSDUSize = aOverride.iMinAcceptableMaxSDUSize;
	if (aOverride.iReqMaxRate.iUplinkRate > 0)
		aPolicy.iReqMaxRate.iUplinkRate = aOverride.iReqMaxRate.iUplinkRate;
	if (aOverride.iReqMaxRate.iDownlinkRate > 0)
		aPolicy.iReqMaxRate.iDownlinkRate = aOverride.iReqMaxRate.iDownlinkRate;
	if (aOverride.iMinAcceptableMaxRate.iUplinkRate > 0)
		aPolicy.iMinAcceptableMaxRate.iUplinkRate = aOverride.iMinAcceptableMaxRate.iUplinkRate;
	if (aOverride.iMinAcceptableMaxRate.iDownlinkRate > 0)
		aPolicy.iMinAcceptableMaxRate.iDownlinkRate = aOverride.iMinAcceptableMaxRate.iDownlinkRate;
	if (aOverride.iReqBER != RPacketQoS::EBERUnspecified)
		aPolicy.iReqBER = aOverride.iReqBER;
	if (aOverride.iMaxBER != RPacketQoS::EBERUnspecified)
		aPolicy.iMaxBER = aOverride.iMaxBER;
	if (aOverride.iReqSDUErrorRatio != RPacketQoS::ESDUErrorRatioUnspecified)
		aPolicy.iReqSDUErrorRatio = aOverride.iReqSDUErrorRatio;
	if (aOverride.iMaxSDUErrorRatio != RPacketQoS::ESDUErrorRatioUnspecified)
		aPolicy.iMaxSDUErrorRatio = aOverride.iMaxSDUErrorRatio;
	if (aOverride.iReqTrafficHandlingPriority != RPacketQoS::ETrafficPriorityUnspecified)
		aPolicy.iReqTrafficHandlingPriority = aOverride.iReqTrafficHandlingPriority;
	if (aOverride.iMinTrafficHandlingPriority != RPacketQoS::ETrafficPriorityUnspecified)
		aPolicy.iMinTrafficHandlingPriority = aOverride.iMinTrafficHandlingPriority;
	if (aOverride.iReqTransferDelay > 0)
		aPolicy.iReqTransferDelay = aOverride.iReqTransferDelay;
	if (aOverride.iMaxTransferDelay > 0)
		aPolicy.iMaxTransferDelay = aOverride.iMaxTransferDelay;
	if (aOverride.iReqGuaranteedRate.iUplinkRate > 0)
		aPolicy.iReqGuaranteedRate.iUplinkRate = aOverride.iReqGuaranteedRate.iUplinkRate;
	if (aOverride.iReqGuaranteedRate.iDownlinkRate > 0)
		aPolicy.iReqGuaranteedRate.iDownlinkRate = aOverride.iReqGuaranteedRate.iDownlinkRate;
	if (aOverride.iMinGuaranteedRate.iUplinkRate > 0)
		aPolicy.iMinGuaranteedRate.iUplinkRate = aOverride.iMinGuaranteedRate.iUplinkRate;
	if (aOverride.iMinGuaranteedRate.iDownlinkRate > 0)
		aPolicy.iMinGuaranteedRate.iDownlinkRate = aOverride.iMinGuaranteedRate.iDownlinkRate;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	if (aOverride.iSignallingIndication == 0 && aOverride.iReqTrafficClass == RPacketQoS::ETrafficClassInteractive )
		aPolicy.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority1;
#endif // SYMBIAN_NETWORKING_UMTSR5
	aPolicy.iHeaderCompression = aOverride.iHeaderCompression;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	aPolicy.iSignallingIndication = aOverride.iSignallingIndication;
	aPolicy.iSourceStatisticsDescriptor =aOverride.iSourceStatisticsDescriptor;
#endif // SYMBIAN_NETWORKING_UMTSR5
	}

// if SBLP presents, this method will return ETrue
TBool RFlowExtensionParams::ParsePolicyDataForSblp( CExtensionPolicy * aExtensionPolicy )
	{
	TBool retValue = EFalse;

	TExtensionQueueIter i(aExtensionPolicy->Extensions());
	CExtension* extension;
	while ((extension = i++) != NULL)
		{
		if (extension->Type() != STATIC_CAST(TUint, KPfqosExtensionSBLP))
			continue;
		
		LOG(Log::Printf(_L("")));
		LOG(Log::Printf(_L("PARSED SBLP VALUES")));
		
		// KPfqosExtensionSBLP presents! so return ETrue
		retValue = ETrue;

		TVariableQueueIter iter(extension->Queue());
		TVariableBase *var;

		TUint16 mediaComponentNumberCount = 0;
		TUint16 ipFlowNumberCount = 0;
		RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier flowIdTemp;

		while ((var = iter++) != NULL)
			{
			switch (var->Type())
				{
			case KPfqosTypeInteger:
				{
				TIntVariable* tmp = (TIntVariable*) var;
				//lint -e{961} would want terminating 'else' (but we don't?)
				if (tmp->Name().Compare(KDescSblpMediaComponentNumber)==0)
					{
					TInt value = tmp->Value();
					flowIdTemp.iMediaComponentNumber = static_cast<TUint16>(value);
					LOG(Log::Printf(_L("\tMedia component number = %d"),flowIdTemp.iMediaComponentNumber));
					mediaComponentNumberCount++;
					}
				else if (tmp->Name().Compare(KDescSblpIPFlowNumber)==0)
					{
					TInt value = tmp->Value();
					flowIdTemp.iIPFlowNumber = static_cast<TUint16>(value);
					LOG(Log::Printf(_L("\tIP flow number = %d"),flowIdTemp.iIPFlowNumber));
					ipFlowNumberCount++;

					if(mediaComponentNumberCount == ipFlowNumberCount)
						{
						iFlowIds.Append(flowIdTemp);
						}
					} 
				}
				break;
				
			case KPfqosTypeString:
				{
				TStringVariable* stringVar = (TStringVariable*) var;
				if (stringVar->Name().Compare(KDescSblpMediaAuthorizationToken)==0)
					{
					//TBuf<KMatSize> strValue = stringVar->Value();
					iAuthorizationToken.Copy(stringVar->Value());
					LOG(Log::Printf(_L("\tMAT string = '%S'"), &stringVar->Value()));
					}
				}
				break;

			default:
				break;
				}
			}
		}
	return retValue;
	}

//lint -e{1927} would like to use initializer list
RFlowExtensionParams::RFlowExtensionParams()
	{
	}

RFlowExtensionParams& RFlowExtensionParams::operator=(const RFlowExtensionParams& aFlowExtParam)
	{
	// NOP, if assigned to self
	if (this != &aFlowExtParam)
		{
		iAuthorizationToken.Copy(aFlowExtParam.iAuthorizationToken);
		iFlowIds.Reset();
		for(TInt i=0; i<aFlowExtParam.iFlowIds.Count();i++)
			{
			iFlowIds.Append(aFlowExtParam.iFlowIds[i]);
			}
		}
	return *this;
	}

RFlowExtensionParams::~RFlowExtensionParams()
	{
	iFlowIds.Close();
	}
