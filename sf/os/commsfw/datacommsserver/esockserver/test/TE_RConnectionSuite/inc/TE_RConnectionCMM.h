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

#if !defined(TE_RCONNECTIONCMM_H)
#define TE_RCONNECTIONCMM_H

#include "TE_RConnectionStep.h"

class TE_RConnectionTest400 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest401 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest402 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest403 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest404 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest405 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest406 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest407 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest408 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest409 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest410 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest411 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest412 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest413 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest414 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest415 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest416 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest417 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest418 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest419 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest419b : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest420 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest421 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest422 : public TE_RConnectionStep
/**
* Test the erroring of sockets following an EStopNormal shutdown of an interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest423 : public TE_RConnectionStep
/**
* Test the erroring of sockets following an EStopAuthoritative shutdown of an interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest424 : public TE_RConnectionStep
/**
* Test stopping subconnections while using the Nifman compatibility layer
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest425 : public TE_RConnectionStep
/**
* Multiple interfaces dealing with requests (both using compatibility layer in NIFMAN) - test DataTransferredRequest()
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest426 : public TE_RConnectionStep
/**
* Multiple interfaces dealing with requests (both using compatibility layer in NIFMAN) - test data send notification requests and cancel
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest427 : public TE_RConnectionStep
/**
* Multiple interfaces dealing with requests (both using compatibility layer in NIFMAN) - test data received notification requests and cancel
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest428 : public TE_RConnectionStep
/**
* Explicitly create a single test NIF connection (using commdb settings) enumerate subconnections and get subconnection info (both with and without subconnection ID)
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest429 : public TE_RConnectionStep
/**
* AllSubConnectionNotification test using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest430 : public TE_RConnectionStep
/**
* Test of cancellation of AllSubConnectionNotification when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest431 : public TE_RConnectionStep
/**
* Test of DataTransferredRequest() when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest432 : public TE_RConnectionStep
/**
* Test of DataTransferredRequest() for subconnection when using test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest433 : public TE_RConnectionStep
/**
* Test of DataSentNotification() request when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest434 : public TE_RConnectionStep
/**
* Test of DataSentNotification() request for subconnections when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest435 : public TE_RConnectionStep
/**
* Test of DataSentNotification() cancellation when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest436 : public TE_RConnectionStep
/**
* Test of DataSentNotification cancellation for subconnections when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest437 : public TE_RConnectionStep
/**
* Test of DataReceivedNotification when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest438 : public TE_RConnectionStep
/**
* Test of DataReceivedNotification() request for subconnections when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest439 : public TE_RConnectionStep
/**
* Test of DataReceivedNotification() cancellation when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest440 : public TE_RConnectionStep
/**
* Test of DataReceivedNotification() cancellation for subconnections when using the test NIF
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest441 : public TE_RConnectionQoSStep
/**
* Test DataTransferredRequest for subconnection with primary and secondary contexts present
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest441* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest442 : public TE_RConnectionQoSStep
/**
* Test DataTransferred Request() for subconnection (primary context) with a secondary context present
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest442* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest443 : public TE_RConnectionQoSStep
/**
* Test DataTransferredRequest() for subconnection (secondary context) with another secondary context present
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest443* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest444 : public TE_RConnectionQoSStep
/**
* Test DataTransferredRequest() for complete connection when multiple subconnections are active
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest444* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest445 : public TE_RConnectionQoSStep
/**
* Test for DataTransferred Request on one context then another
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest445* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest446 : public TE_RConnectionQoSStep
/**
* Test DataSentNotification() for a secondary context while the primary is also active
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest446* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest447 : public TE_RConnectionQoSStep
/**
* Test DataSentNotification() for primary context while a secondary is also active
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest447* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest448 : public TE_RConnectionQoSStep
/**
* Test DataSentNotification() for complete connection while multiple subconnections are active and passing traffic
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest448* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest449 : public TE_RConnectionQoSStep
/**
* Test for DataSentNotification on one context then another
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest449* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
protected:
};

class TE_RConnectionTest450 : public TE_RConnectionQoSStep
/**
* Tests for multiple parallel DataSentNotification requests and cancellation with multiple contexts
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest450* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest451 : public TE_RConnectionQoSStep
/**
* Test DataReceivedNotification() for a secondary context while the primary is also active
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest451* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest452 : public TE_RConnectionQoSStep
/**
* Test DataReceivedNotification() for primary context while a secondary is also active
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest452* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest453 : public TE_RConnectionQoSStep
/**
* Test DataReceivedNotification() for complete connection while multiple subconnections are active and passing traffic
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest453* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest454 : public TE_RConnectionQoSStep
/**
* Test for DataReceivedNotification on one context then another
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest454* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest455 : public TE_RConnectionQoSStep
/**
* Tests for multiple parallel DataReceivedNotification requests and cancellation with multiple contexts
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest455* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();

};


class TE_RConnectionTest456 : public TE_RConnectionQoSStep
/**
* AllSubConnectionNotification test with multiple subconnections
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest456* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest457 : public TE_RConnectionStep
/**
* Simple test with two interfaces - one which supports the new M classes one which doesn't. Try out DataTransferredRequest for each
* @internalComponent
* @test
*/
{
public:

	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest458 : public TE_RConnectionStep
/**
* Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF not supporting extra interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest459 : public TE_RConnectionStep
/**
* Use DataSentNotification while two interfaces (one supporting the M class interface and one not) are active - make request on NIF which does support the extra interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest460 : public TE_RConnectionStep
/**
* Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF not supporting extra interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest461 : public TE_RConnectionStep
/**
* Use DataReceivedNotification while two interfaces (one supporting the M class inteface and one not) are active - make request on NIF which does support the interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest462 : public TE_RConnectionStep
/**
* Simple test with two interfaces neither of which support the extended management interface. Try out DataTransferredRequest for  each
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest463 : public TE_RConnectionStep
/**
* Test parallel use of DataTransferredRequest on two interfaces neither of which support the extended management interface
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest464 : public TE_RConnectionStep
/**
* Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest465 : public TE_RConnectionStep
/**
* Use DataSentNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest466 : public TE_RConnectionStep
/**
* Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on first interface created
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest467 : public TE_RConnectionStep
/**
* Use DataReceivedNotification while two interfaces are active (neither of which supports the extended management interface). - make request on second interface created
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest468 : public TE_RConnectionStep
/**
* Use AllSubConnectionNotification while two interfaces are active (neither of which supports the extended management interface)
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest469 : public TE_RConnectionStep
/**
* Normal stop of single existing subconnection (a primary context).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest470 : public TE_RConnectionQoSStep
/**
* Normal stop of a secondary context.
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest470* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();

};

class TE_RConnectionTest471 : public TE_RConnectionStep
/**
* Authoritative stop of single existing subconnection (a primary context).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest472 : public TE_RConnectionQoSStep
/**
* Authoritative stop of a secondary context.
* @internalComponent
* @test
*/
{
public:
	static TE_RConnectionTest472* NewL(TPtrC aName);
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest473 : public TE_RConnectionStep
/**
* Test normal stop (while using test NIF) of complete connection passing special subconnection ID for complete connection (should fail).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest474 : public TE_RConnectionStep
/**
* Test normal stop (while using dummy NIF) of complete connection passing special subconnection ID for complete connection (should fail).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest475 : public TE_RConnectionStep
/**
* Test authoritative stop (while using test NIF) of complete connection passing special subconnection ID for complete connection (should fail).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest476 : public TE_RConnectionStep
/**
* Test authoritative stop (while using dummy NIF) of complete connection passing special subconnection ID for complete connection (should fail).
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest477 : public TE_RConnectionStep
/**
* 
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL ();
};


class TE_RConnectionTest478 : public TE_RConnectionStep
/**
* 
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL ();
};


class TE_RConnectionTest479 : public TE_RConnectionStep
/**
* 
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL ();
};

class TE_RConnectionTest480 : public TE_RConnectionStep
/**
* 
* @internalComponent
* @test
*/
{
public:
	virtual enum TVerdict doTestStepL ();
};

class TE_RConnectionTest481 : public TE_RConnectionStep
{
public:
    virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest482 : public TE_RConnectionStep
{
public:
    virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest483 : public TE_RConnectionStep
{
public:
    virtual enum TVerdict doTestStepL();
};

class TE_RConnectionTest484 : public TE_RConnectionStep
{
public:
    virtual enum TVerdict doTestStepL();
};


class TE_RConnectionTest319 : public TE_RConnectionStep
{
public:
	virtual enum TVerdict doTestStepL();

};

#endif // TE_RConnectionCMM_H

