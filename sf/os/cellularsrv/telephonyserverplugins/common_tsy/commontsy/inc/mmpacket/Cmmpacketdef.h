/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CLASS DECLARATION
* This file contains the constants used by MMPACKET.
* CMmPacketQos constants
*
*/





        const TUint KMmPrecedence = 
            RPacketQoS::EUnspecifiedPrecedence +
            RPacketQoS::EPriorityHighPrecedence +
            RPacketQoS::EPriorityMediumPrecedence +
            RPacketQoS::EPriorityLowPrecedence ;

        const TUint KMmDelay = 
            RPacketQoS::EUnspecifiedDelayClass +
            RPacketQoS::EDelayClass1 +
            RPacketQoS::EDelayClass2 +
            RPacketQoS::EDelayClass3 +
            RPacketQoS::EDelayClass4 ;

        const TUint KMmReliability = 
            RPacketQoS::EUnspecifiedReliabilityClass +
            RPacketQoS::EReliabilityClass1 +
            RPacketQoS::EReliabilityClass2 +
            RPacketQoS::EReliabilityClass3 +
            RPacketQoS::EReliabilityClass4 +
            RPacketQoS::EReliabilityClass5 ;

        const TUint KMmPeak = 
            RPacketQoS::EUnspecifiedPeakThroughput +
            RPacketQoS::EPeakThroughput1000 +
            RPacketQoS::EPeakThroughput2000 +
            RPacketQoS::EPeakThroughput4000 +
            RPacketQoS::EPeakThroughput8000 +
            RPacketQoS::EPeakThroughput16000 +
            RPacketQoS::EPeakThroughput32000 +
            RPacketQoS::EPeakThroughput64000 +
            RPacketQoS::EPeakThroughput128000 +
            RPacketQoS::EPeakThroughput256000 ;

        const TUint KMmMean = 
            RPacketQoS::EUnspecifiedMeanThroughput +
            RPacketQoS::EMeanThroughput100 +
            RPacketQoS::EMeanThroughput200 +
            RPacketQoS::EMeanThroughput500 +
            RPacketQoS::EMeanThroughput1000 +
            RPacketQoS::EMeanThroughput2000 +
            RPacketQoS::EMeanThroughput5000 +
            RPacketQoS::EMeanThroughput10000 +
            RPacketQoS::EMeanThroughput20000 +
            RPacketQoS::EMeanThroughput50000 +
            RPacketQoS::EMeanThroughput100000 +
            RPacketQoS::EMeanThroughput200000 +
            RPacketQoS::EMeanThroughput500000 +
            RPacketQoS::EMeanThroughput1000000 +
            RPacketQoS::EMeanThroughput2000000 +
            RPacketQoS::EMeanThroughput5000000 +
            RPacketQoS::EMeanThroughput10000000 +
            RPacketQoS::EMeanThroughput20000000 +
            RPacketQoS::EMeanThroughput50000000;

        const TUint KMmTrafficClass = 
            RPacketQoS::ETrafficClassUnspecified +
            RPacketQoS::ETrafficClassConversational +
            RPacketQoS::ETrafficClassStreaming +
            RPacketQoS::ETrafficClassInteractive +
            RPacketQoS::ETrafficClassBackground;

        const TUint KMmDeliveryOrderReqd = 
            RPacketQoS::EDeliveryOrderUnspecified +
            RPacketQoS::EDeliveryOrderRequired +
            RPacketQoS::EDeliveryOrderNotRequired;

        const TUint KMmErroneousSDUDelivery = 
            RPacketQoS::EErroneousSDUDeliveryUnspecified +
            RPacketQoS::EErroneousSDUNoDetection +
            RPacketQoS::EErroneousSDUDeliveryRequired +
            RPacketQoS::EErroneousSDUDeliveryNotRequired;

        const TUint KMmBitErrorRatio = 
            RPacketQoS::EBERUnspecified +
            RPacketQoS::EBERFivePerHundred +
            RPacketQoS::EBEROnePerHundred +
            RPacketQoS::EBERFivePerThousand +
            RPacketQoS::EBERFourPerThousand +
            RPacketQoS::EBEROnePerThousand +
            RPacketQoS::EBEROnePerTenThousand +
            RPacketQoS::EBEROnePerHundredThousand +
            RPacketQoS::EBEROnePerMillion +
            RPacketQoS::EBERSixPerHundredMillion;

        const TUint KMmSDUErrorRatio = 
            RPacketQoS::ESDUErrorRatioUnspecified +
            RPacketQoS::ESDUErrorRatioOnePerTen +
            RPacketQoS::ESDUErrorRatioOnePerHundred +
            RPacketQoS::ESDUErrorRatioSevenPerThousand +
            RPacketQoS::ESDUErrorRatioOnePerThousand +
            RPacketQoS::ESDUErrorRatioOnePerTenThousand +
            RPacketQoS::ESDUErrorRatioOnePerHundredThousand +
            RPacketQoS::ESDUErrorRatioOnePerMillion;

        const TUint KMmTrafficHandlingPriority = 
            RPacketQoS::ETrafficPriorityUnspecified +
            RPacketQoS::ETrafficPriority1 +
            RPacketQoS::ETrafficPriority2 +
            RPacketQoS::ETrafficPriority3;
