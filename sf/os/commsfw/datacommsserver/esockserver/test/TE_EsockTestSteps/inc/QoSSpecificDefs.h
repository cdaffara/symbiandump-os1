/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file Te_EsockStepBaseDefs.h
*/
#if (!defined QOSSPECIFICDEFS_H)
#define QOSSPECIFICDEFS_H

//qos specific

_LIT(KTe_TrafficClass,"TrafficClass");
_LIT(KTe_DeliveryOrder,"DeliveryOrder");
_LIT(KTe_ErroneousSDUDelivery,"ErrSDUDeliv");
_LIT(KTe_ResidualBitErrorRatio,"ResBitErrorRatio");
_LIT(KTe_SDUErrorRatio,"SDUErrorRatio");
_LIT(KTe_TrafficHandlingPriority,"TraffHandlPriority");
_LIT(KTe_TransferDelay,"TransferDelay");
_LIT(KTe_MaxSduSize,"MaxSduSize");
_LIT(KTe_MaxBitrateUplink,"MaxBitrUplink");
_LIT(KTe_MaxBitrateDownlink,"MaxBitrDownlink");
_LIT(KTe_GuaBitrateUplink,"GuaBitrUplink");
_LIT(KTe_GuaBitrateDownlink,"GuaBitrDownlink");
//Qos R5 parameters
_LIT(KTe_SignallingIndicator,"SignallingIndicator");
_LIT(KTe_SrcStatisticsDescriptor,"SourceStatisticsDescriptor");
//IMS Parameters
_LIT(KTe_IMSSignallingIndicator,"IMSSignallingIndicator");

//qos specific options
_LIT(KTe_SubConIPLink99TrafficClassUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99TrafficClassConversational,"Conversational");
_LIT(KTe_SubConIPLink99TrafficClassStreaming,"Streaming");
_LIT(KTe_SubConIPLink99TrafficClassInteractive,"Interactive");
_LIT(KTe_SubConIPLink99TrafficClassBackground,"Background");

_LIT(KTe_SubConIPLink99DeliveryOrderUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99DeliveryOrderRequired,"Required");
_LIT(KTe_SubConIPLink99DeliveryOrderNotRequired,"NotRequired");

_LIT(KTe_SubConIPLink99ErrSDUDeliveryUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99ErrSDUDeliveryDetection,"Detection");
_LIT(KTe_SubConIPLink99ErrSDUDeliveryRequired,"Required");
_LIT(KTe_SubConIPLink99ErrSDUDeliveryNotRequired,"NotRequired");

_LIT(KTe_SubConIPLink99ResBitErrorRatioUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99ResBitErrorRatioFivePerHundred,"FivePerHundred");
_LIT(KTe_SubConIPLink99ResBitErrorRatioOnePerHundred,"OnePerHundred");
_LIT(KTe_SubConIPLink99ResBitErrorRatioFivePerThousand,"FivePerThousand");
_LIT(KTe_SubConIPLink99ResBitErrorRatioFourPerThousand,"FourPerThousand");
_LIT(KTe_SubConIPLink99ResBitErrorRatioOnePerThousand,"OnePerThousand");
_LIT(KTe_SubConIPLink99ResBitErrorRatioOnePerTenThousand,"OnePerTenThousand");
_LIT(KTe_SubConIPLink99ResBitErrorRatioOnePerHundredThousand,"OnePerHundredThousand");
_LIT(KTe_SubConIPLink99ResBitErrorRatioOnePerMillion,"OnePerMillion");
_LIT(KTe_SubConIPLink99ResBitErrorRatioSixPerHundredMillion,"SixPerHundredMillion");

_LIT(KTe_SubConIPLink99SDUErrorRatioUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerTen,"OnePerTen");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerHundred,"OnePerHundred");
_LIT(KTe_SubConIPLink99SDUErrorRatioSevenPerThousand,"SevenPerThousand");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerThousand,"OnePerThousand");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerTenThousand,"OnePerTenThousand");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerHundredThousand,"OnePerHundredThousand");
_LIT(KTe_SubConIPLink99SDUErrorRatioOnePerMillion,"OnePerMillion");

_LIT(KTe_SubConIPLink99TrafficPriorityUnspecified,"Unspecified");
_LIT(KTe_SubConIPLink99TrafficPriority1,"Priority1");
_LIT(KTe_SubConIPLink99TrafficPriority2,"Priority2");
_LIT(KTe_SubConIPLink99TrafficPriority3,"Priority3");

//Qos R5 specific options
_LIT(KTe_SubConIPLinkR5SrcStatisticDescUnknown,"Unknown");
_LIT(KTe_SubConIPLinkR5SrcStatisticDescSpeech,"Speech");

_LIT(KTe_SubConIPLink99ParamsSetName,"IP99Set");

#endif //QOSSPECIFICDEFS_H

