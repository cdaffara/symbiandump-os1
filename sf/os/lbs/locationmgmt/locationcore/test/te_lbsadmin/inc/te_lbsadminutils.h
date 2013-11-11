// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test-only replacement some LbsAdminUtils functions, to
// allow the component test (te_LbsAdminsuite) to fully test the
// LbsAdmin api.
// 
//

#ifndef TE_LBSADMINUTILS_H_
#define TE_LBSADMINUTILS_H_

class te_LbsAdminUtils
	{
public:
	static TInt ConfirmSettingPluginExists(TLbsAdminSetting aSetting, TInt aValue);
	};

#endif // TE_LBSADMINUTILS_H_

