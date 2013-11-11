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

#if !defined(TE_RCONNECTIONSERV_TESTSTEPS_H)
#define TE_RCONNECTIONSERV_TESTSTEPS_H

class TE_RConnectionServStep : public CTestStep
/**
* TE_RConnectionStep implements all generic connection behaviour needed by all the tests
* @internalComponent
* @test
*/
	{
public:
	TE_RConnectionStep();
	virtual ~TE_RConnectionStep();

private:
	enum TVerdict doTestStepPreambleL();
	enum TVerdict doTestStepPostambleL();
	TInt ReadIniFile(void);
	
protected:					
	
	// to all of these methods pass in a pointer to objects you have created on the stack...
	// lots of them will also create temporary automatics but don't worry about that
	}



class CTestStep_NET_RCONNECTION_SERV_04_000 : public TE_RConnectionServStep
{
public:
	virtual enum TVerdict doTestStepL();

};

#endif // TE_RCONNECTIONSERV_TESTSTEPS_H

