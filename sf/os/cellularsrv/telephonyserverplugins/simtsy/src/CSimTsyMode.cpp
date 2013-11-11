// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements determining the TSY mode to run in (ie CDMA or GSM)
// 
//

/**
 @file
*/

#include "CSimTsyMode.h"
#include "e32err.h"

// read in from config file
TInt CSimTsyMode::InitL(CSimPhone *aPhone)
{
	__ASSERT_ALWAYS(Dll::Tls() == NULL, SimPanic(ETlsAlreadySet));

	TTsyMode* tsyMode = new(ELeave) TTsyMode;
	*tsyMode = ENoCdma;
	Dll::SetTls(tsyMode);

	TBool tsyModeFound = FALSE;
	const CTestConfigItem* tsyModeItem = aPhone->CfgFile()->Item(KTsyMode); // CfgFile is guaranteed to exist in phone object
	TInt tmp = 0;
	TInt tsyModeElement = KErrNone;
	if (tsyModeItem)
		{
		tsyModeElement = CTestConfig::GetElement(tsyModeItem->Value(),KStdDelimiter,0,tmp);
		if (tsyModeElement == KErrNone)
			{
			*tsyMode = static_cast<CSimTsyMode::TTsyMode>(tmp);
			tsyModeFound = TRUE;
			}
		}
	if (!tsyModeFound)
		{
		const CTestConfigSection* defaultConfigSection = aPhone->DefaultCfgFile();
		if (defaultConfigSection)
			{
			tsyModeItem = defaultConfigSection->Item(KTsyMode);
			if (tsyModeItem)
				{
				tsyModeElement = CTestConfig::GetElement(tsyModeItem->Value(),KStdDelimiter,0,tmp);
				if (tsyModeElement == KErrNone)
					{
					*tsyMode = static_cast<CSimTsyMode::TTsyMode>(tmp);
					tsyModeFound = TRUE;
					}
				}
			}
		}
	if (!tsyModeFound)
		{
		*tsyMode = CSimTsyMode::ENoCdma;
		}
	return KErrNone;
}


void CSimTsyMode::FreeMode()
	{
	TTsyMode* tsyMode = static_cast<TTsyMode*>(Dll::Tls());
	Dll::FreeTls();
	delete tsyMode;
	tsyMode = NULL;
	}

CSimTsyMode::TTsyMode CSimTsyMode::GetMode()
	{
	TTsyMode* tsyMode = static_cast<TTsyMode*>(Dll::Tls());
	__ASSERT_ALWAYS(tsyMode != NULL, SimPanic(ETlsNotSet));
	return *tsyMode;
	}
