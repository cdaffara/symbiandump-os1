// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SURFACEUPDATESERVER_H__
#define __SURFACEUPDATESERVER_H__

/**
@file
@internalTechnology
*/

#include <e32std.h>
#include <e32base.h>
#include <graphics/updateserverprovider.h>
#include <graphics/compositionsurfaceupdate.h>

enum  TSurfaceUpdateServPanic
	{
	EUpdateServPanicBadRequest = 1,
	EUpdateServPanicNoMemory,
	EUpdateServPanicStartUp,
	EUpdateServPanicDataIntegrity,
	EUpdateServPanicRegister,
	EUpdateServPanicGlobalFastLock,
	};

enum TSurfaceUpdateEvent
	{
	EUpdateServEventRegister = 1,
	EUpdateServEventTerminate //this event will only be used for test purpose
	};
	
class CSurfaceUpdateServer;

/**
The class implements interface(s) required for content update
Intended to be used by Surface-update control flow
@see MSurfaceUpdateServerProvider
*/
class CSurfaceUpdateServerProvider : public CActive, public MSurfaceUpdateServerProvider
	{
public:
	static CSurfaceUpdateServerProvider* NewL(CActive::TPriority aPriority, CSurfaceUpdateServer* aServer);
	~CSurfaceUpdateServerProvider();
	//from MSurfaceUpdateServerProvider
	IMPORT_C TInt Register(TInt aScreen, CBase* aUpdateReceiver, TInt aPriority);
	//the following function is intended to be used for test purpose only, 
	//will have no effect in the release build 
	IMPORT_C void Terminate();
protected:
	CSurfaceUpdateServerProvider(CActive::TPriority aPriority, CSurfaceUpdateServer* aServer);
	void ConstructL();
	void RunL();
	void DoCancel();
protected:
	CSurfaceUpdateServer* iServer;
	TThreadId iThreadId;
	TInt iRegisterErr;
	CBase* iUpdateReceiver;
	TInt iScreen;
	TInt iPriority;
	RSemaphore iSemaphore;
	};
/**
Priority is used to identify the master screen for the surface. 
*/
class TUpdateReceiverPriorityEntry
	{
public:
	TInt iPriority; //Highest number signifies highest priority of the screen.
	MCompositionSurfaceUpdate* iUpdateReceiver; //doesn't own
	};


/**
The server maintains session with the clients. 
It starts during the initialization of the Compositor thread.  
*/
class CSurfaceUpdateServer : public CServer2
	{
friend class CSurfaceUpdateSession;
public:
	static CSurfaceUpdateServer* NewL();
	
	~CSurfaceUpdateServer();

	TInt Register(TInt aScreen, CBase* aUpdateReceiver, TInt aPriority); 
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	static void PanicServer(TSurfaceUpdateServPanic aPanic);

	/**
	Provide surface update provider for usage by the content update receiver.
	
	@return surface update provider
	*/
	MSurfaceUpdateServerProvider* SurfaceUpdateProvider() const
		{
		return static_cast <MSurfaceUpdateServerProvider*> (iServerProvider);
		}
	static TInt ThreadFunction(TAny* aAny);
	// for DEF118736: [v9.5] turfaceupdatetestserver unexecuted
	// For test purposes, only if iNumberPendingNotification reaches zero, the surface update server can be shut.
	// The counter is incremented by one when the CUpdateReceiverNotification AO is set to self-destroyed in surface update session's destructor()
	// The counter is decremented by one before its self-destruction in CUpdateReceiverNotification AO's RunL()
#ifdef TEST_SURFACE_UPDATE
	TInt iNumberPendingNotification;
#endif
protected:
	void ConstructL();
	CSurfaceUpdateServer(CActive::TPriority aPriority);
	MCompositionSurfaceUpdate* UpdateReceiver(TInt aScreen) const;
	/**
	 @return content update receiver priority list   
	 */
	inline const RPointerArray<TUpdateReceiverPriorityEntry>& UpdateReceiverPriority() const;
	
	inline TInt NumUpdateReceivers() const;
	static TInt CompareUpdateReceiverPriority(const TUpdateReceiverPriorityEntry& aEntry1, const TUpdateReceiverPriorityEntry& aEntry2);
protected:
	RPointerArray<MCompositionSurfaceUpdate> iUpdateReceiver; //Screen number is the index for the access of the receiver entry in the array
	RPointerArray<TUpdateReceiverPriorityEntry> iUpdateReceiverPriorityOrder; //stored in priority order, the higher the priority of the receiver the closer to the beginning of the array
	CSurfaceUpdateServerProvider* iServerProvider;
	};


enum TNotificationType
	{
	EUpdateSrvReusable = 1, /**< When a notification batch doesn’t contain any active notifiers, i.e. they have been processed by the content update receiver, the batch will not be deleted but becomes available for the following reuse.*/
	EUpdateSrvAvailable,
	EUpdateSrvDisplayed,
	EUpdateSrvDisplayedXTimes,
	};

class CSurfaceUpdateSession;
class CUpdateReceiverNotificationBatch;

/**
Notification object. Content update receiver signals the server via this object of composition events.
*/
class CUpdateReceiverNotification : public CActive
	{
public:
	CUpdateReceiverNotification(CActive::TPriority aPriority, TInt aReceiverPriority, CUpdateReceiverNotificationBatch *aParentNotificationBatch);
	~CUpdateReceiverNotification();
	TRequestStatus& Status();
	void Activate();
protected:
	void DoCancel();
	virtual void RunL();
public:
	TUint32	iTimeStamp;
	TInt iUpdateReceiverPriority;
	TBool iSelfDestructWhenRun;
protected:
#ifdef TEST_SURFACE_UPDATE
	void DecNumberPendingNotifications();
#endif	
protected:
	CUpdateReceiverNotificationBatch *iParentNotificationBatch; //doesn't own
#ifdef TEST_SURFACE_UPDATE
	CSurfaceUpdateServer* iServer;
#endif
	};

/**
 The class manages the batch of notification objects with the same type and 
 initiated by the particular SubmitUpdate request.
*/
class CUpdateReceiverNotificationBatch : public CBase
	{
public:
	CUpdateReceiverNotificationBatch(CSurfaceUpdateSession *aSession, TInt aNumReceivers);
	~CUpdateReceiverNotificationBatch();
	void ProcessNotificationEvent(CUpdateReceiverNotification* aReceiverNotification);
	TBool IsActive() const;
	CUpdateReceiverNotification* UpdateReceiverNotification(TInt aReceiverPriority = 0);
	void CheckForReuse();
	void SetNumUpdateReceivers(TInt aNumUpdateReceivers);
#ifdef TEST_SURFACE_UPDATE
	CSurfaceUpdateServer* Server();
	void IncNumberPendingNotifications();
#endif
public:
	RMessage2 iMsg;
	TNotificationType iType;
	RPointerArray<CUpdateReceiverNotification> iUpdateReceiverNotifications; //for multiple entries stored in priority order, the higher the priority of the receiver the closer to the beginning of the array
protected:
	CSurfaceUpdateSession *iSession; //doesn't own
	TInt iNumUpdateReceivers;
	TBool iCompleteWithSuccess;
	TInt iHighestPriority; //attributed to successful notification
	TUint32	iTimeStamp; //attributed to successful notification
	};

/**
Maintain the channel between clients and the server. 
Functions are provided will respond appropriately to client messages. 
*/
class CSurfaceUpdateSession : public CSession2
	{
public:
	CSurfaceUpdateSession(const RPointerArray<TUpdateReceiverPriorityEntry>& aReceiverEntryList);
	~CSurfaceUpdateSession();
	void SubmitUpdate(const RMessage2& aMessage);
	void NotifyWhenAvailable(const RMessage2& aMessage);
	void NotifyWhenDisplayed(const RMessage2& aMessage);
	void NotifyWhenDisplayedXTimes(const RMessage2& aMessage);
	void CancelAllUpdateNotifications();
protected:
	void DoSubmitUpdateL(const RMessage2& aMessage);
	void StoreNotification(CUpdateReceiverNotificationBatch*& aNotifier, const RMessage2& aMessage, TNotificationType aType);
	virtual void ServiceL(const RMessage2& aMessage);
	void PanicClient(const RMessage2& aMessage, TInt aPanic) const;
	CUpdateReceiverNotificationBatch* UpdateReceiverNotificationBatchL();
	void IssueRequestComplete(TInt aErr);
	void DispatchUpdate(const TSurfaceId& aSurfaceId, TInt aBuffer, RRegion* aRegion, TInt* aDisplayedXTimes,  MCompositionSurfaceUpdate* aReceiver = NULL );
#ifdef TEST_SURFACE_UPDATE
	void SetHeapFailure(const RMessage2& aMessage);
#endif	
private:
	RPointerArray<CUpdateReceiverNotificationBatch> iUpdateReceiverNotificationBatches;
	CUpdateReceiverNotificationBatch* iAvailable;	
	CUpdateReceiverNotificationBatch* iDisplayed;	
	CUpdateReceiverNotificationBatch* iDisplayedXTimes;
	const RPointerArray<TUpdateReceiverPriorityEntry>& iUpdateReceiverEntryList; //doesn't own
	enum 
		{
		EUpdateMethodNone,
		EUpdateMethodPerScreen,
		EUpdateMethodGlobal,
		} iUpdateMethod;
	};

inline const RPointerArray<TUpdateReceiverPriorityEntry>& CSurfaceUpdateServer::UpdateReceiverPriority() const
	{
	return iUpdateReceiverPriorityOrder;
	}

#endif
