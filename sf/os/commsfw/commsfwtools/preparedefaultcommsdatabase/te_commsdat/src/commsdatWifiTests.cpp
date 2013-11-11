
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
// Test of Commsdat Wifi Security tables
// 
//


#include "commsdatWifiTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>

#include <commsdat_partner.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#include <commsdattypeinfov1_1_internal.h>

//----------------------------------------------------------------------------------------------------------------

CCommsdat197Step::CCommsdat197Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat197Step);
	}


TVerdict CCommsdat197Step::doTestStepL()
/**
 * @return TVerdict code
 * @internalComponent
*/
	{
	
   	SetTestStepResult(EFail);
	INFO_PRINTF1(_L("I am in CCommsdat197Step::doTestStepL() Store and Find values in the WLAN Table Fields"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	CCDWLANServiceExtRecord *wlanRecord = static_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(wlanRecord);

	// create Wlan table entries
	_LIT(KWLANProfile, "1234567890abcdefghijklmnopqrstuvwxyz");
	wlanRecord->SetRecordId(KCDNewRecordRequest);
	wlanRecord->iWLANProfile.SetL(KWLANProfile);
	wlanRecord->iWLANAllowUnencryptMode = 0;
	wlanRecord->iWLANEncryptionType = ECommsDatWlanEncryptionTypeAES;
	wlanRecord->StoreL(*cmdbSession);

	INFO_PRINTF1(_L("WLANEncryptionType has been set"));

	//Find the above record
	CCDWLANServiceExtRecord *wlanFindRecord = static_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(wlanFindRecord);
	wlanFindRecord->iWLANEncryptionType = ECommsDatWlanEncryptionTypeAES;
	if(wlanFindRecord->FindL(*cmdbSession))
		{
		TInt temp(0);
		CMDBField<TDesC> *wLanProfile = static_cast<CMDBField<TDesC> *>(wlanFindRecord->GetFieldByNameL(KCDTypeNameWLANProfile, temp));
  		TPtrC wlanProfileValue(*wLanProfile);
		if(wlanProfileValue == KWLANProfile)
			{
			SetTestStepResult(EPass);	
			}
		}
	CleanupStack::PopAndDestroy(wlanFindRecord);
	CleanupStack::PopAndDestroy(wlanRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------

CCommsdat198Step::CCommsdat198Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat198Step);
	}


TVerdict CCommsdat198Step::doTestStepL()
/**
 * @return TVerdict code
 * @internalComponent
*/
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("I am in CCommsdat198Step::doTestStepL() testing all the Wifi specific tables via cfg"));
	
	//create a session
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	//and with each of the tables
	INFO_PRINTF1(_L("Create Instance of each Table"));
	CCDEAPSecRecord *EAPSecRecord = static_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEAPSecRecord));
	CleanupStack::PushL(EAPSecRecord);	
	CCDTunEAPRecord *TunEAPRecord = static_cast<CCDTunEAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTunEAPRecord));
	CleanupStack::PushL(TunEAPRecord);
	CCDEAPTLSRecord *EAPTLSRecord = static_cast<CCDEAPTLSRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEAPTLSRecord));
	CleanupStack::PushL(EAPTLSRecord);
	CCDLEAPRecord *LEAPRecord = static_cast<CCDLEAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLEAPRecord));
	CleanupStack::PushL(LEAPRecord);
	
	//prime
	EAPSecRecord->SetRecordId(1);
	TunEAPRecord->SetRecordId(1);
	EAPTLSRecord->SetRecordId(1);
	LEAPRecord->SetRecordId(1);
	
	//load
	INFO_PRINTF1(_L("Load Record 1 of each table"));
	EAPSecRecord->LoadL(*cmdbSession);
	TunEAPRecord->LoadL(*cmdbSession);
	EAPTLSRecord->LoadL(*cmdbSession);
	LEAPRecord->LoadL(*cmdbSession);
	
	//set te values
	INFO_PRINTF1(_L("Test Values"));
	_LIT(KEAPTLSClientCertName, "0123abcd");
	TPtrC EAPTLSClientCertNameValue(EAPTLSRecord->iEAPTLSClientCertName);
	_LIT(KEAPSecId, "abcd0123");
	TPtrC EAPSecIdValue(EAPSecRecord->iEAPSecId);
	
	//check the value is correctly loaded via ced
	if ( (LEAPRecord->iLEAPTimeout == 10) &&
		 (EAPTLSClientCertNameValue == KEAPTLSClientCertName) &&
		 (TunEAPRecord->iTUNEAPInnerType == ECommsDatTunEapMSCHAPv2) &&
		 (EAPSecRecord->iEAPSecOuterEAPType == ECommsDatEAPSecPEAP) &&
		 (EAPSecIdValue == KEAPSecId))
		{
		//Value is correct to the Test Result is Pass
		SetTestStepResult(EPass);
		}				

	//cleanup
	CleanupStack::PopAndDestroy(LEAPRecord);
	CleanupStack::PopAndDestroy(EAPTLSRecord);
	CleanupStack::PopAndDestroy(TunEAPRecord);
	CleanupStack::PopAndDestroy(EAPSecRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------

CCommsdat199Step::CCommsdat199Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat199Step);
	}


TVerdict CCommsdat199Step::doTestStepL()
/**
 * @return TVerdict code
 * @internalComponent
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("I am in CCommsdat199Step::doTestStepL() set the security type and Test link to correct security table "));
	
	//Sets the EAP Security Outer Type to MSCHAPv2
	TCommsDatEAPSECOuterEAPType EAPSecOuterEAPTypeValue = ECommsDatEAPSecEAPMSCHAPv2;
	
 	//create a session
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
	
	//set and store the value if the record
	INFO_PRINTF1(_L("Storing WLAN Security Record"));
	CCDEAPSecRecord *EAPSecRecord= static_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEAPSecRecord));
	CleanupStack::PushL(EAPSecRecord);	
	EAPSecRecord->SetRecordId(KCDNewRecordRequest);
	EAPSecRecord->iEAPSecOuterEAPType = EAPSecOuterEAPTypeValue;
	EAPSecRecord->StoreL(*cmdbSession);
	
	//set and store the value if the record
	INFO_PRINTF1(_L("Storing WLAN Service Record"));
	CCDWLANServiceExtRecord *WLANServiceExtRecord = static_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(WLANServiceExtRecord);
	WLANServiceExtRecord->SetRecordId(KCDNewRecordRequest);
	WLANServiceExtRecord->iWLANSecData = EAPSecRecord->ElementId();
	WLANServiceExtRecord->StoreL(*cmdbSession);

	TUint recordId = WLANServiceExtRecord->RecordId(); 

	//Cleanup
	INFO_PRINTF1(_L("Cleanup"));
	CleanupStack::PopAndDestroy(WLANServiceExtRecord);
	CleanupStack::PopAndDestroy(EAPSecRecord);
	
	//Load the Wlan table
	INFO_PRINTF1(_L("Loading WLAN Record"));
	WLANServiceExtRecord = reinterpret_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(WLANServiceExtRecord);
	WLANServiceExtRecord->SetRecordId(recordId);
	WLANServiceExtRecord->LoadL(*cmdbSession);
	
	//	Linked record in action 
	INFO_PRINTF1(_L("Loading WLAN Security Record"));
	EAPSecRecord = reinterpret_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(WLANServiceExtRecord->iWLANSecData));
	CleanupStack::PushL(EAPSecRecord);
	EAPSecRecord->LoadL(*cmdbSession);

	//compare
	INFO_PRINTF1(_L("Comparing Data"));
	TCommsDatEAPSECOuterEAPType eapTypeValue = EAPSecRecord->iEAPSecOuterEAPType; 
	INFO_PRINTF3(_L("Outer EAP Type: Got %d Expected %d"), eapTypeValue, EAPSecOuterEAPTypeValue);
	TESTL(eapTypeValue == EAPSecOuterEAPTypeValue);

	//Cleanup
	INFO_PRINTF1(_L("Cleanup"));
	CleanupStack::PopAndDestroy(EAPSecRecord);
	CleanupStack::PopAndDestroy(WLANServiceExtRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	//Test has not left so Result is Pass
	SetTestStepResult(EPass);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------

CCommsdat200Step::CCommsdat200Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat200Step);
	}


TVerdict CCommsdat200Step::doTestStepL()
/**
 * @return TVerdict code
 * @internalComponent
 */
	{
    SetTestStepResult(EFail);
	INFO_PRINTF1(_L("I am in CCommsdat200Step::doTestStepL() test the Security Link fields"));
		
	//create a commsdat session
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	//load the table
	INFO_PRINTF1(_L("Retrieving WLAN Record 1"));
	CCDWLANServiceExtRecord *WLANServiceExtRecord = reinterpret_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(WLANServiceExtRecord);
	WLANServiceExtRecord->SetRecordId(1);
	WLANServiceExtRecord->LoadL(*cmdbSession);

	// Load Security Record
	INFO_PRINTF1(_L("Retrieving EAP Security"));
	CCDEAPSecRecord *EAPSecRecord = reinterpret_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(WLANServiceExtRecord->iWLANSecData));
	CleanupStack::PushL(EAPSecRecord);
	EAPSecRecord->LoadL(*cmdbSession);
	// Compare link with value
	TESTL(EAPSecRecord->iEAPSecOuterEAPType == ECommsDatEAPSecPEAP);	

	// Load Tunnelled Record
	INFO_PRINTF1(_L("Retrieving Tunnelled EAP Security"));
	CCDTunEAPRecord *TunEAPSecRecord = reinterpret_cast<CCDTunEAPRecord*>(CCDRecordBase::RecordFactoryL(EAPSecRecord->iEAPSecData));
	CleanupStack::PushL(TunEAPSecRecord);
	TunEAPSecRecord->LoadL(*cmdbSession);
	TESTL(TunEAPSecRecord->iTUNEAPInnerType == ECommsDatTunEapMSCHAPv2);	

	INFO_PRINTF1(_L("Cleanup"));
	CleanupStack::PopAndDestroy(TunEAPSecRecord);

	CleanupStack::PopAndDestroy(EAPSecRecord);

	CleanupStack::PopAndDestroy(WLANServiceExtRecord);

	//load the table
	INFO_PRINTF1(_L("Retrieving WLAN Record 2"));
	WLANServiceExtRecord = reinterpret_cast<CCDWLANServiceExtRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord));
	CleanupStack::PushL(WLANServiceExtRecord);
	WLANServiceExtRecord->SetRecordId(2);
	WLANServiceExtRecord->LoadL(*cmdbSession);	

	// Load Security Record
	INFO_PRINTF1(_L("Retrieving EAP Security"));
	EAPSecRecord= reinterpret_cast<CCDEAPSecRecord*>(CCDRecordBase::RecordFactoryL(WLANServiceExtRecord->iWLANSecData));
	CleanupStack::PushL(EAPSecRecord);
	EAPSecRecord->LoadL(*cmdbSession);
	// Compare link with value
	TESTL(EAPSecRecord->iEAPSecOuterEAPType == ECommsDatEAPSecEAPTLS);	

	// Load Tunnelled Record
	INFO_PRINTF1(_L("Retrieving TLS EAP Security"));
	CCDEAPTLSRecord *TLSEAPSecRecord = reinterpret_cast<CCDEAPTLSRecord*>(CCDRecordBase::RecordFactoryL(EAPSecRecord->iEAPSecData));
	CleanupStack::PushL(TLSEAPSecRecord);
	TLSEAPSecRecord->LoadL(*cmdbSession);
	
	TPtrC certName (TLSEAPSecRecord->iEAPTLSClientCertName);
	TESTL(certName.Compare(_L("0123abcd")) == 0);	

	INFO_PRINTF1(_L("Cleanup"));
	CleanupStack::PopAndDestroy(TLSEAPSecRecord);

	CleanupStack::PopAndDestroy(EAPSecRecord);

	CleanupStack::PopAndDestroy(WLANServiceExtRecord);

	INFO_PRINTF1(_L("Test Complete"));
	CleanupStack::PopAndDestroy(cmdbSession);

	//Test has not left so Result is Pass
	SetTestStepResult(EPass);
	return TestStepResult();
	}
//----------------------------------------------------------------------------------------------------------------



