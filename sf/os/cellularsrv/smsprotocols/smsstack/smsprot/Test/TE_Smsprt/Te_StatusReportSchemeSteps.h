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


#ifndef __TE_STATUS_REPORT_SCHEME_STEPS_H__
#define __TE_STATUS_REPORT_SCHEME_STEPS_H__


/**
  Declaration for all status report scheme test cases.
*/

#include "Te_StatusReportSchemeTransfer.h"
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

/**
Concrete test step - Sending TP-SRR set in every PDU
*/
class CTestSendingTPSRRSchemeInAllPDUs : public CTestSendingTPSRRScheme
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Sending TP-SRR set in last PDU
*/
class CTestSendingTPSRRSchemeInLastPDU : public CTestSendingTPSRRScheme
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for TP-SRR set in last PDUs
*/
class CTestReceivingLastTPSRR : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for TPSRR set in All PDUs
*/
class CTestReceivingTPSRRAllPDU : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for TP-SRR set in some PDUs
*/
class CTestReceivingTPSRRSomePDU : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


//SMSC

/**
Concrete test step - Sending PDU with SMSC not set
*/
class CTestSendingSMSCSchemeSinglePDUNoBitSet : public CTestSendingSMSCScheme
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Sending PDU with SMSC set in some bits
*/
class CTestSendingSMSCSchemeSinglePDUSomeBitsSet : public CTestSendingSMSCScheme
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Sending multiple PDUs with SMSC set in some bits
*/
class CTestSendingSMSCSchemeMultiplePDUsSomeBitsSet : public CTestSendingSMSCScheme
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for SMSC set (0x01) in last PDUs
*/
class CTestReceivingLastSMSC : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for SMSC set (0x01) in every PDU
*/
class CTestReceivingSMSCAllPDU : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving status reports for SMSC set in some PDUs
*/
class CTestReceivingSMSCSomePDU : public CTestReceivingStatusReports
	{
public:
	virtual TVerdict doTestStepPreambleL();
	};


/**
Concrete test step - Receiving Messages set with SMSC Scheme - Multiple PDUs
*/
class CTestReceivingSMSCMultiplePDU : public CTestReceivingMessages
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	};


/**
Concrete test step - Receiving Messages set with SMSC Scheme - Single PDU
*/
class CTestReceivingSMSCSinglePDU : public CTestReceivingMessages
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	};


/**
Concrete test step - Receiving Messages set with TPSRR Scheme
*/
class CTestReceivingDeliversWithTPSRR : public CTestReceivingMessages
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	};


/**
Concrete test step - Deliver Messages with TPSRR Scheme
*/
class CTestDeliversWithTPSRRScheme : public CTestReceivingDeliversWithTPSRR
	{
public:
	virtual TVerdict doTestStepL();	
	};

#endif
