// featreglistboxdata.cpp
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

CFeatRegListBoxData::CFeatRegListBoxData(CKernListBoxModel* aModel)
	: CKernListBoxData(aModel)
	{
	}

void CFeatRegListBoxData::DoFormatL(TObjectKernelInfo* aInfo, RBuf& name, RBuf& more, TInt& /*itemId*/)
	{
	SFeature& info = *reinterpret_cast<SFeature*>(aInfo);
	more.Format(_L("Value: %i Info: %i"), info.iErr, info.iInfo);
	gPlugin->FormatValue(name, MProductPlugin::EFeature, info.iFeature, 0);
	}

void CFeatRegListBoxData::DumpToCloggerL(RClogger& clogger, TInt i, TInt /*count*/)
	{
	_LIT(KFeatureDesc,"Feature;Uid;Name;Value;Info");
	_LIT(KFeatureFmt, "Feature;0x%x;%S;%i;%i");

	if (i == 0) clogger.Log(KFeatureDesc);
	SFeature& info = *reinterpret_cast<SFeature*>(iInfo);
	TBuf<256> buf;
	gPlugin->FormatValue(buf, MProductPlugin::EFeature, info.iFeature, 0);
	clogger.Log(KFeatureFmt, info.iFeature, &buf, info.iErr, info.iInfo);
	}

void CFeatRegListBoxData::DoInfoForDialogL(RBuf& aTitle, RBuf& inf, TDes* /*name*/)
	{
	SFeature& info = *reinterpret_cast<SFeature*>(iInfo);

	_LIT(KInfo, "Feature info");
	aTitle.Copy(KInfo);

	gPlugin->FormatValue(inf, MProductPlugin::EFeature, info.iFeature, 0);
	_LIT(KFeatureFmt, "\n0x%x\n\nFeature present: %i\n(Additional info: %i)");

	inf.AppendFormat(KFeatureFmt, info.iFeature, info.iErr, info.iInfo);
	}
