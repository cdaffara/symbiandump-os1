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
// WSGRAPHIC.CPP
// The client-side representation of a WsGraphic artwork
// 
//

#include "../SERVER/w32cmd.h"
#include "Graphics/WSGRAPHICDRAWER.H"
#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>
#include <s32mem.h>
#include "panics.h"

GLDEF_C void Panic(TWsGraphicDrawerPanic aPanic)
	{
	_LIT(KCategory,"WsGraphicDrawer");
	User::Panic(KCategory,aPanic);
	}

//
// TWservCrEvent implementation
//
EXPORT_C TWservCrEvent::TWservCrEvent(TUint32 aType): iType(aType), iInfo(0), iData(NULL), iWindow(NULL)
	{
	}

EXPORT_C TWservCrEvent::TWservCrEvent(TUint32 aType, TUint32 aInfo): iType(aType), iInfo(aInfo), iData(NULL), iWindow(NULL)
	{
	}

EXPORT_C TWservCrEvent::TWservCrEvent(TUint32 aType, TUint32 aInfo, TAny* aData): iType(aType), iInfo(aInfo), iData(aData), iWindow(NULL)
	{
	}

EXPORT_C TWservCrEvent::TWservCrEvent(TUint32 aType, TUint32 aInfo, TAny* aData, MWsWindow* aWindow): iType(aType), iInfo(aInfo), iData(aData), iWindow(aWindow)
	{
	}

EXPORT_C TUint32 TWservCrEvent::Type() const
	{
	return iType;
	}

EXPORT_C TInt TWservCrEvent::SizeMode() const
	{
	return iInfo;
	}

EXPORT_C const RRegion* TWservCrEvent::VisibleRegion() const
	{
	return reinterpret_cast<RRegion*>(iData);
	}

EXPORT_C TInt TWservCrEvent::ScreenNumber() const
	{
	return iInfo;	
	}

EXPORT_C CFbsBitGc::TGraphicsOrientation TWservCrEvent::Orientation() const
	{
	return *reinterpret_cast<CFbsBitGc::TGraphicsOrientation*>(iData);
	}

EXPORT_C const TRegion* TWservCrEvent::DrawingRegion() const
	{
	return reinterpret_cast<TRegion*>(iData);
	}

EXPORT_C TInt TWservCrEvent::WindowGroupIdentifier() const
	{
	return reinterpret_cast<TInt>(iData);
	}

//This is NGA-specific, but does not rely on any NGA-compiled code
EXPORT_C const TSurfaceId* TWservCrEvent::SurfaceId() const
	{
	return reinterpret_cast<const TSurfaceId*>(iData);
	}

EXPORT_C TBool TWservCrEvent::WasVisible() const
	{
	return iInfo;
	}

EXPORT_C MWsWindow * TWservCrEvent::Window() const
	{
	return iWindow;
	}

// CWsGraphicDrawer::CPimpl \\\\\\\\\\\\\\\\\\\\\\\\

NONSHARABLE_STRUCT(CWsGraphicDrawer::CPimpl): public CBase
/** @internalComponent
	@released
*/	{
	enum
		{
		EActive			= 0x01,
		EIsPublic		= 0x02,
		EInContains		= 0x04,
		EInDraw			= 0x08,
		EDrawn			= 0x10,
		};
	CPimpl(CWsGraphicDrawer& aGraphic,MWsGraphicDrawerEnvironment& aEnv,MWsClient& aOwner);
	~CPimpl();
	CWsGraphicDrawer& iGraphic;
	MWsGraphicDrawerEnvironment& iEnv;
	MWsClient& iOwner;
	TGraphicDrawerId iId;
	RArray<TUint> iSharedWith;
	TUint iFlags;
	MWsEventHandler* iEventHandler;
	};

CWsGraphicDrawer::CPimpl::CPimpl(CWsGraphicDrawer& aGraphic,MWsGraphicDrawerEnvironment& aEnv,MWsClient& aOwner):
	iGraphic(aGraphic), iEnv(aEnv), iOwner(aOwner)
	{
	}

CWsGraphicDrawer::CPimpl::~CPimpl()
	{
	iSharedWith.Close();
	}

// CWsGraphicDrawer \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C CWsGraphicDrawer::CWsGraphicDrawer()
/** Constructor
*/	{
	}

EXPORT_C CWsGraphicDrawer::~CWsGraphicDrawer()
/** Destructor
*/	{
	delete iPimpl;

	if(KNullUid != iDtor_ID_Key)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		}
	}

EXPORT_C void CWsGraphicDrawer::BaseConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner)
/** Completes construction of the baseclass. Derived drawers should call this from their ConstructL() implementations
	@param aEnv the environment this drawer exists in
	@param aId the ID of this drawer
	@param aOwner the client session that owns this drawer
	@param aData arbitrary data for constructing this instance, sent from the client.
*/	{
	iPimpl = new(ELeave) CPimpl(*this,aEnv,aOwner);
	iPimpl->iId = aId;
	}

EXPORT_C MWsGraphicDrawerEnvironment& CWsGraphicDrawer::Env()
/** The environment this drawer exists in
	@return the environment
*/	{
	return iPimpl->iEnv;
	}

EXPORT_C const MWsGraphicDrawerEnvironment& CWsGraphicDrawer::Env() const
/** The environment this drawer exists in
	@return the environment
*/	{
	return iPimpl->iEnv;
	}

EXPORT_C const TGraphicDrawerId& CWsGraphicDrawer::Id() const
/** The ID of this graphic
	@return the ID
*/	{
	return iPimpl->iId;
	}

EXPORT_C const MWsClient& CWsGraphicDrawer::Owner() const
/** Only the owner can delete, replace or send messages to this graphic.
	The owner and any session sharing the owner's Secure ID can always
	draw the graphic.
	@return the Client session which created and owns this graphic.
*/	{
	return iPimpl->iOwner;
	}

EXPORT_C TBool CWsGraphicDrawer::IsSharedWith(TSecureId aClientId) const
/** tests whether the client is allowed to draw this graphic
	Conditions include whether the client is the owner of this graphic,
	whether the graphic is shared globally, or whether this graphic
	has been explicitly shared with the client
	@param aClientId the Secure ID of the client session's process
	@return ETrue if the client can draw this graphic
*/	{
	return ((iPimpl->iFlags & CPimpl::EIsPublic) ||
		(0 <= iPimpl->iSharedWith.FindInOrder(aClientId.iId)) ||
		(aClientId == iPimpl->iOwner.SecureId()));
	}

EXPORT_C TInt CWsGraphicDrawer::Share(TSecureId aClientId)
/** Explicitly shares this graphic with client sessions with the specified Secure ID
	@param aClientId the Secure ID of the client sessions to share with
	@return KErrNone if the graphic was shared, else one of the system-wide error codes
*/	{
	return iPimpl->iSharedWith.InsertInOrder(aClientId.iId);
	}

EXPORT_C TInt CWsGraphicDrawer::ShareGlobally()
/** Share this drawer with all client sessions
	Sharing globally trumps explicit shares
	@return KErrNone if the graphic is globally shared, else one of the system-wide error codes
*/	{
	iPimpl->iFlags |= CPimpl::EIsPublic;
	return KErrNone;
	}

EXPORT_C TInt CWsGraphicDrawer::UnShareGlobally()
/** Stop this drawer from being shared with all client sessions
	A drawer that isn't shared explicitly is only available to clients it
	has been explicitly shared with using Share()
	@return KErrNone if the graphic is not globally shared, else one of the system-wide error codes
*/	{
	iPimpl->iFlags &= ~CPimpl::EIsPublic;
	return KErrNone;
	}

EXPORT_C TInt CWsGraphicDrawer::UnShare(TSecureId aClientId)
/** Stop this drawer from being shared with all client sessions with the specific Secure ID
	ShareGlobally() trumps explicit sharing
	@param aClientId the Secure ID of the client sessions to not share with
	@return KErrNone if the graphic is no longer shared, KErrNotFound if the graphic was not shared anyway, else one of the system-wide error codes
*/	{
	const TInt idx = iPimpl->iSharedWith.FindInOrder(aClientId.iId);
	if(0 <= idx)
		{
		iPimpl->iSharedWith.Remove(idx);
		return KErrNone;
		}
	else
		{
		return idx;
		}
	}

EXPORT_C void CWsGraphicDrawer::Draw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const
	{
	// avoid infinite recursion
	if(iPimpl->iFlags & CPimpl::EInDraw)
		{
		return;
		}
	// draw
	iPimpl->iFlags |= (CPimpl::EInDraw | CPimpl::EDrawn);
	DoDraw(aGc,aRect,aData); // implemented by derived classes
	iPimpl->iFlags &= ~CPimpl::EInDraw;
	}


EXPORT_C TInt CWsGraphicDrawer::SendMessage(const TDesC8& aData)
/** Sends this message to the client-side peer
	@param aData the data to send
	@return KErrNone if successful, else one of the System-wide error codes
*/	{
	return iPimpl->iOwner.SendMessage(this,aData);
	}

EXPORT_C TInt CWsGraphicDrawer::SendMessage(CWsMessageData& aData)
/** Sends this message to the client-side peer
	@param aData the data to send
	@return KErrNone if successful, else one of the System-wide error codes
*/	{
	return iPimpl->iOwner.SendMessage(this,aData);
	}

EXPORT_C void CWsGraphicDrawer::Invalidate()
	/** Schedules all potentially affected parts of the screen to repaint
	Called by derived classes when their presentation changes (e.g. they
	receive new artwork via a SendMessage())
	Note: this is not a suitable method for animating a graphic.  For animation,
	use MWsGc::Invalidate when the graphic is being drawn to schedule the next
	animation frame instead.
*/	{
	if(iPimpl && (iPimpl->iFlags & CPimpl::EDrawn))
		{
		iPimpl->iEnv.Invalidate(iPimpl->iId);
		iPimpl->iFlags &= ~CPimpl::EDrawn;
		}
	}

EXPORT_C TBool CWsGraphicDrawer::Contains(const TArray<TGraphicDrawerId>& aIds) const
/** Tests whether this drawer is, or itself draws, any of the graphic drawers identified by the IDs.
	Calls HasAsChild() on itself to determine if this graphic does itself contain
	any of the IDs.  Derived classes which themselves draw other graphic drawers should
	override HasAsChild().
	@param aIds the (sorted in TGraphicDrawerId::Compare order) list of IDs
	@return ETrue if this graphic is, or contains, any of the graphic drawers listed
*/	{
	// avoid infinite recursion
	if(iPimpl->iFlags & CPimpl::EInContains)
		{
		return EFalse;
		}
	// is it us?
	const TInt count = aIds.Count();
	for(TInt i=0; i<count; i++)
		{
		if(0 == aIds[i].Compare(iPimpl->iId))
			{
			return ETrue;
			}
		}
	// is it a child of us?
	iPimpl->iFlags |= CPimpl::EInContains;
	const TBool ret = HasAsChild(aIds);
	iPimpl->iFlags &= ~CPimpl::EInContains;
	return ret;
	}

EXPORT_C TBool CWsGraphicDrawer::HasAsChild(const TArray<TGraphicDrawerId>& /*aIds*/) const
/** Called by Contains() to determine if the graphic identified by Id is contained within the
	other graphic drawers that this graphic drawer itself draws.
	Derived classes which draw other drawers should override this method and call
	Contains() on all graphics which they draw.
	@param aIds a (sorted in TGraphicDrawerId::Compare order) list of the drawer IDs to see if this drawer draws
	@return ETrue if this drawer draws any of those graphics
*/	{
	return EFalse;
	}

EXPORT_C void CWsGraphicDrawer::HandleEvent(const TWservCrEvent& aEvent)
	{
	if (iPimpl->iEventHandler)
		iPimpl->iEventHandler->DoHandleEvent(aEvent);
	}

EXPORT_C void CWsGraphicDrawer::SetEventHandler(MWsEventHandler* aHandler)
	{
	iPimpl->iEventHandler = aHandler;
	}

EXPORT_C TBool CWsGraphicDrawer::HasEventHandler() const
	{
	return iPimpl->iEventHandler!=NULL;
	}
