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

/**
 @file
 @test
 @internalComponent
*/

#include <e32base.h>

#ifndef __T_PERF_DATA_H__
#define __T_PERF_DATA_H__

class CTPerfData : public CBase
	{
public:
	IMPORT_C static CTPerfData* NewL();

	/**
	* Start performance counter. Use this to mark the
	* point at which measurement is to start e.g. beginning
	* of draw frame.
	*/
	IMPORT_C void StartCounter();

	/**
	* Stop performance counter. Call this to mark the point
	* at which measurement is to finish e.g. end of draw
	* frame.
	*/
	IMPORT_C void StopCounterL();

	/**
	* Write performance results to designated file
	*
	* @param	aFileName Name of file to write to
	* @leave	System wide error code.
	*/
	IMPORT_C void WriteResultsL(const TDesC& aFileName);


	~CTPerfData();

private:
	void Construct();
	void AnalyseResultsL();
	void TrimedMeanL();
	void MaxTime();
	void MinTime();
	void FrameRateL();
	CTPerfData() {}

private:
	RArray<TUint32> iResults;
	TUint32 iCounter;
	TUint32 iTrimedMean;
	TUint32 iMaxTime;
	TUint32 iMinTime;
	TReal iFrameRate;
	};

#endif //__T_PERF_DATA_H__
