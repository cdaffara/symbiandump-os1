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
#ifndef __ETELDEFAULTQOS_H__
#define __ETELDEFAULTQOS_H__

#ifndef __ETELQOS_H__

class RPacketQoS
    {
    // The enums TTrafficClass, TDeliveryOrder,TErroneousSDUDelivery, TBitErrorRatio,
     // TSDUErrorRatio, TTrafficHandlingPriority have been assigned values because
     // the same enums are used both in the TQoSR99_R4Requested / Negotiated classes and 
     // in the TQoSCapsR99_R4 class. The Caps class has to indicate which, for instance, 
     // traffic classes are supported in a bitfield, so the enums have been defined as 
     // different bits in a bit field.
public: 
    enum TTrafficClass          
         {
         ETrafficClassUnspecified    = 0x01,     //< Traffic class - Unspecified
         ETrafficClassConversational = 0x02,     //< Traffic class - Conversational
         ETrafficClassStreaming      = 0x04,     //< Traffic class - Streaming
         ETrafficClassInteractive    = 0x08,     //< Traffic class - Interactive
         ETrafficClassBackground     = 0x10      //< Traffic class - Background
         };

     enum TDeliveryOrder     
         {
         EDeliveryOrderUnspecified   = 0x01,     //< SDU Delivery order - Unspecified
         EDeliveryOrderRequired      = 0x02,     //< SDU Delivery order - Required to be in sequence
         EDeliveryOrderNotRequired   = 0x04      //< SDU Delivery order - Not Required to be in sequence
         };

     enum TErroneousSDUDelivery      // Erroneous SDU Delivery
         {
         EErroneousSDUDeliveryUnspecified    = 0x01, //< Unspecified
         EErroneousSDUNoDetection            = 0x02, //< Erroneous SDUs delivered - Error detection not considered.
         EErroneousSDUDeliveryRequired       = 0x04, //< Erroneous SDUs delivered + error indication - Error detection employed.
         EErroneousSDUDeliveryNotRequired    = 0x08  //< Erroneous SDUs discarded - Error detection is employed.
         };

     enum TBitErrorRatio             // Residual Bit Error Rate
         {
         EBERUnspecified             = 0x01,     //< Target residual undetected BER - Unspecified
         EBERFivePerHundred          = 0x02,     //< Target residual BER - 0.05
         EBEROnePerHundred           = 0x04,     //< Target residual BER - 0.01
         EBERFivePerThousand         = 0x08,     //< Target residual BER - 0.005
         EBERFourPerThousand         = 0x10,     //< Target residual BER - 0.004
         EBEROnePerThousand          = 0x20,     //< Target residual BER - 0.001
         EBEROnePerTenThousand       = 0x40,     //< Target residual BER - 0.0001
         EBEROnePerHundredThousand   = 0x80,     //< Target residual BER - 0.00001
         EBEROnePerMillion           = 0x100,    //< Target residual BER - 0.000001
         EBERSixPerHundredMillion    = 0x200     //< Target residual BER - 0.00000006
         };

     enum TSDUErrorRatio             // SDU Error Ratio
         {
         ESDUErrorRatioUnspecified           = 0x01, //< Target value of Erroneous SDUs - Unspecified
         ESDUErrorRatioOnePerTen             = 0x02, //< Target SDU error ratio - 0.1
         ESDUErrorRatioOnePerHundred         = 0x04, //< Target SDU error ratio - 0.01
         ESDUErrorRatioSevenPerThousand      = 0x08, //< Target SDU error ratio - 0.007
         ESDUErrorRatioOnePerThousand        = 0x10, //< Target SDU error ratio - 0.001
         ESDUErrorRatioOnePerTenThousand     = 0x20, //< Target SDU error ratio - 0.0001
         ESDUErrorRatioOnePerHundredThousand = 0x40, //< Target SDU error ratio - 0.00001
         ESDUErrorRatioOnePerMillion         = 0x80  //< Target SDU error ratio - 0.000001
         };

     enum TTrafficHandlingPriority   // Traffic handling priority
         {
         ETrafficPriorityUnspecified = 0x01,     //< Unspecified Priority level
         ETrafficPriority1           = 0x02,     //< Priority level 1
         ETrafficPriority2           = 0x04,     //< Priority level 2
         ETrafficPriority3           = 0x08      //< Priority level 3
         };
     /**
      Source statistics descriptor - as defined in 3GPP TS 23.107 and TS 24.008.
      
      @publishedPartner
      @released
      */
      enum TSourceStatisticsDescriptor    
          {
          /** Unknown source statistics descriptor. */
          ESourceStatisticsDescriptorUnknown  = 0x0,      
          /** Speech source statistics descriptor. */
          ESourceStatisticsDescriptorSpeech   = 0x01,      
          };
    };

#endif//#ifndef__ETELQOS_H__

#endif//#ifndef__ETELDEFAULTQOS_H__
