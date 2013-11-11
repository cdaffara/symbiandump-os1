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
 
#ifndef BFTP_H
#define BFTP_H

class IrTranpUtil;
class SCEP;
class CTranpProtocol;

/*
 * Name:		BFTP
 *
 * Description:	bFTP - abstraction layer on top of SCEP
 *
 */

NONSHARABLE_CLASS(BFTP) : public CBase
{
public:
	static BFTP* NewL(CTranpProtocol* aTranpProtocol, const MTranpNotification* aNotifier);
	void ConstructL(CTranpProtocol* aTranpProtocol, const MTranpNotification* aNotifier);
	~BFTP();
	
	void ReqPDU(const TDesC8& a_Header, TInt offset);

	// These methods are called from the application
	void Put(TPtr8* a_picture, TPtr a_fileName);
	//TPtr8* Get();
	void /*TPtr8*/ GetFileName();

	// Pure pass-through to SCEP
	TInt Pump(TLatticeSize aLatticeSize);
	void SetPMID(const TDesC8& aPMID);
	void SetSMID(const TDesC8& aSMID);
	void SetPDU(TUint8);

	void Abort();

private:
	BFTP();
	// Pointer to the SCEP layer
	SCEP* m_oSCEP;
	// Buffer to hold the name of the file
	TBuf8<256> m_FileName;
	TBuf8<256> iLongFilename;
	// Pointer to the TPtr which will point JPEG part of the received picture
	TPtr8 iPicture;
	// Pointer to the TPtr which will point JPEG part of the recieved picture
	TPtr8 iUPFPicture;
	TLatticeSize iLatticeSize;	// Size of picture 

	CTranpProtocol* iTranpProtocol;
	};
#endif //BFTP_H
