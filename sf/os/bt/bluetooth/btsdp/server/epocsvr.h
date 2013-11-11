// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef EPOCSVR_H
#define EPOCSVR_H

#include <e32base.h>
#include <btdevice.h>
#include <bluetooth/eirpublisher.h>
#include "SDPDatabase.h"

class CSdpServSession;
class CSdpDatabaseSubSession;
class CSdpUuidManager;

/**
SDP Server Shutdown Timer.
Closes the SDP server after a preset delay. Activated before the first client connects
and after the last client disconnects.
**/
NONSHARABLE_CLASS(CSdpServerShutdown) : public CTimer
	{
	enum {KSdpServerShutdownDelay=5000000};	// approx 5s
public:
	inline CSdpServerShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};

_LIT(KSdpServerPanic,"SdpServInternal");
enum TSdpServerPanic
	{
	ESdpStoredAttrValNotEncoded = 0,
	ESdpAttributeEncodingFailed = 1,
	ESdpBadState = 2,
	ESdpReadAlreadyOutstanding = 3,
	ESdpOutstandingOperation = 4,
	};

void PanicServer(TSdpServerPanic aPanic);
void PanicClient(const RMessage2& aMessage, TSdpClientPanic aPanic);

/**
   The EPOC SDP Server.
   This is the start of the EPOC client/server
   class hierarchy
**/
NONSHARABLE_CLASS(CSdpServer) : public CPolicyServer
	{
public:
	//construct / destruct
	static CSdpServer* NewLC();
	CSdpDatabase* Database();
	void BuildRecordZeroL();
	~CSdpServer();

	void AddSession();
	void DropSession();

	inline CSdpListener& Listener() const;

	TInt RunError(TInt aError);
	
	//database operations
	CSdpServRecord* CreateServiceRecordL(const RMessage2& aMessage);
	CSdpServRecord* FindAndCheckServiceRecordForDeletion(const RMessage2& aMessage);
	void UpdateAttributeL(const RMessage2& aMessage);
	void DeleteAttribute(const RMessage2& aMessage);
	void DeleteServiceRecord(CSdpServRecord* aServiceRecord);
	
private:
	CSdpServer();
	void ConstructL();
	//open/close a session
	CSession2* NewSessionL(const TVersion &aVersion) const;
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;

	void CheckAllowedL(const RMessage2& aMessage, const CSdpServRecord& aRecord);
	TBool Allowed(const RMessage2& aMessage, const CSdpServRecord& aRecord);
	
	CSdpServRecord* FindRecordByHandle(const TSdpServRecordHandle aHandle);
	CSdpAttr* FindAttributeByID(CSdpServRecord& aRecord, const TSdpAttributeID aAttrId);

	void UpdateOrCreateAtributeL(CSdpServRecord& aRecord, TSdpAttributeID aAttrId, const TDesC8& aPtr);
	void DatabaseStateChange();

private:
	TInt iMaxSessionCount;
	TInt iSessionCount;

	CSdpServerShutdown		iShutdown;
	
	RSocketServ				iSocketServ;
	CSdpListener*			iSdpListener;
	CSdpDatabase*			iSdpDatabase;

	CSdpServRecord*			iRecZero;
	CSdpAttrValueUint*		iDbState;			// This value is used to update a state attribute whenever a record is 
												// added or removed to facilitate caching by remotes
	CAttrEncoderVisitor*	iEncoderVisitor;	// We maintain a pre-alloced encoder visitor so we can update 
												// attributes without having to allocate memory.
	RBuf8					iEncodeBuf;			// Used by the encoder to encode into
	CSdpUuidManager*		iUuidManager;
	};


/**
   SDP Server Session.
   General purpose session used to create functionally specific subsessions.
**/
NONSHARABLE_CLASS(CSdpServSession) : public CSession2
	{
private:
	enum TSubSessionType{ESdpAgent, ESdpDatabase}; /*however sdpagent is still 
												     part of esock and may never
													 be moved here*/

public:
	CSdpServSession(const RMessage2& aMessage);
	~CSdpServSession();

	void ServiceL(const RMessage2 &aMessage);

	//Create/delete subsession
	void NewSubSessionL(TSubSessionType aType, const RMessage2& aMessage);
	void CloseSubSession(const RMessage2& aMessage); //can't fail - can panic client

	//resource counting
	void NumResourcesL(const RMessage2& aMessage);
	TInt CountResources();

	CSdpServer& Server();
	void CreateL(const CServer2& aServer);

private:
	void ConstructL();
	void CreateL();

	void DeleteSubsession(TUint aHandle, const RMessage2& aMessage);
	void RemoveSessionRecords();
	
private:
	TUint iSubSessionCount;
	RPointerArray<CSdpServRecord> iSessionRecords;
	};


//**********************************
// CEirPublisherSdpUuidBase
//**********************************
/**
CEirPublisherSdpUuidBase provides base class for Sdp Uuid Eir Publisher.
**/
NONSHARABLE_CLASS(CEirPublisherSdpUuidBase) : public CBase, public MEirPublisherNotifier
	{
public:
	static CEirPublisherSdpUuidBase* NewL(CSdpUuidManager& aSdpUuidManager, TEirTag aTag);

	~CEirPublisherSdpUuidBase();
	void UpdateUuids(TInt aLength);

protected:
	CEirPublisherSdpUuidBase(CSdpUuidManager& aSdpUuidManager, TEirTag aTag);
	void ConstructL();

protected:
	CSdpUuidManager& iParent;
	CEirPublisher* iPublisher; 
	TEirTag iTag;
	};

//**********************************
// CEirPublisherSdpUuid16
//**********************************
/**
Provides functionality to publish 16 bit UUIDs to EIR.
**/
NONSHARABLE_CLASS(CEirPublisherSdpUuid16) : public CEirPublisherSdpUuidBase
	{
public:
	static CEirPublisherSdpUuid16* NewL(CSdpUuidManager& aSdpDatabase);
	~CEirPublisherSdpUuid16();

private:
	CEirPublisherSdpUuid16(CSdpUuidManager& aSdpDatabase);

	// From MEirPublisherNotifier
	void MepnSpaceAvailable(TUint aBytesAvailable);
	void MepnSetDataError(TInt aResult);

private:
	HBufC8* iExtracted;
	};

//**********************************
// CEirPublisherSdpUuid128
//**********************************
/**
Provides functionality to publish 128 bit UUIDs to EIR.
**/
NONSHARABLE_CLASS(CEirPublisherSdpUuid128) : public CEirPublisherSdpUuidBase
	{
public:
	static CEirPublisherSdpUuid128* NewL(CSdpUuidManager& aSdpDatabase);
	~CEirPublisherSdpUuid128();

private:
	CEirPublisherSdpUuid128(CSdpUuidManager& aSdpDatabase);

	// From MEirPublisherNotifier
	void MepnSpaceAvailable(TUint aBytesAvailable);
	void MepnSetDataError(TInt aResult);

private:
	HBufC8* iExtracted;
	};


//**********************************
// CSdpUuidManager
//**********************************
/**
Retrieve UUIDs from CSdpDatabase and then store them in 16 or 128 bit Eir Uuid PublishersUUIDs
**/
NONSHARABLE_CLASS(CSdpUuidManager) : public CBase
	{
public:
	static CSdpUuidManager* NewL(CSdpDatabase& aSdpDatabase);
	~CSdpUuidManager();
	
	void NotifySdpRecordChange();
	void AddUuid(TUUID aUuid);
	void ResetUuids();
	HBufC8* GetAll16BitUUIDs(TInt aBytesAvailable, TBool& aPartial);
	HBufC8* GetAll128BitUUIDs(TInt aBytesAvailable, TBool& aPartial);

private:
	CSdpUuidManager(CSdpDatabase& aSdpDatabase);
	void ConstructL();

private:
	CSdpDatabase& 				iSdpDatabase;
	CAttrUuidVisitor*			iUuidVisitor;
	CEirPublisherSdpUuid16* 	iEirPublisherSdpUuid16;
	CEirPublisherSdpUuid128*	iEirPublisherSdpUuid128;
	RUUIDContainer				i16BitUUIDs;
	RUUIDContainer				i128BitUUIDs;
	};

enum TSdpServerPanics
	{
	ESdpServerUuidFlattenBroken = 0,
	ESdpServerDeleteServiceRecordHandleZero = 1,
	ESdpServerUpdateAttributeRecordHandleZero = 2,
	ESdpServerDeleteAttributeRecordHandleZero = 3
	};
_LIT(KSdpServerPanicName, "SdpServer");

#endif
