
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
// This main DLL entry point for the TSU_MMF_ACOD.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_ACODSuite.h"
#include "TSU_MMF_ACOD.h"


/** @xxxx
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite_MMF_ACOD* NewTestSuiteL() 
    { 
	CTestSuite_MMF_ACOD* result = new (ELeave) CTestSuite_MMF_ACOD;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

//destructor
CTestSuite_MMF_ACOD::~CTestSuite_MMF_ACOD()
	{
	}

//Get Test Suite version
_LIT(KTxtVersion,"0.1");
TPtrC CTestSuite_MMF_ACOD::GetVersion( void )
	{
	return KTxtVersion();
	}

/** @xxxx
 * Add a test step into the suite
 * @param	"CTestStep_MMF_ACOD* aPtrTestStep" 
 *			Test step pointer
 */
void CTestSuite_MMF_ACOD::AddTestStepL( CTestStep_MMF_ACOD* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);
	// add the step using tyhe base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}


//------------------------------------------------------------------

/** @xxxx 
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestSuite_MMF_ACOD 
 */
void CTestSuite_MMF_ACOD::InitialiseL( void )
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_ACOD");

	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0001() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0002() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0003() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0004() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0005() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0006() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0007() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0008() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0009() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0010() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0011() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0012() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0013() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0014() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0015() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0016() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0017() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0018() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0019() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0020() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0021() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0022() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0023() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0024() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0025() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0026() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0027() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0028() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0029() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0030() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0031() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0032() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0033() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0034() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0035() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0036() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0037() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0038() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0039() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0040() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0041() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0042() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0043() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0044() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0045() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0046() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0047() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0048() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0049() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0050() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0051() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0052() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0053() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0054() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0055() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0056() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0057() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0058() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0059() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0060() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0061() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0062() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0063() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0064() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0065() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0066() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0067() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0068() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0069() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0070() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0071() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0072() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0073() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0074() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0075() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0076() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0077() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0078() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0079() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0080() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0081() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0082() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0083() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0084() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0085() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0086() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0087() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0088() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0089() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0090() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0091() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0092() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0093() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0094() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0095() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0096() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0097() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0098() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0099() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0100() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0101() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0102() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0103() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0104() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0105() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0106() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0107() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0108() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0109() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0110() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0111() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0112() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0113() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0114() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0115() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0116() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0117() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0118() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0119() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0120() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0121() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0122() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0123() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0124() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0125() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0126() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0127() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0128() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0129() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0130() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0131() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0132() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0133() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0134() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0135() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0136() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0137() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0138() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0139() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0140() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0141() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0142() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0143() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0144() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0145() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0146() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0147() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0148() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0149() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0150() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0151() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0152() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0153() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0154() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0155() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0156() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0157() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0158() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0159() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0160() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0161() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0162() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0163() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0164() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0165() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0166() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0167() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0168() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0169() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0170() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0171() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0172() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0173() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0174() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0175() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0176() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0177() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0178() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0179() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0180() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0181() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0182() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0183() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0184() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0185() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0186() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0187() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0188() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0189() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0190() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0191() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0192() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0193() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0194() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0195() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0196() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0197() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0198() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0199() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0200() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0201() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0202() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0203() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0204() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0205() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0206() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0207() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0208() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0209() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0210() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0211() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0212() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0213() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0214() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0215() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0216() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0217() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0218() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0219() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0220() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0221() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0222() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0223() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0224() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0225() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0226() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0227() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0228() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0229() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0230() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0231() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0232() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0233() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0234() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0235() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0236() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0237() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0238() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0239() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0240() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0241() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0242() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0243() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0244() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0245() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0246() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0247() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0248() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0249() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0250() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0251() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0252() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0253() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0254() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0255() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0256() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0257() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0258() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0259() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0260() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0261() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0262() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0263() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0264() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0265() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0266() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0267() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0268() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0269() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0270() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0271() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0272() );
	AddTestStepL( new(ELeave) CTest_MMF_ACOD_U_0273() );


	}


