// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPlaybackGc and associated classes definitions
// 
//

#ifndef __PLAYBACKGC_H__
#define __PLAYBACKGC_H__

#include "wnredraw.h"
#include "cliwin.h"

class CWsFbsFont;
class MWsGraphicsContext;

class CPlaybackGc : public CBase, public MWsGc, public MWsSurfacePlacement
	{
public:
	static void InitStaticsL();
	static void DeleteStatics();
	static CPlaybackGc * Instance() { return iSelf; }
public:
	void ConstructL();
	void Activate(CWsClientWindow * aWin, MWsGraphicsContext * aGc, const TRegion * aRegion);
	void CommandL(TWsGcOpcodes aOpcode, const TDesC8& aCmdData);
	void Deactivate();
	inline CFbsBitGc *Gdi() const;
	void BitGcChanged(CFbsBitGc *aGc);
	void Resized();
	void SetOrigin(const TPoint &aOrigin);
	void InternalizeL(const CBufBase& aBuffer,TInt& aStartPos);
	void InternalizeClippingRegionL(RReadStream& aReadStream);
	TInt FbsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	TInt WsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	// implementing MWsGc
	MWsClient& Client();
	MWsScreen& Screen();
	const TTime& Now() const;
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	void SetGcOrigin(const TPoint& aOrigin);
	
	void SetTargetRegion(const TRegion* aRegion);
	void Reset();
	TInt	GcDrawingCount();

public: // implementing MWsObjectProvider
	TAny * ResolveObjectInterface(TUint aId);

  /** 
  * @deprecated
  */
	TInt   PlaceSurface(const TSurfaceConfiguration& aConfig); 
	
private:
	CPlaybackGc();
	~CPlaybackGc();
	void DoDrawing(TWsGcOpcodes aOpcode, const TDesC8& aCmdData);
	void CancelUserClippingRegion();
	void SetFadingParams(CWsClientWindow* aWin);
	void RemoteReadDataAndDrawL(const CWsGraphicDrawer* aGraphic, CWsClient* aOwner, const TWsGcCmdUnion &aData);
	void DoDrawCommand(TWsGcOpcodes aOpcode, const TDesC8& aCmdData, const TRegion *aRegion);
	void SetClippingRect(const TRect &aRect);
	void DoDrawPolygon(const TWsGcCmdDrawPolygon *aDrawPolygon);
	void DoDrawPolyLine(const TWsGcCmdDrawPolyLine *aDrawPolyLine, TBool aContinued);
	void sendGdiMessage(TInt opcode,TAny *params);
	void GcOwnerPanic(TClientPanic aPanic);
	void ResetClippingRect();
	void EndSegmentedPolygon();
	void StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon *aDrawPolygon);
	void SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData *aDrawPolygon);
	void UpdateJustification(TText* aText, TInt aLen, const TDesC8& aCmdData, CGraphicsContext::TTextParameters* aParam);
#if defined(_DEBUG)
	inline TBool IsPolyPointData() {return(iPolyPointListSize>0 || iPolyPoints!=NULL);}
#endif
	void InternalizeAlphaValueL(RReadStream& aReadStream);
	void ChangeDevice();
	void CheckPolyData(const TAny* aDataPtr, TInt aHeaderSize, TInt aNumPoints);
	TPtrC BufferTPtr(TText* aStart, TInt aLen, const TDesC8& aCmdData);
	void	GcDrawingDone();
	void DoDrawResource(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &aData);
	void SendOriginIfRequired();
	void SendClippingRegionIfRequired(const TRegion* aRegion);
// Data
private:
	static CPlaybackGc * iSelf;
	CWsClientWindow *iWin;
	TPoint iMasterOrigin;     // Origin available to wserv.  Usually the window origin.
	MWsGraphicsContext * iGc;
	CWsFbsFont * iFont;
	const TRegion *iTargetRegion;          //Clipping region set by wserv window management.
	RWsRegion* iUserDefinedClippingRegion; //Clipping region that is set explicitly by the user.
	const TRegion *iDrawRegion;            //Pointer to one of the above regions
	TRect iClippingRect;	// All drawing clipped to this rectangle in addition to other clipping
	TBool iClippingRectSet;	// ETrue if the iClippingRect is in use
	RRegion iIntersectedRegion; // iDrawRegion clipped to iClippingRect, or just iDrawRegion if no user clipping rect active
	TPoint iOrigin;			// User defined origin, relative to the top left of the window
	TPoint iLinePos;		// Current line position for LineBy, LineTo etc
	TPoint *iPolyPoints;
	TInt iPolyPointListSize;
	// members used for keeping state when drawing CWsGraphicDrawers
	CBufBase* iGcBuf;
	const TRegion* iCurrentClippingRegion; // Clipping region while doing DoDrawCommand
	TInt	iGcDrawingCounter;				// Flags that some drawing has occurred between readings

	TBool iSendOrigin; // MWsGraphicsContext::SetOrigin must be called at next opportunity
    TPoint iLastSentOrigin; // last origin value sent to render stage
	};

#endif // __PLAYBACKGC_H__
