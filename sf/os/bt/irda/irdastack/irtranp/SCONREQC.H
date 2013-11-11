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
 
#ifndef SCONREQCOMMAND_H
#define SCONREQCOMMAND_H

#include "ABSTRACT.H"
#include <e32base.h>

class IrTranpUtil;

/*
 * Name:		SCONreqCommand
 *
 * Description:	Connection Request command
 *
 */

NONSHARABLE_CLASS(SCONreqCommand) : public AbstractCommand
{
private:
	SCONreqCommand();
	// Buffer to hold this packet
	TBuf8<255> m_oData;
	// Primary Machine ID
	TBufC8<8> m_PMID;
	// Secondary Machine ID
	TBufC8<8> m_SMID;
	// Maximum size of the PDU
	TUint8 m_PDU;

public:
	SCONreqCommand(CComReadWrite* iComReadWrite);
	~SCONreqCommand();
	void DoIt();
	void GetReply();
	TDesC8& GetData();

	void SetPMID(TPtr8);
	void SetSMID(TPtr8);
	void SetPDU(TUint8);

	void Create();
};

#endif // SCONREQCOMMAND_H
