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
// WSGRAPHICDRAWERINTEFACE.H
// Server-side base-classes for graphic drawer plugins
// 
//

#ifndef __WSGRAPHICDRAWERINTEFACE_H__
#define __WSGRAPHICDRAWERINTEFACE_H__

#include <e32base.h>
#include <w32std.h>
#include <gdi.h>
#include <bitstd.h>

class CWsGraphicDrawer;
struct TGraphicDrawerId;
class MWsEventHandler;
class CWsGraphicMessageQueue;
class CWsClient;
class CWsWindow;

#if !defined(SYMBIAN_GRAPHICS_GCE)
	class MWsFlickerFreeBufferObserver;
#endif

class MWsScreenRedrawObserver;
class MWsMemoryRelease;
class MEventHandler;
class MWsElement;
class MWsGraphicsContext;
class MWsAnimationScheduler;

/** 
@publishedPartner
@released
*/
enum TWinType
	{
	EWinTypeClient,
	EWinTypeRoot,
	EWinTypeGroup,
	};

/** Declares an object type, ETypeId, for a class, in order to allow the WSERV object
provider mechanism to locate and provide objects from the class.
@publishedPartner
@released
@see MWsObjectProvider */
#define DECLARE_WS_TYPE_ID(id) enum { EWsObjectInterfaceId = id };

class MWsObjectProvider
/** A class for dynamic extension of object instances
	WSERV classes which wish to allow dynamic extension show derive those
	extensions from MWsObjectProvider and use the DECLARE_WS_TYPE_ID macro
	Similiar in principle and operation to CONE's MObjectProvider
	@publishedPartner
	@released
	@see MObjectProvider
*/	{
public:
	template<class T>
	T* ObjectInterface()
	/** Gets an object of the type defined by the template parameter.

	@return A pointer to an object of the type required, or NULL if none can be
	found. */
		{ return (T*)ResolveObjectInterface(T::EWsObjectInterfaceId); }
	/** Gets an object of the type defined by the template parameter.
	@return A pointer to a const object of the type required, or NULL if none can be
	found. */
	template<class T>
	const T* ObjectInterface() const
		{ return (T*)const_cast<MWsObjectProvider*>(this)->ResolveObjectInterface(T::EWsObjectInterfaceId); }
	/** Resolve an instance of an interface
	should be overriden by implementations when they have custom interfaces to provide. */
	IMPORT_C virtual TAny* ResolveObjectInterface(TUint aTypeId);
	};

class MWsScreen: public MWsObjectProvider
/** A destination for a drawing occasion of a CWsGraphicDrawer
	Used by animation schedulers to update screens
	@publishedPartner
	@released
*/	{
public:
	using MWsObjectProvider::ResolveObjectInterface;
	
private:
	friend class MWsAnimationScheduler;
#if !defined(SYMBIAN_GRAPHICS_GCE)
	/** Called by the MWsAnimationScheduler when scheduled animation is to be serviced
		
		WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

		@released */
	virtual void OnAnimation() = 0;
#else
	/** Called by the MWsAnimationScheduler when a scheduled animation is to be serviced.
		@param aFinished If not NULL then this is signalled when the animation has 
		been processed and further animations may be submitted. An animation may be
		submitted prior to signalling, but the render stage pipeline may not be ready
		to process it. 
		
		WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

		@released */
	virtual void OnAnimation(TRequestStatus* aFinished) = 0;
#endif
	/** Called by the MWsAnimationScheduler when the screen needs to be redrawn in its entirity
		
		WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

		@released */
	virtual void Redraw() = 0;
	/** Called by MWsAnimationScheduler when the any parts of the screen that potentially contain
		commands to draw graphics in the list of IDs passed need to be redrawn
		@param aInvalid the list of IDs that are invalid
		@return whether the screen did any redrawing
		
		WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.

		@released */
	virtual TBool RedrawInvalid(const TArray<TGraphicDrawerId>& aInvalid) = 0;
	};

/**
@publishedPartner
@prototype
*/
class CWsMessageData : public CBase, public MWsObjectProvider
	{
public:
	virtual TPtrC8 Data() const = 0;
	virtual void Release() = 0;
	IMPORT_C TInt ClientHandle() const;
	IMPORT_C TInt Id() const;
	IMPORT_C const CWsGraphicDrawer* Drawer();
	IMPORT_C void SetClientHandle(TInt aClientHandle);
	IMPORT_C void SetId( TInt aId);
	IMPORT_C void SetDrawer(const CWsGraphicDrawer* aDrawer);
private:
	friend class CWsGraphicMessageQueue;
	friend class CWsClient;
	CWsMessageData* iNext;
	const CWsGraphicDrawer* iDrawer;
	TInt iId;
	TInt iClientHandle;
	TInt iMWsMessageDataSpare[3];
	};

class MWsClient: public MWsObjectProvider
/*** Represents a client session
	@publishedPartner 
	@released  */
	{
public:
	/** Determines if this client has the specified capability */
	virtual TBool HasCapability(TCapability aCapability) const = 0;
	/** Retrieves the Secure ID of the client's process */
	virtual TSecureId SecureId() const = 0;
	/** Retrieves the Vendor ID of the client's process */
	virtual TVendorId VendorId() const = 0;
private:
	friend class CWsGraphicDrawer;
	virtual TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf,const TDesC8& aData) = 0;
	virtual TInt SendMessage(const CWsGraphicDrawer* aOnBehalfOf,CWsMessageData& aData) = 0;
	};

class MWsGraphicDrawerEnvironment: public MWsObjectProvider
/** The environment for a graphic drawer
@publishedPartner
@released
*/	{
public:
	/** Locate a graphic by ID
	@return the graphic with that ID, else NULL if no such graphic exists */
	virtual const CWsGraphicDrawer* ResolveGraphic(const TGraphicDrawerId& aId) const = 0;
	/** The number of screens the device has; this is constant for a device */
	virtual TInt ScreenCount() const = 0;
	/** Get a particular screen by ordinal
	@return the screen, or NULL if aIndex is out of bounds */
	virtual MWsScreen* Screen(TInt aIndex) = 0;
	/** Get a particular screen by ordinal
	@return the screen, or NULL if aIndex is out of bounds */
	virtual const MWsScreen* Screen(TInt aIndex) const = 0;
	/**  Not supported
	@return success of overriding the default scheduler */
	virtual TBool SetCustomAnimationScheduler(MWsAnimationScheduler* aScheduler) = 0;
	/** Query whether a custom animation scheduler has been set
	@return ETrue if the custom animation scheduler has been set */
	virtual TBool HasCustomAnimationScheduler() const = 0;
	/** Clears a custom animation scheduler
	@return success if the custom scheduler was set, and hence removed */
	virtual TBool ClearCustomAnimationScheduler(MWsAnimationScheduler* aCurrentScheduler) = 0;
protected:
	friend class CWsGraphicDrawer;
	/** Schedules all potentially affected parts of the screen to repaint
	This is called automatically whenever a graphic is added, replaced or removed.
	However, if a graphic which changes representation during runtime must call this
	explicitly.
	Note: this is not a suitable method for animating a graphic.  For animation,
	use MWsGc::Invalidate when the graphic is being drawn to schedule the next
	animation frame instead.
	@param aId the ID of the artwork; the artwork may not exist */
	virtual void Invalidate(const TGraphicDrawerId& aId) = 0;
public:
	/** Register to be notified when some events occur.
	@return KErrNone if successful */
	virtual TInt RegisterEventHandler(CWsGraphicDrawer* aDrawer, MWsEventHandler* aHandler, TUint32 aEventMask) = 0;
	/** Unregister from events notification
	@return KErrNone if successful */
	virtual TInt UnregisterEventHandler(CWsGraphicDrawer* aDrawer) = 0;
	/** Register to be notified when some events occur.
	@return KErrNone if successful
	@prototype */
	virtual TInt RegisterWsEventHandler(MWsEventHandler* aHandler, TUint32 aEventMask) = 0;
	/** Unregister from events notification
	@return KErrNone if successful 
	@prototype */
	virtual TInt UnregisterWsEventHandler(MWsEventHandler* aHandler) = 0;
	/** Logs a message to the wserv logger, if one is loaded
	@param aPriority The priority of the message - low priority is 1, high is 9
	@param aFmt The string to log.  This may contain a single %d, %x or equivalent
	@param aParam An integer parameter corresponding to a %d, if present
	@prototype */
	virtual void Log(TInt aPriority,const TDesC &aFmt,TInt aParam=0) = 0;
	/** Registers an interface for releasing memory in low memory situations.
	The release function may be called as a result of any Alloc or ReAlloc attempts in the window server
	process.
	@param aMemoryRelease a callback interface
	@return KErrNone or a system wide error code
	@prototype */
	virtual TInt RegisterMemoryRelease(MWsMemoryRelease * aMemoryRelease) = 0;
	/** Removes a registration set by RegisterMemoryRelease
	@param aMemoryRelease the registered release object to remove
	@prototype */
	virtual void UnregisterMemoryRelease(MWsMemoryRelease * aMemoryRelease) = 0;
	};

class MWsGc: public MWsObjectProvider
/** A destination for a drawing occasion of a CWsGraphicDrawer
	Used to register invalidation schedules for animation
	@publishedPartner
	@released
*/	{
public:
	/** The Client requesting this drawing occasion
		@return the client
		@publishedPartner 
		@released  */
	virtual MWsClient& Client() = 0;
	/** The screen that is currently being drawn to
		@return the screen
		@publishedPartner 
		@released  */
	virtual MWsScreen& Screen() = 0;
#if !defined(SYMBIAN_GRAPHICS_GCE)
	/** The origin of the GC relative to the screen's origin
		@return the origin
		@publishedPartner
		@released  */
	virtual TPoint GcOrigin() const = 0;
	/** The clipping region currently being used
		@return the clipping region
		@publishedPartner
		@released */
	virtual const TRegion& ClippingRegion() = 0;
	/** The BITGDI graphics context to draw to
		@publishedPartner
		@released */
	virtual CFbsBitGc& BitGc() = 0;
	/** Saves the state of the GC to an internal buffer.  Several GC states can be saved in a FILO.
		Do not restore a GC (using PopBitGcSettings()) that wasn't properly saved!
		@return KErrNone if successful, else one of the system-wide error codes. */
	virtual TInt PushBitGcSettings() = 0;
	/** Restores the last GC state that was saved. */
	virtual void PopBitGcSettings() = 0;
#endif	
	/** The time for the current redraw
		@publishedPartner 
		@released */
	virtual const TTime& Now() const = 0;
	/** Schedule an animation of a rectangle in the future.
	    Scheduled animation doesn't work in low memory conditions or
	    if aRect values fall outside the window area of this animation.
		@param aRect the rectangle to animate, in the GC's coordinates
		@param aFromNow the time in microseconds from Now() that the rectangle will be invalid
		@publishedPartner 
		@released */
	virtual void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow) = 0;
	/** Schedule an animation of a rectangle in the future
		@param aRect the rectangle to animate, in the GC's coordinates
		@param aFromNow the time in microseconds from Now() that the rectangle will be invalid
		@param aFreq microseconds between frames that this rectangle will probably be invalid in the future (0 means no such hint is known)
		@param aStop the duration in microseconds from Now() that the aFreq hint will be valid for (0 means indefinitely)
		@publishedPartner 
		@released */
	virtual void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop) = 0;
	/** Sets the origin of gc
		@param aOrigin The new origin of the gc
		@publishedPartner 
		@prototype	*/
	virtual void SetGcOrigin(const TPoint& aOrigin) = 0;
	};

/** Interface Extension
*/
enum
	{
	KMWsScreenConfigInterfaceId = 1,
#if !defined(SYMBIAN_GRAPHICS_GCE)
	KMWsBackBufferInterfaceId = 2,
	KMWsFrontBufferInterfaceId = 3,
#endif
	KMWsGraphicDrawerMessageAlloc = 4,
	KMWsGraphicMessageAlloc = 5,
	KWsGraphicHandleSynchronMessageId = 6,
	KWsGraphicMessageCallbackInterfaceId = 7,
	KWsActiveSchedulerDebug = 8,
#if !defined(SYMBIAN_GRAPHICS_GCE)
	KWsFlickerFreeBufferObserver = 9,
#endif
	KMWsWindow = 10,
	KWsScreenRedraw = 11,
	KWsScreenRedrawObserver = 12,
	KCWsRenderStage = 13,
	KMWsRenderStageFactory = 14,
	KMWsMemoryRelease = 15,
#if !defined(SYMBIAN_GRAPHICS_GCE)	
	KMWsGcClipRect = 16,
#endif	
	KMWsFader = 17,
	KMWsPluginManager = 18,
	KMWsIniFile = 19,
	KMWsRawEventServer = 20,
	KMWsPlaceSurfaceId = 21,
#if defined(SYMBIAN_GRAPHICS_GCE)
	KMWsTextCursor = 22,				// CWsRenderStage Object Extension will yield an MWsTextCursor
#endif
	KMWsGraphicsContext = 23,			// CWsRenderStage Object Extension will yield an MWsGraphicsContext
	KMWsCompositionContext = 24,		// CWsRenderStage Object Extension will yield an MWsCompositionContext
										// 		MWsCompositionContext::CreateLayer will yield a MWsLayer
	KMWsEventHandler = 25,				// CWsRenderStage Object Extension will yield an MWsEventHandler
	KMWsScreenDevice = 26,				// CWsRenderStage Object Extension will yield an MWsScreenDevice
	KMWsPalette = 27,					// CWsRenderStage Object Extension will optionally yield an MWsPalette
#if defined(SYMBIAN_GRAPHICS_GCE)
	KMWsUiBufferInterfaceId = 28,		// CWsRenderStage Object Extension will optionally yield an MWsUiBuffer
#endif
	KMWsDebugBar = 29,					// CWsRenderStage Object Extension will optionally yield an MWsDebugBar
	KMWsDrawableSourceProvider = 30,	// CWsRenderStage Object Extension will optionally yield an MWsDrawableSourceProvider
	KMWsScreenConfigListInterfaceId = 31,	//MWsScreen can return this list interface
	KMWsWindowTreeObserver = 32,        // CWsRenderStage Object Extension will optionally yield an MWsWindowTreeObserver
	KMWsDrawAnnotationObserver = 33, 	// CWsRenderStage Object Extension will optionally yield an MWsDrawAnnotationObserver
	KMWsWindowVisibilityNotifier = 34, 	// CWsRenderStage Object Extension will optionally yield an MWsWindowVisibilityNotifier
	KMWsWindowTree = 35,				// Object Extension to be used by render stages for recovery of MWsWindowTree
	KMWsScene = 36,                     // CWsRenderStage Object Extension will yield an MWsScene
	                                    //      MWsScene::CreateSceneElement will yield a MWsElement
	KMWsContentReadyForComposition = 37,// CWsRenderStage Object Extension will optionally yield an MWsContentReadyForComposition
	};

class TSurfaceConfiguration;


/** Extension to GC to allow surface placement.

@publishedPartner
@prototype
@deprecated

NOTE: This interface is deprecated. Do not use!
*/
class MWsSurfacePlacement: public MWsObjectProvider
	{
	public:
	DECLARE_WS_TYPE_ID(KMWsPlaceSurfaceId)
	virtual TInt	PlaceSurface(const TSurfaceConfiguration& aConfig)=0;
	};

/** Current screen mode configuration.Values return from this interface shall not be cached as they
can change at anytime when wserv changes its screen mode. Plugin can listen to event EScreenSizeModeChanged
to be notified when it happens.

@publishedPartner
@released
*/
class MWsScreenConfig: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsScreenConfigInterfaceId)

	/** Get screen display mode */
	virtual TDisplayMode DisplayMode() const = 0;
	/** Get physical screen size in pixels */
	virtual TSize SizeInPixels() const = 0;
	/** Get screen size in pixels for current screen size mode as defined in wsini */
	virtual TSize ScreenModeSizeInPixels() const = 0;
	/** Get frame buffer line pitch */
	virtual TInt Stride() const = 0;
	/** Get screen orientation */
	virtual CFbsBitGc::TGraphicsOrientation Orientation() const = 0;
	/** Get screen size mode */
	virtual TInt SizeMode() const = 0;
	/** Get screen scaling factor */
	virtual TSize ScalingFactor() const = 0;
	/** Get screen origin */
	virtual TPoint Origin() const = 0;
	/** Get screen scaled origin */
	virtual TPoint ScaledOrigin() const = 0;
	};

/**	Available list of screen modes as specified during start-up.
 * 	This interface can generally be enquired from the MWsScreen instance.
 * 	Can be read by render stage to configure its Display Configuration policy.
 * 	
 * 	Note that mode list is not contiguous, so methods may leave if missing index is requested.
 **/
class MWsScreenConfigList: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsScreenConfigListInterfaceId)

	enum	TModeFlags
		{
		EDynamic=			0x00000001,		//"-1,-1"
		EHighRes=			0x00000002,		//High-res policy mode
		EDisconnected=		0x00000004,		//Mode to use when disconnected
		EHalDefault=		0x00000008,
		ETwipsSpecified=	0x00000010,
		EClientDefinedDigitiserArea=	0x00000020,	
		};
	
	/** Get  list of valid display modes - may be sparse **/
	virtual TInt	GetScreenSizeModeList(RArray<TInt>&aList) const =0;
	/** Get screen display mode */
	virtual TDisplayMode DisplayModeL(TInt aIndex) const = 0;
	/** Get screen size in pixels for current screen size mode as defined in wsini */
	virtual TSize ScreenModeSizeInPixelsL(TInt aIndex) const = 0;
	/** Get screen size in twips for current screen size mode as defined in wsini */
	virtual TSize ScreenModeSizeInTwipsL(TInt aIndex) const = 0;
	/** Get screen orientation */
	virtual CFbsBitGc::TGraphicsOrientation OrientationL(TInt aIndex) const = 0;
	/** Get screen orientation */
	virtual TInt AvailableOrientationsL(TInt aIndex) const = 0;
	/** Get screen scaling factor - note this is expected to stay at 1:1 */
	virtual TSize ScalingFactorL(TInt aIndex) const = 0;
	/** Get screen origin */
	virtual TPoint OriginL(TInt aIndex) const = 0;
	/** Get screen scaled origin - note this is expected to stay at 1:1  */
	virtual TPoint ScaledOriginL(TInt aIndex) const = 0;
	/** Get the modeflags for specified mode  */
	virtual TInt ModeFlagsL(TInt aIndex) const = 0;
	
	};
	
class MWsWindowTree : public MWsObjectProvider
	{
public:	
	DECLARE_WS_TYPE_ID(KMWsWindowTree)
	virtual void SendTree() const = 0;
	};
 
#if !defined(SYMBIAN_GRAPHICS_GCE)
/** Wserv flickerfree buffer access

@publishedPartner
@released
*/
class MWsBackBuffer: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsBackBufferInterfaceId)
	/** Get wserv flickerfree buffer bitmap
	*/
	virtual CFbsBitmap* GetBitmap() = 0;
	/** Get default wserv flickerfree buffer drawing context. This will always return the original
	context not the current context (e.g. not the redirecting context when it is redirected).
	*/
	virtual CFbsBitGc* GetBitGc() = 0;
	/** Set wserv flickerfree buffer drawing context to redirect its drawing output. Drawing context must
	have been activated on a device prior calling this function.
	@param aBitGc The context where flickerfree buffer drawing output is redirected to. If it is NULL
	the redirection will be cancelled and default drawing context will be restored.
	@return KErrNone if successful, otherwise one of system-wide error codes.
	*/
	virtual TInt SetBitGc(CFbsBitGc* aBitGc) = 0;
	/** Redirects drawing to another back buffer. Target back buffer must provide bitmap and drawing context
	which has been activated on a bitmap device.
	@param aTarget Target back buffer where drawing is redirected to. If it is NULL
	the redirection will be terminated.
	@return KErrNone if successful, otherwise one of system-wide error codes.
	@prototype
	*/
	virtual TInt RedirectTo(MWsBackBuffer* aTarget) = 0;
	
	/** Sets the back buffers observer.  The observer will be called just before and just after each
	complete sequence of drawing to the back buffer.
	@param aObserver The observer handling the callbacks
	@prototype
	*/
	virtual void SetObserver(MWsFlickerFreeBufferObserver* aObserver) = 0;
	
	/** This returns the observer set by SetObserver.
	@return The observer set by SetObserver, or NULL
	@prototype
	*/
	virtual MWsFlickerFreeBufferObserver* Observer() = 0;

	/** Get current wserv flickerfree buffer drawing context. This will return the redirected context 
	when it is set.
	@return The graphics context to use to draw to the flicker buffer directly.
	@prototype
	*/
	virtual CFbsBitGc* GetBitGcCurrent() = 0;
	};

/** Wserv screen buffer access

@publishedPartner
@released
*/
class MWsFrontBuffer: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsFrontBufferInterfaceId)
	/** Get pointer to framebuffer
	*/
	virtual const TAny* GetBits() = 0;
	/** Get default wserv screen drawing context. This will always return the original
	context not the current context (e.g. not the redirecting context when it is redirected).
	*/
	virtual CFbsBitGc* GetBitGc() = 0;
	/** Set wserv screen drawing context to redirect its drawing output. Drawing context must
	have been activated on a device prior calling this function.
	@param aBitGc The context where screen drawing output is redirected to. If it is NULL
	the redirection will be cancelled and default drawing context will be restored.
	@return KErrNone if successful, otherwise one of system-wide error codes.
	*/
	virtual TInt SetBitGc(CFbsBitGc* aBitGc) = 0;
	/** Set wserv screen drawing context to redirect its drawing output and perform fullscreen 
	redraw if necessary. Drawing context must have been activated on a device prior calling this 
	function.
	@param aBitGc The context where screen drawing output is redirected to. If it is NULL
	the redirection will be cancelled and default drawing context will be restored.
	@param aInvalidateScreen Flag to tell wserv to perform fullscreen redraw or not
	@return KErrNone if successful, otherwise one of system-wide error codes.
	@prototype
	*/
	virtual TInt SetBitGc(CFbsBitGc* aBitGc, TBool aInvalidateScreen) = 0;	
	
	/** Get current wserv screen drawing context. This will return the redirected context 
	when it is set.
	@prototype
	*/
	virtual CFbsBitGc* GetBitGcCurrent() = 0;
	};
#endif // !SYMBIAN_GRAPHICS_GCE

/** Optional Memory Allocator for incoming messages
	To be implemented by CRPs which want to control large incoming messages themselves

@publishedPartner
@released
*/
class MWsGraphicDrawerMessageAlloc: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsGraphicDrawerMessageAlloc)
	virtual TAny* Alloc(TInt aSize) = 0;
	virtual void Free(TAny* aCell) = 0;
	};

/** Optional Memory Allocator for incoming messages
	To be implemented by CWsGraphics which want to control large incoming messages themselves

@publishedPartner
@released
*/
class MWsGraphicMessageAlloc: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsGraphicMessageAlloc)
	NONSHARABLE_CLASS(MBuffer): public MWsObjectProvider
		{
	public:
		virtual TPtr8 Buffer() = 0;
		virtual void Release() = 0;
		};
	virtual MBuffer* Alloc(TInt aSize) = 0;
	};

/** For debug purposes only

@publishedPartner
@prototype
*/	
class MWsActiveSchedulerDebug: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KWsActiveSchedulerDebug)
	virtual TInt64 Requests() const = 0;
	virtual TInt64 Errors() const = 0;
	virtual TInt64 Draws() const = 0;
	virtual TInt64 Total() const = 0;
	virtual TInt64 Preparing() const = 0;
	virtual TInt64 Drawing() const = 0;
	virtual TInt64 Idle() const = 0;
	};

#if !defined(SYMBIAN_GRAPHICS_GCE)
/** This forms part of the MWsBackBuffer interface

@publishedPartner
@prototype
*/
class MWsFlickerFreeBufferObserver: public MWsObjectProvider 
	{
public:	
	DECLARE_WS_TYPE_ID(KWsFlickerFreeBufferObserver)
	/** Function that gets called before the flicker buffer is updated
	@param aBuffer The back buffer that is about to be updated
	@param aRegion The region of the back buffer that is about to be updated
	*/
	virtual void BeforeUpdate(MWsBackBuffer& aBuffer,const TRegion& aRegion) = 0;
	/** Function that gets called after the flicker buffer has been updated
	@param aBuffer The back buffer that has been updated
	@param aRegion The region of the the back buffer that has been updated
	*/
	virtual void AfterUpdate(MWsBackBuffer& aBuffer,const TRegion& aRegion) = 0;
	};
#endif // !SYMBIAN_GRAPHICS_GCE

/** An interface through which a window can be examined
This returns non const copies instead of const references, and the
functions themselves are non const.

@publishedPartner
@prototype
*/
class MWsWindow : public MWsObjectProvider
	{
public:	
	DECLARE_WS_TYPE_ID(KMWsWindow)
public:
	/** Gets the origin of the window.
	@return TPoint containing the origin of the window.
	*/
	virtual TPoint Origin() const = 0;
	/** Gets the windows rect in absolute co-ordinates.
	@return TRect
	*/
	virtual TRect AbsRect() const = 0;
	/** Gets the size of the window
	@return TSize containing the size of the window
	*/
	virtual TSize Size() const = 0;
	/** Get the window's handle
	@return TUint32 containing the window's handle.
	*/
	virtual TUint32 Handle() const = 0;
	/** Invalidate an area of the window.
	@param aRect TRect containing the area to invalidate.
	*/
	virtual void Invalidate(const TRect * aRect = 0) = 0;
	/** Get the MWsScreen for this window.
	@return A pointer to the MWsScreen that this window is on.
	*/
	virtual MWsScreen * WsScreen() const = 0;
	/** Finds a child of this window by specifiying its handle.
	@param aHandle A handle to the window to find.
	@return an MWsWindow pointer to the found window
	*/
	virtual MWsWindow * FindChildByHandle(TUint32 aHandle) = 0;
	/**
	Returns the ordinal priority of the node. 
	*/
	virtual TInt OrdinalPriority() const = 0;
	/** Get the window's basic area before any clipping is done.
	For windows with rounded corners, the window area is different from AbsRect.
	@return a reference to the window area 
	*/	
	virtual const TRegion& WindowArea() const = 0;
	};

/** 
An interface providing information about a window group.
@publishedPartner
@prototype
*/
class MWsWindowGroup
	{
public:
	virtual TInt Identifier() const = 0;
	virtual TPtrC Name() const = 0;
	virtual TBool IsFocusable() const = 0;
	virtual TInt OrdinalPriority() const = 0;
	virtual const MWsClient * Client() const = 0;
	};

/**
An interface through which a sprite can be examined. 
@publishedPartner
@prototype
*/
class MWsSprite 
	{
public:
	enum TSpriteType
		{
		EWindowSprite,
		EFloatingSprite,
		ECustomTextCursorSprite,
		EPointerCursorSprite
		};
public:
	virtual TRect Rect() const = 0;
	virtual TSpriteType SpriteType() const = 0;
	};

/**
An interface through which a standard text cursor can be examined. 
@publishedPartner
@prototype
*/
class MWsStandardTextCursor
	{
public:
	/**
	Cursor type.
	@return The cursor type; either TTextCursor::ETypeRectangle or TTextCursor::ETypeHollowRectangle.
	*/
	virtual TInt Type() const = 0;
	/**
	Draw rectangle.
	@return Draw rectangle of the cursor in <b>window</b> co-ordinates.
	*/
	virtual TRect Rect() const = 0;
	/**
	Clipping rectangle in <b>window</b> co-ordinates.
	@return If a clipping rect has been set, the cursor must be drawn clipped to the returned rectangle.
	@see MWsWindowTreeObserver::ECursorClipRectSet
	*/
	virtual TRect ClipRect() const = 0;
	/**
	Cursor flags.
	For possible values, see TTextCursor::EFlags.
	Note, only flags included by the mask TTextCursor::EUserFlags are provided.
	@see TTextCursor::EFlags
	*/
    virtual TUint Flags() const = 0;
	/**
	Cursor color.
	@return The cursor color.
	*/
	virtual TRgb Color() const = 0;
	/**
	Flash time interval.
	@return If TTextCursor::EFlagNoFlash is set, zero; otherwise the time interval between cursor flash on/off.
	@see MWsStandardTextCursor::Flags()
	*/
	virtual TTimeIntervalMicroSeconds32 FlashInterval() const = 0;
	};


/** An interface providing information about a window tree node.
@publishedPartner
@prototype
*/
class MWsWindowTreeNode
	{
public:
	
	enum TType
		{
		EWinTreeNodeClient = EWinTypeClient,	//0
		EWinTreeNodeRoot = EWinTypeRoot,		//1
		EWinTreeNodeGroup = EWinTypeGroup,		//2 
		EWinTreeNodeAnim = 16,					//3-15 reserved for expansion of TWinType
		EWinTreeNodeSprite,
		EWinTreeNodeStandardTextCursor,
		};
	
	/** 
	 @return The Node Type for this MWsWindowTreeNode. 	 */
	virtual TType NodeType() const = 0;
	/**
	 @return A pointer to a MWsWindow interface only when this 
	 MWsWindowTreeNode represents a client window 
	 (i.e. node type EWinTreeNodeClient), NULL otherwise. */
	virtual const MWsWindow* Window() const = 0;
	/**
	 @return A pointer to a MWsSprite interface only when this 
	 MWsWindowTreeNode represents a sprite, NULL otherwise. */
	virtual const MWsSprite* Sprite() const = 0;
	/**
	 @return A pointer to a MWsStandardTextCursor interface only when this 
	 MWsWindowTreeNode represents a standard text cursor, NULL otherwise. */
	virtual const MWsStandardTextCursor* StandardTextCursor() const = 0;
	/** 
	 @return The MWsWindowGroup this MWsWindowTreeNode belongs to, or "this"
	 MWsWindowTreeNode object if the NodeType is EWinTreeNodeGroup. Will  
	 return NULL if this MWsWindowTreeNode is of type EWinTreeNodeRoot. */
	virtual const MWsWindowGroup* WindowGroup() const = 0;
	/**
	 @return The parent MWsWindowTreeNode. NULL if current node is of type
	 EWinTreeNodeRoot. The parent of a floating sprite is the root window. */
	virtual const MWsWindowTreeNode* ParentNode() const = 0;
	};

/** 
An interface optionally provided by render stages, to observe changes in 
window tree structure, or changes to tree nodes.

@publishedPartner
@prototype
*/
class MWsWindowTreeObserver : public MWsObjectProvider
	{
public:	
	DECLARE_WS_TYPE_ID(KMWsWindowTreeObserver)
	
public:
	/**
	 This enum encapsulates the set of boolean attribute changes that can be observed
	 using the MWsWindowTreeObserver interface. 
	 @see FlagChanged */
	enum TFlags
		{
		/** For a window to be drawn, it needs to be activated and visible and have visible parents.
	 	Making a window invisible, implies that its children should be invisible as well.
	 	The default value for a window is true. */
		EVisible = 1,
		/** Non-fading windows should not be drawn faded regardless of their fade-count.
		The default value is false. */
		ENonFading,
		/** Windows with this attribute enabled uses alpha channel to control its transparency.
		The default value is false. */
		EAlphaChannelTransparencyEnabled,
		/** True if a clipping rect should be applied when drawing a standard text cursor, false otherwise.
		The default value is false. */
		ECursorClipRectSet,
		/** Implicitly defaults to true. Will remain true as long as the application's primary screendevice 
		is in the same orientation as the device. This information is only given for top-windows, 
		i.e. children of group-windows, and only when wsini.ini is configured with SIZE_MODE > 0. */
		EScreenDeviceValid
		};
	/**
	 This enum encapsulates the set of non-boolean attribute changes that can be observed
	 using the MWsWindowTreeObserver interface. 
	 @see AttributeChanged */
	enum TAttributes
		{
		/** Cursor type.
		@see MWsStandardTextCursor::Type()*/
		ECursorType = 1,
		/** Cursor clip rect.
		@see MWsStandardTextCursor::ClipRect()*/
		ECursorClipRect,
		/** Cursor flags.
		@see MWsStandardTextCursor::Flags()*/
		ECursorFlags,
		/** Cursor color.
		@see MWsStandardTextCursor::Color()*/
		ECursorColor,
		/** Window shape.
		@see MWsWindow::WindowArea()*/
		EWindowShape,
		/** Window group name
		@see MWsWindowGroup::Name()*/
		EWindowGroupName
		};
	/** 
	 Callback to notify that a MWsWindowTreeNode has just been created. 
	 Nodes are always added as the left-most sibling as this means they are
	 added in front of exisitng nodes.
	 @param aWindowTreeNode Use to query attributes of the node that has been 
	 						created. Note that attributes such as e.g. the extent
	 						is not yet set by the client when this notification arrives.
	 @param aParent The parent node which aWindowTreeNode has been attached to. 
	 				Special cases: aParent will be set to NULL for root windows, 
	 				and set to the root window for floating sprites. */
	virtual void NodeCreated(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode* aParent) = 0;
	/** 
	 Callback to notify that a node in the window tree is about to be destroyed. 
	 @param aWindowTreeNode Use to query attributes of node that's about to be destroyed. */
	virtual void NodeReleased(const MWsWindowTreeNode& aWindowTreeNode) = 0;
	/**
	 Callback to notify that a node in the window tree has been activated. This 
	 event is only generated for windows which has a visual appearance 
	 (i.e. root windows and group windows are not activated). Before a node 
	 can be drawn it needs to be activated and visible and have a non-zero size.
	 @param aWindowTreeNode Use to query attributes of node that has been activated. */
	virtual void NodeActivated(const MWsWindowTreeNode& aWindowTreeNode) = 0;
	/**
	 Callback to notify that a node's extent has changed. It is guaranteed that there
	 will be at least one NodeExtentChanged notification between the NodeCreated and 
	 NodeActivated notifications.
	 Note that when a window has child-windows, those are always moving along with their
	 parent but no NodeExtentChanged notifications will be generated for the child-windows.
	 @param aWindowTreeNode Use to query attributes of the node that has been changed.
	 @param aRect The new rectangle. */
	virtual void NodeExtentChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRect& aRect) = 0;
	/**
	 Callback to notify that the ordinal position of a window has changed.
	 @param aWindowTreeNode Use to query attributes of the node that has been changed.. 
	 @param aNewPos The new position of this node among its siblings. */
	virtual void SiblingOrderChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aNewPos) = 0;
	/**
	 Callback to notify that a node has had one of its boolean attributes changed.
	 @param aWindowTreeNode Use to query attributes of the node that has been changed.
	 @param aFlag The attribute that has changed.
	 @param aNewValue The new value for the attribute. */
	virtual void FlagChanged(const MWsWindowTreeNode& aWindowTreeNode, TFlags aFlag, TBool aNewValue) = 0;
	/**
	 Callback to notify that a node has had one of its non-boolean attributes changed.
	 @param aWindowTreeNode Use to identify the affected node and query the new value of the changed attribute.
	 @param aAttribute The attribute that has changed. */
	virtual void AttributeChanged(const MWsWindowTreeNode& aWindowTreeNode, TAttributes aAttribute) = 0;
	/**
	 Callback to notify that a window's fade count has been changed.
	 @param aWindowTreeNode Use to query attributes of the node that has been changed.
	 @param aFadeCount Zero means the window is not faded, an integer > 0 means 
	 					faded if absolute fading is used, or the number of times 
	 					SetFaded has been called if using counting fade. */
	virtual void FadeCountChanged(const MWsWindowTreeNode& aWindowTreeNode, TInt aFadeCount) = 0;
	/**
	 Callback to notify that the user defined transparent region of a window has changed. 
	 This information is for optimisation purpose only, the rendering engine don't need 
	 to do a full back-to-front rendering behind the opaque region. 
	 @param aWindowTreeNode  Use to query attributes of the node that has been changed.
	 @param aNewTransparentRegion   The transparent region defined by the user. Coordinates 
	 								are relative the window's origin.
	 @param aNewOpaqueRegion 	The inverse of aNewTransparentRegion.  Coordinates are 
	 							relative the window's origin. */	
	virtual void TransparentRegionChanged(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aNewTransparentRegion, const TRegion* aNewOpaqueRegion) = 0;
	/**
	 Callback to notify that a element has been added to this window.
	 The render stages implementing this interface know when elements are added or removed through MWsElement interface.
	 The purpose of the ElementAdded API is only to enable render stages to make an association between the element and the window.
	 @param aWindowTreeNode Use to query attributes of the node that has been changed.
	 @param aElement Use to query attributes of the new element. */
	virtual void ElementAdded(const MWsWindowTreeNode& aWindowTreeNode, const MWsElement& aElement) = 0;
	/**
	 Callback to notify that a window has been moved to another window group.
	 @param aWindowTreeNode Use to query attributes of the node that has been moved.
	 @param aNewWindowGroupNode The window group aWindowTreeNode has been moved to. 
	 @see RWindowBase::MoveToGroup */
	virtual void MovedToWindowGroup(const MWsWindowTreeNode& aWindowTreeNode, const MWsWindowTreeNode& aNewWindowGroupNode) = 0;
	/**
	 Callback to notify that a window group chain has been formed or extended.
	 @param aParent The window group node to which a child has been attached.
	 @param aChild The window group node that has been attached to aParent. */
	virtual void WindowGroupChained(const MWsWindowTreeNode& aParent, const MWsWindowTreeNode& aChild) = 0;
	/**
	 Callback to notify that a window group chain has been broken. 
	 Special case: Deleting a window group that is part of a chain will break all chaining 
	 after the deleted window group and a notification for each node being dequed will be sent.
	 @param aWindowGroupNode The window group chain is broken directly after this window group. */
	virtual void WindowGroupChainBrokenAfter(const MWsWindowTreeNode& aWindowGroupNode) = 0;
	/**
	 Callback to notify that all nodes below aWindowTreeNode should be drawn faded,
	 unless they have the ENonFading flag set. 
	 This API is primarily used in conjunction with application frameworks that uses 
	 absolute fading rather than counting fade.
	 @param aWindowTreeNode Typically a root window, for which all children should have their
	 					fade state set to aFaded.
	 @param aFaded The new fade state to apply. ETrue means faded, EFalse not faded. */
	virtual void FadeAllChildren(const MWsWindowTreeNode& aWindowTreeNode, TBool aFaded) = 0;
	};

/** 
An interface optionally provided by render stages, to observe which node in 
the window tree structure that is being rendered.

Text cursors and sprites (floating or non) are annotated by the same APIs.

@publishedPartner
@prototype
*/
class MWsDrawAnnotationObserver : public MWsObjectProvider
	{
public:	
	DECLARE_WS_TYPE_ID(KMWsDrawAnnotationObserver)
	virtual void WindowRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion) = 0;
	virtual void WindowRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode) = 0;
	virtual void WindowAnimRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion) = 0;
	virtual void WindowAnimRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode) = 0;
	virtual void SpriteRedrawStart(const MWsWindowTreeNode& aWindowTreeNode, const TRegion& aRegion) = 0;
	virtual void SpriteRedrawEnd(const MWsWindowTreeNode& aWindowTreeNode) = 0;
	virtual void SpriteFlash(const MWsWindowTreeNode& aWindowTreeNode, TBool aFlashOn) = 0;
	virtual void SegmentRedrawStart(const TRegion& aRegion) = 0;
	virtual void SegmentRedrawEnd() = 0;
	};

/** 
Implemented by a WSERV screen to respond to window visibility change notifications sent
by a render stage implementing MWsWindowVisibilityNotifier.

Immediately after WSERV sucessfully obtains a pointer to MWsWindowVisibilityNotifier, 
WSERV will call MWsWindowVisibilityNotifier::RegisterWindowVisibilityObserver()
to provide a means for the render stage to notify WSERV, when window visibility changes.

@see MWsWindowVisibilityNotifier

@publishedPartner
@prototype
*/
class MWsWindowVisibilityObserver
	{
public:
	/** 
	Called by the render stage to indicate a window's visible region has changed.
	After this method has been called, it does not need to be called again until
	after WSERV calls SendVisibilityChanges.
	@see MWsWindowVisibilityNotifier::SendVisibilityChanges
	*/
	virtual void VisibilityChanged() = 0;

	/**
	Called by the render stage to update a window's visible region.
	@param aWindow The window whose visible region has changed.
	@param aVisibleRegion Region of the window not obscured by a fully opaque window. Part of this region could potentially still
	be obscured by opaque parts of translucent windows on top of aWindow.
	*/
	virtual void SetWindowVisibility(const MWsWindow& aWindow, const TRegion& aVisibleRegion) = 0;
	};

/** 
This interface may be implemented by a render stage wishing to take over responsibility
from WSERV, for updating a window's visible region.
For example, if a render stage introduces non-WSERV visuals which overlap WSERV windows, WSERV may
need to receive updated window visibility information.

Immediately after sucessfully obtaining this interface, WSERV will call RegisterWindowVisibilityObserver
to provide a means for the render stage to notify WSERV when window visibility changes.

@see MWsWindowVisibilityObserver

@publishedPartner
@prototype
*/
class MWsWindowVisibilityNotifier : public MWsObjectProvider
	{
public:	
	DECLARE_WS_TYPE_ID(KMWsWindowVisibilityNotifier)

	/** 
	Register to be notified when window visibility changes.
	@param aWindowVisibilityObserver Observer to be notified.
	*/
	virtual void RegisterWindowVisibilityObserver(MWsWindowVisibilityObserver* aWindowVisibilityObserver) = 0;

	/** 
	Register to be notified when visibility of a specific window changes.
	After registering a window, the render stage must immediately provide the
	current visible region by calling VisibilityChanged() and later, in response
	to SendVisibilityChanges, call SetWindowVisibility.
	RegisterWindow and UnregisterWindow are symmetrical operations. For each
	call to RegisterWindow, there will follow exactly one call to UnregisterWindow.
	@param aWindow Window for which WSERV needs accurate visibility information.
	@see MWsWindowVisibilityObserver
	*/
	virtual void RegisterWindow(const MWsWindow& aWindow) = 0;

	/** 
	Stops visibility notifications being sent for the specified window.
	RegisterWindow and UnregisterWindow are symmetrical operations. For each
	call to RegisterWindow, there will follow exactly one call to UnregisterWindow.
	@param aWindow Window for which visibility notifications are no longer required.
	*/
	virtual void UnregisterWindow(const MWsWindow& aWindow) = 0;

	/** 
	Asks the render stage to call SetWindowVisibility for each window whose
	visible region has changed.
	All visible region changes must be sent before this method returns.
	@see MWsWindowVisibilityObserver
	*/
	virtual void SendVisibilityChanges() = 0;
	};

/** This is an extension of the MWsScreen interface for handling the
animation aspects of the redraw sequence.

@publishedPartner
@prototype
*/
class MWsScreenRedraw: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KWsScreenRedraw)
	/**Sets the screen redraw observer. 
	@param aObserver
	*/
	virtual void SetObserver(MWsScreenRedrawObserver* aObserver) = 0;
	/** Checks if there is a redraw in progress or scheduled to take place.
	@return Returns ETrue if there is an update pending.
	*/	
	virtual TBool IsUpdatePending() = 0;
	/** Gets the current time according to the animation scheduler
	@return The current time as a TTime reference.
	*/
	virtual const TTime& Now() const = 0;
	/** Schedule a render of the screen without requesting any area of it
	to be redrawn.  Effectively, request a rerun of the render stages.
	@param aFromNow The time from now, in microseconds, to perform the render.
	*/
	virtual void ScheduleRender(const TTimeIntervalMicroSeconds& aFromNow) = 0;
	/** Gets the the animation region for this screen redraw.
	@return the Region being redrawn during the animation
	*/
	virtual const TRegion * AnimationRegion() const = 0;
	/** Updates the screen device to reflect any animation changes
	*/
	virtual void UpdateDevice() = 0;
	};
	
/** This is part of the MWsScreenRedraw interface

@publishedPartner
@prototype
*/
class MWsScreenRedrawObserver: public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KWsScreenRedrawObserver)
	/** Callback function that gets called after the completion of every redraw sequence.
	@see MWsScreenRedraw::SetObserver
	@publishedPartner
	@prototype
	*/
	virtual void ScreenUpdated(TInt aScreenNumber) = 0;
	};

/** This is part of the MGraphicDrawerInterface

@publishedPartner
@prototype
*/
class MWsMemoryRelease : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsMemoryRelease)
	enum TMemoryReleaseLevel
		{
		ELow,
		EMedium,
		EHigh
		};
	/** This function is called on registered memory release objects whenever there
	is insufficient memory available to honour an Alloc or ReAlloc in the window server
	process.  It may be called up to three times, once for each level defined in TLevel.
	The meaning of each level is open to interpretation by individual implementations.
	@see MGraphicDrawerEnvironment::RegisterMemoryRelease
	@param aLevel 
	@return ETrue to indicate that memory was freed, EFalse otherwise.
	@publishedPartner
	@prototype
	*/
	virtual TBool ReleaseMemory(TMemoryReleaseLevel aLevel) = 0;
	};

#if !defined(SYMBIAN_GRAPHICS_GCE)
/** This is an extension of the MWsGc.  This interface is not used in
non-NGA configurations.  It has been removed from NGA configurations.

@see MWsGc
@publishedPartner
@prototype
*/
class MWsGcClipRect : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsGcClipRect)
	/** Sets a master clipping rectangle on the GC.  This will combine with any clipping rectangles
	specified by the client.
	@param aRect
	*/
	virtual void SetClipRect(TRect aRect) = 0;
	/** Resets the master clipping rectangle specified in SetMasterClipRect
	*/
	virtual void ResetClipRect() = 0;
	/** Returns the combined clipping rectangle.
	@param aRect this is set to the clipping rectangle
	@param aSet this is true if there is a clipping rectangle in effect
	*/
	virtual void GetClipRect(TRect & aRect, TBool & aSet) = 0;
	};
#endif

/** DebugBarDrawer interface
     
@publishedPartner
@prototype
*/
class MWsDebugBar : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsDebugBar)
	
public:
	/**
	 * Prints a debug information bar overlaid at the top the screen.
	 * 
	 * @param aDebugText An array of text lines.
	 */
	virtual void DrawDebugBar(const TArray<TPtrC>& aDebugText)=0;
	};

/** This is an extension of MWsGc and provides plugin fading capabilities for wserv.
    Default implementation of the plugin uses bitgdi fade. 
    
@see MWsGc
@publishedPartner
@prototype
*/
class MWsFader : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsFader)
	
public:
	/** Sets any parameters for the fade.
	@param aData Descriptor containing the fade parameters
	*/
	virtual void SetFadingParameters(const TDesC8& aData)=0;
#if !defined(SYMBIAN_GRAPHICS_GCE)
	/** Perform a fade of the specified region
	@param aBitGc CFbsBitGc to perform the fading with.
	@param aRegion TRegion containing the region that the fading is to be performed on.
	*/
	virtual void FadeArea(CFbsBitGc* aBitGc,const TRegion * aRegion)=0;
#else
	/** Perform a fade of the specified region
	@param aRegion TRegion containing the region that the fading is to be performed on.
	*/
	virtual void FadeArea(const TRegion& aRegion)=0;
#endif
	};

/** Plugin Manager Interface
@publishedPartner
@prototype
*/	
class MWsPluginManager : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsPluginManager)
	
	/** Resolve an instance of an interface
	@param aTypeId
	*/
	virtual TAny* ResolvePluginInterface(TUint aTypeId) = 0;
	};
	
/** An Interface to allow CRP's and CWsPlugins to access the wserv ini file
@publishedPartner
@prototype
*/
class MWsIniFile : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsIniFile)
	
	/** Find the value of the specified variable if present in the window server ini file.
	@param aVarName TDesC containing the name of the variable to find.
	@param aResult TPtrC that gets set to the value of the ini variable.
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC &aVarName, TPtrC &aResult) = 0;

	/** Find the value of the specified variable if present in the window server ini file.
	@param aVarName TDesC containing the name of the variable to find
	@param aResult TInt that gets set to the value of the ini variable
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC &aVarName, TInt &aResult) = 0;

	/** Find if the variable is present in the ini file
	@param aVarName TDesC containing the name of the variable to find
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC &aVarName) = 0;

	/** Find if the variable is present in the [SCREENx] section
	@param aScreen TInt containing the screen number
	@param aVarName TDesC containing the name of the variable to find
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar( TInt aScreen, const TDesC &aVarName) = 0;

	/** Find the value of the specified variable if present in the [SCREENx] section of the ini file
	@param aScreen TInt containing the screen number
	@param aVarName TDesC containing the name of the variable to find
	@param aResult TInt that gets set to the value of the ini variable
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar( TInt aScreen, const TDesC &aVarName, TInt &aResult) = 0;

	/** Find the value of the specified variable if present in the [SCREENx] section of the ini file
	@param aScreen TInt containing the screen number
	@param aVarName TDesC containing the name of the variable to find
	@param aResult TPtrC that gets set to the value of the ini variable.
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar( TInt aScreen, const TDesC& aVarName, TPtrC &aResult) = 0;

	/** Find if the specified variable is present in the named section of the ini file
	@param aSection TDesC containing the name of the section 
	@param aVarName TDesC containing the name of the variable to find
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC& aSection, const TDesC &aVarName) = 0;
	
	/** Find the value of the specified variable in the named section of the ini file.
	@param aSection TDesC containing the name of the section 
	@param aVarName TDesC containing the name of the variable to find
	@param aResult TInt that gets set to the value of the ini variable
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC& aSection, const TDesC &aVarName, TInt &aResult) = 0;

	/** Find the value of the specified variable in the named section of the ini file.
	@param aSection TDesC containing the name of the section 
	@param aVarName TDesC containing the name of the variable to find
	@param aResult TPtrC that gets set to the value of the ini variable.
	@return ETrue if ini file variable found
	*/
	virtual TBool FindVar(const TDesC& aSection, const TDesC& aVarName, TPtrC &aResult) = 0;
	};

/** An interface that allows CRP's and CWsPlugin derived objects to handle events like anim dll's
@publishedPartner
@prototype
*/
class MWsRawEventServer : public MWsObjectProvider
	{
public:
	DECLARE_WS_TYPE_ID(KMWsRawEventServer)

	/** Register to be notified when some kernel events occur.
	@return KErrNone if successful
	@prototype */
	virtual TInt RegisterRawEventHandler(MEventHandler* aHandler) = 0;
	/** Unregister from events notification
	@prototype */
	virtual void UnregisterRawEventHandler(MEventHandler* aHandler) = 0;
	/** Creates a new raw event
	@prototype */
	virtual void PostRawEvent(const TRawEvent & aEvent) = 0;
	/** Creates a new key event
	@prototype */
	virtual void PostKeyEvent(const TKeyEvent & aEvent) = 0;
	};

#endif //#ifndef __WSGRAPHICDRAWERINTEFACE_H__
