// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to CellBroadcast related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHCELLBROADCASTINTERFACE_H_
#define MLTSYDISPATCHCELLBROADCASTINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>


namespace DispatcherCellBroadcast
	{
	
	/**
	 * This namespace contains all types relating to the Cell Broadcast dispatcher.
	 */
	
	
	struct TWcdmaCbsMsgBase
		{
	    /*
	     * Number of Pages
	     */
		TInt iNumberOfPages;

	    /*
	     * Message Type
	     */ 			
		TUint8 iMessageType;

	    /*
	     * Message ID
	     */ 		
		TUint16 iMessageId;

	    /*
	     * Serial Number
	     */
		TUint16 iSerialNum;
		
	    /*
	     * Data Coding Scheme
	     */
		TUint8 iDCS;

	    /*
	     * Info length
	     */
		TUint8 iInfoLength;
		};

	}

class MLtsyDispatchCellBroadcastSetBroadcastFilterSetting : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId = KDispatchCellBroadcastFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileBroadcastMessagingSetFilterSetting
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCellBroadcastSetBroadcastFilterSettingComp()
	 * 
	 * Implementation of this interface sets a new value for the filter setting.
	 * The filter specifies which broadcast messages are accepted and which are rejected
	 *
	 *
     * @param aFilterSetting struct settings for a filter to accept/reject messages
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileBroadcastMessaging::SetFilterSetting()
	 */
	virtual TInt HandleSetBroadcastFilterSettingReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting) = 0;

	}; // class MLtsyDispatchCellBroadcastSetBroadcastFilterSetting



class MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId = KDispatchCellBroadcastFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileBroadcastMessagingReceiveMessage
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCellBroadcastActivateBroadcastReceiveMessageComp()
	 * 
	 * RMobileBroadcastMessaging::ReceiveMessage will be completed when a new incoming broadcast message has been received
	 * via CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd() or CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd()
	 * 
     * @param aFilterSetting struct settings for a filter to accept/reject messages
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileBroadcastMessaging::ReceiveMessage()
	 * @see CCtsyDispatcherCallback::CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd()
	 * @see CCtsyDispatcherCallback::CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd()
	 */
	virtual TInt HandleActivateBroadcastReceiveMessageReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting) = 0;

	}; // class MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage



class MLtsyDispatchCellBroadcastReceiveMessageCancel : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCellBroadcastReceiveMessageCancelApiId = KDispatchCellBroadcastFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileBroadcastMessagingReceiveMessageCancel
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCellBroadcastReceiveMessageCancelComp()
	 *
	 *
     * @param aFilterSetting struct settings for a filter to accept/reject messages
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileBroadcastMessaging::ReceiveMessage()
	 * @see RMobileBroadcastMessaging::Close()
	 */
	virtual TInt HandleReceiveMessageCancelReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting ) = 0;

	}; // class MLtsyDispatchCellBroadcastReceiveMessageCancel
	
class MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId = KDispatchCellBroadcastFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomStartSimCbTopicBrowsingIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCellBroadcastStartSimCbTopicBrowsingComp()
	 *     
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStartSimCbTopicBrowsingReqL() = 0;

	}; // class MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing	

class MLtsyDispatchCellBroadcastDeleteSimCbTopic : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId = KDispatchCellBroadcastFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomDeleteSimCbTopicIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackCellBroadcastDeleteSimCbTopicComp()
	 *     
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleDeleteSimCbTopicReqL(TUint aIndex, TBool aDeleteFlag) = 0;

	}; // class MLtsyDispatchCellBroadcastDeleteSimCbTopic	


#endif /*MLTSYDISPATCHCELLBROADCASTINTERFACE_H_*/
