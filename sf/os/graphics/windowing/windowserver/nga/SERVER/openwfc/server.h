// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Window server C++ Class definitions
// 
//

#ifndef __SERVER_H__
#define __SERVER_H__

#include <e32std.h>
#include "W32STD.H"
#include "w32cmd.h"
#include <bitstd.h>
#include <bitdev.h>
#include <e32svr.h>
#include "OBJECT.H"
#include "CLIENT.H"
#include "ScrDev.H"
#include <graphics/wskeyrouter.h>
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "graphics/wsgraphicdrawerinternal.h"
#include "WSGRAPHICDRAWERARRAY.H"
#include "panics.h"

class CWsGc;
class CWsDirectScreenAccess;
class CWsAnim;

const TUid KDllUid={0x10000079};

const TUint KMaxNumberOfMsgsInQueue=20;
const TUint KMaxNumberOfMsgsInInactiveQueue=2;

enum TWservActiveObjectPriorities
	{
	EWsGraphicAnimateAwaitIdlePriority=0, // lowest of all, when the system is really idle
	EWsGraphicAnimatePriority=50,		// used by the default animation scheduler; custom schedulers might use other priorities as they see fit
	EComposeMergePriority=80,			// Merges normal priority screen composition requests
	EMainServerPriority=100,
	ESpriteAnimatePriority=900,			// Below normal heartbeat
	EPointerCursorPriority=950,
	EHeartBeatPriority=1000,
	EKeyRepeatPriority=1980,
	EPointerRepeatPriority=1980,// Same as for key repeats
	EEventPriority=2000,
	EComposeCompletePriority=2500,		// Triggers further composition if necessary
	EWsShellLogonPriority=3000,	// Log on to the shell dying
	EDirectTimeOut=4000,
	EDirectAbort=4010,
	};

enum {ETidyCountSetting=8};

class CWsClient;
class DWsScreenDevice;

GLREF_C RWsRegion *GetRegionFromClientL(CWsClient *aClient, TInt aCount);

//---------------------------
// The event queue
//---------------------------

#define WS_EVENT_QUEUE_SIZE 50

// Forward references

class CWsRedrawMsgWindow;
class CWsWindowRedraw;
class CWsWindow;
class CWsWindowBase;
class CWsWindowGroup;
class CWsClientWindow;
class CWsTopClientWindow;
class CWsClient;
class CWsBackedUpWindow;
class CWsPointerCursor;
class TWindowServerEvent;
class CWsRootWindow;

enum TFlashState {EFlashOn,EFlashOff};

struct TPointerKeyList
	{
	TRect iRect;
	TInt iScanCode;
	TBool iActivatedByPointerSwitchOn;
	TDblQueLink iQue;
	};

struct TWsMessage
	{
	TUid iUid;
	TUint8 iTheRest[4];	// Dummy elemnt where a variable length component will be
	};

class WsKeyboardEmulator
	{
public:
	static TBool PointerEvent(TPointerEvent::TType type,const TPoint &aPoint, TDblQue<TPointerKeyList> *iPointerKeyList);
private:
	static void RawKeyEvent(TRawEvent::TType aType);
private: // Private data
	static TBool iKeyIsDown;
	static TInt iCurrentKeyDown;
	};

struct TPointerEventInternal
	{
	TPointerEvent::TType iType;		// Type of pointer event
	TUint iModifiers;	// State of pointing device and associated buttons
	TPoint iPosition; 	// Screen co-ordinates of mouse event
	};

//------------------------------
// Client object
//------------------------------

class CWsCliObj : public CWsObject
	{
public:
	static CWsCliObj* NewL(CWsClient *aOwner);
private:
	CWsCliObj(CWsClient *aOwner);
	void ConstructL();
public: //from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	};


//---------------------------
// Capture Key
//---------------------------
class CWsCaptureKey : public CWsObject
	{
public:
	CWsCaptureKey(CWsWindowGroup *owner);
	~CWsCaptureKey();
	void ConstructL(const TWsWinCmdCaptureKey &aCaptureKey);
	void SetL(const TWsWinCmdCaptureKey &aCaptureKey);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	inline CWsWindowGroup *WindowGroup();
	inline const CWsWindowGroup *WindowGroup() const;
private:
	void CmdToRequest(const TWsWinCmdCaptureKey &aCaptureKey, TKeyCaptureRequest &aParams);
private:
	CWsWindowGroup *iWindowGroup;
	};

class CWsHotKey : public CBase
	{
public:
	CWsHotKey(TInt aHotKey, TBool aIsDefault);
	~CWsHotKey();
	void ConstructLD(const TWsWinCmdCaptureKey &aCaptureKey);
	inline TInt HotKeyType() const;
	inline TInt KeyHandle() const;
	inline TBool IsDefault() const;
	inline void SetLink(CWsHotKey *aLink);
	void SetL(const TWsWinCmdCaptureKey &aCaptureKey);
public:
	CWsHotKey *iNext;
private:
	TInt iHotKeyType;
	TBool iIsDefault;
	CWsCaptureKey *iCaptureKey;
	};

class CWsCaptureKeyUpsAndDowns : public CWsObject
	{
public:
	CWsCaptureKeyUpsAndDowns(CWsWindowGroup *owner);
	~CWsCaptureKeyUpsAndDowns();
	void ConstructL(const TWsWinCmdCaptureKey &aCaptureKey);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	inline CWsWindowGroup *WindowGroup();
	inline const CWsWindowGroup *WindowGroup() const;
private:
	CWsWindowGroup *iWindowGroup;
	};

class CWsCaptureLongKey : public CWsObject
	{
	friend class CKeyboardRepeat;
	friend class TWindowServerEvent;
public:
	CWsCaptureLongKey(CWsWindowGroup *owner);
	~CWsCaptureLongKey();
	void ConstructL(const TWsWinCmdCaptureLongKey &aCaptureKey);
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	inline CWsWindowGroup *WindowGroup();
	inline const CWsWindowGroup *WindowGroup() const;
private:
	TTimeIntervalMicroSeconds32 iDelay;
	TUint iFlags;
	CWsWindowGroup *iWindowGroup;
	};

//--------------------------------
// Owner of CWsGraphicDrawer
//--------------------------------

class CWsGraphicDrawerObject: public CWsObject
	{
public:
	static CWsGraphicDrawerObject* NewL(CWsClient* aClient,const TWsClCmdUnion &aParams);
	~CWsGraphicDrawerObject();
	// from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	// access
	CWsGraphicDrawer* Drawer();
	const CWsGraphicDrawer* Drawer() const;
	inline TInt ClientHandle() const;
private:
	CWsGraphicDrawerObject(CWsClient* aOwner);
	void ConstructL(TUid aType,MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,const TDesC8& aData,TInt aClientHandle);
private:
	static TInt TransientSequence;
	static TBool RollOver;
	CWsGraphicDrawer* iDrawer;
	TInt iClientHandle;
	};

//----------------------------
// Window's
//----------------------------
enum TWalkMode
	{
	EWalkBehind,
	EWalkChildren,
	EWalkChildrenAndBehind
	};

enum TWalkModeBackToFront
	{
	EVisitParentNodesFirst, //Useful for re-sending the entire window tree.
	};

enum TWindowFlags
	{
	EFlagHidden=0x1,			// True if the window is invisble for any reason
	EFlagPointerCaptured=0x2,
	EFlagPointerGrab=0x4,
	EFlagInvisible=0x8,			// True if the client has requested the window to be invisible
	EFlagActive=0x10,			// True if the window has been activated
	EFlagValidRedraw=0x20,      // True if a Begin/End redraw was called with GC commands. An empty Begin/End redraw is used to trigger redraw loop without actually drawing to the window with GC.
//	EFlagHasGcDrawing=0x40,    // True if a GC has drawn on a window.
	EFlagDrawnToScreen=0x80,    // True if the window has been drawn to the screen since it became visible
	//0x100
	EFlagPointerCaptureDragDrop=0x200,	// True if pointure capture in Drag/Drop mode
	EFlagPointerCaptureAllGroups=0x400,	// True if pointer capture affects all groups
	EFlagUsingPointerBuffer=0x800,		// True if using a pointer event buffer
	EFlagHasPointerBuffer=0x1000,		// True if has pointer event buffer allocated
	//0x2000
	EFlagFadeBehind=0x4000,             // True if fade behind requested for this window
	EFlagIsTopClientWindow=0x8000,	// True if is a top level client window
	EFlagNonFadingWindow=0x00010000,		//True if the window should never be faded
	EFlagScreenDeviceInvalid=0x00020000,
	EFlagShutDownInProgress=0x00040000,		// True if the window is currently being shut down
#ifdef SYMBIAN_GRAPHICS_WSERV_QT_EFFECTS
	EFlagHasTransparentSurface=0x00080000,
	EFlagPaintTransparentHoles=0x00100000,
#endif
	//0x00200000,
	//0x00400000,
	//0x00800000,
	EFlagGeneratesVisibilityEvents=0x02000000,	// True if visibility events are enabled for this window
	EFlagCanBeSeen=0x04000000,					// True if the last visibility event indicated that part of the window was visible
//	EFlagTransparencyAlphaChannel=0x08000000, // True if the window is transparent with an alpha channel (and no transparency mask)
	EFlagNotCantBeSeen=0x10000000,					// True if the last visibility event did not indicate that part of the window was not visible
//	EFlagTransparencyPolicyFreezeUnder=0x20000000,	// True if the transparent window enables freezing under policy
//	EFlagChildClippedRegionValid=0x40000000,  // True if the ChildClippedRegion is valid
	EFlagHasAlpha=            0x80000000,  // Equivalent to EFlagTransperencyAlphaChannel - new name to prevent confusion
	};

//
// Sprites
//

class TWsDeltaTimerEntry
	{
	friend class CWsDeltaTimer;
public:
	inline TWsDeltaTimerEntry(TCallBack& aCallback);
	inline TWsDeltaTimerEntry();
	inline void Set(TCallBack& aCallback);
	inline TBool IsPending();
private:
	TCallBack iCallBack; 
	TDeltaQueLink iLink;
	};
class CWsDeltaTimer : public CActive
	{
	enum {CWsDeltaTimerGranularity=100000};
public:
	static CWsDeltaTimer* NewL(TInt aPriority);
	void Queue(TTimeIntervalMicroSeconds32 aTimeInMicroSeconds,TWsDeltaTimerEntry& aHandle);
	void Remove(TWsDeltaTimerEntry& anEntry);
	void Activate();
	~CWsDeltaTimer();
private:
	CWsDeltaTimer(TInt aPriority);
	void DoCancel();
	void RunL();
protected:
	RTimer iTimer;
	TDeltaQue<TWsDeltaTimerEntry> iQueue;
	};

//---------------------------
// Top level command class
//---------------------------

/** The main server class.

@internalComponent
@released
*/
class CWindowServer : public CBase, public MWsGraphicDrawerEnvironment, public MWsMemoryRelease
	{
public:
	static CWindowServer *NewL();
	~CWindowServer();
	void StartL();
	TInt SessionCount();
	MWsAnimationScheduler* AnimationScheduler();
	void PrepareShutdown();
	TBool ReleaseMemory();
	void DestroySessionsForShutdown();
	void SetPinClientDescriptors(TBool aPin);
public:	// from MWsGraphicDrawerEnvironment
	const CWsGraphicDrawer* ResolveGraphic(const TGraphicDrawerId& aId) const;
	void Invalidate(const TGraphicDrawerId& aId);
	TInt ScreenCount() const;
	MWsScreen* Screen(TInt aIndex);
	const MWsScreen* Screen(TInt aIndex) const;
	TBool SetCustomAnimationScheduler(MWsAnimationScheduler* aScheduler);
	TBool HasCustomAnimationScheduler() const;
	TBool ClearCustomAnimationScheduler(MWsAnimationScheduler* aCurrentScheduler);
	TInt RegisterEventHandler(CWsGraphicDrawer* aDrawer, MWsEventHandler* aHandler, TUint32 aEventMask);
	TInt UnregisterEventHandler(CWsGraphicDrawer* aDrawer);
	TInt RegisterWsEventHandler(MWsEventHandler* aHandler, TUint32 aEventMask);
	TInt UnregisterWsEventHandler(MWsEventHandler* aHandler);
	void Log(TInt aPriority,const TDesC &aFmt,TInt aParam=0);
	TInt RegisterMemoryRelease(MWsMemoryRelease * aMemoryRelease);
	void UnregisterMemoryRelease(MWsMemoryRelease * aMemoryRelease);

public:
	// CWsGraphicDrawer master index
	TInt AddGraphicDrawer(CWsGraphicDrawer* aDrawer);
	TInt SwapGraphicDrawer(CWsGraphicDrawer* aDrawer);
	TInt RemoveGraphicDrawer(const TGraphicDrawerId& aId);
	TInt RemoveAllGraphicDrawers(const MWsClient& aOwner);
public: // from MWsMemoryRelease
	TBool ReleaseMemory(TMemoryReleaseLevel aLevel);
private:
	CWindowServer();
	void ConstructL();
	TAny* ResolveObjectInterface(TUint aTypeId);
private:
	class CServer;
	class CDefaultAnimationScheduler;
	CServer* iServer;
	CDefaultAnimationScheduler* iDefaultAnimationScheduler;
	MWsAnimationScheduler* iCustomAnimationScheduler;
	RPointerArray<MWsMemoryRelease> iMemoryReleases;
	/** An index to all the CWsGraphicDrawer objects.
	
	All the CWsGraphicDrawer objects are created and owned by the CWsClient objects but they are
	indexed centrally because some operations require access to the full list.
	*/
	CWsGraphicDrawerArray iDrawerMasterIndex;
	};

//
inline CWsWindowGroup *CWsCaptureKey::WindowGroup()
	{return(iWindowGroup);}
inline const CWsWindowGroup *CWsCaptureKey::WindowGroup() const
	{return(iWindowGroup);}
//
inline CWsWindowGroup *CWsCaptureKeyUpsAndDowns::WindowGroup()
	{return(iWindowGroup);}
inline const CWsWindowGroup *CWsCaptureKeyUpsAndDowns::WindowGroup() const
	{return(iWindowGroup);}
//
inline CWsWindowGroup *CWsCaptureLongKey::WindowGroup()
	{return(iWindowGroup);}
inline const CWsWindowGroup *CWsCaptureLongKey::WindowGroup() const
	{return(iWindowGroup);}
//
inline CWsPointerCursor *CWsClient::DefaultSystemPointerCursor()
	{return(iDefaultSystemPointerCursor);}
//
inline TInt CWsHotKey::HotKeyType() const
	{return(iHotKeyType);}
inline TInt CWsHotKey::KeyHandle() const
	{return((TInt)iCaptureKey);}
inline TBool CWsHotKey::IsDefault() const
	{return(iIsDefault);}
inline void CWsHotKey::SetLink(CWsHotKey *aLink)
	{iNext=aLink;}
//
inline TWsDeltaTimerEntry::TWsDeltaTimerEntry(TCallBack& aCallback)
	{iCallBack=aCallback;}
inline TWsDeltaTimerEntry::TWsDeltaTimerEntry()
	{}
inline void TWsDeltaTimerEntry::Set(TCallBack& aCallback)
	{iCallBack=aCallback;}
inline TBool TWsDeltaTimerEntry::IsPending()
	{return(iLink.iNext!=NULL);}
//
inline TInt CWsGraphicDrawerObject::ClientHandle() const
	{return iClientHandle;}

#endif
