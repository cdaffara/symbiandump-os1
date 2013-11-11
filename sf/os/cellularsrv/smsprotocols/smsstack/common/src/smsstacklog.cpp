// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Logging macros code specific structures in SMSStack.
// 
//

/**
 @file
 @internalComponent
*/
 

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsstacklogTraces.h"
#endif

#include "gsmupdu.h"
#include "smsstacklog.h"
#include <s32mem.h>

//
// All functions in this file are available only if logging is enabled.
//
#ifdef OST_TRACE_COMPILER_IN_USE


/**
 *  Number of characters per line during hex dump logging.
 */
const TInt  KHexDumpCharsPerLine = 32;


/**
 *  Converts a PDU from a buffer into a ASCII string and decodes it.
 *
 *  @param aPDU         PDU to log.
 *  @param aCommandPdu  Flag to say if this is a command PDU.
 */
void LogSmsIfPDUL(const TDesC8& aText, const TDesC8& aPDU, TBool aCommandPdu)
	{
	TBuf8<128>  tmpBuf;

	//
	// Log the PDU as raw hex...
	//
	tmpBuf.Copy(aText);
	tmpBuf.Append(_L8("RAW:  "));

	LogSmsIfHexBuf(tmpBuf, aPDU);

	//
	// Log the first octet...
	//
	// bit	   7	6	 5	  4    3	2	 1	  0
	// submit  rp	udhi srr  vpf  vpf	rd	 0	  1  mti, rd,  vpf, rp, udhi, srr,
	// deliver rp	udhi sri			mms  0	  0  mti, mms, rp, udhi, sri
	// status		udhi srq			mms  1	  0  mti, udhi, mms, srq
	// command		udhi srr				 1    0  mti, udhi, srr
	//
	enum TSmsVp
		{
		EVpAbs,
		EVpEnh,
		EVpRel,
		EVpNone,
		};
	
	CSmsPDU::TSmsPDUType  pduType = (CSmsPDU::TSmsPDUType) -1;
	TInt  octetOffset = 0;
	TText8  firstOctet = aPDU[octetOffset++];
	TBool  udhiPresent = EFalse;
	TSmsVp  vpf = EVpRel;

	tmpBuf.Zero();
	
	if ((firstOctet & 0x03) == 0x00)
		{
		pduType = CSmsPDU::ESmsDeliver;
		tmpBuf.Append(_L8("DELIVER "));
		}
	else if ((firstOctet & 0x03) == 0x01)
		{
		pduType = CSmsPDU::ESmsSubmit;
		tmpBuf.Append(_L8("SUBMIT  "));
		}
	else if((firstOctet & 0x03) == 0x02)
		{
		if (aCommandPdu)
			{
			pduType = CSmsPDU::ESmsCommand;
			tmpBuf.Append(_L8("COMMAND "));
			}
		else
			{
			pduType = CSmsPDU::ESmsStatusReport;
			tmpBuf.Append(_L8("STATUS  "));
			}
		}
	else
		{
		tmpBuf.Append(_L8("UNKNOWN "));
		}

	if (pduType == CSmsPDU::ESmsSubmit  ||  pduType == CSmsPDU::ESmsDeliver)
		{
		if (firstOctet & 0x80)
			{
			tmpBuf.Append(_L8("    1"));
			}
		else
			{
			tmpBuf.Append(_L8("    0"));
			}
		}
	else
		{
		tmpBuf.Append(_L8("    -"));
		}

	if (firstOctet & 0x40)
		{
		tmpBuf.Append(_L8("    1"));
		udhiPresent = ETrue;
		}
	else
		{
		tmpBuf.Append(_L8("    0"));
		}

	if (firstOctet & 0x20)
		{
		tmpBuf.Append(_L8("    1"));
		}
	else
		{
		tmpBuf.Append(_L8("    0"));
		}

	if (pduType == CSmsPDU::ESmsDeliver  ||  pduType == CSmsPDU::ESmsStatusReport)
		{
		// MMS is Deliver or Status Report PDUs only...
		if (firstOctet & 0x04)
			{
			tmpBuf.Append(_L8("    1"));
			}
		else
			{
			tmpBuf.Append(_L8("    0"));
			}
		}
	else
		{
		tmpBuf.Append(_L8("    -"));
		}

	if (pduType == CSmsPDU::ESmsSubmit)
		{
		// Reject Duplicates and VP is for Sumbit PDUs only.
		if (firstOctet & 0x04)
			{
			tmpBuf.Append(_L8("    1"));
			}
		else
			{
			tmpBuf.Append(_L8("    0"));
			}

		if ((firstOctet & 0x18) == 0x18)
			{
			tmpBuf.Append(_L8("    ABS"));
			vpf = EVpAbs;
			}
		else if ((firstOctet & 0x18) == 0x10)
			{
			tmpBuf.Append(_L8("    REL"));
			vpf = EVpRel;
			}
		else if ((firstOctet & 0x18) == 0x08)
			{
			tmpBuf.Append(_L8("    ENH"));
			vpf = EVpEnh;
			}
		else
			{
			tmpBuf.Append(_L8("    NO "));
			vpf = EVpNone;
			}
		}
	else
		{
		tmpBuf.Append(_L8("    -      -"));
		}

	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_1, "%s      HEX   MTI        RP UDHI  SRX  MMS   RD     VP", aText);
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_2, "%sFO:   0x%02X  %s", aText, (TUint)firstOctet, tmpBuf);

	if (pduType == -1)
		{
		// Not supported!
		return;
		}

	//
	//
	// Message reference (submit, status report and command only)...
	//
	if (pduType == CSmsPDU::ESmsSubmit  ||
	    pduType == CSmsPDU::ESmsStatusReport  ||
	    pduType == CSmsPDU::ESmsCommand)
		{
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_3, "%sMR:   0x%02X", aText, aPDU[octetOffset]);
        octetOffset++;
		}

	//
	// Command's PID, CT and MN...
	//
	if (pduType == CSmsPDU::ESmsCommand)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_4, "%sPID:  0x%02X", aText, aPDU[octetOffset]);
        octetOffset++;

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_5, "%sCT:   0x%02X", aText, aPDU[octetOffset]);
        octetOffset++;

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_6, "%sMN:   0x%02X", aText, aPDU[octetOffset]);
        octetOffset++;
		}

	//
	// Log the telephone number...
	//
	TInt  telLength = (TInt) aPDU[octetOffset];
	TInt  typeOfNumber = aPDU[octetOffset+1];
	
	octetOffset += 2;
		
	if (telLength >= 0  &&  telLength <= 20)
		{
		// Convert the number to ascii...
		tmpBuf.Zero();
		
		for (TInt  count = 0;  count < (telLength + 1) / 2;  count++)
			{
			tmpBuf.AppendFormat(_L8("%1X%1X"), aPDU[octetOffset] & 0x0f,
								(aPDU[octetOffset] >> 4) & 0x0f);
			octetOffset++;
			}
		
		tmpBuf.SetLength(telLength);

		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_7, "%sTEL:  0x%02X  %s", aText, (TUint)typeOfNumber, tmpBuf);
		}
	else
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_8, "%sTEL:  Illegal length value (%d)!", aText, telLength);
		return;
		}

	//
	// PID and the DCS (submit and deliver only)...
	//
	if (pduType == CSmsPDU::ESmsSubmit  ||  pduType == CSmsPDU::ESmsDeliver)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_9, "%sPID:  0x%02X", aText, aPDU[octetOffset]);
		octetOffset++;

		tmpBuf.Zero();

		TInt  dcs = aPDU[octetOffset];
		octetOffset++;

		if ((dcs & 0xF0 != 0xF0)  &&
		    ((dcs & 0xD0) == 0x00  ||  (dcs & 0xC0) == 0xC0  ||
		     (dcs & 0xD0) == 0xD0  ||  (dcs & 0xE0) == 0xE0))
			{
			//
			// Classless messages:
			// 00x0xxxx GSM 7 bit default alphabet with no message class
			// 1100xxxx Message Waiting Indication Group: Discard Message
			// 1101xxxx Message Waiting Indication Group: Store Message
			// 1110xxxx Message Waiting Indication Group: Store Message (UCS2 character set)
			//
			tmpBuf.Append(_L8("NO        "));
			}
		else if ((dcs & 0xC0) == 0x00  ||  (dcs & 0x40) == 0x40  ||  (dcs & 0xF0) == 0xF0)
			{
			//
			// Message has a class:
			// 00xxxxxx General Data Coding indication
			// 01xxxxxx Message Marked for Automatic Deletion Group
			// 1111xxxx Data coding/message class
			//
			if ((dcs & 0x03) == 0x00)		//class 0 sms
				{
				tmpBuf.Append(_L8("0         "));
				}
			else if((dcs & 0x03) == 0x03)	//class 3 sms
				{
				tmpBuf.Append(_L8("3         "));
				}
			else if((dcs & 0x01) == 0x01)	//class 1 sms
				{
				tmpBuf.Append(_L8("1         "));
				}
			else if((dcs & 0x02) == 0x02)	//class 2 sms
				{
				tmpBuf.Append(_L8("2         "));
				}
			}
		else
			{
			//
			// 1000xxxx...1011xxxx Reserved coding groups
			//
			tmpBuf.Append(_L8("NO        "));
			}

		if ((dcs & 0xF0) == 0xF0)
			{
			// Data coding/message class
			if ((dcs & 0x04) == 0x00)
				{
				tmpBuf.Append(_L8(" 7 BIT   "));
				}
			else if ((dcs & 0x04) == 0x04)
				{
				tmpBuf.Append(_L8(" 8 BIT   "));
				}
			}
		else if ((dcs & 0xC0) == 0xC0  ||  (dcs & 0xD0) == 0xD0)
			{
			//Message Waiting Indication: Discard Message or Store Message
			tmpBuf.Append(_L8(" 7 BIT   "));
			}
		else if ((dcs & 0xE0) == 0xE0)
			{
			//Message Waiting Indication: Store Message (UCS2 character set)
			tmpBuf.Append(_L8("16 BIT   "));
			}
		else if ((dcs & 0xC0) == 0x00 ||  (dcs & 0x40) == 0x40)
			{
			// General Data Coding indication or Automatic Deletion Group
			if ((dcs & 0x0C) == 0x00)
				{
				tmpBuf.Append(_L8(" 7 BIT   "));
				}
			else if ((dcs & 0x0C) == 0x0C)
				{
				tmpBuf.Append(_L8("Reserved "));
				}
			else if ((dcs & 0x04) == 0x04)
				{
				tmpBuf.Append(_L8(" 8 BIT   "));
				}
			else if ((dcs & 0x08) == 0x08)
				{
				tmpBuf.Append(_L8("16 BIT   "));
				}
			}
		else
			{
			// Reserved coding groups
			tmpBuf.Append(_L("Reserved "));
			}

		if ((dcs & 0xF0 != 0xF0)  &&
		    ((dcs & 0xC0) == 0xC0  ||  (dcs & 0xD0) == 0xD0  ||  (dcs & 0xE0) == 0xE0))
			{
			if ((dcs & 0x03) == 0x00)
				{
				// Voicemail Message Waiting
				tmpBuf.Append(_L8("Voicemail Message Waiting"));
				}
			else if ((dcs & 0x03) == 0x03)
				{
				// Other Message Waiting
				tmpBuf.Append(_L8("Other Message Waiting"));
				}
			else if ((dcs & 0x01) == 0x01)
				{
				// Fax Message Waiting
				tmpBuf.Append(_L8("Fax Message Waiting"));
				}
			else if ((dcs & 0x02) == 0x02)
				{
				// Electronic Mail Message Waiting
				tmpBuf.Append(_L8("Electronic Mail Message Waiting"));
				}
			else
				{
				tmpBuf.Append(_L8("-"));
				}
			}
		else
			{
			tmpBuf.Append(_L8("-"));
			}

		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_10, "%sDCS:  HEX   CLASS      DCS     INDICATION TYPE", aText);
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_11, "%s      0x%02X  %s", aText, (TUint) dcs, tmpBuf);
		}

	//
	// VP (submit only)...
	//
	if (pduType == CSmsPDU::ESmsSubmit)
		{
		if (vpf == EVpRel)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_12, "%sVP:   %d (Relative)", aText, aPDU[octetOffset++]);
			}
		else if (vpf == EVpAbs)
			{
			// absolute format	  -  TODO to look for the right format
			tmpBuf.Zero();
			
			for (TInt index = 0;  index < 7;  index++)
				{
				tmpBuf.AppendFormat(_L8("%02X"), aPDU[octetOffset + index]);
				}
			
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_13, "%sVP:   %s (Absolute)", aText, tmpBuf);
			octetOffset += 7;
			}
		else if (vpf == EVpEnh)
			{
			// enhanced validity period   -  TODO to look for the right format
			tmpBuf.Zero();
			
			for (TInt index = 0;  index < 7;  index++)
				{
				tmpBuf.AppendFormat(_L8("%02X"), aPDU[octetOffset + index]);
				}
			
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_14, "%sVP:   %s (Enhanced)", aText, tmpBuf);
			octetOffset += 7;
			}
		}

	//
	// SCTS (deliver and status)  -  TODO to look for the right format
	//
	if (pduType == CSmsPDU::ESmsDeliver  ||  pduType == CSmsPDU::ESmsStatusReport)
		{
		tmpBuf.Zero();
		
		for (TInt index = 0;  index < 7;  index++)
			{
			tmpBuf.AppendFormat(_L8("%02X"), aPDU[octetOffset + index]);
			}
			
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_15, "%sSCTS: %s", aText, tmpBuf);
		octetOffset += 7;
		}
	
	//
	// Getting the DT and the status of the SR (SR only)
	// TODO	to look for some appended octet after the status, eg. carried ud
	//      and to analise the status
	//
	if (pduType == CSmsPDU::ESmsStatusReport)
		{
		tmpBuf.Zero();
		
		for (TInt index = 0;  index < 7;  index++)
			{
			tmpBuf.AppendFormat(_L8("%02X"), aPDU[octetOffset + index]);
			}
			
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_16, "%sDT:   %s", aText, tmpBuf);
		octetOffset += 7;

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_17, "%sST:   %02X", aText, aPDU[octetOffset]);
		octetOffset++;
		}

	//
	// Getting the UDL   -    TODO to add UD later and to decode the UDH
	//
	if (pduType == CSmsPDU::ESmsDeliver  ||  pduType == CSmsPDU::ESmsSubmit)
		{
		if (udhiPresent)
			{
			TInt  udl  = aPDU[octetOffset++];
			TInt  udhl = aPDU[octetOffset++];
			
			if (udhl > 1)
				{
				tmpBuf.Zero();
			
				// getting iei_a
				TInt  iei_a = aPDU[octetOffset++];
	
				// getting ieidl_a
				TInt  ieidl_a = aPDU[octetOffset++];
	
				if (ieidl_a < udhl)
					{
					//
					// With EDS, IEIDL can  be almost the whole payload; need to limit
					// length of debug or increase udlbuf2 to around 300 bytes (which would
					// make stack frame around 700 bytes out of allowed 2kB; seems too much)
					// TODO: think whether we should explicitly show this truncation, eg with "..."
					//
					for (TInt index = 0;  index < ieidl_a;  index++)
						{
						tmpBuf.AppendFormat(_L8("%02X"), aPDU[octetOffset + index]);
						if (tmpBuf.Length() > tmpBuf.MaxLength() - 2  ||
							octetOffset + index == aPDU.Length() - 1)
							{
							break;
							}
						}
					}
				else
					{
					OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_18, "%sUDL:  Problems with the ieidl_a %d being less that UDHL",aText, ieidl_a);
					}
	
				if ((ieidl_a + 1) > udhl)
					{
					OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_19, "%sUDL:  Corrupted or implement decoding for second iei_b, iei_n!",aText);
					}
	
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_20, "%sUDL:  HEX    UDHL   IEI_A  IEIDL_A   ", aText);

				TBuf8<200> data;
				RDesWriteStream dataStream(data);

				dataStream.WriteInt32L(aText.Length());
				dataStream.WriteL(aText);

				dataStream.WriteInt32L(udl);
				dataStream.WriteInt32L(udhl);
				dataStream.WriteInt32L(iei_a);
				dataStream.WriteInt32L(ieidl_a);

				dataStream.WriteInt32L(tmpBuf.Length());
				dataStream.WriteL(tmpBuf);

				dataStream.Close();
                OstTraceDefData( OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_21, "%s      0x%02X   0x%02X   0x%02X   0x%02X  %s", data.Ptr(), data.Size() );
				}
			else
				{
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_22, "%sUDL:  Corrupted because TP-UDHP is TRUE and TP-UDHL is less than 1!", aText);
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_23, "%sUDL:  HEX   UDHL   IEI_A", aText);
				OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_24, "%s      0x%02X    0x%02X", aText, udl, udhl);
				}
			}
		else
			{
			TInt  udl  = aPDU[octetOffset++];
			
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_25, "%sUDL:  HEX    UDHL", aText);
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFPDUL_26, "%s      0x%02X   -", aText, udl);
			}
		}
	} // LogSmsIfPDUL


#ifdef OST_TRACE_COMPILER_IN_USE
void LogSmsIfHexBuf(const TDesC8& aText, const TDesC8& aHexBuf)
#else
void LogSmsIfHexBuf(const TDesC8& /*aText*/, const TDesC8& aHexBuf)
#endif
    {
	//
	// Print the PDU in hex in rows of upto KHexDumpCharsPerLine bytes...
	//
	TBuf8<KHexDumpCharsPerLine * 2>  hexLine;
    TInt  length = aHexBuf.Length();
	TInt  position;

	for (position = 0;  position < length;  position += KHexDumpCharsPerLine)
		{
		TInt  bytesToLog = length - position;
		TInt  byteIndex;
		
		if (bytesToLog > KHexDumpCharsPerLine)
			{
			bytesToLog = KHexDumpCharsPerLine;
			}

		hexLine.Zero();

		for (byteIndex = 0;  byteIndex < bytesToLog;  byteIndex++)
			{
			hexLine.AppendFormat(_L8("%02X"), aHexBuf[position + byteIndex]);
			}

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFHEXBUF_1, "%s%s", aText, hexLine);
		}
    } // LogSmsIfHexBuf


/**
 *	Logs a Type of Number enum.
 *
 *  @param aTON  Enum to log.
 */
#ifdef OST_TRACE_COMPILER_IN_USE
void LogSmsIfTypeOfNumber(const TDesC8& aText, RMobilePhone::TMobileTON aTON)
#else
void LogSmsIfTypeOfNumber(const TDesC8& /*aText*/, RMobilePhone::TMobileTON aTON)
#endif
	{
	switch (aTON)
		{
		case RMobilePhone::EUnknownNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_1, "%sEUnknownNumber", aText);
			}
			break;

		case RMobilePhone::EInternationalNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_2, "%sEInternationalNumber", aText);
			}
			break;

		case RMobilePhone::ENationalNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_3, "%sENationalNumber", aText);
			}
			break;

		case RMobilePhone::ENetworkSpecificNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_4, "%sENetworkSpecificNumber", aText);
			}
			break;

		case RMobilePhone::ESubscriberNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_5, "%sESubscriberNumber", aText);
			}
			break;

		case RMobilePhone::EAlphanumericNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_6, "%sEAlphanumericNumber", aText);
			}
			break;

		case RMobilePhone::EAbbreviatedNumber:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_7, "%sEAbbreviatedNumber", aText);
			}
			break;

		default:
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFTYPEOFNUMBER_8, "%s<unknown enum %d>", aText, aTON);
			}
			break;
		}
	} // LogSmsIfTypeOfNumber


/**
 *	Logs a Numbering Plan enum.
 *
 *  @param aNPI  Enum to log.
 */
#ifdef OST_TRACE_COMPILER_IN_USE
void LogSmsIfNumberingPlan(const TDesC8& aText, RMobilePhone::TMobileNPI aNPI)
#else
void LogSmsIfNumberingPlan(const TDesC8& /*aText*/, RMobilePhone::TMobileNPI aNPI)
#endif
	{
	switch (aNPI)
		{
		case RMobilePhone::EUnknownNumberingPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_1, "%sEUnknownNumberingPlan", aText);
			}
			break;

		case RMobilePhone::EIsdnNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_2, "%sEIsdnNumberPlan", aText);
			}
			break;

		case RMobilePhone::EDataNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_3, "%sEDataNumberPlan", aText);
			}
			break;

		case RMobilePhone::ETelexNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_4, "%sETelexNumberPlan", aText);
			}
			break;

		case RMobilePhone::EServiceCentreSpecificPlan1:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_5, "%sEServiceCentreSpecificPlan1", aText);
			}
			break;

		case RMobilePhone::EServiceCentreSpecificPlan2:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_6, "%sEServiceCentreSpecificPlan2", aText);
			}
			break;

		case RMobilePhone::ENationalNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_7, "%sENationalNumberPlan", aText);
			}
			break;

		case RMobilePhone::EPrivateNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_8, "%sEPrivateNumberPlan", aText);
			}
			break;

		case RMobilePhone::EERMESNumberPlan:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_9, "%sEERMESNumberPlan", aText);
			}
			break;

		default:
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFNUMBERINGPLAN_10, "%s<unknown enum %d>", aText, aNPI);
			}
			break;
		}
	} // LogSmsIfNumberingPlan


/**
 *	Logs contents of TMobileGsmSmsEntryV1 object.
 *
 *  @param aSmsGsmEntryV1  Entry to log.
 */
void LogSmsIfSmsEntry(const TDesC8& aText,
					  const RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsGsmEntryV1)
	{
	//
	// Header and index...
	//
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_1, "%sRMobileSmsStore::TMobileGsmSmsEntryV1:", aText);
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_2, "%s  iIndex=%d (0x%08x)", aText, aSmsGsmEntryV1.iIndex,aSmsGsmEntryV1.iIndex);
	
	//
	// Message status...
	//
	switch (aSmsGsmEntryV1.iMsgStatus)
		{
		case RMobileSmsStore::EStoredMessageUnknownStatus:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_3, "%s  iStoreStats=EStoredMessageUnknownStatus", aText);
			}
			break;

		case RMobileSmsStore::EStoredMessageUnread:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_4, "%s  iStoreStats=EStoredMessageUnread", aText);
			}
			break;

		case RMobileSmsStore::EStoredMessageRead:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_5, "%s  iStoreStats=EStoredMessageRead", aText);
			}
			break;

		case RMobileSmsStore::EStoredMessageUnsent:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_6, "%s  iStoreStats=EStoredMessageUnsent", aText);
			}
			break;

		case RMobileSmsStore::EStoredMessageSent:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_7, "%s  iStoreStats=EStoredMessageSent", aText);
			}
			break;

		case RMobileSmsStore::EStoredMessageDelivered:
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_8, "%s  iStoreStats=EStoredMessageDelivered", aText);
			}
			break;

		default:
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_9, "%s  iStoreStats=<unknown enum %d>", aText,aSmsGsmEntryV1.iMsgStatus);
			}
			break;
		}

	//
	// Service centre number...
	//
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize>  numberIn8bit;

	numberIn8bit.Copy(aSmsGsmEntryV1.iServiceCentre.iTelNumber);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSMSENTRY_10, "%s  SRC Address=\"%s\"", aText, numberIn8bit);

	//
	// Service Centre Type Of Number...
	//
	TBuf8<128>  tmpBuf;
	
	tmpBuf.Copy(aText);
	tmpBuf.Append(_L8("  SRC AddrTON="));

	LogSmsIfTypeOfNumber(tmpBuf, aSmsGsmEntryV1.iServiceCentre.iTypeOfNumber);

	//
	// Number Plan...
	//
	tmpBuf.Copy(aText);
	tmpBuf.Append(_L8("  SRC AddrNPI="));

	LogSmsIfNumberingPlan(tmpBuf, aSmsGsmEntryV1.iServiceCentre.iNumberPlan);

	//
	// PDU...
	//
	tmpBuf.Copy(aText);
	tmpBuf.Append(_L8("  PDU: "));

	LogSmsIfPDUL(tmpBuf, aSmsGsmEntryV1.iMsgData, EFalse);
	} // LogSmsIfSmsEntry


/**
 *	Logs contents of TMobileSmsSendAttributesV1 object.
 *
 *  @param aAttrib  Attributes to log.
 */
void LogSmsIfSendAttributes(const TDesC8& aText,
							const RMobileSmsMessaging::TMobileSmsSendAttributesV1& aAttrib)
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_1, "%sRMobileSmsMessaging::TMobileSmsSendAttributesV1:", aText);
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_2, "%s  iFlags=0x%08x", aText, (TInt)(aAttrib.iFlags));

	if (aAttrib.iFlags & RMobileSmsMessaging::KGsmServiceCentre)
		{
		TBuf8<128 + RMobilePhone::KMaxMobileTelNumberSize>  tmpBuf;
		
		tmpBuf.Copy(aAttrib.iGsmServiceCentre.iTelNumber);
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_3, "%s  SRC Address=%s", aText, tmpBuf);

		tmpBuf.Copy(aText);
		tmpBuf.Append(_L8("  SRC AddrTON="));

		LogSmsIfTypeOfNumber(tmpBuf, aAttrib.iGsmServiceCentre.iTypeOfNumber);

		tmpBuf.Copy(aText);
		tmpBuf.Append(_L8("  SRC AddrNPI="));

		LogSmsIfNumberingPlan(tmpBuf, aAttrib.iGsmServiceCentre.iNumberPlan);
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KSmsDataFormat)
		{
		switch (aAttrib.iDataFormat)
			{
			case RMobileSmsMessaging::EFormatUnspecified:
				{
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_4, "%s  iDataFormat=EFormatUnspecified", aText);
				}
				break;

			case RMobileSmsMessaging::EFormatGsmTpdu:
				{
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_5, "%s  iDataFormat=EFormatGsmTpdu", aText);
				}
				break;

			case RMobileSmsMessaging::EFormatCdmaTpdu:
				{
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_6, "%s  iDataFormat=EFormatCdmaTpdu", aText);
				}
				break;

			default:
				{
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_7, "%s  iDataFormat=<unknown enum %d>", aText,aAttrib.iDataFormat);
				}
				break;
			}
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KCdmaTeleservice)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_8, "%s  iCdmaTeles=0x%08X", aText, (TUint)aAttrib.iCdmaTeleservice);
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KCdmaServiceCategory)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_9, "%s  iCdmaServ=0x%08X", aText, (TUint)aAttrib.iCdmaServiceCategory);
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KRemotePartyInfo)
		{
		TBuf8<RMobilePhone::KMaxMobileTelNumberSize>  tmpBuf;
		
		tmpBuf.Copy(aAttrib.iDestination.iTelNumber);
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_10, "%s  DST Address=%s", aText, tmpBuf);

		tmpBuf.Copy(aText);
		tmpBuf.Append(_L8("  DST AddrTON="));

		LogSmsIfTypeOfNumber(tmpBuf, aAttrib.iDestination.iTypeOfNumber);

		tmpBuf.Copy(aText);
		tmpBuf.Append(_L8("  DST AddrNPI="));

		LogSmsIfNumberingPlan(tmpBuf, aAttrib.iDestination.iNumberPlan);
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KMoreToSend)
		{
		if (aAttrib.iMore)
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_11, "%s  iMore=ETrue", aText);
			}
		else
			{
			OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_12, "%s  iMore=EFalse", aText);
			}
		}

	if (aAttrib.iFlags & RMobileSmsMessaging::KMessageReference)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, LOGSMSIFSENDATTRIBUTES_13, "%s  iMsgRef=0x%08X", aText, (TUint)aAttrib.iMsgRef);
		}

	if(aAttrib.iFlags & RMobileSmsMessaging::KGsmSubmitReport)
		{
		TBuf8<RMobilePhone::KMaxMobileTelNumberSize>  tmpBuf;
		
		tmpBuf.Copy(aText);
		tmpBuf.Append(_L8("  iSubmitReport PDU: "));

		LogSmsIfPDUL(tmpBuf, aAttrib.iSubmitReport, EFalse);
		}
	} // LogSmsIfSendAttributes

#endif // OST_TRACE_COMPILER_IN_USE
