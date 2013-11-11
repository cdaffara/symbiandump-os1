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

#ifndef TESTSTEPCIEAACPLUSDECODER_H
#define TESTSTEPCIEAACPLUSDECODER_H

#include <testframework.h>

// Custom interface definitions
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>

#include "devsoundciutestdevices.hrh"

// Test system includes
#include "tsu_mmf_devsound_ciu_step.h"
#include "tsu_mmf_devsound_ciu_suite.h"


class CTestStepCIEAacPlusDecoder : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoder();
	TVerdict DoTestStepL();
	};
	
class CTestStepCIEAacPlusDecoderGetSetFrequency : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderGetSetFrequency(TBool aIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCIEAacPlusDecoderGetSetObjType : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderGetSetObjType(TBool aIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCIEAacPlusDecoderGetSetNumChannels : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderGetSetNumChannels(TBool aIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCIEAacPlusDecoderGetSetSbr : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderGetSetSbr(TBool aIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};

class CTestStepCIEAacPlusDecoderGetSetDsm : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderGetSetDsm(TBool aIsGetTest);
	TVerdict DoTestStepL();
private:
	TBool iIsGetTest;
	};
	
class CTestStepCIEAacPlusDecoderApplyConfig : public CTestStepCustominterfaceUnificationUnit
	{
public:
	CTestStepCIEAacPlusDecoderApplyConfig();
	TVerdict DoTestStepL();
	};

#endif
