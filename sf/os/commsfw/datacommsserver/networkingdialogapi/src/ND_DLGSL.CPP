// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <commsdattypesv1_1.h>
#include <commsdattypesv1_1_partner.h>
#include <metadatabase.h>
using namespace CommsDat;
#include <cdbpreftable.h>
#include <agentdialog.h>
#include "ND_DLGSL.H"
#include <es_ini.h>
#include <commsdattypeinfov1_1_internal.h>

/** Literal accepts the String */
_LIT(KSelectTitle,"Select %S");

/** Literal accepts the String */
_LIT(KSelectPrompt,"%S");

/** Literal accepts the Integer */
_LIT(KWarnPrompt,"Connection failed with %d");

/** Literal Specifies the New Connection */
_LIT(KWarnPromptNoError,"New connection");

/** Literal Conain the String trying to connect to server */
_LIT(KAttemptPrompt,"Attempt to connect to %S?");

/** Literal prompts to Select IAP */
_LIT(KIAP,"IAP");
/** Literal prompts to Select IAP */
_LIT(KAP, "Access Point");

/** Literal Specifies QoSWarning */
_LIT(KQoSWarnTitle,"The connection quality is lower than required");

/** Literal Specifies Disconnect option */
_LIT(KQoSWarnPrompt,"Do you wish to disconnect?");

namespace ND_DLDSL
	{
const TInt KOneSecond=1000000;			//< TimeInterval of OneSecond 
const TInt KTimeInterval=KOneSecond;	//< TimeInterval to start the CPctTimer
const TInt KNotifierLength=16;			//< NotifierLength
const TText KFullStop='.';				//< Constant to hold FullStop
	}

CDialogBase::CDialogBase(CNetDialDialogSession* aSession, TInt aPriority)
	: CActive(aPriority),
	iSession(aSession),
	iAgentDialogInput(iAgentDialogInputBuf()),
	iAgentDialogOutput(iAgentDialogOutputBuf())
/**
CDialogBase Constructor.
*/
	{
	__FLOG_OPEN(KDlgSvrLogSubsys, KDlgSvrLogComponent);
	}

void CDialogBase::ConstructL()
/**
Completes construction of the CDialogBase object.
*/
	{
	iNotifier = new(ELeave) RNotifier();
	User::LeaveIfError(iNotifier->Connect());
	
	_LIT(KAgentDialogIniFileName, "agentdialog.ini");
	_LIT(KNotifierUidTag, "NotifierUid");
	CESockIniData* cfgFile = NULL;
	
	// coverity[SYMBIAN.CLEANUP_STACK] coverity [alloc_fn]
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [assign]
	TRAPD(err, cfgFile = CESockIniData::NewL(KAgentDialogIniFileName));
	// coverity[SYMBIAN.CLEANUP_STACK] coverity [leave_without_push]
	__FLOG_1(_L("Agent dialog config file for  - returned %d"),err);

        if (err == KErrNone)
		{
		// the file was opened correctly
		TInt val = 0;
		if (cfgFile->FindVar(KNotifierUidTag, val))
			{
			// the value was found, build a TUid to assign to the member variable
			iUidAgentDialogNotifier = TUid::Uid(val);
			__FLOG(_L("Uid Found in ini file"));
			}
		else
			{
			// value was not found, use the default UID for agent dialog notifiers
			iUidAgentDialogNotifier = KUidAgentDialogNotifier;
			}
		delete cfgFile;	
		}
	else
		{
		// the ini file was not found so use the default UID for agent dialog notifiers
		iUidAgentDialogNotifier = KUidAgentDialogNotifier;
		}
	__FLOG_1(_L("Uid Loaded ===>%x"),iUidAgentDialogNotifier.iUid);	
	}

CDialogBase::~CDialogBase()
/**
Destructor.
*/
	{
	if (iNotifier!=NULL)
		{
		iNotifier->Close();
		delete iNotifier;
		}
	delete iCallBack;
	__FLOG_CLOSE;
	}	
	

void CDialogBase::StartNotifierAndGetResponse()
/**
Utility function to start the dialog notifier
 */
	{
	__ASSERT_DEBUG(iNotifier!=NULL, User::Invariant());

	__FLOG_1(_L("CDialogBase::StartNotifierAndGetResponse - Openning Dialog - Uid Loaded ===>%x "),
		iUidAgentDialogNotifier.iUid);
	iNotifier->StartNotifierAndGetResponse( iStatus, iUidAgentDialogNotifier, iAgentDialogInputBuf, iAgentDialogOutputBuf );
	SetActive();
	}

void CDialogBase::CancelNotifier()
/**
 Utility function to cancel the dialog notifier
*/
	{
	__ASSERT_DEBUG(iNotifier!=NULL, User::Invariant());
	iCancelFlag=ETrue;
	iNotifier->CancelNotifier(iUidAgentDialogNotifier);
	}

void CDialogBase::DoCancel()
/**
 Cancel the outstanding request - most of the derived classes have a
 common DoCancel method, so it seems reasonable to factor it here
*/
	{
	CancelNotifier();
	}





CIAPSelection* CIAPSelection::NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
/**
IAP Selection

@param aDb pointer to communications database.
@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to IAPSelection object.
*/
	{
	CIAPSelection* r=new(ELeave) CIAPSelection(aDb,aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CIAPSelection::CIAPSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority),
	iDb(aDb)

	
/**
CIAPSelection Constructor
*/
	{}

void CIAPSelection::ConstructL()
/**
Completes construction of the CIAPSelection object.

@see CDialogBase::ConstructL().
*/
	{
	CDialogBase::ConstructL();
	CActiveScheduler::Add(this);
		InitRecordSetL();
	}

CIAPSelection::~CIAPSelection()
/**
CIAPSelection Destructor.
*/
	{
	Cancel();
	delete iTable;
	}
	
void CIAPSelection::InitRecordSetL()
	{
	delete iTable;
	iTable = NULL;
	iTable = new (ELeave)CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
	//CCDConnectionPrefsRecord* ptrTemplate = new (ELeave) CCDConnectionPrefsRecord();
	CCDConnectionPrefsRecord* ptrTemplate = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(ptrTemplate);
	User::LeaveIfError((iTable->iRecords).Append(ptrTemplate));
	CleanupStack::Pop(ptrTemplate);
	iRecordIndex = 0;
	}
	
void CIAPSelection::GetIAPL(TUint32& aIAPId, const TConnectionPrefs& aPrefs, TInt aError, TCallBack aCallBack)
/**
Gets the IAP.

@param aIAPId The id of the IAP service.
@param aPrefs Specifies the rank and desired direction of the connection and bearer.
@param aError Any error code, or KErrNone if no error.
@param aCallBack Encapsulates a general call-back function.
*/
	{
	iCancelFlag = EFalse;
	iCallBack = new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	iIAPId = &aIAPId;
	iRecordsPresent = ETrue;

	if(iRecordIndex != 0)
		{
		InitRecordSetL();
		}

	static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iBearerSet = aPrefs.iBearerSet;
	static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iDirection = aPrefs.iDirection;

	if(iTable->FindL(*iDb))
		{
		CCDIAPRecord* ptrIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
		CleanupStack::PushL(ptrIapRecord);
		
		CCDConnectionPrefsRecord* connpref = static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex]);
		ptrIapRecord->SetRecordId(connpref->iDefaultIAP);
		ptrIapRecord->LoadL(*iDb);
		connpref->iDefaultIAP.iLinkedRecord = ptrIapRecord;
		CleanupStack::Pop(ptrIapRecord);
		SelectL(aError);
		}
	else
	// else case is common part for failure
		{
		if (aError!=KErrNone)
			{
			iAgentDialogInput.iTitle.Format(KWarnPrompt,aError);
			}
		_LIT(noService,"No IAP Records Found");
		iAgentDialogInput.iTitle.Append(noService);
		iAgentDialogInput.iDialog = EContinueDialog;
		StartNotifierAndGetResponse();
		iRecordsPresent = EFalse;
		}
	}

void CIAPSelection::SelectL(TInt aError)
/**
@param aError Any error code, or KErrNone if no error.
*/
	{
	if (aError!=KErrNone)
		{
		iAgentDialogInput.iTitle.Format(KWarnPrompt,aError);
//		iAgentDialogInput.iTitle.Append(KCarriageReturn);
		}
	iAgentDialogInput.iTitle.AppendFormat(KSelectTitle,&KIAP);
	ReselectL();
	}

void CIAPSelection::ReselectL()
	{
	CCDIAPRecord* ptrIapRecord  = static_cast<CCDIAPRecord*>(static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iDefaultIAP.iLinkedRecord);

	if(!ptrIapRecord)
		{
		CCDIAPRecord* ptrIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
		CleanupStack::PushL(ptrIapRecord);
		
		CCDConnectionPrefsRecord* connpref = static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex]);
		ptrIapRecord->SetRecordId(connpref->iDefaultIAP);
		ptrIapRecord->LoadL(*iDb);
		connpref->iDefaultIAP.iLinkedRecord = ptrIapRecord;
		
		CleanupStack::Pop(ptrIapRecord);
		}

	TBuf<KCommsDbSvrDefaultTextFieldLength> tmpBuffer;
    // coverity[var_deref_model] - ptrIapRecord guaranteed initialised by RecordFactoryL
	tmpBuffer = ptrIapRecord->iRecordName;
	iAgentDialogInput.iLabel.Format(KSelectPrompt,&tmpBuffer);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	}

void CIAPSelection::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus < KErrNone)
		{
		__FLOG_1(_L("Dialog returned with %d"),iStatus.Int());
		iResultStatus=iStatus.Int();
		if((iResultStatus==KErrNone)&&(iCancelFlag))
			{
			iResultStatus=KErrCancel;
			}
		iCallBack->Call();
		return;
		}

	if (iRecordsPresent)
		{
		if (iStatus.Int() == EAgentYesPressed)	//YES Pressed
			{
			__FLOG(_L("Dialog returned after \"Yes\" was pressed"));
			*iIAPId = static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iDefaultIAP;
			iResultStatus=KErrNone;
			iCallBack->Call();	
			}
		else
			{
			__FLOG_1(_L("No pressed selecting next record - returned %d"),iStatus.Int());
			if(iRecordIndex < (iTable->iRecords.Count() - 1))
				{
				iRecordIndex++;
				}
			else
				{
				iRecordIndex = 0;
				}					
			
			CCDIAPRecord* ptrIapRecord  = static_cast<CCDIAPRecord*>(static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iDefaultIAP.iLinkedRecord);
	
			if(!ptrIapRecord)
				{
				CCDIAPRecord* ptrIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
				
				CCDConnectionPrefsRecord* connpref = static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex]);
				ptrIapRecord->SetRecordId(connpref->iDefaultIAP);
				connpref->iDefaultIAP.iLinkedRecord = ptrIapRecord;				
				}				
			
			static_cast<CCDIAPRecord*>(static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[iRecordIndex])->iDefaultIAP.iLinkedRecord)->LoadL(*iDb);
				
			ReselectL();
			}
		}
	else
		{
		iResultStatus=KErrCancel;
		iCallBack->Call();
		}
	}

TInt CIAPSelection::Status()
/**
Gets the Status
@return the status.
*/
	{
	return iResultStatus;
	}


CAccessPointSelection* CAccessPointSelection::NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
/**
AccessPoint Selection

@param aDb pointer to communications database.
@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to AccessPointSelection object.
*/
	{
	CAccessPointSelection* r=new(ELeave) CAccessPointSelection(aDb,aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CAccessPointSelection::CAccessPointSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority),
	iDb(aDb)
/**
CIAPSelection Constructor
*/
	{
	}

void CAccessPointSelection::ConstructL()
/**
Completes construction of the CAccessPointSelection object.

@see CDialogBase::ConstructL().
*/
	{
	CDialogBase::ConstructL();
	CActiveScheduler::Add(this);
	}

CAccessPointSelection::~CAccessPointSelection()
/**
CAccessPointSelection Destructor.
*/
	{
	Cancel();
	delete iTable;
	}
	
void CAccessPointSelection::InitRecordSetL(TUint32 aDefaultAP, TInt aTierId)
	{
	ASSERT(iTable==0);
	iTable = new (ELeave) CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
	iTable->LoadL(*iDb);
	
    //just to show the TableId and RecordId
    TInt bitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;

	// sort with default SNAP first
	for (TInt i = 0; i < iTable->iRecords.Count(); i++)
		{
		//if (static_cast<CCDAccessPointRecord*>(iTable->iRecords[i])->iRecordTag == aDefaultAP)
		CCDAccessPointRecord* aprec = static_cast<CCDAccessPointRecord*>(iTable->iRecords[i]);
		TInt id = aprec->ElementId() & bitMask;
		if ( (static_cast<CCDAccessPointRecord*>(iTable->iRecords[i])->ElementId() & bitMask) == aDefaultAP)
			{
			CMDBRecordBase* t = iTable->iRecords[i];
			iTable->iRecords[i] = iTable->iRecords[0];
			iTable->iRecords[0] = t;
			break;
			}
		}

	// filter out all access points that do not belong to the specified group
    CMDBField<TInt>* tierTagId = new(ELeave) CMDBField<TInt>(KCDTIdTierRecord | KCDTIdRecordTag);
    CleanupStack::PushL(tierTagId);

	for (TInt i = iTable->iRecords.Count() - 1; i >= 0; i--)
		{
		ASSERT((iTable->iRecords[i]->TypeId() & KCDMaskShowRecordType) == KCDTIdAccessPointRecord);
        //this is the ElementId of the linked TierRec...
		TMDBElementId tierElementId = static_cast<CCDAccessPointRecord*>(iTable->iRecords[i])->iTier;	// was iAccessPointGID; tiers subsume groups concept
		TInt tierRecId = (tierElementId & KCDMaskShowRecordId) >> 8;
		
		tierTagId->SetRecordId(tierRecId);
		
		tierTagId->LoadL(*iDb);
		
		TInt tierId = *tierTagId;
		if (tierId != aTierId)
			{
			CMDBRecordBase* ptr = iTable->iRecords[i];
			iTable->iRecords.Remove(i);
			delete ptr;			
			}
		}
    CleanupStack::PopAndDestroy(tierTagId);
	}
	
void CAccessPointSelection::GetAccessPointL(TUint32& aAPId, TInt aTierId, TCallBack aCallBack)
/**
Gets the access point.

@param aAPId The id of the access point.
@param aTierId Specifies the tier id.
@param aError Any error code, or KErrNone if no error.
@param aCallBack Encapsulates a general call-back function.
*/
	{
	User::LeaveIfError(aTierId>0? KErrNone : KErrArgument);

	iCallBack = new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	iCancelFlag = EFalse;
	iAPId = &aAPId;
	iRecordsPresent = ETrue;

	//Get UID of the tier's manager having the tier's id.
    CCDTierRecord* tierRec = static_cast<CCDTierRecord*>(CCDTierRecord::RecordFactoryL(KCDTIdTierRecord));
	CleanupStack::PushL(tierRec);
    tierRec->iRecordTag = aTierId;
	TBool found = tierRec->FindL(*iDb);
	if (!found)
    	{
    	User::Leave(KErrNotFound);
    	}
	ASSERT(tierRec->iTierManagerName.TypeId() == KCDTIdTierManagerName); // Panics if built against incorrect CommsDat.

	if (tierRec->iDefaultAccessPoint != 0)
		{
		TInt defaultAP = tierRec->iDefaultAccessPoint;
		InitRecordSetL(defaultAP,aTierId);
		if (iTable->iRecords.Count() > 0)
			{
			iAgentDialogInput.iTitle.AppendFormat(KSelectTitle, &KAP);
			ReselectL();
			}
		else
			{
			iAgentDialogInput.iTitle.Format(KWarnPrompt,KErrNotFound);
			_LIT(noService, "No Access Point Records Found");
			iAgentDialogInput.iTitle.Append(noService);
			iAgentDialogInput.iDialog = EContinueDialog;
			StartNotifierAndGetResponse();
			iRecordsPresent = EFalse;
			}
		}
	CleanupStack::PopAndDestroy(tierRec);

	}

void CAccessPointSelection::ReselectL()
	{
	TBuf<KCommsDbSvrDefaultTextFieldLength> tmpBuffer;
	CCDAccessPointRecord* snapRecord = static_cast<CCDAccessPointRecord*>(iTable->iRecords[iRecordIndex]);
	tmpBuffer = snapRecord->iRecordName;
	iAgentDialogInput.iLabel.Format(KSelectPrompt,&tmpBuffer);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	}

void CAccessPointSelection::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus < KErrNone)
		{
		__FLOG_1(_L("Dialog returned with %d"),iStatus.Int());
		iResultStatus=iStatus.Int();
		if((iResultStatus==KErrNone)&&(iCancelFlag))
			{
			iResultStatus=KErrCancel;
			}
		iCallBack->Call();
		return;
		}

	if (iRecordsPresent)
		{
		if (iStatus.Int() == EAgentYesPressed)	//YES Pressed
			{
			__FLOG(_L("Dialog returned after \"Yes\" was pressed"));
			CCDAccessPointRecord* snapRecord = static_cast<CCDAccessPointRecord*>(iTable->iRecords[iRecordIndex]);
			*iAPId = snapRecord->iRecordTag;
			iResultStatus=KErrNone;
			iCallBack->Call();	
			}
		else
			{
			__FLOG_1(_L("No pressed selecting next record - returned %d"),iStatus.Int());
			if(iRecordIndex < (iTable->iRecords.Count() - 1))
				{
				iRecordIndex++;
				}
			else
				{
				iRecordIndex = 0;
				}					

			ReselectL();
			}
		}
	else
		{
		iResultStatus=KErrCancel;
		iCallBack->Call();
		}
	}

TInt CAccessPointSelection::Status()
/**
Gets the Status
@return the status.
*/
	{
	return iResultStatus;
	}


CModemAndLocationSelection* CModemAndLocationSelection::NewL(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
/**
Modem And Location Selection
@param aDb pointer to communications database/commsdat session.
@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return A Pointer to CModemAndLocationSelection object.
*/
	{
	CModemAndLocationSelection* r=new(ELeave) CModemAndLocationSelection(aDb,aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CModemAndLocationSelection::CModemAndLocationSelection(CMDBSession* aDb,CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority),
	iDb(aDb)
	
/**
Constructor
*/
	{}

void CModemAndLocationSelection::ConstructL()
/**
Create a new CModemAndLocationSelection object.
*/
	{
	CDialogBase::ConstructL();
	CActiveScheduler::Add(this);
	}

void CModemAndLocationSelection::InitRecordSetL(TMDBElementId aRecordId)
	{
	delete iTable;
	iTable = NULL;

	CCDRecordBase* ptrTemplate;
	if(aRecordId == KCDTIdModemBearerRecord)
		{
		iTable = new (ELeave)CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
		ptrTemplate = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
		}
	else
		{
		iTable = new (ELeave)CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);
		ptrTemplate = static_cast<CCDLocationRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
		}		

	CleanupStack::PushL(ptrTemplate);
	User::LeaveIfError((iTable->iRecords).Append(ptrTemplate));
	CleanupStack::Pop(ptrTemplate);

	iRecordIndex = 0;
	}
CModemAndLocationSelection::~CModemAndLocationSelection()
/**
Destructor
*/
	{
	Cancel();
	delete iTable;
	}

void CModemAndLocationSelection::GetModemAndLocationL(TUint32& aModemId, TUint32& aLocationId, TCallBack aCallBack)
/**
Shows a dialog to enable the user to choose which modem and location to use for data transactions

@param aModemId The id of of the record in the modem table to be used for data transactions
@param aLocationId The id of the record in the location table to be used for the modem for data transactions
@param aStatus Any error code, or KErrNone if no error
*/
	{
	iCancelFlag = EFalse;
	iCallBack = new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	iModemId = &aModemId;
	iLocationId = &aLocationId;
	
	StartModemSelectL();
	}

void CModemAndLocationSelection::StartModemSelectL()
/**
Shows a dialog to enable the user to start the modem.
*/
	{
	iState=EEnterModem;
	InitRecordSetL(KCDTIdModemBearerRecord);	
	TRAPD(result, iTable->LoadL(*iDb)); 

	if (result != KErrNone)	
		{
		DisplayNoRecordsL();
		}
	else
		{
		SelectL(TPtrC(MODEM),KErrNone);
		}
	}

void CModemAndLocationSelection::StartLocationSelectL()
	{
	iState=EEnterLocation;
	delete iTable;
	iTable = NULL;
	InitRecordSetL(KCDTIdLocationRecord);	
	TRAPD(result, iTable->LoadL(*iDb)); 

	if (result != KErrNone)
		{
		DisplayNoRecordsL();
		}
	else
		{
		SelectL(TPtrC(LOCATION),KErrNone);
		}
	}

void CModemAndLocationSelection::SelectL(const TDesC& aTable, TInt aError)
	{
	if (aError!=KErrNone)
		{
		iAgentDialogInput.iTitle.Format(KWarnPrompt,aError);
		iAgentDialogInput.iTitle.Append(KCarriageReturn);
		iAgentDialogInput.iTitle.AppendFormat(KSelectTitle,&aTable);
		}
	else
		{
		iAgentDialogInput.iTitle.Format(KSelectTitle,&aTable);
		}
	ReselectL();
	}

void CModemAndLocationSelection::ReselectL()
	{
	TBuf<KCommsDbSvrDefaultTextFieldLength> tmpBuffer;
		tmpBuffer = static_cast<CCDRecordBase*>(iTable->iRecords[iRecordIndex])->iRecordName;
		iAgentDialogInput.iLabel.Format(KSelectPrompt,&tmpBuffer);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	}

void CModemAndLocationSelection::DisplayNoRecordsL()
/**
@Leave ENoRecordFound If no records found.
*/
	{
	switch(iState)
		{
	case EEnterModem:
			{
			_LIT(noModem,"No Modem Records Found");
			iAgentDialogInput.iTitle.Copy(noModem);
			}
		break;
	case EEnterLocation:
			{
			_LIT(noLocation,"No Location Records Found");
			iAgentDialogInput.iTitle.Copy(noLocation);
			}
		break;
	default:
			{	
			_LIT(noRecords,"No Records Found");
			iAgentDialogInput.iTitle.Copy(noRecords);
			}
		break;
		}	
	
	iAgentDialogInput.iDialog = EContinueDialog;
	StartNotifierAndGetResponse();

	iState=ENoRecordFound;
	}

void CModemAndLocationSelection::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus < KErrNone)
		{
		iResultStatus=iStatus.Int();
		if((iResultStatus==KErrNone)&&(iCancelFlag))
			{
			iResultStatus=KErrCancel;
			}
		iCallBack->Call();
		return;
		}
	
	switch(iState)
		{
		case EEnterModem:
			if(iStatus.Int() == EAgentYesPressed)	//Yes Pressed
				{
				*iModemId = (iTable->iRecords[iRecordIndex])->RecordId();
				//iStatusResult is not yet KErrNone because need to enter the location too
				StartLocationSelectL();
				}
			else
				{
				if(iRecordIndex < (iTable->iRecords.Count() - 1))
					{
					iRecordIndex++;
					}
				else
					{
					iRecordIndex = 0;
					}						
				ReselectL();
			}
			break;
		case EEnterLocation:
			if(iStatus.Int()  == EAgentYesPressed) //Yes Pressed
				{
				*iLocationId = (iTable->iRecords[iRecordIndex])->RecordId();
				iResultStatus=KErrNone;
				iCallBack->Call();	
				}
			else
				{
				if(iRecordIndex < (iTable->iRecords.Count() - 1))
					{
					iRecordIndex++;
					}
				else
					{
					iRecordIndex = 0;
					}	
				ReselectL();
				}
			break;
		case ENoRecordFound:
			iResultStatus=KErrCancel;
			iCallBack->Call();
			break;
	
		default:
			User::Leave(KErrNotFound);
			break;
		}
	}

TInt CModemAndLocationSelection::Status()
/**
Gets the Status.

@return the status.
*/
	{
	return iResultStatus;
	}

CIAPWarning* CIAPWarning::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
IAP Warning.

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return A Pointer to CIAPWarning object.
*/
	{
	CIAPWarning* r=new(ELeave) CIAPWarning(aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CIAPWarning::CIAPWarning(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority)
/**
Constructor
*/
	{}

void CIAPWarning::ConstructL()
/**
Create a new CIAPWarning object.
*/
	{
	CDialogBase::ConstructL();
	CActiveScheduler::Add(this);
	}

CIAPWarning::~CIAPWarning()
/**
Destructor.
*/
	{
	Cancel();
	}

void CIAPWarning::WarnIAPL(const TConnectionPrefs& /*aPrefs*/, TInt aLastError, const TDesC& aNewIapName, TBool& aResponse, TCallBack aCallBack)
/**
Warn IAP

@param aPrefs Specifies the rank and desired direction of the connection and bearer.
@param aLastError The error with which previous connection failed.
@param aNewIapName The name of the IAP to be used for next connection.
@param aResponse Specifies whether to proceed with the connection or stop the connection attempt.
@param aCallBack Constructs the callback object with the specified callback function .
*/
	{
	iCancelFlag = EFalse;
	iCallBack = new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	iResponse = &aResponse;

	if (aLastError < KErrNone)
		{
		iAgentDialogInput.iTitle.Format(KWarnPrompt,aLastError);
		iAgentDialogInput.iLabel.Format(KAttemptPrompt,&aNewIapName);
		iAgentDialogInput.iDialog = ENoYesDialog;
		}
	else
		{
		iAgentDialogInput.iTitle.Copy(KWarnPromptNoError);
		iAgentDialogInput.iLabel.Format(KAttemptPrompt,&aNewIapName);
		iAgentDialogInput.iDialog = ENoYesDialog;
		}
	StartNotifierAndGetResponse();

	}

void CIAPWarning::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	iResultStatus = iStatus.Int();
	*iResponse = (iStatus.Int() == EAgentYesPressed);

	if(iResultStatus == KErrNone && iCancelFlag)
		{
		iResultStatus = KErrCancel;
		}

	if (iResultStatus > KErrNone)
		{
		iResultStatus = KErrNone;
		}
		
	iCallBack->Call();	
	}

TInt CIAPWarning::Status()
/**
Gets the Status

@return the status.
*/
	{
	return iResultStatus;
	}


CLogin* CLogin::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
Login Dialog

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to CLogin object.
*/
	{
	CLogin* login=new(ELeave) CLogin(aSession,aPriority);
	CleanupStack::PushL(login);
	login->ConstructL();
	CleanupStack::Pop();
	return login;
	}

CLogin::CLogin(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority)
/**
Constructor.
*/
	{
	CActiveScheduler::Add(this);
	}

CLogin::~CLogin()
/**
Destructor.
*/
	{
	Cancel();
	}

void CLogin::GetUserPassL(TDes& aUsername, TDes& aPassword, TCallBack aCallBack)
/**
Get the User Name and Password.

@param aUsername Username.
@param aPassword Password.
@param aCallBack Constructs the callback object with the specified callback function.
*/
	{
	iCancelFlag=EFalse;
	iCallBack=new(ELeave) CAsyncCallBack(aCallBack,0);
	iUsername=&aUsername;
	iPassword=&aPassword;
	StartGetUsername();
	}

void CLogin::StartGetUsername()
	{
	_LIT(KLoginUsernamePrompt,"Select Login Username");
	iAgentDialogInput.iTitle.Copy(KLoginUsernamePrompt);
	iAgentDialogInput.iLabel.Copy(*iUsername);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	iState = EEnterName;
	}

void CLogin::StartGetPassword()
	{
	_LIT(KLoginPasswordPrompt,"Select Login Password");
	iAgentDialogInput.iTitle.Copy(KLoginPasswordPrompt);
	iAgentDialogInput.iLabel.Copy(*iPassword);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	iState = EEnterPass;
	}

void CLogin::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus.Int() < KErrNone)
		{
		iResultStatus=iStatus.Int();
		if((iResultStatus==KErrNone)&&(iCancelFlag))
			{
			iResultStatus=KErrCancel;
			}
		iCallBack->Call();
		return;
		}

	switch(iState)
		{
	case EEnterName:
		if(iStatus.Int() == EAgentYesPressed)	//yes Pressed
			{
			StartGetPassword();
			}
		else
			{
			for(TInt i=0;i<iUsername->Length();i++)
				{
				TChar aChar=(TChar)(++(*iUsername)[i]);
				if(!aChar.IsAlphaDigit())
					{
					(*iUsername)[i]=ND_DLDSL::KFullStop;
					}
				}
			StartGetUsername();
			}
		break;

	case EEnterPass:
		if(iStatus.Int() == EAgentYesPressed)	//Yes PRessed
			{
			iResultStatus=KErrNone;
			iCallBack->Call();
			}
		else
			{
			for(TInt i=0;i<iPassword->Length();i++)
				{
				TChar aChar=(TChar)(++(*iPassword)[i]);
				if(!aChar.IsAlphaDigit())
					{
					(*iPassword)[i]=ND_DLDSL::KFullStop;
					}
				}
			StartGetPassword();
			}
		break;

	default:
		User::Leave(KErrNotFound);
		break;
		}

	}


TInt CLogin::Status()
/**
Gets the Status.

@return the status.
*/
	{
	return iResultStatus;
	}

CAuthenticate* CAuthenticate::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
Authentication Dialog.

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to CLogin object.
*/
	{
	CAuthenticate* authenticate=new(ELeave) CAuthenticate(aSession,aPriority);
	CleanupStack::PushL(authenticate);
	authenticate->ConstructL();
	CleanupStack::Pop();
	return authenticate;
	}

CAuthenticate::CAuthenticate(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority)
/**
Constructor.
*/
	{
	CActiveScheduler::Add(this);
	}

CAuthenticate::~CAuthenticate()
/**
Destructor.
*/
	{
	Cancel();
	}

void CAuthenticate::GetUserPassL(TDes& aUsername, TDes& aPassword, TCallBack aCallBack)
/**
Get the User Name and Password.

@param aUsername Username.
@param aPassword Password.
@param aCallBack Constructs the callback object with the specified callback function.
*/
	{
	iCancelFlag=EFalse;
	iCallBack=new(ELeave) CAsyncCallBack(aCallBack,0);
	iUsername=&aUsername;
	iPassword=&aPassword;
	StartGetUsername();
	}

void CAuthenticate::StartGetUsername()
	{
	_LIT(KAuthUsernamePrompt,"Select Authentication Username");
	iAgentDialogInput.iTitle.Copy(KAuthUsernamePrompt);
	iAgentDialogInput.iLabel.Copy(*iUsername);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	iState = EEnterName;
	}

void CAuthenticate::StartGetPassword()
	{
	_LIT(KAuthPasswordPrompt,"Select Authentication Password");
	iAgentDialogInput.iTitle.Copy(KAuthPasswordPrompt);
	iAgentDialogInput.iLabel.Copy(*iPassword);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	iState = EEnterPass;
	}

void CAuthenticate::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus.Int() < KErrNone)
		{
		iResultStatus=iStatus.Int();
		if((iResultStatus==KErrNone)&&(iCancelFlag))
			{
			iResultStatus=KErrCancel;
			}
		iCallBack->Call();
		return;
		}

	if (iStatus.Int() == EAgentNoPressed)
		{
		iResultStatus=EAgentNoPressed;
		iCallBack->Call();
		return;
		}

	switch(iState)
		{
	case EEnterName:
		if(iStatus.Int() == EAgentYesPressed)	//Yes PRessed
			{
			StartGetPassword();
			}
		else
			{
			for(TInt i=0;i<iUsername->Length();i++)
				{
				TChar aChar=(TChar)(++(*iUsername)[i]);
				if(!aChar.IsAlphaDigit())
					{
					(*iUsername)[i]=ND_DLDSL::KFullStop;
					}
				}
			StartGetUsername();
			}
		break;

	case EEnterPass:
		if(iStatus.Int() == EAgentYesPressed)	//Yes PRessed
			{
			iResultStatus=KErrNone;
			iCallBack->Call();
			}
		else
			{
			for(TInt i=0;i<iPassword->Length();i++)
				{
				TChar aChar=(TChar)(++(*iPassword)[i]);
				if(!aChar.IsAlphaDigit())
					{
					(*iPassword)[i]=ND_DLDSL::KFullStop;
					}
				}
			StartGetPassword();
			}
		break;
		
	default:
		User::Leave(KErrNotFound);
		break;
		}
	}

TInt CAuthenticate::Status()
/**
Gets the Status.

@return the status.
*/
	{
	return iResultStatus;
	}


CReconnect* CReconnect::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
Reconnection Dialog

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to CReconnect object.
*/
	{
	CReconnect* reconnect=new(ELeave) CReconnect(aSession,aPriority);
	CleanupStack::PushL(reconnect);
	reconnect->ConstructL();
	CleanupStack::Pop();
	return reconnect;
	}

CReconnect::CReconnect(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority)
/**
Constructor.
*/
	{
	CActiveScheduler::Add(this);
	}

CReconnect::~CReconnect()
/**
Destructor.
*/
	{
	Cancel();
	}

void CReconnect::ReconnectL(TCallBack aCallBack)
/**
Shows a reconnect dialog when connection has been broken during data transfer.

@param aCallBack Encapsulates a general call-back function.
*/
	{
	iCancelFlag=EFalse;
	iCallBack=new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	_LIT(KReconnectTitle,"Reconnect Dialog");
	iAgentDialogInput.iTitle.Copy(KReconnectTitle);
	_LIT(KReconnectPrompt,"Reconnect? ");
	iAgentDialogInput.iLabel.Copy(KReconnectPrompt);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	}

void CReconnect::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iStatus>=KErrNone)
		{
		if(iStatus.Int() == EAgentYesPressed)
			{
			iResultStatus=ETrue;
			}
		else
			{
			iResultStatus=EFalse;
			}
		if(iCancelFlag)
			{
			iResultStatus=EFalse;
			}
		iCallBack->Call();
		return;
		}
	iResultStatus=EFalse;
	iCallBack->Call();
	}

TBool CReconnect::Status()
/**
Gets the Status

@return the status.
*/
	{
	return iResultStatus;
	}

void CReconnect::DoCancel()
/**
Implements cancellation of an outstanding request 
*/
	{
	CancelNotifier();
	
	iResultStatus = EFalse; // choose "no"
	// need to call this so that the previous reconnect request is completed (otherwise the cancellation can block forever)
	iCallBack->Call();
	}

CPct* CPct::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
CPct definitions

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to CPct object.
*/ 
	{
	CPct* r=new(ELeave) CPct(aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CPct::CPct(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority),
	iState(ENone),
	iDestroyCallBack(EPriorityStandard)
/**
Constructor
*/
	{
	CActiveScheduler::Add(this);
	}	

void CPct::ConstructL()
/**
Create a new CPct object.
*/
	{
	iTimer=CPctTimer::NewL(this);
	iStartBuffer=0;
	CDialogBase::ConstructL();
	}

CPct::~CPct()
/**
Destructor
*/
	{
	Cancel();
	delete iTimer;
	}

void CPct::WritePct(TDes& aBuffer)
	{
	if (iState==ERead)
		return;
	
	iCancelFlag=EFalse;
	iWriteBuffer.Copy(aBuffer);

	for(TInt i=0;i<iWriteBuffer.Length();i++)
		{
		if (iWriteBuffer[i]==KCarriageReturn || iWriteBuffer[i]==KLineFeed)
			iWriteBuffer[i]=ND_DLDSL::KFullStop;
		}

	if (iState==EWrite)
		{
		__ASSERT_DEBUG(iTimer!=NULL, User::Invariant());
		iTimer->Cancel();
		}

	DoWrite();
	}

void CPct::DoWrite()
	{
	__ASSERT_DEBUG(iTimer!=NULL, User::Invariant());
	__ASSERT_DEBUG(iNotifier!=NULL, User::Invariant());

	if (iStartBuffer<(iWriteBuffer.Length()-ND_DLDSL::KNotifierLength))
		{
		iNotifier->InfoPrint(iWriteBuffer.Mid(iStartBuffer,ND_DLDSL::KNotifierLength));
		iStartBuffer+=4;
		iTimer->Start();
		iState=EWrite;
		}
	else
		iState=ENone;
	}
	
void CPct::PctTimerComplete(TInt /*aStatus*/)
	{
	DoWrite();
	}

void CPct::ReadPctL(TDes& aBuffer,TCallBack& aCallBack)
	{
	if (iTimer!=NULL)
		iTimer->Cancel();
	
	iCancelFlag=EFalse;
	delete iCallBack;
	iCallBack=new(ELeave) CAsyncCallBack(aCallBack,0);
	iReadBuffer=&aBuffer;
	StartRead();
	}

void CPct::StartRead()
	{
	__ASSERT_DEBUG(iNotifier!=NULL, User::Invariant());

	iAgentDialogInput.iTitle.Copy(iWriteBuffer.Right(Min(ND_DLDSL::KNotifierLength,iWriteBuffer.Length())));
	iAgentDialogInput.iLabel.Copy(*iReadBuffer);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	iState=ERead;	
	}

void CPct::DestroyPctNotificationL(TCallBack& aCallBack)
	{
	__ASSERT_DEBUG(!iDestroyCallBackOutstanding, User::Invariant());

	iDestroyCallBackOutstanding = ETrue;
	iDestroyCallBack.Set(aCallBack);
	}

void CPct::ClosePct()
/**
Close the Pct dialog.
*/
	{
	__ASSERT_DEBUG(!iDestroyCallBackOutstanding, User::Invariant());

	DoTimerCancel();
	}	
	
TInt CPct::Status()
/**
Gets the Status

@return the status.
*/
	{
	return iResultStatus;
	}

void CPct::DoReadCancel()
/**
Cancels read only.  Does not cancel destroy notification
*/
	{
	iCancelFlag=ETrue;		
	}

void CPct::DoTimerCancel()
/**
Cancels timer only.  This is not a real cancel as the timer is just so that you can see the
writes all the time.
*/
	{
	if (iTimer!=NULL)
		{
		iTimer->Cancel();
		delete iTimer;
		iTimer=NULL;
		}	
	}

void CPct::DoNotificationCancel()
/**
Cancel Notification only.
*/
	{
	if (iDestroyCallBackOutstanding)
		{
		iResultStatus = KErrCancel;
		iDestroyCallBack.Call();
		iDestroyCallBackOutstanding = EFalse;
		}
	}

void CPct::DoCancel()
/**
Implements cancellation of an outstanding request 
*/
	{
	DoReadCancel();
	DoTimerCancel();
	DoNotificationCancel();
	}

void CPct::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	if (iState!=ERead)
		User::Leave(KErrNotFound);

	if (iCancelFlag)
		{
		iResultStatus=KErrCancel;
		iCallBack->Call();
		iState=ENone;
		return;
		}
	
	if (iStatus.Int() < KErrNone)
		{
		
		iResultStatus=iStatus.Int();
		iCallBack->Call();
		iState=ENone;
		return;
		}

	if(iStatus.Int() == EAgentYesPressed)
		{
		iResultStatus=KErrNone;
		iCallBack->Call();
		iState=EWrite;
		WritePct(iWriteBuffer);
		}
	else
		{
		for(TInt i=0;i<iReadBuffer->Length();i++)
			{
			TChar ch=(TChar)(++(*iReadBuffer)[i]);
			if(!ch.IsAlphaDigit())
				(*iReadBuffer)[i]=ND_DLDSL::KFullStop;
			}
		StartRead();
		}
	}

CPctTimer* CPctTimer::NewL(CPct* aNotifier)
/**
CPctTimer Definition. Create a new CPctTimer object.

@return A Pointer to CPctTimer object.
*/
	{
	CPctTimer* r=new(ELeave) CPctTimer(aNotifier);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CPctTimer::CPctTimer(CPct* aNotifier) 
	: CTimer(0),
	iNotifier(aNotifier)
/**
Constructor.
*/
	{
	CActiveScheduler::Add(this);
	}

CPctTimer::~CPctTimer()
/**
Destructor.
*/
	{
	Cancel();
	}

void CPctTimer::Start()
/**
Start the Pct Timer.
*/
	{
	After(ND_DLDSL::KTimeInterval);
	}

void CPctTimer::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	iNotifier->PctTimerComplete(iStatus.Int());
	}

CQoSWarning* CQoSWarning::NewL(CNetDialDialogSession* aSession, TInt aPriority)
/**
QoS Warning.

@param aSession pointer to NetDialDialogSession.
@param aPriority Priority.
@return Pointer to CQoSWarning object.
*/
	{
	CQoSWarning* r=new(ELeave) CQoSWarning(aSession,aPriority);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CQoSWarning::CQoSWarning(CNetDialDialogSession* aSession, TInt aPriority)
	: CDialogBase(aSession, aPriority)
/**
Constructor.
*/
	{}

void CQoSWarning::ConstructL()
/**
Create a new CPctTimer object.
*/
	{
	CDialogBase::ConstructL();
	CActiveScheduler::Add(this);
	}

CQoSWarning::~CQoSWarning()
/**
Destructor.
*/
	{
	Cancel();
	}

void CQoSWarning::WarnQoSL(TBool& aResponse, TCallBack aCallBack)
/**
Warn QoS

@param aResponse Specifies whether to proceed with the connection or stop the connection attempt.
@param aCallBack Encapsulates a general call-back function.
*/
	{
	iCancelFlag = EFalse;
	iCallBack = new(ELeave) CAsyncCallBack(aCallBack,CActive::EPriorityStandard);
	iResponse = &aResponse;

	iAgentDialogInput.iTitle.Copy(KQoSWarnTitle);
	iAgentDialogInput.iLabel.Copy(KQoSWarnPrompt);
	iAgentDialogInput.iDialog = ENoYesDialog;
	StartNotifierAndGetResponse();
	}

void CQoSWarning::RunL()
/**
Handles an active object’s request completion event.
*/
	{
	iResultStatus = iStatus.Int();
	*iResponse = (iStatus.Int() == EAgentYesPressed);

	if(iResultStatus == KErrNone && iCancelFlag)
		iResultStatus = KErrCancel;

	if (iResultStatus > KErrNone)
		iResultStatus = KErrNone;

	iCallBack->Call();	
	}

TInt CQoSWarning::Status()
/**
Gets the Status.

@return the status.
*/
	{
	return iResultStatus;
	}
