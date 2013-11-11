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
//

/**
 @file
 @internalComponent
 @deprecated
*/

#ifndef SOCKETWRITER_H_
#define SOCKETWRITER_H_

#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <securesocket.h>
#include "lbstimer.h"
#include <lbs/lbshostsettings.h>
#include "observers.h"



// Forwards
class CSuplSocketRequestQueue;
class CSocketReader;
class CSecureSocketReader;
class CLbsCallbackTimer;

class CSocketWriterBase : public CActive, 
						  public MLbsCallbackTimerObserver
	{
public:
	~CSocketWriterBase();

	// Methods
	void Connect(const TSuplLocalSessionId sessionId);
	void Disconnect();
	void SendMessageL(CSuplMessageBase* aMessage, const TSuplLocalSessionId& aSessionId);
	TLbsHostSettingsId HostSettingsId();
	TInt CallbackId();	
	
	// CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
protected:
	// Constructors
	CSocketWriterBase(const TLbsHostSettingsId& aSlpId,
					  TInt iCallbackId,
		   	  		  MSuplSocketObserver& aSocketWriterObserver,
		   	  		  RSocketServ& aSocketServer,
		   	  		  const TLbsHostSettingsSupl& aHostSettings);
	void BaseConstructL();
	
	// Methods
	void ResolveHost();
	void ConnectConnection();
	void CheckQueue();
	void DoConnect();
	void SelfComplete();
	
	// Virtual Methods
	virtual void DoConnectSocket() = 0;
	virtual void DoDisconnect() = 0;
	virtual void DoStartReaderL() = 0;
	virtual void DoSendMessage(TDesC8& aMessage) = 0;

	// Timer
	void StartTimer();
	void StopTimer();
private:
	// MLbsCallbackTimerObserver
	virtual void OnTimerEventL(TInt aTimerId);
	virtual TInt OnTimerError(TInt aTimerId, TInt aError);
protected:
	enum TSuplSocketState
	{
		EIdleDisconnected = 0,
		EResolvingSlpAddress,
		EConnectingConnection,
		EConnectingSocket,
		ETlsHandshaking,
		EIdleConnected,
		ESendingMessage,
		EClosing,
		ENone
	};
	
	TSuplSocketState iState;
	
	// TCP Socket
	RSocket iSocket;
	
	// Information about the resolved address
	TNameEntry iNameEntry;
	
	// Actual internet address
	TInetAddr iAddr;
	
	// Socket server
	RSocketServ& iSocketServ;
	
	// Connection
	RConnection iConnection;
	
	// Observer
	MSuplSocketObserver& iObserver;
	
	// Session index in connection manager => Used for call backs
	TInt iCallbackId;
	
	// Count of number of current sessions
	TInt iSessionCount;
	
	// Session Id of active request
	TSuplLocalSessionId iLastSessionId;
	
	// The actual host settings
	const TLbsHostSettingsSupl iHostSettings;
private:
	// Index into the Host Settings DataBase where SLP info is stored
	TLbsHostSettingsId iSlpSettings;
	
	// Resolver to find out the SLP's IP address
	RHostResolver iResolver;
	
	// Queue of messages to send
	CArrayFixFlat<HBufC8*> iQueue;
	
	// Call back timer
	CLbsCallbackTimer* iTimeout;
	};


class CSocketWriter : public CSocketWriterBase
	{
public:
	// Constructors
	static CSocketWriter* NewL(const TLbsHostSettingsId& aSlpId,
							   TInt aCallbackIndex,
							   MSuplSocketObserver& aSocketWriterObserver,
							   RSocketServ& aSocketServer,
							   TLbsHostSettingsSupl& aHostSettings);
	~CSocketWriter();
protected:
	// Constructors
	CSocketWriter(const TLbsHostSettingsId& aSlpId,
			   	  TInt aCallbackIndex,
				  MSuplSocketObserver& aSocketWriterObserver,
			   	  RSocketServ& aSocketServer,
			   	  TLbsHostSettingsSupl& aHostSettings);

	// CSocketWriterBase
	void DoConnectSocket();
	void DoDisconnect();
	void DoSendMessage(TDesC8& aMessage);
	void DoStartReaderL();
private:
	CSocketReader* iSocketReader;
	};


class CSecureSocketWriter : public CSocketWriterBase
	{
public:
	// Constructors
	static CSecureSocketWriter* NewL(const TLbsHostSettingsId& aSlpId,
							   		 TInt aCallbackIndex,
							   		 MSuplSocketObserver& aSocketWriterObserver,
							   		 RSocketServ& aSocketServer,
							   		 TLbsHostSettingsSupl& aHostSettings,
					   	  		     const CSuplSessionRecord::TServiceType& aServiceType);
	~CSecureSocketWriter();
protected:
	// Constructors
	CSecureSocketWriter(const TLbsHostSettingsId& aSlpId,
			   	  		TInt aCallbackIndex,
			   	  		MSuplSocketObserver& aSocketWriterObserver,
			   	  		RSocketServ& aSocketServer,
			   	  		TLbsHostSettingsSupl& aHostSettings,
		   	  		    const CSuplSessionRecord::TServiceType& aServiceType);
	
	// CActive
	void RunL(); // Override of RunL to handle security handshaking
	void DoCancel();

	// CSocketWriterBase
	void DoConnectSocket();
	void DoDisconnect();
	void DoSendMessage(TDesC8& aMessage);
	void DoStartReaderL();
	
	// New functions
	void StartSecureSocketL();
	TBool CheckTlsSecureL();
private:
	CSecureSocketReader* iSocketReader;
	
	CSecureSocket* iSecureSocket;
	
  	CSuplSessionRecord::TServiceType iServiceType;
	
	};

#endif /*SOCKETWRITER_H_*/
