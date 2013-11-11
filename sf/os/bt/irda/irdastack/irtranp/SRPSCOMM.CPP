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

#include <e32base.h>
#include "comreadwrite.h"
#include "GLOBAL.H"
#include "SRPSCOMM.H"


SRPSCommand::SRPSCommand()
	{
	}

SRPSCommand::SRPSCommand(CComReadWrite* aComReadWrite)
	{
	iComReadWrite = aComReadWrite;
	}

SRPSCommand::~SRPSCommand()
	{
	}

	/*
	* Method description:	Builds the packet
	*
*/

void SRPSCommand::Create()
	{
	//TPtr8 d = m_oData.Des();
	
	m_oData.Append(0x00);
	m_oData.Append(KTranpMsgTypeData);
	m_oData.Append(KTranpInfTypeUser);
	
	m_oData.Append(m_userDataPtr.Length() + 28 + 4); // Hardcoded, but always the same .. size of the parts below.
	
	m_oData.Append(KTranpInfVer);
	m_oData.Append(KTranpDFLGnseg); // hardcoded, must be different for different commands
	
	TUint16 length3 = (TUint16)(m_userDataPtr.Length() + 28); // Hardcoded, but always the same .. size of the parts below.
	IrTranpUtil::DAppend(&m_oData, length3);
	
	m_oData.Append(0x58);
	m_oData.Append(m_PduType);
	
	TUint32 length4 = m_userDataPtr.Length() + 22; // Hardcoded, but always the same .. size of the parts below.
	IrTranpUtil::LAppend(&m_oData, length4);
	
	m_oData.Append(m_PMID);
	m_oData.Append(m_SMID);
	
	IrTranpUtil::DAppend(&m_oData, m_SPID);//reversed the order of these 2 lines - CAR
	IrTranpUtil::DAppend(&m_oData, m_DPID);
	IrTranpUtil::DAppend(&m_oData, m_CmdId);
	
	m_oData.Append(m_userDataPtr);
	
	}

	/*
	* Method description:	Sends the packet
	*
*/

void SRPSCommand::DoIt()
	{
	iComReadWrite->Send(m_oData);
	}

void SRPSCommand::GetReply()
	{
	// We won't use this at all!
	}

	/*
	* Method description:	Retrieves the packet - used for debugging purposes
	*
	* Returns:				Pointer to the packet
*/

TDesC8& SRPSCommand::GetData()
	{
	return m_oData;
	}

	/*
	* Method description:	Sets the Primary Machine ID
	*
	* Parameter:			a_PMID - the new Machine ID
*/

void SRPSCommand::SetPMID(TPtr8 a_PMID)
	{
	m_PMID = a_PMID;
	}

	/*
	* Method description:	Sets the Secondary Machine ID
	*
	* Parameter:			a_SMID - the new Machine ID
*/

void SRPSCommand::SetSMID(TPtr8 a_SMID)
	{
	m_SMID = a_SMID;
	}

	/*
	* Method description:	Sets the type of PDU where acking
	*
	* Parameter:			a_PduType - the type of PDU
*/

void SRPSCommand::SetPDUType(TUint8 a_PduType)
	{
	m_PduType = a_PduType;
	}

	/*
	* Method description:	Sets the ID of the command where acking
	*
	* Parameter:			a_CmdId - the command id
*/

void SRPSCommand::SetCmdId(TUint16 a_CmdId)
	{
	m_CmdId = a_CmdId;
	}

	/*
	* Method description:	Sets the destination process id
	*
	* Parameter:			a_DPID - the process id
*/

void SRPSCommand::SetDPID(TUint16 a_DPID)
	{
	m_DPID = a_DPID;
	}

	/*
	* Method description:	Sets the source process id
	*
	* Parameter:			a_SPID - the process id
*/

void SRPSCommand::SetSPID(TUint16 a_SPID)
	{
	m_SPID = a_SPID;
	}

	/*
	* Method description:	Sets the userdata specific for this ack
	*
	* Parameter:			a_userDataPtr - pointer to the data
*/

void SRPSCommand::SetUserData(TDes8& a_userDataPtr)
	{
	m_userDataPtr.Set(a_userDataPtr);
	}
