// remote_console.cpp
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32uid.h>
#include <in_sock.h>
#ifdef USE_RCONNECTION
#include <es_enum.h>
#include <commdbconnpref.h>
#endif
#include <fshell/consoleextensions.h>
#include "remote_console.h"

const TInt KMajorVersion = 1;
const TInt KMinorVersion = 0;

#define DEBUG_LOG(x) if (iUnderlyingConsole && iConfig.Debug()) iUnderlyingConsole->Write(_L(x))
#define DEBUG_LOG_1(x, a) if (iUnderlyingConsole && iConfig.Debug()) iUnderlyingConsole->Printf(_L(x), a)
#define DEBUG_LOG_2(x, a, b) if (iUnderlyingConsole && iConfig.Debug()) iUnderlyingConsole->Printf(_L(x), a, b)
#define DEBUG_LOG_3(x, a, b, c) if (iUnderlyingConsole && iConfig.Debug()) iUnderlyingConsole->Printf(_L(x), a, b, c)
#define CLEANUP_AND_RETURN_IF_ERROR(x) { TInt __err = x; if (__err < 0) { CleanupConnection(); return __err; } }

enum TPacketType
	{
	ECommandCreateConsoleWindow,
	ECommandAttachKeyEventSocket,
	ECommandWrite,
	ECommandGetCursorPos,
	ECommandSetAbsCursorPos,
	ECommandSetRelCursorPos,
	ECommandSetCursorHeight,
	ECommandGetScreenSize,
	ECommandSetTitle,
	ECommandClearScreen,
	ECommandClearToEndOfLine
	};


class THeaderBase
	{
public:
	THeaderBase(TPacketType aCommand, TInt aPacketSize);
private:
	const TInt iPacketType;
	const TInt iPacketSize;
	};

THeaderBase::THeaderBase(TPacketType aCommand, TInt aPacketSize)
	: iPacketType(aCommand), iPacketSize(aPacketSize - sizeof(THeaderBase))
	{
	}

class TCreateConsoleWindowHeader : public THeaderBase
	{
public:
	TCreateConsoleWindowHeader(const TDesC& aTitle, TSize aSize);
private:
	const TInt iMajorVersion;
	const TInt iMinorVersion;
	TInt iWidth;
	TInt iHeight;
	TInt iTitleLength;
	};

TCreateConsoleWindowHeader::TCreateConsoleWindowHeader(const TDesC& aTitle, TSize aSize)
	: THeaderBase(ECommandCreateConsoleWindow, sizeof(TCreateConsoleWindowHeader) + aTitle.Size()), iMajorVersion(KMajorVersion), iMinorVersion(KMinorVersion), iWidth(aSize.iWidth), iHeight(aSize.iHeight), iTitleLength(aTitle.Length())
	{
	}

class TAttachKeyEventSocketHeader : public THeaderBase
	{
public:
	TAttachKeyEventSocketHeader(TInt aWindowId);
private:
	TInt iWindowId;
	};

TAttachKeyEventSocketHeader::TAttachKeyEventSocketHeader(TInt aWindowId)
	: THeaderBase(ECommandAttachKeyEventSocket, sizeof(TAttachKeyEventSocketHeader)), iWindowId(aWindowId)
	{
	}

class TWriteHeader : public THeaderBase
	{
public:
	TWriteHeader(const TDesC& aDes);
private:
	TInt iDesLength;
	};

TWriteHeader::TWriteHeader(const TDesC& aDes)
	: THeaderBase(ECommandWrite, sizeof(TWriteHeader) + aDes.Size()), iDesLength(aDes.Length())
	{
	}

class TWriteOneHeader : public THeaderBase
	{
public:
	TWriteOneHeader(const TDesC& aDes);
private:
	TInt iDesLength;
	TUint16 iChar;
	};

TWriteOneHeader::TWriteOneHeader(const TDesC& aDes)
	: THeaderBase(ECommandWrite, sizeof(TWriteOneHeader)), iDesLength(1), iChar(aDes[0])
	{
	}


class TSetCursorPosHeader : public THeaderBase
	{
public:
	TSetCursorPosHeader(TPacketType aCommand, const TPoint& aPos);
private:
	TInt iX;
	TInt iY;
	};

TSetCursorPosHeader::TSetCursorPosHeader(TPacketType aCommand, const TPoint& aPos)
	: THeaderBase(aCommand, sizeof(TSetCursorPosHeader)), iX(aPos.iX), iY(aPos.iY)
	{
	ASSERT((aCommand == ECommandSetAbsCursorPos) || (aCommand == ECommandSetRelCursorPos));
	}

class TSetCursorHeightHeader : public THeaderBase
	{
public:
	TSetCursorHeightHeader(TInt aHeight);
private:
	TInt iHeight;
	};

TSetCursorHeightHeader::TSetCursorHeightHeader(TInt aHeight)
	: THeaderBase(ECommandSetCursorHeight, sizeof(TSetCursorHeightHeader)), iHeight(aHeight)
	{
	}

class TSetTitleHeader : public THeaderBase
	{
public:
	TSetTitleHeader(const TDesC& aTitle);
	};

TSetTitleHeader::TSetTitleHeader(const TDesC& aTitle)
	: THeaderBase(ECommandSetTitle, sizeof(TSetTitleHeader) + aTitle.Size())
	{
	}


CRemoteConsole::CRemoteConsole()
	: iKeyEventPckg(iKeyEvent)
	{
	}

CRemoteConsole::~CRemoteConsole()
	{
	CleanupConnection();
	delete iUnderlyingConsole;
	}

TInt CRemoteConsole::Create(const TDesC& aTitle, TSize aSize)
	{
	TInt ret = iConfig.Init();
	if (ret == KErrNone)
		{
		ret = Connect(aTitle, aSize);
		}
	if (ret && iUnderlyingConsole)
		{
		iUnderlyingConsole->Printf(_L("Remote connection failed: %d\r\n"), ret);
#ifdef __WINS__
		// Give the user the chance to read the error message.
		User::After(10000000);
#endif
		}
	return ret;
	}

TInt CRemoteConsole::Connect(const TDesC& aTitle, TSize aSize)
	{
	// Connect to Socket Server.
	DEBUG_LOG("Connecting to ESock\r\n");
	TInt err = iSocketSession.Connect();
	CLEANUP_AND_RETURN_IF_ERROR(err);

#ifdef USE_RCONNECTION
	switch (iConfig.ConnectionType())
		{
		default:
		case TRemoteConsoleConfig::EImplicit:
			{
			DEBUG_LOG("Opening command socket\r\n");
			CLEANUP_AND_RETURN_IF_ERROR(iCommandSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol()));
			DEBUG_LOG("Opening event\r\n");
			CLEANUP_AND_RETURN_IF_ERROR(iKeyEventSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol()));
			break;
			}
		case TRemoteConsoleConfig::EExplicit:
			{
			DEBUG_LOG("Opening connection\r\n");
			CLEANUP_AND_RETURN_IF_ERROR(iConnection.Open(iSocketSession));
			TUint numConnections;
			DEBUG_LOG("Enumerating existing connections\r\n");
			err = iConnection.EnumerateConnections(numConnections);
			CLEANUP_AND_RETURN_IF_ERROR(err);
			DEBUG_LOG_2("Looking for IapId: %d NetId: %d\r\n", iConfig.IapId(), iConfig.NetworkId());

			TBool connectionAttached(EFalse);
			for (TInt i = 1; i <= (TInt)numConnections; ++i)
				{
				DEBUG_LOG_1("Getting info for connection %d\r\n", i);
				TConnectionInfoBuf connectionInfoBuf;
				CLEANUP_AND_RETURN_IF_ERROR(iConnection.GetConnectionInfo(i, connectionInfoBuf));
				const TConnectionInfo& info = connectionInfoBuf();
				DEBUG_LOG_2("Found IapId: %d NetId: %d\r\n", info.iIapId, info.iNetId);
				if ((info.iIapId == iConfig.IapId()) && (info.iNetId == iConfig.NetworkId()))
					{
					DEBUG_LOG("Matching IAP and Network IDs found, attaching...\r\n");
					CLEANUP_AND_RETURN_IF_ERROR(iConnection.Attach(connectionInfoBuf, RConnection::EAttachTypeNormal));
					DEBUG_LOG("Opening command socket on connection\r\n");
					CLEANUP_AND_RETURN_IF_ERROR(iCommandSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol(), iConnection));
					DEBUG_LOG("Opening event socket on connection\r\n");
					CLEANUP_AND_RETURN_IF_ERROR(iKeyEventSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol(), iConnection));
					connectionAttached = ETrue;
					}
				}
			if (!connectionAttached)
				{
				CLEANUP_AND_RETURN_IF_ERROR(KErrCouldNotConnect);
				}
			break;
			}
		}
#else	// USE_RCONNECTION
	DEBUG_LOG("Opening command socket\r\n");
	CLEANUP_AND_RETURN_IF_ERROR(iCommandSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol()));
	DEBUG_LOG("Opening event\r\n");
	CLEANUP_AND_RETURN_IF_ERROR(iKeyEventSocket.Open(iSocketSession, KAfInet, KSockStream, iConfig.Protocol()));
#endif

	iCommandSocket.SetOpt(KSoTcpNoDelay, KSolInetTcp, ETrue);	// Ignore error (may not be supported).
	iKeyEventSocket.SetOpt(KSoTcpNoDelay, KSolInetTcp, ETrue);	// Ignore error (may not be supported).

	// Resolve IP address (if need be).
	const TDesC& remoteHostName = iConfig.RemoteHostName();
	TInetAddr remoteAddress(iConfig.RemotePort());
	err = remoteAddress.Input(remoteHostName);
	if (err)
		{
		RHostResolver resolver;
#ifdef USE_RCONNECTION
		if (iConnection.SubSessionHandle())
			{
			CLEANUP_AND_RETURN_IF_ERROR(resolver.Open(iSocketSession, KAfInet, iConfig.Protocol(), iConnection));
			}
		else
			{
			CLEANUP_AND_RETURN_IF_ERROR(resolver.Open(iSocketSession, KAfInet, iConfig.Protocol()));
			}
#else
		CLEANUP_AND_RETURN_IF_ERROR(resolver.Open(iSocketSession, KAfInet, iConfig.Protocol()));
#endif
		TNameEntry nameEntry;
		err = resolver.GetByName(remoteHostName, nameEntry);
		if (err)
			{
			resolver.Close();
			CLEANUP_AND_RETURN_IF_ERROR(err);
			}
		remoteAddress.SetAddress(TInetAddr::Cast(nameEntry().iAddr).Address());
		resolver.Close();
		}

	// Connect command socket.
	DEBUG_LOG_2("Connecting command socket to %S : %d\r\n", &remoteHostName, iConfig.RemotePort());
	TRequestStatus status;
	iCommandSocket.Connect(remoteAddress, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());

	// Request a new console window in the remote host.
	DEBUG_LOG("Requesting new console window\r\n");
	TCreateConsoleWindowHeader createConsoleWindowHeader(aTitle, aSize);
	TPckgC<TCreateConsoleWindowHeader> createConsoleWindowHeaderPckg(createConsoleWindowHeader);
	iCommandSocket.Write(createConsoleWindowHeaderPckg, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());
	DEBUG_LOG("Sending title\r\n");
	TPtrC8 narrowTitlePtr((TUint8*)aTitle.Ptr(), aTitle.Size());
	iCommandSocket.Write(narrowTitlePtr, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());

	// Read response (window identifier if successful).
	DEBUG_LOG("Reading window identifier\r\n");
	TInt response;
	TPckg<TInt> responsePckg(response);
	iCommandSocket.Read(responsePckg, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());
	CLEANUP_AND_RETURN_IF_ERROR(response);

	// Connect key event socket.
	DEBUG_LOG_2("Connecting event socket to %S : %d\r\n", &remoteHostName, iConfig.RemotePort());
	iKeyEventSocket.Connect(remoteAddress, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());

	// Attach key event socket to the new console window.
	DEBUG_LOG("Attaching event socket to console window\r\n");
	TAttachKeyEventSocketHeader attachKeyEventSocketHeader(response);
	TPckgC<TAttachKeyEventSocketHeader> attachKeyEventSocketHeaderPckg(attachKeyEventSocketHeader);
	iKeyEventSocket.Write(attachKeyEventSocketHeaderPckg, status);
	User::WaitForRequest(status);
	CLEANUP_AND_RETURN_IF_ERROR(status.Int());

	return KErrNone;
	}

void CRemoteConsole::Read(TRequestStatus& aStatus)
	{
	iKeyEventSocket.Read(iKeyEventPckg, aStatus);
	}

void CRemoteConsole::ReadCancel()
	{
	iKeyEventSocket.CancelRead();
	}

void CRemoteConsole::Write(const TDesC& aDes)
	{
	if (aDes.Length() == 1)
		{
		// Only do a single ESock write for the case where there's only 1 character.
		TWriteOneHeader writeHeader(aDes);
		TPckgC<TWriteOneHeader> writeHeaderPckg(writeHeader);
		WriteCommand(writeHeaderPckg);
		}
	else
		{
		TWriteHeader writeHeader(aDes);
		TPckgC<TWriteHeader> writeHeaderPckg(writeHeader);
		WriteCommand(writeHeaderPckg);
		TPtrC8 narrowPtr((TUint8*)aDes.Ptr(), aDes.Size());
		WriteCommand(narrowPtr);
		}
	}

TPoint CRemoteConsole::CursorPos() const
	{
	THeaderBase header(ECommandGetCursorPos, sizeof(THeaderBase));
	TPckgC<THeaderBase> headerPckg(header);
	WriteCommand(headerPckg);
	TPoint response;
	TPckg<TPoint> responsePckg(response);
	ReadCommandResponse(responsePckg);
	return response;
	}

void CRemoteConsole::SetCursorPosAbs(const TPoint& aPoint)
	{
	TSetCursorPosHeader header(ECommandSetAbsCursorPos, aPoint);
	TPckgC<TSetCursorPosHeader> headerPckg(header);
	WriteCommand(headerPckg);
	}

void CRemoteConsole::SetCursorPosRel(const TPoint& aPoint)
	{
	TSetCursorPosHeader header(ECommandSetRelCursorPos, aPoint);
	TPckgC<TSetCursorPosHeader> headerPckg(header);
	WriteCommand(headerPckg);
	}

void CRemoteConsole::SetCursorHeight(TInt aPercentage)
	{
	TSetCursorHeightHeader header(aPercentage);
	TPckgC<TSetCursorHeightHeader> headerPckg(header);
	WriteCommand(headerPckg);
	}

void CRemoteConsole::SetTitle(const TDesC& aTitle)
	{
	TSetTitleHeader header(aTitle);
	TPckgC<TSetTitleHeader> headerPckg(header);
	WriteCommand(headerPckg);
	TPtrC8 narrowTitlePtr((TUint8*)aTitle.Ptr(), aTitle.Size());
	WriteCommand(narrowTitlePtr);
	}

void CRemoteConsole::ClearScreen()
	{
	THeaderBase header(ECommandClearScreen, sizeof(THeaderBase));
	TPckgC<THeaderBase> headerPckg(header);
	WriteCommand(headerPckg);
	}

void CRemoteConsole::ClearToEndOfLine()
	{
	THeaderBase header(ECommandClearToEndOfLine, sizeof(THeaderBase));
	TPckgC<THeaderBase> headerPckg(header);
	WriteCommand(headerPckg);
	}

TSize CRemoteConsole::ScreenSize() const
	{
	THeaderBase header(ECommandGetScreenSize, sizeof(THeaderBase));
	TPckgC<THeaderBase> headerPckg(header);
	WriteCommand(headerPckg);
	TSize response;
	TPckg<TSize> responsePckg(response);
	ReadCommandResponse(responsePckg);
	return response;
	}

TKeyCode CRemoteConsole::KeyCode() const
	{
	return (TKeyCode)(iKeyEvent.iChar);
	}

TUint CRemoteConsole::KeyModifiers() const
	{
	return iKeyEvent.iModifiers;
	}

TInt CRemoteConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == UnderlyingConsole::KSetUnderlyingConsoleExtension)
		{
		iUnderlyingConsole = (CConsoleBase*)a1;
		return KErrNone;
		}
	else
		{
		return CConsoleBase::Extension_(aExtensionId, a0, a1);
		}
	}

void CRemoteConsole::CleanupConnection()
	{
	iKeyEventSocket.Close();
	iCommandSocket.Close();
#ifdef USE_RCONNECTION
	iConnection.Close();
#endif
	iSocketSession.Close();
	}

void CRemoteConsole::WriteCommand(const TDesC8& aDes) const
	{
	TRequestStatus status;
	iCommandSocket.Write(aDes, status);
	User::WaitForRequest(status);
	}

void CRemoteConsole::ReadCommandResponse(TDes8& aDes) const
	{
	TRequestStatus status;
	iCommandSocket.Read(aDes, status);
	User::WaitForRequest(status);
	}


EXPORT_C TAny* NewConsole()
	{
	return new CRemoteConsole;
	}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
	{
	return(KErrNone);
	}
#endif
