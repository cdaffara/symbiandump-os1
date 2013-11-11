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
// top-level header file for C32 that defines some pervasive values and classes and
// other things to generally avoid circular import problems.
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef CS_COMMON_H
#define CS_COMMON_H

#include <elements/cftransport.h>

class CC32Dealer;
class CC32Player;
class CCommSession;

class CC32WorkerThread;

// Decided to have 9 threads. One main dealer (can have co-resident player) and eight player threads
const TInt KMaxWorkerThreadId = 8;
const TInt KMainThreadId=0;





NONSHARABLE_CLASS(TC32WorkerThreadPublicInfo) : public CommsFW::TWorkerThreadDataBase
	{
public:
	enum {
	    ENullWorkerId      = 0xFFFF,     //< Non-existent worker
		EMainThread        = 0,		//< the Pit Boss and Main Dealer run here
		EFirstPlayerThread = 1,	//< Lowest possible player id
		EMaxWorkerThreadId = KMaxWorkerThreadId	//< Highest possible player id (NB: this is an inclusive bound. Be very careful in array-bound contexts!)
		};

	CC32WorkerThread* iWorker;
	};


/**
@class
TC32WorkerThreadRegister
Structure containing information pertaining to a particular worker thread.
*/
NONSHARABLE_CLASS(TC32WorkerThreadRegister) : public TC32WorkerThreadPublicInfo
	{
	friend class CC32Dealer;
	friend class CC32Player;
	friend class CC32WorkerThread;
	typedef TC32WorkerThreadPublicInfo inherited;
public:
	TC32WorkerThreadRegister();

private:
	void Clear();

private:
	CC32Dealer* iDealer;			//< Dealer instance of the worker thread (if exists).
	CC32Player* iPlayer;			//< Player instance of the worker thread (if exists).
#ifdef _DEBUG
  	TBool iHasGlobalAllocFails;
#endif
	};


#endif // CS_COMMON_H


