// halListboxdata.cpp
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
#include <HAL.h>

CHalListBoxData::CHalListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CHalListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& itemId)
	{
	SHalInfo& info = *reinterpret_cast<SHalInfo*>(aInfo);
	name.Copy(ToStringHal(info.iAttribute));
	ToStringHalVal(more, info.iAttribute, info.iValue);
	if (info.iProperties & HAL::EEntryDynamic)
		{
		more.Append(_L(" (Dynamic)"));
		}
	itemId = info.iAttribute;
	}

void CHalListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	_LIT(KHalDesc,"HAL;Number;Name;Value;ValueHumanReadable;Flags");
	_LIT(KHalFmt,"HAL;%i;%S;%i;%S;%i");

	if (i == 0) clogger.Log(KHalDesc);
	SHalInfo& info = *reinterpret_cast<SHalInfo*>(iInfo);
	TPtrC name = ToStringHal(info.iAttribute);
	RBuf val;
	val.CreateL(256);
	ToStringHalVal(val, info.iAttribute, info.iValue);
	clogger.Log(KHalFmt, info.iAttribute, &name, info.iValue, &val, info.iProperties);
	val.Close();
	}

void CHalListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*aName*/)
	{
	SHalInfo& info = *reinterpret_cast<SHalInfo*>(iInfo);
	_LIT(KInfo, "HAL info");
	aTitle.Copy(KInfo);
	TPtrC name = ToStringHal(info.iAttribute);
	RBuf val;
	val.CreateL(256);
	ToStringHalVal(val, info.iAttribute, info.iValue);
	_LIT(KHalFmt, "%S\n%S\n\n(Id: %i Val: %i/0x%x)");
	inf.Format(KHalFmt, &name, &val, info.iAttribute, info.iValue, info.iValue);
	val.Close();
	}
