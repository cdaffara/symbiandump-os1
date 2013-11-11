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
// Contains the CEtelSessionMgr class definition.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "cetelsessionmgr.h"
#include "cctsytestlogging.h"


/********************************************************************************************
* PLEASE READ WHEN UPDATING SUBSESSION SUPPORT
*
* Subsessions which provide notificaion support, e.g. RMobileLine::NotifyMobileLineStatusChange
* must be tidied up between tests. This means providing a method to consume any outstanding 
* notifications being held on the server for that subsession, and to cancel the internal repost
* mechanism within the Etel server. The steps are
* 1) Write a method along the lines of the methods in cetelnotifycleanup.cpp, e.g. 
*    CleanupMobileLineNotifyMobileLineStatusChange. The naming convention is 
*	 Cleanup<<subsession>><<api>>.
* 2) Add that function to the method which calls all the related methods for that subsession, in
*    the same file, e.g.CleanupMobileLineNotifiers. Add a new method for a new subsession type.
* 3) If a new subsession has been added, then the Cleanup<<subsession>>Notifiers method should be 
*    called from the appropriate place within the cascade of methods starting at TidyPhoneNotifiers
*    - e.g. TidyPhoneNotifiers calls TidyLineNotifiers for every line on the phone, which calls 
*	TidyCallNotifiers for it's dependent call subsessions and then CleanupLineNotifiers and 
*	CleanupMobileLineNotifiers. The naming convention is that TidyXXX forms part of the hierarchy of
*	subsession objects and Cleanup<<subsession>>Notifiers call the individual notification method 
*	cleanups.
**********************************************************************************************/

/**
 * Factory constructor.
*/
CEtelSessionMgr* CEtelSessionMgr::NewL(TName& aTsyName, TName& aPhoneName, TBool aKeepSessOpen)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::NewL aTsyName=%S aPhoneName=%S aKeepSessOpen=%d "), &aTsyName, &aPhoneName, aKeepSessOpen);
	CEtelSessionMgr* self = new(ELeave) CEtelSessionMgr(aTsyName, aPhoneName, aKeepSessOpen);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
/**
 * Constructor.
*/
CEtelSessionMgr::CEtelSessionMgr(TName& aTsyName, TName& aPhoneName, TBool aKeepSessOpen)
: iTsyName(aTsyName), iPhoneName(aPhoneName), iKeepSessOpen(aKeepSessOpen)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::CEtelSessionMgr aTsyName=%S aPhoneName=%S aKeepSessOpen=%d "), &aTsyName, &aPhoneName, aKeepSessOpen);
	iSmsStoreNames[KMeSmsStore] = &KETelMeSmsStore;
	iSmsStoreNames[KIccSmsStore] = &KETelIccSmsStore; 
	iSmsStoreNames[KCombinedSmsStore] = &KETelCombinedSmsStore;

	iPhoneBookStoreNames[KMeAdnPhoneBook].Copy(KETelMeAdnPhoneBook);
	iPhoneBookStoreNames[KMeDialledPhoneBook].Copy(KETelMeDialledPhoneBook);
	iPhoneBookStoreNames[KMeMissedPhoneBook].Copy(KETelMeMissedPhoneBook);
	iPhoneBookStoreNames[KMeReceivedPhoneBook].Copy(KETelMeReceivedPhoneBook);
	iPhoneBookStoreNames[KCombinedAdnPhoneBook].Copy(KETelCombinedAdnPhoneBook);
	iPhoneBookStoreNames[KTaAdnPhoneBook].Copy(KETelTaAdnPhoneBook);
	iPhoneBookStoreNames[KIccAdnPhoneBook].Copy(KETelIccAdnPhoneBook);
	iPhoneBookStoreNames[KIccFdnPhoneBook].Copy(KETelIccFdnPhoneBook);
	iPhoneBookStoreNames[KIccSdnPhoneBook].Copy(KETelIccSdnPhoneBook);
	iPhoneBookStoreNames[KIccBdnPhoneBook].Copy(KETelIccBdnPhoneBook);
	iPhoneBookStoreNames[KIccLndPhoneBook].Copy(KETelIccLndPhoneBook);
	iPhoneBookStoreNames[KIccVoiceMailBox].Copy(KETelIccVoiceMailBox);
	iPhoneBookStoreNames[KIccMbdnPhoneBook].Copy(KETelIccMbdnPhoneBook);

	}

/**
 * Second phase constructor.
*/
void CEtelSessionMgr::ConstructL()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::ConstructL"));
	iThreadExec = CThreadExec::NewL();
	}
	
/**
 * Destructor.
*/
CEtelSessionMgr::~CEtelSessionMgr()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::~CEtelSessionMgr"));
	delete iThreadExec;
	}

/**
* Post constructor open
*/
void CEtelSessionMgr::OpenL(TBool aAutoOpenSess)
	{
	TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::OpenL aAutoOpenSess=%d"), aAutoOpenSess);
	if (iIsOpen || !aAutoOpenSess)
		{
		// Close all sessions if :
		// - the Session Mgr was not closed properly
		// - the caller doesn't want any sessions open
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::OpenL closing all sessions"));
		CThreadExec::TFunctorP1<CEtelSessionMgr,TBool> fn(this,&CEtelSessionMgr::DoCloseAllSessL,ETrue);
		iThreadExec->Exec(&fn);
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::OpenL done closing all sessions"));
		iAutoSessionOpened = EFalse;
		}

	if (aAutoOpenSess && !iAutoSessionOpened)
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::OpenL opening all sessions"));
		// Open auto session if requested and if they are not open yet
		CThreadExec::TFunctor<CEtelSessionMgr> fn(this,&CEtelSessionMgr::DoOpenAutoSessL);
		User::LeaveIfError(iThreadExec->Exec(&fn));
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::OpenL done opening all sessions"));
		iAutoSessionOpened = ETrue;
		}

	iIsOpen = ETrue;
	}
	
/** 
* Close the subsession manager
*/
void CEtelSessionMgr::Close()
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::Close iAutoSessionOpened=%d iIsOpen=%d"), iAutoSessionOpened, iIsOpen);
	// Close only manually opened sessions if iKeepSessOpen, all session else
	CThreadExec::TFunctorP1<CEtelSessionMgr,TBool> fn(this,&CEtelSessionMgr::DoCloseAllSessL,!iKeepSessOpen);
	TInt err = iThreadExec->Exec(&fn);
	iAutoSessionOpened = iKeepSessOpen;
	iIsOpen = EFalse;
	}


/**
* Force a close on the session manager including auto opened sessions 
* so that the server is unloaded, then re-open
*/
void CEtelSessionMgr::RestartL()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::RestartL"));
	TBool savedKeepFlag = iKeepSessOpen;
	iKeepSessOpen = EFalse;
	
	Close();
	
	iKeepSessOpen = savedKeepFlag;
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::RestartL calling OpenL"));
	OpenL(savedKeepFlag);
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::RestartL after OpenL"));
	}


/**
* Get a server subsubsession
*/
RTelServer& CEtelSessionMgr::GetServerL(TServerId aServerId)
	{
	TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::GetServerL aServerId=%d"), aServerId);
	
	if (iTelServersStatus[aServerId] == ESessClosed)
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::GetServerL iTelServersStatus[aServerId] == ESessClosed"));
		CThreadExec::TFunctorP1<CEtelSessionMgr, TServerId> fn(this,&CEtelSessionMgr::DoOpenServerL,aServerId);
		TInt err = iThreadExec->Exec(&fn);
		TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::GetServerL err=%d"), err);
		User::LeaveIfError(err);
		iTelServersStatus[aServerId] = ESessOpenManually;
		}
		
	return iTelServers[aServerId];
	}

/**
* Get a RMobilePhone subsession
*/
RMobilePhone& CEtelSessionMgr::GetPhoneL(TServerId aServerId, TPhoneId aPhoneId)
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::GetPhoneL aServerId=%d aPhoneId=%d"), aServerId, aPhoneId);
	
	if (iTelServersStatus[aServerId] == ESessClosed)
		GetServerL(aServerId); // open parent session if required
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		{
		CThreadExec::TFunctorP2<CEtelSessionMgr, TServerId, TPhoneId> fn(this,&CEtelSessionMgr::DoOpenPhoneL,aServerId,aPhoneId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iPhonesStatus[aServerId][aPhoneId] = ESessOpenManually;
		}
		
	return iPhones[aServerId][aPhoneId];
	}

/**
* Get a RMobileLine subsession
*/
RMobileLine& CEtelSessionMgr::GetLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetLineL aServerId=%d aPhoneId=%d aLine=%d"), aServerId, aPhoneId, aLineId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if require

	if (iLinesStatus[aServerId][aPhoneId][aLineId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TLineId> fn(this,&CEtelSessionMgr::DoOpenLineL,aServerId,aPhoneId,aLineId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iLinesStatus[aServerId][aPhoneId][aLineId] = ESessOpenManually;
		}
	return iLines[aServerId][aPhoneId][aLineId];
	}

/**
* Get a RMobileCall subsession
*/
RMobileCall& CEtelSessionMgr::GetCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::GetCallL aServerId=%d aPhoneId=%d aLine=%d aCallId=%d"), aServerId, aPhoneId, aLineId, aCallId);
	
	if (iLinesStatus[aServerId][aPhoneId][aLineId] == ESessClosed)
		GetLineL(aServerId, aPhoneId, aLineId); // open parent session if require

	if (iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] == ESessClosed)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TLineId, TCallId> fn(this,&CEtelSessionMgr::DoOpenCallL,aServerId,aPhoneId,aLineId,aCallId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] = ESessOpenManually;
		}
		
	return iCalls[aServerId][aPhoneId][aLineId][aCallId];
	}


/**
* Get a RMobileCall subsession that has been assigned to an incoming call represented by name.
*/
RMobileCall& CEtelSessionMgr::GetIncomingCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, const TName& aName, TCallId& aCallId)
	{
	TCallId callId = KCall3;
	
	while(!(callId==KMaxCalls || (iCallsRefs[aLineId][callId].GetCount()==0 && iCallsStatus[aServerId][aPhoneId][aLineId][callId]==ESessClosed)))
		{
		callId = (TCallId)(callId +1);
		}
	
	if(callId == KMaxCalls)
		{
		User::Leave(KErrInUse);
		}

	if (iLinesStatus[aServerId][aPhoneId][aLineId] == ESessClosed)
		GetLineL(aServerId, aPhoneId, aLineId); // open parent session if required
	
	CThreadExec::TFunctorP5 <CEtelSessionMgr, TServerId, TPhoneId, TLineId, TCallId, const TName&> fn(this,&CEtelSessionMgr::DoAssignCallToIncomingL,aServerId,aPhoneId,aLineId,callId,aName);
	TInt err = iThreadExec->Exec(&fn);
	User::LeaveIfError(err);
	iCallsStatus[aServerId][aPhoneId][aLineId][callId] = ESessOpenManually;

	aCallId = callId;
	return iCalls[aServerId][aPhoneId][aLineId][callId];
	}
	
	

/**
* Get a RFax subsession
*/
RFax& CEtelSessionMgr::GetFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId)
	{
	
	if (iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] == ESessClosed)
		GetCallL(aServerId, aPhoneId, aLineId, aCallId); // open parent session if require

	if (iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId] == ESessClosed)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TLineId, TCallId, TFaxId> 
						fn(this,&CEtelSessionMgr::DoOpenFaxL,aServerId,aPhoneId,aLineId,aCallId,aFaxId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId] = ESessOpenManually;
		}
		
	return iFaxes[aServerId][aPhoneId][aLineId][aCallId][aFaxId];
	}
	

/**
* Get a RMobileConferenceCall subsession
*/
RMobileConferenceCall& CEtelSessionMgr::GetConferenceCallL(TServerId aServerId, TPhoneId aPhoneId,
														TConferenceCallId aConferenceCallId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetConferenceCallL aServerId=%d aPhoneId=%d aConferenceCallId=%d"), aServerId, aPhoneId, aConferenceCallId);

	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iConferenceCallsStatus[aServerId][aPhoneId][aConferenceCallId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TConferenceCallId>
								fn(this,&CEtelSessionMgr::DoOpenConferenceCallL,aServerId,aPhoneId,aConferenceCallId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iConferenceCallsStatus[aServerId][aPhoneId][aConferenceCallId] = ESessOpenManually;
		}
		
	return iConferenceCalls[aServerId][aPhoneId][aConferenceCallId];
	}


/**
* Get a RPacketService subsession
*/
RPacketService& CEtelSessionMgr::GetPacketServiceL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetPacketServiceL aServerId=%d aPhoneId=%d aPacketServiceId=%d"), aServerId, aPhoneId, aPacketServiceId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId> fn(this,&CEtelSessionMgr::DoOpenPacketServiceL,aServerId,aPhoneId,aPacketServiceId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] = ESessOpenManually;
		}
		
	return iPacketServices[aServerId][aPhoneId][aPacketServiceId];
	}
	
	
/**
* Get a primary RPacketContext subsession
*/
RPacketContext& CEtelSessionMgr::GetPrimaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId,
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::GetPrimaryPacketContextL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	
	if (iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] == ESessClosed)
		GetPacketServiceL( aServerId, aPhoneId, aPacketServiceId );
	
	if (iPrimaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] == ESessClosed)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, TPrimaryPacketContextId>
							fn(this,&CEtelSessionMgr::DoOpenPrimaryPacketContextL,aServerId,aPhoneId,aPacketServiceId,aPrimaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iPrimaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] = ESessOpenManually;
		}
		
	return iPrimaryPacketContexts[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	}


/**
* Get a RPacketQoS subsession for a primary packet context
*/
RPacketQoS& CEtelSessionMgr::GetPrimaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId,
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::GetPrimaryPacketContextQoSL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);	
	
	if (iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] == ESessClosed)
		GetPacketServiceL( aServerId, aPhoneId, aPacketServiceId );
	
	if (iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] == ESessClosed)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, TPrimaryPacketContextId>
							fn(this,&CEtelSessionMgr::DoOpenPrimaryPacketContextQoSL,aServerId,aPhoneId,aPacketServiceId,aPrimaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] = ESessOpenManually;
		}
		
	return iPrimaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	}


/**
* Get a secondary RPacketContext subsession
*/
RPacketContext& CEtelSessionMgr::GetSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId,
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	if (iPrimaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] == ESessClosed)
		GetPrimaryPacketContextL( aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId );
	
	if (iSecondaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId]
											[aPrimaryPacketContextId][aSecondaryPacketContextId] == ESessClosed)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, 
													TPrimaryPacketContextId,TSecondaryPacketContextId>
							fn(this,&CEtelSessionMgr::DoOpenSecondaryPacketContextL,aServerId,aPhoneId,aPacketServiceId,
																aPrimaryPacketContextId,aSecondaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iSecondaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId]
						[aPrimaryPacketContextId][aSecondaryPacketContextId] = ESessOpenManually;
		}
		
	return iSecondaryPacketContexts[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId][aSecondaryPacketContextId];
	}
	
/**
* Get a RPacketQoS subsession for a secondary packet context
*/
RPacketQoS& CEtelSessionMgr::GetSecondaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId,
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	if (iSecondaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId]
											[aPrimaryPacketContextId][aSecondaryPacketContextId] == ESessClosed)
											
		GetSecondaryPacketContextL( aServerId, aPhoneId, aPacketServiceId, 
											aPrimaryPacketContextId,aSecondaryPacketContextId );
	
	if (iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId]
											[aPrimaryPacketContextId][aSecondaryPacketContextId] == ESessClosed)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, 
													TPrimaryPacketContextId,TSecondaryPacketContextId>
							fn(this,&CEtelSessionMgr::DoOpenSecondaryPacketContextQoSL,aServerId,aPhoneId,aPacketServiceId,
																aPrimaryPacketContextId,aSecondaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId]
						[aPrimaryPacketContextId][aSecondaryPacketContextId] = ESessOpenManually;
		}
		
	return iSecondaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId][aSecondaryPacketContextId];
	}

/**
* Get a RMobileONStore subsession
*/
RMobileONStore& CEtelSessionMgr::GetONStoreL(TServerId aServerId, TPhoneId aPhoneId,TONStoreId aONStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetONStoreL aServerId=%d aPhoneId=%d aONStoreId=%d"), aServerId, aPhoneId, aONStoreId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iONStoresStatus[aServerId][aPhoneId][aONStoreId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TONStoreId> 
					fn(this,&CEtelSessionMgr::DoOpenONStoreL,aServerId,aPhoneId,aONStoreId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iONStoresStatus[aServerId][aPhoneId][aONStoreId] = ESessOpenManually;
		}
		
	return iONStores[aServerId][aPhoneId][aONStoreId];
	}

/**
* Get a RMobileENStore subsession
*/
RMobileENStore& CEtelSessionMgr::GetENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetENStoreL aServerId=%d aPhoneId=%d aENStoreId=%d"), aServerId, aPhoneId, aENStoreId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iENStoresStatus[aServerId][aPhoneId][aENStoreId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TENStoreId> 
					fn(this,&CEtelSessionMgr::DoOpenENStoreL,aServerId,aPhoneId,aENStoreId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iENStoresStatus[aServerId][aPhoneId][aENStoreId] = ESessOpenManually;
		}
		
	return iENStores[aServerId][aPhoneId][aENStoreId];
	}

/**
* Get a RMobilePhoneBookStore subsession
*/
RMobilePhoneBookStore& CEtelSessionMgr::GetPhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId,TPhoneBookStoreId aPhoneBookStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetPhoneBookStoreL aServerId=%d aPhoneId=%d aPhoneBookStoreId=%d"), aServerId, aPhoneId, aPhoneBookStoreId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookStoreId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TPhoneBookStoreId> 
					fn(this,&CEtelSessionMgr::DoOpenPhoneBookStoreL,aServerId,aPhoneId,aPhoneBookStoreId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookStoreId] = ESessOpenManually;
		}
		
	return iPhoneBookStores[aServerId][aPhoneId][aPhoneBookStoreId];
	}

/**
* Get a RMobileSmsStore subsession
*/
RMobileSmsStore& CEtelSessionMgr::GetSmsStoreL(TServerId aServerId, TPhoneId aPhoneId,
											TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::GetSmsStoreL aServerId=%d aPhoneId=%d aSmsMessagingId=%d aSmsStoreId=%d"), aServerId, aPhoneId, aSmsMessagingId, aSmsStoreId);
	
	if (iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMessagingId] == ESessClosed)
		GetSmsMessagingL(aServerId, aPhoneId,aSmsMessagingId ); // open parent session if required

	if (iSmsStoresStatus[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId] == ESessClosed)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TSmsMessagingId, TSmsStoreId>
					fn(this,&CEtelSessionMgr::DoOpenSmsStoreL,aServerId,aPhoneId,aSmsMessagingId,aSmsStoreId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iSmsStoresStatus[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId] = ESessOpenManually;
		}
		
	return iSmsStores[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId];
	}

/**
* Get a RMobileSmsMessaging subsession
*/
RMobileSmsMessaging& CEtelSessionMgr::GetSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId,TSmsMessagingId aSmsMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetSmsStoreL aServerId=%d aPhoneId=%d aSmsMessagingId=%d"), aServerId, aPhoneId, aSmsMessagingId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMessagingId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TSmsMessagingId> 
					fn(this,&CEtelSessionMgr::DoOpenSmsMessagingL,aServerId,aPhoneId,aSmsMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMessagingId] = ESessOpenManually;
		}
		
	return iSmsMessagings[aServerId][aPhoneId][aSmsMessagingId];
	}

/**
* Get a RMobileBroadcastMessaging subsession
*/
RMobileBroadcastMessaging& CEtelSessionMgr::GetBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId,TBroadcastMessagingId aBroadcastMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetBroadcastMessagingL aServerId=%d aPhoneId=%d aBroadcastMessagingId=%d"), aServerId, aPhoneId, aBroadcastMessagingId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iBroadcastMessagingsStatus[aServerId][aPhoneId][aBroadcastMessagingId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TBroadcastMessagingId> 
					fn(this,&CEtelSessionMgr::DoOpenBroadcastMessagingL,aServerId,aPhoneId,aBroadcastMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iBroadcastMessagingsStatus[aServerId][aPhoneId][aBroadcastMessagingId] = ESessOpenManually;
		}
		
	return iBroadcastMessagings[aServerId][aPhoneId][aBroadcastMessagingId];
	}

/**
* Get a RMobileUssdMessaging subsession
*/
RMobileUssdMessaging& CEtelSessionMgr::GetUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId,TUssdMessagingId aUssdMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetUssdMessagingL aServerId=%d aPhoneId=%d aUssdMessagingId=%d"), aServerId, aPhoneId, aUssdMessagingId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMessagingId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TUssdMessagingId> 
					fn(this,&CEtelSessionMgr::DoOpenUssdMessagingL,aServerId,aPhoneId,aUssdMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMessagingId] = ESessOpenManually;
		}
		
	return iUssdMessagings[aServerId][aPhoneId][aUssdMessagingId];
	}

/**
* Get a RMmCustomAPI subsession
*/
RMmCustomAPI& CEtelSessionMgr::GetCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
    {
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::GetCustomApiL aServerId=%d aPhoneId=%d aCustomIpcId=%d"), aServerId, aPhoneId, aCustomApiId);
	
	if (iPhonesStatus[aServerId][aPhoneId] == ESessClosed)
		GetPhoneL(aServerId, aPhoneId); // open parent session if required

	if (iCustomApisStatus[aServerId][aPhoneId][aCustomApiId] == ESessClosed)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TCustomApiId> 
					fn(this,&CEtelSessionMgr::DoOpenCustomApiL,aServerId,aPhoneId,aCustomApiId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iCustomApisStatus[aServerId][aPhoneId][aCustomApiId] = ESessOpenManually;
		}

	return iCustomApis[aServerId][aPhoneId][aCustomApiId];
    }

/**
* Release a server object
*/
void CEtelSessionMgr::ReleaseServer(TServerId aServerId)
	{
	TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::ReleaseServer aServerId=%d"), aServerId);
	if (iTelServersStatus[aServerId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP1<CEtelSessionMgr, TServerId> fn(this,&CEtelSessionMgr::DoCloseServerL,aServerId);
		TInt err = iThreadExec->Exec(&fn);
		iTelServersStatus[aServerId] = ESessClosed;
		}
	}

/**
* Release a phone object
*/
void CEtelSessionMgr::ReleasePhone(TServerId aServerId, TPhoneId aPhoneId)
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::ReleasePhone aServerId=%d aPhoneId=%d"), aServerId, aPhoneId);
	if (iPhonesStatus[aServerId][aPhoneId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP2<CEtelSessionMgr, TServerId, TPhoneId> fn(this,&CEtelSessionMgr::DoClosePhoneL,aServerId,aPhoneId);
		TInt err = iThreadExec->Exec(&fn);
		iPhonesStatus[aServerId][aPhoneId] = ESessClosed;
		}
	}

/**
* Release a line object
*/
void CEtelSessionMgr::ReleaseLine(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseLine aServerId=%d aPhoneId=%d aLineId=%d"), aServerId, aPhoneId, aLineId);
	if (iLinesStatus[aServerId][aPhoneId][aLineId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TLineId> fn(this,&CEtelSessionMgr::DoCloseLineL,aServerId,aPhoneId,aLineId);
		TInt err = iThreadExec->Exec(&fn);
		iLinesStatus[aServerId][aPhoneId][aLineId] = ESessClosed;
		}
	}

/**
* Release a call object
*/
void CEtelSessionMgr::ReleaseCall(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::ReleaseCall aServerId=%d aPhoneId=%d aLineId=%d"), aServerId, aPhoneId, aLineId, aCallId);
	if (iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TLineId, TCallId> fn(this,&CEtelSessionMgr::DoCloseCallL,aServerId,aPhoneId,aLineId,aCallId);
		TInt err = iThreadExec->Exec(&fn);
		iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] = ESessClosed;
		}
	}

/**
* Release a conference call object
*/
	
void CEtelSessionMgr::ReleaseConferenceCall( TServerId aServerId, TPhoneId aPhoneId,TConferenceCallId aConferenceCallId )
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseConferenceCall aServerId=%d aPhoneId=%d aConferenceCallId=%d"), aServerId, aPhoneId, aConferenceCallId);
	if (iConferenceCallsStatus[aServerId][aPhoneId][aConferenceCallId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TConferenceCallId> 
								fn(this,&CEtelSessionMgr::DoCloseConferenceCallL,aServerId,aPhoneId,aConferenceCallId);
		TInt err = iThreadExec->Exec(&fn);
		iConferenceCallsStatus[aServerId][aPhoneId][aConferenceCallId] = ESessClosed;
		}
	}

/**
* Release a Fax object
*/	
void CEtelSessionMgr::ReleaseFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::ReleaseFax aServerId=%d aPhoneId=%d aLineId=%d aCallId=%d aFaxId=%d"), aServerId, aPhoneId, aLineId, aCallId, aFaxId);
	if (iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TLineId, TCallId, TFaxId> 
						fn(this,&CEtelSessionMgr::DoCloseFaxL,aServerId,aPhoneId,aLineId,aCallId,aFaxId);
		TInt err = iThreadExec->Exec(&fn);
		User::LeaveIfError(err);
		iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId] = ESessClosed;
		}
			
	}

/**
* Release the a service object
*/
void CEtelSessionMgr::ReleasePacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleasePacketService aServerId=%d aPhoneId=%d aPacketServiceId=%d"), aServerId, aPhoneId, aPacketServiceId);
	if (iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId> fn(this,&CEtelSessionMgr::DoClosePacketServiceL,aServerId,aPhoneId,aPacketServiceId);
		TInt err = iThreadExec->Exec(&fn);
		iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId] = ESessClosed;
		}
	}
	

/**
* Release a primary packet context object
*/
void CEtelSessionMgr::ReleasePrimaryPacketContext(TServerId aServerId,
											TPhoneId aPhoneId, 
											TPacketServiceId aPacketServiceId, 
											TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::ReleasePrimaryPacketContext aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	if (iPrimaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, TPrimaryPacketContextId> 
							fn(this,&CEtelSessionMgr::DoClosePrimaryPacketContextL,aServerId,aPhoneId,aPacketServiceId, aPrimaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		iPrimaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] = ESessClosed;
		}
	}

/**
* Release the PacketContextQoS object for a primary packet context
*/
void CEtelSessionMgr::ReleasePrimaryPacketContextQoS(TServerId aServerId,
											TPhoneId aPhoneId, 
											TPacketServiceId aPacketServiceId, 
											TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::ReleasePrimaryPacketContextQoS aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	if (iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, TPrimaryPacketContextId> 
							fn(this,&CEtelSessionMgr::DoClosePrimaryPacketContextQoSL,aServerId,aPhoneId,aPacketServiceId, aPrimaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId] = ESessClosed;
		}
	}

	
/**
* Release a secondary packet context object
*/
void CEtelSessionMgr::ReleaseSecondaryPacketContext(TServerId aServerId,
											TPhoneId aPhoneId, 
											TPacketServiceId aPacketServiceId, 
											TPrimaryPacketContextId aPrimaryPacketContextId,
											TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::ReleaseSecondaryPacketContext aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	if (iSecondaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId]
								[aPrimaryPacketContextId][aSecondaryPacketContextId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, 
										TPrimaryPacketContextId,TSecondaryPacketContextId> 
							fn(this,&CEtelSessionMgr::DoCloseSecondaryPacketContextL,aServerId,aPhoneId,aPacketServiceId, 
													aPrimaryPacketContextId,aSecondaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		iSecondaryPacketContextsStatus[aServerId][aPhoneId][aPacketServiceId]
										[aPrimaryPacketContextId][aSecondaryPacketContextId] = ESessClosed;
		}
	}

/**
* Release the PacketContextQoS object for a primary packet context
*/
void CEtelSessionMgr::ReleaseSecondaryPacketContextQoS(TServerId aServerId,
											TPhoneId aPhoneId, 
											TPacketServiceId aPacketServiceId, 
											TPrimaryPacketContextId aPrimaryPacketContextId,
											TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::ReleaseSecondaryPacketContextQoS aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	if (iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId]
								[aPrimaryPacketContextId][aSecondaryPacketContextId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP5<CEtelSessionMgr, TServerId, TPhoneId, TPacketServiceId, 
										TPrimaryPacketContextId,TSecondaryPacketContextId> 
							fn(this,&CEtelSessionMgr::DoCloseSecondaryPacketContextQoSL,aServerId,aPhoneId,aPacketServiceId, 
													aPrimaryPacketContextId,aSecondaryPacketContextId);
		TInt err = iThreadExec->Exec(&fn);
		iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId][aPacketServiceId]
										[aPrimaryPacketContextId][aSecondaryPacketContextId] = ESessClosed;
		}
	}
	

/**
* Release an ONStore object
*/
void CEtelSessionMgr::ReleaseONStore(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseONStore aServerId=%d aPhoneId=%d aONStoreId=%d"), aServerId, aPhoneId, aONStoreId);
	if (iONStoresStatus[aServerId][aPhoneId][aONStoreId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TONStoreId> fn(this,&CEtelSessionMgr::DoCloseONStoreL,aServerId,aPhoneId,aONStoreId);
		TInt err = iThreadExec->Exec(&fn);
		iONStoresStatus[aServerId][aPhoneId][aONStoreId] = ESessClosed;
		}
	}
	
/**
* Release an ENStore object
*/
void CEtelSessionMgr::ReleaseENStore(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseENStore aServerId=%d aPhoneId=%d aENStoreId=%d"), aServerId, aPhoneId, aENStoreId);
	if (iENStoresStatus[aServerId][aPhoneId][aENStoreId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TENStoreId> fn(this,&CEtelSessionMgr::DoCloseENStoreL,aServerId,aPhoneId,aENStoreId);
		TInt err = iThreadExec->Exec(&fn);
		iENStoresStatus[aServerId][aPhoneId][aENStoreId] = ESessClosed;
		}
	}
	
/**
* Release a PhoneBookStore object
*/
void CEtelSessionMgr::ReleasePhoneBookStore(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleasePhoneBookStore aServerId=%d aPhoneId=%d aPhoneBookStoreId=%d"), aServerId, aPhoneId, aPhoneBookStoreId);
	if (iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookStoreId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TPhoneBookStoreId> fn(this,&CEtelSessionMgr::DoClosePhoneBookStoreL,aServerId,aPhoneId,aPhoneBookStoreId);
		TInt err = iThreadExec->Exec(&fn);
		iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookStoreId] = ESessClosed;
		}
	}
	
/**
* Release an SMS Store object
*/
void CEtelSessionMgr::ReleaseSmsStore(TServerId aServerId, TPhoneId aPhoneId, 
											TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::ReleaseSmsStore aServerId=%d aPhoneId=%d aSmsMessagingId=%d aSmsStoreId=%d"), aServerId, aPhoneId, aSmsMessagingId, aSmsStoreId);
	if (iSmsStoresStatus[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP4<CEtelSessionMgr, TServerId, TPhoneId, TSmsMessagingId, TSmsStoreId> 
									fn(this,&CEtelSessionMgr::DoCloseSmsStoreL,aServerId,aPhoneId,aSmsMessagingId,aSmsStoreId);
		TInt err = iThreadExec->Exec(&fn);
		iSmsStoresStatus[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId] = ESessClosed;
		}
	}
	
/**
* Release an SMS Messaging object
*/
void CEtelSessionMgr::ReleaseSmsMessaging(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseSmsMessaging aServerId=%d aPhoneId=%d aSmsMessagingId=%d"), aServerId, aPhoneId, aSmsMessagingId);
	if (iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMessagingId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TSmsMessagingId> fn(this,&CEtelSessionMgr::DoCloseSmsMessagingL,aServerId,aPhoneId,aSmsMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMessagingId] = ESessClosed;
		}
	}
	
/**
* Release a broadcast messaging object
*/
void CEtelSessionMgr::ReleaseBroadcastMessaging(TServerId aServerId, TPhoneId aPhoneId, TBroadcastMessagingId aBroadcastMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseBroadcastMessaging aServerId=%d aPhoneId=%d aBroadcastMessagingId=%d"), aServerId, aPhoneId, aBroadcastMessagingId);
	if (iBroadcastMessagingsStatus[aServerId][aPhoneId][aBroadcastMessagingId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TBroadcastMessagingId> fn(this,&CEtelSessionMgr::DoCloseBroadcastMessagingL,aServerId,aPhoneId,aBroadcastMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		iBroadcastMessagingsStatus[aServerId][aPhoneId][aBroadcastMessagingId] = ESessClosed;
		}
	}
	
/**
* Release a UssdMessaging object
*/
void CEtelSessionMgr::ReleaseUssdMessaging(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseUssdMessaging aServerId=%d aPhoneId=%d aUssdMessagingId=%d"), aServerId, aPhoneId, aUssdMessagingId);
	if (iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMessagingId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TUssdMessagingId> fn(this,&CEtelSessionMgr::DoCloseUssdMessagingL,aServerId,aPhoneId,aUssdMessagingId);
		TInt err = iThreadExec->Exec(&fn);
		iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMessagingId] = ESessClosed;
		}
	}
	
/**
* Release a RMmCustomAPI object
*/
void CEtelSessionMgr::ReleaseCustomApi(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
{
    TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::ReleaseCustomApi aServerId=%d aPhoneId=%d aCustomApiId=%d"), aServerId, aPhoneId, aCustomApiId);
	if (iCustomApisStatus[aServerId][aPhoneId][aCustomApiId] == ESessOpenManually)
		{
		CThreadExec::TFunctorP3<CEtelSessionMgr, TServerId, TPhoneId, TCustomApiId> fn(this,&CEtelSessionMgr::DoCloseCustomApiL,aServerId,aPhoneId,aCustomApiId);
		TInt err = iThreadExec->Exec(&fn);
		iCustomApisStatus[aServerId][aPhoneId][aCustomApiId] = ESessClosed;
		}
    }


//
// The following DoXXXMethods are called on the test suite server thread not on
// the test client thread
//

/**
* Called when OpenL is called to open the main subsessions 
*/
void CEtelSessionMgr::DoOpenAutoSessL()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenAutoSessL"));
	// only open the main sessions
	DoOpenServerL(KMainServer);
	iTelServersStatus[KMainServer] = ESessOpenAutomatically;
	DoOpenPhoneL(KMainServer,KMainPhone); 
	iPhonesStatus[KMainServer][KMainPhone] = ESessOpenAutomatically;
	// open all existings lines
	for (TLineId lineId = KVoiceLine; lineId < KMaxLines; lineId = TLineId(lineId+1))
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenAutoSessL open existing lines"));
		if (iLinesName[lineId].Length() >0)
			{
			DoOpenLineL(KMainServer,KMainPhone,lineId)	;	
			iLinesStatus[KMainServer][KMainPhone][lineId] = ESessOpenAutomatically;
			}
		}
	// open 2 calls on the main voice line (assume there is always a voice line)
	DoOpenCallL(KMainServer,KMainPhone,KVoiceLine,KCall1);
	iCallsStatus[KMainServer][KMainPhone][KVoiceLine][KCall1] = ESessOpenAutomatically;
	DoOpenCallL(KMainServer,KMainPhone,KVoiceLine,KCall2);
	iCallsStatus[KMainServer][KMainPhone][KVoiceLine][KCall2] = ESessOpenAutomatically; 
	}

/**
* Close all subsessions post test and prior to re-opening
*/
void CEtelSessionMgr::DoCloseAllSessL(TBool aIncludeAutoOpenedSess)
	{
	TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoCloseAllSessL aIncludeAutoOpenedSess=%d"), aIncludeAutoOpenedSess);
	// if aIncludeAutoOpenedSess, close sessions with staus ESessOpenAutomatically and ESessOpenManually
	// else close only sessions with status ESessOpenManually

	// comparison made is if( status >= iStatusCloseThresh)
	iStatusCloseThresh = aIncludeAutoOpenedSess ? ESessOpenAutomatically : ESessOpenManually;
	
	// do some cleanup before closing sessions
	DoCleanup();	
	
	for (TServerId serverId = KMainServer; serverId < KMaxServers; serverId = TServerId(serverId+1))
		{
		for (TPhoneId phoneId = KMainPhone; phoneId < KMaxPhones; phoneId = TPhoneId(phoneId+1))
			{
			ClosePhoneSessL( serverId, phoneId );
			}
			
		// close servers
		if (iTelServersStatus[serverId] >= iStatusCloseThresh)
			{
			DoCloseServerL(serverId);
			iTelServersStatus[serverId] = ESessClosed;
			}
		}
	}


//
// The following DoXXXMethods are called on the test suite server thread not on
// the test client thread
//


/**
* Cleanup all sessions and bring them back to the default configuration 
* ** default configuration to be defined***
*/	
void CEtelSessionMgr::DoCleanup()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoCleanup"));
	CleanupPacketService();
	CleanupPhoneService();
	}

/**
* Cleanup phone service
*/
void CEtelSessionMgr::CleanupPhoneService()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPhoneService"));

	// Phone cleanup
	RMobilePhone* phone = NULL;
	
	TServerId serverId = KMainServer;
	while (serverId < KMaxServers && phone == NULL) 
		{
		TPhoneId phoneId = KMainPhone;
		while (phoneId < KMaxPhones && phone == NULL)
			{
			if( iPhonesStatus[serverId][phoneId] != ESessClosed )
				{
				phone = &iPhones[serverId][phoneId];
				
				TidyPhoneNotifiers( serverId, phoneId );
				
				}
			phoneId = TPhoneId(phoneId+1);
			}
		serverId = TServerId(serverId+1);
		}
	
	// a phone session is open	
	if (phone)
		{
		TRequestStatus reqStatus;
		// terminate all calls
		phone->TerminateAllCalls(reqStatus);
		User::WaitForRequest(reqStatus);
		
		}
	}

/**
* Cleanup packet service
*/
void CEtelSessionMgr::CleanupPacketService()
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::CleanupPacketService"));

	RPacketService* packetService = NULL;

	TServerId serverId = KMainServer;
	while (serverId < KMaxServers && packetService == NULL) 
		{
		TPhoneId phoneId = KMainPhone;
		while (phoneId < KMaxPhones && packetService == NULL)
			{
			TPacketServiceId packetServiceId = KMainPacketService; 
			while (packetServiceId < KMaxPacketService && packetService == NULL)
				{
				if (iPacketServicesStatus[serverId][phoneId][packetServiceId] != ESessClosed)
					{
					packetService = &iPacketServices[serverId][phoneId][packetServiceId];
					}
				packetServiceId = TPacketServiceId(packetServiceId+1);
				}
			phoneId = TPhoneId(phoneId+1);
			}
		serverId = TServerId(serverId+1);
		}
	
	if (packetService)
		{
		RPacketService::TStatus packetServiceStatus;
		TInt err = packetService->GetStatus(packetServiceStatus);
		if (packetServiceStatus != RPacketService::EStatusUnattached)
			{
			TRequestStatus status;
			packetService->Detach(status);
			User::WaitForRequest(status);
			}
		}	
	}



//
// The following DoOpenXXXMethods are called on the test suite server thread not on
// the test client thread
//

/**
* Open the server subsession
*/
void CEtelSessionMgr::DoOpenServerL(TServerId aServerId)
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenServerL"));
	User::LeaveIfError(iTelServers[aServerId].Connect());
	CleanupClosePushL(iTelServers[aServerId]);
	iTelServers[aServerId].ShareAuto();
	User::LeaveIfError(iTelServers[aServerId].SetExtendedErrorGranularity(RTelServer::EErrorExtended));
	CleanupStack::Pop(&iTelServers[aServerId]);
	User::LeaveIfError(iTelServers[aServerId].LoadPhoneModule(iTsyName));
	}
	
/**
* Close the server
*/
void CEtelSessionMgr::DoCloseServerL(TServerId aServerId)
	{
	TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoCloseServerL aServerId=%d"), aServerId);
	iTelServers[aServerId].UnloadPhoneModule(iTsyName);
	iTelServers[aServerId].Close();
	}

/**
* Open a phone subsession
*/
void CEtelSessionMgr::DoOpenPhoneL(TServerId aServerId, TPhoneId aPhoneId)
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::DoOpenPhoneL aServerId=%d aPhoneId=%d"), aServerId, aPhoneId);
	TInt err = iPhones[aServerId][aPhoneId].Open(iTelServers[aServerId], iPhoneName);
	TEST_FRAMEWORK_LOG2(_L("iPhones[][].Open err=%d"), err);
	User::LeaveIfError(err);
	if (!iHaveLineNames)
		DoGetLineNamesL(iPhones[aServerId][aPhoneId]);
	}
	
/**
* Close a phone subsession
*/
void CEtelSessionMgr::DoClosePhoneL(TServerId aServerId, TPhoneId aPhoneId)
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::DoClosePhoneL aServerId=%d aPhoneId=%d"), aServerId, aPhoneId);
	iPhones[aServerId][aPhoneId].Close();
	}

/**
* Open a line subsession
*/
void CEtelSessionMgr::DoOpenLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId)
	{
	TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::DoOpenLineL aServerId=%d aPhoneId=%d"), aServerId, aPhoneId);
	User::LeaveIfError(iLines[aServerId][aPhoneId][aLineId].Open(iPhones[aServerId][aPhoneId], iLinesName[aLineId]));
	}
	
/**
* Close a line subsession
*/
void CEtelSessionMgr::DoCloseLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseLineL aServerId=%d aPhoneId=%d aLineId=%d"), aServerId, aPhoneId, aLineId);
	iLines[aServerId][aPhoneId][aLineId].Close();
	}

/**
* Open a call subsession
*/
void CEtelSessionMgr::DoOpenCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoOpenCallL aServerId=%d aPhoneId=%d aLineId=%d aCallId=%d"), aServerId, aPhoneId, aLineId, aCallId);

	RMobileCall& call = iCalls[aServerId][aPhoneId][aLineId][aCallId];
	RMobileLine& line = iLines[aServerId][aPhoneId][aLineId];
	TRefCountItem& refCount = iCallsRefs[aLineId][aCallId];
	
	TName callName;
	if( refCount.GetCount() )
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenCallL calling RCall::OpenExistingCall"));
		callName = refCount.GetName();
		User::LeaveIfError( call.OpenExistingCall( line, callName) );
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenCallL called RCall::OpenExistingCall"));
		}
	else
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenCallL calling RCall::OpenNewCall"));
		User::LeaveIfError(call.OpenNewCall(line, callName));
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoOpenCallL called RCall::OpenNewCall"));
		}
	refCount.AddRef( callName );
	}
	
	
/**
* Assign a RMobilCall to be the incoming call.
*/
void CEtelSessionMgr::DoAssignCallToIncomingL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, const TName& aIncomingCallName)
	{
	RMobileCall& call = iCalls[aServerId][aPhoneId][aLineId][aCallId];
	TRefCountItem& refCount = iCallsRefs[aLineId][aCallId];
	RMobileLine& line = iLines[aServerId][aPhoneId][aLineId];
	User::LeaveIfError(call.OpenExistingCall(line, aIncomingCallName));
	
	// Check callname
	RCall::TCallInfo callInfo;
	User::LeaveIfError(call.GetInfo(callInfo));
	if(callInfo.iCallName != aIncomingCallName)
		{
		User::Leave(KErrGeneral);
		}
	refCount.AddRef( const_cast<TName&>(aIncomingCallName));
	}


/**
* Close a call subsession
*/
void CEtelSessionMgr::DoCloseCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoCloseCallL aServerId=%d aPhoneId=%d aLineId=%d aCallId=%d"), aServerId, aPhoneId, aLineId, aCallId);
	RMobileCall& call = iCalls[aServerId][aPhoneId][aLineId][aCallId];
	RCall::TStatus callStatus = RCall::EStatusIdle;
	call.GetStatus(callStatus); // ignore error
	if (callStatus != RCall::EStatusIdle)
		{
		call.HangUp();
		}
	call.Close();
	TRefCountItem& refCount = iCallsRefs[aLineId][aCallId];
	refCount.ReleaseL();
	}


/**
* Open a conference call subsession
*/
 void CEtelSessionMgr::DoOpenConferenceCallL(TServerId aServerId, TPhoneId aPhoneId, TConferenceCallId aConferenceCallId)
 	{
 	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenConferenceCallL aServerId=%d aPhoneId=%d aConferenceCallId=%d"), aServerId, aPhoneId, aConferenceCallId);
 	RMobileConferenceCall& confCall = iConferenceCalls[aServerId][aPhoneId][aConferenceCallId];
 	User::LeaveIfError(iConferenceCalls[aServerId][aPhoneId][aConferenceCallId].Open(iPhones[aServerId][aPhoneId]));
 	}


/**
* Close a conference call subsession
*/
 void CEtelSessionMgr::DoCloseConferenceCallL(TServerId aServerId, TPhoneId aPhoneId, TConferenceCallId aConferenceCallId)
 	{
 	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseConferenceCallL aServerId=%d aPhoneId=%d aConferenceCallId=%d"), aServerId, aPhoneId, aConferenceCallId);
 	iConferenceCalls[aServerId][aPhoneId][aConferenceCallId].Close();
 	}


/**
* Open a RFax subsession
*/
void CEtelSessionMgr::DoOpenFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoOpenFaxL aServerId=%d aPhoneId=%d aLineId=%d aCallId=%d aFaxId=%d"), aServerId, aPhoneId, aLineId, aCallId, aFaxId);
	RFax &fax = iFaxes[aServerId][aPhoneId][aLineId][aCallId][aFaxId];
	User::LeaveIfError(fax.Open(iCalls[aServerId][aPhoneId][aLineId][aCallId]));
	}
	
/**
* Close a RFax subsession
*/
void CEtelSessionMgr::DoCloseFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoCloseFaxL aServerId=%d aPhoneId=%d aLineId=%d aCallId=%d aFaxId=%d"), aServerId, aPhoneId, aLineId, aCallId, aFaxId);
	iFaxes[aServerId][aPhoneId][aLineId][aCallId][aFaxId].Close();
	}	


/**
* Open a packet service subsession
*/
void CEtelSessionMgr::DoOpenPacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenPacketServiceL aServerId=%d aPhoneId=%d aPacketServiceId=%d"), aServerId, aPhoneId, aPacketServiceId);
	RPacketService& packetService = iPacketServices[aServerId][aPhoneId][aPacketServiceId];
	User::LeaveIfError(packetService.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a packet service subsession
*/
void CEtelSessionMgr::DoClosePacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoClosePacketServiceL aServerId=%d aPhoneId=%d aPacketServiceId=%d"), aServerId, aPhoneId, aPacketServiceId);
	iPacketServices[aServerId][aPhoneId][aPacketServiceId].Close();
	}
	
	
/**
* Open a primary packet context subsession
*/
void CEtelSessionMgr::DoOpenPrimaryPacketContextL(TServerId aServerId, 
												TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	RPacketService& packetService = iPacketServices[aServerId][aPhoneId][aPacketServiceId];
	RPacketContext& context = iPrimaryPacketContexts[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	TRefCountItem& refCount = iPrimaryPacketContextRefs[aPrimaryPacketContextId];
	
	TName contextName;
	if( refCount.GetCount() )
		{
		contextName = refCount.GetName();
		User::LeaveIfError( context.OpenExistingContext( packetService, contextName) );
		}
	else
		{
		User::LeaveIfError( context.OpenNewContext( packetService, contextName) );
        TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextL context name = %S"), &contextName);

        // Open channel
        TUint channelNumber;
        TLex lex(contextName);
        while (!lex.Eos())
            {
            TInt err = lex.Val(channelNumber);
            TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextL err = %d, channel number = %d"), err, channelNumber);
            lex.Inc();
            };

        TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextL channel number = %d"), channelNumber);
        TRequestStatus status;
        RPacketContext::TDataChannelV2 dataChannel;
        TPckg<RPacketContext::TDataChannelV2> dataChannelPckg(dataChannel);
        context.InitialiseContext(status,dataChannelPckg);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
		    {
            TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextL Failed to Initialise Context error = %d"), status.Int());
            User::Leave(status.Int());
		    }
		}
	refCount.AddRef( contextName );
	}


/**
* Close a primary packet context subsession
*/
void CEtelSessionMgr::DoClosePrimaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoClosePrimaryPacketContextL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	RPacketContext& context = iPrimaryPacketContexts[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	TRefCountItem& refCount = iPrimaryPacketContextRefs[aPrimaryPacketContextId];
	
	context.Close();
	refCount.ReleaseL();

	}
	

/**
* Open the RPacketQos for a primary packet context 
*/
void CEtelSessionMgr::DoOpenPrimaryPacketContextQoSL(TServerId aServerId, 
												TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoOpenPrimaryPacketContextQoSL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	RPacketContext& context = iPrimaryPacketContexts[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	RPacketQoS& qos = iPrimaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	TRefCountItem& refCount = iPrimaryPacketContextQoSRefs[aPrimaryPacketContextId];
	
	TName serviceName;
	if( refCount.GetCount() )
		{
		serviceName = refCount.GetName();
		User::LeaveIfError( qos.OpenExistingQoS( context, serviceName) );
		}
	else
		{
		User::LeaveIfError( qos.OpenNewQoS( context, serviceName) );
		}
	refCount.AddRef( serviceName );
	}


/**
* Close the RPacketQos for a primary packet context 
*/
void CEtelSessionMgr::DoClosePrimaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, TPrimaryPacketContextId aPrimaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoClosePrimaryPacketContextQoSL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId);
	RPacketQoS& qos = iPrimaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId][aPrimaryPacketContextId];
	TRefCountItem& refCount = iPrimaryPacketContextQoSRefs[aPrimaryPacketContextId];
	
	qos.Close();
	refCount.ReleaseL();
	}	

/**
* Open a secondary packet context subsession
*/
void CEtelSessionMgr::DoOpenSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId )
										
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoOpenSecondaryPacketContextL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	RPacketService &packetService = iPacketServices[aServerId][aPhoneId][aPacketServiceId];
	RPacketContext& secContext = iSecondaryPacketContexts[aServerId][aPhoneId][aPacketServiceId]
									[aPrimaryPacketContextId][aSecondaryPacketContextId];
									
	TRefCountItem& secRefCount = iSecondaryPacketContextRefs[aPrimaryPacketContextId][aSecondaryPacketContextId];
	
	TName serviceName;
	if( secRefCount.GetCount() )
	
		{
		serviceName = secRefCount.GetName();
		User::LeaveIfError( secContext.OpenExistingContext( packetService, serviceName) );
		}
	else
		{
		// Get the related primary context name
		TRefCountItem& priRefCount = iPrimaryPacketContextRefs[aPrimaryPacketContextId];
		
		if( !priRefCount.GetCount() )
			{
				User::Leave( KErrGeneral );
			}
		
		User::LeaveIfError( secContext.OpenNewSecondaryContext( packetService, priRefCount.GetName(), serviceName) );

        // Open channel
        TUint channelNumber;
        TLex lex(serviceName);
        while (!lex.Eos())
            {
            TInt err = lex.Val(channelNumber);
            TEST_FRAMEWORK_LOG3(_L("CEtelSessionMgr::DoOpenSecondaryPacketContextL err = %d, channel number = %d"), err, channelNumber);
            lex.Inc();
            };

        TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoOpenSecondaryPacketContextL channel number = %d"), channelNumber);
        TRequestStatus status;

        RPacketContext::TDataChannelV2 dataChannel;
        TPckg<RPacketContext::TDataChannelV2> dataChannelPckg(dataChannel);
        secContext.InitialiseContext(status,dataChannelPckg);
        User::WaitForRequest(status);

        if (status.Int() != KErrNone)
		    {
            TEST_FRAMEWORK_LOG2(_L("CEtelSessionMgr::DoOpenSecondaryPacketContextL Failed to Initialise Context error = %d"), status.Int());
            User::Leave(status.Int());
		    }
        
		}
		
	secRefCount.AddRef( serviceName );
	}


/**
* Close a secondary packet context subsession
*/
void CEtelSessionMgr::DoCloseSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId )
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoCloseSecondaryPacketContextL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	RPacketContext& secContext = iSecondaryPacketContexts[aServerId][aPhoneId][aPacketServiceId]
									[aPrimaryPacketContextId][aSecondaryPacketContextId];
									
	TRefCountItem& secRefCount = iSecondaryPacketContextRefs[aPrimaryPacketContextId][aSecondaryPacketContextId];
	
	secContext.Close();
	secRefCount.ReleaseL();
	}

/**
* Open the RPacketQos for a secondary packet context 
*/
void CEtelSessionMgr::DoOpenSecondaryPacketContextQoSL(TServerId aServerId, 
												TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoOpenSecondaryPacketContextQoSL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	RPacketContext& context = iSecondaryPacketContexts[aServerId][aPhoneId][aPacketServiceId]
										[aPrimaryPacketContextId][aSecondaryPacketContextId];
	RPacketQoS& qos = iSecondaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId]
										[aPrimaryPacketContextId][aSecondaryPacketContextId];
										
	TRefCountItem& refCount = iSecondaryPacketContextQoSRefs[aPrimaryPacketContextId][aSecondaryPacketContextId];
	
	TName serviceName;
	if( refCount.GetCount() )
		{
		serviceName = refCount.GetName();
		User::LeaveIfError( qos.OpenExistingQoS( context, serviceName) );
		}
	else
		{
		User::LeaveIfError( qos.OpenNewQoS( context, serviceName) );
		}
	refCount.AddRef( serviceName );
	}


/**
* Close the RPacketQos for a secondary packet context 
*/
void CEtelSessionMgr::DoCloseSecondaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId)
	{
	TEST_FRAMEWORK_LOG6(_L("CEtelSessionMgr::DoCloseSecondaryPacketContextQoSL aServerId=%d aPhoneId=%d aPacketServiceId=%d aPrimaryPacketContextId=%d aSecondaryPacketContextId=%d"), aServerId, aPhoneId, aPacketServiceId, aPrimaryPacketContextId, aSecondaryPacketContextId);
	RPacketQoS& qos = iSecondaryPacketContextQoSs[aServerId][aPhoneId][aPacketServiceId]
													[aPrimaryPacketContextId][aSecondaryPacketContextId];
													
	TRefCountItem& refCount = iSecondaryPacketContextQoSRefs[aPrimaryPacketContextId][aSecondaryPacketContextId];
	
	qos.Close();
	refCount.ReleaseL();
	}	

/**
* Open a RMobileONStore subsession
*/
void CEtelSessionMgr::DoOpenONStoreL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenONStoreL aServerId=%d aPhoneId=%d aONStoreId=%d"), aServerId, aPhoneId, aONStoreId);
	RMobileONStore& ONStore = iONStores[aServerId][aPhoneId][aONStoreId];
	User::LeaveIfError(ONStore.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a RMobileONStore subsession
*/
void CEtelSessionMgr::DoCloseONStoreL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseONStoreL aServerId=%d aPhoneId=%d aONStoreId=%d"), aServerId, aPhoneId, aONStoreId);
	iONStores[aServerId][aPhoneId][aONStoreId].Close();
	}

/**
* Open a RMobileENStore subsession
*/
void CEtelSessionMgr::DoOpenENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenENStoreL aServerId=%d aPhoneId=%d aENStoreId=%d"), aServerId, aPhoneId, aENStoreId);
	RMobileENStore& ENStore = iENStores[aServerId][aPhoneId][aENStoreId];
	User::LeaveIfError(ENStore.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a RMobileENStore subsession
*/
void CEtelSessionMgr::DoCloseENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseENStoreL aServerId=%d aPhoneId=%d aENStoreId=%d"), aServerId, aPhoneId, aENStoreId);
	iENStores[aServerId][aPhoneId][aENStoreId].Close();
	}

/**
* Open a RMobilePhoneBookStore subsession
*/
void CEtelSessionMgr::DoOpenPhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenPhoneBookStoreL aServerId=%d aPhoneId=%d aPhoneBookStoreId=%d"), aServerId, aPhoneId, aPhoneBookStoreId);
	RMobilePhoneBookStore& PhoneBookStore = iPhoneBookStores[aServerId][aPhoneId][aPhoneBookStoreId];
	User::LeaveIfError(PhoneBookStore.Open(iPhones[aServerId][aPhoneId],iPhoneBookStoreNames[aPhoneBookStoreId]));
	}

/**
* Close a RMobilePhoneBookStore subsession
*/
void CEtelSessionMgr::DoClosePhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoClosePhoneBookStoreL aServerId=%d aPhoneId=%d aPhoneBookStoreId=%d"), aServerId, aPhoneId, aPhoneBookStoreId);
	iPhoneBookStores[aServerId][aPhoneId][aPhoneBookStoreId].Close();
	}

/**
* Open a RMobileSmsStore subsession
*/
void CEtelSessionMgr::DoOpenSmsStoreL(TServerId aServerId, TPhoneId aPhoneId, 
									TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoOpenSmsStoreL aServerId=%d aPhoneId=%d aSmsMessagingId=%d aSmsStoreId=%d"), aServerId, aPhoneId, aSmsMessagingId, aSmsStoreId);
	RMobileSmsStore& SmsStore = iSmsStores[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId];
	User::LeaveIfError(SmsStore.Open(iSmsMessagings[aServerId][aPhoneId][aSmsMessagingId],*iSmsStoreNames[aSmsStoreId]));
	}

/**
* Close a RMobileSmsStore subsession
*/
void CEtelSessionMgr::DoCloseSmsStoreL(TServerId aServerId, TPhoneId aPhoneId, 
									TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId)
	{
	TEST_FRAMEWORK_LOG5(_L("CEtelSessionMgr::DoCloseSmsStoreL aServerId=%d aPhoneId=%d aSmsMessagingId=%d aSmsStoreId=%d"), aServerId, aPhoneId, aSmsMessagingId, aSmsStoreId);
	iSmsStores[aServerId][aPhoneId][aSmsMessagingId][aSmsStoreId].Close();
	}

/**
* Open a RMobileSmsMessaging subsession
*/
void CEtelSessionMgr::DoOpenSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenSmsMessagingL aServerId=%d aPhoneId=%d aSmsMessagingId=%d"), aServerId, aPhoneId, aSmsMessagingId);
	RMobileSmsMessaging& SmsMessaging = iSmsMessagings[aServerId][aPhoneId][aSmsMessagingId];
	User::LeaveIfError(SmsMessaging.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a RMobileSmsMessaging subsession
*/
void CEtelSessionMgr::DoCloseSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseSmsMessagingL aServerId=%d aPhoneId=%d aSmsMessagingId=%d"), aServerId, aPhoneId, aSmsMessagingId);
	iSmsMessagings[aServerId][aPhoneId][aSmsMessagingId].Close();
	}

/**
* Open a RMobileBroadcastMessaging subsession
*/
void CEtelSessionMgr::DoOpenBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId, TBroadcastMessagingId aBroadcastMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenBroadcastMessagingL aServerId=%d aPhoneId=%d aBroadcastMessagingId=%d"), aServerId, aPhoneId, aBroadcastMessagingId);
	RMobileBroadcastMessaging& BroadcastMessaging = iBroadcastMessagings[aServerId][aPhoneId][aBroadcastMessagingId];
	User::LeaveIfError(BroadcastMessaging.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a RMobileBroadcastMessaging subsession
*/
void CEtelSessionMgr::DoCloseBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId, TBroadcastMessagingId aBroadcastMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseBroadcastMessagingL aServerId=%d aPhoneId=%d aBroadcastMessagingId=%d"), aServerId, aPhoneId, aBroadcastMessagingId);
	iBroadcastMessagings[aServerId][aPhoneId][aBroadcastMessagingId].Close();
	}

/**
* Open a RMobileUssdMessaging subsession
*/
void CEtelSessionMgr::DoOpenUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenUssdMessagingL aServerId=%d aPhoneId=%d aUssdMessagingId=%d"), aServerId, aPhoneId, aUssdMessagingId);
	RMobileUssdMessaging& ussdMessaging = iUssdMessagings[aServerId][aPhoneId][aUssdMessagingId];
	User::LeaveIfError(ussdMessaging.Open(iPhones[aServerId][aPhoneId]));
	}

/**
* Close a RMobileUssdMessaging subsession
*/
void CEtelSessionMgr::DoCloseUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId)
	{
	TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseUssdMessagingL aServerId=%d aPhoneId=%d aUssdMessagingId=%d"), aServerId, aPhoneId, aUssdMessagingId);
	iUssdMessagings[aServerId][aPhoneId][aUssdMessagingId].Close();
	}

/**
 * Open a RMmCustomAPI subsession
 */
void CEtelSessionMgr::DoOpenCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
    {
    TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoOpenCustomApiL aServerId=%d aPhoneId=%d aCustomApiId=%d"), aServerId, aPhoneId, aCustomApiId);
	RMmCustomAPI& customApi = iCustomApis[aServerId][aPhoneId][aCustomApiId];
	User::LeaveIfError(customApi.Open(iPhones[aServerId][aPhoneId]));
    }

/**
 * Close a RMmCustomAPI subsession
 */
void CEtelSessionMgr::DoCloseCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
    {
    TEST_FRAMEWORK_LOG4(_L("CEtelSessionMgr::DoCloseCustomApiL aServerId=%d aPhoneId=%d aCustomApiId=%d"), aServerId, aPhoneId, aCustomApiId);
	iCustomApis[aServerId][aPhoneId][aCustomApiId].Close();
    }

	
/**
* Load the line names into our members
*/
void CEtelSessionMgr::DoGetLineNamesL(RPhone& aPhone)
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::DoGetLineNamesL"));
	//get line info for the phone
	TInt cnt;
	User::LeaveIfError(aPhone.EnumerateLines(cnt));

	RPhone::TLineInfo info;
	for(TInt i = 0; i < cnt; i++)
		{
		User::LeaveIfError(aPhone.GetLineInfo(i, info));
		if(info.iLineCapsFlags & RLine::KCapsData)
			{
			iLinesName[KDataLine] = info.iName;
			}
		else if(info.iLineCapsFlags & RLine::KCapsVoice)
			{
			iLinesName[KVoiceLine] = info.iName;
			}
		else if(info.iLineCapsFlags & RMobileLine::KCapsAuxVoice)
			{
			iLinesName[KAuxLine] = info.iName;
			}
		else if(info.iLineCapsFlags & RLine::KCapsFax)
			{
			iLinesName[KFaxLine] = info.iName;
			}
		}
	iHaveLineNames = ETrue;
	}
	

/**
* TRefCountItem - C'tor
*/
CEtelSessionMgr::TRefCountItem::TRefCountItem() : iRefCount(0)
	{
	
	}
	
/**
* Returns count before add operation, i.e. zero if new
*/
TInt CEtelSessionMgr::TRefCountItem::AddRef(TDesC &aName)
	{
	if( !iRefCount )
		{
		iName = aName;
		}
	return iRefCount++;			
	}

/**
* Returns count after release operation, i.e. zero if now unused
* Leaves if the count was already 0
*/
TInt CEtelSessionMgr::TRefCountItem::ReleaseL()
	{
	if( !iRefCount )
		{
		TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::TRefCountItem::ReleaseL Leaving with KErrNotFound!"));
		User::Leave(KErrNotFound);
		}
	return --iRefCount;
	}

/**
* Returns the reference count 
*/
TInt CEtelSessionMgr::TRefCountItem::GetCount() const 
	{
	TEST_FRAMEWORK_LOG1(_L("CEtelSessionMgr::TRefCountItem::GetCount iRefCount"));
	return iRefCount;
	}
	
/**
* Returns a reference to the name associated with the ref count
*/
const TName& CEtelSessionMgr::TRefCountItem::GetName() const
	{
	return iName;
	}

void CEtelSessionMgr::AsyncExec(CThreadExec::MFunctor *aFunction)
/**
 *   Execute a function asynchronously.
 *   @param aFunction the function to execute
 */
	{
	iThreadExec->AsyncExec(aFunction);
	}

void CEtelSessionMgr::SyncExec(CThreadExec::MFunctor *aFunction)
/**
 *   Execute a function synchronously from another thread.
 *   @param aFunction the function to execute
 */
	{
	iThreadExec->Exec(aFunction);
	}

