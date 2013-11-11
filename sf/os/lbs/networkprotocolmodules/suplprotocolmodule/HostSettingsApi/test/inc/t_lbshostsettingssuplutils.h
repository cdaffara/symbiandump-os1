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
//

#ifndef T_LBSHOSTSETTINGSSUPLUTILS_H
#define T_LBSHOSTSETTINGSSUPLUTILS_H

#include <e32base.h>

/**
SUPL test utility class
@internalTechnology
@test
*/
class CTestSuplUtils : public CBase
	{
public:
	static TInt  GetHostSettingFromConfig(CTestConfig& aConfig, TLbsHostSettingsSupl& aSetting, const TPtrC& aSectionName);
	static TBool AreSettingsEqual(const TLbsHostSettingsSupl& aSetting1, const TLbsHostSettingsSupl& aSetting2);
	};

#endif // T_LBSHOSTSETTINGSSUPLUTILS_H
