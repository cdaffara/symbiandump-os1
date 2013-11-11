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
// CWsGc and associated classes definitions
// 
//

#ifndef __GC_H__
#define __GC_H__

#include "wnredraw.h"
#include "cliwin.h"
#include "graphicscontextstate.h"

class CWsFbsFont;

class CWsGc : public CWsObject
	{
	enum {ETextPtrBufLen=0x100};	// Buffer of TText, used on stack
// Functions
public:
	static CWsGc* NewL(CWsClient *aOwner);
	~CWsGc();
	static void InitStaticsL();
	static void DeleteStatics();
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void Deactivate();
	inline void SetNextWinGc(CWsGc *aGc);
	inline CWsGc *NextWinGc() const;
	TInt ExternalizeL(CBufBase& aBuffer, TInt aStartPos);
	TInt ExternalizeClippingRegionL(RWriteStream& aWriteStreem);
	TInt FbsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	TInt WsBitmapHandle(TInt aOpcode, const TWsGcCmdUnion &pData, TInt& aMaskHandle);
	TInt WsDrawableSourceHandle(TInt aOpcode, const TWsGcCmdUnion &aData);
	void Reactivate();
private:
	CWsGc(CWsClient *aOwner);
	void ConstructL();
	void Activate(CWsClientWindow *win);
	void DoDrawing2(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData);
	void SetGcAttribute(TInt aOpcode, TWsGcCmdUnion pData);
	void SetOrigin(const TPoint &aOrigin);

	void DoDrawing0L(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData);
	void DoDrawing1(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData);
	void Activate(const TInt &aHandle);
	void DoDrawCommand(TWsGcOpcodes aOpcode, const TWsGcCmdUnion &pData);
	void SetClippingRegionL(TInt aCountRegion);
	void SetClippingRect(const TRect &aRect);
	void CancelClippingRegion();
	void DoDrawPolygon(const TWsGcCmdDrawPolygon *aDrawPolygon);
	void DoDrawPolyLine(const TWsGcCmdDrawPolyLine *aDrawPolyLine, TBool aContinued);
	void GcOwnerPanic(TClientPanic aPanic);
	void ResetClippingRect();
	void EndSegmentedPolygon();
	void StartSegmentedDrawPolygonL(const TWsGcCmdStartSegmentedDrawPolygon* aDrawPolygon);
	void SegmentedDrawPolygonData(const TWsGcCmdSegmentedDrawPolygonData* aDrawPolygon);
	void UpdateJustification(TText* aText,TInt aLen);
	void UpdateJustification(TText* aText,TInt aLen,CGraphicsContext::TTextParameters* aParam);
#if defined(_DEBUG)
	inline TBool IsPolyPointData() {return(iPolyPointListSize>0 || iPolyPoints!=NULL);}
#endif
	void ExternalizeAlphaValueL(RWriteStream& aWriteStream);
	TPtrC BufferTPtr(TText* aStart,TInt aLen);
	void CheckPolyData(const TAny* aDataPtr, TInt aHeaderSize, TInt aNumPoints);
// Data
private:
	CFbsBitGc *iBackedUpWinGc;
	CWsClientWindow *iWin;
	CWsGc *iNextWinGc;
	CWsFbsFont *iFont;
	RWsRegion* iUserDefinedClippingRegion; //Clipping region that is set explicitly by the user.
	TRect iClippingRect;	// All drawing clipped to this rectangle in addition to other clipping
	TBool iClippingRectSet;	// ETrue if the iClippingRect is in use
	TPoint iOrigin;			// User defined origin, relative to the top left of the window
	TPoint iLinePos;		// Current line position for LineBy, LineTo etc
	TPoint *iPolyPoints;
	TInt iPolyPointListSize;
	//member used for storing graphics context data for Internalize/Externalize
	TInternalGcStatus iInternalStatus;
	// static variables
	static CFbsBitmap *iScratchBitmap;
	static CFbsBitmap *iScratchMaskBitmap;
	};

inline void CWsGc::SetNextWinGc(CWsGc *aGc)
	{
	iNextWinGc=aGc;
	}

inline CWsGc *CWsGc::NextWinGc() const
	{
	return(iNextWinGc);
	}

#endif
