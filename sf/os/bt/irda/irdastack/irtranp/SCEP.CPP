// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "debug.h"

#include <e32math.h>
#include <e32svr.h> 
#include "comreadwrite.h"
#include "tranp.h"
#include "tranpprot.h"
#include "SCEP.H"
#include "COMMANDP.H"
#include "ABSTRACT.H"
#include "SCONREQC.H"
#include "SCONCONC.H"
#include "SRPSCOMM.H"
#include "GLOBAL.H"
#include "BFTP.H"


SCEP::SCEP()
	{
	}

/*
* Method description:	Leavable constructor for the SCEP layer
*
*/

SCEP* SCEP::NewL(CTranpProtocol* aTranpProtocol, BFTP* a_oBFTP, const MTranpNotification* aNotifier)
	{
	DEBUG_OPEN();
	SCEPPRINT(_L("SCEP::NewL()\n"));
	
	SCEP* self = new(ELeave) SCEP();
	CleanupStack::PushL(self);
	self->ConstructL(aTranpProtocol, a_oBFTP, aNotifier);
	CleanupStack::Pop();
	return self;
	}

/*
* Method description:	Second part of the SCEP construction - creates the CommandProcessor and CComReadWrite.
*
* Parameter:			Pointer to the BFTP class
*
*/

void SCEP::ConstructL(CTranpProtocol* aTranpProtocol, BFTP* a_oBFTP, const MTranpNotification* aNotifier)
	{
	
	SCEPPRINT(_L("SCEP::ConstructL()\n"));
	iNotifier = const_cast<MTranpNotification*>(aNotifier);
	iState = EWaiting;

	m_uiMaxPDU = 4;
	
	m_oCommandP = new(ELeave) CommandProcessor();
	iComReadWrite = CComReadWrite::NewL(*this);
	
	m_oBFTP = a_oBFTP;
	iTranpProtocol = aTranpProtocol;
	
	m_packet = NULL;
	m_iPicSize = 0;
	}

SCEP::~SCEP()
	{
	delete m_oCommandP;
	delete iComReadWrite;
	//delete m_packet; Ownership now belongs to TTranpPicture Object
	}

/*
* Method description:	Pump - receives and sends data, should be called periodically. Due to the way IrTran-P and
*						serial communication works, it also tries to check when it has received a complete SCEP
*						packet. This should be handled in a differerent (and more nice) way.
*
* Returns:				An integer containing the progress from 0 to 100
*/

void SCEP::Abort()
	{
	iComReadWrite->Cancel();
	iComReadWrite->Signal(KErrNone);
	}

TInt SCEP::Pump()
	{
	// Set up the IR port connection with a null write
	//delete m_packet;
	//m_packet = NULL;
	//iState = EWaiting;
	//iSCEPFrame.SetLength(0);
	iComReadWrite->Open();
	return 0;
	}

void SCEP::ReceiveComplete(TDesC8& /*aBuffer*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

void SCEP::SendComplete()
	{
	// Post Read Request
	SCEPPRINT(_L("SCEP: Send Complete\n"));
	iComReadWrite->Receive();
	}

void SCEP::SendError(TInt aError)
	{
	SCEPPRINT_2(_L("SCEP: Send Error %d\n"), aError);
	iNotifier->Error(aError);
	iState = EError;
	}

void SCEP::ReceiveError(TInt aError)
	{
	SCEPPRINT_2(_L("SCEP: Receive Error %d\n"), aError);
	iNotifier->Error(aError);
	iState = EError;
	}
void SCEP::Error(TInt aError)
	{
	SCEPPRINT_2(_L("SCEP: Error %d\n"), aError);
	iNotifier->Error(aError);
	iState = EError;
	}


/*
* Method description:	Called from BFTP when a connection request should be sent
*
*/

void SCEP::SCONreqL()
	{
	SCONreqCommand* command = new(ELeave) SCONreqCommand(iComReadWrite);
	command->SetPMID(m_PMID.Des());
	command->SetSMID(m_SMID.Des());
	command->SetPDU(m_uiMaxPDU);
	command->Create();
	//IrTranpUtil::HexOut(command->GetData());
	m_oCommandP->Add(command);
	}

/*
* Method description:	Called from BFTP when a connection confirmed should be sent
*
*/

void SCEP::SCONconL()
	{
	SCONconCommand* command = new(ELeave) SCONconCommand(iComReadWrite);
	command->SetPMID(m_PMID.Des());
	command->SetSMID(m_SMID.Des());
	command->SetPDU(m_uiMaxPDU);
	command->Create();
//	IrTranpUtil::HexOut(command->GetData());
	m_oCommandP->Add(command);
	}

/*
* Method description:	Called from BFTP when a common BFTP packet should be sent
*
*/

void SCEP::SCEPSendCommand(TDes8& /*a_userData*/, TUint8 /*a_pduType*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

/*
* Method description:	Private method called when a Connection Request packet should be parsed
*
* Parameter:			a_Header - pointer to the packet buffer
* Parameter:			offset - where in the buffer to start parsing
*/

void SCEP::SCEPConRequest(TDes8& a_Header, TInt offset)
	{
	SCEPPRINT(_L("Inside SCEPConRequest\n"));
	
	TInt length = a_Header.Length();

	while(offset<length)
		{
		if(a_Header[offset] == KTranpInfTypeVer)
			//		case IrTranpUtil::InfTypeVer:
			{
			offset+=2;
			}
		else if(a_Header[offset] == KTranpInfTypeNeg)
			{
			// Negotiation information
			//				TUint8 length = a_Header[offset+1];
			//				TUint8 InfVer = a_Header[offset+2];
			//				TUint8 CFLG = a_Header[offset+3];
			
			//TPtr8 SMID((unsigned char*)a_Header.Ptr()+offset+4, 8, a_Header.MaxLength());
			TPtr8 PMID((unsigned char*)a_Header.Ptr()+offset+12, 8, a_Header.MaxLength());
			
			m_PMID = PMID;
			//				m_SMID = SMID; // We want to keep ours
			
			NegInf(a_Header, offset+20);
			
			offset+=(length+2);
			}
		else if(a_Header[offset] == KTranpInfTypeExt)
			{
			// Ehh .. dunno .. "extend in the future"
			offset+=4;
			}
		else
			{
			// Error Not a Valid SCEP connection request...
			SCEPPRINT(_L("Not a Valid SCEPConRequest\n"));
			iNotifier->Error(KErrIrtranpPeerDoesNotHaveIrtranp); //KErrIrtranpPeerDoesNotHaveIrtranp
			break;
			}
		}
	SCEPPRINT(_L("Exiting SCEPConRequest\n"));
	}

/*
* Method description:	Private method called when a commond Data packet should be parsed
*
* Parameter:			a_Header - pointer to the packet buffer
* Parameter:			offset - where in the packet to start parsing
*/

void SCEP::SCEPDataCommand(TDes8& /*a_Header*/, TInt /*offset*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

/*
* Method description:	Adds segmented packets into a complete packet
*
* Parameter:			a_Header - pointer to the segment
* Parameter:			a_offset - where in the segment data to be assembled is
*/

void SCEP::AddSegment(TDes8& a_Header, TInt a_offset)
	{
	SCEPPRINT(_L("Entered AddSegment\n"));
	TPtr8 temp((unsigned char*)a_Header.Ptr()+a_offset, a_Header.Length()-a_offset, a_Header.MaxLength());
	TPtr8 temp2 = m_packet->Des();
	temp2.Append(temp);
	SCEPPRINT(_L("Exited AddSegment\n"));
	}

	/*
	* Method description:	Private method called to parse an SCEP disconnetion packet
	*
	* Parameter:			a_Header - pointer to the packet
	* Parameter:			offset - where in the packet to start parsing
*/

void SCEP::SCEPDisconnect(TDes8& a_Header, TInt offset)
	{
	SCEPPRINT(_L("SCEP:  SCEPDisconnect\n"));
	SCEPPRINT_2(_L("SCEP:  %d -> EDisconnected\r\n"), iState);
	
	if(a_Header[offset] == KTranpInfTypeRsn)
		{
		TUint16 reason = IrTranpUtil::DExtract(a_Header, offset+2);
		if(reason == KTranpDisPDUrsnUR)
			{
			SCEPPRINT(_L("SCEP: Disconnect: Unspecified Reason\n"));
			iState = EDisconnected;

			}
		else if(reason == KTranpDisPDUrsnUD)
			{
			SCEPPRINT(_L("SCEP: Disconnect: User Disconnect\n"));
			iState = EDisconnected;

			}
		else if(reason == KTranpDisPDUrsnPD)
			{
			SCEPPRINT(_L("SCEP: Disconnect: Provider Disconnect\n"));
			iState = EDisconnected;

			}
		else
			{
			// reserved
			}
		}
	else
		{
		// oops
		}
	SCEPPRINT(_L("SCEP: Exiting SCEPDisconnect\n"));
	return;
	}

/*
* Method description:	Private method called when parsing a Connection Confirmed packet
*
* Parameter:			a_Header - pointer to the packet
* Parameter:			offset - where in the packet to start parsing
*/

void SCEP::SCEPConConfirm(TDes8& a_Header, TInt offset)
	{
	SCEPPRINT(_L("Inside SCEPConConfirm\n"));
	
	TInt length = a_Header.Length();
	
	while(offset<length)
		{
		if(a_Header[offset] == KTranpInfTypeVer)
			{
			// Should always be 01?
			offset+=2;
			}
		else if(a_Header[offset] == KTranpInfTypeNeg)
			{
			// Negotiation information
			TUint8 length = a_Header[offset+1];
			//				TUint8 InfVer = a_Header[offset+2];
			//				TUint8 CFLG = a_Header[offset+3];
			
			TPtr8 PMID((unsigned char*)a_Header.Ptr()+offset+4, 8, a_Header.MaxLength());
			TPtr8 SMID((unsigned char*)a_Header.Ptr()+offset+12, 8, a_Header.MaxLength());
			
			m_PMID = PMID;
			m_SMID = SMID;
			
			NegInf(a_Header, offset+20);
			offset+=(length+2);
			}
		else if(a_Header[offset] == KTranpInfTypeExt)
			{
			// "extend in the future"
			
			SCEPPRINT(_L("InfTypeExt appeared in ConConfirm - error in protocol\n"));
			// Should not appear in a ConConfirm!
			offset+=4;
			}
		else //default
			{
			}
		} // End of While loop
	}

/*
* Method description:	Private method called when parsing the Command-section of a packet
*
* Parameter:			a_Header - pointer to the packet
* Parameter:			a_offset - where in the packet the command is
*/

void SCEP::DoCommand(const TDesC8& /*a_Header*/, TInt /*a_offset*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

/*
* Method description:	Private method called to parse the NegInf section of a packet
*
* Parameter:			a_Header - pointer to the packet
* Parameter:			a_offset - where in the packet the NegInf is
*/

void SCEP::NegInf(TDes8& a_Header, TInt a_offset)
	{
	SCEPPRINT(_L("SCEP: NegInf\n"));
	
	if(a_Header[a_offset] != KTranpNegVer)
		{
		return; // skip the NegInf totally
		}
	TInt length = a_Header.Length();
	while(a_offset<length) // Parse all NegContent attributes
		{
		TBuf<8> l(8);
		l.Num(a_offset);
		TBuf<8> k(8);
		k.Num(length);
		
		TBufC8<4> temp;
		TPtr8 temp2 = temp.Des();
		for(TInt j=0;j<3;j++) // we don't like it .. hardcoded
			{
			temp2.Append(a_Header[a_offset+1+j]);
			}
		
		TInt valueOffset = a_offset+1+temp.Length();
		if(temp.Compare(KTranpFR) == 0)
			{
			SCEPPRINT(_L("SCEP: Found FR attribute\n"));
			// pump whitespace
			if(a_Header[valueOffset] == ' ')
				{
				valueOffset++;
				}
			
			m_uiMaxPDU = a_Header[valueOffset];
			
			// Now skip CRLF - pump
			TBool found = FALSE;
			while(found == FALSE)
				{
				if(a_Header[valueOffset] == KTranpLF) // Why look for both CR and LF? LF always follows CR .. 
					{
					found = TRUE;
					}
				else
					valueOffset++;
				}
			}
		else if(temp.Compare(KTranpID) == 0)
			{
			SCEPPRINT(_L("SCEP: Found ID attribute\n"));
			TBool found = FALSE;
			TPtr8 temp = m_szPIS.Des();
			temp.Zero();
			do
				{
				if(a_Header[valueOffset] != KTranpCR)
					{
					temp.Append(a_Header[valueOffset++]);
					}
				else
					{
					found = TRUE;
					valueOffset+=2; // skip LF too
					}
				}
				while(found == FALSE);
			}
		else if(temp.Compare(KTranpNM) == 0)
			{
			SCEPPRINT(_L("SCEP: Found NM attribute\n"));
			TBool found = FALSE;
			TPtr8 temp = m_szUserName.Des();
			temp.Zero();
			do
				{
				if(a_Header[valueOffset] != KTranpCR)
					{
					temp.Append(a_Header[valueOffset++]);
					}
				else
					{
					found = TRUE;
					valueOffset+=2; // skip LF too
					}
				}
				while(found == FALSE);
			}
		else if(temp.Compare(KTranpPW) == 0)
			{
			SCEPPRINT(_L("SCEP: Found PW attribute\n"));
			TBool found = FALSE;
			TPtr8 temp = m_szPassword.Des();
			temp.Zero();
			do
				{
				if(a_Header[valueOffset] != KTranpCR)
					{
					temp.Append(a_Header[valueOffset++]);
					}
				else
					{
					found = TRUE;
					valueOffset+=2; // skip LF too
					}
				}
				while(found == FALSE);
			}
		else
			{
			// Something that wasn't an attribute
			}
		a_offset = valueOffset;
		
	}
	iState = EConnected;
	//iNotifier->Connected();
	SCEPPRINT(_L("SCEP: Exiting NegInf\n"));
}

//
// Method description:	Private method called when a packet has been received. Here is where the parsing begins.
//						Also creates the necessary reply-commands at SCEP level.
//
// Parameter:			a_TempBuffer - the packet itself
//

void SCEP::ParseCommand(TDes8& /*a_TempBuffer*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

//
// Method description:	Sets the Primary Machine ID
//
// Parameter:			a_PMID - the new Machine ID
//

void SCEP::SetPMID(const TDesC8& aPMID)
	{
	m_PMID = aPMID;
	}

//
// Method description:	Sets the Secondary Machine ID
//
// Parameter:			a_SMID - the new Machine ID
//

void SCEP::SetSMID(const TDesC8& aSMID)
	{
	m_SMID = aSMID;
	}

//
// Method description:	Sets the maximum receivable PDU size
//
// Parameter:			a_PDU - the new PDU size
//
void SCEP::SetPDU(TUint8 a_PDU)
	{
	m_uiMaxPDU = a_PDU;
	}

