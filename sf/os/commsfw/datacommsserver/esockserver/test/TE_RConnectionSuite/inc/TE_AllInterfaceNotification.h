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
// TS_AllInterfaceNotification.h 
// 
//
 
#if !defined(TS_ALLINTERFACENOTIFICATION_H)
#define TS_ALLINTERFACENOTIFICATION_H

#include "TE_RConnectionStep.h"

class TE_RConnectionTest300 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest301 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest302 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest303 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest304 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest305 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest306 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest307 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest308 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest309 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest310 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest311 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest312 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest313 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest314 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest315 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest316 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest317 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest318 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTestAllInterfaceNot210 : public CTestStep
    {
public:
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();

    ~TE_RConnectionTestAllInterfaceNot210();
    
private:
    RSocketServ iSS;
    RConnection iConnStart;
    RConnection iConnAttach;
    RConnection iConnAllIfaceNotify;
    RTimer iTimer;
    };

#endif
// TS_ALLINTERFACENOTIFICATION_H

