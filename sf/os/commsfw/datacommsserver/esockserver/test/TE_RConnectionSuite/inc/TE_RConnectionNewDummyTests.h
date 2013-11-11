// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file that contains the declaration of the new test cases that use Dummy Nif
// added to improve test coverage of RConnection APIs
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionNEWDUMMYTESTS_H__)
#define __TE_RConnectionNEWDUMMYTESTS_H__


#include "TE_RConnectionStep.h"


class TE_RConnectionTest800 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest800();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest801 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest801();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest802 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest802();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest803 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest803();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest804 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest804();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest805 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest805();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest806 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest806();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest807 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest807();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest808 : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionTest808();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionDataMonitorStepBase : public TE_RConnectionStep
	{
public:
	virtual ~TE_RConnectionDataMonitorStepBase();

protected:	
	void StartThreeConnectionsAndOpenUdpSocketLC(RSocketServ& aSs, TInt aIap, RSocket& aSocket, RConnection& aConn1, RConnection& aConn2, RConnection& aConn3);
	void StopConnectionAndCleanup(RConnection& aConn);
	void SendDataAndCheckCompletionsL(RSocket& aSocket, TInt aCount, TUint aAmountToSend[], TRequestStatus* aCompletionOrder[]);
	};

class TE_RConnectionTest809 : public TE_RConnectionDataMonitorStepBase
	{
public:
	virtual ~TE_RConnectionTest809();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest810 : public TE_RConnectionDataMonitorStepBase
	{
public:
	virtual ~TE_RConnectionTest810();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest811 : public TE_RConnectionDataMonitorStepBase
	{
public:
	virtual ~TE_RConnectionTest811();

	virtual enum TVerdict doTestStepL(void);
	};

class TE_RConnectionTest812 : public TE_RConnectionDataMonitorStepBase
	{
public:
	virtual ~TE_RConnectionTest812();

	virtual enum TVerdict doTestStepL(void);
	};

#endif

