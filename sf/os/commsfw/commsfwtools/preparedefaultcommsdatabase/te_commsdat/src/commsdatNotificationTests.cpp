// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommsdat150Step
// CCommsdat151Step
// CCommsdat152Step
// CCommsdat153Step
// CCommsdat154Step
// CCommsdat155Step
// CCommsdat156Step
// CCommsdat157Step
// CCommsdat158Step
// CCommsdat159Step
// CCommsdat160Step
// CCommsdat161Step
// CCommsdat162Step
// CCommsdat163Step
// CCommsdat164Step
// CCommsdat165Step
// NOTE:
// Ensure that PublishProperties() is called in any tests that need it - relying on
// a previous test having called it and setup the properties for subsequent tests will
// mean that the subsequent tests will fail if ever run in isolation.
// 
//

/**
 @file commsdatNotificationTests.cpp
*/
#include "commsdatNotificationTests.h"
#include "Te_commsdatSuiteDefs.h"
#include "notificationtester.h"
#include "CommsDatInternalDefs.h"
#include <e32test.h>
#include <d32comm.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

const TInt KNetworkWeighting = 10;

CCommsdat150Step::CCommsdat150Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat150Step);                                                                                           
	}

TVerdict CCommsdat150Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface during RollBack Transaction; must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat150Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
    cmdbSession->PublishProperties();
    
    cmdbSession->OpenTransactionL();
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
    TBool found = record->FindL(*cmdbSession);
	
	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
    if (found)
        {
        _LIT(KThreadName, "TestThread150");
        CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
        CleanupStack::PushL(tester);    
	    TRequestStatus notified;
	    tester->LogOn(notified);

	    TPtrC testBearer(_L("Test Bearer"));
        record->iBearerType.SetMaxLengthL(testBearer.Length());
	    record->iBearerType = testBearer;
	    
		record->ModifyL(*cmdbSession);		
		
	    if(tester->IsAdded())
	    	{
	    	INFO_PRINTF1(_L("added to active scheduler"));
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("not added to active scheduler"));
	    	}
	    	
		cmdbSession->RollbackTransactionL();

	    User::WaitForRequest (notified);
   	    TInt testEvtType = 0;
	    RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

	    if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ERollback)
	    	{
	    	SetTestStepResult(EPass);	
	    	}

	    cmdbSession->Close();
	    CleanupStack::PopAndDestroy(tester);
        }
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat151Step::CCommsdat151Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat151Step);                                                                                           
	}

TVerdict CCommsdat151Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test Notification after doing a Modify and Commit.
 * CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat151Step::doTestStepL() in the file commsdat151Step.cpp"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	cmdbSession->OpenTransactionL();

	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	record->iNetworkWeighting = KNetworkWeighting;

	INFO_PRINTF1(_L("Finding IAP record..."));

	TBool found = record->FindL(*cmdbSession);

	INFO_PRINTF3(_L("FindL result for record %08x was %b, should be 1"), record->ElementId(), found);

	if (found)
		{
		_LIT(KThreadName,"TestThread151");
		CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
		CleanupStack::PushL(tester);

		TRequestStatus notified;
		tester->LogOn(notified);

		INFO_PRINTF1(_L("Set NotificationTester"));

		TPtrC testService(_L("Test Service"));
		record->iServiceType.SetMaxLengthL(testService.Length());
		record->iServiceType = testService;

		record->ModifyL(*cmdbSession);	    

		if(tester->IsAdded())
			{
			INFO_PRINTF1(_L("added to active scheduler"));
			}
		else
			{
			INFO_PRINTF1(_L("not added to active scheduler"));
			}

		cmdbSession->CommitTransactionL();

		User::WaitForRequest (notified);

		TInt testEvtType = 0;
		RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

		if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ECommit)
			{
			SetTestStepResult(EPass); 	
			}

		cmdbSession->Close();	    	
		CleanupStack::PopAndDestroy(tester); 
        }
	else
		{
		cmdbSession->CommitTransactionL();		
		cmdbSession->Close();
		}
        
	CleanupStack::PopAndDestroy(record);   
	CleanupStack::PopAndDestroy(cmdbSession);
    
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat152Step::CCommsdat152Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat152Step);                                                                                           
	}


TVerdict CCommsdat152Step::doTestStepL()
/**
 * @return - TVerdict code
 * test CancelAllNotifications when there are no notifications
 * CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat152Step::doTestStepL() and trying to test CancelAllNotifications"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL();
	cmdbSession->CancelAllNotifications();
	cmdbSession->RollbackTransactionL();
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	SetTestStepResult(EPass);
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat153Step::CCommsdat153Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat153Step);                                                                                           
	}
	
TVerdict CCommsdat153Step::doTestStepL()
/**
 * @return - TVerdict code
 * test Cancelling a Notification
 * CED -i Z:\testdata\configs\te_commsdat_modembearer_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat153Step::doTestStepL() and trying to test CancelAllNotifications"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL();
	
	CCDModemBearerRecord *record = static_cast<CCDModemBearerRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(record);

	record->iDataBits = EData8;
	
	TBool found = record->FindL(*cmdbSession);
	
	if(found)
		{
		_LIT(KThreadName, "TestThread153");
		CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
        CleanupStack::PushL(tester);
       	
  	    TRequestStatus notified;
	    tester->LogOn(notified);

        INFO_PRINTF1(_L("Set NotificationTester"));  
        record->iDataBits = EData7;
        
	    record->ModifyL(*cmdbSession);
	     		
   	    if(tester->IsAdded())
	    	{
	    	INFO_PRINTF1(_L("added to active scheduler"));
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("not added to active scheduler"));
	    	}

	   	tester->CancelNotification(notified);
	   	
	   	User::WaitForRequest (notified);

        if(tester->iStatus == KErrCancel)
	    	{
	    	SetTestStepResult(EPass);
	    	}
	    	
	    cmdbSession->Close();	
	    
        CleanupStack::PopAndDestroy(tester);
		}
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat154Step::CCommsdat154Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat154Step);                                                                                           
	}

TVerdict CCommsdat154Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface while Closing a session; must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat154Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	cmdbSession->OpenTransactionL();

	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 

	record->iNetworkWeighting = KNetworkWeighting;
	TBool found = record->FindL(*cmdbSession);

	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
	if (found)
		{
		_LIT(KThreadName, "TestThread154");
		CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
		CleanupStack::PushL(tester);    
		TRequestStatus notified;
		tester->LogOn(notified);

		if(tester->IsAdded())
			{
			INFO_PRINTF1(_L("added to active scheduler"));
			}
		else
			{
			INFO_PRINTF1(_L("not added to active scheduler"));
			}

		cmdbSession->Close();

		User::WaitForRequest (notified);

		TInt testEvtType = 0;
		RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

		if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::EClose)
			{
			SetTestStepResult(EPass);	
			}
		CleanupStack::PopAndDestroy(tester);
		}

	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat155Step::CCommsdat155Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat155Step);                                                                                           
	}


TVerdict CCommsdat155Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test notification on linked records with modifying records
 * CED -i Z:\testdata\configs\te_commsdat_wap_access_sms.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing notification over linked records CCommsdat155Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	cmdbSession->OpenTransactionL();

	CCDWAPAccessPointRecord* apRecord = static_cast<CCDWAPAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
	
	apRecord->SetRecordId(41);	
	
	TPtrC name(_L("Voda"));
	apRecord->iWAPCurrentBearer.SetMaxLengthL(name.Length());
	apRecord->iWAPCurrentBearer = name;
	
	apRecord->LoadL(*cmdbSession);

	_LIT(KThreadName, "TestThread155");
	CNotificationTester *tester = CNotificationTester::NewL(KThreadName,cmdbSession, apRecord);
	CleanupStack::PushL(tester);
	
	TRequestStatus notified;
	tester->LogOn(notified);
	
	CCDWAPSMSBearerRecord* sbRecord = static_cast<CCDWAPSMSBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPSMSBearerRecord));
	CleanupStack::PushL(sbRecord);
	
	sbRecord->SetRecordId(1);
	sbRecord->iWAPAccessPointId = apRecord->RecordId();
	sbRecord->iWAPAccessPointId.iLinkedRecord = apRecord;
	sbRecord->LoadL(*cmdbSession);
	
	CMDBElement* linkToAPRecord = sbRecord->iWAPAccessPointId.iLinkedRecord;
	
	if(tester->IsAdded())
	    {
	    INFO_PRINTF1(_L("added to active scheduler"));
	    }
	else
	  	{
	   	INFO_PRINTF1(_L("not added to active scheduler"));
	   	}	
	
	_LIT(KWapBearer, "ArbitraryName");
	if(linkToAPRecord != NULL)
		{
		static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->iWAPCurrentBearer.SetL(KWapBearer);
		static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->ModifyL(*cmdbSession);
		cmdbSession->CommitTransactionL();
		}
	   	
	User::WaitForRequest(notified);
	
	TInt testEvtType = 0;
	RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);
	
	if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ECommit)
	   	{
		 SetTestStepResult(EPass); 	
	   	}
	   	
	cmdbSession->Close();	   	
	CleanupStack::PopAndDestroy(sbRecord);
	CleanupStack::PopAndDestroy(tester);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat156Step::CCommsdat156Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat156Step);                                                                                           
	}

TVerdict CCommsdat156Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test notification on linked records with modifying links
 * CED -i Z:\testdata\configs\te_commsdat_wap_access_sms.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing notification over linked records CCommsdat156Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	cmdbSession->OpenTransactionL();
	
	CCDWAPAccessPointRecord* apRecord = static_cast<CCDWAPAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
	
	apRecord->SetRecordId(41);	

	TPtrC name(_L("Voda"));
	apRecord->iWAPCurrentBearer.SetMaxLengthL(name.Length());
	apRecord->iWAPCurrentBearer = name;
	apRecord->LoadL(*cmdbSession);

   	_LIT(KThreadName, "TestThread156");
	CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, apRecord);
	CleanupStack::PushL(tester);
	
    TRequestStatus notified;
	tester->LogOn(notified);
	
	CCDWAPSMSBearerRecord* sbRecord = static_cast<CCDWAPSMSBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPSMSBearerRecord));
	CleanupStack::PushL(sbRecord);
	
	sbRecord->SetRecordId(1);
	sbRecord->iWAPAccessPointId = apRecord->RecordId();
	sbRecord->iWAPAccessPointId.iLinkedRecord = apRecord;
    sbRecord->LoadL(*cmdbSession);   
	
	CMDBElement* linkToAPRecord = sbRecord->iWAPAccessPointId.iLinkedRecord;
	
	if(tester->IsAdded())
	    {
	    INFO_PRINTF1(_L("added to active scheduler"));
	    }
	else
	  	{
	   	INFO_PRINTF1(_L("not added to active scheduler"));
	   	}	
  
	_LIT(KWapBearer, "ArbitraryName");
	if(linkToAPRecord != NULL)
		{
		static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->iWAPCurrentBearer.SetL(KWapBearer);
		static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->iWAPCurrentBearer.ModifyL(*cmdbSession);
		cmdbSession->CommitTransactionL();		
		}		
	   	
	User::WaitForRequest(notified);
	
	TInt testEvtType = 0;
	RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);
	
	if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ECommit)
	   	{
		 SetTestStepResult(EPass); 	
	   	}
	   	
	cmdbSession->Close();	   		   	
	CleanupStack::PopAndDestroy(sbRecord);
	CleanupStack::PopAndDestroy(tester);
  	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}


//----------------------------------------------------------------------------------------------------------------
CCommsdat157Step::CCommsdat157Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat157Step);                                                                                           
	}


TVerdict CCommsdat157Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test notification on linked records with deleting records
 * CED -i Z:\testdata\configs\te_commsdat_wap_access_sms.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing notification over linked records CCommsdat157Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	CCDWAPAccessPointRecord* apRecord = static_cast<CCDWAPAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
	
	apRecord->SetRecordId(41);

	TPtrC name(_L("Voda"));
	apRecord->iWAPCurrentBearer.SetMaxLengthL(name.Length());
	apRecord->iWAPCurrentBearer = name;	
	
	apRecord->LoadL(*cmdbSession);
	
	_LIT(KThreadName, "TestThread157");
	CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, apRecord);
	CleanupStack::PushL(tester);
	
    TRequestStatus notified;
	tester->LogOn(notified);
	
	CCDWAPSMSBearerRecord* sbRecord = static_cast<CCDWAPSMSBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPSMSBearerRecord));
	CleanupStack::PushL(sbRecord);
	
	sbRecord->SetRecordId(1);
	sbRecord->iWAPAccessPointId = apRecord->RecordId();
	sbRecord->iWAPAccessPointId.iLinkedRecord = apRecord;

	TBool found = sbRecord->FindL(*cmdbSession);	
	
	if(tester->IsAdded())
	    {
	    INFO_PRINTF1(_L("added to active scheduler"));
	    }
	else
	  	{
	   	INFO_PRINTF1(_L("not added to active scheduler"));
	   	}	
	
	if(found)
		{
		CMDBElement* linkToAPRecord = sbRecord->iWAPAccessPointId.iLinkedRecord;
		if(linkToAPRecord != NULL)
			{
			static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->DeleteL(*cmdbSession);	
			}
		}
	
	User::WaitForRequest(notified);
	
	TInt testEvtType = 0;
	RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);
	
	if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ECommit)
	    {
	   	SetTestStepResult(EPass); 	
	    }

	cmdbSession->Close();		    
	
	CleanupStack::PopAndDestroy(sbRecord);
	CleanupStack::PopAndDestroy(tester);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat158Step::CCommsdat158Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat158Step);                                                                                           
	}

TVerdict CCommsdat158Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test notification on linked records with deleting fields
 * CED -i Z:\testdata\configs\te_commsdat_wap_access_sms.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing notification over linked records CCommsdat158Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	CCDWAPAccessPointRecord* apRecord = static_cast<CCDWAPAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord));
	
	apRecord->SetRecordId(41);	

	TPtrC name(_L("Voda"));
	apRecord->iWAPCurrentBearer.SetMaxLengthL(name.Length());
	apRecord->iWAPCurrentBearer = name;	
	
	apRecord->LoadL(*cmdbSession);
	
	_LIT(KThreadName, "TestThread158");
	CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, apRecord);
	CleanupStack::PushL(tester);
	
    TRequestStatus notified;
	tester->LogOn(notified);
	
	CCDWAPSMSBearerRecord* sbRecord = static_cast<CCDWAPSMSBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPSMSBearerRecord));
	CleanupStack::PushL(sbRecord);
	
	sbRecord->SetRecordId(1);
	sbRecord->iWAPAccessPointId = apRecord->RecordId();
	sbRecord->iWAPAccessPointId.iLinkedRecord = apRecord;
	
	if(tester->IsAdded())
	    {
	    INFO_PRINTF1(_L("added to active scheduler"));
	    }
	else
	  	{
	   	INFO_PRINTF1(_L("not added to active scheduler"));
	   	}	
	
	TBool found = sbRecord->FindL(*cmdbSession);
	if(found )
		{
		CMDBElement* linkToAPRecord = sbRecord->iWAPAccessPointId.iLinkedRecord;
		if(linkToAPRecord != NULL)
			{
			static_cast<CCDWAPAccessPointRecord *>(linkToAPRecord)->iWAPCurrentBearer.DeleteL(*cmdbSession);	
			}
		}
	
	User::WaitForRequest(notified);
	
	TInt testEvtType = 0;
	RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);
	
	if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ECommit)
	   	{
		SetTestStepResult(EPass); 	
	   	}
	
	cmdbSession->Close();	   	

	CleanupStack::PopAndDestroy(sbRecord);
	CleanupStack::PopAndDestroy(tester);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//---------------------------------------------------------------------------------------------------------------
CCommsdat159Step::CCommsdat159Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat159Step);                                                                                           
	}


TVerdict CCommsdat159Step::doTestStepL()
/**
 * @return - TVerdict code
 * test CancelAllNotifications when there are no notifications
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing transaction behaviour in CCommsdat159Step::doTestStepL() "));
	
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TRAPD(rollbackErr, cmdbSession->RollbackTransactionL()); // should give KErrAlreadyInTransaction
	INFO_PRINTF2(_L("Rollback without transaction returns : %d"), rollbackErr);
	if(rollbackErr == KErrNotFound)
		{
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat160Step::CCommsdat160Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat160Step);                                                                                           
	}


TVerdict CCommsdat160Step::doTestStepL()
/**
 * @return - TVerdict code
 * .tests type operator RECORD_TYPE with different data types. 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat160Step::doTestStepL() in the file commsdat160Step.cpp"));

	//Initialisation of ptrIapRecord and ptrService  
	
    CCDIAPRecord* ptrIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
    CleanupStack::PushL(ptrIapRecord);
    CCDServiceRecordBase* ptrService = static_cast<CCDServiceRecordBase*>(CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord));   
    
    TInt testVal = 10;        
    
    ptrService->iServiceEnableLlmnr = ETrue;//ptrService is used to access  iServiceEnableLlmnr of type TBool    
    ptrIapRecord->iService.iLinkedRecord  = ptrService;//ptrIapRecord is used to access  iService of type CCDServiceRecordBase
    ptrIapRecord->iService = testVal; // ptrService is used to access SetRecordId of type TInt
    ptrService = NULL;
  
   
   	//Checking if type operators are working
   	TInt serviceId = ptrIapRecord->iService; 
    TESTL(serviceId == testVal);           //accessing integer value
       
   
    CCDServiceRecordBase* ptrIapService = ptrIapRecord->iService; 
    if(ptrIapService->iServiceEnableLlmnr)
    	{
    	SetTestStepResult(EPass);            //accesing Boolean value  
    	}
    	
    CleanupStack::PopAndDestroy(ptrIapRecord);
	
	
	
    return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

CCommsdat161Step::CCommsdat161Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat161Step);                                                                                           
	}

TVerdict CCommsdat161Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface. Request notification using one session instance and change  database with another one. Must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat161Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties (doing it on one session is enough)
	cmdbSession->PublishProperties();

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession2 = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession2 = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession2); 	
    
    cmdbSession->OpenTransactionL();
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
    TBool found = record->FindL(*cmdbSession);
	
	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
    if (found)
        {
        _LIT(KThreadName, "TestThread161");
        CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession2, record);
        CleanupStack::PushL(tester);    
	    TRequestStatus notified;
	    tester->LogOn(notified);

	    TPtrC testBearer(_L("Test Bearer"));
        record->iBearerType.SetMaxLengthL(testBearer.Length());
	    record->iBearerType = testBearer;
	    
		record->ModifyL(*cmdbSession);        
		
	    if(tester->IsAdded())
	    	{
	    	INFO_PRINTF1(_L("added to active scheduler"));
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("not added to active scheduler"));
	    	}
	    	
		cmdbSession->RollbackTransactionL();	    	    	

	    User::WaitForRequest (notified);
   	    TInt testEvtType = 0;
	    RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

	    if(tester->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ERollback)
	    	{
	    	SetTestStepResult(EPass);	
	    	}

	    cmdbSession->Close();
	    CleanupStack::PopAndDestroy(tester);
        }
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession2);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

CCommsdat162Step::CCommsdat162Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat162Step);
	}

TVerdict CCommsdat162Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface. Request notification by several different clients. Check that all are completed. Must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat162Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
	cmdbSession->PublishProperties();

    cmdbSession->OpenTransactionL();
	
	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
    TBool found = record->FindL(*cmdbSession);
	
	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
    if (found)
        {
        _LIT(KThreadName, "TestThread162");
        CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
        CleanupStack::PushL(tester);    
	    TRequestStatus notified;
	    tester->LogOn(notified);
	    
        _LIT(KThreadName1, "TestThread162_1");
        CNotificationTester *tester1 = CNotificationTester::NewL(KThreadName1, cmdbSession, record);
        CleanupStack::PushL(tester1);    
	    TRequestStatus notified1;
	    tester1->LogOn(notified1);

        _LIT(KThreadName2, "TestThread162_2");
        CNotificationTester *tester2 = CNotificationTester::NewL(KThreadName2, cmdbSession, record);
        CleanupStack::PushL(tester2);    
	    TRequestStatus notified2;
	    tester2->LogOn(notified2);
	    

	    TPtrC testBearer(_L("Test Bearer"));
        record->iBearerType.SetMaxLengthL(testBearer.Length());
	    record->iBearerType = testBearer;
	    
		record->ModifyL(*cmdbSession);			    
		
	    if(tester->IsAdded() && tester1->IsAdded() && tester2->IsAdded())
	    	{
	    	INFO_PRINTF1(_L("added to active scheduler"));
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("not added to active scheduler"));
	    	}
	    	
		cmdbSession->RollbackTransactionL();	    	

	    User::WaitForRequest (notified);
	    User::WaitForRequest (notified1);
	    User::WaitForRequest (notified2);
	    
   	    TInt testEvtType = 0;
	    RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

	    if(tester->iStatus == KErrNone && tester1->iStatus == KErrNone && tester2->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ERollback)
	    	{
	    	SetTestStepResult(EPass);	
	    	}

	    cmdbSession->Close();
	    CleanupStack::PopAndDestroy(tester2);
	    CleanupStack::PopAndDestroy(tester1);
	    CleanupStack::PopAndDestroy(tester);
        }
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

CCommsdat163Step::CCommsdat163Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat163Step);
	}

TVerdict CCommsdat163Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface. Request notification by several different clients and then cancel prticular one. Check that all are completed. Must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 *
 * NOTE: this test seems to require step 162 to have been run.
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat163Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
	cmdbSession->PublishProperties();

	cmdbSession->OpenTransactionL();

	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
	TBool found = record->FindL(*cmdbSession);

	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
	if (found)
		{
		_LIT(KThreadName, "TestThread163");
		CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
		CleanupStack::PushL(tester);    
		TRequestStatus notified;
		tester->LogOn(notified);

		_LIT(KThreadName1, "TestThread163_1");
		CNotificationTester *tester1 = CNotificationTester::NewL(KThreadName1, cmdbSession, record);
		CleanupStack::PushL(tester1);    
		TRequestStatus notified1;
		tester1->LogOn(notified1);

		_LIT(KThreadName2, "TestThread163_2");
		CNotificationTester *tester2 = CNotificationTester::NewL(KThreadName2, cmdbSession, record);
		CleanupStack::PushL(tester2);    
		TRequestStatus notified2;
		tester2->LogOn(notified2);


		TPtrC testBearer(_L("Test Bearer"));
		record->iBearerType.SetMaxLengthL(testBearer.Length());
		record->iBearerType = testBearer;

		record->ModifyL(*cmdbSession);			    

		if(tester->IsAdded() && tester1->IsAdded() && tester2->IsAdded())
			{
			INFO_PRINTF1(_L("added to active scheduler"));
			}
		else
			{
			INFO_PRINTF1(_L("not added to active scheduler"));
			}

		tester1->CancelNotification(notified1);

		cmdbSession->RollbackTransactionL();	    	

		User::WaitForRequest (notified);
		User::WaitForRequest (notified1);
		User::WaitForRequest (notified2);

		TInt testEvtType = 0;
		RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

		if(tester->iStatus == KErrNone && tester1->iStatus == KErrCancel && tester2->iStatus == KErrNone && testEvtType == TCDNotifiableEvent::ERollback)
			{
			SetTestStepResult(EPass);	
			}

		cmdbSession->Close();
		CleanupStack::PopAndDestroy(tester2);
		CleanupStack::PopAndDestroy(tester1);
		CleanupStack::PopAndDestroy(tester);
		}

	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

CCommsdat164Step::CCommsdat164Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat164Step);
	}

TVerdict CCommsdat164Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface. Request notification by several different clients and then cancel all. Check that all are completed. Must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 *
 * NOTE: this test seems to require step 163 to have been performed.
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat164Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

    cmdbSession->OpenTransactionL();

	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
    TBool found = record->FindL(*cmdbSession);
	
	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
    if (found)
        {
        _LIT(KThreadName, "TestThread164");
        CNotificationTester *tester = CNotificationTester::NewL(KThreadName, cmdbSession, record);
        CleanupStack::PushL(tester);    
	    TRequestStatus notified;
	    tester->LogOn(notified);
	    
        _LIT(KThreadName1, "TestThread164_1");
        CNotificationTester *tester1 = CNotificationTester::NewL(KThreadName1, cmdbSession, record);
        CleanupStack::PushL(tester1);    
	    TRequestStatus notified1;
	    tester1->LogOn(notified1);

        _LIT(KThreadName2, "TestThread164_2");
        CNotificationTester *tester2 = CNotificationTester::NewL(KThreadName2, cmdbSession, record);
        CleanupStack::PushL(tester2);    
	    TRequestStatus notified2;
	    tester2->LogOn(notified2);
	    

	    TPtrC testBearer(_L("Test Bearer"));
        record->iBearerType.SetMaxLengthL(testBearer.Length());
	    record->iBearerType = testBearer;
	    
		record->ModifyL(*cmdbSession);			    
		
	    if(tester->IsAdded() && tester1->IsAdded() && tester2->IsAdded())
	    	{
	    	INFO_PRINTF1(_L("added to active scheduler"));
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("not added to active scheduler"));
	    	}
	    	
	    cmdbSession->CancelAllNotifications();
	    
	    User::WaitForRequest (notified);
	    User::WaitForRequest (notified1);
	    User::WaitForRequest (notified2);
	    
   	    TInt testEvtType = 0;
	    RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

	    if(tester->iStatus == KErrCancel && tester1->iStatus == KErrCancel && tester2->iStatus == KErrCancel && testEvtType == TCDNotifiableEvent::EClose)
	    	{
	    	SetTestStepResult(EPass);	
	    	}

	    cmdbSession->Close();
	    CleanupStack::PopAndDestroy(tester2);
	    CleanupStack::PopAndDestroy(tester1);
	    CleanupStack::PopAndDestroy(tester);
        }
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------

CCommsdat165Step::CCommsdat165Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat165Step);
	}

TVerdict CCommsdat165Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * Testing notification interface. Request notification by a client twice or more. Must do CED -i Z:\testdata\configs\te_commsdat_iap_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("I am in CCommsdat165Step::doTestStepL(). Testing notification interface..."));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	// need to define P&S properties	
	cmdbSession->PublishProperties();

    cmdbSession->OpenTransactionL();

	CCDIAPRecord *record = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(record); 
	
	record->iNetworkWeighting = KNetworkWeighting;
    TBool found = record->FindL(*cmdbSession);
	
	INFO_PRINTF3(_L("FindL returned %b for record %08x"), found, record->ElementId());
    
    if (found)
        {
	    TRequestStatus notified;
	    
		record->RequestNotification(*cmdbSession, notified);
		record->RequestNotification(*cmdbSession, notified);
		record->RequestNotification(*cmdbSession, notified);	    

	    TPtrC testBearer(_L("Test Bearer"));
        record->iBearerType.SetMaxLengthL(testBearer.Length());
	    record->iBearerType = testBearer;
	    
		record->ModifyL(*cmdbSession);		
	    	
		cmdbSession->RollbackTransactionL();	    	
		
	    User::WaitForRequest (notified);
	    
   	    TInt testEvtType = 0;
	    RProperty::Get(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, testEvtType);

	    if(notified == KErrNone && testEvtType == TCDNotifiableEvent::ERollback)
	    	{
	    	SetTestStepResult(EPass);	
	    	}

	    cmdbSession->Close();
        }
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}

//-------------------------------------------------------------------------------------------------------------------------------------------
