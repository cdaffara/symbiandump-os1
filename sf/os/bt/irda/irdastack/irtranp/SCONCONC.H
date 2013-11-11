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
 
#ifndef SCONCONCOMMAND_H
#define SCONCONCOMMAND_H

class IrTranpUtil;

/*
 * Name:		SCONconCommand
 *
 * Description:	Connection confirmed command
 *
 */

NONSHARABLE_CLASS(SCONconCommand) : public AbstractCommand
{
private:
	SCONconCommand();
	// Buffer to hold this packet
	TBuf8<255> m_oData;
	// Primary Machine ID
	TBufC8<8> m_PMID;
	// Secondary Machine ID
	TBufC8<8> m_SMID;
	// Maximum size of the PDU
	TUint8 m_PDU;

public:
	SCONconCommand(CComReadWrite* aComReadWrite);
	~SCONconCommand();
	void DoIt();
	void GetReply();
	TDesC8& GetData(); // From Abstract Command

	void SetPMID(TPtr8);
	void SetSMID(TPtr8);
	void SetPDU(TUint8);

	void Create();
};

#endif //SCONCONCOMMAND_H
