// xmodem.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <fshell/ioutils.h>

using namespace IoUtils;


class CCmdXmodem : public CCommandBase
	{
public:
	enum TSyncResult
		{
		ENormal,
		EEot
		};
public:
	static CCommandBase* NewLC();
	~CCmdXmodem();
protected:
	CCmdXmodem();
	void ConstructL();
	void SendStdinL();
	void SendFileL(const TDesC& aFileName);
	void ReceiveToStdoutL();
	void ReceiveToFileL(const TDesC& aFileName);
	void ReceiveToNullL();
	void SendBlockL(const TDesC& aBlock);
	TPtrC ReceiveBlockL(TBool aIsFirstBlock, TBool& aIsFinalBlock);
	void DoReceiveBlockL();
	void SendTerminateL();
	void WaitForSyncL();
	TSyncResult SendSyncL();
	TUint8 ReceiveByteL(TInt aTimeout, TBool* aTimeoutOccurred = NULL);
	TUint16 ReceiveShortL(TInt aTimeout, TBool* aTimeoutOccurred = NULL);
	void ReceiveWithTimeoutL(TDes& aBuf, TInt aTimeout, TBool* aTimeoutOccurred = NULL);
	void DoReceiveWithTimeoutL(TDes& aBuf, TInt aTimeout, TBool* aTimeoutOccurred = NULL);
	void PurgeInputL();
	void SendL(const TDesC& aData);
	TInt CheckSize() const;
	TInt ProtocolOverhead() const;
	TBool CheckBlock() const;
	void Progress(TRefByValue<const TDesC> aFmt, ...) const;
	void Dump(const TDesC& aData);
	void Abort();
	void HandleTelnetCommandL();
	void Unget(TChar aChar);
	void PrepareConsoleToTransferL();
	void CleanupClonsoleAfterTransferL();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void HandleLeave(TInt aError);
protected:
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TTelnetMode
		{
		ESendBinary = 0x00000001,
		ERecvBinary = 0x00000002
		};
protected:
	enum 
		{
		ESend, EReceive
		} iMode;
	TFileName2 iFileName;
	TInt iBlockSize;
	TUint8 iPacketNumber;
	TBool iOverwrite;
private:
	TBool iVerbose;
	TBool iCrc;
	TBool iStarted;
	TUint iTelnetMode;
	HBufC* iBuf;
	TBuf<2> iUngetBuf;
	RTimer iTimer;
	};

#endif // __XMODEM_H__
