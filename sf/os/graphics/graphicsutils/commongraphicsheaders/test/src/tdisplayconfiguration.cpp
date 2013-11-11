// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test class for TDisplayConfiguration
// 
//

#include "tdisplayconfiguration.h"
#include <test/extendtef.h>

#include "../inc/displayconfiguration.h"

// This handles any non-member uses of the extended ASSERT_XXX macros
void TefUnitFailLeaveL()
	{
	
	User::Leave(KErrTEFUnitFail);
	}

// Create a suite of all the tests
CTestSuite* CTestDisplayConfiguration::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE_OPT(CTestDisplayConfiguration,NULL);

		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0001L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0002L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0003L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0004L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0005L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0006L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0007L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0008L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0009L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0010L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0011L);
		ADD_THIS_TEST_STEP(GRAPHICS_COMMONHEADER_0012L);
	
	END_SUITE;	
	}

// Published Tests

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0001L
@SYMTestCaseDesc		TDisplayConfiguration field validation
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Check default configuration is empty
@SYMTestActions			
	Create an instance of TDisplayConfiguration
	Check default version is size of TDisplayConfiguration
	Check each member is not defined and does not change a passed in field
@SYMTestExpectedResults	
	Holds correct size and fields are empty
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0001L()
	{
	TDisplayConfiguration testConfig;
	
	ASSERT_EQUALS	(testConfig.Version(),(TInt)sizeof(testConfig));
	
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	
	TSize size(10,11);
	ASSERT_FALSE	(testConfig.GetResolution(size));
	ASSERT_EQUALS	(size,TSize(10,11));
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0002L
@SYMTestCaseDesc		Setting TDisplayConfiguration variables
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Check setting variables works correctly
@SYMTestActions			
	Set resolution 1
	Check resolution 1 set
	Set resolution 2
	Check resolution 2 set
	Set twips resolution 1
	Check twips resolution 1 set
	Set twips resolution 2
	Check twips resolution 2 set
@SYMTestExpectedResults	
	All 'gets' return as expected
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0002L()
	{
	TDisplayConfiguration testConfig;
	
	TSize sizeSet1(10,11);
	TSize sizeSet2(12,13);
	TSize sizeCheck1(14,15);
	TSize sizeCheck2(16,17);
	TSize twipsSizeSet1(7620,2858);
	TSize twipsSizeSet2(7630,2868);	
	TSize twipsSizeCheck1(7640,2878);
	TSize twipsSizeCheck2(7650,2888);
	
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	//Set size check
	testConfig.SetResolution(sizeSet1);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_TRUE		(testConfig.GetResolution(sizeCheck1));
	ASSERT_EQUALS	(sizeSet1,sizeCheck1);
	//Set size check 2
	testConfig.SetResolution(sizeSet2);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_TRUE		(testConfig.GetResolution(sizeCheck2));
	ASSERT_EQUALS	(sizeSet2,sizeCheck2);
	//Set twips check
	testConfig.SetResolutionTwips(twipsSizeSet1);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolutionTwips));
	ASSERT_TRUE		(testConfig.GetResolutionTwips(twipsSizeCheck1));
	ASSERT_EQUALS	(twipsSizeSet1,twipsSizeCheck1);
	//Set twips check 2
	testConfig.SetResolutionTwips(twipsSizeSet2);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolutionTwips));
	ASSERT_TRUE		(testConfig.GetResolutionTwips(twipsSizeCheck2));
	ASSERT_EQUALS	(twipsSizeSet2,twipsSizeCheck2);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0003L
@SYMTestCaseDesc		Clearing TDisplayConfiguration variables
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Checking Clear works as expected
@SYMTestActions			
	Set resolution
	Clear resolution
	Get resolution *should do nothing*
@SYMTestExpectedResults	
	Clears variables as expected
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0003L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size1(10,11);
	TSize size2(12,13);
	TSize size2copy(size2);
	
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	testConfig.SetResolution(size1);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	testConfig.Clear(TDisplayConfiguration::EResolution);
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_FALSE	(testConfig.GetResolution(size2));
	//check size2 has not been changed
	ASSERT_EQUALS	(size2,size2copy);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0004L
@SYMTestCaseDesc		Clear all TDisplayConfiguration variables
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Check ClearAll performs correctly
@SYMTestActions			
	Set resolution
	Set color
	ClearAll()
	Get resolution *should do nothing*
@SYMTestExpectedResults	
	ClearAll should clear all!
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0004L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size1(10,11);
	TSize size2(12,13);
	TSize size2copy(size2);
	
	TRgb color1(50,255);
	TRgb color2(52,255);
	TRgb color2copy(color2);
	
	//set values
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	testConfig.SetResolution(size1);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	
	testConfig.ClearAll();
	
	//check cleared variables
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_FALSE	(testConfig.GetResolution(size2));
	//check size2 has not been changed
	ASSERT_EQUALS(size2,size2copy);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0005L
@SYMTestCaseDesc		TDisplayConfiguration copy operator
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Checking copy works correctly
@SYMTestActions			
	Set values
	Make a copy
	Check copy has copied values
	Make slight changes to copy, check operator== works (thorough test fo coverage)
@SYMTestExpectedResults	
	Should copy variables to new config
	Should perform operator== correctly
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0005L()
	{
	TDisplayConfiguration testConfig1;
	
	TSize size1(10,11);
	TSize size2(12,13);
	TSize sizeTemp(0,1);
	TDisplayConfiguration1::TRotation rotTemp = TDisplayConfiguration1::ERotationIllegal;
	
	testConfig1.SetResolution(size1);
	testConfig1.SetResolutionTwips(size2);
	testConfig1.SetRotation(TDisplayConfiguration1::ERotationNormal);
	
	TDisplayConfiguration testConfig2(testConfig1);
	
	ASSERT_TRUE		(testConfig2.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_TRUE		(testConfig2.GetResolution(sizeTemp));
	ASSERT_EQUALS	(size1,sizeTemp);
	
	ASSERT_TRUE		(testConfig2.IsDefined(TDisplayConfiguration::EResolutionTwips));
	ASSERT_TRUE		(testConfig2.GetResolutionTwips(sizeTemp));
	ASSERT_EQUALS	(size2,sizeTemp);
	
	ASSERT_TRUE		(testConfig2.IsDefined(TDisplayConfiguration::ERotation));
	ASSERT_TRUE		(testConfig2.GetRotation(rotTemp));
	ASSERT_EQUALS	(rotTemp,TDisplayConfiguration1::ERotationNormal);
	
	ASSERT_TRUE		(testConfig1==testConfig2);
	
	testConfig1.Clear(TDisplayConfiguration::EResolution);
	testConfig2.Clear(TDisplayConfiguration::EResolution);
	testConfig1.Clear(TDisplayConfiguration::EResolutionTwips);
	testConfig2.Clear(TDisplayConfiguration::EResolutionTwips);
	testConfig1.Clear(TDisplayConfiguration::ERotation);
	testConfig2.Clear(TDisplayConfiguration::ERotation);

	ASSERT_TRUE		(testConfig1==testConfig2);
	
	TDisplayConfiguration largeConfig(200);
	TDisplayConfiguration emptyConfig;
	ASSERT_FALSE	(largeConfig==emptyConfig);
	ASSERT_FALSE	(emptyConfig==largeConfig);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0006L
@SYMTestCaseDesc		Small configs do nothing
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Older config versions should not try to access newer variables
@SYMTestActions			
	Create very small config
	Set resolution
	Get resolution *should not have set it*
@SYMTestExpectedResults	
	Setting when version is too old should fail silently.  No returned error, no panic
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0006L()
	{
	TDisplayConfiguration testConfig(sizeof(TDisplayConfigurationBase));
	
	TSize size1(10,11);
	TSize size2(12,13);
	
	testConfig.SetResolution(size1);
	ASSERT_FALSE	(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_FALSE	(testConfig.GetResolution(size2));
	ASSERT_NOT_EQUALS	(size1,size2);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0007L
@SYMTestCaseDesc		Negative resolution panics client
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Negative resolutions are not allowed and should panic the client
@SYMTestActions			
	Set negative X resolution
@SYMTestExpectedResults	
	Should panic with DISPCONF 1
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0007L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size(-10,10);
	testConfig.SetResolution(size);
	
	//Should not get here
	ASSERT_TRUE(0);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0008L
@SYMTestCaseDesc		Negative resolution panics client
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			Negative resolutions are not allowed and should panic the client
@SYMTestActions			
	Set negative Y resolution
@SYMTestExpectedResults	
	Should panic with DISPCONF 1
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0008L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size(10,-10);
	testConfig.SetResolution(size);
	
	//Should not get here
	ASSERT_TRUE(0);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0009L
@SYMTestCaseDesc		One resolution axis 0 panics client
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			A resolution with only 1 axis at 0 is invalid
@SYMTestActions			
	Set 0 on X axis only for resolution
@SYMTestExpectedResults	
	Should panic with DISPCONF 2
	
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0009L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size(0,10);
	testConfig.SetResolution(size);
	
	//Should not get here
	ASSERT_TRUE(0);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0010L
@SYMTestCaseDesc		One resolution axis 0 panics client
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			A resolution with only 1 axis at 0 is invalid
@SYMTestActions			
	Set 0 on Y axis only for resolution
@SYMTestExpectedResults	
	Should panic with DISPCONF 2
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0010L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size(10,0);
	testConfig.SetResolution(size);
	
	//Should not get here
	ASSERT_TRUE(0);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0011L
@SYMTestCaseDesc		Both resolution axis 0 valid
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			A resolution with only both axis at 0 is valid
@SYMTestActions			
	Set 0 on both axes for resolution
@SYMTestExpectedResults	
	Should complete and be able to return the same size
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0011L()
	{
	TDisplayConfiguration testConfig;
	
	TSize size1(0,0);
	TSize size2(1,2);
	testConfig.SetResolution(size1);
	ASSERT_TRUE		(testConfig.IsDefined(TDisplayConfiguration::EResolution));
	ASSERT_TRUE		(testConfig.GetResolution(size2));
	ASSERT_EQUALS	(size1,size2);
	}

/**
@SYMTestCaseID			GRAPHICS_COMMONHEADER_0012L
@SYMTestCaseDesc		Invalid preferred version causes panic
@SYMREQ					REQ10325
@SYMPREQ				PREQ2102
@SYMTestType			CT
@SYMTestPurpose			An invalid version will cause a panic
@SYMTestActions			
	Set version to be 1
	Should panic
@SYMTestExpectedResults	
	Should panic with DISPCONF 7
**/
void CTestDisplayConfiguration::GRAPHICS_COMMONHEADER_0012L()
	{
	TDisplayConfiguration testConfig (1);

	//Should not get here
	ASSERT_TRUE(0);
	}
