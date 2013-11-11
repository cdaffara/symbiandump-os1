// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitstd.h>
#include "BITPANIC.H"

// TCompareEdgesUpperY

NONSHARABLE_CLASS(TCompareEdgesUpperY) : public TKey
	{
public:
	TCompareEdgesUpperY(const CPolygonFiller::SFastData& aFastData);
private:
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
private:
	const CPolygonFiller::SFastData& iFastData;
	};

TCompareEdgesUpperY::TCompareEdgesUpperY(const CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

TInt TCompareEdgesUpperY::Compare(TInt aLeft,TInt aRight) const
	{
	const TInt leftUpperY=iFastData.vertexList[iFastData.edgeList[aLeft].upperVertex].iY;
	const TInt rightUpperY=iFastData.vertexList[iFastData.edgeList[aRight].upperVertex].iY;
	if (leftUpperY<rightUpperY)
		return -1;
	if (leftUpperY>rightUpperY)
		return 1;
	return 0;
	}

// TSwapEdges

NONSHARABLE_CLASS(TSwapEdges) : public TSwap
	{
public:
	TSwapEdges(CPolygonFiller::SFastData& aFastData);
private:
	virtual void Swap(TInt aLeft,TInt aRight) const;
private:
	CPolygonFiller::SFastData& iFastData;
	};

TSwapEdges::TSwapEdges(CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

void TSwapEdges::Swap(TInt aLeft,TInt aRight) const
	{
	CPolygonFiller::SFastEdge& leftEdge=iFastData.edgeList[aLeft];
	CPolygonFiller::SFastEdge& rightEdge=iFastData.edgeList[aRight];

	const CPolygonFiller::SFastEdge temp(leftEdge);
	leftEdge=rightEdge;
	rightEdge=temp;
	}

// TCompareActiveEdgesFirstVertex

NONSHARABLE_CLASS(TCompareActiveEdgesFirstVertex) : public TKey
	{
public:
	TCompareActiveEdgesFirstVertex(const CPolygonFiller::SFastData& aFastData);
private:
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
private:
	const CPolygonFiller::SFastData& iFastData;
	};

TCompareActiveEdgesFirstVertex::TCompareActiveEdgesFirstVertex(const CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

TInt TCompareActiveEdgesFirstVertex::Compare(TInt aLeft,TInt aRight) const
	{
	const TInt leftFirstVertex=iFastData.activeEdgeList[aLeft].edgePtr->firstVertex;
	const TInt rightFirstVertex=iFastData.activeEdgeList[aRight].edgePtr->firstVertex;
	if (leftFirstVertex<rightFirstVertex)
		return -1;
	if (leftFirstVertex>rightFirstVertex)
		return 1;
	return 0;
	}

// TSwapActiveEdges

NONSHARABLE_CLASS(TSwapActiveEdges) : public TSwap
	{
public:
	TSwapActiveEdges(CPolygonFiller::SFastData& aFastData);
private:
	virtual void Swap(TInt aLeft,TInt aRight) const;
private:
	CPolygonFiller::SFastData& iFastData;
	};

TSwapActiveEdges::TSwapActiveEdges(CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

void TSwapActiveEdges::Swap(TInt aLeft,TInt aRight) const
	{
	CPolygonFiller::SFastActiveEdge& leftActiveEdge=iFastData.activeEdgeList[aLeft];
	CPolygonFiller::SFastActiveEdge& rightActiveEdge=iFastData.activeEdgeList[aRight];

	const CPolygonFiller::SFastActiveEdge temp(leftActiveEdge);
	leftActiveEdge=rightActiveEdge;
	rightActiveEdge=temp;

	if (leftActiveEdge.scanLineIntersectionPtr!=NULL)
		leftActiveEdge.scanLineIntersectionPtr->activeEdgePtr=&leftActiveEdge;
	if (rightActiveEdge.scanLineIntersectionPtr!=NULL)
		rightActiveEdge.scanLineIntersectionPtr->activeEdgePtr=&rightActiveEdge;
	}

// TCompareScanLineIntersectionsFirstPixel

NONSHARABLE_CLASS(TCompareScanLineIntersectionsFirstPixel) : public TKey
	{
public:
	TCompareScanLineIntersectionsFirstPixel(const CPolygonFiller::SFastData& aFastData);
private:
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
private:
	const CPolygonFiller::SFastData& iFastData;
	};

TCompareScanLineIntersectionsFirstPixel::TCompareScanLineIntersectionsFirstPixel(const CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

TInt TCompareScanLineIntersectionsFirstPixel::Compare(TInt aLeft,TInt aRight) const
	{
	const TInt leftFirstPixel=iFastData.scanLineIntersectionList[aLeft].firstPixel;
	const TInt rightFirstPixel=iFastData.scanLineIntersectionList[aRight].firstPixel;
	if (leftFirstPixel<rightFirstPixel)
		return -1;
	if (leftFirstPixel>rightFirstPixel)
		return 1;
	return 0;
	}

// TSwapScanLineIntersections

NONSHARABLE_CLASS(TSwapScanLineIntersections) : public TSwap
	{
public:
	TSwapScanLineIntersections(CPolygonFiller::SFastData& aFastData);
private:
	virtual void Swap(TInt aLeft,TInt aRight) const;
private:
	CPolygonFiller::SFastData& iFastData;
	};

TSwapScanLineIntersections::TSwapScanLineIntersections(CPolygonFiller::SFastData& aFastData):
	iFastData(aFastData)
	{
	}

void TSwapScanLineIntersections::Swap(TInt aLeft,TInt aRight) const
	{
	CPolygonFiller::SFastScanLineIntersection& leftScanLineIntersection=iFastData.scanLineIntersectionList[aLeft];
	CPolygonFiller::SFastScanLineIntersection& rightScanLineIntersection=iFastData.scanLineIntersectionList[aRight];

	const CPolygonFiller::SFastScanLineIntersection temp(leftScanLineIntersection);
	leftScanLineIntersection=rightScanLineIntersection;
	rightScanLineIntersection=temp;

	leftScanLineIntersection.activeEdgePtr->scanLineIntersectionPtr=&leftScanLineIntersection;
	rightScanLineIntersection.activeEdgePtr->scanLineIntersectionPtr=&rightScanLineIntersection;
	}

// the sorting function

LOCAL_C void Sort(TInt aCount,const TKey& aKey,const TSwap& aSwap)
	{
#if 1 // quick sort
	const TInt error=User::QuickSort(aCount,aKey,aSwap);
	BG_ASSERT_ALWAYS_INVARIANT(error==KErrNone);
#elif 0 // bubble sort
	for (TInt i=1; i<aCount; ++i)
		{
		for (TInt j=i; j>0; --j)
			{
			if (aKey.Compare(j-1,j)>0)
				{
				aSwap.Swap(j-1,j);
				}
			}
		}
#else // heap sort
	TInt startOfSortedPortion=aCount;
	if (startOfSortedPortion>1)
		{
		TInt startOfHeap=startOfSortedPortion>>1;
		FOREVER
			{
			BG_ASSERT_DEBUG_INVARIANT(startOfHeap>=0);
			if (startOfHeap!=0)
				{
				--startOfHeap;
				}
			else
				{
				--startOfSortedPortion;
				aSwap.Swap(startOfSortedPortion,0);
				BG_ASSERT_DEBUG_INVARIANT(startOfSortedPortion>=1);
				if (startOfSortedPortion==1)
					{
					break;
					}
				}
			// put aArray[startOfHeap] into the correct place in the heap
			TInt i=startOfHeap;
			FOREVER
				{
				TInt j=(i+1)<<1;
				if ((j>=startOfSortedPortion) || (aKey.Compare(j-1,j)>0))
					{
					--j;
					}
				if ((j>=startOfSortedPortion) || (aKey.Compare(i,j)>=0))
					{
					break;
					}
				aSwap.Swap(i,j);
				i=j;
				}
			}
		}
#endif
#if defined(_DEBUG)
	{
	for (TInt i=1; i<aCount; ++i)
		{
		BG_ASSERT_DEBUG_INVARIANT(aKey.Compare(i-1,i)<=0);
		}
	}
#endif
	}

/*
 CPolygonFiller
 */
 
/**
 Constructor which initializes all member data to zero, EFalse or null for 
 TInt, TBool pointers respectively.
 */
EXPORT_C CPolygonFiller::CPolygonFiller():
	CBase(),
	iPointArray(NULL),
	iPointList(NULL),
	iFillRule(CGraphicsContext::EAlternate),
	iUseFastAlgorithm(EFalse),
	iNumVertexes(0),
	iToggler(EFalse),
	iNestingLevel(0),
	iScanLineIntersection(0),
	iRightMostPixelOnScanLine(0),
	iFirstVertex(0),
	iPolygonIsAllHorizontal(EFalse),
	iFirstScanLine(0),
	iLastScanLine(0),
	iCurrentScanLine(0)
	{
	iFastData.vertexList=NULL;
	iFastData.edgeList=NULL;
	iFastData.activeEdgeList=NULL;
	iFastData.scanLineIntersectionList=NULL;
	iFastData.numActiveEdges=0;
	iFastData.numScanLineIntersections=0;
	iFastData.nextEdgeToActivate=0;
	iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet=0;
	iSlowData.numIntersectionsWithSameFirstPixelMetThisTime=0;
	iSlowData.numScanLineIntersections=0;
	iSlowData.scanLineComplete=EFalse;
	iSlowData.firstPixelOfLastIntersectionInPrevBuffer=0;
	}

/**
Destructor calls reset on the polygon.
*/
EXPORT_C CPolygonFiller::~CPolygonFiller()
	{
	Reset();
	}

/**
Takes a list of points to be the points for the new polygon and sets the number of points in the shape. 
After this has been done it transfers the task to <code>Construct(aFillRule,aUsage)</code>. This should not fail.
@param aPointList A list of points for the polygon. 
@param aNumPoints The number of points in the list.
@param aFillRule How filling should be achieved, as described by a CGraphicsContext::TFillRule object.
@param aUsage How the polygon should be used, see TUsage enumeration.
*/
EXPORT_C void CPolygonFiller::Construct(const TPoint* aPointList,TInt aNumPoints, 
										CGraphicsContext::TFillRule aFillRule, TUsage aUsage)
	{
	BG_ASSERT_ALWAYS(aPointList!=NULL,EBitgdiPanicPolygonFiller);
	iPointList=aPointList;
	iNumVertexes=aNumPoints;
	Construct(aFillRule,aUsage);
	}

/**
An overloaded version of Construct which allows the list of points to be passed in as a point array.
Exactly the same behaviour and structure as above. This should not fail. This method does not require the 
number of nodes to be given as a parameter.
@param PointArray A pointer to point array, as opposed to a pointer to a point list.
@param aFillRule How filling should be achieved, as described by a CGraphicsContext::TFillRule object.
@param aUsage How the polygon should be used.
*/
EXPORT_C void CPolygonFiller::Construct(const CArrayFix<TPoint>* aPointArray, 
										CGraphicsContext::TFillRule aFillRule, TUsage aUsage)
	{
	BG_ASSERT_ALWAYS(aPointArray!=NULL,EBitgdiPanicPolygonFiller);
	iPointArray=aPointArray;
	iNumVertexes=iPointArray->Count();
	Construct(aFillRule,aUsage);
	}

void CPolygonFiller::Construct(CGraphicsContext::TFillRule aFillRule, TUsage aUsage)
	{
	BG_ASSERT_ALWAYS((aFillRule==CGraphicsContext::EAlternate) || (aFillRule==CGraphicsContext::EWinding),
																					EBitgdiPanicPolygonFiller);
	BG_ASSERT_ALWAYS((aUsage==EGetAllPixelRunsSequentially) || (aUsage==EGetPixelRunsSequentiallyForSpecifiedScanLines),
																					EBitgdiPanicPolygonFiller);
	TInt i, j;
	iFillRule=aFillRule;
	iUseFastAlgorithm=(aUsage==EGetAllPixelRunsSequentially);
	iToggler=EFalse;
	iNestingLevel=0;
	iScanLineIntersection=0;
	iRightMostPixelOnScanLine=KMinTInt;
	// find the first vertex and see if the polygon is all horizontal
	iFirstVertex=0; // dummy default value
	iPolygonIsAllHorizontal=ETrue;

	if (iNumVertexes==0)
		return;

	for (i=0; i<iNumVertexes; ++i)
		if (Point(i).iY!=Point((i+1)%iNumVertexes).iY)
			{
			// i is now set to the vertex before the first non-horizontal edge

			// set j%iNumVertexes to the vertex before the next non-horizontal edge
			for (j=i+1; Point(j%iNumVertexes).iY==Point((j+1)%iNumVertexes).iY; ++j)
				;
			j%=iNumVertexes;
			TInt first=Point(i).iY;
			TInt middle=Point(j).iY;
			TInt last=Point((j+1)%iNumVertexes).iY;

			// if vertex j is a max or min point, set the first-vertex to be j
			if ((middle<first)==(middle<last))
				{
				iFirstVertex=j;
				iPolygonIsAllHorizontal=EFalse;
				break;
				}
			}

	if (iUseFastAlgorithm)
		{
		iFastData.vertexList=(TPoint*)User::Alloc(sizeof(TPoint)*iNumVertexes);
		iFastData.edgeList=(SFastEdge*)User::Alloc(sizeof(SFastEdge)*iNumVertexes);
		iFastData.activeEdgeList=(SFastActiveEdge*)User::Alloc(sizeof(SFastActiveEdge)*iNumVertexes);
		iFastData.scanLineIntersectionList=(SFastScanLineIntersection*)User::Alloc(sizeof(SFastScanLineIntersection)*iNumVertexes);
		if ((iFastData.vertexList==NULL) ||
			(iFastData.edgeList==NULL) ||
			(iFastData.activeEdgeList==NULL) ||
			(iFastData.scanLineIntersectionList==NULL))
			{
			Reset(); // sets iUseFastAlgorithm to EFalse among other things
			}
		}

	if (iUseFastAlgorithm)
		{
		for(TInt vertexcount=0;vertexcount<iNumVertexes;vertexcount++)
			new(&iFastData.activeEdgeList[vertexcount]) SFastActiveEdge;
		iFastData.numActiveEdges=0;
		iFastData.numScanLineIntersections=0;
		iFastData.nextEdgeToActivate=0;

		// put the points into the vertex-list
		// N.B. this array is uesd for speed since CArrayXxxs are slower for indexing into than built-in arrays
		for (i=0; i<iNumVertexes; ++i)
			iFastData.vertexList[i]=Point((i+iFirstVertex)%iNumVertexes);

		// create edge-list
		for (i=0; i<iNumVertexes; ++i)
			{
			if (iFastData.vertexList[i].iY<iFastData.vertexList[(i+1)%iNumVertexes].iY)
				{
				iFastData.edgeList[i].upperVertex=i;
				iFastData.edgeList[i].lowerVertex=(i+1)%iNumVertexes;
				}
			else
				{
				iFastData.edgeList[i].upperVertex=(i+1)%iNumVertexes;
				iFastData.edgeList[i].lowerVertex=i;
				}
			iFastData.edgeList[i].firstVertex=i;
			}

		// sort edge-list into order of increasing upper y-position
		Sort(iNumVertexes,TCompareEdgesUpperY(iFastData),TSwapEdges(iFastData));

		// find the first scan-line
		iFirstScanLine=iFastData.vertexList[iFastData.edgeList[0].upperVertex].iY;

		// find the last scan-line
		iLastScanLine=iFirstScanLine;
		for (i=0; i<iNumVertexes; ++i)
			if (iLastScanLine<iFastData.vertexList[i].iY)
				iLastScanLine=iFastData.vertexList[i].iY;
		}
	else
		{
		iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet=0;
		iSlowData.scanLineComplete=EFalse;
		iSlowData.firstPixelOfLastIntersectionInPrevBuffer=KMinTInt;

		// find the first and last scan-lines
		iFirstScanLine=KMaxTInt;
		iLastScanLine=KMinTInt;
		for (i=0; i<iNumVertexes; ++i)
			{
			TInt y=Point(i).iY;
			if (iFirstScanLine>y)
				iFirstScanLine=y;
			if (iLastScanLine<y)
				iLastScanLine=y;
			}
		}

	iCurrentScanLine=iFirstScanLine;
	}

/**
Frees any data held in the polygons lists of all edges, vertexs and scan lines and sets these values to NULL.
It also has the feature of setting iUseFastAlgorithm = EFalse.
*/
EXPORT_C void CPolygonFiller::Reset()
	{
	if(iUseFastAlgorithm)
		{
		if(iFastData.vertexList)
			{
			User::Free(iFastData.vertexList);
			iFastData.vertexList=NULL;
			}
		if(iFastData.edgeList)
			{
			User::Free(iFastData.edgeList);
			iFastData.edgeList=NULL;
			}
		if(iFastData.activeEdgeList)
			{
			User::Free(iFastData.activeEdgeList);
			iFastData.activeEdgeList=NULL;
			}
		if(iFastData.scanLineIntersectionList)
			{
			User::Free(iFastData.scanLineIntersectionList);
			iFastData.scanLineIntersectionList=NULL;
			}
		iUseFastAlgorithm=EFalse;
		}
	}

/**
Method is used to calculate the locations of vertex interactions between the polygon and scan lines. 
An initial scan line is required. It calculates the start and end positions on the line. The method
can use either the fast or slow polygon algorithm depending upon the state of aUsage. Polygon filling
is also addressed by this method.
@param aExists Will be set to false if a polygon with no vertexes is passed in, otherwise ETrue on return.
@param aScanline On return will contain iScanline at the beginning of the operation.
@param aStart The position on the scan line to start the run, on returned.
@param aEnd The position on the scan line to end the run, returned.
*/
EXPORT_C void CPolygonFiller::GetNextPixelRun(TBool& aExists, TInt& aScanLine, TInt& aStart, 
											  TInt& aEnd)
	{
	if (iNumVertexes==0 || iCurrentScanLine>iLastScanLine)
		{
		aExists=EFalse;
		return;
		}

	aExists=ETrue;
	aScanLine=iCurrentScanLine;

	if (iPolygonIsAllHorizontal)
		{
		// set the start after the end
		aStart=KMinTInt+1;
		aEnd=KMinTInt;
		++iCurrentScanLine;
		return;
		}

	if (iUseFastAlgorithm)
		{
		TInt i, j;

		if (iScanLineIntersection==0)
			{
			// add any new edges to the active-edge-list
			for (; (iFastData.nextEdgeToActivate<iNumVertexes) &&
					(iFastData.vertexList[iFastData.edgeList[iFastData.nextEdgeToActivate].upperVertex].iY==iCurrentScanLine);
					++iFastData.numActiveEdges, ++iFastData.nextEdgeToActivate)
				{
				iFastData.activeEdgeList[iFastData.numActiveEdges].edgePtr=&iFastData.edgeList[iFastData.nextEdgeToActivate];
				iFastData.activeEdgeList[iFastData.numActiveEdges].lineGenerator.Construct(
														iFastData.vertexList[iFastData.edgeList[iFastData.nextEdgeToActivate].upperVertex],
														iFastData.vertexList[iFastData.edgeList[iFastData.nextEdgeToActivate].lowerVertex]);
				iFastData.activeEdgeList[iFastData.numActiveEdges].scanLineIntersectionPtr=NULL;
				}

			// sort the active-edge-list into order of adjacent edges (if possible)
			Sort(iFastData.numActiveEdges,TCompareActiveEdgesFirstVertex(iFastData),TSwapActiveEdges(iFastData));

			// find the intersection of each active-edge with the current scan-line
			// for max/min vertex-runs (e.g. \/, \_/, \__/, etc.) add 2 intersections for each run
			// for other vertex-runs (e.g. /----/) add 1 intersection for each run
			for (i=0; i<iFastData.numActiveEdges; ++i)
				{
				// check that active-edge i is not horizontal
				BG_ASSERT_DEBUG(iFastData.vertexList[iFastData.activeEdgeList[i].edgePtr->upperVertex].iY!=
							   iFastData.vertexList[iFastData.activeEdgeList[i].edgePtr->lowerVertex].iY, EBitgdiPanicPolygonFiller);

				if (iFastData.vertexList[iFastData.activeEdgeList[i].edgePtr->upperVertex].iY==iCurrentScanLine)
					// the scan-line is intersecting active-edge i at its upper-vertex
					FastHandleVertexIntersection(i, EFalse);
				else if (iFastData.vertexList[iFastData.activeEdgeList[i].edgePtr->lowerVertex].iY==iCurrentScanLine)
					// the scan-line is intersecting active-edge i at its lower-vertex
					FastHandleVertexIntersection(i, ETrue);
				else
					// the scan-line is intersecting active-edge i at neither of its vertices
					SetFastIntersection(iFastData.activeEdgeList[i],*iFastData.activeEdgeList[i].scanLineIntersectionPtr);
				}

			// N.B. iFastData.numScanLineIntersections is less than or equal to iFastData.numActiveEdges

			// sort the intersection-list into increasing order of first-pixel
			Sort(iFastData.numScanLineIntersections,TCompareScanLineIntersectionsFirstPixel(iFastData),TSwapScanLineIntersections(iFastData));

			BG_ASSERT_DEBUG(iFastData.numScanLineIntersections>=2, EBitgdiPanicPolygonFiller);
			}

		// depending on the rule used, find the pixel-run
		TBool doFill=EFalse; // dummy initialization to prevent compiler warning
		if (iScanLineIntersection<iFastData.numScanLineIntersections-1)
			{
			switch (iFillRule)
				{
			case CGraphicsContext::EAlternate:
				iToggler=!iToggler;
				doFill=iToggler;
				break;
			case CGraphicsContext::EWinding:
					BG_ASSERT_DEBUG(iFastData.vertexList[iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->lowerVertex].iY!=
								   iFastData.vertexList[iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->upperVertex].iY,
																										EBitgdiPanicPolygonFiller);

					if (iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->lowerVertex==
						(iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->upperVertex+1)%iNumVertexes)
						++iNestingLevel;
					else
						--iNestingLevel;

				doFill=(iNestingLevel!=0);
				break;
				}

			if (doFill)
				{
				aStart=Max(iRightMostPixelOnScanLine, iFastData.scanLineIntersectionList[iScanLineIntersection].lastPixel)+1;
				aEnd=iFastData.scanLineIntersectionList[iScanLineIntersection+1].firstPixel-1;
				}
			else
				{
				// set the start after the end
				aStart=KMinTInt+1;
				aEnd=KMinTInt;
				}

			if (iRightMostPixelOnScanLine<iFastData.scanLineIntersectionList[iScanLineIntersection].lastPixel)
				iRightMostPixelOnScanLine=iFastData.scanLineIntersectionList[iScanLineIntersection].lastPixel;
			++iScanLineIntersection;
			}

		if (iScanLineIntersection==iFastData.numScanLineIntersections-1)
			{
			BG_ASSERT_DEBUG(iFastData.vertexList[iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->lowerVertex].iY!=
						   iFastData.vertexList[iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->upperVertex].iY,
																											EBitgdiPanicPolygonFiller);

			switch (iFillRule)
				{
			case CGraphicsContext::EAlternate:
				iToggler=!iToggler;
				BG_ASSERT_DEBUG(iToggler==0, EBitgdiPanicPolygonFiller);
				break;
			case CGraphicsContext::EWinding:
				if (iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->lowerVertex==
					(iFastData.scanLineIntersectionList[iScanLineIntersection].activeEdgePtr->edgePtr->upperVertex+1)%iNumVertexes)
					++iNestingLevel;
				else
					--iNestingLevel;
				BG_ASSERT_DEBUG((iNumVertexes==2) || (iNestingLevel==0), EBitgdiPanicPolygonFiller);
				break;
				}

			// remove any scan-line-intersections associated with old active-edges
			for (i=0; i<iFastData.numScanLineIntersections; )
				if (iFastData.vertexList[iFastData.scanLineIntersectionList[i].activeEdgePtr->edgePtr->lowerVertex].iY==iCurrentScanLine)
					{
					iFastData.scanLineIntersectionList[i].activeEdgePtr->scanLineIntersectionPtr=NULL;

					// ripple all the entries in the scan-line-intersection-list after this one back one place
					for (j=i+1; j<iFastData.numScanLineIntersections; ++j)
						{
						iFastData.scanLineIntersectionList[j-1]=iFastData.scanLineIntersectionList[j];
						iFastData.scanLineIntersectionList[j-1].activeEdgePtr->scanLineIntersectionPtr=&iFastData.scanLineIntersectionList[j-1];
						}

					iFastData.scanLineIntersectionList[j-1].activeEdgePtr=NULL;
					--iFastData.numScanLineIntersections;
					}
				else
					++i;

			// remove any old edges from the active-edge-list
			for (i=0; i<iFastData.numActiveEdges; )
				if (iFastData.vertexList[iFastData.activeEdgeList[i].edgePtr->lowerVertex].iY==iCurrentScanLine)
					{
					BG_ASSERT_DEBUG(iFastData.activeEdgeList[i].scanLineIntersectionPtr==NULL, EBitgdiPanicPolygonFiller);

					// ripple all the entries in the active-edge-list after this one back one place
					for (j=i+1; j<iFastData.numActiveEdges; ++j)
						{
						iFastData.activeEdgeList[j-1]=iFastData.activeEdgeList[j];
						if (iFastData.activeEdgeList[j-1].scanLineIntersectionPtr)
							iFastData.activeEdgeList[j-1].scanLineIntersectionPtr->activeEdgePtr=&iFastData.activeEdgeList[j-1];
						}

					iFastData.activeEdgeList[j-1].scanLineIntersectionPtr=NULL;
					--iFastData.numActiveEdges;
					}
				else
					++i;

#if defined(_DEBUG)
			for (i=0; i<iFastData.numActiveEdges; ++i)
				{
				BG_ASSERT_DEBUG(iFastData.activeEdgeList[i].scanLineIntersectionPtr->activeEdgePtr==
								&iFastData.activeEdgeList[i], EBitgdiPanicPolygonFiller);
				}

			for (i=0; i<iFastData.numScanLineIntersections; ++i)
				{
				BG_ASSERT_DEBUG(iFastData.scanLineIntersectionList[i].activeEdgePtr->scanLineIntersectionPtr==
								&iFastData.scanLineIntersectionList[i], EBitgdiPanicPolygonFiller);
				}
#endif

			iScanLineIntersection=0;
			++iCurrentScanLine;
			iRightMostPixelOnScanLine=KMinTInt;
			}
		}
	else
		{
		GetNextPixelRunOnSpecifiedScanLine(aExists, iCurrentScanLine, aStart, aEnd);
		if (!aExists)
			GetNextPixelRunOnSpecifiedScanLine(aExists, ++iCurrentScanLine, aStart, aEnd);
		}
	}

/**
Similar to GetNextPixelRun(aExists, aScanLine, aStart, aEnd) this method is used to draw the relevant 
vertex intersections for a polygon but only for an individual specified scan line. The method
can use either the fast or slow polygon algorithm depending upon the state of aUsage.
@param aExists Will be set to false if the line does not pass through the polygon or if
a polygon with no vertices is specified, otherwise ETrue on return.
@param aScanline The scan line to be drawn on. Used to set iScanline
@param aStart The position on the scan line to start the run, on returned.
@param aEnd The position on the scan line to end the run, returned.
*/
EXPORT_C void CPolygonFiller::GetNextPixelRunOnSpecifiedScanLine(TBool& aExists, 
																 TInt aScanLine, 
																 TInt& aStart, 
																 TInt& aEnd)
	{
	TInt i, j, k;

	BG_ASSERT_DEBUG(!iUseFastAlgorithm, EBitgdiPanicPolygonFiller);

	if (iNumVertexes==0 || aScanLine<iCurrentScanLine || aScanLine>iLastScanLine)
		{
		aExists=EFalse;
		return;
		}

	aExists=ETrue;
	iCurrentScanLine=aScanLine;

	if (iPolygonIsAllHorizontal)
		{
		// set the start after the end
		aStart=KMinTInt+1;
		aEnd=KMinTInt;
		++iCurrentScanLine;
		return;
		}

	if (iScanLineIntersection==0)
		{
		iSlowData.numIntersectionsWithSameFirstPixelMetThisTime=0;
		iSlowData.numScanLineIntersections=0;
		iSlowData.scanLineComplete=ETrue;

		// find the left-most iSlowData::EStoreSize number (or less) of intersections with this scan-line
		for (i=iFirstVertex; i<iNumVertexes+iFirstVertex; ++i)
			{
			TPoint upper=Point(i%iNumVertexes);
			TPoint lower=Point((i+1)%iNumVertexes);
			if (upper.iY>lower.iY)
				{
				TPoint temp=upper;
				upper=lower;
				lower=temp;
				}

			if ((iCurrentScanLine>=upper.iY) && (iCurrentScanLine<=lower.iY))
				{
				// check that the edge starting at vertex i%iNumVertexes is not horizontal
				BG_ASSERT_DEBUG(upper.iY!=lower.iY, EBitgdiPanicPolygonFiller);

				// step through the line-generator until the current scan-line is reached
				TPoint startPos, endPos;
				JumpToCurrentScanLine(iSlowData.lineGenerator, upper, lower, startPos, endPos);

				// find the intersection start and end pixels
				SSlowScanLineIntersection scanLineIntersection;
				scanLineIntersection.firstPixel=Min(startPos.iX, endPos.iX);
				scanLineIntersection.lastPixel=Max(startPos.iX, endPos.iX);
				scanLineIntersection.firstVertexOfEdge=i%iNumVertexes;

				// handle horizontal runs and minima/maxima
				if (upper.iY==iCurrentScanLine)
					SlowHandleVertexIntersection(scanLineIntersection, i, EFalse);
				else if (lower.iY==iCurrentScanLine)
					SlowHandleVertexIntersection(scanLineIntersection, i, ETrue);

				// see if there have been other intersections with the same first-pixel
				if (scanLineIntersection.firstPixel==iSlowData.firstPixelOfLastIntersectionInPrevBuffer)
					++iSlowData.numIntersectionsWithSameFirstPixelMetThisTime;

				// if the intersection has not already been included in a previous buffer-load
				if ((scanLineIntersection.firstPixel>iSlowData.firstPixelOfLastIntersectionInPrevBuffer) ||
					((scanLineIntersection.firstPixel==iSlowData.firstPixelOfLastIntersectionInPrevBuffer) &&
													(iSlowData.numIntersectionsWithSameFirstPixelMetThisTime>=
													iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet)))
					{
					// put the intersection in the right place in the intersection list (if there is room)
					for (j=0; j<iSlowData.numScanLineIntersections; ++j)
						if (scanLineIntersection.firstPixel<iSlowData.scanLineIntersectionList[j].firstPixel)
							{
							if (iSlowData.numScanLineIntersections<SSlowData::EStoreSize)
								++iSlowData.numScanLineIntersections;
							else
								iSlowData.scanLineComplete=EFalse;

							for (k=iSlowData.numScanLineIntersections-1; k>j; --k)
								iSlowData.scanLineIntersectionList[k]=iSlowData.scanLineIntersectionList[k-1];
							iSlowData.scanLineIntersectionList[j]=scanLineIntersection;
							break;
							}
					if (j==iSlowData.numScanLineIntersections)
						{
						if (iSlowData.numScanLineIntersections<SSlowData::EStoreSize)
							iSlowData.scanLineIntersectionList[iSlowData.numScanLineIntersections++]=scanLineIntersection;
						else
							iSlowData.scanLineComplete=EFalse;
						}
					}
				}
			}

		if (!iSlowData.scanLineComplete)
			{
			BG_ASSERT_DEBUG(iSlowData.numScanLineIntersections==SSlowData::EStoreSize, EBitgdiPanicPolygonFiller);

			if (iSlowData.firstPixelOfLastIntersectionInPrevBuffer==iSlowData.scanLineIntersectionList[SSlowData::EStoreSize-1].firstPixel)
				iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet+=SSlowData::EStoreSize-1;
			else
				{
				iSlowData.firstPixelOfLastIntersectionInPrevBuffer=iSlowData.scanLineIntersectionList[SSlowData::EStoreSize-1].firstPixel;
				iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet=1;
				for (i=SSlowData::EStoreSize-1; (i>0) && (iSlowData.firstPixelOfLastIntersectionInPrevBuffer==
															iSlowData.scanLineIntersectionList[i-1].firstPixel); --i)
					++iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet;
				}
			}
		}

	// depending on the rule used, find the pixel-run
	TBool doFill=EFalse; // dummy initialization to prevent compiler warning
	if (iScanLineIntersection<iSlowData.numScanLineIntersections-1)
		{
		switch (iFillRule)
			{
		case CGraphicsContext::EAlternate:
			iToggler=!iToggler;
			doFill=iToggler;
			break;
		case CGraphicsContext::EWinding:
				BG_ASSERT_DEBUG(Point(iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge).iY!=
							   Point((iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge+1)%iNumVertexes).iY,
																										EBitgdiPanicPolygonFiller);

				if (Point(iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge).iY>
					Point((iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge+1)%iNumVertexes).iY)
					++iNestingLevel;
				else
					--iNestingLevel;

			doFill=(iNestingLevel!=0);
			break;
			}

		if (doFill)
			{
			aStart=Max(iRightMostPixelOnScanLine, iSlowData.scanLineIntersectionList[iScanLineIntersection].lastPixel)+1;
			aEnd=iSlowData.scanLineIntersectionList[iScanLineIntersection+1].firstPixel-1;
			}
		else
			{
			// set the start after the end
			aStart=KMinTInt+1;
			aEnd=KMinTInt;
			}

		if (iRightMostPixelOnScanLine<iSlowData.scanLineIntersectionList[iScanLineIntersection].lastPixel)
			iRightMostPixelOnScanLine=iSlowData.scanLineIntersectionList[iScanLineIntersection].lastPixel;
		++iScanLineIntersection;
		}

	if (iScanLineIntersection==iSlowData.numScanLineIntersections-1)
		{
		if (iSlowData.scanLineComplete)
			{
			BG_ASSERT_DEBUG(Point(iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge).iY!=
						   Point((iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge+1)%iNumVertexes).iY,
																											EBitgdiPanicPolygonFiller);

			switch (iFillRule)
				{
			case CGraphicsContext::EAlternate:
				iToggler=!iToggler;
				BG_ASSERT_DEBUG(iToggler==0, EBitgdiPanicPolygonFiller);
				break;
			case CGraphicsContext::EWinding:
				if (Point(iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge).iY>
					Point((iSlowData.scanLineIntersectionList[iScanLineIntersection].firstVertexOfEdge+1)%iNumVertexes).iY)
					++iNestingLevel;
				else
					--iNestingLevel;

				BG_ASSERT_DEBUG((!iSlowData.scanLineComplete) || (iNumVertexes==2) || (iNestingLevel==0), EBitgdiPanicPolygonFiller);
				break;
				}
			}

		iScanLineIntersection=0;
		if (iSlowData.scanLineComplete)
			{
			++iCurrentScanLine;
			iRightMostPixelOnScanLine=KMinTInt;
			iSlowData.numIntersectionsWithSameFirstPixelPreviouslyMet=0;
			iSlowData.scanLineComplete=EFalse;
			iSlowData.firstPixelOfLastIntersectionInPrevBuffer=KMinTInt;
			}
		}
	}

void CPolygonFiller::FastHandleVertexIntersection(TInt& aCurrentActiveEdge, 
												  TBool aIsLowerVertex)
	{
	BG_ASSERT_DEBUG(iUseFastAlgorithm, EBitgdiPanicPolygonFiller);

	if (iFastData.vertexList[(iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->firstVertex+1)%iNumVertexes].iY==iCurrentScanLine)
		// it is the second vertex of active-edge aCurrentActiveEdge that coincides with the current scan-line
		{
		TInt origActiveEdge=aCurrentActiveEdge;
		SFastScanLineIntersection scanLineIntersection;
		scanLineIntersection.activeEdgePtr=NULL;
		SetFastIntersection(iFastData.activeEdgeList[origActiveEdge], scanLineIntersection);

		// walk through subsequent adjacent horizontal active-edges
		for (; ; )
			{
			// exit the loop if the vertex-run *is* a maximum or a minimum
			const SFastEdge* tempEdgePtr=iFastData.activeEdgeList[(aCurrentActiveEdge+1)%iFastData.numActiveEdges].edgePtr;
			TBool isMaxOrMin = EFalse;

			switch(aIsLowerVertex)
				{
				case EFalse:
					isMaxOrMin = (iFastData.vertexList[tempEdgePtr->lowerVertex].iY > iCurrentScanLine);
					break;

				case ETrue:
					isMaxOrMin = (iFastData.vertexList[tempEdgePtr->upperVertex].iY < iCurrentScanLine);
					break;
				}

			if (isMaxOrMin)
 				// the vertex-run is a maximum or a minimum
				{
				if (aIsLowerVertex)
					{
					*iFastData.activeEdgeList[origActiveEdge].scanLineIntersectionPtr=scanLineIntersection;
					iFastData.activeEdgeList[origActiveEdge].scanLineIntersectionPtr->activeEdgePtr=&iFastData.activeEdgeList[origActiveEdge];
					}
				else
					{
					// add an intersection
					iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections]=scanLineIntersection;
					iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections].activeEdgePtr=&iFastData.activeEdgeList[origActiveEdge];
					iFastData.activeEdgeList[origActiveEdge].scanLineIntersectionPtr=&iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections];
					++iFastData.numScanLineIntersections;
					}
				break;
				}

			// the active-edge is horizontal, or the vertex-run is not a maximum or a minimum

			++aCurrentActiveEdge;
			BG_ASSERT_DEBUG(aCurrentActiveEdge<iFastData.numActiveEdges, EBitgdiPanicPolygonFiller);

			// update scanLineIntersection
			TPoint startPos, endPos;
			TInt minX, maxX;
			iFastData.activeEdgeList[aCurrentActiveEdge].lineGenerator.SingleScanline(startPos, endPos);
			minX=Min(startPos.iX, endPos.iX);
			maxX=Max(startPos.iX, endPos.iX);
			if (scanLineIntersection.firstPixel>minX)
				scanLineIntersection.firstPixel=minX;
			if (scanLineIntersection.lastPixel<maxX)
				scanLineIntersection.lastPixel=maxX;

			tempEdgePtr=iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr;
			TBool isNeitherMaxOrMin = EFalse;

			switch(aIsLowerVertex)
				{
				case EFalse:
					isNeitherMaxOrMin = (iFastData.vertexList[tempEdgePtr->upperVertex].iY < iCurrentScanLine);
					break;

				case ETrue:
					isNeitherMaxOrMin = (iFastData.vertexList[tempEdgePtr->lowerVertex].iY > iCurrentScanLine);
					break;
				}

 			// exit the loop if the vertex-run is *not* a maximum or a minimum
			if (isNeitherMaxOrMin)
				{
				TInt newActiveEdge;
				TInt oldActiveEdge;
				if (aIsLowerVertex)
					{
					newActiveEdge=aCurrentActiveEdge;
					oldActiveEdge=origActiveEdge;
					}
				else
					{
					newActiveEdge=origActiveEdge;
					oldActiveEdge=aCurrentActiveEdge;
					}
				iFastData.activeEdgeList[newActiveEdge].scanLineIntersectionPtr=iFastData.activeEdgeList[oldActiveEdge].scanLineIntersectionPtr;
				iFastData.activeEdgeList[oldActiveEdge].scanLineIntersectionPtr=NULL;
				*iFastData.activeEdgeList[newActiveEdge].scanLineIntersectionPtr=scanLineIntersection;
				iFastData.activeEdgeList[newActiveEdge].scanLineIntersectionPtr->activeEdgePtr=&iFastData.activeEdgeList[newActiveEdge];
				break;
				}
			}
		}
	else
		// it is the first vertex of active-edge aCurrentActiveEdge that coincides with the current scan-line
		{
#if defined(_DEBUG)
		// check that the vertex we are at is a maximum or a minimum
		TInt previousNotLevelVertex;
		TInt SFastEdge::*vertex=(aIsLowerVertex)? &SFastEdge::lowerVertex: &SFastEdge::upperVertex;
		for (previousNotLevelVertex=iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->*vertex;
								iFastData.vertexList[iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->*vertex].iY==iFastData.vertexList[previousNotLevelVertex].iY;
								previousNotLevelVertex=(previousNotLevelVertex+iNumVertexes-1)%iNumVertexes)
			;
		TInt nextNotLevelVertex=(iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->*vertex+1)%iNumVertexes;
		BG_ASSERT_DEBUG((iFastData.vertexList[iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->*vertex].iY>iFastData.vertexList[previousNotLevelVertex].iY)==
					   (iFastData.vertexList[iFastData.activeEdgeList[aCurrentActiveEdge].edgePtr->*vertex].iY>iFastData.vertexList[nextNotLevelVertex].iY),
																						EBitgdiPanicPolygonFiller);
#endif

		if (aIsLowerVertex)
			SetFastIntersection(iFastData.activeEdgeList[aCurrentActiveEdge],*iFastData.activeEdgeList[aCurrentActiveEdge].scanLineIntersectionPtr);
		else
			{
			// add an intersection
			SetFastIntersection(iFastData.activeEdgeList[aCurrentActiveEdge], iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections]);
			iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections].activeEdgePtr=&iFastData.activeEdgeList[aCurrentActiveEdge];
			iFastData.activeEdgeList[aCurrentActiveEdge].scanLineIntersectionPtr=&iFastData.scanLineIntersectionList[iFastData.numScanLineIntersections];
			++iFastData.numScanLineIntersections;
			}
		}
	}

void CPolygonFiller::SetFastIntersection(SFastActiveEdge& aActiveEdge, SFastScanLineIntersection& aScanLineIntersection)
	{
	BG_ASSERT_DEBUG(iUseFastAlgorithm, EBitgdiPanicPolygonFiller);

	TPoint startPos, endPos;

	aActiveEdge.lineGenerator.SingleScanline(startPos, endPos);
	aScanLineIntersection.firstPixel=Min(startPos.iX, endPos.iX);
	aScanLineIntersection.lastPixel=Max(startPos.iX, endPos.iX);
	}

void CPolygonFiller::SlowHandleVertexIntersection(SSlowScanLineIntersection& aScanLineIntersection, 
												  TInt& aVertexStartingCurrentEdge,
												  TBool aIsLowerVertex)
	{
	if (Point((aVertexStartingCurrentEdge+1)%iNumVertexes).iY==iCurrentScanLine)
		// it is the second vertex of the edge starting at vertex aVertexStartingCurrentEdge%iNumVertexes
		// that coincides with the current scan-line
		{
		// walk through subsequent adjacent horizontal active-edges
		for (; ; )
			{
			TPoint nextVertexButOne=Point((aVertexStartingCurrentEdge+2)%iNumVertexes);
			TBool isMaxOrMin = EFalse;

			switch(aIsLowerVertex)
				{
				case EFalse:
					isMaxOrMin = (nextVertexButOne.iY > iCurrentScanLine);
					break;

				case ETrue:
					isMaxOrMin = (nextVertexButOne.iY < iCurrentScanLine);
					break;
				}

			// exit the loop if the vertex-run *is* a maximum or a minimum
			if (isMaxOrMin)
				{
				break;
				}

			// the edge starting at vertex aVertexStartingCurrentEdge%iNumVertexes is horizontal, or the vertex-run is not a
			// maximum or a minimum

			++aVertexStartingCurrentEdge;
			BG_ASSERT_DEBUG(aVertexStartingCurrentEdge%iNumVertexes!=iFirstVertex, EBitgdiPanicPolygonFiller);

			// step through the line-generator until the current scan-line is reached
			TPoint upper=Point(aVertexStartingCurrentEdge%iNumVertexes);
			TPoint lower=nextVertexButOne;
			if (upper.iY>lower.iY)
				{
				TPoint temp=upper;
				upper=lower;
				lower=temp;
				}

			TPoint startPos, endPos;
			if (upper.iY!=lower.iY)
				JumpToCurrentScanLine(iSlowData.lineGenerator, upper, lower, startPos, endPos);
			else
				{
				// N.B. which is set to which doesn't matter, as long as startPos is set to either upper or lower, and endPos is set to the other
				startPos=upper;
				endPos=lower;
				}

			// expand the intersection, if necessary
			TInt minX=Min(startPos.iX, endPos.iX);
			TInt maxX=Max(startPos.iX, endPos.iX);
			if (aScanLineIntersection.firstPixel>minX)
				aScanLineIntersection.firstPixel=minX;
			if (aScanLineIntersection.lastPixel<maxX)
				aScanLineIntersection.lastPixel=maxX;

			TBool isNeitherMaxOrMin = EFalse;

			switch(aIsLowerVertex)
				{
				case EFalse:
					isNeitherMaxOrMin = (nextVertexButOne.iY < iCurrentScanLine);
					break;

				case ETrue:
					isNeitherMaxOrMin = (nextVertexButOne.iY > iCurrentScanLine);
					break;
				}
							  
			// exit the loop if the vertex-run is *not* a maximum or a minimum					   
			if (isNeitherMaxOrMin)
				{
				if (aIsLowerVertex)
					{
					aScanLineIntersection.firstVertexOfEdge=aVertexStartingCurrentEdge%iNumVertexes;
					}
				break;
				}
			}
		}
	else
		// it is the first vertex of the edge starting at vertex aVertexStartingCurrentEdge%iNumVertexes
		// that coincides with the current scan-line
		{
#if defined(_DEBUG)
		// check that the vertex we are at is a maximum or a minimum
		TInt previousNotLevelVertex;
		for (previousNotLevelVertex=aVertexStartingCurrentEdge%iNumVertexes;
														Point(aVertexStartingCurrentEdge%iNumVertexes).iY==
														Point(previousNotLevelVertex).iY;
														previousNotLevelVertex=(previousNotLevelVertex+iNumVertexes-1)%iNumVertexes)
			;
		TInt nextNotLevelVertex=(aVertexStartingCurrentEdge+1)%iNumVertexes;
		TInt previousY=Point(previousNotLevelVertex).iY;
		TInt currentY=Point(aVertexStartingCurrentEdge%iNumVertexes).iY;
		TInt nextY=Point(nextNotLevelVertex).iY;
		BG_ASSERT_DEBUG((currentY>previousY) == (currentY>nextY), EBitgdiPanicPolygonFiller);
#endif
		}
	}

void CPolygonFiller::JumpToCurrentScanLine(TLinearDDA& aLineGenerator, 
										   const TPoint& aUpper, 
										   const TPoint& aLower,
										   TPoint& aStartPos, 
										   TPoint& aEndPos) const
	{
	BG_ASSERT_DEBUG(aUpper.iY<=aLower.iY, EBitgdiPanicPolygonFiller);
	aLineGenerator.Construct(aUpper, aLower);
	if (aUpper.iY<iCurrentScanLine)
		{
		TInt notUsed;
		aLineGenerator.JumpToYCoord(notUsed, iCurrentScanLine-2);
		}
	do
		aLineGenerator.SingleScanline(aStartPos, aEndPos);
	while (aStartPos.iY!=iCurrentScanLine);
	BG_ASSERT_DEBUG(aStartPos.iY==iCurrentScanLine, EBitgdiPanicPolygonFiller);
	BG_ASSERT_DEBUG(aEndPos.iY==iCurrentScanLine, EBitgdiPanicPolygonFiller);
	}

const TPoint& CPolygonFiller::Point(TInt aIndex)
	{
	if(iPointList) return(iPointList[aIndex]);
	BG_ASSERT_DEBUG(iPointArray,EBitgdiPanicPolygonFiller);
	return((*iPointArray)[aIndex]);
	}

