// sandbox.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <e32base.h>
#include "sandbox.h"
#include <APMREC.H>
#include <fshell/memoryaccess.h>

enum TMsg
	{
	EGetRecog,
	EGetRecog2,
	ECloseServer,
	EGetVTablePtr,
	EMaxArgs,
	};

#ifndef SERVER


class RSandboxSession : public RSessionBase
	{
public:
	TInt Connect(const TDesC& aServerName)
		{
		return CreateSession(aServerName, TVersion(0,0,0));
		}

	TInt GetRecogniserInfo(TUid aImplementationUid, RArray<TDataType>& aMimeTypes)
		{
		TInt size = SendReceive(EGetRecog, TIpcArgs(aImplementationUid.iUid));
		if (size < 0) return size;
		TInt err = aMimeTypes.Reserve(size/sizeof(TDataType));
		if (err) return err;
		RBuf8 buf;
		err = buf.Create(size);
		if (err) return err;
		err = SendReceive(EGetRecog2, TIpcArgs(&buf));
		if (err) { buf.Close(); return err; }

		const TDataType* ptr = (const TDataType*)buf.Ptr();
		for (TInt i = 0; i < size/sizeof(TDataType); i++)
			{
			err = aMimeTypes.Append(ptr[i]);
			if (err) break;
			}
		buf.Close();
		return err;
		}

	TInt GetVTablePtrFromEcomUid(TUid aUid, TAny*& vtablePtr)
		{
		TPckgBuf<TAny*> pkg(NULL);
		TInt err = SendReceive(EGetVTablePtr, TIpcArgs(aUid.iUid, &pkg));
		if (err) return err;
		vtablePtr = pkg();
		return KErrNone;
		}

	void Byebye()
		{
		SendReceive(ECloseServer);
		}
	};

#define GETSESSIONL(process, session)										\
	RProcess process;														\
	User::LeaveIfError(process.Create(_L("QR3Sandbox.exe"), KNullDesC));	\
	TRequestStatus stat;													\
	process.Rendezvous(stat);												\
	process.Resume();														\
	User::WaitForRequest(stat);												\
																			\
	RSandboxSession session;												\
	User::LeaveIfError(session.Connect(sandbox.FullName()));


void Sandbox::GetRecogniserInfoL(TUid aImplementationUid, RArray<TDataType>& aMimeTypes)
	{
	GETSESSIONL(sandbox, sess);

	User::LeaveIfError(sess.GetRecogniserInfo(aImplementationUid, aMimeTypes));

	sess.Byebye();
	sandbox.Close();
	}

void Sandbox::GetDllNameFromEcomUidL(RMemoryAccess& aMemAccess, TUid aUid, TFileName& aFileName)
	{
	// Try and instanciate object and figure out what DLL it belongs to by looking up its vtable pointer
	// We do the instanciation in a separate process so as to avoid worrying about cleanup or side-effects of instanciation

	GETSESSIONL(sandbox, sess);

	TAny* vtablePtr = NULL;
	TInt err = sess.GetVTablePtrFromEcomUid(aUid, vtablePtr);

	TFullName8 dllName;
	if (!err)
		{
		err = aMemAccess.FindAddressInCodeSegments(dllName, vtablePtr);
		}
	sess.Byebye(); // We have to call FindAddressInCodeSegments before sandbox exits, because when it does the code segment will most likely be unloaded!
	sandbox.Close();
	User::LeaveIfError(err);
	
	aFileName.Copy(dllName);
	}


#else

#include <e32property.h>
#include <ecom/ecom.h>

void GoL();

class CSandboxSession : public CSession2
	{
	void ServiceL(const RMessage2 &aMessage);
	RBuf8 iBuf;
	};

class CSandboxServer : public CServer2
	{
public:
	CSandboxServer() : CServer2(0,ESharableSessions) {}

protected:
	CSession2* NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
		{
		return new(ELeave) CSandboxSession();
		}
	};


TInt E32Main()
	{
	User::SetJustInTime(EFalse); // Don't want to debug problems in the sandbox

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(err, GoL());
	return err;
	}

void GoL()
	{
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(s);
	CSandboxServer* server = new(ELeave) CSandboxServer();
	// Scope the TFullName
		{
		TFullName serverName = RProcess().FullName();
		server->StartL(serverName);
		}
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	}

void CSandboxSession::ServiceL(const RMessage2 &aMessage)
	{
	if (aMessage.Function() >= EMaxArgs)
		{
		aMessage.Complete(KErrArgument);
		return;
		}

	if (aMessage.Function() == EGetRecog2)
		{
		aMessage.WriteL(0, iBuf);
		iBuf.Close();
		aMessage.Complete(KErrNone);
		}
	else if (aMessage.Function() == EGetRecog)
		{
		if (iBuf.MaxLength() == 0)
			{
			iBuf.ReAllocL(1024); // Must be > sizeof(TDataType) otherwise the reallocating logic below is flawed
			}
		TUid uid = TUid::Uid(aMessage.Int0());
		TUid destructorKey;
		CApaDataRecognizerType* rec = static_cast<CApaDataRecognizerType*>(REComSession::CreateImplementationL(uid, destructorKey));
		TInt count = rec->MimeTypesCount();
		for (TInt j = 0; j < count; j++)
			{
			TDataType type = rec->SupportedDataTypeL(j);
			TPckg<TDataType> buf(type);
			if (iBuf.Length() + buf.Length() >= iBuf.MaxLength())
				{
				iBuf.ReAllocL(iBuf.MaxLength() * 2);
				}
			iBuf.Append(buf);
			}

		aMessage.Complete(iBuf.Size());
		}
	else if (aMessage.Function() == ECloseServer)
		{
		aMessage.Complete(KErrNone);
		CActiveScheduler::Stop();
		}
	else if (aMessage.Function() == EGetVTablePtr)
		{
		TUid destructorKey;
		TAny* obj = NULL;
		obj = REComSession::CreateImplementationL(TUid::Uid(aMessage.Int0()), destructorKey);
		TAny* vtablePtr = *((TAny**)obj);	
		TPckg<TAny*> res(vtablePtr);
		aMessage.WriteL(1, res);
		aMessage.Complete(KErrNone);
		}
	else
		{
		ASSERT(0);
		}
	}

#endif
