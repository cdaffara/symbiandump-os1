// xmodem.cpp
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

#include <fshell/ioutils.h>
#include "xmodem.h"

using namespace IoUtils;

const TInt KMaxRetries = 5;
const TInt KSmallBlockSize = 128;
const TInt KLargeBlockSize = 1024;
const TInt KLongTimeout = 60;
const TInt KMediumTimeout = 10;
const TInt KShortTimeout = 1;
const TUint8 KByteSoh = 0x01;
const TUint8 KByteStx = 0x02;
const TUint8 KByteEot = 0x04;
const TUint8 KByteAck = 0x06;
const TUint8 KByteNak = 0x15;
const TUint8 KByteCan = 0x18;
const TUint8 KByteSub = 0x1a;
const TUint8 KByteTelnetIac = 0xff;
const TUint8 KByteTelnetDo = 0xfd;
const TUint8 KByteTelnetWill = 0xfb;
const TUint8 KByteTelnetBinaryMode = 0x00;
_LIT(KLitEot, "\x04");
_LIT(KLitAck, "\x06");
_LIT(KLitNak, "\x15");
_LIT(KLitCancel, "\x18\x18\x18");
_LIT(KLitC, "C");
_LIT(KLitTelnetDoBinaryMode, "\xff\xfd\x00");
_LIT(KLitTelnetWillBinaryMode, "\xff\xfb\x00");

#ifdef FSHELL_CORE_SUPPORT_XMODEM_CRCNOTAB

TUint16 Crc16(const TDesC& aData)
	{
	TInt crc	= 0;
	const TInt length = aData.Length();
	for (TInt i = 0; i < length; ++i)
		{
		crc = crc ^ ((TInt)(aData[i]&0x00FF) << 8);
		for (TInt j = 0; j < 8; ++j)
			{
			if (crc & 0x8000)
				{
				crc = crc << 1 ^ 0x1021;
				}
			else
				{
				crc = crc << 1;
				}
			}
		}
	return (crc	& 0xFFFF);
	}

#else

static TUint16 KCrc16Tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
TUint16 Crc16(const TDesC& aData)
	{
	TUint16 crc = 0;
	const TInt length = aData.Length();
	for (TInt i = 0; i < length; ++i)
		{
		crc = (crc<<8) ^ KCrc16Tab[((crc>>8) ^ (TUint8)aData[i])&0x00FF];
		}
	return crc;
	}

#endif // FSHELL_CORE_SUPPORT_XMODEM_CRCNOTAB

CCommandBase* CCmdXmodem::NewLC()
	{
	CCmdXmodem* self = new(ELeave) CCmdXmodem();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	self->ConstructL();
	return self;
	}

CCmdXmodem::~CCmdXmodem()
	{
	delete iBuf;
	iTimer.Close();
	}

CCmdXmodem::CCmdXmodem() : iBlockSize(KSmallBlockSize), iPacketNumber(1)
	{
	}

void CCmdXmodem::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	}

TUint8 CCmdXmodem::ReceiveByteL(TInt aTimeout, TBool* aTimeoutOccurred)
	{
	return (TUint8)ReceiveShortL(aTimeout, aTimeoutOccurred);
	}

TUint16 CCmdXmodem::ReceiveShortL(TInt aTimeout, TBool* aTimeoutOccurred)
	{
	TBuf<1> buf;
	ReceiveWithTimeoutL(buf, aTimeout, aTimeoutOccurred);
	if (buf.Length() == 1)
		{
		return buf[0];
		}
	return 0;
	}

void CCmdXmodem::ReceiveWithTimeoutL(TDes& aBuf, TInt aTimeout, TBool* aTimeoutOccurred)
	{
	aBuf.Zero();
	TBool timeOutOccurred(EFalse);

	if (iTelnetMode & ERecvBinary)
		{
		TBool prevReceiveEndedWithIac(EFalse);
		while ((aBuf.Length() < aBuf.MaxLength()) && !timeOutOccurred)
			{
			TPtr ptr(const_cast<TUint16*>(aBuf.Ptr()) + aBuf.Length(), 0, aBuf.MaxLength() - aBuf.Length()); // Create a TPtr over the unused part of aBuf.
			DoReceiveWithTimeoutL(ptr, aTimeout, &timeOutOccurred);
			if (!timeOutOccurred)
				{
				if (prevReceiveEndedWithIac && (ptr.Length() >= 1) && (ptr[0] == KByteTelnetIac))
					{
					ptr.Delete(0, 1);
					}
				prevReceiveEndedWithIac = EFalse;
				TInt numChars = ptr.Length();
				for (TInt i = 0; i < numChars; ++i)
					{
					if (ptr[i] == KByteTelnetIac)
						{
						if ((i < (numChars - 1)) && (ptr[i + 1] == KByteTelnetIac))
							{
							ptr.Delete(i, 1);
							--numChars;
							}
						else if (i == (numChars - 1))
							{
							prevReceiveEndedWithIac = ETrue;
							}
						}
					}
				aBuf.SetLength(aBuf.Length() + ptr.Length());
				}
			}
		}
	else
		{
		DoReceiveWithTimeoutL(aBuf, aTimeout, &timeOutOccurred);
		}

	if (!timeOutOccurred)
		{
		Progress(_L("Received:\r\n"));
		Dump(aBuf);
		}

	if (aTimeoutOccurred)
		{
		*aTimeoutOccurred = timeOutOccurred;
		}
	}

void CCmdXmodem::DoReceiveWithTimeoutL(TDes& aBuf, TInt aTimeout, TBool* aTimeoutOccurred)
	{
	iStarted = ETrue;
	aBuf.Zero();
	if (aBuf.MaxLength() <= iUngetBuf.Length())
		{
		aBuf.Copy(iUngetBuf.Mid(0, aBuf.MaxLength()));
		iUngetBuf.Delete(0, aBuf.MaxLength());
		Progress(_L("Received (from unget buf):\r\n"));
		Dump(aBuf);
		}
	else
		{
		aBuf.Copy(iUngetBuf);
		TPtr ptr(const_cast<TUint16*>(aBuf.Ptr()) + aBuf.Length(), 0, aBuf.MaxLength() - aBuf.Length()); // Create a TPtr over the unused part of aBuf.
		iUngetBuf.Zero();
		TRequestStatus readStatus;
		TRequestStatus timeoutStatus;
		iTimer.After(timeoutStatus, aTimeout * 1000000);
		Stdin().Read(aBuf, readStatus);
		User::WaitForRequest(readStatus, timeoutStatus);
		if (readStatus == KRequestPending)
			{
			Progress(_L("Timeout expired, cancelling read...\r\n"));
			if (aTimeoutOccurred)
				{
				*aTimeoutOccurred = ETrue;
				}
			Stdin().ReadCancel();
			User::WaitForRequest(readStatus);
			return;
			}
		else
			{
			if (aTimeoutOccurred)
				{
				*aTimeoutOccurred = EFalse;
				}
			iTimer.Cancel();
			User::WaitForRequest(timeoutStatus);
			User::LeaveIfError(readStatus.Int());
			aBuf.SetLength(aBuf.Length() + ptr.Length());
			Progress(_L("Received (raw):\r\n"));
			Dump(aBuf);
			return;
			}
		}
	}

void CCmdXmodem::PurgeInputL()
	{
	Progress(_L("Purging input...\r\n"));
	TBool timeoutOccurred;
	do
		{
		ReceiveByteL(KShortTimeout, &timeoutOccurred);
		if (timeoutOccurred)
			{
			Progress(_L("Timed out\r\n"));
			}
		else
			{
			Progress(_L("Received byte\r\n"));
			}
		}
		while (!timeoutOccurred);
	}

void CCmdXmodem::SendL(const TDesC& aData)
	{
	Progress(_L("Sending:\r\n"));
	Dump(aData);
	iStarted = ETrue;

	if (iTelnetMode & ESendBinary)
		{
		const TInt numChars = aData.Length();
		TInt numIacs = 0;
		for (TInt i = 0; i < numChars; ++i)
			{
			if (aData[i] == KByteTelnetIac)
				{
				++numIacs;
				}
			}
		if (numIacs > 0)
			{
			Progress(_L("Escaping %d IACs\r\n"), numIacs);
			HBufC* newBuf = HBufC::NewLC(aData.Length() + numIacs);
			TPtr newBufPtr(newBuf->Des());
			for (TInt i = 0; i < numChars; ++i)
				{
				if (aData[i] == KByteTelnetIac)
					{
					newBufPtr.Append(KByteTelnetIac);
					}
				newBufPtr.Append(aData[i]);
				}
			Dump(*newBuf);
			User::LeaveIfError(Stdout().Write(*newBuf));
			CleanupStack::PopAndDestroy(newBuf);
			}
		else
			{
			User::LeaveIfError(Stdout().Write(aData));
			}
		}
	else
		{
		User::LeaveIfError(Stdout().Write(aData));
		}
	}

TInt CCmdXmodem::CheckSize() const
	{
	if (iCrc)
		{
		return 2;
		}
	else
		{
		return 1;
		}
	}

TInt CCmdXmodem::ProtocolOverhead() const
	{
	return 3 + CheckSize();
	}

TBool CCmdXmodem::CheckBlock() const
	{
	TBool match(EFalse);

	if (iCrc)
		{
		TUint16 crc = Crc16(iBuf->Mid(2, iBlockSize));
		TUint16 receivedCrc = ((*iBuf)[iBlockSize + 2] << 8) + (*iBuf)[iBlockSize + 3];
		match = (crc == receivedCrc);
		if (!match)
			{
			Progress(_L("CRC failed (calculated %u, received %u)\r\n"), crc, receivedCrc);
			}
		}
	else
		{
		TUint8 sum = 0;
		for (TInt i = 2; i < (iBlockSize + 2); ++i)	// Plus 2 because iBuf contains the two packet number bytes.
			{
			sum += (*iBuf)[i];
			}
		TUint8 received = (*iBuf)[iBlockSize + 2];
		match = (sum == received);
		if (!match)
			{
			Progress(_L("CheckSum failed (calculated %u, received %u)\r\n"), sum, received);
			}
		}

	return match;
	}

void CCmdXmodem::Progress(TRefByValue<const TDesC> aFmt, ...) const
	{
	if (iVerbose)
		{
		TOverflowTruncate overflow;
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf<0x100> buf;
		buf.AppendFormatList(aFmt, list, &overflow);
		const_cast<CCmdXmodem*>(this)->Stderr().Write(buf);
		}
	}

void CCmdXmodem::Dump(const TDesC& aData)
	{
	if (iVerbose)
		{
		TBuf<80> out;
		TBuf<16> ascii;
		TInt dataIndex = 0;
		TInt pos = 0;
		do
			{
			out.Zero();
			ascii.Zero();
			out.AppendNumFixedWidthUC(pos, EHex, 8);
			out.Append(_L(": "));
			for (TInt i = 0; i < 16; ++i)
				{
				if (dataIndex < aData.Length())
					{
					TUint8 byte = (TUint8)aData[dataIndex++];
					out.AppendNumFixedWidthUC(byte, EHex, 2);
					out.Append(_L(" "));
					if ((byte < 0x20) || (byte >= 0x7f) || byte == '%')
						{
						byte = '.';
						}
					ascii.Append(TChar(byte));
					++pos;
					}
				else
					{
					out.Append(_L("   "));
					}
				}
			out.Append(ascii);
			out.Append(_L("\r\n"));
			Stderr().Write(out);
			}
			while (dataIndex < aData.Length());
		}
	}

void CCmdXmodem::Abort()
	{
	TRAP_IGNORE(
				SendL(KLitCancel);
				PurgeInputL();
				);
	}

void CCmdXmodem::WaitForSyncL()
	{
	Progress(_L("Waiting for sync...\r\n"));
	for (TInt numRetries = 0; numRetries < KMaxRetries; ++numRetries)
		{
		switch (ReceiveByteL(KLongTimeout))
			{
			case 'C':
				Progress(_L("Receiver supports CRC\r\n"));
				iCrc = ETrue;
				return;
			case KByteNak:
				Progress(_L("Receiver does not support CRC\r\n"));
				iCrc = EFalse;
				return;
			case KByteCan:
				Progress(_L("Receiver cancelled\r\n"));
				SendL(KLitAck);
				User::Leave(KErrCancel);
				break;
			case KByteTelnetIac:
				HandleTelnetCommandL();
				break;
			default:
				break;
			}
		}

	Progress(_L("Failed to sync\r\n"));
	SendL(KLitCancel);
	User::Leave(KErrCancel);
	}

CCmdXmodem::TSyncResult CCmdXmodem::SendSyncL()
	{
	Progress(_L("Sending sync...\r\n"));
	iCrc = ETrue;
	const TDesC* syncLit = &KLitC();
	FOREVER
		{
		TBool skipNextSync(EFalse);
		for (TInt numRetries = 0; numRetries < KMaxRetries; ++numRetries)
			{
			if (skipNextSync)
				{
				skipNextSync = EFalse;
				}
			else
				{
				SendL(*syncLit);
				}
			switch (ReceiveByteL(KMediumTimeout))
				{
				case KByteSoh:
					Progress(_L("Using 128 byte block size\r\n"));
					iBlockSize = KSmallBlockSize;
					return ENormal;
				case KByteStx:
					Progress(_L("Using 1024 byte block size\r\n"));
					iBlockSize = KLargeBlockSize;
					return ENormal;
				case KByteCan:
					Progress(_L("Sender cancelled\r\n"));
					SendL(KLitAck);
					PurgeInputL();
					User::Leave(KErrCancel);
					break;
				case KByteEot:
					Progress(_L("Received EOT\r\n"));
					SendL(KLitAck);
					return EEot;
				case KByteTelnetIac:
					HandleTelnetCommandL();
					--numRetries;
					skipNextSync = ETrue; // This is to avoid HyperTerminal complaining about an "Unexpected response".
					break;
				default:
					break;
				}
			}
		if (syncLit == &KLitC)
			{
			Progress(_L("Sender doesn't support CRC\r\n"));
			syncLit = &KLitNak;
			iCrc = EFalse;
			}
		else
			{
			Progress(_L("Failed to sync\r\n"));
			SendL(KLitCancel);
			User::Leave(KErrCancel);
			}
		}
	}

TBool IsValidTelnetCommand(TChar aChar)
	{
	return (aChar == KByteTelnetDo) || (aChar == KByteTelnetWill);
	}

void CCmdXmodem::HandleTelnetCommandL()
	{
	Progress(_L("Received (what looks like) the start of a Telnet command\r\n"));
	TBuf<1> buf;
	TBool timedOut(EFalse);
	ReceiveWithTimeoutL(buf, KShortTimeout, &timedOut);
	if (!timedOut)
		{
		TChar command = buf[0];
		if (IsValidTelnetCommand(command))
			{
			Progress(_L("Valid telnet command received\r\n"));
			timedOut = EFalse;
			ReceiveWithTimeoutL(buf, KShortTimeout, &timedOut);
			if (timedOut)
				{
				Progress(_L("Timed-out, ungetting last character... (2)\r\n"));
				Unget(command);
				}
			else if (buf[0] == KByteTelnetBinaryMode)
				{
				if (command == KByteTelnetDo)
					{
					iTelnetMode &= (~ESendBinary);
					SendL(KLitTelnetWillBinaryMode);
					iTelnetMode |= ESendBinary;
					Progress(_L("Set ESendBinary\r\n"));
					}
				else if (command == KByteTelnetWill)
					{
					TBool enableSend = iTelnetMode & ESendBinary;
					iTelnetMode &= (~ESendBinary);
					SendL(KLitTelnetDoBinaryMode);
					iTelnetMode |= ERecvBinary;
					Progress(_L("Set ERecvBinary\r\n"));
					if (enableSend)
						{
						iTelnetMode |= ESendBinary;
						}
					}
				else
					{
					ASSERT(EFalse);
					}
				}
			else
				{
				Progress(_L("Unknown command option, ungetting last two characters...\r\n"));
				Unget(command);
				Unget(buf[0]);
				}
			}
		else
			{
			Progress(_L("Timed-out, ungetting last character... (1)\r\n"));
			Unget(command);
			}
		}
	Progress(_L("Telnet command handled - resuming...\r\n"));
	}

void CCmdXmodem::Unget(TChar aChar)
	{
	ASSERT(iUngetBuf.Length() <= 1);
	iUngetBuf.Append(aChar);
	}

void CCmdXmodem::PrepareConsoleToTransferL()
	{
	Write(_L("Please start the file transfer in your terminal...\r\n"));
	User::LeaveIfError(Stdin().CaptureAllKeys());			// To prevent other things (like fshell) interpreting binary data as special key presses (like cntrl-C).
	LeaveIfErr(Stdin().SetMode(RIoReadWriteHandle::EBinary), _L("Unable to set stdin to binary mode"));	// To prevent vt100cons from scanning for ANSI escape sequences. 
	LeaveIfErr(Stdout().SetMode(RIoReadWriteHandle::EBinary), _L("Unable to set stdout to binary mode")); // To tell iosrv to not mess about with line endings.
	}

void CCmdXmodem::CleanupClonsoleAfterTransferL()
	{
	PurgeInputL();
	LeaveIfErr(Stdin().SetMode(RIoReadWriteHandle::EText), _L("Unable to set stdin back to text mode"));
	LeaveIfErr(Stdout().SetMode(RIoReadWriteHandle::EText), _L("Unable to set stdout back to text mode"));
	if (Stdout().AttachedToConsole())
		{
		RIoConsoleWriteHandle stdout = Stdout();
		TPoint pos(stdout.GetCursorPosL());
		stdout.SetCursorPosAbsL(TPoint(0, pos.iY));
		stdout.ClearToEndOfLineL();
		}
	}

void CCmdXmodem::SendBlockL(const TDesC& aBlock)
	{
	Progress(_L("Sending block...\r\n"));
	ASSERT(aBlock.Length() <= iBlockSize);

	if (iBuf == NULL)
		{
		iBuf = HBufC::NewL(iBlockSize + ProtocolOverhead());
		}

	TPtr buf(iBuf->Des());

	FOREVER
		{
		buf.Zero();
		buf.Append((TUint16)KByteSoh);
		buf.Append((TUint16)iPacketNumber);
		buf.Append((TUint16)((TUint8)(~iPacketNumber)));
		buf.Append(aBlock);
		if (aBlock.Length() < iBlockSize)
			{
			// There's not enough data to fill this block, so pad with SUB.
			buf.AppendFill((TUint16)KByteSub, iBlockSize - aBlock.Length());
			}
		if (iCrc)
			{
			TUint16 crc = Crc16(buf.Mid(3, iBlockSize));
			buf.Append((crc >> 8) & 0xFF);
			buf.Append(crc & 0xFF);
			}
		else
			{
			TUint8 sum = 0;
			for (TInt i = 3; i < (iBlockSize + 3); ++i)
				{
				sum += (*iBuf)[i];
				}
			buf.Append((TUint16)sum);
			}

		Progress(_L("Sending block %d\r\n"), iPacketNumber);

		for (TInt numRetries = 0; numRetries < KMaxRetries; ++numRetries)
			{
			SendL(*iBuf);
			TBool timedOut(EFalse);
			TUint8 byte = ReceiveByteL(KLongTimeout, &timedOut);
			if (timedOut)
				{
				User::Leave(KErrTimedOut);
				}
			switch (byte) 
				{
				case KByteAck:
					Progress(_L("Block %d successfully sent\r\n"), iPacketNumber);
					++iPacketNumber;
					return;
				case KByteCan:
					Progress(_L("Receiver cancelled\r\n"));
					SendL(KLitAck);
					PurgeInputL();
					User::Leave(KErrCancel);
					break;
				case KByteNak:
					Progress(_L("Block NAK'd, sending again...\r\n"));
					break;
				default:
					Progress(_L("Unexpected response (0x%x), sending again...\r\n"), byte);
					break;
				}
			}
		}
	}

TPtrC CCmdXmodem::ReceiveBlockL(TBool aIsFirstBlock, TBool& aIsFinalBlock)
	{
	_LIT(KFirst, "first ");
	Progress(_L("Receiving %Sblock...\r\n"), aIsFirstBlock ? &KFirst : &KNullDesC);
	TPtrC ret(NULL, 0);
	TInt repeats = 0;
	aIsFinalBlock = EFalse;

	if (iBuf == NULL)
		{
		iBuf = HBufC::NewL(iBlockSize + ProtocolOverhead());
		}
	if (!aIsFirstBlock)	// The first byte of the first block should have already been read by SendSyncL.
		{
again:
		iBuf->Des().Zero();
		TInt numRetries;
		for (numRetries = 0; numRetries < KMaxRetries; ++numRetries)
			{
			TBool startReceiving(EFalse);
			Progress(_L("Receiving first byte of block...\r\n"));
			TBool timedOut(EFalse);
			TUint8 c = ReceiveByteL(KMediumTimeout, &timedOut);
			if (timedOut)
				{
				Progress(_L("Timed out - sending NAK...\r\n"));
				SendL(KLitNak);
				}
			else
				{
				switch (c)
					{
					case KByteSoh:
						if (iBlockSize != KSmallBlockSize)
							{
							Progress(_L("Block size set to 128 bytes\r\n"));
							iBlockSize = KSmallBlockSize;
							}
						startReceiving = ETrue;
						break;
					case KByteStx:
						if (iBlockSize != KLargeBlockSize)
							{
							Progress(_L("Block size set to 1024 bytes\r\n"));
							iBlockSize = KLargeBlockSize;
							}
						startReceiving = ETrue;
						break;
					case KByteEot:
						Progress(_L("Last block received\r\n"));
						aIsFinalBlock = ETrue;
						SendL(KLitAck);
						return ret;
					case KByteCan:
						Progress(_L("Sender cancelled\r\n"));
						SendL(KLitAck);
						PurgeInputL();
						User::Leave(KErrCancel);
						break;
					default:
						break;
					}

				if (startReceiving)
					{
					break;
					}
				}
			}

		if (numRetries == KMaxRetries)
			{
			Progress(_L("Failed to receive valid block\r\n"));
			User::Leave(KErrCommsLineFail);
			}
		}

	if (iBuf->Des().MaxLength() < (iBlockSize + ProtocolOverhead()))
		{
		Progress(_L("Reallocating buffer from %d to %d bytes\r\n"), iBuf->Des().MaxLength(), iBlockSize + ProtocolOverhead());
		iBuf = iBuf->ReAllocL(iBlockSize + ProtocolOverhead());
		}

	Progress(_L("Receiving remainder of block...\r\n"));
	TPtr ptr(const_cast<TUint16*>(iBuf->Ptr()), 0, iBlockSize + ProtocolOverhead() - 1); // - 1 because the first byte of the header isn't in the buffer. Note, can't use HBufC::Des because the max length of the resulting TPtr could be larger than the amount of data we are expecting.
	TBool timeoutOccurred;
	ReceiveWithTimeoutL(ptr, KLongTimeout, &timeoutOccurred);
	if (timeoutOccurred)
		{
		Progress(_L("Timed out, retrying...\r\n"));
		SendL(KLitNak);
		PurgeInputL();
		goto again;
		}
	iBuf->Des().SetLength(ptr.Length());

	TUint8 b0 = (*iBuf)[0];
	TUint8 b1 = (*iBuf)[1];
	// workaround for a compiler bug: and b0 and ~b1 with 0xff, otherwise some compilers will compare
	// the full 32-bit values, the top 3 bytes of which contain garbage. Observed with WINSW and GCCE
	// compilers.
	if (((b0 & 0xff) == ((~b1) & 0xff)) && ((b0 == iPacketNumber) || (b0 == (iPacketNumber - 1))) && CheckBlock())
		{
		Progress(_L("Block successfully received\r\n"));
		if ((*iBuf)[0] == iPacketNumber)
			{
			Progress(_L("Packet number matched (%d)\r\n"), iPacketNumber);
			ret.Set(iBuf->Mid(2, iBuf->Length() - CheckSize() - 2));
			++iPacketNumber;
			}
		repeats = 0;
		SendL(KLitAck);
		}
	else if (++repeats >= KMaxRetries)
		{
		Progress(_L("Block not successfully received after %d retries, cancelling...\r\n"), KMaxRetries);
		SendL(KLitCancel);
		PurgeInputL();
		User::Leave(KErrCancel);
		}
	else
		{
		Progress(_L("Block not successfully received, retrying...\r\n"));
		SendL(KLitNak);
		goto again;
		}

	return ret;
	}

void CCmdXmodem::SendTerminateL()
	{
	for (TInt numRetries = 0; numRetries < KMaxRetries; ++numRetries)
		{
		SendL(KLitEot);
		if (ReceiveByteL(KMediumTimeout) == KByteAck)
			{
			return;
			}
		}
	PurgeInputL();
	User::Leave(KErrCompletion);
	}

void CCmdXmodem::SendStdinL()
	{
	HBufC* buf = HBufC::NewLC(iBlockSize);
	TPtr bufPtr(buf->Des());

	while (Stdin().Read(bufPtr) == KErrNone)
		{
		SendBlockL(*buf);
		bufPtr.Zero();
		}
	SendTerminateL();

	CleanupStack::PopAndDestroy(buf);
	}

void CCmdXmodem::SendFileL(const TDesC& aFileName)
	{
	Progress(_L("Sending file \"%S\"...\r\n"), &aFileName);
	RFile file;
	User::LeaveIfError(file.Open(FsL(), aFileName, EFileRead));
	CleanupClosePushL(file);

	HBufC8* buf = HBufC8::NewLC(iBlockSize);
	HBufC* wideBuf = HBufC::NewLC(iBlockSize);
	TPtr8 bufPtr(buf->Des());
	while ((file.Read(bufPtr, iBlockSize) == KErrNone) && (buf->Length() > 0))
		{
		wideBuf->Des().Copy(*buf);
		SendBlockL(*wideBuf);
		}
	SendTerminateL();

	CleanupStack::PopAndDestroy(3, &file);
	}

void CCmdXmodem::ReceiveToStdoutL()
	{
	TBool firstBlock(ETrue);
	TBool finalBlock(EFalse);
	while (!finalBlock)
		{
		TPtrC block(ReceiveBlockL(firstBlock, finalBlock));
		User::LeaveIfError(Stdout().Write(block));
		firstBlock = EFalse;
		}
	}

void CCmdXmodem::ReceiveToFileL(const TDesC& aFileName)
	{
	RFile file;
	if (iOverwrite)
		{
		FsL().MkDirAll(aFileName); // Create the directory if it doesn't already exist
		User::LeaveIfError(file.Replace(FsL(), aFileName, EFileWrite));
		}
	else
		{
		User::LeaveIfError(file.Create(FsL(), aFileName, EFileWrite));
		}
	CleanupClosePushL(file);

	HBufC8* narrowBuf = HBufC8::NewLC(iBlockSize);
	TPtr8 narrowBufPtr(narrowBuf->Des());
	TBool firstBlock(ETrue);
	TBool finalBlock(EFalse);
	while (!finalBlock)
		{
		TPtrC block(ReceiveBlockL(firstBlock, finalBlock));
		firstBlock = EFalse;
		if (block.Length() > narrowBufPtr.MaxLength())
			{
			HBufC8* newNarrowBuf = narrowBuf->ReAllocL(iBuf->Length());
			CleanupStack::Pop(narrowBuf);
			narrowBuf = newNarrowBuf;
			CleanupStack::PushL(narrowBuf);
			narrowBufPtr.Set(narrowBuf->Des());
			}
		narrowBufPtr.Copy(block);
		User::LeaveIfError(file.Write(*narrowBuf));
		narrowBufPtr.Zero();
		}

	CleanupStack::PopAndDestroy(2, &file);
	}

void CCmdXmodem::ReceiveToNullL()
	{
	TBool firstBlock(EFalse);
	TBool finalBlock(EFalse);
	while (!finalBlock)
		{
		ReceiveBlockL(firstBlock, finalBlock);
		firstBlock = EFalse;
		}
	}

const TDesC& CCmdXmodem::Name() const
	{
	_LIT(KName, "xmodem");	
	return KName;
	}

void CCmdXmodem::DoRunL()
	{
	PrepareConsoleToTransferL();

	if (iMode == EReceive)
		{
		if (iFileName.Length() > 0)
			{
			LeaveIfFileExists(iFileName);
			}
		TSyncResult syncResult = SendSyncL();
		if (syncResult == ENormal)
			{
			if (iFileName.Length() > 0)
				{
				ReceiveToFileL(iFileName);
				}
			else
				{
				ReceiveToStdoutL();
				}

			CleanupClonsoleAfterTransferL();
			if (iFileName.Length() > 0)
				{
				Printf(_L("Successfully received \"%S\".\r\n"), &iFileName);
				}
			}
		}
	else if (iMode == ESend)
		{
		if (iFileName.Length() > 0)
			{
			LeaveIfFileNotFound(iFileName);
			}
		WaitForSyncL();
		if (iFileName.Length() > 0)
			{
			SendFileL(iFileName);
			}
		else
			{
			SendStdinL();
			}

		CleanupClonsoleAfterTransferL();
		if (iFileName.Length() > 0)
			{
			Printf(_L("Successfully sent \"%S\".\r\n"), &iFileName);
			}
		else
			{
			Printf(_L("Successfully stdin.\r\n"));
			}
		}
	User::LeaveIfError(Stdin().CancelCaptureAllKeys());
	}

void CCmdXmodem::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg1, "mode");
	aArguments.AppendEnumL((TInt&)iMode, KArg1);
	_LIT(KArg2, "file_name");
	aArguments.AppendFileNameL(iFileName, KArg2);
	}

void CCmdXmodem::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptOverwrite, "overwrite");
	aOptions.AppendBoolL(iOverwrite, KOptOverwrite);
	}

void CCmdXmodem::HandleLeave(TInt aError)
	{
	if (iStarted)
		{
		Abort();
		CleanupClonsoleAfterTransferL();
		}
	CCommandBase::HandleLeave(aError);
	}
