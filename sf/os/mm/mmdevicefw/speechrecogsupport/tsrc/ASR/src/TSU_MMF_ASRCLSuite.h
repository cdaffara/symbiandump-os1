// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_ASRCLSUITE_H__
#define __TSU_MMF_ASRCLSUITE_H__

class  CTestMMF_ASRCLSuite : public CTestSuite
{
public:
	void InitialiseL( void );
	virtual ~CTestMMF_ASRCLSuite();
	void AddTestStepL( RTestMMF_ASRCLStep * ptrTestStep );
	TPtrC GetVersion( void );
};

#endif //__TSU_MMF_ASRCLSUITE_H__

