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
 
#ifndef SRPSCOMMAND_H
#define SRPSCOMMAND_H

#include "ABSTRACT.H"
#include <e32base.h>

class Global;

/*
 * Name:		SRPSCommand
 *
 * Description:	Data Confirmed command
 *
 */

NONSHARABLE_CLASS(SRPSCommand) : public AbstractCommand
{
private:
	SRPSCommand();
	// Buffer to hold this packet
	TBuf8<255> m_oData;
	// Primary Machine ID
	TBufC8<8> m_PMID;
	// Secondary Machine ID
	TBufC8<8> m_SMID;
	// Type of the PDU
	TUint8 m_PduType;
	// Pointer to the userdata we get
	TPtrC8 m_userDataPtr;
	// Id of the command
	TUint16 m_CmdId;
	// Source Process ID
	TUint16 m_SPID;
	// Destination Process ID
	TUint16 m_DPID;

public:
	SRPSCommand(CComReadWrite* aComReadWrite);
	~SRPSCommand();
	void DoIt();
	void GetReply();
	TDesC8& GetData();

	void SetPMID(TPtr8);
	void SetSMID(TPtr8);
	void SetPDUType(TUint8);
	void SetCmdId(TUint16 a_CmdId);
	void SetDPID(TUint16 a_DPID);
	void SetSPID(TUint16 a_SPID);
	void SetUserData(TDes8& a_userDataPtr);

	void Create();
};

#endif // SRPSCOMMAND_H
