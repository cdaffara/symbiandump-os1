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
// This is the main DLL entry point for the TSU_MMF_BASECL.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_BASECLSuite.h"
#include "TSU_MMF_BASECL_Buffers.h"
#include "TSU_MMF_BASECL_DataPath.h"


/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteBaseClassesL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuiteBaseClasses*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_BASECL* NewTestSuiteL() 
    { 
	CTestSuite_MMF_BASECL* result = new (ELeave) CTestSuite_MMF_BASECL;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }


CTestSuite_MMF_BASECL::~CTestSuite_MMF_BASECL()
	{
	}


/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 * @xxxx
 *
 */
TPtrC CTestSuite_MMF_BASECL::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.1");
	return KTxtVersion();
	}

/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteBaseClasses
 *
 * @xxxx
 * 
 */
void CTestSuite_MMF_BASECL::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_BASECL");

	// add test steps
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0002 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0003 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0004 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0005 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0006 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0007 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0008 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0009 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0010 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0011 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0012 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0100 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0101 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0102 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0103 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0104 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0105 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0106 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0107 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0108 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0109 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0110 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0111 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0112 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0113 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0114 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0115 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0116 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0117 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0118 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0119 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0120 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0121 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0150 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0151 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0152 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0153 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0154 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0157 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0159 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0160 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0200 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0201 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0202 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0203 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0204 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0205 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0206 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0207 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0208 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0209 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0210 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0211 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0212 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0213 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0214 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0215 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0216 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0217 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0218 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0219 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0220 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0221 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0222 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0223 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0224 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0225 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0226 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0227 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0228 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0229 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0230 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0231 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0232 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0233 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0234 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0235 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0236 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0237 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0238 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0239 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0240 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0241 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0242 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0243 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0244 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0245 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0246 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0260 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0261 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0262 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0263 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0264 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0265 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0266 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0267 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0280 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0281 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0282 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0300 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0301 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0302 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0303 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0304 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0305 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0306 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0307 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0308 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0309 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0310 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0311 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0312 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0313 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0314 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0315 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0316 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0317 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0318 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0319 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0320 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0321 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0322 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0323 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0324 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0325 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0326 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0327 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0328 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0329 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0330 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0331 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0332 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0333 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0334 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0335 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0336 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0337 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0338 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0339 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0340 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0341 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0342 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0343 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0344 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0345 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0360 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0361 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0362 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0363 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0364 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0365 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0366 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0367 );
	AddTestStepL( new(ELeave) CTestStep_MMF_BASECL_U_0380 );
	AddTestStepL( RTestStep_MMF_BASECL_U_1000::NewL() );
	AddTestStepL( RTestStep_MMF_BASECL_U_1100::NewL() );
	AddTestStepL( RTestStep_MMF_BASECL_U_1200::NewL() );
	}


// -------------------------
