// timerlistboxdata.cpp
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

CTimerListBoxData::CTimerListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CTimerListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	TTimerKernelInfo& info = *(TTimerKernelInfo*)aInfo;
	name.Copy(info.iFullName);
	PrettyName(iType, name);

	more.Format(_L("Client status=0x%08x"), info.iClientStatus);
	}

void CTimerListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	TTimerKernelInfo& info = *(TTimerKernelInfo*)iInfo;
	_LIT8(KDesc,"Timer;KernObjAddr;FullName;AccessCount;State;Type;TReqStat");
	_LIT8(KFmt, "Timer;%x;%S;%d;%d;%d;%x");
	
	if (i == 0) clogger.Log(KDesc);
	clogger.Log(KFmt, info.iAddressOfKernelObject, &info.iFullName, info.iAccessCount, info.iState, info.iType, info.iClientStatus);
	}

#define CASE_LIT2(val, name) case val: { _LIT(KName, #name); return KName; }

const TDesC& Type(TUint8 aType)
	{
	switch (aType)
		{
		CASE_LIT2(1, ERelative);
		CASE_LIT2(2, EAbsolute);
		CASE_LIT2(4, ELocked);
		CASE_LIT2(8, EHighRes);
		CASE_LIT2(16, EInactivity);
		default:
			_LIT(KHuh, "?");
			return KHuh;
		}
	}

const TDesC& State(TUint8 aState)
	{
	switch (aState)
		{
		CASE_LIT2(0, EIdle);
		CASE_LIT2(1, EWaiting);
		CASE_LIT2(2, EWaitHighRes);
		default:
			_LIT(KHuh, "?");
			return KHuh;
		}
	}

void CTimerListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*aTemp*/)
	{
	TTimerKernelInfo& info = *(TTimerKernelInfo*)iInfo;
	_LIT(KInfo, "Timer info");
	aTitle.Copy(KInfo);

	inf.Copy(iInfo->iFullName);
	inf.Append(Klflf);
	
	inf.AppendFormat(_L("Kernel object address: 0x%08x\n"), info.iAddressOfKernelObject);
	inf.AppendFormat(_L("Access count: %u\n"), info.iAccessCount);
	const TDesC& type = Type(info.iType);
	inf.AppendFormat(_L("Type: %d (%S)\n"), info.iType, &type);
	const TDesC& state = State(info.iState);
	inf.AppendFormat(_L("State: %d (%S)\n"), info.iState, &state);
	inf.AppendFormat(_L("TRequestStatus: 0x%08x"), info.iClientStatus);
	}
