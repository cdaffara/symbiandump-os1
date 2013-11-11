// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MEDIACLIENTVIDEODISPLAYBODY_H
#define MEDIACLIENTVIDEODISPLAYBODY_H

#include "mediaclientextdisplayhandler.h"
#include "mediaclientwseventobserver.h"
#include <graphics/surface.h>
#include <graphics/surfaceconfiguration.h>
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <videoplayer.h>
#include <mmf/plugin/mmfmediaclientextdisplayinterface.h>

class MMMFSurfaceEventHandler;
class CMediaClientPolicyServerClient;

/**
@internalTechnology
@released

*/
NONSHARABLE_CLASS(CMediaClientVideoDisplayBody) : public CBase, public MExtDisplayConnectionProviderCallback,
public MMediaClientWsEventObserverCallback
	{
private:
	NONSHARABLE_CLASS(TWindowData)
		{
	public:
		TWindowData() :
		   	iWindow(NULL)
			{
			}
		
		TWindowData(const RWindowBase* aWindow) :
			iWindow(const_cast<RWindowBase*>(aWindow))
			{
			}
				
		TWindowData(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aVideoExtent,
					TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation, TAutoScaleType aAutoScaleType,
					TInt aHorizPos, TInt aVertPos,RWindow* aWindow2) :
			iWindow(const_cast<RWindowBase*>(aWindow)),
			iClipRect(aClipRect),
			iVideoExtent(aVideoExtent),
			iScaleWidth(aScaleWidth),
			iScaleHeight(aScaleHeight),
			iRotation(aRotation),
			iAutoScaleType(aAutoScaleType),
			iHorizPos(aHorizPos),
			iVertPos(aVertPos),
			iWindow2(aWindow2)
			{
			}
		
		RWindowBase* iWindow;
		TSurfaceConfiguration iSurfaceConfig;
		TRect iClipRect;
		TRect iVideoExtent;
				
		TReal32 iScaleWidth;
		TReal32 iScaleHeight;
		TVideoRotation iRotation;
			
		TAutoScaleType iAutoScaleType;
		TInt iHorizPos;
		TInt iVertPos;
		
		// iWindow2 is set when using CVideoPlayerUtility2.  This is used to avoid 
		// casting iWindow from RWindowBase* to RWindow*.
		RWindow* iWindow2;
		

		static TBool CompareByWsHandle(const TInt* aWsHandle, const TWindowData& aWindowData)
			{
			TBool ret = EFalse;
			if (aWsHandle)
				{
				ret = (*aWsHandle == aWindowData.iWindow->WsHandle());
				}
			return ret;
			}
		};
public:

	static CMediaClientVideoDisplayBody* NewL(TInt aDisplayId, TBool aExtDisplaySwitchingControl);
	static CMediaClientVideoDisplayBody* NewL(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, TBool aExtDisplaySwitchingControl);
	
	~CMediaClientVideoDisplayBody();
	
	void AddDisplayL(MMMFSurfaceEventHandler& aEventHandler);
	void AddDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion, const TRect& aVideoExtent,
							TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation,
							TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2);
	void UpdateDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion, const TRect& aVideoExtent, 
                            TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation, 
                            TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2);
	void RemoveDisplay();
	TInt RemoveDisplayWindow(const RWindowBase& aWindow);
	
	TInt SurfaceCreated(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, const TRect& aCropRegion);
	TInt SurfaceParametersChanged(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio);
	void RemoveSurface(TBool aControllerEvent);
	void RemoveBackgroundSurface(TBool aTriggerRedraw);
	TInt RedrawWindows(const TRect& aCropRegion);
		
	void SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion);
	void SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation, const TRect& aCropRegion);
	TVideoRotation RotationL(const RWindowBase& aWindow);
	void SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion);
	void GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage);

	void SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion);
	void SetRotationL(TVideoRotation aRotation, const TRect& aCropRegion);
	void SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion);
	void SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect, const TRect& aCropRegion);
	void SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent, const TRect& aCropRegion);
	
	void SetExternalDisplaySwitchingL(TBool aControl);
	
private:
	
	CMediaClientVideoDisplayBody(TInt aDisplayId);
	CMediaClientVideoDisplayBody(TInt aDisplayId, const TSurfaceId& aSurfaceId, const TRect& aCropRect,	TVideoAspectRatio aAspectRatio);
	
	void ConstructL(TBool aExtDisplaySwitchingControl);
	
	CFbsBitGc::TGraphicsOrientation ConvertRotation(TVideoRotation aRotation);
	
	RArray<CMediaClientVideoDisplayBody::TWindowData>& Windows();
	TInt DisplayId() const;
	TBool IsUsed() const;
	static TInt CompareByDisplay(const TInt* aDisplayId, const CMediaClientVideoDisplayBody& aDisplay);
	static TInt Compare(const CMediaClientVideoDisplayBody& aLeft, const CMediaClientVideoDisplayBody& aRight);
	static CMediaClientVideoDisplayBody* FindDisplayWithWindowL(const RPointerArray<CMediaClientVideoDisplayBody>& aDisplays, const RWindowBase& aWindow);
	TBool IsSurfaceCreated() const;
	TInt SetBackgroundSurface(TWindowData& aWindowData, const TRect& aCropRegion);
	TBool HasWindows() const;
	
    void UpdateCropRegionL(const TRect& aCropRegion, TInt aPos, TBool aRedrawIndividualWindow);
    void CreateExtDisplayPluginL();
    void RemoveExtDisplayPlugin();
    void CreateExtDisplayHandlerL();
    void RemoveExtDisplayHandler();
    void SetWindowArrayPtr2Client();
    void SetWindowArrayPtr2Ext();
    void SwitchSurface();
    void UpdateFocus();
	TBool IsRotationValid(TVideoRotation aVideoRotation);
	TBool IsAutoScaleTypeValid(TAutoScaleType aAutoScaleType);
	TBool SurfaceCropRectChangeRequiresRedraw(TRect aOldSurfaceCropRect, TRect aNewSurfaceCropRect, TRect aClientCropRegion);
	TBool ClientCropRegionChangeRequiresRedraw(TRect aOldClientCropRegion, TRect aNewClientCropRegion, TRect aSurfaceCropRect);
	TBool IntersectionAreaChanged(TRect aOldRect, TRect aNewRect, TRect aOtherRect);
	void UpdateDeltaForExtDisplay(TReal32 aViewportAspect, const TRect& aVideoExtent, TInt& aDeltaHeight, TInt& aDeltaWidth);
	TBool IsWideScreenL();
	TAutoScaleType ExtDisplayAutoScaleTypeL();
	
    // MExtDisplayConnectionProviderCallback
    void MedcpcExtDisplayNotifyConnected(TExtDisplayConnectionProviderConnType aExtDisplayConnType);

    // MMediaClientWsEventObserverCallback
    void MmcweoFocusWindowGroupChanged();
    TBool MmcweoIgnoreProcess(TSecureId aId);
    
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
    void HideAllClientWindows();
    TInt HideWindow(RWindowBase* aWindow);
    void SetHiddenBackgroundSurfaceOnAllClientWindows();
    TInt SetHiddenBackgroundSurfaceOnClientWindow(TWindowData& aWindowData);
#endif
    
private:

    MMMFSurfaceEventHandler* iEventHandler;
	const TInt iDisplayId;
	TSurfaceId iSurfaceId;
	TRect iCropRect;
	TVideoAspectRatio iAspectRatio;
	
	RArray<TWindowData> iClientWindows;
	RArray<TWindowData> iExtDisplayWindows;
	RArray<TWindowData>* iWindowsArrayPtr;
	TBool iSwitchedToExternalDisplay;
	
	TRect iCropRegion;
	TBool iClientRequestedExtDisplaySwitching;
	CExtDisplayConnectionProviderInterface* iExtDisplayConnectionProvider;
	CMediaClientExtDisplayHandler* iExtDisplayHandler;
	CMediaClientWsEventObserver* iWsEventObserver;
	TBool iClientWindowIsInFocus;
    TBool iExtDisplayConnected;
    TBool iExtDisplaySwitchingSupported;
    TExtDisplayConnectionProviderConnType iExtDisplayConnType;
    
    CMediaClientPolicyServerClient* iServerClient;
    
	friend class CVideoPlayerUtility::CBody;	
	friend class CTestStepUnitMMFVidClient;
	};

#endif // MEDIACLIENTVIDEODISPLAYBODY_H

