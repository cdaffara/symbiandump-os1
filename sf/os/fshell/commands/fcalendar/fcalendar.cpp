// fcalendar.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "caliterator.h"
#include <calsession.h>
#include <calentryview.h>
#include <calentry.h>
#include <calalarm.h>
#include <calrrule.h>
#include <caluser.h>
#include <calprogresscallback.h>
#include <fshell/ioutils.h>

using namespace IoUtils;


// Destroy the RPointerArray
void DestroyCalEntryRPointerArray(TAny* aPtr)
    {
    RPointerArray<CCalEntry>* self = static_cast<RPointerArray<CCalEntry>*> (aPtr);
    self->ResetAndDestroy();
    }



enum TCurrentOperation
	{
	ECONewEntryView,
	ECONone	
	};

class CCmdCalendar : public CCommandBase, public MCalProgressCallBack
	{
public:
	static CCommandBase* NewLC();
	~CCmdCalendar();
private:
	CCmdCalendar();

	void DoListEntryL();
	void DoDeleteEntryL();
	void DoAddEntryL();
	void DoChangeEntryL();
	
	void ReadCmdLineAmendEntryContentL(CCalEntry& aEntry);	
	void ListGSDetailL(const TDesC8& aGlobalId, TInt aVerboseLevel);	
	void PrintEntryDetailL(CCalEntry* aEntry, TInt aVerboseLevel);	
	
	void PrintAcceptedOptions();
	
	TInt CheckAlarmAlertServer();
	
	static const TDesC* UidName(TUid aUid);
	static const TDesC* EntryMethodToString(CCalEntry::TMethod aEntryMethod);
	static const TDesC* EntryStatusToString(CCalEntry::TStatus aEntryStatus);	
	static const TDesC* EntryTypeToString(CCalEntry::TType aEntryType);	
	static const TDesC* RepeatTypeToString(TCalRRule::TType aType);
	void PrintTime(const TTime& aTime, TBool aNewline);	
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
private:	//MCalProgressCallBack
	virtual void Progress(TInt aPercentageCompleted);
	virtual void Completed(TInt aError);
	virtual TBool NotifyProgress();
	
private:
	CActiveSchedulerWait* iActiveWait;
	TCurrentOperation iCurOpr;
	
	CCalSession* iCalSes;
	CCalEntryView* iEntryView;
	
	RArray<TBool> iVerbose;
	HBufC* iOptFileName;	//calendar database file full path
	TInt iOptLocalId;		//local ID
	HBufC* iOptGlobalID;
	CCalEntry::TType iOptType;
	HBufC* iOptSummary;
	HBufC* iOptStartTime;
	HBufC* iOptEndTime;

	
	enum 
		{
		EListEntry,
		EDeleteEntry,
		EAddEntry,
		EChangeEntry,		
		} iCommand;
	};

CCommandBase* CCmdCalendar::NewLC()
	{
	CCmdCalendar* self = new(ELeave) CCmdCalendar();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCalendar::CCmdCalendar()
	{
	iCurOpr = ECONone;
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	}


CCmdCalendar::~CCmdCalendar()
	{
	delete iOptFileName;
	delete iOptGlobalID;
	delete iOptSummary;	
	delete iOptStartTime;
	delete iOptEndTime;
	delete iEntryView;
	delete iCalSes;
	delete iActiveWait;
	iVerbose.Close();
	}

//////////////////////////////////////////////////////

//virtual 
void CCmdCalendar::Progress(TInt /*aPercentageCompleted*/)
	{
	;
	}

//virtual 
void CCmdCalendar::Completed(TInt /*aError*/)
	{
	TBool bWaitStarted = iActiveWait->IsStarted();
	ASSERT(bWaitStarted);
	(void)bWaitStarted;
	
	if (iCurOpr == ECONewEntryView)
		{
		iActiveWait->AsyncStop();
		}

	iCurOpr = ECONone;
	}

TBool CCmdCalendar::NotifyProgress()
	{
	return ETrue;
	}

const TDesC& CCmdCalendar::Name() const
	{
	_LIT(KName, "fcalendar");
	return KName;
	}

void CCmdCalendar::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgCommand, "command");
	aArguments.AppendEnumL((TInt&)iCommand, KArgCommand);
	}

void CCmdCalendar::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptFile, "file");
	aOptions.AppendStringL(iOptFileName, KOptFile);

	_LIT(KOptLocalId, "local_id");
	aOptions.AppendUintL((TUint&)iOptLocalId, KOptLocalId);

	_LIT(KOptGlobalId, "global_id");
	aOptions.AppendStringL(iOptGlobalID, KOptGlobalId);

	_LIT(KOptType, "type");
	aOptions.AppendEnumL((TInt&)iOptType, KOptType);

	_LIT(KOptSummary, "summary");
	aOptions.AppendStringL(iOptSummary, KOptSummary);

	_LIT(KOptStartTime, "start_time");
	aOptions.AppendStringL(iOptStartTime, KOptStartTime);

	_LIT(KOptEndTime, "end_time");
	aOptions.AppendStringL(iOptEndTime, KOptEndTime);
	}

//calendar have dependency on !AlamAlertServer,
//this server is usually part of eikon and not likely to be started separately
//we need to check if this server is up and running,
//if not, no point to contiune
//
//return:
//-1 (KErrNotFound): this server is not running
//0 (KErrNone): OK
TInt CCmdCalendar::CheckAlarmAlertServer()
	{
	TInt Ret;
	TInt Res;
	TFindServer findASServer(_L("!AlarmAlertServer"));
	TFullName name;
	Res = findASServer.Next(name);
	
	if (Res==KErrNotFound )
		Ret = KErrNotFound;
	else
		Ret = KErrNone;
	
	return Ret;
	}


void CCmdCalendar::DoRunL()
	{
	TInt res;
	res = CheckAlarmAlertServer();
	LeaveIfErr(res, _L("!AlarmAlertServer not running"));
	
	TRAPD(err, iCalSes = CCalSession::NewL() );
	LeaveIfErr(err, _L("Could not connect to Calendar server"));
	
	//open the calendar database file
	if (iOptFileName && iOptFileName->Length()>0 ) 
		{
		Printf(_L("Calendar Database file: %S \r\n"), iOptFileName);
		}
	else
		{
		Printf(_L("Calendar Database file unspecified, by default S60 database file is c:Calendar\r\n"));
		delete iOptFileName; iOptFileName = NULL;
		iOptFileName = HBufC::NewL(128);
		*iOptFileName = _L("c:Calendar");
		}
	
	//open database file
	TRAP(err, iCalSes->OpenL(*iOptFileName) );
	LeaveIfErr(err, _L("Could not open database file"));
	
	//Create an entry view 
	iCurOpr = ECONewEntryView;
	TRAP(err, iEntryView = CCalEntryView::NewL(*iCalSes, *this));
	LeaveIfErr(err, _L("Could not create entry view"));
	
	//entry view takes a long time to build index, and we must not do anything 
	//until we received callback, therefore, must wait here.
	iActiveWait->Start();
	
	///////////////////////////////////////////////////////////////////////////
	
	switch (iCommand)
		{
	case EListEntry:
		DoListEntryL();
		break;
	case EDeleteEntry:
		DoDeleteEntryL();	
		break;
	case EAddEntry:
		DoAddEntryL();
		break;
	case EChangeEntry:
		DoChangeEntryL();
		break;
		};

	}

void CCmdCalendar::DoChangeEntryL()
	{
	//compulsory: globalid, these can not be changed later
	if (!iOptGlobalID || iOptGlobalID->Length()== 0)
		LeaveIfErr(KErrArgument, _L("No global id specified. Use -g to specify one"));
	
	//type cannot be changed by calendar API, warning user of this fact
	if (iOptions.IsPresent(&iOptType))
		{
		Printf(_L("Type is not allowed to change, this option (-t) will be ignored.\r\n"));
		}
		
	RPointerArray<CCalEntry> CalEntryArray;
	CleanupStack::PushL(TCleanupItem(DestroyCalEntryRPointerArray, &CalEntryArray));
		
	TBuf8<128> TmpGlobalId;
	TmpGlobalId.Copy(*iOptGlobalID);
	HBufC8* pGlobalId = HBufC8::New(128);
	CleanupStack::PushL(pGlobalId);
	*pGlobalId = TmpGlobalId;
	
	//do an extra check if the same global ID is already used.
	iEntryView->FetchL(*pGlobalId, CalEntryArray);
	CleanupStack::PopAndDestroy(pGlobalId);
	pGlobalId = NULL;	//this HBufC is owned by CalEntry
	
	TInt Cnt = CalEntryArray.Count();
	if (Cnt == 0)
		LeaveIfErr(KErrNotFound, _L("No such Global ID."));					

	Printf(_L("%d entries found\r\n"), Cnt);
			
	CCalEntry* pEntry = CalEntryArray[0];
	
	//amend necessary calendar entry properties
	ReadCmdLineAmendEntryContentL(*pEntry);
		
	TInt SucessfulEntryCnt = 0;
	iEntryView->UpdateL(CalEntryArray, SucessfulEntryCnt);
	
	Printf(_L("%d entries changed\r\n"), SucessfulEntryCnt);
	
	CleanupStack::PopAndDestroy();	
	}

void CCmdCalendar::DoAddEntryL()
	{
	//compulsory: globalid, type, these can not be changed later
	if (!iOptGlobalID || iOptGlobalID->Length()== 0)
		LeaveIfErr(KErrArgument, _L("No global id specified. Use -g to specify one"));							
	if (!iOptions.IsPresent(&iOptType))
		LeaveIfErr(KErrArgument, _L("No type specified. Use -t to specify one"));
		
	RPointerArray<CCalEntry> CalEntryArray;
	CleanupStack::PushL(TCleanupItem(DestroyCalEntryRPointerArray, &CalEntryArray));
		
	TBuf8<128> TmpGlobalId;
	TmpGlobalId.Copy(*iOptGlobalID);
	HBufC8* pGlobalId = HBufC8::New(128);
	CleanupStack::PushL(pGlobalId);
	*pGlobalId = TmpGlobalId;
	
	//do an extra check if the same global ID is already used.
	//if yes, then adding with such global id is not allowed
		{
		iEntryView->FetchL(*pGlobalId, CalEntryArray);
		TInt Cnt = CalEntryArray.Count();
		if (Cnt > 0)
			LeaveIfErr(KErrAlreadyExists, _L("The same Global ID is already used."));					
		}
	
	CCalEntry* pEntry = CCalEntry::NewL(iOptType, pGlobalId, 
			CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(pGlobalId);
	pGlobalId = NULL;	//this HBufC is owned by CalEntry
		
	CalEntryArray.Append(pEntry);
	
	//amend necessary calendar entry properties
	ReadCmdLineAmendEntryContentL(*pEntry);
		
	TInt SucessfulEntryCnt = 0;
	iEntryView->StoreL(CalEntryArray, SucessfulEntryCnt);
	
	Printf(_L("%d entries added\r\n"), SucessfulEntryCnt);
	
	CleanupStack::PopAndDestroy();	
	}

void CCmdCalendar::DoDeleteEntryL()
	{
	//TInt VerboseLevel = iVerbose.Count();
	if (iOptGlobalID && iOptGlobalID->Length())
		{
		RPointerArray<CCalEntry> CalEntryArray;
		CleanupStack::PushL(TCleanupItem(DestroyCalEntryRPointerArray, &CalEntryArray));

		//FetchL is not Async
		//and there will be new CCalEntry objects generared 
		//and pointer stored in CalEntryArray
		TBuf8<256> GlobalID;
		GlobalID.Copy(*iOptGlobalID);
		
		iEntryView->FetchL(GlobalID, CalEntryArray);
		
		TInt EntryCount = CalEntryArray.Count();
		Printf(_L("%d Entries found\r\n"), EntryCount);
		
		for (TInt i=0; i<EntryCount; i++)
			{
			CCalEntry* pEntry = CalEntryArray[0];
			CalEntryArray.Remove(0);				
			
			CleanupStack::PushL(pEntry);
			iEntryView->DeleteL(*pEntry);
			CleanupStack::PopAndDestroy(pEntry);
			}		
		
		Printf(_L("%d Entries deleted\r\n"), EntryCount);
		CleanupStack::PopAndDestroy();
		}
	else
		{
		LeaveIfErr(KErrArgument, _L("No global id specified. Use -g to specify one"));							
		}	
	}

//list all agenda entry in the agenda server
void CCmdCalendar::DoListEntryL()
	{
	TInt VerboseLevel = iVerbose.Count();
	
	CCalIter* pIter = CCalIter::NewL(*iCalSes);
	CleanupStack::PushL(pIter);
	
	const TDesC8* pGlb = &(pIter->FirstL());
	TBuf<128> GlbId;
	
	while(*pGlb != KNullDesC8)
		{	
		GlbId.Copy(*pGlb);
		Printf(_L("================================\r\n"));
		Printf(_L("Group Scheduling Entry, Global ID: %S\r\n"), &GlbId);
		ListGSDetailL(*pGlb, VerboseLevel);
		
		pGlb = &(pIter->NextL());
		}		
	
	CleanupStack::PopAndDestroy(pIter);
	}

//this function read user command line options,
//and amend the corresponding Entry properties
//
void CCmdCalendar::ReadCmdLineAmendEntryContentL(CCalEntry& aEntry)
	{
	TInt res;
	//summary is optional
	if (iOptSummary && iOptSummary->Length() > 0)
		aEntry.SetSummaryL(*iOptSummary);	

	TCalTime StartTime = aEntry.StartTimeL();
	TCalTime EndTime = aEntry.EndTimeL();

	if (iOptStartTime && iOptStartTime->Length() > 0)
		{
		TTime Tmp(Time::NullTTime());
		res = Tmp.Parse(*iOptStartTime);
		if (res >= 0)
			{
			Printf(_L("StartTime recognised as ") );
			PrintTime(Tmp, ETrue);
			StartTime.SetTimeUtcL(Tmp);					
			}
		else
			{
			Printf(_L("Start Date/Time format not recogised.\r\n") );			
			}		
		}
	if (iOptEndTime && iOptEndTime->Length() > 0)
		{
		TTime Tmp(Time::NullTTime());
		res = Tmp.Parse(*iOptEndTime);		
		if (res >= 0)
			{
			Printf(_L("EndTime recognised as ") );
			PrintTime(Tmp, ETrue);
			EndTime.SetTimeUtcL(Tmp);		
			}
		else
			{
			Printf(_L("End Date/Time format not recogised.\r\n") );			
			}		
		
		}
	
	aEntry.SetStartAndEndTimeL(StartTime, EndTime);
	
	if (iOptLocalId)
		{
		TCalLocalUid LocalId = (TCalLocalUid) iOptLocalId;
		aEntry.SetLocalUidL(LocalId);		
		}	
	}


//verbose level
//-v = 1, -vv = 2
//
void CCmdCalendar::ListGSDetailL(const TDesC8& aGlobalId, TInt aVerboseLevel)
	{
	RPointerArray<CCalEntry> CalEntryArray;
	CleanupStack::PushL(TCleanupItem(DestroyCalEntryRPointerArray, &CalEntryArray));

	//FetchL is not Async
	//and there will be new CCalEntry objects generared 
	//and pointer stored in CalEntryArray  
	iEntryView->FetchL(aGlobalId, CalEntryArray);
	
	TInt Count = CalEntryArray.Count();
	for (TInt i=0; i<Count; i++)
		{
		CCalEntry* pCalEntry= CalEntryArray[0];
		CalEntryArray.Remove(0);		
		CleanupStack::PushL(pCalEntry);
		
		//make sure leave does not occur inside this scope, 
		//or contents in CalEntryArray may not be cleaned
		TRAP_IGNORE(PrintEntryDetailL(pCalEntry, aVerboseLevel));
		
		CleanupStack::PopAndDestroy(pCalEntry);
		}
	
	
	CleanupStack::PopAndDestroy();
	}


void CCmdCalendar::PrintEntryDetailL(CCalEntry* aEntry, TInt aVerboseLevel)
	{
	//this matches the Group Scheduling global ID, no need to print it out
	//const TDesC8& EntryID = aEntry->UidL();	
	
	//for verbose level 0
	//entry type, summery, start time, end time, priority
	CCalEntry::TType EntryType = aEntry->EntryTypeL();
	const TDesC& EntrySummary = aEntry->SummaryL();
	TCalTime EntryStartTime = aEntry->StartTimeL();
	TCalTime EntryEndTime = aEntry->EndTimeL();
	
	TCalTime EntryCompTime = aEntry->CompletedTimeL();
	TCalTime EntryModDate = aEntry->LastModifiedDateL();
	
	TUint EntryPriority = aEntry->PriorityL();

	Printf(_L("Type:%S Priority: %u Summary:%S\r\n"), 
			EntryTypeToString(EntryType), EntryPriority, &EntrySummary);
		
	Printf(_L(" StartTime:"));
	PrintTime(EntryStartTime.TimeUtcL(), EFalse);
	
	Printf(_L(" EndTime:"));
	PrintTime(EntryEndTime.TimeUtcL(), EFalse);

	Printf(_L("\r\n"));
	Printf(_L(" CompletedTime:"));
	PrintTime(EntryCompTime.TimeUtcL(), EFalse);
	
	Printf(_L(" Last Modified:"));
	PrintTime(EntryModDate.TimeUtcL(), EFalse);

	Printf(_L("\r\n"));
	
	
	if (aVerboseLevel < 1)
		return;
	
	//local id, location, status, method, description
	TCalLocalUid EntryLocalId = aEntry->LocalUidL();	
	const TDesC& EntryLocation = aEntry->LocationL();
	CCalEntry::TStatus EntryStatus = aEntry->StatusL();
	CCalEntry::TMethod EntryMethod = aEntry->MethodL();
	const TDesC& EntryDesc = aEntry->DescriptionL();
	TInt EntrySequenceNo = aEntry->SequenceNumberL();
	
	Printf(_L(" LocalId:%u, SeqNo:%d,Location:%S Description:%S\r\n"),
			EntryLocalId, EntrySequenceNo, &EntryLocation, &EntryDesc);
	Printf(_L(" Status:%S Method:%S\r\n"),
			EntryStatusToString(EntryStatus), EntryMethodToString(EntryMethod));
	
	
	//alarm
	CCalAlarm* pAlarm = aEntry->AlarmL();
	TPtrC AlarmYesNo = (pAlarm)?_L("Yes"):_L("No");
	Printf(_L(" Alarm: %S "), &AlarmYesNo);
	
	if (pAlarm && aVerboseLevel > 1)
		{
		CleanupStack::PushL(pAlarm);
		const TDesC& AlarmSound = pAlarm->AlarmSoundNameL();
		TTimeIntervalMinutes AlarmOffset = pAlarm->TimeOffset();
		
		Printf(_L("AlarmSound:%S, MinutesInAdvance:%d"), 
				&AlarmSound, AlarmOffset.Int());
		
		//CCalContent is optional, will be included from calalarm if it's present
#ifdef __CALCONTENT_H__
		CCalContent* pAlarmContent = pAlarm->AlarmAction();
		if (pAlarmContent)
			{
			const TDesC8& AlarmContent = pAlarmContent->Content();
			const TDesC8& AlarmMime = pAlarmContent->MimeType();

			Printf(_L8("\r\nAlarmContent:%S, AlarmMime:%S "), 
					&AlarmContent, &AlarmMime);
			
			}
#endif
		CleanupStack::PopAndDestroy(pAlarm);
		}
	Printf(_L("\r\n"));
	
	//repeat rules
	TCalRRule RepeatRule;
	TBool bRepeat = aEntry->GetRRuleL(RepeatRule);
	TPtrC RepeatYesNo = (bRepeat)?_L("Yes"):_L("No");
	Printf(_L(" Repeat: %S "), &RepeatYesNo);
	
	if (bRepeat && aVerboseLevel > 1)
		{
		TCalRRule::TType RepeatType = RepeatRule.Type();
		TInt RepeatInterval = RepeatRule.Interval();
		TUint RepeatCount = RepeatRule.Count();

		Printf(_L("RepeatType: %S Interval: %d Count:%u\r\n"),
				RepeatTypeToString(RepeatType), RepeatInterval, RepeatCount);
		TCalTime RepeatStart = RepeatRule.DtStart();
		TCalTime RepeatUntil = RepeatRule.Until();
		
		Printf(_L(" RepeatStart:"));
		PrintTime(RepeatStart.TimeUtcL(), EFalse);
		
		Printf(_L(" RepeatUntil:"));
		PrintTime(RepeatUntil.TimeUtcL(), EFalse);
		}
	Printf(_L("\r\n"));
		
	//attendee, in most of phone implementation it is not used
	RPointerArray<CCalAttendee>& AttendeeArray = aEntry->AttendeesL();
	TInt AttendeeCnt = AttendeeArray.Count();
	TPtrC AttendeeYesNo = (AttendeeCnt>0)?_L("Yes"):_L("No");
	Printf(_L(" Attendee: %S "), &AttendeeYesNo);

	if (AttendeeCnt>0 && aVerboseLevel > 1)
		{
		for (TInt i=0; i<AttendeeCnt; i++)
			{
			CCalAttendee* pAttendee = AttendeeArray[i];
			const TDesC& AttendeeName = pAttendee->CommonName();
			Printf(_L("Attendee:%S "), &AttendeeName);
			}
		}
	Printf(_L("\r\n"));
		
	}

//print the accepted options from the command line
void CCmdCalendar::PrintAcceptedOptions()
	{
	}


void CCmdCalendar::PrintTime(const TTime& aTime, TBool aNewline)
	{
	
	TTime NullTime = Time::NullTTime();
	if (aTime == NullTime) 
		{
		Printf(_L("(NullTime)"));
		}
	else
		{
		_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
		TDateTime dt = aTime.DateTime();
		Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
		}
	
	if (aNewline) Printf(_L("\r\n"));
	}


#define CASE_LIT(x) case x: { _LIT(KName, #x); return &KName; }


const TDesC* CCmdCalendar::RepeatTypeToString(TCalRRule::TType aType)
	{
	switch(aType)
		{
		CASE_LIT(TCalRRule::EInvalid)
		CASE_LIT(TCalRRule::EDaily)
		CASE_LIT(TCalRRule::EWeekly)
		CASE_LIT(TCalRRule::EMonthly)
		CASE_LIT(TCalRRule::EYearly)
		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

const TDesC* CCmdCalendar::EntryMethodToString(CCalEntry::TMethod aEntryMethod)
	{
	switch(aEntryMethod)
		{
		CASE_LIT(CCalEntry::EMethodNone)
		CASE_LIT(CCalEntry::EMethodPublish)
		CASE_LIT(CCalEntry::EMethodRequest)
		CASE_LIT(CCalEntry::EMethodReply)
		CASE_LIT(CCalEntry::EMethodAdd)
		CASE_LIT(CCalEntry::EMethodCancel)
		CASE_LIT(CCalEntry::EMethodRefresh)
		CASE_LIT(CCalEntry::EMethodCounter)
		CASE_LIT(CCalEntry::EMethodDeclineCounter)
		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

const TDesC* CCmdCalendar::EntryStatusToString(CCalEntry::TStatus aEntryStatus)
	{
	switch(aEntryStatus)
		{
		CASE_LIT(CCalEntry::ETentative)
		CASE_LIT(CCalEntry::EConfirmed)
		CASE_LIT(CCalEntry::ECancelled)
		CASE_LIT(CCalEntry::ETodoNeedsAction)
		CASE_LIT(CCalEntry::ETodoCompleted)
		CASE_LIT(CCalEntry::ETodoInProcess)
		CASE_LIT(CCalEntry::ENullStatus)
		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

const TDesC* CCmdCalendar::EntryTypeToString(CCalEntry::TType aEntryType)
	{
	switch(aEntryType)
		{
		CASE_LIT(CCalEntry::EAppt)
		CASE_LIT(CCalEntry::ETodo)
		CASE_LIT(CCalEntry::EEvent)
		CASE_LIT(CCalEntry::EReminder)
		CASE_LIT(CCalEntry::EAnniv)
		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

const TDesC* CCmdCalendar::UidName(TUid aUid)
	{
	switch (aUid.iUid)
		{

		default:
			{
			_LIT(KUnknown, "Unknown");
			return &KUnknown;
			}
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////


EXE_BOILER_PLATE(CCmdCalendar)

