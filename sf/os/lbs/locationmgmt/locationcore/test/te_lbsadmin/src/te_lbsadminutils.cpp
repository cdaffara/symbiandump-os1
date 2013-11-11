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

#include "lbsadminimpl.h"
#include "te_lbsadminutils.h"


TInt te_LbsAdminUtils::ConfirmSettingPluginExists(TLbsAdminSetting aSetting, TInt aValue)
	{
	// Check that the aValue refers to one of the test values, if not
	// then use the original function.
	TInt err(KErrNotFound);
	switch (aSetting)
		{
		case KLbsSettingHomeProtocolModule:
		case KLbsSettingRoamingProtocolModule:
			{
			if (aValue == 1)
				{
				err = KErrNone;
				}
			else
				{
				err = LbsAdminUtils::ConfirmSettingPluginExists(aSetting, aValue);
				}
			break;
			}
		case KLbsSettingSetClockModule:
			{
			if (aValue == 23)
				{
				err = KErrNone;
				}
			else
				{
				err = LbsAdminUtils::ConfirmSettingPluginExists(aSetting, aValue);
				}
			break;
			}
		default:
			{
			break;
			}
		}
	return err;
	}

