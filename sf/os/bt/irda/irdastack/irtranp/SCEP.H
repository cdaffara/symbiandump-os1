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
 
#ifndef SCEP_H
#define SCEP_H

#include <e32cons.h>

class AbstractCommand;
class CommandProcessor;
class CComReadWrite;
class MComReadWriteObserver;
class BFTP;
class IrTranpUtil;
class MTranpNotification;
class CTranpProtocol;
/*
 * Name:		SCEP
 *
 * Description:	SCEP layer in IrTran-P, aswell as the main interface to the application together with bFTP
 *
 */

NONSHARABLE_CLASS(SCEP) : public CBase,
			              public MComReadWriteObserver
	{
public:
	enum TState { EConnected, EDisconnected, EProgress, EWaiting, EError, ECommunicationInt };
public:
	~SCEP();

	static SCEP* NewL(CTranpProtocol* aTranpProtocol, BFTP* a_oBFTP, const MTranpNotification* aNotifier);
	void ConstructL(CTranpProtocol* aTranpProtocol, BFTP* a_oBFTP, const MTranpNotification* aNotifier);
	void Abort();

	TInt Pump();
	void SCONreqL();
	void SCONconL();
	void SCEPSendCommand(TDes8& a_userData, TUint8 a_pduType);

	void SetPMID(const TDesC8& aPMID);
	void SetSMID(const TDesC8& aSMID);
	void SetPDU(TUint8);

	void ReceiveComplete(TDesC8& aBuffer); // Virtuial from MEtanolObserver
	void SendComplete();
	void SendError(TInt aError);
	void ReceiveError(TInt aError);
	void Error(TInt aError);

private:
	SCEP();

	void ParseCommand(TDes8& a_TempBuffer);
	void SCEPConRequest(TDes8& a_Header, TInt offset);
	void SCEPConConfirm(TDes8& a_Header, TInt offset);
	void SCEPDataCommand(TDes8& a_Header, TInt a_offset);
	void NegInf(TDes8& a_Header, TInt a_offset);
	void DoCommand(const TDesC8& a_Header, TInt a_offset);
	void AddSegment(TDes8& a_Header, TInt a_offset);
	void SCEPDisconnect(TDes8& a_Header, TInt offset);

	// Pointer to the CommandProcessor
	CommandProcessor* m_oCommandP;
	// Pointer to the CComReadWrite communications layer
	CComReadWrite* iComReadWrite;
	// Buff to hold incoming data
	TBuf8<8192> iSCEPFrame;
	// Pointer to the bFTP layer
	BFTP* m_oBFTP;
	CTranpProtocol* iTranpProtocol;
	MTranpNotification* iNotifier;

	// Pointer to the HBufC used for reassembling incoming packets
	HBufC8* m_packet;
	// Used for counting the segment sequence
	TUint32 m_seq;

	// Primary Machine ID
	TBufC8<8> m_PMID;
	// Secondary Machine ID
	TBufC8<8> m_SMID;
	// Maximum size of the sender's PDU 
	TUint8 m_uiMaxPDU;
	// Destination process ID
	TUint16 m_DPID;
	// Source process ID
	TUint16 m_SPID;
	// Command ID
	TUint16 m_CmdId;

	// Product information string
	TBufC8<255> m_szPIS;
	// User name
	TBufC8<255> m_szUserName;
	// Password
	TBufC8<255> m_szPassword;
	// Contains the size of the picture being transferred
	TInt m_iPicSize;

	TState iState;
	};

#endif //SCEP_H
