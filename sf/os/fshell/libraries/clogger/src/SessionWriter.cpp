// SessionWriter.cpp
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

#include "SessionWriter.h"
#include "cliserv.h"
#include "CloggerServer.h"

CSession2* CSessionWriterServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new(ELeave) CSessionWriterSession();
	}

CSessionWriterServer::CSessionWriterServer(CCloggerServer& aCloggerServer)
	: CServer2(CActive::EPriorityHigh, ESharableSessions), iCloggerServer(aCloggerServer)
	{
	}

void CSessionWriterServer::ConstructL()
	{
	StartL(KSessionLogServer);
	}

CSessionWriterServer& CSessionWriterSession::Server()
	{
	return *static_cast<CSessionWriterServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

void CSessionWriterSession::ServiceL(const RMessage2 &aMessage)
	{
	TInt res = KErrNone;
	TBool handled = ETrue;

	switch (aMessage.Function())
		{
		case ERegisterForLogMessages:
			{
			if (iSessionWriter)
				{
				// We're already registered
				res = KErrAlreadyExists;
				break;
				}
			CSessionWriter* writer = Server().CloggerServer().RegisterSessionWithSessionWriter(this);
			if (!writer)
				{
				// Only one session can be registered at once, and some other session is
				res = KErrInUse;
				break;
				}
			iSessionWriter = writer;
			RChunk& bufChunk = Server().CloggerServer().GetBufChunk();
			aMessage.Complete(bufChunk);
			return; // As we've handled it ourself
			}
		case EGetNextLog:
			{
			if (!iSessionWriterMessage.IsNull())
				{
				res = KErrAlreadyExists;
				break;
				}
			if (!iSessionWriter)
				{
				PanicClient(aMessage, ENotRegisteredForLogs);
				return;
				}
			iClientSharedChunkBase = (TAny*)aMessage.Ptr1();
			iSessionWriter->SetEnabled(ETrue);
			iSessionWriterMessage = aMessage;
			if (iWaitingForClient)
				{
				iWaitingForClient = EFalse;
				iSessionWriter->Completed();
				}
			return; // Don't want to complete this yet!
			}
		case ECancelGetNextLog:
			if (iSessionWriter)
				{
				iSessionWriter->SetEnabled(EFalse);
				}
			if (!iSessionWriterMessage.IsNull())
				{
				iSessionWriterMessage.Complete(KErrCancel);
				}
			break;
		default:
			handled = EFalse;
		}

	if (handled)
		{
		aMessage.Complete(res);
		}
	else
		{
		PanicClient(aMessage, EPanicIllegalFunction);
		}
	}

void CSessionWriterSession::WriteBuffer(const TDesC8& aBuf)
	{
	const TUint8* ptr = aBuf.Ptr();
	const TUint8* base = Server().CloggerServer().GetBufChunk().Base();
	const TUint8* clientDataPtr = (const TUint8*)((TInt)iClientSharedChunkBase + (TInt)(ptr - base));
	TPtrC8 clientPtr(clientDataPtr, aBuf.Length());
	TPckg<TPtrC8> ptrBuf(clientPtr);

	iWaitingForClient = ETrue;
	TInt err = iSessionWriterMessage.Write(0, ptrBuf);
	iSessionWriterMessage.Complete(err);
	}

TBool CSessionWriterSession::WaitingForClient()
	{
	return iWaitingForClient;
	}

CSessionWriterSession::~CSessionWriterSession()
	{
	if (iSessionWriter)
		{
		Server().CloggerServer().RegisterSessionWithSessionWriter(NULL);
		}
	}

//////////////////////////////

CSessionWriter::CSessionWriter(CCloggerServer& aServer)
	: iServer(aServer)
	{
	}

void CSessionWriter::WriteBuf(TInt aBuf)
	{
	ASSERT(!IsBusyWriting() && iEnabled && iSession);
		{
		iBuf = aBuf;
		const TDesC8& buf = iServer.GetBuf(aBuf);
		iSession->WriteBuffer(buf);
		}
	}

TBool CSessionWriter::IsBusyWriting()
	{
	return iSession && iSession->WaitingForClient();
	}

CSessionWriter::~CSessionWriter()
	{
	}

void CSessionWriter::CloseWriter()
	{
	delete this;
	}

void CSessionWriter::SetEnabled(TBool aEnabled)
	{
	if (!iSession)
		{
		// If there's no session, we can't enable ourselves even if someone asks us to
		aEnabled = EFalse;
		}
	iEnabled = aEnabled;
	if (IsBusyWriting() && !aEnabled)
		{
		// Need to say we've completed this buffer
		iServer.CompletedWritingBuf(this, iBuf);
		}
	}

void CSessionWriter::Completed()
	{
	iServer.CompletedWritingBuf(this, iBuf);
	}
