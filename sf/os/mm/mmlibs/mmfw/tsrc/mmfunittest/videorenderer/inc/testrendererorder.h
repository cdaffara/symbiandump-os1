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

#ifndef TESTRENDERERORDER_H
#define TESTRENDERERORDER_H

// INCLUDES
#include "testrenderer.h"

/* 
Test out of presentation order for video renderer in timed mode
MM-MMF-VIDREND-U-0004
*/
class RTestRendererOOOStep : public RTestRendererStep
	{
public:
	static RTestRendererOOOStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererOOOStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test out of presentation order for video renderer in non-timed mode
MM-MMF-VIDREND-U-0005
*/
class RTestRendererNonTimedOOOStep : public RTestRendererStep
	{
public:
	static RTestRendererNonTimedOOOStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererNonTimedOOOStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with presentation time too closed together in timed and non-timed mode
MM-MMF-VIDREND-U-0006 and MM-MMF-VIDREND-U-0007
*/
class RTestRendererSkipStep : public RTestRendererStep
	{
public:
	static RTestRendererSkipStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererSkipStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with out of date presentation time in timed and non-timed mode
MM-MMF-VIDREND-U-0008 and MM-MMF-VIDREND-U-0009
*/
class RTestRendererOODStep : public RTestRendererStep
	{
public:
	static RTestRendererOODStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererOODStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with same presentation time in timed mode
MM-MMF-VIDREND-U-0012
*/
class RTestRendererSameTimeStep : public RTestRendererStep
	{
public:
	static RTestRendererSameTimeStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererSameTimeStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with same presentation time in non-timed mode
MM-MMF-VIDREND-U-0013
*/
class RTestRendererSameTimeNonTimedStep : public RTestRendererStep
	{
public:
	static RTestRendererSameTimeNonTimedStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererSameTimeNonTimedStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with presentation time 0 in timed mode
MM-MMF-VIDREND-U-0014
*/
class RTestRendererZeroTimeStep : public RTestRendererStep
	{
public:
	static RTestRendererZeroTimeStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererZeroTimeStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test buffer with presentation time 0 in non-timed mode
MM-MMF-VIDREND-U-0015
*/
class RTestRendererZeroTimeNonTimedStep : public RTestRendererStep
	{
public:
	static RTestRendererZeroTimeNonTimedStep* NewL(const TDesC& aStepName);

protected:
	RTestRendererZeroTimeNonTimedStep(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);
	};

#endif // TESTRENDERERORDER_H
