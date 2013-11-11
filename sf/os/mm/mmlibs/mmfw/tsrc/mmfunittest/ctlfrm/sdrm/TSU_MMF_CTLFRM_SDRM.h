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

#ifndef __TSU_MMF_CTLFRM_SDRM_H__
#define __TSU_MMF_CTLFRM_SDRM_H__

#include <testframework.h>

#include <s32mem.h>
#include <badesca.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfcontrollerframework.h>
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

#include "TS_Codes.h"

class CTestSuite;
class CTestSuite_MMF_CTLFRM_SDRM;

/**
 *
 * CTestStep_MMF_CTLFRM_SDRM, which is the base class for all 
 * the SelfTest suite test steps.
 *
 * @lib "TSU_MMF_CTLFRM_SDRM.lib"
 *
 * @xxxx
 *
 */
class CTestStep_MMF_CTLFRM_SDRM : public CTestStep
	{
public:
	CTestStep_MMF_CTLFRM_SDRM();
	~CTestStep_MMF_CTLFRM_SDRM();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite_MMF_CTLFRM_SDRM* aTestSuite )
  { iMMFCTLFRMTestSuite = aTestSuite;}  

	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	// pointer to suite which owns this test 
	const CTestSuite_MMF_CTLFRM_SDRM * iMMFCTLFRMTestSuite;
	};

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0001
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0001 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0001() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0001(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0007
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0007 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0007() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0007(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0008
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0008 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0008() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0008(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0009
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0009 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0009() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0009(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0010
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0010 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0010() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0010(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0011
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0011 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0011() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0011(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0012
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0012 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0012() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0012(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0013
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0013 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0013() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0013(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0014
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0014 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0014() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0014(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0015
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0015 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0015() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0015(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0016
 *@test Req. under test REQ172.5.5 REQ172.5.5.8
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0016 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0016() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0016(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0017
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0017 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0017() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0017(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0018
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0018 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0018() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0018(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0019
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0019 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0019() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0019(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0020
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0020 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0020() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0020(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0021
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0021 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0021() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0021(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0022
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0022 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0022() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0022(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0023
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0023 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0023() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0023(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0024
 *@test Req.
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0024 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0024() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0024(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0027
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0027 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0027() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0027(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0028
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0028 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0028() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0028(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0029
 *@test Req. 
 */
// Failure to Open Controller, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0029 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0029() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0029(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0030
 *@test Req. 
 */
// Failure to Open Data Source, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0030 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0030() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0030(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0031
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0031 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0031() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0031(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0032
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0032 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0032() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0032(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0033
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0033 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0033() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0033(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0034
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0034 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0034() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0034(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0035
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0035 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0035() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0035(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0038
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0038 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0038() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0038(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
 /**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0039
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0039 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0039() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0039(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;
 
/**
 * @class CTestGT0137_15_I_0040
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0040 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0040() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0040(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0041
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0041 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0041() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0041(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0042
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0042 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0042() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0042(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0043
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0043 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0043() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0043(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0044
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0044 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0044() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0044(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0045
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0045 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0045() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0045(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0050
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0050 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0050() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0050(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0051
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0051 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0051() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0051(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0052
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0052 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0052() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0052(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0053
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0053 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0053() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0053(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0054
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0054 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0054() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0054(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0060
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0060 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0060() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0060(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0061
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0061 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0061() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0061(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0070
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0070 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0070() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0070(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0071
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0071 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0071() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0071(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0072
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0072 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0072() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0072(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0073
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0073 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0073() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0073(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0074
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0074 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0074() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0074(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0075
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0075 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0075() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0075(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0076
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0076 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0076() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0076(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0080
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0080 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0080() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0080(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0081
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0081 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0081() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0081(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0082
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0082 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0082() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0082(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0083
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0083 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0083() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0083(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0084
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0084 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0084() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0084(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0137_15_I_0085
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0085 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0085() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0085(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0086
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0086 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0086() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0086(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0100
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0100 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0100() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0100(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

	//---------------------------------//
	// Memory Management Tests (Alloc) //
	//---------------------------------//

/**
 * @class CTestGT0137_15_I_0110
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0110 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0110() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0110(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0111
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0111 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0111() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0111(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0112
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0112 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0112() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0112(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0120
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0120 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0120() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0120(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0121
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0121 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0121() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0121(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0122
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0122 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0122() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0122(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0123
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0123 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0123() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0123(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0124
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0124 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0124() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0124(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0127
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0127 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0127() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0127(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0130
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0130 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0130() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0130(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0137_15_I_0131
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0131 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0131() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0131(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0140
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0140 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0140() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0140(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0141
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0141 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0141() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0141(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0142
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0142 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0142() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0142(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0143
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0143 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0143() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0143(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0144
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0144 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0144() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0144(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0145
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0145 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0145() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0145(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0146
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0146 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0146() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0146(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

// CMMFControllerImplelementationInformnation

/**
 * @class CTestGT0147_15_I_0150
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0150 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0150() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0150(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0151
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0151 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0151() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0151(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0152
 *@test Req. INC038123
 */
// Open controllers with different max heap specified
class CTestStep_MMF_CTLFRM_SDRM_U_0152 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0152() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0152(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;



/**
 * @class CTestGT0147_15_I_0160
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0160 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0160() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0160(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0161
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0161 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0161() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0161(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;


/**
 * @class CTestGT0147_15_I_0162
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0162 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0162() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0162(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0163
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0163 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0163() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0163(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0164
 *@test Req.
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0164 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0164() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0164(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestGT0147_15_I_0165
 *@test Req. 
 */
// Failure to Open Data Sink, this test is supposed to fail.
class CTestStep_MMF_CTLFRM_SDRM_U_0165 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0165() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0165(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * 
 *@test Req. 
 */
class CTestStep_MMF_CTLFRM_SDRM_U_0197 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0197() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0197(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_0200
 *@test Req. 
 */
// OOM test for OpenInSecureDRMProcess()
class CTestStep_MMF_CTLFRM_SDRM_U_0200 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0200() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0200(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_OpenController
 *@test Req.
 */
// Calling IsSecureDrmMode() in controller's ConstructL()
class CTestStep_MMF_CTLFRM_SDRM_U_OpenController : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_OpenController(const TDesC& aTestName, 
 											TUid aControllerUid) ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_OpenController(){} ;
 virtual TVerdict DoTestStepL( void );
private:
 TUid iControllerUid;
 } ;

/**
 * @class CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController
 *@test Req.
 */
// Calling IsSecureDrmMode() in controller's ConstructL() in Secure DRM Mode
class CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(const TDesC& aTestName, 
 											TUid aControllerUid) ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_OpenSecureController(){} ;
 virtual TVerdict DoTestStepL( void );
private:
 TUid iControllerUid;
 } ;

//CR1655 -  MMF support for configurable controller stack size
class CTestStep_MMF_CTLFRM_SDRM_U_0300 : public CTestStep_MMF_CTLFRM_SDRM
 {
public:
 CTestStep_MMF_CTLFRM_SDRM_U_0300() ;
 ~CTestStep_MMF_CTLFRM_SDRM_U_0300(){} ;
 virtual TVerdict DoTestStepL( void );
 } ;

#endif
