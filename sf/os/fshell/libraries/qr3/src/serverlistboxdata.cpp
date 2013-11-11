// serverlistboxdata.cpp
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
#include "KernLbxModel.h"
#include "Utils.h"
#include <fshell/clogger.h>
#include <fshell/memoryaccess.h>
#include <fshell/qr3dll.h>
#include "QResources3.hrh"

CServerListBoxData::CServerListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CServerListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	TServerKernelInfo& info = *(TServerKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	more.Format(_L("%d sessions"), info.iSessionQ.iCount);
	}

void CServerListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	TServerKernelInfo& info = *(TServerKernelInfo*)iInfo;
	_LIT8(KServerDesc,"Server;FullName;SessionCount;OwnerThreadId");
	_LIT8(KServerFmt,"Server;%S;%i;%u");
	
	if (i == 0) clogger.Log(KServerDesc);
	clogger.Log(KServerFmt, &info.iFullName, info.iSessionQ.iCount, info.iOwningThreadId);
	}

void CServerListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* aTemp)
	{
	TServerKernelInfo& info = *(TServerKernelInfo*)iInfo;
	_LIT(KInfo, "Server info");
	aTitle.Copy(KInfo);

	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	inf.AppendFormat(_L("Server thread: %u"), info.iOwningThreadId);
	RThread owner;
	TInt err = Model().MemAccess().RThreadForceOpen(owner, info.iOwningThreadId);
	if (!err)
		{
		*aTemp = owner.FullName();
		inf.AppendFormat(_L(" (%S)"), aTemp);
		owner.Close();
		}
	inf.AppendFormat(_L("\n%d sessions"), info.iSessionQ.iCount);
	}

TBool CServerListBoxData::SupportsCommand(TInt /*aCommand*/)
	{
	return EFalse;
	}
