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

#include "comreadwrite.h"
#include "ABSTRACT.H"
#include "SCONCONC.H"
#include "GLOBAL.H"
#include <e32base.h>

SCONconCommand::SCONconCommand()
	{
	}

SCONconCommand::SCONconCommand(CComReadWrite* aComReadWrite)
	{
	iComReadWrite = aComReadWrite;
	}

SCONconCommand::~SCONconCommand()
	{
	}

	/*
	* Method description:	Builds the packet
	*
*/

void SCONconCommand::Create()
	{
	//TPtr8 d = m_oData.Des();
	
	TBuf8<255> neginf; // neginf
	//TPtr8 n = neginf.Des();
	
	neginf.Append(KTranpNegVer);
	neginf.Append(KTranpFR);
	neginf.Append(m_PDU);
	neginf.Append(KTranpCR);
	neginf.Append(KTranpLF);
	neginf.Append(KTranpID);
	neginf.Append(KTranpProductInformationString);
	neginf.Append(KTranpCR);
	neginf.Append(KTranpLF);
	
	m_oData.Append(0x00);
	m_oData.Append(KTranpMsgTypeCEC);
	m_oData.Append(KTranpInfTypeNeg);
	
	m_oData.Append(neginf.Length() + 18); // Hardcoded, but always the same .. size of the parts below.
	
	m_oData.Append(KTranpInfVer);
	m_oData.Append(KTranpCFLGie);
	
	m_oData.Append(m_PMID);
	m_oData.Append(m_SMID);
	
	m_oData.Append(neginf);
	}

	/*
	* Method description:	Sends the packet
	*
*/

void SCONconCommand::DoIt()
	{
	iComReadWrite->Send(m_oData);
	}

void SCONconCommand::GetReply()
	{
	// We won't use this at all!
	}

	/*
	* Method description:	Retrieves the packet - used for debugging purposes
	*
	* Returns:				Pointer to the packet
*/

TDesC8& SCONconCommand::GetData()
	{
	return m_oData;
	}

	/*
	* Method description:	Sets the Primary Machine ID
	*
	* Parameter:			a_PMID - the new Machine ID
*/

void SCONconCommand::SetPMID(TPtr8 a_PMID)
	{
	m_PMID = a_PMID;
	}

	/*
	* Method description:	Sets the Secondary Machine ID
	*
	* Parameter:			a_SMID - the new Machine ID
*/

void SCONconCommand::SetSMID(TPtr8 a_SMID)
	{
	m_SMID = a_SMID;
	}

	/*
	* Method description:	Sets the maximum receivable PDU size
	*
	* Parameter:			a_PDU - the new PDU size
*/

void SCONconCommand::SetPDU(TUint8 a_PDU)
	{
	m_PDU = a_PDU;
	}
