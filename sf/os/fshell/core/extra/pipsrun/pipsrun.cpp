// pipsrun.cpp
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
#include <fshell/ltkutils.h>
#include <stdapis/unistd.h>
#include <stdapis/stdio.h>
#include <stdapis/pthread.h>

const TInt KBlockSize = 512;

using namespace IoUtils;

class CCmdPipsRun : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPipsRun();
private:
	CCmdPipsRun();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	};


CCommandBase* CCmdPipsRun::NewLC()
	{
	CCmdPipsRun* self = new(ELeave) CCmdPipsRun();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPipsRun::~CCmdPipsRun()
	{
	}

CCmdPipsRun::CCmdPipsRun() : CCommandBase(ESharableIoSession)
	{
	}

const TDesC& CCmdPipsRun::Name() const
	{
	_LIT(KName, "pipsrun");	
	return KName;
	}

const TDesC& CCmdPipsRun::Description() const
	{
	_LIT(KDescription, "Internal helper executable for launching processes that use PIPS in such a way that they can interact with fshell's I/O primitives. You should not call it directly.");
	return KDescription;
	}

const TUint8* GetDesParamLC(TInt aSlot)
	{
	TInt paramLen = User::ParameterLength(aSlot);
	StaticLeaveIfErr(paramLen, _L("Couldn't read environment slot %d"), aSlot);
	HBufC* buf = HBufC::NewLC(paramLen + 1); // + 1 for the terminating null character.
	TPtr bufPtr(buf->Des());
	User::LeaveIfError(User::GetDesParameter(aSlot, bufPtr));
	TPtr8 narrowPtr = buf->Des().Collapse();
	narrowPtr.ZeroTerminate();
	return narrowPtr.Ptr();
	}

void AppendNarrowDes(TUint8*& aTarget, const TDesC& aDes)
	{
	TInt len = aDes.Length();
	const TUint16 *pS = aDes.Ptr();
	const TUint16 *pE = pS + len;
	while (pS < pE)
		{
		TUint c = (*pS++);
		if (c >= 0x100)
			{
			c = 1;
			}
		*aTarget++ = (TUint8)c;
		}
	}

class TGlueParams
	{
public:
	TGlueParams(TInt aFileDescriptor, RIoWriteHandle aWriteHandle);
public:
	TInt iFileDescriptor;
	RIoWriteHandle iWriteHandle;
	};

TGlueParams::TGlueParams(TInt aFileDescriptor, RIoWriteHandle aWriteHandle)
	: iFileDescriptor(aFileDescriptor), iWriteHandle(aWriteHandle)
	{
	}

void* GlueFileDescriptor(void* aParams)
	{
	TGlueParams* params = (TGlueParams*)aParams;
	TBuf8<KBlockSize * 2> buf;
	ssize_t s;
	TInt err = KErrNone;
	do
		{
		s = read((int)params->iFileDescriptor, (char*)buf.Ptr(), KBlockSize);
		if (s > 0)
			{
			buf.SetLength(s);
			err = params->iWriteHandle.Write(buf.Expand());
			}
		}
		while ((s > 0) && (err == KErrNone));
	return (void*)err;
	}

void CCmdPipsRun::DoRunL()
	{
	// Pack the envronment object we inherited from fshell in the way POSIX expects.
	_LIT(KEquals, "=");
	_LIT(KNull, "\x0");
	const CEnvironment& env = Env();
	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	env.GetKeysL(keys);
	const TInt numVars = keys.Count();
	TUint8** envp = (TUint8**)User::AllocLC((numVars  + 1) * sizeof(TInt*)); // + 1 because the last pointer must be NULL.
	for (TInt i = 0; i < numVars; ++i)
		{
		const TDesC& key = *keys[i];
		const TDesC& val = env.GetAsDesL(key);
		const TInt length = (key.Length() + 2 + val.Length()); // 2 for the delimiting '=' and the terminating null.
		TUint8* p = (TUint8*)User::AllocLC(length);
		envp[i] = p;
		AppendNarrowDes(p, key);
		AppendNarrowDes(p, KEquals);
		AppendNarrowDes(p, val);
		AppendNarrowDes(p, KNull);
		}
	envp[numVars] = NULL;

	const TUint8* commandName(GetDesParamLC(IoUtils::KPipsCommandNameProcessSlot));
	const TUint8* commandLine(GetDesParamLC(IoUtils::KPipsCommandLineProcessSlot));
	TInt fds[3];
	TInt pid = popen3((const char*)commandName, (const char*)commandLine, (char**)envp, fds);
	if (pid == -1)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2 + numVars + 1);
	CleanupStack::PopAndDestroy(&keys);

	RProcess childProcess;
	User::LeaveIfError(childProcess.Open(pid));
	CleanupClosePushL(childProcess);
	TRequestStatus childProcessStatus;
	childProcess.Logon(childProcessStatus);
	RProcess::Rendezvous(pid);

	pthread_t stdoutReaderThreadId = 0;
	pthread_t stderrReaderThreadId = 0;
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
	TGlueParams stdoutParams(fds[1], Stdout());
	TGlueParams stderrParams(fds[2], Stderr());
	TInt err = pthread_create(&stdoutReaderThreadId, &threadAttr, GlueFileDescriptor, (void*)&stdoutParams);
	if (err == 0)
		{
		err = pthread_create(&stderrReaderThreadId, &threadAttr, GlueFileDescriptor, (void*)&stderrParams);
		}
	if (err != 0)
		{
		User::Leave(err);
		}

	Stdin().SetReadMode(RIoReadHandle::EOneOrMore);
	TBuf<KBlockSize> buf;
	TRequestStatus stdinReadStatus;
	TBool finished(EFalse);
	while (!finished)
		{
		Stdin().Read(buf, stdinReadStatus);
		User::WaitForRequest(childProcessStatus, stdinReadStatus);
		if (childProcessStatus == KRequestPending)
			{
			TPtr8 narrowPtr(buf.Collapse());
			narrowPtr.ZeroTerminate();			
			do
				{
				TInt ret = write(fds[0], narrowPtr.Ptr(), narrowPtr.Length());
				if (ret < 0)
					{
					finished = ETrue;
					}
				else
					{
					narrowPtr.Set(narrowPtr.MidTPtr(ret));
					}
				}
				while (!finished && narrowPtr.Length());
			}
		else
			{
			finished = ETrue;
			Stdin().ReadCancel();
			User::WaitForRequest(stdinReadStatus);
			}
		}

	CleanupStack::PopAndDestroy(&childProcess);
	pthread_join(stdoutReaderThreadId, NULL);
	pthread_join(stderrReaderThreadId, NULL);
	Complete(childProcessStatus.Int());
	}

EXE_BOILER_PLATE(CCmdPipsRun)

