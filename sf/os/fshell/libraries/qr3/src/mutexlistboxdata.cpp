// mutexlistboxdata.cpp
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

CMutexListBoxData::CMutexListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CMutexListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	TMutexKernelInfo& info = *(TMutexKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	more.Format(_L("Order=%d Held=%d Waiting=%d"), info.iOrder, info.iHoldCount, info.iWaitCount);
	}

void CMutexListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	TMutexKernelInfo& info = *(TMutexKernelInfo*)iInfo;
	_LIT8(KDesc,"Mutex;KernObjAddr;FullName;AccessCount;HoldCount;WaitCount;Order");
	_LIT8(KFmt, "Mutex;%x;%S;%d;%x;%u");
	
	if (i == 0) clogger.Log(KDesc);
	clogger.Log(KFmt, info.iAddressOfKernelObject, &info.iFullName, info.iAccessCount, info.iHoldCount, info.iWaitCount, info.iOrder);
	}

#define CASE_LIT2(val, name) case val: { _LIT(KName, #name); return KName; }

const TDesC& Name(TUint8 aOrder)
	{
	switch (aOrder)
		{
		CASE_LIT2(0xff, KMutexOrdNone);
		CASE_LIT2(0xfe, KMutexOrdUser);
		CASE_LIT2(0xf7, KMutexOrdGeneral7);
		CASE_LIT2(0xf6, KMutexOrdGeneral6);
		CASE_LIT2(0xf5, KMutexOrdGeneral5);
		CASE_LIT2(0xf4, KMutexOrdGeneral4);
		CASE_LIT2(0xf3, KMutexOrdGeneral3);
		CASE_LIT2(0xf2, KMutexOrdGeneral2);
		CASE_LIT2(0xf1, KMutexOrdGeneral1);
		CASE_LIT2(0xf0, KMutexOrdGeneral0);
		CASE_LIT2(0x60, KMutexOrdCodeSegLock);
		CASE_LIT2(0x5e, KMutexOrdPubSub2);
		CASE_LIT2(0x5c, KMutexOrdDemandPaging);
		CASE_LIT2(0x58, KMutexOrdPowerMgr);
		CASE_LIT2(0x50, KMutexOrdPubSub);
		CASE_LIT2(0x48, KMutexOrdProcessLock);
		CASE_LIT2(0x47, KMutexOrdDebug);
		CASE_LIT2(0x40, KMutexOrdTimer);
		CASE_LIT2(0x38, KMutexOrdObjectCon2);
		CASE_LIT2(0x30, KMutexOrdHandle);
		CASE_LIT2(0x28, KMutexOrdObjectCon);
		CASE_LIT2(0x20, KMutexOrdMachineConfig);
		CASE_LIT2(0x10, KMutexOrdHwChunk);
		CASE_LIT2(0x08, KMutexOrdKernelHeap);
		CASE_LIT2(0x01, KMutexOrdRamAlloc);
		default:
			_LIT(KHuh, "?");
			return KHuh;
		}
	}

void CMutexListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*aTemp*/)
	{
	TMutexKernelInfo& info = *(TMutexKernelInfo*)iInfo;
	_LIT(KInfo, "Mutex info");
	aTitle.Copy(KInfo);

	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	inf.AppendFormat(_L("Kernel object address: 0x%08x\n"), info.iAddressOfKernelObject);
	TPtrC ord = Name(info.iOrder);
	inf.AppendFormat(_L("Order: 0x%02x (%S)\n"), info.iOrder, &ord);
	inf.AppendFormat(_L("Access count: %u\n"), info.iAccessCount);
	inf.AppendFormat(_L("Hold count: %d\nWaiting count: %d"), info.iHoldCount, info.iWaitCount);
	}
