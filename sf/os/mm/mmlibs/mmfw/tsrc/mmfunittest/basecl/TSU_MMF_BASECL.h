// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_BASECL_H__
#define __TSU_MMF_BASECL_H__

#include <testframework.h>

class CTestSuite;
class CTestSuite_MMF_BASECL;

/**
 *
 * CTestStep_MMF_BASECL, which is the base class for all 
 * the MMF BASECL suite test steps.
 *
 * @lib "TSU_MMF_BASECL.lib"
 *
 * @xxxx
 *
 */
class CTestStep_MMF_BASECL : public CTestStep
	{
public:
	static void CleanUp(TAny *aPtr) ;
	};


#endif /* __TSU_MMF_BASECL_H__ */
