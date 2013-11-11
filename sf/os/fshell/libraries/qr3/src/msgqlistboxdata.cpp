// msgqlistboxdata.cpp
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

CMsgQueListBoxData::CMsgQueListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CMsgQueListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	TMsgQueueKernelInfo& info = *(TMsgQueueKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	more.Format(_L("Message max size=%u"), info.iMaxMsgLength);
	}

void CMsgQueListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	TMsgQueueKernelInfo& info = *(TMsgQueueKernelInfo*)iInfo;
	_LIT8(KDesc,"MsgQ;KernObjAddr;FullName;AccessCount;MsgLength");
	_LIT8(KFmt,"MsgQ;%x;%S;%d;%d");
	
	if (i == 0) clogger.Log(KDesc);
	clogger.Log(KFmt, info.iAddressOfKernelObject, &info.iFullName, info.iAccessCount, info.iMaxMsgLength);
	}

void CMsgQueListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*aTemp*/)
	{
	TMsgQueueKernelInfo& info = *(TMsgQueueKernelInfo*)iInfo;
	_LIT(KInfo, "Message queue info");
	aTitle.Copy(KInfo);

	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	inf.AppendFormat(_L("Kernel object address: 0x%08x\n"), info.iAddressOfKernelObject);
	inf.AppendFormat(_L("Message max size: %u\n"), info.iMaxMsgLength);
	inf.AppendFormat(_L("Access count: %u"), info.iAccessCount);
	//inf.AppendFormat(_L("Kernel owner address:  0x%08x\n"), info.iAddressOfKernelOwner);
	}
