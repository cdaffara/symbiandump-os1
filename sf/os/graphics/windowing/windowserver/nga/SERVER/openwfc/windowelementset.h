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
//

#if !defined(__WINDOWELEMENTSET_H__)
#define __WINDOWELEMENTSET_H__

/**
@file
@internalComponent
*/

#include <graphics/wselement.h>

class CBase;
class CWsClientWindow;
class TSurfaceConfiguration;
class MWsScene;

class TAttributes
{
public:
	TAttributes(){iFlags = 0;}
	enum
	{
	EConcealed			= 0x00000001,
	EDrawnOver			= 0x00000002,
	EDrawnOverLast		= 0x01000000,
	};
	//flag accessors
	inline TBool Concealed() const {return iFlags&EConcealed;}
	inline void SetConcealed(TBool aSetting = ETrue) {aSetting?iFlags|=EConcealed:iFlags&=~EConcealed;}
	inline TBool DrawnOver() const {return iFlags&EDrawnOver;}
	inline void SetDrawnOver(TBool aSetting = ETrue) {aSetting?iFlags|=EDrawnOver:iFlags&=~EDrawnOver;}
	inline TBool DrawnOverLast() const {return iFlags&EDrawnOverLast;}
	inline void SetDrawnOverLast(TBool aSetting = ETrue) 
		{	iFlags= aSetting? iFlags|EDrawnOverLast:iFlags&~EDrawnOverLast;	}
	inline TInt Flags()const
		{	return iFlags;	}
	TBool Destroy(MWsScene& aScene, TBool aUnregister);
public:
	TInt DebugFlags()const	{	return iFlags;	}
protected:
	TUint iFlags;
public:
	MWsElement* iElement;
};

class TBackgroundAttributes : public TAttributes
{
public:
	enum
	{
	EExplicitExtent		= 0x00010000,
	//EAbsoluteExtent		= 0x00020000,	//This would be used to represent wallpaper or DSA per window
	EExplicitViewPort      = 0x00040000,
	};
	//flag accessors
	inline TBool ExplicitExtent() const {return iFlags&EExplicitExtent;}
	inline void SetExplicitExtent(TBool aSetting = ETrue) 
		{	iFlags= aSetting? iFlags|EExplicitExtent:iFlags&~EExplicitExtent;}
//Absolute extent functionality is currently not required or used
//	inline TBool AbsoluteExtent() const {return iFlags&EAbsoluteExtent;}
//	inline void SetAbsoluteExtent(TBool aSetting = ETrue) 
//		{	iFlags= aSetting? iFlags|EAbsoluteExtent:iFlags&~EAbsoluteExtent;}
	inline TBool ExplicitViewPort() const {return iFlags&EExplicitViewPort;}
    inline void SetExplicitViewPort(TBool aSetting = ETrue) 
        {   iFlags = aSetting ? iFlags|EExplicitViewPort : iFlags&~EExplicitExtent;}
};

class TPlacedAttributes : public TAttributes
{
public:
	enum
	{
	EUnassigned			= 0x00010000,
	ELockedConfig		= 0x00020000,
	EChangedClip	= 0x00040000,
	EOverlapping		= 0x00080000,
	};
	//flag accessors
	inline TBool Unassigned() const {return iFlags&EUnassigned	;}
	inline void SetUnassigned(TBool aSetting = ETrue) 
		{	iFlags= aSetting? iFlags|EUnassigned	:iFlags&~EUnassigned;	}
	inline TBool LockedConfig() const {return iFlags&ELockedConfig	;}
	inline void SetLockedConfig(TBool aSetting = ETrue) 
		{aSetting?iFlags|=ELockedConfig:iFlags&=~ELockedConfig;}
	inline TBool ChangedClip() const {return iFlags&EChangedClip;}
	inline void SetChangedClip(TBool aSetting = ETrue) 
		{aSetting?iFlags|=EChangedClip:iFlags&=~EChangedClip;}
	inline TBool Overlapping() const {return iFlags&EOverlapping;}
	inline void SetOverlapping(TBool aSetting = ETrue) 
		{	iFlags= aSetting? iFlags|EOverlapping:iFlags&~EOverlapping;	}
	inline void ClearRedrawFlags()
		{	iFlags &= (~(EUnassigned|ELockedConfig|EChangedClip|EOverlapping));	}
};

class CWindowElement : public CBase
{
friend class CWindowElementSet;
public:
	enum
	{
	EFastPath			= 0x00000001,
	EPauseComposition	= 0x00000002,	//assign flags this if pause is needed immediately
	EResort				= 0x00000004,
	EFailed				= 0x00010000,	//cleanup failure needs to be reported separately
	};
	
	CWindowElement(const CWsClientWindow& aWindow);
  
	TInt UnassignPlacedElements(const TRegion& aRegion,TInt aGcDrawingCount);
	void FlagAssignedElementsDrawnOver(TInt aGcDrawingCount);
	static TBool IsElementAMatch(MWsElement&,const TSurfaceConfiguration&,TInt);
	static TInt SetElement(MWsElement&,const TSurfaceConfiguration&,TBool);
	TBool InsertCache(TInt aIndexToInsert);
	static void NotifyReleasingSurface(TSurfaceId aSurfaceId);
	TInt AssignPlacedElement(TPlacedAttributes*& aPlacedAttributes, 
			const TSurfaceConfiguration& aSurfaceConfiguration,
			const TRegion& aUserDefinedRegion, MWsScene& aScene,TInt aGcDrawingCount);
	
	TInt CleanUpPlacedElements(MWsScene& aScene,TInt aGcDrawingCount);
	void MoveToAboveGivenElement(MWsScene& aScene, MWsElement* aAboveElement);
	void UpdateElementExtent(const TPoint* aOffset);
	TBool SetVisibleRegion(const TRegion& aRegion, MWsScene& aScene);
	TInt ReleaseAllElements();
	TBool SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId);
	void SetElementOpacity(TInt aOpacity, MWsScene& aScene);
	void ResubmitAllElementExtents();
	MWsElement* Element() const;

protected:
	TInt AssignRecycleElement(const TInt aIndex,
			const TSurfaceConfiguration& aSurfaceConfiguration,
			const TRegion& aUserDefinedRegion,
			MWsScene& aScene);
	TInt AssignCreateElement(const TSurfaceConfiguration& aSurfaceConfiguration,
			const TRegion& aUserDefinedRegion,
			MWsScene& aScene);
	TInt AssignExtendElement(const TInt aIndex,
			const TSurfaceConfiguration& aSurfaceConfiguration,
			const TRegion& aUserDefinedRegion,
			MWsScene& aScene);
	
public:
	const CWsClientWindow& iWindow;
private:
	TBackgroundAttributes iBackgroundElement;
	RArray<TPlacedAttributes> iPlacedElements;
private:	//The following are all exclusively used during window rendering
	RArray<RRegion> iCache;
	//memory of surfaces that need to be removed
	RArray<TSurfaceId> iRemovedSurfaces;
	TBool iRemovedSurfacesValid;

	RRegion iNonRedrawVisibleRegion;
	TInt iHighestReusedIndex;
	TInt iStartGcDrawingCount;
	TUint iFlags;
};

/**
Owns the set of elements and their associated identification information. Only
references are provided for clients.
*/
class CWindowElementSet : public CBase
{
public:
	static CWindowElementSet* NewL(MWsScene& aScene);
	~CWindowElementSet();
	TBackgroundAttributes& AcquireBackgroundElementL(CWsClientWindow& aWindow);
	TInt ReleaseBackgroundElement(const CWsClientWindow&, TBool aWindowClosing = EFalse);
	TInt ReleaseAllElements(const CWsClientWindow&, TBool aWindowClosing = EFalse);
	TInt UnassignPlacedElements(const TRegion& aRedrawRegion, const CWsClientWindow& aWindow,TInt aGcDrawingCount);
	TInt AssignPlacedElement(TPlacedAttributes*&, const TSurfaceConfiguration&, const TRegion& aUserDefinedRegion,
			const CWsClientWindow&,TInt aGcDrawingCount);
	TInt CleanUpPlacedElements(const CWsClientWindow& aWindow,TInt aGcDrawingCount);
	TInt Count() const;
	TBackgroundAttributes* FindBackgroundElement(const CWsClientWindow&);	
	TBool FindElements(const CWsClientWindow&,const TBackgroundAttributes*&,const RArray<TPlacedAttributes>*&);	
	TInt RegisterSurface(const TSurfaceId& aSurface);
	void UnregisterSurface(const TSurfaceId& aSurface);
	TBool SortByZOrder();
	TInt SetVisibleRegion(CWsClientWindow& aWindow);	//cant be a const win as VisibleRegion() isnt const??
	void UpdateElementExtent(const CWsClientWindow&, const TPoint* aOffset);
	static TInt GetConfiguration(TSurfaceConfiguration&,MWsElement&);
	TBool SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId);
	void SetElementOpacity(CWsClientWindow& aWindow, TInt aOpacity);
	void ResubmitAllElementExtents();
	MWsElement* GetElementFromWindow(const CWsClientWindow& aWindow) const;
	
public:
	const CWsClientWindow* DebugWindowAt(TUint aWin)const;
	const TBackgroundAttributes* DebugBackgroundAt(TUint aWin)const;
	const TPlacedAttributes* DebugPlacedAt(TUint aWin,TUint aPlace)const;
//Note: The following methods are currently unused
//	const TPlacedAttributes* DebugPlacedAt(const CWsClientWindow& aWindow,TInt aPlace)const;
//	const RRegion* DebugCacheAt(TInt aWin,TInt aPlace)const;
	TInt DebugPlacedCountAt(TUint aWin)const; 

protected:
	CWindowElement* FindElement(const CWsClientWindow& aWindow) const;

private:
	CWindowElementSet(MWsScene& aScene);
	TInt FindEntry(const CWsClientWindow& aWindow, TInt& aIndex, TBool aLinearSearch = EFalse) const;
	static TBool WindowMatch(const CWindowElement& aFirst, const CWindowElement& aSecond);
	static TInt WindowOrder(const CWindowElement& aFirst, const CWindowElement& aSecond);
	void DestroyWindowElementEntry(const TInt aWinElementIndex);

private:
	RPointerArray<CWindowElement> iElements;
	MWsScene& iScene;
};

/** Gc to element rotation function
*/
inline MWsElement::TElementRotation GcToElementRotation(CFbsBitGc::TGraphicsOrientation aGcRotation)
	{
	MWsElement::TElementRotation elementRotation = MWsElement::EElementAntiClockwise0;
	
	switch (aGcRotation)
		{
		case CFbsBitGc::EGraphicsOrientationRotated90:
			elementRotation = MWsElement::EElementAntiClockwise90;
			break;
		case CFbsBitGc::EGraphicsOrientationRotated180:
			elementRotation = MWsElement::EElementAntiClockwise180;
			break;
		case CFbsBitGc::EGraphicsOrientationRotated270:
			elementRotation = MWsElement::EElementAntiClockwise270;
			break;
		default:
			break;
		}
	return elementRotation;
	}

#endif // !defined(__WINDOWELEMENTSET_H__)
