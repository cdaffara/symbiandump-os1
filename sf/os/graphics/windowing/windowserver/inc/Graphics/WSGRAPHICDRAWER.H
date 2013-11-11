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
// Server-side base-classes for graphic drawer plugins
// 
//

#ifndef __WSGRAPHICDRAWER_H__
#define __WSGRAPHICDRAWER_H__

#include <e32base.h>
#include <e32std.h>

#include <graphics/wsgraphicdrawerinterface.h>

class CFbsBitGc;
class TSurfaceId;

NONSHARABLE_STRUCT(TGraphicDrawerId)
/** Represents a graphic drawer on the window-server side
A much-abridged version of TWsGraphicId, with trivial constructor
@publishedPartner
@released
*/	{
	TInt iId;
	TBool iIsUid;
	IMPORT_C TInt Compare(const TGraphicDrawerId& aOther) const;
	IMPORT_C static TInt Compare(const TGraphicDrawerId& aFirst,const TGraphicDrawerId& aSecond);
	};

/** Wserv event representation to plugin side

@publishedPartner
@released
*/
NONSHARABLE_STRUCT(TWservCrEvent)
	{
public:
	/** Type of wserv events which plugin can listen to. It is a mask that can be combined
	when subscribing to notification. */
	enum
		{
		EScreenSizeModeChanged		= 0x00000001,
		EWindowVisibilityChanged 	= 0x00000002,
		EDsaDrawingBegin			= 0x00000004,
		EDsaDrawingEnd				= 0x00000008,
		EScreenSizeModeAboutToChange= 0x00000010,
		EScreenUpdated				= 0x00000020,
		EScreenDrawing				= 0x00000040,
		EWindowGroupChanged			= 0x00000080,
		EScreenOrientationChanged	= 0x00000100,
		EDeviceOrientationChanged	= 0x00000200,
		EWindowClosing              = 0x00000400,
		ESurfaceUnreferenced		= 0x00000800,
		};
public:
	IMPORT_C TWservCrEvent(TUint32 aType);
	IMPORT_C TWservCrEvent(TUint32 aType, TUint32 aInfo);
	IMPORT_C TWservCrEvent(TUint32 aType, TUint32 aInfo, TAny* aData);
	IMPORT_C TWservCrEvent(TUint32 aType, TUint32 aInfo, TAny* aData, MWsWindow* aWindow);

	/** Returns wserv event type */
	IMPORT_C TUint32 Type() const;
	/** Returns current wserv screen size mode as the result of event EScreenSizeModeChanged.
	Screen mode is 0,1,2,...
	*/
	IMPORT_C TInt SizeMode() const;
	/** Returns visibile region as the result of event EWindowVisibilityChanged, this region represents
	full or some part of window which is changing visibility. NULL if window is becoming not visible
	completely.
	*/
	IMPORT_C const RRegion* VisibleRegion() const;
	/** Returns screen number where a DSA, screen updated or window group changed event occurs
	*/
	IMPORT_C TInt ScreenNumber() const;
	/** Returns the drawing region which is referred to by an EScreenDrawing
	*/
	IMPORT_C const TRegion* DrawingRegion() const;
	/** Returns the new window group identifier where a window group changed event occurs
	*/
	IMPORT_C TInt WindowGroupIdentifier() const;
	/** Returns the new window server display oriention
	*/
	IMPORT_C CFbsBitGc::TGraphicsOrientation Orientation() const;
	/** Returns whether or not a window was already visible before a visibility event occurred
	*/
	IMPORT_C TBool WasVisible() const;
	/** Returns the window this event is for.  This may be null.
	@prototype
	*/
	IMPORT_C MWsWindow* Window() const;	
	/** Returns the released surface ID
	*/
	IMPORT_C const TSurfaceId* SurfaceId() const;

private:
	TWservCrEvent() {}
	
private:
	TUint32 iType;
	TUint32 iInfo;
	TAny* iData;
	MWsWindow* iWindow;
	TInt iReserved[7];
	};

/** Event notification callback. Need to be implemented to allow CWsGraphicDrawer to listen to
wserv events.

@publishedPartner
@released
*/
class MWsEventHandler
	{
public:
	/** Plugin event handler, will be called once for each event.
	*/
	virtual void DoHandleEvent(const TWservCrEvent& aEvent) = 0;
	};

/**
Implementing this interface will give a callback notification wheater a
graphic message which was sent has either failed or succeded.
@publishedPartner
@prototype
*/
class MWsGraphicMessageCallback : public MWsObjectProvider
 	{
public:
 	DECLARE_WS_TYPE_ID(KWsGraphicMessageCallbackInterfaceId)

public:
 	/**
 	Called with unique ID of the message, aError will be set to KErrNone if message delivery is
 	successful, otherwise one of the system-wide error code.
 	*/
 	virtual void HandleMessageDelivery(TInt aMessageId, TInt aError) = 0;
 	};

/**
Implementing this interface will give you the possibility to send synchron messages with a returnvalue to the client.
@publishedPartner
@prototype
*/
class MWsGraphicHandleSynchronMessage: public MWsObjectProvider
 	{
public:
 	DECLARE_WS_TYPE_ID(KWsGraphicHandleSynchronMessageId)

public:
 	/**
	Synchron handlemessage method.
	*/
	virtual TInt HandleSynchronMessage(const TDesC8& aData) = 0;
	};


class CWsGraphicDrawer: public CBase, public MWsObjectProvider
/** A window-server-side peer to a CWsGraphic
@publishedPartner
@released
*/	{
public:
	/** This function should be overriden by all derived classes.  The first call the implementation of this function
		should make is to BaseConstructL().
		@param aEnv the environment this drawer exists in
		@param aId the ID of this drawer
		@param aOwner the client session that owns this drawer
		@param aData arbitrary data for constructing this instance, sent from the client.
	*/
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData) = 0;
	IMPORT_C ~CWsGraphicDrawer();
	IMPORT_C const TGraphicDrawerId& Id() const;
	IMPORT_C const MWsClient& Owner() const;
	IMPORT_C TBool IsSharedWith(TSecureId aClientId) const;
	IMPORT_C TInt ShareGlobally();
	IMPORT_C TInt UnShareGlobally();
	IMPORT_C TInt Share(TSecureId aClientId);
	IMPORT_C TInt UnShare(TSecureId aClientId);
	IMPORT_C void Draw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
	IMPORT_C TBool Contains(const TArray<TGraphicDrawerId>& aIds) const;
	virtual void HandleMessage(const TDesC8& aData) = 0;
	IMPORT_C void HandleEvent(const TWservCrEvent& aEvent);
	IMPORT_C void SetEventHandler(MWsEventHandler* aHandler);
	IMPORT_C TBool HasEventHandler() const;
protected:
	IMPORT_C CWsGraphicDrawer();
	IMPORT_C void BaseConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner);
	IMPORT_C MWsGraphicDrawerEnvironment& Env();
	IMPORT_C const MWsGraphicDrawerEnvironment& Env() const;
	IMPORT_C TInt SendMessage(const TDesC8& aData);
	IMPORT_C TInt SendMessage(CWsMessageData& aData);
	IMPORT_C void Invalidate();
private:
	IMPORT_C virtual TBool HasAsChild(const TArray<TGraphicDrawerId>& aIds) const;
	virtual void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const = 0;
private:
	class CPimpl;
	friend class CPimpl;
	CPimpl* iPimpl;
	TInt iWsGraphicDrawerSpare[3];
private:
	friend class WsGraphicDrawer;
	TUid iDtor_ID_Key;
	};


#endif //#ifndef __WSGRAPHICDRAWER_H__
