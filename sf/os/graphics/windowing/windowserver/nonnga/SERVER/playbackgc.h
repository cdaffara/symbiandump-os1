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

class CPlaybackGc : public CBase, public MWsGc
	{
public:
	static void InitStaticsL();
	static void DeleteStatics();
	static CPlaybackGc * Instance() { return iSelf; }
public:
	void ConstructL();
	void Activate(CWsClientWindow * aWin, CFbsBitGc * aGc, const TRegion * aRegion);
	void CommandL(TWsGcOpcodes aOpcode, const TDesC8& aCmdData);
	void Deactivate();
	inline CFbsBitGc *Gdi() const;
	void SetOrigin(const TPoint &aOrigin);
	void InternalizeL(const CBufBase& aBuffer,TInt& aStartPos);
	void InternalizeClippingRegionL(RReadStream& aReadStream);
	TInt FbsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	TInt WsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	// implementing MWsGc
	MWsClient& Client();
	MWsScreen& Screen();
	TPoint GcOrigin() const;
	const TRegion& ClippingRegion();
	CFbsBitGc& BitGc();
	TInt PushBitGcSettings();
	void PopBitGcSettings();	
	const TTime& Now() const;
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow);
	void ScheduleAnimation(const TRect& aRect,const TTimeIntervalMicroSeconds& aFromNow,const TTimeIntervalMicroSeconds& aFreq,const TTimeIntervalMicroSeconds& aStop);
	void SetGcOrigin(const TPoint& aOrigin);
	void SetTargetRegion(const TRegion* aRegion);
	void Reset();
public: // from MWsObjectProvider
	TAny * ResolveObjectInterface(TUint aId);
private:
	CPlaybackGc();
	~CPlaybackGc();
	void DoDrawing(TWsGcOpcodes aOpcode, const TDesC8& aCmdData);
	void CancelUserClippingRegion();
	void RemoteReadDataAndDrawL(const CWsGraphicDrawer* aGraphic, CWsClient* aOwner, const TWsGcCmdUnion &aData);
	void DoDrawCommand(TWsGcOpcodes aOpcode, const TDesC8& aCmdData, const TRegion *aRegion);
	void SetClippingRect(const TRect &aRect);
	const RWsRegion& Region();
	void DoDrawPolygon(const TWsGcCmdDrawPolygon *aDrawPolygon);
	void DoDrawPolyLine(const TWsGcCmdDrawPolyLine *aDrawPolyLine, TBool aContinued);
	void GcOwnerPanic(TClientPanic aPanic);
	void ResetClippingRect();
	void EndSegmentedPolygon();
	void StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon *aDrawPolygon);
	void SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData *aDrawPolygon);
	void UpdateJustification(TText* aText,TInt aLen, const TDesC8& aCmdData);
	void InternalizeAlphaValueL(RReadStream& aReadStream);

	void CheckPolyData(const TAny* aDataPtr, TInt aHeaderSize, TInt aNumPoints);
	TPtrC BufferTPtr(TText* aStart, TInt aLen, const TDesC8& aCmdData);
// Data
private:
	static CPlaybackGc * iSelf;
	CWsClientWindow *iWin;
	TPoint iMasterOrigin;     // Origin available to wserv.  Usually the window origin.
	CFbsBitGc * iGc;
	CWsFbsFont * iFont;
	const TRegion *iTargetRegion;          //Clipping region set by wserv window management.
	RWsRegion* iUserDefinedClippingRegion; //Clipping region that is set explicitly by the user.
	const TRegion *iDrawRegion;            //Pointer to one of the above regions
	TRect iClippingRect;	// All drawing clipped to this rectangle in addition to other clipping
	TBool iClippingRectSet;	// ETrue if the iClippingRect is in use
	TPoint iOrigin;			// User defined origin, relative to the top left of the window
	TPoint iLinePos;		// Current line position for LineBy, LineTo etc
	TPoint *iPolyPoints;
	TInt iPolyPointListSize;
	CFbsBitmap *iScratchBitmap;
	CFbsBitmap *iScratchMaskBitmap;
	// members used for keeping state when drawing CWsGraphicDrawers
	CBufBase* iGcBuf;
	const TRegion* iCurrentClippingRegion; // Clipping region while doing DoDrawCommand
	};

#endif // __PLAYBACKGC_H__
