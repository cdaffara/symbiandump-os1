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

#ifndef TESTSTEPCISBCENCODER_H
#define TESTSTEPCISBCENCODER_H

#include <testframework.h>

// Custom interface definitions
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>

#include "devsoundciutestdevices.hrh"

// Test system includes
#include "tsu_mmf_devsound_ciu_step.h"
#include "tsu_mmf_devsound_ciu_suite.h"

class CTestStepCISbcEncoder : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoder();
	TVerdict DoTestStepL();
	};

class CTestStepCISbcEncoderGetSupportedFrequencies : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedFrequencies();
	TVerdict DoTestStepL();
private:
	TBool ValidateArray(const RArray<TUint>& aArray) const;
	};

class CTestStepCISbcEncoderGetSupportedSubbands : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedSubbands();
	TVerdict DoTestStepL();
private:
	TBool ValidateArray(const RArray<TUint>& aArray) const;
	};

class CTestStepCISbcEncoderGetSupportedNumOfBlocks : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedNumOfBlocks();
	TVerdict DoTestStepL();
private:
	TBool ValidateArray(const RArray<TUint>& aArray) const;
	};

class CTestStepCISbcEncoderGetSupportedChannelModes : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedChannelModes();
	TVerdict DoTestStepL();
private:
	TBool ValidateArray(const RArray<MSbcEncoderIntfc::TSbcChannelMode>& aArray) const;
	};

class CTestStepCISbcEncoderGetSupportedAllocationMethods : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedAllocationMethods();
	TVerdict DoTestStepL();
private:
	TBool ValidateArray(const RArray<MSbcEncoderIntfc::TSbcAllocationMethod>& aArray) const;
	};

class CTestStepCISbcEncoderGetSupportedBitpoolRange : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSupportedBitpoolRange();
	TVerdict DoTestStepL();
	};
	
class CTestStepCISbcEncoderGetSetFrequency : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetFrequency(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCISbcEncoderGetSetChannelMode : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetChannelMode(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCISbcEncoderGetSetNumOfSubbands : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetNumOfSubbands(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCISbcEncoderGetSetNumOfBlocks : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetNumOfBlocks(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCISbcEncoderGetSetAllocationMethod : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetAllocationMethod(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCISbcEncoderGetSetBitpoolSize : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderGetSetBitpoolSize(TBool iIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};
	
class CTestStepCISbcEncoderApplyConfig : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCISbcEncoderApplyConfig();
	TVerdict DoTestStepL();
	};

#endif
