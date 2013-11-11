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
// Contains the CEtelSessionMgr class declaration.
// 
//

/**
 @file 
 @internalTechnology
*/
 
 
#ifndef CETELSESSIONMGR_H
#define CETELSESSIONMGR_H

#include <e32base.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <etelqos.h>
#include <ctsy/rmmcustomapi.h>


#include "cthreadexec.h"
#include "tetelrequeststatus.h"

enum TServerId
	{
	KMainServer,
	KAltServer,
	KMaxServers
	};	
	
enum TPhoneId
	{
	KMainPhone,
	KMaxPhones
	};	
	
enum TLineId
	{
	KVoiceLine,
	KDataLine,
	KFaxLine,
	KAuxLine,
	KMaxLines
	};

enum TCallId
	{
	KCall1,
	KCall2,
	KCall3,
	KCall4,
	KCall5,
	KCall6,
	KMaxCalls
	};

enum TPacketServiceId
	{
	KMainPacketService,
	KAltPacketService,
	KMaxPacketService
	};
	
enum TPrimaryPacketContextId
	{
	KPrimaryPacketContext1,
	KPrimaryPacketContext2,
	KPrimaryPacketContext3,
	KPrimaryPacketContext4,
	KMaxPrimaryPacketContext
	};

enum TSecondaryPacketContextId
	{
	KSecondaryPacketContext1,
	KSecondaryPacketContext2,
	KSecondaryPacketContext3,
	KSecondaryPacketContext4,
	KMaxSecondaryPacketContext
	};
	
enum TConferenceCallId
	{
	KMainConferenceCall,
	KMaxConferenceCall
	};
enum TONStoreId
	{
	KMainONStore,
	KMaxONStore
	};

enum TENStoreId
	{
	KMainENStore,
	KMaxENStore
	};

enum TPhoneBookStoreId
	{
	KMeAdnPhoneBook,
	KMeDialledPhoneBook,
	KMeMissedPhoneBook,
	KMeReceivedPhoneBook,
	KCombinedAdnPhoneBook,
	KTaAdnPhoneBook,
	KIccAdnPhoneBook,
	KIccFdnPhoneBook,
	KIccSdnPhoneBook,
	KIccBdnPhoneBook,
	KIccLndPhoneBook,
	KIccVoiceMailBox,
	KIccMbdnPhoneBook,
	KMaxPhoneBookStore
	};

enum TSmsStoreId
	{
	KMeSmsStore,
	KIccSmsStore,
	KCombinedSmsStore,
	KMaxSmsStore
	};

enum TSmsMessagingId
	{
	KMainSmsMessaging,
	KMaxSmsMessaging
	};

enum TBroadcastMessagingId
	{
	KMainBroadcastMessaging,
	KMaxBroadcastMessaging
	};

enum TUssdMessagingId
	{
	KMainUssdMessaging,
	KMaxUssdMessaging
	};
	
enum TFaxId
	{
	KMainFax,
	KMaxFax
	};
		
enum TCustomApiId
    {
    KMainCustomApi,
    KMaxCustomApi
    };

/**
 * The session manager is responsible for opening and closing
 * all Etel-related subsessions.
*/

class CEtelSessionMgr : public CBase
	{
public:

public:
	static CEtelSessionMgr* NewL(TName& aTsyName, TName& aPhoneName, TBool aKeepSessOpen);
	virtual ~CEtelSessionMgr();

public:
	void OpenL(TBool aAutoOpenSess); 
	void Close();
	void RestartL();
	
public:
	RTelServer& GetServerL(TServerId aServerId);	
	RMobilePhone& GetPhoneL(TServerId aServerId, TPhoneId aPhoneId);
	RMobileLine& GetLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId);
	RMobileCall& GetCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId);
	RFax& GetFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId);

	RMobileCall& GetIncomingCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, const TName& aName, TCallId& aCallId);
	
	RMobileConferenceCall& GetConferenceCallL( TServerId aServerId, 
												TPhoneId aPhoneId,TConferenceCallId aConferenceCallId );
	RPacketService& GetPacketServiceL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId);
	RPacketContext& GetPrimaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId,
												TPrimaryPacketContextId aPrimaryPacketContextId);
												
	RPacketQoS& GetPrimaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId,
												TPrimaryPacketContextId aPrimaryPacketContextId);
												
												
	RPacketContext& GetSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId,
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId);

	RPacketQoS& GetSecondaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId,TPacketServiceId aPacketServiceId,
												TPrimaryPacketContextId aPrimaryPacketContextId,
												TSecondaryPacketContextId aSecondaryPacketContextId);
												
	RMobileONStore& GetONStoreL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId);
	RMobileENStore& GetENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId);
	RMobilePhoneBookStore& GetPhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId);

	RMobileSmsMessaging& GetSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId);
	RMobileSmsStore& GetSmsStoreL(TServerId aServerId, TPhoneId aPhoneId, 
											TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId);
	RMobileBroadcastMessaging& GetBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId, TBroadcastMessagingId aBroadcastMessagingId);
	RMobileUssdMessaging& GetUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId);
    RMmCustomAPI& GetCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);
	

	void ReleaseServer(TServerId aServerId);
	void ReleasePhone(TServerId aServerId, TPhoneId aPhoneId);
	void ReleaseLine(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId);
	void ReleaseCall(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId);
	void ReleaseFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId);
	void ReleaseConferenceCall( TServerId aServerId, 
												TPhoneId aPhoneId,TConferenceCallId aConferenceCallId );
	void ReleasePacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId);
	void ReleasePrimaryPacketContext(TServerId aServerId, TPhoneId aPhoneId, 
									TPacketServiceId aPacketServiceId, TPrimaryPacketContextId aPrimaryPacketContextId);
	void ReleasePrimaryPacketContextQoS(TServerId aServerId, TPhoneId aPhoneId, 
									TPacketServiceId aPacketServiceId, TPrimaryPacketContextId aPrimaryPacketContextId);
									
	void ReleaseSecondaryPacketContext(TServerId aServerId, TPhoneId aPhoneId, 
									TPacketServiceId aPacketServiceId, 
									TPrimaryPacketContextId aPrimaryPacketContextId,
									TSecondaryPacketContextId aSecondaryPacketContextId);
	void ReleaseSecondaryPacketContextQoS(TServerId aServerId, TPhoneId aPhoneId, 
									TPacketServiceId aPacketServiceId, 
									TPrimaryPacketContextId aPrimaryPacketContextId,
									TSecondaryPacketContextId aSecondaryPacketContextId);
									

	void ReleaseONStore(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId);
	void ReleaseENStore(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId);
	void ReleasePhoneBookStore(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId);
	void ReleaseSmsStore(TServerId aServerId, TPhoneId aPhoneId, 
										TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId);

	void ReleaseSmsMessaging(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId);
	void ReleaseBroadcastMessaging(TServerId aServerId, TPhoneId aPhoneId, TBroadcastMessagingId aBroadcastMessagingId);
	void ReleaseUssdMessaging(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId);
    void ReleaseCustomApi(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);
	
	inline TBool HasLine(TLineId aLineId);
	
	inline TDesC& PhoneName();
	
	void AsyncExec(CThreadExec::MFunctor* aFunction);
	void SyncExec(CThreadExec::MFunctor* aFunction);

private:
	enum TSessionStatus
		{
		ESessClosed =0,				//< Session is closed
		ESessOpenAutomatically =1,	//< Session was open automatically 
		ESessOpenManually =2		//< Session was open by an explicit call
		};

private:
	CThreadExec* iThreadExec;
	TBool iIsOpen;
	TBool iAutoSessionOpened;
	
	/**
	*	Class to hold a simple reference count for a name, e.g. for use with call names
	*/
	class TRefCountItem
		{
	public:
		TRefCountItem();
		/**
		* Returns count before add operation, i.e. zero if new
		*/
		TInt AddRef( TDesC &aName );
		/**
		* Returns count after release operation, i.e. zero if now unused
		*/
		TInt ReleaseL();
		
		const TName &GetName() const;
		TInt GetCount() const;
	private:
		TName	iName;
		TInt	iRefCount;
		};
	
		
	// Servers
	typedef TFixedArray<RTelServer,KMaxServers> TTelServerArray;
	typedef TFixedArray<TSessionStatus,KMaxServers> TTelServerStatusArray;

	// Phones
	typedef TFixedArray<TFixedArray<RMobilePhone,KMaxPhones>,KMaxServers> 	TPhoneArray;
	typedef TFixedArray<TFixedArray<TSessionStatus,KMaxPhones>,KMaxServers> TPhoneStatusArray;

	// Lines
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileLine,KMaxLines>,KMaxPhones>,KMaxServers> 	TLineArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxLines>,KMaxPhones>,KMaxServers> 	TLineStatusArray;
	
	// Calls
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<RMobileCall,KMaxCalls>,KMaxLines>,KMaxPhones>,KMaxServers> 	TCallArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxCalls>,KMaxLines>,KMaxPhones>,KMaxServers> 	TCallStatusArray;
	typedef TFixedArray<TFixedArray<TRefCountItem,KMaxCalls>,KMaxLines> TCallRefArray;

	// Conference call
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileConferenceCall,KMaxConferenceCall>,
								KMaxPhones>,KMaxServers> 	TConferenceCallArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxConferenceCall>,
								KMaxPhones>,KMaxServers> 	TConferenceCallStatusArray;
	
	// Fax
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<RFax,KMaxFax>,
								KMaxCalls>, KMaxLines>,KMaxPhones>,KMaxServers> 	TFaxArray;
								
								
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxFax>,
								KMaxCalls>, KMaxLines>,KMaxPhones>,KMaxServers> 	TFaxStatusArray;	

	// PacketService
	typedef TFixedArray<TFixedArray<TFixedArray<RPacketService,KMaxPacketService>,KMaxPhones>,KMaxServers> 	TPacketServiceArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxPacketService>,KMaxPhones>,KMaxServers> 	TPacketServiceStatusArray;

	// PrimaryPacketContext
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<RPacketContext, KMaxPrimaryPacketContext>,
					KMaxPacketService>,KMaxPhones>,KMaxServers> 	TPrimaryPacketContextArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus, KMaxPrimaryPacketContext>,
						KMaxPacketService>,KMaxPhones>,KMaxServers> TPrimaryPacketContextStatusArray;						
	typedef TFixedArray<TRefCountItem,KMaxPrimaryPacketContext> TPrimaryPacketContextRefs;
	
	// PrimaryPacketContextQoS
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<RPacketQoS, KMaxPrimaryPacketContext>,
					KMaxPacketService>,KMaxPhones>,KMaxServers> 	TPrimaryPacketContextQoSArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus, KMaxPrimaryPacketContext>,
						KMaxPacketService>,KMaxPhones>,KMaxServers> TPrimaryPacketContextQoSStatusArray;
	typedef TFixedArray<TRefCountItem,KMaxPrimaryPacketContext> TPrimaryPacketContextQoSRefs;
						
						
	// SecondaryPacketContext
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<RPacketContext, KMaxSecondaryPacketContext>,
							KMaxPrimaryPacketContext>,KMaxPacketService>,KMaxPhones>,KMaxServers> 	
			TSecondaryPacketContextArray;
														
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxSecondaryPacketContext>,
							KMaxPrimaryPacketContext>,KMaxPacketService>,KMaxPhones>,KMaxServers> 
			TSecondaryPacketContextStatusArray;						
			
	typedef TFixedArray<TFixedArray<TRefCountItem,KMaxSecondaryPacketContext>,
										KMaxPrimaryPacketContext> TSecondaryPacketContextRefs;
										
	// SecondaryPacketContextQoS
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<RPacketQoS, KMaxSecondaryPacketContext>,
							KMaxPrimaryPacketContext>,KMaxPacketService>,KMaxPhones>,KMaxServers> 	
			TSecondaryPacketContextQoSArray;
														
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxSecondaryPacketContext>,
							KMaxPrimaryPacketContext>,KMaxPacketService>,KMaxPhones>,KMaxServers> 
			TSecondaryPacketContextQoSStatusArray;						

	typedef TFixedArray<TFixedArray<TRefCountItem,KMaxSecondaryPacketContext>,
								KMaxPrimaryPacketContext> TSecondaryPacketContextQoSRefs;

	// ON store
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileONStore,KMaxONStore>,KMaxPhones>,KMaxServers> 	TONStoreArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxONStore>,KMaxPhones>,KMaxServers> 	TONStoreStatusArray;

	// EN Store
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileENStore,KMaxENStore>,KMaxPhones>,KMaxServers> 	TENStoreArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxENStore>,KMaxPhones>,KMaxServers> 	TENStoreStatusArray;
	

	// Phone book
	typedef TFixedArray<TFixedArray<TFixedArray<RMobilePhoneBookStore,KMaxPhoneBookStore>,KMaxPhones>,KMaxServers> 	TPhoneBookStoreArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxPhoneBookStore>,KMaxPhones>,KMaxServers> 	TPhoneBookStoreStatusArray;
	
	// SMS messaging
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileSmsMessaging,KMaxSmsMessaging>,KMaxPhones>,KMaxServers> 	TSmsMessagingArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxSmsMessaging>,KMaxPhones>,KMaxServers> 	TSmsMessagingStatusArray;
	
	// SMS store
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray
										<RMobileSmsStore,KMaxSmsStore>,KMaxSmsMessaging>, KMaxPhones>,KMaxServers> 	TSmsStoreArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TFixedArray
										<TSessionStatus,KMaxSmsStore>,KMaxSmsMessaging>, KMaxPhones>,KMaxServers> 	TSmsStoreStatusArray;
	
	// Broadcast messaging
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileBroadcastMessaging,KMaxBroadcastMessaging>,KMaxPhones>,KMaxServers> 	TBroadcastMessagingArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxBroadcastMessaging>,KMaxPhones>,KMaxServers> 	TBroadcastMessagingStatusArray;
	
	// Ussd messaging
	typedef TFixedArray<TFixedArray<TFixedArray<RMobileUssdMessaging,KMaxUssdMessaging>,
								KMaxPhones>,KMaxServers> 	TUssdMessagingArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxUssdMessaging>,KMaxPhones>,KMaxServers> 	TUssdMessagingStatusArray;

    // Custom API
    typedef TFixedArray<TFixedArray<TFixedArray<RMmCustomAPI,KMaxCustomApi>,
								KMaxPhones>,KMaxServers> 	TCustomApiArray;
	typedef TFixedArray<TFixedArray<TFixedArray<TSessionStatus,KMaxCustomApi>,KMaxPhones>,KMaxServers> 	TCustomApiStatusArray;


	TTelServerArray 			iTelServers;
	TTelServerStatusArray 		iTelServersStatus;
	
	TPhoneArray					iPhones;
	TPhoneStatusArray 			iPhonesStatus;
	
	TLineArray    				iLines;
	TLineStatusArray 			iLinesStatus;

	TCallArray    				iCalls;
	TCallStatusArray 			iCallsStatus;
	TCallRefArray				iCallsRefs;

	TConferenceCallArray		iConferenceCalls;
	TConferenceCallStatusArray	iConferenceCallsStatus;
	
	TFaxArray					iFaxes;
	TFaxStatusArray				iFaxesStatus;
	
	TPacketServiceArray			iPacketServices;
	TPacketServiceStatusArray	iPacketServicesStatus;
	
	TPrimaryPacketContextArray			iPrimaryPacketContexts;
	TPrimaryPacketContextStatusArray	iPrimaryPacketContextsStatus;
	TPrimaryPacketContextRefs			iPrimaryPacketContextRefs;
	
	TPrimaryPacketContextQoSArray		iPrimaryPacketContextQoSs;
	TPrimaryPacketContextQoSStatusArray	iPrimaryPacketContextQoSsStatus;
	TPrimaryPacketContextQoSRefs		iPrimaryPacketContextQoSRefs;
	
	TSecondaryPacketContextArray		iSecondaryPacketContexts;
	TSecondaryPacketContextStatusArray	iSecondaryPacketContextsStatus;
	TSecondaryPacketContextRefs			iSecondaryPacketContextRefs;
	
	TSecondaryPacketContextQoSArray			iSecondaryPacketContextQoSs;
	TSecondaryPacketContextQoSStatusArray	iSecondaryPacketContextQoSsStatus;
	TSecondaryPacketContextQoSRefs			iSecondaryPacketContextQoSRefs;
		
	TONStoreArray				iONStores;
	TONStoreStatusArray			iONStoresStatus;
	
	TENStoreArray				iENStores;
	TENStoreStatusArray			iENStoresStatus;
	
	TPhoneBookStoreArray		iPhoneBookStores;
	TPhoneBookStoreStatusArray	iPhoneBookStoresStatus;
	

	TFixedArray< TBuf<64>,KMaxPhoneBookStore> iPhoneBookStoreNames;
	
	TSmsStoreArray			iSmsStores;
	TSmsStoreStatusArray	iSmsStoresStatus;
	TFixedArray<const TDesC*, KMaxSmsStore> iSmsStoreNames;
	
	TSmsMessagingArray			iSmsMessagings;
	TSmsMessagingStatusArray	iSmsMessagingsStatus;
	
	TBroadcastMessagingArray		iBroadcastMessagings;
	TBroadcastMessagingStatusArray	iBroadcastMessagingsStatus;
	
	TUssdMessagingArray			iUssdMessagings;
	TUssdMessagingStatusArray	iUssdMessagingsStatus;

    TCustomApiArray         iCustomApis;
    TCustomApiStatusArray   iCustomApisStatus;
		
private: // config data
	TName& iTsyName;
	TName& iPhoneName;
	TBool iKeepSessOpen;
	TBool iHaveLineNames;
	TFixedArray<TName,KMaxLines> iLinesName;
	
	// Threshold below which sessions are closed by CloseXXXSessL and related methods
	TSessionStatus iStatusCloseThresh;

private:
	CEtelSessionMgr(TName& aTsyName, TName& aPhoneName, TBool aKeepSessOpen);
	void ConstructL();
		
private: 
	void DoOpenAutoSessL();
	void DoCloseAllSessL(TBool aIncludeAutoOpenedSess);
	void ClosePhoneSessL(TServerId aServerId, TPhoneId aPhoneId );
	void CloseLineSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId );
	void CloseCallSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId );
	void CloseFaxSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId );
	
	void CloseConfCallSessL(TServerId aServerId, TPhoneId aPhoneId,TConferenceCallId aConfCallId );
	void ClosePacketServiceSessL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId );

	void ClosePacketServiceContextSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aContextId );


	void ClosePhoneBookStoreSessL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookId );
	void CloseENStoreSessL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId );
	void CloseONStoreSessL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId );
	void CloseSmsMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId );
	void CloseSmsStoreSessL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMsgId, TSmsStoreId aSmsStoreId );
	void CloseBroadcastMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, 
														TBroadcastMessagingId aBcastMessagingId );
    void CloseUssdMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId );
    void CloseCustomApiSessL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);

	void DoCleanup();

	void DoOpenServerL(TServerId aServerId);
	void DoCloseServerL(TServerId aServerId);

	void DoOpenPhoneL(TServerId aServerId, TPhoneId aPhoneId);
	void DoClosePhoneL(TServerId aServerId, TPhoneId aPhoneId);

	void DoOpenLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId);
	void DoCloseLineL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId);

	void DoOpenCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId);
	void DoCloseCallL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId);

	void DoOpenFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId);
	void DoCloseFaxL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId);
	
	void DoOpenConferenceCallL(TServerId aServerId, TPhoneId aPhoneId, TConferenceCallId aConferenceCallId);
	void DoCloseConferenceCallL(TServerId aServerId, TPhoneId aPhoneId, TConferenceCallId aConferenceCallId);
	
	void DoOpenPacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId);
	void DoClosePacketServiceL(TServerId aServerId, TPhoneId aPhoneId, TPacketServiceId aPacketServiceId);
	
	void DoOpenPrimaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId );
	void DoClosePrimaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPacketContextId);

	void DoOpenPrimaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId );
	void DoClosePrimaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPacketContextId);										
											
	void DoOpenSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId );
	void DoCloseSecondaryPacketContextL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId );
										

	void DoOpenSecondaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId );
	void DoCloseSecondaryPacketContextQoSL(TServerId aServerId, TPhoneId aPhoneId, 
										TPacketServiceId aPacketServiceId, 
										TPrimaryPacketContextId aPrimaryPacketContextId,
										TSecondaryPacketContextId aSecondaryPacketContextId );

	void DoOpenONStoreL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId);
	void DoCloseONStoreL(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId);
	
	void DoOpenENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId);
	void DoCloseENStoreL(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId);
	
	void DoOpenPhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId);
	void DoClosePhoneBookStoreL(TServerId aServerId, TPhoneId aPhoneId, TPhoneBookStoreId aPhoneBookStoreId);

	void DoOpenSmsStoreL(TServerId aServerId, TPhoneId aPhoneId, 
													TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId);
	void DoCloseSmsStoreL(TServerId aServerId, TPhoneId aPhoneId, 
													TSmsMessagingId aSmsMessagingId, TSmsStoreId aSmsStoreId);

	void DoOpenSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId);
	void DoCloseSmsMessagingL(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMessagingId);
	
	void DoOpenBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId, 
										TBroadcastMessagingId aBroadcastMessagingId);
	void DoCloseBroadcastMessagingL(TServerId aServerId, TPhoneId aPhoneId, 
										TBroadcastMessagingId aBroadcastMessagingId);
	
	void DoOpenUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId);
	void DoCloseUssdMessagingL(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMessagingId);

    void DoOpenCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);
    void DoCloseCustomApiL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);

	void DoAssignCallToIncomingL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, const TName& aIncomingCallName);
		
private: // helpers
	void DoGetLineNamesL(RPhone& aPhone);

	// Generated cleanup methods

	static void CleanupMobileBroadcastMessagingNotifyBroadcastIdListChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging );

	static void CleanupMobileBroadcastMessagingNotifyFilterSettingChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging );

	static void CleanupMobileBroadcastMessagingNotifyLanguageFilterChange( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging );

	static void CleanupMobileLineNotifyMobileLineStatusChange( 
					RMobileLine& aMobileLine );

	static void CleanupMobilePhoneNotifyALSLineChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyAlternatingCallCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyAlternatingCallModeChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCCBSRecall( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCCBSRecall1( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCCBSStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCallBarringStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCallForwardingActive( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCallForwardingStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCallServiceCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCallWaitingStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCipheringIndicatorStatus( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCostCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyDTMFCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyDTMFEvent( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyFdnStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIccAccessCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIdentityServiceStatus( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIdentitySuppressionRejected( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIncomingCallTypeChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIndicatorChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMessageWaiting( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyModeChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMulticallIndicatorChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMulticallParamsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMultimediaCallPreferenceChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNetworkInvScanChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNetworkInvScanEvent( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNetworkRegistrationStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNetworkSecurityLevelChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySecurityCapsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySecurityEvent( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySendNetworkServiceRequest( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySignalStrengthChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyPreferredNetworksListChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyStopInDTMFString( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyAirTimeDurationChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCostInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange1( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange2( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange3( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange4( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyCurrentNetworkChange5( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyLockInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNITZInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyNetworkSelectionSettingChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySecurityCodeInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyUSimApplicationsInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyUSimAppsSelectionModeChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyUUSSettingChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyIccMessageWaitingIndicatorsChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMmsConfig( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMmsUpdate( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyBatteryInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyImsAuthorizationInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifySmartCardApplicationInfoChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyDefaultPrivacyChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyAPNListChanged( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyAPNControlListServiceStatusChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupMobilePhoneNotifyMailboxNumbersChange( 
					RMobilePhone& aMobilePhone );

	static void CleanupPacketQoSNotifyProfileChanged( 
					RPacketQoS& aPacketQoS );

	static void CleanupPacketQoSNotifyProfileChanged1( 
					RPacketQoS& aPacketQoS );

	static void CleanupPacketQoSNotifyProfileChanged2( 
					RPacketQoS& aPacketQoS );

	static void CleanupMobileSmartCardEapNotifyEapMethodAccessStatusChange( 
					RMobileSmartCardEap& aMobileSmartCardEap );

	static void CleanupMobileSmsMessagingNotifyMoSmsBearerChange( 
					RMobileSmsMessaging& aMobileSmsMessaging );

	static void CleanupMobileSmsMessagingNotifyReceiveModeChange( 
					RMobileSmsMessaging& aMobileSmsMessaging );

	static void CleanupMobileSmsMessagingNotifySmspListChange( 
					RMobileSmsMessaging& aMobileSmsMessaging );

	static void CleanupMobileCallNotifyAlternatingCallSwitch( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyAudioToneEvent( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyCallEvent( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyHscsdInfoChange( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyMobileCallCapsChange( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyMobileCallStatusChange( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyMobileDataCallCapsChange( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyUUSCapsChange( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyVoiceFallback( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyPrivacyConfirmation( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyTrafficChannelConfirmation( 
					RMobileCall& aMobileCall );

	static void CleanupMobileCallNotifyRemotePartyInfoChange( 
					RMobileCall& aMobileCall );

	static void CleanupLineNotifyCapsChange( 
					RLine& aLine );

	static void CleanupLineNotifyCallAdded( 
					RLine& aLine );

	static void CleanupLineNotifyHookChange( 
					RLine& aLine );

	static void CleanupLineNotifyStatusChange( 
					RLine& aLine );

	static void CleanupLineNotifyIncomingCall( 
					RLine& aLine );

	static void CleanupMobilePhoneStoreNotifyStoreEvent( 
					RMobilePhoneStore& aMobilePhoneStore );

	static void CleanupMobileConferenceCallNotifyCapsChange( 
					RMobileConferenceCall& aMobileConferenceCall );

	static void CleanupMobileConferenceCallNotifyConferenceEvent( 
					RMobileConferenceCall& aMobileConferenceCall );

	static void CleanupMobileConferenceCallNotifyConferenceStatusChange( 
					RMobileConferenceCall& aMobileConferenceCall );

	static void CleanupPacketContextNotifyConnectionSpeedChange( 
					RPacketContext& aPacketContext );

	static void CleanupPacketContextNotifyStatusChange( 
					RPacketContext& aPacketContext );

	static void CleanupPacketContextNotifyConfigChanged( 
					RPacketContext& aPacketContext );

	static void CleanupPacketContextNotifyConfigChanged1( 
					RPacketContext& aPacketContext );

	static void CleanupPacketContextNotifyConfigChanged2( 
					RPacketContext& aPacketContext );

	static void CleanupPacketContextNotifyDataTransferred( 
					RPacketContext& aPacketContext );

	static void CleanupMobileUssdMessagingNotifyNetworkRelease( 
					RMobileUssdMessaging& aMobileUssdMessaging );

	static void CleanupPacketServiceNotifyAttachModeChange( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyChangeOfNtwkRegStatus( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyContextActivationRequested( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyContextActivationRequested1( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyContextActivationRequested2( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyContextAdded( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyDynamicCapsChange( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyMSClassChange( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyReleaseModeChange( 
					RPacketService& aPacketService );

	static void CleanupPacketServiceNotifyStatusChange( 
					RPacketService& aPacketService );

	static void CleanupCallNotifyCapsChange( 
					RCall& aCall );

	static void CleanupCallNotifyCallDurationChange( 
					RCall& aCall );

	static void CleanupCallNotifyHookChange( 
					RCall& aCall );

	static void CleanupCallNotifyStatusChange( 
					RCall& aCall );

	static void CleanupPhoneNotifyCapsChange( 
					RPhone& aPhone );

	static void CleanupPhoneNotifyModemDetected( 
					RPhone& aPhone );

	static void CleanupMobileLocationServicesNotifyMtlr( 
					RMobileLocationServices& aMobileLocationServices );

	static void CleanupMobileLocationServicesNotifyMeasurementControl( 
					RMobileLocationServices& aMobileLocationServices );

	static void CleanupMmCustomAPINotifyAlsBlockedChanged( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyCellInfoChange( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyCipheringInfoChange( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyDtmfEvent( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyEGprsInfoChange( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyHSxPAStatus( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyIccCallForwardingStatusChange( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyNetworkConnectionFailure( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyNSPSStatus( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyPndCacheReady( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyProgrammableOperatorLogoChange( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifyRauEvent( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifySimCardStatus( 
					RMmCustomAPI& aMmCustomAPI );

	static void CleanupMmCustomAPINotifySsNetworkEvent( 
					RMmCustomAPI& aMmCustomAPI );

	//
	//Session Cleanups 
	//

	static void CleanupMobileBroadcastMessagingNotifiers( 
					RMobileBroadcastMessaging& aMobileBroadcastMessaging );

	static void CleanupMobileLineNotifiers( 
					RMobileLine& aMobileLine );

	static void CleanupMobilePhoneNotifiers( 
					RMobilePhone& aMobilePhone );

	static void CleanupPacketQoSNotifiers( 
					RPacketQoS& aPacketQoS );

	static void CleanupMobileSmartCardEapNotifiers( 
					RMobileSmartCardEap& aMobileSmartCardEap );

	static void CleanupMobileSmsMessagingNotifiers( 
					RMobileSmsMessaging& aMobileSmsMessaging );

	static void CleanupMobileCallNotifiers( 
					RMobileCall& aMobileCall );

	static void CleanupLineNotifiers( 
					RLine& aLine );

	static void CleanupMobilePhoneStoreNotifiers( 
					RMobilePhoneStore& aMobilePhoneStore );

	static void CleanupMobileConferenceCallNotifiers( 
					RMobileConferenceCall& aMobileConferenceCall );

	static void CleanupPacketContextNotifiers( 
					RPacketContext& aPacketContext );

	static void CleanupMobileUssdMessagingNotifiers( 
					RMobileUssdMessaging& aMobileUssdMessaging );

	static void CleanupPacketServiceNotifiers( 
					RPacketService& aPacketService );

	static void CleanupCallNotifiers( 
					RCall& aCall );

	static void CleanupPhoneNotifiers( 
					RPhone& aPhone );

	static void CleanupMobileLocationServicesNotifiers( 
					RMobileLocationServices& aMobileLocationServices );

	static void CleanupCustomAPINotifiers( 
					RMmCustomAPI& aMmCustomAPI );

	// End of generated cleanup methods
	
	void TidyPhoneNotifiers(TServerId aServerId, TPhoneId aPhoneId);
	
	void TidyLineNotifiers(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId );
	
	void TidyCallNotifiers(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId );
	
	void TidyFaxNotifiers(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, 
											TCallId aCallId, TFaxId aFaxId );
											
	void TidyConfCallNotifiers(TServerId aServerId, TPhoneId aPhoneId, TConferenceCallId aConfCallId );
	
	void TidyPacketServiceNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId );
												
	void TidyPacketServiceContextNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
												TPacketServiceId aPacketServiceId, 
												TPrimaryPacketContextId aContextId);
												
	void TidyPhoneBookStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
													TPhoneBookStoreId aPhoneBookId );
													
	void TidyENStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aEnStoreId );
	
	void TidyONStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aOnStoreId );
	
	void TidySmsMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMsgId );
	
	void TidySmsStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, TSmsMessagingId aSmsMsgId, 
													TSmsStoreId aSmsStoreId );
	
	void TidyBroadcastMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
														TBroadcastMessagingId aBcastMsgId );
	
	void TidyUssdMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, TUssdMessagingId aUssdMsgId );

    void TidyCustomApiNotifiers(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId);

	void CleanupPhoneService();
	void CleanupPacketService();
	
	
	}; // class CEtelSessionMgr

inline TBool CEtelSessionMgr::HasLine(TLineId aLineId)
	{
	return iLinesName[aLineId].Length() > 0;
	}

inline TDesC& CEtelSessionMgr::PhoneName()
	{
	return iPhoneName;
	}

#endif // CETELSESSIONMGR_H
