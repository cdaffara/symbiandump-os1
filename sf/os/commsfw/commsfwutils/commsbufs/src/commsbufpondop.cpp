// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "es_commsbuf_internal.h"
#include <comms-infras/commsbufpondop.h>
#include "commsbufpondintf.h"
#include "systemsharedbufs.h"

EXPORT_C RCommsBufPondOp::RCommsBufPondOp()
	{
	}

/**
Opens the CommsBuf pond with the supplied pool information. An array of pools 
will be created. The pool information will be typically read from c32start.ini

@aPoolInfo	An array of pool creation information 

@return KErrNoMemory on failure otherwise KErrNone
*/
EXPORT_C TInt RCommsBufPondOp::Open(RArray<TCommsBufPoolCreateInfo>& aPoolInfo, TInt aMaxMBufSize /* =0 */)
	{
	// Do not allow multiple opens.
	__ASSERT_ALWAYS(Dll::Tls() == NULL, CommsBuf::Panic(EMBuf_AlreadyInit));  // Probably this panic code is not good?
	
	// Check whether we do have a mbuf mgr implemenation (leagcy)
	_LIT ( KMBufMgrImplDllName, "mbufmgrimpl.dll");		
	RLibrary lib;
	if (lib.Load(KMBufMgrImplDllName) == KErrNone)
		{
		typedef MCommsBufPondIntf*(* TMBufMgrNew)(RArray <TCommsBufPoolCreateInfo>& aPoolInfo, RLibrary& aLibrary, TInt aMaxMBufSize);

		// ok We do have the library. We will get the first function ordinal via 
		// lookup and initialize the MBufmgr.			
		TMBufMgrNew libFn = (TMBufMgrNew)lib.Lookup(1); // We have only one export in the MBufmgr which CMBufManager::New
		iPondImpl = libFn(aPoolInfo, lib, aMaxMBufSize);		
		}
	else
		{
		iPondImpl = CSystemSharedBufPond::New(aPoolInfo);
		}
	if (iPondImpl == NULL )
		return KErrNoMemory;
	
	TCommsBufPondTLSOp tlsOp(*this);
	tlsOp.Set();
	return KErrNone;
	}

EXPORT_C void RCommsBufPondOp::Close()
	{	
	RLibrary lib;
	if(iPondImpl)
		iPondImpl->Release(lib);
	Dll::SetTls(NULL);
	iPondImpl = NULL;	
	lib.Close();
	}


EXPORT_C void TCommsBufPondTLSOp::Set()
	{
	Dll::SetTls(iPond.iPondImpl);
	iPond.iPondImpl->SetContext();				
	}

EXPORT_C RCommsBufPond TCommsBufPondTLSOp::Get()
	{
	MCommsBufPondIntf* impl = STATIC_CAST(MCommsBufPondIntf*, Dll::Tls());
	return RCommsBufPond(impl);		
	}

