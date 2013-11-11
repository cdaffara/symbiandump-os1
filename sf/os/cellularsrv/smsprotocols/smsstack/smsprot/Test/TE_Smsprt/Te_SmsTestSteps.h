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
//

/**
 @file
*/
 
#ifndef TE_SMSTESTSTEPS_H
#define TE_SMSTESTSTEPS_H

#include <Gsmumsg.h>
#include <gsmubuf.h>

#include "smsstackbaseteststeps.h"

/**
  Test Step to Load SMS Protocol
*/
class CLoadSMSProtocol : public CSmsBaseTestStep
{
public:
	CLoadSMSProtocol(RSocketServ &aSocketServer);
	
	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
};

/**
  Test Step to receive a SMS message
*/
class CReceiveSMS : public CSmsBaseTestStep
	{
public:
	CReceiveSMS(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
	};

/**
  Test Step to send a SMS message
*/
class CSendSMS : public CSmsBaseTestStep
{
public:
	CSendSMS(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
};

/**
  Close the socket and the socket server session
*/
class CCloseSMSProtocol : public CSmsBaseTestStep
{
public:
	CCloseSMSProtocol(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
};


/**
  Delete segmentation and reasembly stores
*/
class CDeleteSARStores : public CSmsBaseTestStep
{
public:
	//From CTestStep
    CDeleteSARStores();
	virtual TVerdict doTestStepL();
};

/**
  Forwards system time by x number of minutes
*/
class CForwardSystemTime : public CSmsBaseTestStep
{
public:
    CForwardSystemTime();
	//From CTestStep
	virtual TVerdict doTestStepL();
};

/**
  Tests receive of SMS message while SMSStack Out of Memory (OOM)
*/
class CSendReceiveSMSWhileServerInOOM : public CSmsBaseTestStep
{
public:
	CSendReceiveSMSWhileServerInOOM(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
};

/**
  Compares the text contents of a SMS message with some text
*/
class CCompareSMS : public CSmsBaseTestStep
{
public:
	CCompareSMS(CSmsMessage &aSmsMessage);
	
	//From CTestStep
	virtual TVerdict doTestStepL();
};


/**
  Checks if the Class0 Stores have been enabled
*/
class CClass0StoreEnabled : public CSmsBaseTestStep
{
public:
	CClass0StoreEnabled(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
	
public:	
    RSocketServ* iSharedSocketServer;   
};

/**
  Test for Enumeration in an OOD Condition 
*/
class CEnumerateInOODCondition : public CSmsBaseTestStep
{
public:
	CEnumerateInOODCondition(RSocketServ &aSocketServer);

	//From CTestStep
	virtual TVerdict doTestStepL();
    
public: 
    RSocketServ* iSharedSocketServer;   
};


#endif // TE_SMSTESTSTEPS_H
