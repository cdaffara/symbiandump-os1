// CloggerClient.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "clogger.h"
#include "cliserv.h"
#include "SensibleClient_Internals.h"
#include "common.h"
#include <badesca.h>
#include <s32mem.h>
#include <e32property.h>

#define KFlagsMask 0x0FFFFFFF
#define KTestTagEnabled 0x10000000
#define KNoLogMask 0x80000000 // This is the log mask used internally if you call an overload which doesn't specify one

#define iBody (*reinterpret_cast<RCloggerBody*>(&iPimpl))

#ifndef PERFORMANCE_CRITICAL

// declares RClogger* clogger
#define CheckConnected()                                       \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	GetTls(clogger, ETrue);                                    \
	if (!clogger) return

#define GetTlsOrConnect(aTag)                                  \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	TInt err;                                                  \
	TBool isHandle = GetTls(clogger, ETrue, &aTag, &err)

#define GetTlsOrNull()                                         \
	RClogger _cloggerHandle;                                   \
	RClogger* clogger = &_cloggerHandle;                       \
	TBool isHandle = GetTls(clogger, EFalse)

TBool RClogger::GetTls(RClogger*& aClogger, TBool aAutoConnect, const TDesC* aTag, TInt* aError)
	{
	TAny* tls = Dll::Tls();
	if ((TUint)tls & 1)
		{
		// Then it's a handle
		aClogger->SetHandle((TInt)((TUint)tls >> 1));
		return ETrue;
		}
	else if (tls)
		{
		// Then it's a pointer
		aClogger = (RClogger*)tls;
		return EFalse;
		}
	else if (aAutoConnect)
		{
		TInt err = aClogger->Connect(aTag ? *aTag : KNullDesC);
		aClogger->iFlags &= ~EUseHeapBuffer; // Don't (by default) use a buffer for static connections because we can't track it without upgrading TLS
		if (err)
			{
			if (aError) *aError = err;
			aClogger = NULL;
			return ETrue;
			}
		__ASSERT_DEBUG((aClogger->Handle() & 0x80000000) == 0, User::Panic(_L("HandleTooBig"), 0));

		err = Dll::SetTls((TAny*) ((aClogger->Handle() << 1) | 1));
		if (err)
			{
			aClogger->Close();
			if (aError) *aError = err;
			aClogger = NULL;
			}
		return ETrue;
		}
	else
		{
		aClogger = NULL;
		return ETrue;
		}
	}

#endif // PERFORMANCE_CRITICAL

NONSHARABLE_CLASS(RCloggerBody) : public RSensibleSessionBody
	{
public:
	RCloggerBody(RClogger& aSession);
	TInt DoCreateSession(const TDesC &aServer, const TVersion &aVersion, TInt aAsyncMessageSlots);
	TInt DoSendReceive(TInt aFunction, const TIpcArgs &aArgs) const;
	void DoSendReceive(TInt aFunction, const TIpcArgs &aArgs, TRequestStatus& aStatus) const;
	void ServerDiedL();
	void DispatchCallbackL(TServerCallback& aCallback, TCallbackParser& aParser);

private:
	friend class RClogger;
	//RClogger& iSession; // No need to store this, we can calculate it from our this pointer since we know we are always embedded in an RClogger
	};

#define iSession (*(RClogger*)((TUint8*)this - _FOFF(RClogger, iPimpl)))

RCloggerBody::RCloggerBody(RClogger& /*aSession*/)
	: RSensibleSessionBody() //, iSession(aSession)
	{}

TInt RCloggerBody::DoCreateSession(const TDesC &aServer, const TVersion &aVersion, TInt aAsyncMessageSlots)
	{
	return iSession.CreateSession(aServer, aVersion, aAsyncMessageSlots);
	}

TInt RCloggerBody::DoSendReceive(TInt aFunction, const TIpcArgs &aArgs) const
	{
	return iSession.SendReceive(aFunction, aArgs);
	}

void RCloggerBody::DoSendReceive(TInt aFunction, const TIpcArgs &aArgs, TRequestStatus& aStatus) const
	{
	iSession.SendReceive(aFunction, aArgs, aStatus);
	}

EXPORT_C RClogger::RClogger()
	: iFlags(0), iEnabled(EAllEnabled), iBuf(), iSequence(0)
	{
	__ASSERT_COMPILE(sizeof(iPimpl) >= sizeof(RCloggerBody));
	__ASSERT_COMPILE(sizeof(RClogger) == 10*4); // This is the size we've published, it CANNOT change

	// Initialise the body using placement new
	new(iPimpl) RCloggerBody(*this);
	}

EXPORT_C RClogger::RClogger(TAny* aTls)
	: iFlags(0), iEnabled(EAllEnabled), iBuf(), iSequence(0)
	{
	// This overload is only ever supposed to be used by the TlsLog inline functions, hence why it's private but still
	// exported.
	new(iPimpl) RCloggerBody(*this);
	SetHandle(reinterpret_cast<TInt>(aTls));
	}

EXPORT_C TInt RClogger::Connect()
	{
	return Connect(KNullDesC);
	}

EXPORT_C TInt RClogger::Connect(const TDesC& aTag)
	{
	iFlags |= EUseHeapBuffer; // Set by default

	TInt err = iBody.Connect(1, EFalse); // Only need 1 slot as we only have one async function (which is hidden within RSensibleSession)
	if (!err)
		{
		err = ShareAuto();
		}
	if (!err)
		{
		TPckg<TUint32> enabledPkg(iEnabled);
		TInt res = SendReceive(ESetTag, TIpcArgs(&aTag, NULL, &enabledPkg));
		if (res >= 0)
			{
			iSequence = res;
			}
		else
			{
			err = res;
			}
		}
	if (err)
		{
		Close();
		}
	return err;
	}

EXPORT_C TInt RClogger::StaticConnect()
	{
	return StaticConnect(KNullDesC);
	}

#ifndef PERFORMANCE_CRITICAL

EXPORT_C TInt RClogger::StaticConnect(const TDesC& aTag)
	{
	GetTlsOrConnect(aTag);
	if (clogger) return KErrNone;
	else return err;
	}

EXPORT_C void RClogger::StaticClose()
	{
	GetTlsOrNull();
	if (clogger)
		{
		clogger->Close();
		if (!isHandle)
			{
			delete clogger;
			}
		Dll::SetTls(NULL);
		Dll::FreeTls();
		}
	}

#endif // PERFORMANCE_CRITICAL


EXPORT_C void RClogger::Log(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Log(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Log(TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Log(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Log(const char* aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Log(TUint32 aLogMask, const char* aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::LogList(TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	LogList(KNoLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::LogList(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	if (!Enabled(aLogMask)) return;
	TUint32 tickCount = User::NTickCount();

	TPtrC fmt(aFmt);
	if (iFlags & ETrimTrailingNewlines)
		{
		TInt len = fmt.Length();
		if (len && fmt[len-1] == '\n') fmt.Set(fmt.Left(len-1));
		len = fmt.Length();
		if (len && fmt[len-1] == '\r') fmt.Set(fmt.Left(len-1));
		}
	#define SF_FORMAT fmt
	#define SF_LIST aList
	#define SF_BUF iBuf
	#define SF_ACTION(buf) SendReceive(ELog8, TIpcArgs(&buf, tickCount, aLogMask))
	#define SF_WIDE
	#define SF_STACKONLY (!(iFlags & EUseHeapBuffer))
	#include "SensibleFormat.h"

	//if (!(iFlags&ECacheClientBuffers)) iBuf.Close();
	}

EXPORT_C void RClogger::LogList(TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	LogList(KNoLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::LogList(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	if (!Enabled(aLogMask)) return;
	TUint32 tickCount = User::NTickCount();

	TPtrC8 fmt(aFmt);
	if (iFlags & ETrimTrailingNewlines)
		{
		TInt len = fmt.Length();
		if (len && fmt[len-1] == '\n') fmt.Set(fmt.Left(len-1));
		len = fmt.Length();
		if (len && fmt[len-1] == '\r') fmt.Set(fmt.Left(len-1));
		}
	#define SF_FORMAT fmt
	#define SF_LIST aList
	#define SF_BUF iBuf
	#define SF_ACTION(buf) SendReceive(ELog8, TIpcArgs(&buf, tickCount, aLogMask))
	#define SF_STACKONLY (!(iFlags & EUseHeapBuffer))
	#include "SensibleFormat.h"

	//if (!(iFlags&ECacheClientBuffers)) iBuf.Close();
	}

EXPORT_C void RClogger::LogList(const char* aFmt, VA_LIST aList)
	{
	TPtrC8 ptr((const TUint8*)aFmt);
	LogList(KNoLogMask, ptr, aList);
	}

EXPORT_C void RClogger::LogList(TUint32 aLogMask, const char* aFmt, VA_LIST aList)
	{
	TPtrC8 ptr((const TUint8*)aFmt);
	LogList(aLogMask, ptr, aList);
	}

#ifndef PERFORMANCE_CRITICAL

EXPORT_C void RClogger::Slog(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Slog(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Slog(TUint32 aLogMask, const char* aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(aLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Slog(TRefByValue<const TDesC> aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Slog(TRefByValue<const TDesC8> aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::Slog(const char* aFmt, ...)
	{
	CheckConnected();
	VA_LIST args;
	VA_START(args, aFmt);
	clogger->LogList(KNoLogMask, aFmt, args);
	VA_END(args);
	}

EXPORT_C void RClogger::SlogList(TUint32 aLogMask, TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(aLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(TUint32 aLogMask, TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(aLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(TUint32 aLogMask, const char* aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(aLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(TRefByValue<const TDesC> aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(KNoLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(TRefByValue<const TDesC8> aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(KNoLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SlogList(const char* aFmt, VA_LIST aList)
	{
	CheckConnected();
	clogger->LogList(KNoLogMask, aFmt, aList);
	}

EXPORT_C void RClogger::SetStaticLogBehaviour(TUint aLogBehaviour)
	{
	GetTlsOrConnect(KNullDesC);
	if (isHandle && (aLogBehaviour != 0))
		{
		// Then we need to upgrade from handle to pointer
		TInt err = KErrNoMemory;
		RClogger* ptr = new RClogger();
		if (ptr)
			{
			ptr->SetHandle(clogger->Handle());
			err = Dll::SetTls(ptr);
			if (err) delete ptr;
			}
		if (err)
			{
			// Then log it.
			_LIT(KErr, "RClogger couldn't upgrade handle to pointer, err %i");
			clogger->Log(KErr(), err);

			aLogBehaviour = 0; // Don't update flags if we failed to honour them
			}
		else
			{
			clogger = ptr;
			}
		}

	clogger->SetLogBehaviour(aLogBehaviour);
	}

#endif // PERFORMANCE_CRITICAL

EXPORT_C void RClogger::SetLogBehaviour(TUint aLogBehaviour)
	{
	iFlags = (iFlags & ~KFlagsMask) | (aLogBehaviour & KFlagsMask);

	if (!(iFlags & EUseHeapBuffer)) iBuf.Close();
	
	/* This is not needed any more
	if (iFlags & EMonitorTagState)
		{
		iBody.StartCallbackDispatcher();
		}
	else
		{
		iBody.StopCallbackDispatcher();
		}
	*/
	}

EXPORT_C TInt RClogger::SetEnabled(const TDesC& aTag, TUint32 aEnabledMask)
	{
	return SendReceive(ESetEnabled, TIpcArgs(&aTag, aEnabledMask));
	}

EXPORT_C TUint32 RClogger::IsEnabled(const TDesC& aTag)
	{
	TPckgBuf<TUint32> logMask;
	TInt res = SendReceive(EIsEnabled, TIpcArgs(&aTag, &logMask));
	return res < 0 ? EFalse : logMask();
	}

/*
EXPORT_C void RClogger::TailLogFile(TInt aNumberOfChars)
	{
	SendReceive(ETailLogToRDebug, TIpcArgs(aNumberOfChars));
	}

EXPORT_C void RClogger::TailLogFile(TDes8& aBuf)
	{
	SendReceive(ETailLogToBuf, TIpcArgs(&aBuf));
	}
*/

EXPORT_C void RClogger::SetGlobalOptions(TUint aGlobalOptions)
	{
	SendReceive(ESetGlobalOptions, TIpcArgs(aGlobalOptions));
	}

EXPORT_C TUint RClogger::GetGlobalOptions()
	{
	return SendReceive(EGetGlobalOptions, TIpcArgs());
	}

EXPORT_C TInt RClogger::GetRamBufferSize(TInt* aNumberOfBuffers)
	{
	TBuf8<8> buf;
	buf.SetLength(8);
	SendReceive(EGetRamBufferSize, TIpcArgs(&buf));
	TInt* ptr = (TInt*)buf.Ptr();
	TInt size = ptr[0];
	if (aNumberOfBuffers)
		{
		*aNumberOfBuffers = ptr[1];
		}
	return size;
	}

EXPORT_C void RClogger::SetRamBufferSize(TInt aSize, TInt aNum)
	{
	SendReceive(ESetRamBufferSize, TIpcArgs(aSize, aNum));
	}

EXPORT_C TInt RClogger::Rotate()
	{
	return SendReceive(ERotate, TIpcArgs());
	}

EXPORT_C TInt RClogger::Rotate(TDes& aFileName)
	{
	aFileName.Zero();
	return SendReceive(ERotate, TIpcArgs(&aFileName));
	}

EXPORT_C void RClogger::SetRotateBehaviour(TInt aNumberOfOldLogsToKeep, TUint aRotateBehaviour)
	{
	SendReceive(ESetRotateBehaviour, TIpcArgs(aNumberOfOldLogsToKeep, aRotateBehaviour));
	}

EXPORT_C TUint RClogger::GetRotateBehaviour(TInt* aNumberOfOldLogsToKeep)
	{
	TBuf8<8> buf;
	buf.SetLength(8);
	SendReceive(EGetRotateBehaviour, TIpcArgs(&buf));
	TInt* ptr = (TInt*)buf.Ptr();
	TUint behav = ptr[0];
	if (aNumberOfOldLogsToKeep)
		{
		*aNumberOfOldLogsToKeep = ptr[1];
		}
	return behav;
	}

EXPORT_C void RClogger::PersistSettings()
	{
	SendReceive(EPersistSettings, TIpcArgs());
	}

EXPORT_C void RClogger::ResetSettings()
	{
	SendReceive(EResetSettings, TIpcArgs());
	}

EXPORT_C void RClogger::Close()
	{
	iBuf.Close();
	iBody.Close();
	RSessionBase::Close();
	}

void RCloggerBody::DispatchCallbackL(TServerCallback& aCallback, TCallbackParser& aParser)
	{
	switch (aCallback.iCode)
		{
		case ETagEnabledChanged:
			{
			iSession.iEnabled = aParser.NextUint();

			/*
			if (iSession.iFlags & KTestTagEnabled)
				{
				iSession.iFlags &= ~KTestTagEnabled;
				CActiveScheduler::Stop();
				}
			*/
			break;
			}
		}
	}

void RCloggerBody::ServerDiedL()
	{
	}

EXPORT_C void RClogger::GetTagStatesL(CDesCArray*& aTagNames, RBuf8& aEnabled)
	{
	aTagNames = NULL;
	aEnabled.Close();

	TServerCallback cb;
	TPckg<TServerCallback> cbPkg(cb);
	User::LeaveIfError(SendReceive(EGetTagStates1, TIpcArgs(&cbPkg)));
	
	RBuf8 context;
	CleanupClosePushL(context);
	if (cb.iContextLength)
		{
		context.CreateL(cb.iContextLength);
		User::LeaveIfError(SendReceive(EGetTagStates2, TIpcArgs(&context)));
		}
	TCallbackParser parser(cb);
	parser.SetContext(context);

	TPtrC8 enabledBuf = parser.NextDesC8();
	TInt numTags = enabledBuf.Length() / 4; // 32 bits per tag

	CDesCArrayFlat* tags = new(ELeave) CDesCArrayFlat(numTags);
	CleanupStack::PushL(tags);
	while (numTags--)
		{
		tags->AppendL(parser.NextDesC());
		}
	
	aEnabled.CreateL(enabledBuf);
	CleanupStack::Pop(tags);
	CleanupStack::PopAndDestroy(&context);
	aTagNames = tags;
	}

EXPORT_C void RClogger::SetTagStatesL(const CDesCArray* aTagNames, const TDesC8& aEnabled)
	{
	__ASSERT_ALWAYS(aTagNames, User::Leave(KErrArgument));
	TInt count = aTagNames->MdcaCount();
	__ASSERT_ALWAYS(count * 4 == aEnabled.Size(), User::Leave(KErrArgument)); // Number of tags must equal the number of 4-byte entries in aEnabled

	CBufFlat* buf = CBufFlat::NewL(4096);
	CleanupStack::PushL(buf);
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);
	stream.WriteInt32L(count);
	//stream.WriteL(aEnabled);
	stream << aEnabled;
	for (TInt i = 0; i < count; i++)
		{
		const TPtrC name = aTagNames->MdcaPoint(i);
		stream << name;
		}
	CleanupStack::PopAndDestroy(&stream); // Calls close, flushes buffer (probably not necessary with a RBufWriteStream)
	TPtrC8 bufPtr = buf->Ptr(0);
	User::LeaveIfError(SendReceive(ESetTagStates, TIpcArgs(&bufPtr)));
	CleanupStack::PopAndDestroy(buf);
	}

EXPORT_C TInt RClogger::Reserved(TInt aCode, TAny*& aArg, TAny* aArg2)
	{
	TInt res = KErrNone;
	if (aCode == 0xF10C10)
		{
		res = iBody.Connect(2, EFalse); // Used by flogger shim to access internals of clogger that would otherwise be hidden to it
		}
	else if (aCode == 0xC0FFEE)
		{
		RChunk*& chunk = reinterpret_cast<RChunk*&>(aArg);
		TInt size = reinterpret_cast<TInt>(aArg2);
		res = SendReceive(ECreatePerformanceLoggingChunk, TIpcArgs(size));
		if (res > 0)
			{
			chunk->SetHandle(res);
			res = KErrNone;
			}
		if (res == KErrNotSupported)
			{
			// Getting the chunk from the server is only possible if the debug router LDD is included, because there's no API
			// to pass a chunk from a server to a client (without doing it kernel side). So if the LDD isn't present, we need
			// to fall back to constructing it on our side and passing it to the server.
			res = chunk->CreateGlobal(KNullDesC, size, size);
			if (res == KErrNone)
				{
				res = SendReceive(ERegisterPerformanceLoggingChunk, TIpcArgs(*chunk));
				}
			}
		}
	else if (aCode == 0x5e55)
		{
		res = SendReceive(EStartSessionWriterServer);
		}
	else
		{
		User::Panic(_L("CloggerClient"), 0);
		}
	return res;
	}

EXPORT_C TInt RClogger::StaticReserved(TInt aCode, TAny*& aArg, TAny* aArg2)
	{
	TInt res = KErrNone;
	if (aCode == 0xD0715)
		{
		RClogger clogger;
		res = clogger.Connect(*reinterpret_cast<const TDesC*>(aArg2));
		if (res == KErrNone)
			{
			aArg = reinterpret_cast<TAny*>(clogger.Handle());
			}
		}
	else
		{
		User::Panic(_L("CloggerClient"), 0);
		}
	return res;
	}

EXPORT_C void RClogger::HexDump(TUint32 aLogMask, const TDesC8& aHeader, const TDesC8& aData)
	{
	if (!(iEnabled & aLogMask)) return;

	SendReceive(EHexDump, TIpcArgs(&aHeader, &aData, User::NTickCount(), aLogMask));
	}

EXPORT_C void RClogger::HexDump(const TDesC8& aHeader, const TDesC8& aData)
	{
	HexDump(KNoLogMask, aHeader, aData);
	}

#ifndef PERFORMANCE_CRITICAL

EXPORT_C void RClogger::StaticHexDump(TUint32 aLogMask, const TDesC8& aHeader, const TDesC8& aData)
	{
	CheckConnected();
	clogger->HexDump(aLogMask, aHeader, aData);
	}

EXPORT_C void RClogger::StaticHexDump(const TDesC8& aHeader, const TDesC8& aData)
	{
	CheckConnected();
	clogger->HexDump(KNoLogMask, aHeader, aData);
	}

#endif // PERFORMANCE_CRITICAL

inline TBool RClogger::Enabled(TUint32 aMask)
	{
	// First check if anything's changed 
	TInt seq = 0;
	TInt err = RProperty::Get(KCloggerUid, ESequenceNumber, seq);
	if (err == KErrNone && seq > iSequence)
		{
		UpdateEnabled(seq);
		}

	if (!(iEnabled & aMask)) return EFalse;

	return ETrue;
	}

void RClogger::UpdateEnabled(TInt aSequence)
	{
	iSequence = aSequence;
	TPckg<TUint32> buf(iEnabled);
	SendReceive(EUpdateEnabledMask, TIpcArgs(&buf));
	iFlags &= ~KTestTagEnabled;
	}

EXPORT_C RCloggerLogConsumer::RCloggerLogConsumer()
	: iResultBufferPtr(NULL, 0)
	{
	}

EXPORT_C TInt RCloggerLogConsumer::Connect()
	{
	RClogger clogger;
	TInt res = clogger.Connect();
	if (res) return res;

	TAny* arg1 = NULL;
	res = clogger.Reserved(0x5E55, arg1, NULL); // This requests clogger to start the session writer server
	clogger.Close();
	if (res != KErrNone && res != KErrAlreadyExists)
		{
		return res;
		}

	res = CreateSession(KSessionLogServer, TVersion(0,0,0), 2);
	if (res) return res;

	res = SendReceive(ERegisterForLogMessages);
	if (res < 0) return res;
	iSharedChunk.SetHandle(res);
	return KErrNone;
	}

EXPORT_C void RCloggerLogConsumer::GetNextLog(TRequestStatus& aStatus, TPtrC8& aResultBuffer)
	{
	// Server will update aResultBuffer to point to the appropriate location in iSharedChunk.
	// It can do this because we pass in effectively TPckg<TPtrC8>(aResultBuffer) and tell the server
	// what the shared chunk's Base address is in our address space. The server then computes what
	// aResultBuffer should look like and writes it back
	iResultBufferPtr.Set((TUint8*)&aResultBuffer, sizeof(TPtrC8), sizeof(TPtrC8));
	SendReceive(EGetNextLog, TIpcArgs(&iResultBufferPtr, iSharedChunk.Base()), aStatus);
	}

EXPORT_C void RCloggerLogConsumer::CancelGetNextLog()
	{
	SendReceive(ECancelGetNextLog);
	}

EXPORT_C void RCloggerLogConsumer::Close()
	{
	iSharedChunk.Close();
	RSessionBase::Close();
	}

EXPORT_C void Clogger_Slog(const char* aFmt, ...)
	{
	VA_LIST list;
	VA_START(list, aFmt);
	TPtrC8 fmt((const TUint8*)aFmt);
	RClogger::SlogList(fmt, list);
	VA_END(list);
	}
