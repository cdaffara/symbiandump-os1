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

#ifndef __TSU_MMF_CTLFRM_H__
#define __TSU_MMF_CTLFRM_H__

#include <testframework.h>

#include <s32mem.h>
#include <badesca.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include <mmf/plugin/mmfplugininterfaceuids.hrh>

#include "Ts_cmmftestcontroller.h"

#include "TS_MMFTestDataSource.h"
#include "MMFTestDataSourceUIDs.hrh"

#include "TS_MMFTestDataSink.h"
#include "MMFTestDataSinkUIDs.hrh"

#include "TS_MMFTestDataSourceB.h"
#include "MMFTestDataSourceBUIDs.hrh"

#include "TS_MMFTestDataSinkB.h"
#include "MMFTestDataSinkBUIDs.hrh"

#include "TS_CMMFTestCustomCommands.h"

#include "ts_cmmftestterminationcontroller.h"

#include "TS_Codes.h"

class CTestSuite;
class CTestSuite_MMF_CTLFRM;

/**
 *
 * CTestStep_MMF_CTLFRM, which is the base class for all 
 * the SelfTest suite test steps.
 *
 * @lib "TSU_MMF_CTLFRM.lib"
 *
 * @xxxx
 *
 */
class CTestStep_MMF_CTLFRM : public CTestStep
	{
public:
	CTestStep_MMF_CTLFRM();
	~CTestStep_MMF_CTLFRM();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite_MMF_CTLFRM* aTestSuite )
  { iMMFCTLFRMTestSuite = aTestSuite;}  

	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	// pointer to suite which owns this test 
	const CTestSuite_MMF_CTLFRM * iMMFCTLFRMTestSuite;
	};

/**
 * @class CTestGT0137_15_I_0001
 *@test Req. under test REQ172.5.1 REQ172.5.2 REQ172.5.5.12
 */
class CTestStep_MMF_CTLFRM_U_0001 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0001() ;
 ~CTestStep_MMF_CTLFRM_U_0001(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0007
 *@test Req. under test REQ172.5.5 REQ172.5.5.1
 */
class CTestStep_MMF_CTLFRM_U_0007 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0007() ;
 ~CTestStep_MMF_CTLFRM_U_0007(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0008
 *@test Req. under test REQ172.5.5 REQ172.5.5.3
 */
class CTestStep_MMF_CTLFRM_U_0008 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0008() ;
 ~CTestStep_MMF_CTLFRM_U_0008(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0009
 *@test Req. under test REQ172.5.5 REQ172.5.5.1
 */
class CTestStep_MMF_CTLFRM_U_0009 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0009() ;
 ~CTestStep_MMF_CTLFRM_U_0009(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0010
 *@test Req. under test REQ172.5.5 REQ172.5.5.3
 */
class CTestStep_MMF_CTLFRM_U_0010 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0010() ;
 ~CTestStep_MMF_CTLFRM_U_0010(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0011
 *@test Req. under test REQ172.5.5 REQ172.5.5.2
 */
class CTestStep_MMF_CTLFRM_U_0011 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0011() ;
 ~CTestStep_MMF_CTLFRM_U_0011(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0012
 *@test Req. under test REQ172.5.5 REQ172.5.5.4
 */
class CTestStep_MMF_CTLFRM_U_0012 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0012() ;
 ~CTestStep_MMF_CTLFRM_U_0012(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0013
 *@test Req. under test No Requirement Found
 */
class CTestStep_MMF_CTLFRM_U_0013 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0013() ;
 ~CTestStep_MMF_CTLFRM_U_0013(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0014
 *@test Req. under test REQ172.5.5 REQ172.5.5.5
 */
class CTestStep_MMF_CTLFRM_U_0014 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0014() ;
 ~CTestStep_MMF_CTLFRM_U_0014(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0015
 *@test Req. under test REQ172.5.5 REQ172.5.5.6
 */
class CTestStep_MMF_CTLFRM_U_0015 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0015() ;
 ~CTestStep_MMF_CTLFRM_U_0015(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0016
 *@test Req. under test REQ172.5.5 REQ172.5.5.8
 */
class CTestStep_MMF_CTLFRM_U_0016 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0016() ;
 ~CTestStep_MMF_CTLFRM_U_0016(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0017
 *@test Req. under test REQ172.5.5 REQ172.5.5.7
 */
class CTestStep_MMF_CTLFRM_U_0017 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0017() ;
 ~CTestStep_MMF_CTLFRM_U_0017(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0018
 *@test Req. under test REQ172.5.5 REQ172.5.5.10
 */
class CTestStep_MMF_CTLFRM_U_0018 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0018() ;
 ~CTestStep_MMF_CTLFRM_U_0018(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0019
 *@test Req. under test REQ172.5.5 REQ172.5.5.11
 */
class CTestStep_MMF_CTLFRM_U_0019 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0019() ;
 ~CTestStep_MMF_CTLFRM_U_0019(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0020
 *@test Req. under test No Requirement Found
 */
class CTestStep_MMF_CTLFRM_U_0020 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0020() ;
 ~CTestStep_MMF_CTLFRM_U_0020(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0021
 *@test Req. under test No Requirement Found
 */
class CTestStep_MMF_CTLFRM_U_0021 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0021() ;
 ~CTestStep_MMF_CTLFRM_U_0021(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0022
 *@test Req. under test No Requirement Found
 */
class CTestStep_MMF_CTLFRM_U_0022 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0022() ;
 ~CTestStep_MMF_CTLFRM_U_0022(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0023
 *@test Req. under test REQ172.5.5 REQ172.5.5.14
 */
class CTestStep_MMF_CTLFRM_U_0023 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0023() ;
 ~CTestStep_MMF_CTLFRM_U_0023(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0024
 *@test Req. under test REQ172.5.5 REQ172.5.5.14
 */
class CTestStep_MMF_CTLFRM_U_0024 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0024() ;
 ~CTestStep_MMF_CTLFRM_U_0024(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0027
 *@test Req. under test REQ172.5.5 
 */
class CTestStep_MMF_CTLFRM_U_0027 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0027() ;
 ~CTestStep_MMF_CTLFRM_U_0027(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0028
 *@test Req. under test REQ172.5.5 
 */
class CTestStep_MMF_CTLFRM_U_0028 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0028() ;
 ~CTestStep_MMF_CTLFRM_U_0028(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0029
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Controller, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0029 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0029() ;
 ~CTestStep_MMF_CTLFRM_U_0029(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0030
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Source, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0030 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0030() ;
 ~CTestStep_MMF_CTLFRM_U_0030(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0031
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0031 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0031() ;
 ~CTestStep_MMF_CTLFRM_U_0031(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0032
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0032 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0032() ;
 ~CTestStep_MMF_CTLFRM_U_0032(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0033
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0033 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0033() ;
 ~CTestStep_MMF_CTLFRM_U_0033(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0034
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0034 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0034() ;
 ~CTestStep_MMF_CTLFRM_U_0034(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0035
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0035 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0035() ;
 ~CTestStep_MMF_CTLFRM_U_0035(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0040
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0040 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0040() ;
 ~CTestStep_MMF_CTLFRM_U_0040(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0041
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0041 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0041() ;
 ~CTestStep_MMF_CTLFRM_U_0041(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0042
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0042 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0042() ;
 ~CTestStep_MMF_CTLFRM_U_0042(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0043
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0043 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0043() ;
 ~CTestStep_MMF_CTLFRM_U_0043(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0044
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0044 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0044() ;
 ~CTestStep_MMF_CTLFRM_U_0044(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0045
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0045 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0045() ;
 ~CTestStep_MMF_CTLFRM_U_0045(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0050
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0050 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0050() ;
 ~CTestStep_MMF_CTLFRM_U_0050(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0051
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0051 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0051() ;
 ~CTestStep_MMF_CTLFRM_U_0051(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0052
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0052 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0052() ;
 ~CTestStep_MMF_CTLFRM_U_0052(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0053
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0053 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0053() ;
 ~CTestStep_MMF_CTLFRM_U_0053(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0054
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0054 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0054() ;
 ~CTestStep_MMF_CTLFRM_U_0054(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0060
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0060 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0060() ;
 ~CTestStep_MMF_CTLFRM_U_0060(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0061
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0061 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0061() ;
 ~CTestStep_MMF_CTLFRM_U_0061(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0070
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0070 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0070() ;
 ~CTestStep_MMF_CTLFRM_U_0070(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0071
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0071 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0071() ;
 ~CTestStep_MMF_CTLFRM_U_0071(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0072
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0072 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0072() ;
 ~CTestStep_MMF_CTLFRM_U_0072(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0073
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0073 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0073() ;
 ~CTestStep_MMF_CTLFRM_U_0073(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0074
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0074 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0074() ;
 ~CTestStep_MMF_CTLFRM_U_0074(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0075
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0075 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0075() ;
 ~CTestStep_MMF_CTLFRM_U_0075(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0076
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0076 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0076() ;
 ~CTestStep_MMF_CTLFRM_U_0076(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0080
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0080 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0080() ;
 ~CTestStep_MMF_CTLFRM_U_0080(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0081
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0081 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0081() ;
 ~CTestStep_MMF_CTLFRM_U_0081(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0082
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0082 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0082() ;
 ~CTestStep_MMF_CTLFRM_U_0082(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0083
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0083 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0083() ;
 ~CTestStep_MMF_CTLFRM_U_0083(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0084
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0084 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0084() ;
 ~CTestStep_MMF_CTLFRM_U_0084(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0085
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0085 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0085() ;
 ~CTestStep_MMF_CTLFRM_U_0085(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0086
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0086 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0086() ;
 ~CTestStep_MMF_CTLFRM_U_0086(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0100
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0100 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0100() ;
 ~CTestStep_MMF_CTLFRM_U_0100(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

	//---------------------------------//
	// Memory Management Tests (Alloc) //
	//---------------------------------//

/**
 * @class CTestGT0137_15_I_0110
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0110 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0110() ;
 ~CTestStep_MMF_CTLFRM_U_0110(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0111
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0111 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0111() ;
 ~CTestStep_MMF_CTLFRM_U_0111(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0112
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0112 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0112() ;
 ~CTestStep_MMF_CTLFRM_U_0112(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0120
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0120 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0120() ;
 ~CTestStep_MMF_CTLFRM_U_0120(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0121
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0121 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0121() ;
 ~CTestStep_MMF_CTLFRM_U_0121(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0122
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0122 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0122() ;
 ~CTestStep_MMF_CTLFRM_U_0122(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0123
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0123 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0123() ;
 ~CTestStep_MMF_CTLFRM_U_0123(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0124
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0124 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0124() ;
 ~CTestStep_MMF_CTLFRM_U_0124(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0125
 *@test Req. under test REQ172.5.5 
 */
// Test extraction of file extension from URI
class CTestStep_MMF_CTLFRM_U_0125 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0125() ;
 ~CTestStep_MMF_CTLFRM_U_0125(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0127
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0127 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0127() ;
 ~CTestStep_MMF_CTLFRM_U_0127(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0130
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0130 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0130() ;
 ~CTestStep_MMF_CTLFRM_U_0130(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0131
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0131 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0131() ;
 ~CTestStep_MMF_CTLFRM_U_0131(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0140
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0140 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0140() ;
 ~CTestStep_MMF_CTLFRM_U_0140(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0141
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0141 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0141() ;
 ~CTestStep_MMF_CTLFRM_U_0141(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0142
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0142 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0142() ;
 ~CTestStep_MMF_CTLFRM_U_0142(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0143
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0143 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0143() ;
 ~CTestStep_MMF_CTLFRM_U_0143(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0144
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0144 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0144() ;
 ~CTestStep_MMF_CTLFRM_U_0144(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0145
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0145 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0145() ;
 ~CTestStep_MMF_CTLFRM_U_0145(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0146
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0146 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0146() ;
 ~CTestStep_MMF_CTLFRM_U_0146(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

// CMMFControllerImplelementationInformnation

/**
 * @class CTestGT0147_15_I_0150
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0150 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0150() ;
 ~CTestStep_MMF_CTLFRM_U_0150(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0151
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0151 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0151() ;
 ~CTestStep_MMF_CTLFRM_U_0151(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0152
 *@test Req. INC038123
 */
// Open controllers with different max heap specified
class CTestStep_MMF_CTLFRM_U_0152 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0152() ;
 ~CTestStep_MMF_CTLFRM_U_0152(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;



/**
 * @class CTestGT0147_15_I_0160
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0160 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0160() ;
 ~CTestStep_MMF_CTLFRM_U_0160(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0161
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0161 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0161() ;
 ~CTestStep_MMF_CTLFRM_U_0161(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0162
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0162 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0162() ;
 ~CTestStep_MMF_CTLFRM_U_0162(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0163
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0163 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0163() ;
 ~CTestStep_MMF_CTLFRM_U_0163(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0164
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0164 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0164() ;
 ~CTestStep_MMF_CTLFRM_U_0164(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0165
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0165 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0165() ;
 ~CTestStep_MMF_CTLFRM_U_0165(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0170
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0170 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0170() ;
 ~CTestStep_MMF_CTLFRM_U_0170(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0171
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0171 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0171() ;
 ~CTestStep_MMF_CTLFRM_U_0171(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0172
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0172 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0172() ;
 ~CTestStep_MMF_CTLFRM_U_0172(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0173
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0173 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0173() ;
 ~CTestStep_MMF_CTLFRM_U_0173(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0174
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0174 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0174() ;
 ~CTestStep_MMF_CTLFRM_U_0174(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0175
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0175 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0175() ;
 ~CTestStep_MMF_CTLFRM_U_0175(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0176
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0176 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0176() ;
 ~CTestStep_MMF_CTLFRM_U_0176(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0180
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0180 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0180() ;
 ~CTestStep_MMF_CTLFRM_U_0180(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0181
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0181 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0181() ;
 ~CTestStep_MMF_CTLFRM_U_0181(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0182
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0182 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0182() ;
 ~CTestStep_MMF_CTLFRM_U_0182(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0183
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0183 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0183() ;
 ~CTestStep_MMF_CTLFRM_U_0183(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0184
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0184 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0184() ;
 ~CTestStep_MMF_CTLFRM_U_0184(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0190
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0190 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0190() ;
 ~CTestStep_MMF_CTLFRM_U_0190(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0191
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0191 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0191() ;
 ~CTestStep_MMF_CTLFRM_U_0191(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0192
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0192 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0192() ;
 ~CTestStep_MMF_CTLFRM_U_0192(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0193
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0193 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0193() ;
 ~CTestStep_MMF_CTLFRM_U_0193(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0194
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0194 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0194() ;
 ~CTestStep_MMF_CTLFRM_U_0194(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0195
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0195 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0195() ;
 ~CTestStep_MMF_CTLFRM_U_0195(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0196
 *@test Req. under test REQ172.5.5 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_U_0196 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0196() ;
 ~CTestStep_MMF_CTLFRM_U_0196(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * 
 *@test Req. under CR0632
 */
class CTestStep_MMF_CTLFRM_U_0197 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0197() ;
 ~CTestStep_MMF_CTLFRM_U_0197(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
/**
 * 
 *@test Req. under CR0632
 */
class CTestStep_MMF_CTLFRM_U_0198 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0198() ;
 ~CTestStep_MMF_CTLFRM_U_0198(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
 /**
  * @class RTestStep_MMF_CTLFRM_U_0200
  *@test Req. INC108007
  */
// Open controllers and executes an asynchronous custom command that panics subthread (with Active Scheduler).
class MMMF_CTRLFRM_U_0200_Observer
 {
public:
 virtual void OpeningController() = 0;
 virtual void OpenCompleted(TInt aError) = 0;	
 virtual void ClosingController() = 0;
 virtual void ClosingComplete() = 0;
 virtual void SendingCustomCommand() = 0;
 virtual void CustomCommandSent(TInt aError) = 0;
 virtual void CancellingCustomCommand() = 0;
 virtual void CustomCommandCancelled(TInt aError) = 0;
 virtual void TestError(TInt aError) = 0;
 };

class RTestStep_MMF_CTLFRM_U_0200 : public RAsyncTestStep, public MMMF_CTRLFRM_U_0200_Observer
 {
public:
 RTestStep_MMF_CTLFRM_U_0200() ;
 ~RTestStep_MMF_CTLFRM_U_0200(){} ;
 static TInt Timeout(TAny* aArg);
 
 // From RAsyncTestStep
 void KickoffTestL();
 void CloseTest();
 
 // From MMMF_CTRLFRM_U_0200_Observer
 void OpeningController();
 void OpenCompleted(TInt aError);	
 void ClosingController();
 void ClosingComplete();
 void SendingCustomCommand();
 void CustomCommandSent(TInt aError);
 void CancellingCustomCommand();
 void CustomCommandCancelled(TInt aError);
 void TestError(TInt aError);
	 
private:
 class CAsyncTest : public CActive
  {
  public:
   CAsyncTest(MMMF_CTRLFRM_U_0200_Observer* aObserver);
   ~CAsyncTest();
   void Start();
  private:
   void RunL();
   void DoCancel();
   TInt RunError(TInt aError);
  private:
   MMMF_CTRLFRM_U_0200_Observer* iObserver;
   RMMFController iController;
   TMMFMessageDestinationPckg iMessage;
  } *iAsyncTest;
 } ;
 
 /**
  * @class CTestStep_MMF_CTLFRM_U_0201
  *@test Req. INC108007
  */
// Open controllers and executes an asynchronous custom command that panics subthread (without Active Scheduler).
class CTestStep_MMF_CTLFRM_U_0201 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0201() ;
 ~CTestStep_MMF_CTLFRM_U_0201(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

 /**
  * @class CTestStep_MMF_CTLFRM_U_0202
  *@test Req. INC108007
  */
// Open and close controller. There will be a memory allocation failure while closing
 
class CTestStep_MMF_CTLFRM_U_0202 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0202() ;
 ~CTestStep_MMF_CTLFRM_U_0202(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
 /**
  * @class CTestStep_MMF_CTLFRM_U_0203
  *@test Req. INC108007
  */
// Open and close controller. There will be a memory allocation failure while closing
 
class CTestStep_MMF_CTLFRM_U_0203 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0203() ;
 ~CTestStep_MMF_CTLFRM_U_0203(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

#ifdef SYMBIAN_BUILD_GCE
 
/**
 * 
 *@test Req. under REQ 7418: use surface command
 */
class CTestStep_MMF_CTLFRM_U_0301 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0301() ;
 ~CTestStep_MMF_CTLFRM_U_0301(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
/**
 * 
 *@test Req. under REQ 7418: get surface parameters command
 */
class CTestStep_MMF_CTLFRM_U_0303 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0303() ;
 ~CTestStep_MMF_CTLFRM_U_0303(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
/**
 * 
 *@test Req. under REQ 7418: surface removed  command
 */
class CTestStep_MMF_CTLFRM_U_0305 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0305(TSurfaceId& aSurfaceId, TBool aNegTest) ;
 ~CTestStep_MMF_CTLFRM_U_0305(){} ;
 virtual TVerdict DoTestStepL( void );
private:
 TSurfaceId iSurfaceId;
 TSurfaceId iSurfaceId2;
 TBool iNegTest;
 } ;
#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
class CTestStep_MMF_CTLFRM_Subtitles : public CTestStep_MMF_CTLFRM
	{
public:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	void SetExpectedSubtitlesAvailableL(TInt aAvailable);
	TVerdict CheckExpectedFunctionText(const TDesC8& aExpected);
protected:
	RMMFController iController;
	RMMFVideoPlaySubtitleSupportCustomCommands* iSubtitleCommands;
	};

class CTestStep_MMF_CTRLFRM_U_400 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_400();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_CTRLFRM_U_401 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_401();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_402 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_402();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_403 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_403();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_CTRLFRM_U_404 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_404();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_405 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_405();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_406 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_406();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_407 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_407();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_408 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_408();
	virtual TVerdict DoTestStepL();
	};
	
class CTestStep_MMF_CTRLFRM_U_409 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_409();
	virtual TVerdict DoTestStepL();
	};

class CTestStep_MMF_CTRLFRM_U_410 : public CTestStep_MMF_CTLFRM_Subtitles
	{
public:
	CTestStep_MMF_CTRLFRM_U_410();
	virtual TVerdict DoTestStepL();
	};

#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT


// CR1655 -  MMF support for configurable controller stack size
 
class CTestStep_MMF_CTLFRM_U_0300 : public CTestStep_MMF_CTLFRM
 {
public:
 CTestStep_MMF_CTLFRM_U_0300() ;
 ~CTestStep_MMF_CTLFRM_U_0300(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

#endif // __TSU_MMF_CTLFRM_H__
