// semaphorelistboxdata.cpp
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

CSemaphoreListBoxData::CSemaphoreListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CSemaphoreListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	TSemaphoreKernelInfo& info = *(TSemaphoreKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	more.Format(_L("Count=%d"), info.iCount);
	}

void CSemaphoreListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	TSemaphoreKernelInfo& info = *(TSemaphoreKernelInfo*)iInfo;
	_LIT8(KDesc,"Semaphore;KernObjAddr;FullName;AccessCount;Count");
	_LIT8(KFmt, "Semaphore;%x;%S;%d;%d");
	
	if (i == 0) clogger.Log(KDesc);
	clogger.Log(KFmt, info.iAddressOfKernelObject, &info.iFullName, info.iAccessCount, info.iCount);
	}

void CSemaphoreListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*aTemp*/)
	{
	TSemaphoreKernelInfo& info = *(TSemaphoreKernelInfo*)iInfo;
	_LIT(KInfo, "Semaphore info");
	aTitle.Copy(KInfo);

	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	inf.AppendFormat(_L("Kernel object address: 0x%08x\n"), info.iAddressOfKernelObject);
	inf.AppendFormat(_L("Access count: %u\n"), info.iAccessCount);
	inf.AppendFormat(_L("Count: %d"), info.iCount);
	}
