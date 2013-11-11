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
// CR99QoSConversion.cpp
// Contains methods for converting R97 QoS parameters to R99/R4 parameters
// 
//

/**
 @file
 @internalComponent
*/

#include <e32cons.h>
#include <e32std.h>
#include <bacline.h>
#include "ceddumper.h"
#include "R99QoSConversion.h"
#include "ceddumpglobals.h"
#include "dbdef.h"
#include <etelpckt.h>
#include <etelqos.h>
#include <faxdefn.h>
#include <networking/panroles.h>

/**
Creates new class instance
*/
CR99QoSConversion* CR99QoSConversion::NewL()
	{
		CR99QoSConversion *self = new (ELeave) CR99QoSConversion();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;		
	}

/**
Constructes, initialises the object
*/
void CR99QoSConversion::ConstructL()
	{
	iNext = NULL;
	iId = 1;
	iModificationFlag = EFalse;
	
	// Initialize the values. 
	// the remaing, unitialised values are set to '0' by CBase
	iGPRSReqTrafficClass                    = RPacketQoS::ETrafficClassUnspecified;
	iGPRSMinTrafficClass                    = RPacketQoS::ETrafficClassUnspecified;
	iGPRSReqDeliveryOrder                   = RPacketQoS::EDeliveryOrderUnspecified;
	iGPRSMinDeliveryOrder                   = RPacketQoS::EDeliveryOrderUnspecified;
	iGPRSReqDeliverErroneousSDU             = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	iGPRSMinDeliverErroneousSDU             = RPacketQoS::EErroneousSDUDeliveryUnspecified;
	iGPRSReqBER                             = RPacketQoS::EBERUnspecified;
	iGPRSMaxBER                             = RPacketQoS::EBERUnspecified;
	iGPRSReqSDUErrorRatio                   = RPacketQoS::ESDUErrorRatioUnspecified;
	iGPRSMaxSDUErrorRatio                   = RPacketQoS::ESDUErrorRatioUnspecified;
	iGPRSReqTrafficHandlingPriority         = RPacketQoS::ETrafficPriorityUnspecified;
	iGPRSMinTrafficHandlingPriority         = RPacketQoS::ETrafficPriorityUnspecified;
	iGPRSSignallingIndication               = EFalse;
	iGPRS_ImCnSignallingIndication          = EFalse;
	
	//Fixed Value
	iGPRSReqMaxSDUSize                      = 1500;
	iGPRSMinAcceptableMaxSDUSize            = 1500;
	
	iGPRSReqDeliveryOrder                   = RPacketQoS::EDeliveryOrderRequired;
	
	}
	
/**
Deletes object
*/
CR99QoSConversion::~CR99QoSConversion()
	{
	delete iNext;
	}

/**
Check if given column is not deprecated within incoming/outgoing GPRS table. 
Additionally converts the setting to new R99 GPRS QoS values if necessary. 
Assumes setting is given as R97/98 GPRS QoS parameter. 

@param aColumn A reference to a descriptor containing the name of a column in the current record. 
@param aSetting The setting
@return CR99QoSConversion::TConvRecordType
*/
CR99QoSConversion::TConvRecordType CR99QoSConversion::CheckAndConvert(TPtrC& aColumn, TPtrC& aSetting )
	{
	
	TLex temp(aSetting);
	TInt number;
	
	temp.Val(number);
	
	if (aColumn.Compare(TPtrC(GPRS_REQ_PRECEDENCE)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EPriorityHighPrecedence) :
			  iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority1;
			  break;
			case (RPacketQoS::EPriorityMediumPrecedence) :
			  iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority2;
			  break;
			case (RPacketQoS::EPriorityLowPrecedence) :
			  iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority3;
			  break;
			default : break;
			}
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_REQ_DELAY)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EDelayClass1):
			   iGPRSReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority1;
			   break;
			case (RPacketQoS::EDelayClass2):
			   iGPRSReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority2;
			   break;
			case (RPacketQoS::EDelayClass3):
			   iGPRSReqTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSReqTrafficHandlingPriority   = RPacketQoS::ETrafficPriority3;
			   break;
			case (RPacketQoS::EDelayClass4):
			   iGPRSReqTrafficClass = RPacketQoS::ETrafficClassBackground;
			   break;
			default :
			   break;
			}
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_REQ_RELIABILITY)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EReliabilityClass1) :
			    iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
			    iGPRSReqBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass2) :
			    iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
			    iGPRSReqBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass3) :
			    iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
			    iGPRSReqBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass4) :
			    iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
			    iGPRSReqBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass5) :
			    iGPRSReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
			    iGPRSReqBER           = RPacketQoS::EBERFourPerThousand;
			    iGPRSReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
			    break;
			default: break;
			}
		iModificationFlag = ETrue;		
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_REQ_PEAK_THROUGHPUT)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EPeakThroughput1000) :
			     iGPRSReqMaxUplinkRate     = 8;
			     iGPRSReqMaxDownlinkRate       = 8;
			     break;
			 case (RPacketQoS::EPeakThroughput2000) :
			     iGPRSReqMaxUplinkRate    = 16;
			     iGPRSReqMaxDownlinkRate       = 16;
			     break;
			 case (RPacketQoS::EPeakThroughput4000) :
			     iGPRSReqMaxUplinkRate     = 32;
			     iGPRSReqMaxDownlinkRate       = 32;
			     break;
			 case (RPacketQoS::EPeakThroughput8000) :
			     iGPRSReqMaxUplinkRate    = 64;
			     iGPRSReqMaxDownlinkRate       = 64;
			     break;
			 case (RPacketQoS::EPeakThroughput16000) :
			     iGPRSReqMaxUplinkRate     = 128;
			     iGPRSReqMaxDownlinkRate       = 128;
			     break;
			 case (RPacketQoS::EPeakThroughput32000) :
			     iGPRSReqMaxUplinkRate     = 256;
			     iGPRSReqMaxDownlinkRate       = 256;
			     break;
			 case (RPacketQoS::EPeakThroughput64000) :
			     iGPRSReqMaxUplinkRate    = 512;
			     iGPRSReqMaxDownlinkRate       = 512;
			     break;
			 case (RPacketQoS::EPeakThroughput128000) :
			     iGPRSReqMaxUplinkRate     = 1024;
			     iGPRSReqMaxDownlinkRate      = 1024;
			     break;
			 case (RPacketQoS::EPeakThroughput256000) :
			     iGPRSReqMaxUplinkRate    = 2048;
			     iGPRSReqMaxDownlinkRate       = 2048;
			     break;
			 default : break;
			 }
		iModificationFlag = ETrue;		
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_REQ_MEAN_THROUGHPUT)) == 0)
		{
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_MIN_PRECEDENCE)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EPriorityHighPrecedence) :
			  iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority1;
			  break;
			case (RPacketQoS::EPriorityMediumPrecedence) :
			  iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority2;
			  break;
			case (RPacketQoS::EPriorityLowPrecedence) :
			  iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority3;
			  break;
			default : break;
			}
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_MIN_DELAY)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EDelayClass1):
			   iGPRSMinTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority1;
			   break;
			case (RPacketQoS::EDelayClass2):
			   iGPRSMinTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority2;
			   break;
			case (RPacketQoS::EDelayClass3):
			   iGPRSMinTrafficClass = RPacketQoS::ETrafficClassInteractive;
			   iGPRSMinTrafficHandlingPriority   = RPacketQoS::ETrafficPriority3;
			   break;
			case (RPacketQoS::EDelayClass4):
			   iGPRSMinTrafficClass = RPacketQoS::ETrafficClassBackground;
			   break;
			default :
			   break;
			}
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_MIN_RELIABILITY))== 0)
		{
		switch (number)
			{
			case (RPacketQoS::EReliabilityClass1) :
			    iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
			    iGPRSMaxBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass2) :
			    iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerMillion;
			    iGPRSMaxBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass3) :
			    iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
			    iGPRSMaxBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass4) :
			    iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
			    iGPRSMaxBER           = RPacketQoS::EBEROnePerHundredThousand;
			    iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
			    break;
			case (RPacketQoS::EReliabilityClass5) :
			    iGPRSMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand;
			    iGPRSMaxBER           = RPacketQoS::EBERFourPerThousand;
			    iGPRSMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired;
			    break;
			default: break;
			}
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_MIN_PEAK_THROUGHPUT)) == 0)
		{
		switch (number)
			{
			case (RPacketQoS::EPeakThroughput1000) :
			     iGPRSReqMinUplinkRate     = 8;
			     iGPRSReqMinDownlinkRate       = 8;
			     break;
			 case (RPacketQoS::EPeakThroughput2000) :
			     iGPRSReqMinUplinkRate    = 16;
			     iGPRSReqMinDownlinkRate       = 16;
			     break;
			 case (RPacketQoS::EPeakThroughput4000) :
			     iGPRSReqMinUplinkRate     = 32;
			     iGPRSReqMinDownlinkRate       = 32;
			     break;
			 case (RPacketQoS::EPeakThroughput8000) :
			     iGPRSReqMinUplinkRate    = 64;
			     iGPRSReqMinDownlinkRate       = 64;
			     break;
			 case (RPacketQoS::EPeakThroughput16000) :
			     iGPRSReqMinUplinkRate     = 128;
			     iGPRSReqMinDownlinkRate       = 128;
			     break;
			 case (RPacketQoS::EPeakThroughput32000) :
			     iGPRSReqMinUplinkRate     = 256;
			     iGPRSReqMinDownlinkRate       = 256;
			     break;
			 case (RPacketQoS::EPeakThroughput64000) :
			     iGPRSReqMinUplinkRate    = 512;
			     iGPRSReqMinDownlinkRate       = 512;
			     break;
			 case (RPacketQoS::EPeakThroughput128000) :
			     iGPRSReqMinUplinkRate     = 1024;
			     iGPRSReqMinDownlinkRate      = 1024;
			     break;
			 case (RPacketQoS::EPeakThroughput256000) :
			     iGPRSReqMinUplinkRate    = 2048;
			     iGPRSReqMinDownlinkRate       = 2048;
			     break;
			 default : break;
			 }
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else if (aColumn.Compare(TPtrC(GPRS_MIN_MEAN_THROUGHPUT)) == 0)
		{
		iModificationFlag = ETrue;
		return CR99QoSConversion::EDeprecatedRecord;
		}
	else
		{
		return CR99QoSConversion::EValidRecord;
		}
	}


/**
Set iNext to the next CR99QoSConversion object in the list.

@param aNext handle to CR99QoSConversion object
*/
void CR99QoSConversion::SetNext(CR99QoSConversion* aNext)
	{
	iNext = aNext;
	aNext->SetId(iId + 1);
	}

/**
Retrieves iNext values

@return iNext handle to next CR99QoSConversion object in the list
*/
CR99QoSConversion* CR99QoSConversion::GetNext()
	{
	return iNext;
	}

/**
Retrieves identification number

@return iId object's identification number
*/
TInt CR99QoSConversion::GetId()
	{
	return iId;
	}

/**
Set object identification number 

@param aId identification number
*/
void CR99QoSConversion::SetId(TInt aId)
	{
	iId = aId;
	}

/**
Prints out all R99 values in particular format

@param aFileDumper handle to the object responsible for writing to the output file
*/
void CR99QoSConversion::PrintValues(CFileDump* aFileDumper)
	{
	TBuf<MAX_BUFFER_LEN> buf;
	TPtrC column;
	TPtrC setting;
	
	ASSERT(aFileDumper);
	
	aFileDumper->WriteSectionHeader(iId);
	
	buf.Copy(TPtrC(QOS_UMTS_R99_AND_ON_TABLE));
	buf.AppendNum(iId);
	column.Set(TPtrC(COMMDB_NAME));
	setting.Set(buf);
	aFileDumper->WriteColumnValue(column, setting);
	
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_TRAFFIC_CLASS), iGPRSReqTrafficClass);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_TRAFFIC_CLASS), iGPRSMinTrafficClass);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_DELIVERY_ORDER),iGPRSReqDeliveryOrder);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_DELIVERY_ORDER),iGPRSMinDeliveryOrder);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_DELIVER_ERRONEOUS_SDU),iGPRSReqDeliverErroneousSDU); 
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_DELIVER_ERRONEOUS_SDU),iGPRSMinDeliverErroneousSDU);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_MAX_SDUSIZE),iGPRSReqMaxSDUSize);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_ACCEPTABLE_MAX_SDU_SIZE),iGPRSMinAcceptableMaxSDUSize);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_MAX_UPLINK_RATE),iGPRSReqMaxUplinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_MIN_UPLINK_RATE),iGPRSReqMinUplinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_MAX_DOWNLINK_RATE),iGPRSReqMaxDownlinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_MIN_DOWNLINK_RATE),iGPRSReqMinDownlinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_BER),iGPRSReqBER);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MAX_BER),iGPRSMaxBER);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_SDU_ERROR_RATIO),iGPRSReqSDUErrorRatio);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MAX_SDU_ERROR_RATIO),iGPRSMaxSDUErrorRatio);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_TRAFFIC_HANDLING_PRIORITY),iGPRSReqTrafficHandlingPriority);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_TRAFFIC_HANDLING_PRIORITY),iGPRSMinTrafficHandlingPriority);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_TRANSFER_DELAY),iGPRSReqTransferDelay);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MAX_TRANSFER_DELAY),iGPRSMaxTransferDelay);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_GUARANTEED_UPLINK_RATE),iGPRSReqGuaranteedUplinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_GUARANTEED_UPLINK_RATE),iGPRSMinGuaranteedUplinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_REQ_GUARANTEED_DOWNLINK_RATE),iGPRSReqGuaranteedDownlinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_MIN_GUARANTEED_DOWNLINK_RATE),iGPRSMinGuaranteedDownlinkRate);
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_SIGNALLING_INDICATION),(iGPRSSignallingIndication == RPacketQoS::EDeliveryOrderRequired)?TPtrC(_S("TRUE")):TPtrC(_S("FALSE")));
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_IM_CN_SIGNALLING_INDICATOR),(iGPRS_ImCnSignallingIndication == RPacketQoS::EDeliveryOrderRequired)?TPtrC(_S("TRUE")):TPtrC(_S("FALSE")));
	PrintLine(aFileDumper, TPtrC(GPRS_QOS_SOURCE_STATISTICS_DESCRIPTOR),iGPRSSourceStatisticsDescriptor);

	aFileDumper->WriteFieldCount(27);
	aFileDumper->WriteSectionFooter(iId);
	}

/**
Prints out the line to the output file

@param aFileDumper handle to the object responsible for writing to the output file
@param aCol name of the column to print out
@param aSetting intiger column's value to print out
*/
void CR99QoSConversion::PrintLine(CFileDump* aFileDumper, TPtrC aCol, TInt aSetting)
	{
	TBuf<MAX_BUFFER_LEN> buf;
	TPtrC column;
	TPtrC setting;
	
	column.Set(aCol);
	buf.Num(aSetting);
	setting.Set(buf);
	aFileDumper->WriteColumnValue(column, setting);
	}

/**
Prints out all R99 values in particular format

@param aFileDumper handle to the object responsible for writing to the output file
@param aCol name of the column to print out
@param aSetting pointer to string to print out as a column's value
*/
void CR99QoSConversion::PrintLine(CFileDump* aFileDumper, TPtrC aCol, TPtrC aSetting)
	{
	TPtrC column;
	TPtrC setting;
	column.Set(aCol);
	setting.Set(aSetting);
	aFileDumper->WriteColumnValue(column, setting);
	}

/**
Retrieves modification flag

@return iModificationFlag modification flag
*/	
TBool CR99QoSConversion::GetModificationFlag()
	{
	return iModificationFlag;
	}
