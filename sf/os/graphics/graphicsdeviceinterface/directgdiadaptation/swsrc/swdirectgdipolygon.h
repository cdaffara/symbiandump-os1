// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SWDIRECTGDIPOLYGON_H
#define SWDIRECTGDIPOLYGON_H

/**
@file
@internalComponent
*/

#include "directgdiadapter.h"
#include <gdi.h>
#include <e32base.h>

/**
A utility class to efficiently fill polygons.
@see CSwDirectGdiEngine::PolyFill
@see CSwDirectGdiEngine::PolyFillLarge
*/
NONSHARABLE_CLASS(CSwDirectGdiPolygonFiller) : public CBase
	{
public:

	/**
	Describes how pixels are to be displayed in the polygon. aUsage should be set to
	one of these values before CSwDirectGdiPolygonFiller::Construct is used.
	*/
	enum TUsage
		{
		/**
		A request for all pixel runs in sequential order.
		*/
		EGetAllPixelRunsSequentially,
		/**
		A request for all pixel runs in sequential order but only for specified lines.
		*/
		EGetPixelRunsSequentiallyForSpecifiedScanLines
		};
public:
	CSwDirectGdiPolygonFiller();
	~CSwDirectGdiPolygonFiller();
	void Construct(const TArray<TPoint>* aPointArray, DirectGdi::TFillRule aFillRule,TUsage aUsage=EGetAllPixelRunsSequentially); // N.B. this cannot fail
	void Construct(const TPoint* aPointList,TInt aNumPoints, DirectGdi::TFillRule aFillRule, TUsage aUsage=EGetAllPixelRunsSequentially); // N.B. this cannot fail
	void Reset();
	void GetNextPixelRun(TBool& aExists, TInt& aScanLine, TInt& aStart, TInt& aEnd);
	void GetNextPixelRunOnSpecifiedScanLine(TBool& aExists, TInt aScanLine, TInt& aStart, TInt& aEnd);
private: // data-types for the fast algorithm
	struct SFastEdge
		{
		TInt upperVertex;
		TInt lowerVertex;
		TInt firstVertex;
		};
	struct SFastScanLineIntersection;
	struct SFastActiveEdge
		{
		SFastEdge* edgePtr;
		TLinearDDA lineGenerator;
		SFastScanLineIntersection* scanLineIntersectionPtr;
		};
	struct SFastScanLineIntersection
		{
		TInt firstPixel;
		TInt lastPixel;
		SFastActiveEdge* activeEdgePtr;
		};
private: // data-types for the slow algorithm
	struct SSlowScanLineIntersection
		{
		TInt firstPixel;
		TInt lastPixel;
		TInt firstVertexOfEdge;
		};
private: // data-types for both algorithms
	struct SFastData
		{
		TPoint* vertexList;
		SFastEdge* edgeList;
		SFastActiveEdge* activeEdgeList;
		SFastScanLineIntersection* scanLineIntersectionList;
		TInt numActiveEdges;
		TInt numScanLineIntersections;
		TInt nextEdgeToActivate;
		};
	struct SSlowData
		{
		enum {EStoreSize=8};
		TLinearDDA lineGenerator;
		SSlowScanLineIntersection scanLineIntersectionList[EStoreSize];
		TInt numIntersectionsWithSameFirstPixelPreviouslyMet;
		TInt numIntersectionsWithSameFirstPixelMetThisTime;
		TInt numScanLineIntersections;
		TBool scanLineComplete;
		TInt firstPixelOfLastIntersectionInPrevBuffer;
		};
private:
	void Construct(DirectGdi::TFillRule aFillRule, TUsage aUsage);
	void FastHandleVertexIntersection(TInt& aCurrentActiveEdge, TBool aIsLowerVertex);
	void SetFastIntersection(SFastActiveEdge& aActiveEdge, SFastScanLineIntersection& aScanLineIntersection);
	void SlowHandleVertexIntersection(SSlowScanLineIntersection& aScanLineIntersection, TInt& aVertexStartingCurrentEdge,TBool aIsLowerVertex);
	void JumpToCurrentScanLine(TLinearDDA& aLineGenerator, const TPoint& aUpper, const TPoint& aLower,TPoint& aStartPos, TPoint& aEndPos) const;
	const TPoint& Point(TInt aIndex);
private:
	const TArray<TPoint>* iPointArray; // not owned by the class
	DirectGdi::TFillRule iFillRule;
	TBool iUseFastAlgorithm;
	TInt iNumVertexes;
	TBool iToggler; // used by EAlternate fill-rule
	TInt iNestingLevel; // used by EWinding fill-rule
	TInt iScanLineIntersection;
	TInt iRightMostPixelOnScanLine;
	TInt iFirstVertex;
	TBool iPolygonIsAllHorizontal;
	TInt iFirstScanLine;
	TInt iLastScanLine;
	TInt iCurrentScanLine;
	SFastData iFastData;
	SSlowData iSlowData;
private:
	friend class TCompareEdgesUpperY;
	friend class TCompareActiveEdgesFirstVertex;
	friend class TCompareScanLineIntersectionsFirstPixel;
	friend class TSwapEdges;
	friend class TSwapActiveEdges;
	friend class TSwapScanLineIntersections;
	};

#endif /*SWDIRECTGDIPOLYGON_H*/
	
