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
// cetelsesscleanup.cpp
// 
//

#include "cetelsessionmgr.h"

/**
 * This file contains methods for closing the session objects at cleanup time
 * and also for tidying the notification queues
 * @file 
 */


//
// The following CloseXXXMethods are called in cascade from DoCloseAllSessL
//

/**
*	Close phone subsession and all associated subsessions
*/	
void CEtelSessionMgr::ClosePhoneSessL(TServerId aServerId, TPhoneId aPhoneId )
	{		
	
	// Tidy associated sessions
	
	// Lines
	for (TLineId lineId = KVoiceLine; lineId < KMaxLines; lineId = TLineId(lineId+1))
		{
		CloseLineSessL( aServerId, aPhoneId, lineId );
		}
		
	// Conference calls
	for (TConferenceCallId ccId = KMainConferenceCall; ccId < KMaxConferenceCall; ccId = TConferenceCallId(ccId+1))
		{
		CloseConfCallSessL( aServerId, aPhoneId, ccId );
		}
	
	// Packet services
	for (TPacketServiceId packetServiceId = KMainPacketService; 
			packetServiceId < KMaxPacketService; 
			packetServiceId = TPacketServiceId(packetServiceId+1))
		{
		ClosePacketServiceSessL( aServerId, aPhoneId, packetServiceId );
		}
		
	// PhoneBookStore
	for (TPhoneBookStoreId phoneBookId = KMeAdnPhoneBook; 
			phoneBookId < KMaxPhoneBookStore; 
			phoneBookId = TPhoneBookStoreId(phoneBookId+1))
		{
		ClosePhoneBookStoreSessL( aServerId, aPhoneId, phoneBookId );
		}
		
	// ENStore
	for (TENStoreId enStoreId = KMainENStore; 
			enStoreId < KMaxENStore; 
			enStoreId = TENStoreId(enStoreId+1))
		{
		CloseENStoreSessL( aServerId, aPhoneId, enStoreId );
		}
		
	// ONStore
	for (TONStoreId onStoreId = KMainONStore; 
			onStoreId < KMaxONStore; 
			onStoreId = TONStoreId(onStoreId+1))
		{
		CloseONStoreSessL( aServerId, aPhoneId, onStoreId );
		}
				
	// SmsMessaging
	for (TSmsMessagingId smsMsgId = KMainSmsMessaging; 
			smsMsgId < KMaxSmsMessaging; 
			smsMsgId = TSmsMessagingId(smsMsgId+1))
		{
		CloseSmsMessagingSessL( aServerId, aPhoneId, smsMsgId );
		}
		
	// BroadcastMessaging
	for (TBroadcastMessagingId bcastMsgId = KMainBroadcastMessaging; 
			bcastMsgId < KMaxBroadcastMessaging; 
			bcastMsgId = TBroadcastMessagingId(bcastMsgId+1))
		{
		CloseBroadcastMessagingSessL( aServerId, aPhoneId, bcastMsgId );
		}
		
	// UssdMessaging
	for (TUssdMessagingId ussdMsgId = KMainUssdMessaging; 
			ussdMsgId < KMaxUssdMessaging; 
			ussdMsgId = TUssdMessagingId(ussdMsgId+1))
		{
		CloseUssdMessagingSessL( aServerId, aPhoneId, ussdMsgId );
		}

    // CustomAPI
    for (TCustomApiId customApiId = KMainCustomApi;
          customApiId < KMaxCustomApi;
          customApiId = TCustomApiId(customApiId+1)) 
        {
        CloseCustomApiSessL( aServerId, aPhoneId, customApiId);
        }

	// Finally, close phone
	TSessionStatus &stat= iPhonesStatus[aServerId][aPhoneId];
	if ( stat >= iStatusCloseThresh)
		{
		DoClosePhoneL(aServerId, aPhoneId);
		stat = ESessClosed;
		}
	}

	
/**
*	Close lines
*/
void CEtelSessionMgr::CloseLineSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId )
	{
	
	// Tidy associated call sessions
	for (TCallId callId = KCall1; callId < KMaxCalls; callId = TCallId(callId+1))
		{
		CloseCallSessL( aServerId, aPhoneId, aLineId, callId );
		}

	// Tidy this object				
	if (iLinesStatus[aServerId][aPhoneId][aLineId] >= iStatusCloseThresh)
		{
		DoCloseLineL(aServerId, aPhoneId,aLineId);
		iLinesStatus[aServerId][aPhoneId][aLineId] = ESessClosed;			
		}
	}
	

/**
*	Close calls
*/
void CEtelSessionMgr::CloseCallSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId )
	{
	
	// Tidy associated fax sessions
	for (TFaxId faxId = KMainFax; faxId < KMaxFax; faxId = TFaxId(faxId+1))
		{
		CloseFaxSessL( aServerId, aPhoneId, aLineId,aCallId,faxId);
		}

	
	// Tidy this object
	if (iCallsStatus[aServerId][aPhoneId][aLineId][aCallId]  >= iStatusCloseThresh)
		{
		DoCloseCallL(aServerId, aPhoneId,aLineId,aCallId);
		iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] = ESessClosed;			
		}
	}

/**
*	Close fax
*/
void CEtelSessionMgr::CloseFaxSessL(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, TCallId aCallId, TFaxId aFaxId )
	{
	if (iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId]  >= iStatusCloseThresh)
		{
		DoCloseFaxL(aServerId, aPhoneId,aLineId,aCallId,aFaxId);
		iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId] = ESessClosed;			
		}	
	}


/**
*	Close conf calls
*/
void CEtelSessionMgr::CloseConfCallSessL(TServerId aServerId, TPhoneId aPhoneId,
															TConferenceCallId aConfCallId )
	{
	
	TSessionStatus &stat= iConferenceCallsStatus[aServerId][aPhoneId][aConfCallId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseConferenceCallL(aServerId, aPhoneId, aConfCallId);
		stat = ESessClosed;
		}
	}
	
/**
*	Close packet service
*/
void CEtelSessionMgr::ClosePacketServiceSessL(TServerId aServerId, TPhoneId aPhoneId, 
														TPacketServiceId aPacketServiceId )
	{
		// Tidy associated primary contexts
	for (TPrimaryPacketContextId priCtxId = KPrimaryPacketContext1; 
						priCtxId < KMaxPrimaryPacketContext; priCtxId=TPrimaryPacketContextId(priCtxId+1) )
		{
		ClosePacketServiceContextSessL( aServerId, aPhoneId, aPacketServiceId,priCtxId);
		}

	// Tidy this object		
	TSessionStatus &stat= iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId];
	if ( stat >= iStatusCloseThresh)
		{
		DoClosePacketServiceL(aServerId, aPhoneId, aPacketServiceId);
		stat = ESessClosed;
		}
	}

/**
*	Close packet context and associated secondaries / QoS
*/
void CEtelSessionMgr::ClosePacketServiceContextSessL(TServerId aServerId, TPhoneId aPhoneId, 
											TPacketServiceId aPacketServiceId, 
											TPrimaryPacketContextId aContextId)
	{
	
	// Tidy secondaries and associated QoS
	for (TSecondaryPacketContextId secCtxId = KSecondaryPacketContext1; 
						secCtxId < KMaxSecondaryPacketContext; secCtxId=TSecondaryPacketContextId(secCtxId+1) )
		{

		// If the secondary has a QoS, close it first
		TSessionStatus &secQoSStat = iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId];
			
		if( secQoSStat >= iStatusCloseThresh )
			{
			DoCloseSecondaryPacketContextQoSL( aServerId, aPhoneId, aPacketServiceId, aContextId, secCtxId );
			secQoSStat = ESessClosed;
			}		

		// Tidy the secondary context
		TSessionStatus &secStat = iSecondaryPacketContextsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId];
		
		if( secStat >= iStatusCloseThresh )
			{
			DoCloseSecondaryPacketContextL( aServerId, aPhoneId, aPacketServiceId, aContextId, secCtxId );
			secStat = ESessClosed;
			}
			
		}
	
	// Tidy our QoS
	TSessionStatus &priQoSStat = iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId]
										[aPacketServiceId][aContextId];
		
	if( priQoSStat >= iStatusCloseThresh )
		{
		DoClosePrimaryPacketContextQoSL( aServerId, aPhoneId, aPacketServiceId, aContextId );
		priQoSStat = ESessClosed;
		}		


	// Tidy the primary context
	TSessionStatus &priStat = iPrimaryPacketContextsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId];
	if ( priStat >= iStatusCloseThresh )
		{
		DoClosePrimaryPacketContextL(aServerId, aPhoneId, aPacketServiceId, aContextId);
		priStat = ESessClosed;
		}	
	}

/** 
* Close PhoneBookStore
*/
void CEtelSessionMgr::ClosePhoneBookStoreSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TPhoneBookStoreId aPhoneBookId )
	{
	TSessionStatus &stat= iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookId];
	if ( stat >= iStatusCloseThresh)
		{
		DoClosePhoneBookStoreL(aServerId, aPhoneId, aPhoneBookId);
		stat = ESessClosed;
		}
	}

/** 
* Close ENStore
*/
void CEtelSessionMgr::CloseENStoreSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TENStoreId aEnStoreId )
	{
	TSessionStatus &stat= iENStoresStatus[aServerId][aPhoneId][aEnStoreId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseENStoreL(aServerId, aPhoneId, aEnStoreId);
		stat = ESessClosed;
		}
	}

/** 
* Close ONStore
*/
void CEtelSessionMgr::CloseONStoreSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TONStoreId aOnStoreId )
	{
	TSessionStatus &stat= iONStoresStatus[aServerId][aPhoneId][aOnStoreId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseONStoreL(aServerId, aPhoneId, aOnStoreId);
		stat = ESessClosed;
		}
	}

/** 
* Close SmsMessaging
*/
void CEtelSessionMgr::CloseSmsMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TSmsMessagingId aSmsMsgId )
	{
	
	// Tidy associated SmsStore
	for (TSmsStoreId smsStoreId = KMeSmsStore; 
			smsStoreId < KMaxSmsStore; 
			smsStoreId = TSmsStoreId(smsStoreId+1))
		{
		CloseSmsStoreSessL( aServerId, aPhoneId, aSmsMsgId, smsStoreId );
		}
	
	
	TSessionStatus &stat= iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMsgId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseSmsMessagingL(aServerId, aPhoneId, aSmsMsgId);
		stat = ESessClosed;
		}
	}

/** 
* Close SmsStore
*/
void CEtelSessionMgr::CloseSmsStoreSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TSmsMessagingId aSmsMsgId, TSmsStoreId aSmsStoreId )
	{
	TSessionStatus &stat= iSmsStoresStatus[aServerId][aPhoneId][aSmsMsgId][aSmsStoreId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseSmsStoreL(aServerId, aPhoneId, aSmsMsgId, aSmsStoreId);
		stat = ESessClosed;
		}
	}

/** 
* Close BroadcastMessaging
*/
void CEtelSessionMgr::CloseBroadcastMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TBroadcastMessagingId aBcastMsgId )
	{
	TSessionStatus &stat= iBroadcastMessagingsStatus[aServerId][aPhoneId][aBcastMsgId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseBroadcastMessagingL(aServerId, aPhoneId, aBcastMsgId);
		stat = ESessClosed;
		}
	}

/** 
* Close UssdMessaging
*/
void CEtelSessionMgr::CloseUssdMessagingSessL(TServerId aServerId, TPhoneId aPhoneId, 
												TUssdMessagingId aUssdMsgId )
	{
	TSessionStatus &stat= iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMsgId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseUssdMessagingL(aServerId, aPhoneId, aUssdMsgId);
		stat = ESessClosed;
		}
	}

/**
 * Close CustomAPI
 */
void CEtelSessionMgr::CloseCustomApiSessL(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
    {
    TSessionStatus &stat= iUssdMessagingsStatus[aServerId][aPhoneId][aCustomApiId];
	if ( stat >= iStatusCloseThresh)
		{
		DoCloseCustomApiL(aServerId, aPhoneId, aCustomApiId);
		stat = ESessClosed;
		}
    }

//
// Tidy notification cascade
//


/**
*	Tidy notifiers for phone subsession and all associated subsessions
*/	
void CEtelSessionMgr::TidyPhoneNotifiers(TServerId aServerId, TPhoneId aPhoneId)
	{		
	
	// Tidy associated sessions
	
	// Lines
	for (TLineId lineId = KVoiceLine; lineId < KMaxLines; lineId = TLineId(lineId+1))
		{
		TidyLineNotifiers( aServerId, aPhoneId, lineId );
		}
		
	// Conference calls
	for (TConferenceCallId ccId = KMainConferenceCall; ccId < KMaxConferenceCall; 
				ccId = TConferenceCallId(ccId+1))
		{
		TidyConfCallNotifiers( aServerId, aPhoneId, ccId );
		}
	
	// Packet services
	for (TPacketServiceId packetServiceId = KMainPacketService; 
			packetServiceId < KMaxPacketService; 
			packetServiceId = TPacketServiceId(packetServiceId+1))
		{
		TidyPacketServiceNotifiers( aServerId, aPhoneId, packetServiceId );
		}
		
	// PhoneBookStore
	for (TPhoneBookStoreId phoneBookId = KMeAdnPhoneBook; 
			phoneBookId < KMaxPhoneBookStore; 
			phoneBookId = TPhoneBookStoreId(phoneBookId+1))
		{
		TidyPhoneBookStoreNotifiers( aServerId, aPhoneId, phoneBookId );
		}
		
	// ENStore
	for (TENStoreId enStoreId = KMainENStore; 
			enStoreId < KMaxENStore; 
			enStoreId = TENStoreId(enStoreId+1))
		{
		TidyENStoreNotifiers( aServerId, aPhoneId, enStoreId );
		}
		
	// ONStore
	for (TONStoreId onStoreId = KMainONStore; 
			onStoreId < KMaxONStore; 
			onStoreId = TONStoreId(onStoreId+1))
		{
		TidyONStoreNotifiers( aServerId, aPhoneId, onStoreId );
		}
				
	// SmsMessaging
	for (TSmsMessagingId smsMsgId = KMainSmsMessaging; 
			smsMsgId < KMaxSmsMessaging; 
			smsMsgId = TSmsMessagingId(smsMsgId+1))
		{
		TidySmsMessagingNotifiers( aServerId, aPhoneId, smsMsgId );
		}
		
	// BroadcastMessaging
	for (TBroadcastMessagingId bcastMsgId = KMainBroadcastMessaging; 
			bcastMsgId < KMaxBroadcastMessaging; 
			bcastMsgId = TBroadcastMessagingId(bcastMsgId+1))
		{
		TidyBroadcastMessagingNotifiers( aServerId, aPhoneId, bcastMsgId );
		}
		
	// UssdMessaging
	for (TUssdMessagingId ussdMsgId = KMainUssdMessaging; 
			ussdMsgId < KMaxUssdMessaging; 
			ussdMsgId = TUssdMessagingId(ussdMsgId+1))
		{
		TidyUssdMessagingNotifiers( aServerId, aPhoneId, ussdMsgId );
		}

    // CustomAPI
    for (TCustomApiId customApiId = KMainCustomApi; 
            customApiId < KMaxCustomApi; 
            customApiId = TCustomApiId(customApiId+1))
        {
        TidyCustomApiNotifiers( aServerId, aPhoneId, customApiId );
        }

	// Finally, tidy phone
	if ( iPhonesStatus[aServerId][aPhoneId] > ESessClosed)
		{
		// Cleanup both sets of notification for base and derived
		CleanupPhoneNotifiers( iPhones[aServerId][aPhoneId] );
		CleanupMobilePhoneNotifiers( iPhones[aServerId][aPhoneId] );
		}
	}
	
/**
*	Tidy notifiers for lines
*/
void CEtelSessionMgr::TidyLineNotifiers(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId )
	{
	
	// Tidy associated call sessions
	for (TCallId callId = KCall1; callId < KMaxCalls; callId = TCallId(callId+1))
		{
		TidyCallNotifiers( aServerId, aPhoneId, aLineId, callId );
		}

	// Tidy this object
	if( iLinesStatus[aServerId][aPhoneId][aLineId] > ESessClosed)
		{
		// Cleanup both sets of notification for base and derived 
		CleanupLineNotifiers(iLines[aServerId][aPhoneId][aLineId]);
		CleanupMobileLineNotifiers(iLines[aServerId][aPhoneId][aLineId]);
		}
	}
	

/**
*	Tidy notifiers for calls
*/
void CEtelSessionMgr::TidyCallNotifiers(TServerId aServerId, TPhoneId aPhoneId, TLineId aLineId, 
											TCallId aCallId )
	{
	
	// Tidy associated fax sessions
	for (TFaxId faxId = KMainFax; faxId < KMaxFax; faxId = TFaxId(faxId+1))
		{
		TidyFaxNotifiers( aServerId, aPhoneId, aLineId,aCallId,faxId);
		}

	
	// Tidy this object
	if ( iCallsStatus[aServerId][aPhoneId][aLineId][aCallId] > ESessClosed )
		{
		CleanupCallNotifiers( iCalls[aServerId][aPhoneId][aLineId][aCallId] );
		CleanupMobileCallNotifiers( iCalls[aServerId][aPhoneId][aLineId][aCallId] );
		}
	}

/**
*	Tidy notifiers for fax
*/
void CEtelSessionMgr::TidyFaxNotifiers(TServerId, TPhoneId, TLineId, TCallId, TFaxId )
	{
	
	// No notifiers at present 
	//
	//if (iFaxesStatus[aServerId][aPhoneId][aLineId][aCallId][aFaxId]  > ESessClosed)
	//	{
	//	CleanupFaxNotifiers( iFaxes[aServerId][aPhoneId][aLineId][aCallId][aFaxId] );
	//	}	
	}


/**
*	Tidy notifiers for conf calls
*/
void CEtelSessionMgr::TidyConfCallNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
												TConferenceCallId aConfCallId )
	{
	
	TSessionStatus &stat= iConferenceCallsStatus[aServerId][aPhoneId][aConfCallId];
	if ( stat > ESessClosed)
		{
		CleanupMobileConferenceCallNotifiers( iConferenceCalls[aServerId][aPhoneId][aConfCallId]);
		}
	}
	
/**
*	Tidy notifiers for packet service
*/
void CEtelSessionMgr::TidyPacketServiceNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
													TPacketServiceId aPacketServiceId )
	{
		// Tidy associated primary contexts
	for (TPrimaryPacketContextId priCtxId = KPrimaryPacketContext1; 
						priCtxId < KMaxPrimaryPacketContext; priCtxId=TPrimaryPacketContextId(priCtxId+1) )
		{
		TidyPacketServiceContextNotifiers( aServerId, aPhoneId, aPacketServiceId,priCtxId);
		}

	// Tidy this object
	TSessionStatus &stat= iPacketServicesStatus[aServerId][aPhoneId][aPacketServiceId];
	if ( stat > ESessClosed)
		{
		CleanupPacketServiceNotifiers(iPacketServices[aServerId][aPhoneId][aPacketServiceId]);
		}
	}

/**
*	Tidy notifiers for packet context and associated secondaries / QoS
*/
void CEtelSessionMgr::TidyPacketServiceContextNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
															TPacketServiceId aPacketServiceId, 
															TPrimaryPacketContextId aContextId)
	{
	
	// Tidy secondaries and associated QoS
	for (TSecondaryPacketContextId secCtxId = KSecondaryPacketContext1; 
						secCtxId < KMaxSecondaryPacketContext; secCtxId=TSecondaryPacketContextId(secCtxId+1) )
		{

		// If the secondary has a QoS, close it first
		TSessionStatus &secQoSStat = iSecondaryPacketContextQoSsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId];
			
		if( secQoSStat > ESessClosed )
			{
			CleanupPacketQoSNotifiers( iSecondaryPacketContextQoSs[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId] );

			}		

		// Tidy the secondary context
		TSessionStatus &secStat = iSecondaryPacketContextsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId];
		
		if( secStat > ESessClosed )
			{
			CleanupPacketContextNotifiers( iSecondaryPacketContexts[aServerId][aPhoneId]
											[aPacketServiceId][aContextId][secCtxId] );
			}
			
		}
	
	// Tidy our QoS
	TSessionStatus &priQoSStat = iPrimaryPacketContextQoSsStatus[aServerId][aPhoneId]
										[aPacketServiceId][aContextId];
		
	if( priQoSStat > ESessClosed )
		{
		CleanupPacketQoSNotifiers( iPrimaryPacketContextQoSs[aServerId][aPhoneId]
										[aPacketServiceId][aContextId] );

		}		


	// Tidy the primary context
	TSessionStatus &priStat = iPrimaryPacketContextsStatus[aServerId][aPhoneId]
											[aPacketServiceId][aContextId];
	if ( priStat > ESessClosed )
		{
		CleanupPacketContextNotifiers(iPrimaryPacketContexts[aServerId][aPhoneId]
											[aPacketServiceId][aContextId]);

		}	
	}

/** 
* Tidy PhoneBookStore
*/
void CEtelSessionMgr::TidyPhoneBookStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
														TPhoneBookStoreId aPhoneBookStoreId  )
	{
	TSessionStatus &stat= iPhoneBookStoresStatus[aServerId][aPhoneId][aPhoneBookStoreId];
	if ( stat > ESessClosed)
		{
		CleanupMobilePhoneStoreNotifiers(iPhoneBookStores[aServerId][aPhoneId][aPhoneBookStoreId]);
		}
	}

/** 
* Tidy ENStore
*/
void CEtelSessionMgr::TidyENStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, TENStoreId aENStoreId)
	{
	TSessionStatus &stat= iENStoresStatus[aServerId][aPhoneId][aENStoreId];
	if ( stat > ESessClosed)
		{
		CleanupMobilePhoneStoreNotifiers(iENStores[aServerId][aPhoneId][aENStoreId]);
		}
	}

/** 
* Tidy ONStore
*/
void CEtelSessionMgr::TidyONStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, TONStoreId aONStoreId)
	{
	TSessionStatus &stat= iONStoresStatus[aServerId][aPhoneId][aONStoreId];
	if ( stat > ESessClosed)
		{
		CleanupMobilePhoneStoreNotifiers(iONStores[aServerId][aPhoneId][aONStoreId]);
		}
	}

/** 
* Tidy SmsMessaging
*/
void CEtelSessionMgr::TidySmsMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
														TSmsMessagingId aSmsMsgId )
	{
	
	// Tidy associated Sms
	for (TSmsStoreId smsStoreId = KMeSmsStore; 
			smsStoreId < KMaxSmsStore; 
			smsStoreId = TSmsStoreId(smsStoreId+1))
		{
		TidySmsStoreNotifiers( aServerId, aPhoneId, aSmsMsgId, smsStoreId );
		}
	
	
	TSessionStatus &stat= iSmsMessagingsStatus[aServerId][aPhoneId][aSmsMsgId];
	if ( stat > ESessClosed)
		{
		CleanupMobileSmsMessagingNotifiers(iSmsMessagings[aServerId][aPhoneId][aSmsMsgId]);

		}
	}

/** 
* Tidy SmsStore
*/
void CEtelSessionMgr::TidySmsStoreNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
													TSmsMessagingId aSmsMsgId, TSmsStoreId aSmsStoreId )
	{
	TSessionStatus &stat= iSmsStoresStatus[aServerId][aPhoneId][aSmsMsgId][aSmsStoreId];
	if( stat > ESessClosed)
		{
		CleanupMobilePhoneStoreNotifiers(iSmsStores[aServerId][aPhoneId][aSmsMsgId][aSmsStoreId]);
		}
	}

/** 
* Tidy BroadcastMessaging
*/
void CEtelSessionMgr::TidyBroadcastMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
															TBroadcastMessagingId aBcastMsgId )
	{
	TSessionStatus &stat= iBroadcastMessagingsStatus[aServerId][aPhoneId][aBcastMsgId];
	if ( stat > ESessClosed)
		{
		CleanupMobileBroadcastMessagingNotifiers(iBroadcastMessagings[aServerId][aPhoneId][aBcastMsgId]);

		}
	}

/** 
* Tidy UssdMessaging
*/
void CEtelSessionMgr::TidyUssdMessagingNotifiers(TServerId aServerId, TPhoneId aPhoneId, 
														TUssdMessagingId aUssdMsgId )
	{
	TSessionStatus &stat= iUssdMessagingsStatus[aServerId][aPhoneId][aUssdMsgId];
	if ( stat > ESessClosed)
		{
		CleanupMobileUssdMessagingNotifiers(iUssdMessagings[aServerId][aPhoneId][aUssdMsgId]);
		}
    }


/** 
* Tidy CustomApi
*/
void CEtelSessionMgr::TidyCustomApiNotifiers(TServerId aServerId, TPhoneId aPhoneId, TCustomApiId aCustomApiId)
	{
	TSessionStatus &stat= iCustomApisStatus[aServerId][aPhoneId][aCustomApiId];
	if ( stat > ESessClosed)
		{
		CleanupCustomAPINotifiers(iCustomApis[aServerId][aPhoneId][aCustomApiId]);
		}
    }
