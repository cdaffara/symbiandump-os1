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
// generic framework node state machine states
// 
//

/**
 @file 
 @internalAll
*/

#ifndef SYMBIAN_MM_ACTIVITIES_INTERNAL_H
#define SYMBIAN_MM_ACTIVITIES_INTERNAL_H

namespace MeshMachine
{

static const TUint16 KActivityNull                 = 0;
static const TUint16 KActivityCustom               = 0x007f;
//the range is used for activities that can run more than one instance in parallel
//such activities must be type of or derived from CNodeParallerActivityBase (e_mm_activity.h)
static const TUint16 KActivityParallelRangeMin     = 0x0100;
static const TUint16 KActivityParallelRangeMax     = 0xFF00;

class AMMNodeBase;
class CNodeActivityBase;
class TNodeContextBase;

//-=========================================================
//
// TNodeActivity
//
//-=========================================================
struct TNodeActivity
/**
*/
	{
	/*There are now two types of activity c'tors (hence TAny iCtor supported:
	(1) The legacy ctor based on AMMNodeBase that must be supported for source backwards compatibility
	(2) The new ctor based on TNodeContextBase that is desired because TNodeContextBase gives access to AMMNodeBase as well as the message 
	    and peer and it is therefore more powerful. This new ctor has been introduced to aid leave activities to be infallible
		by allowing access to the peer, which stores the space preallocated for the activity
	Ideally this once fixed to only leave (2) especially since differentiating (1) from (2) is rather obscure. iCtor is a pointer
	to a pointer to a ctor function and the distinction is based on where iCtor points (see AMMNodeBase::StartActivityL) */
	enum TNodeActivityFlags
		{
		ENodeCtor = 0,
		EContextCtor = 1
		};

	typedef CNodeActivityBase* (*TStaticActivityCtor)(const TNodeActivity& aActivitySig, AMMNodeBase& aNode);
	typedef CNodeActivityBase* (*TStaticActivityContextCtor)(const TNodeActivity& aActivitySig, TNodeContextBase& aCtx);
	const TUint8 iId;
	const TUint8 iFlags;
	//Message starting the activity (state in iFirstTriple is waiting for),
	//used by error handling, could be NULL for "single triple" activities because
	//than the current message associated with context is used.
    const TUint16 iKickOffMessageId;
    const TUint32 iKickOffMessageRealm;
	const NetStateMachine::TStateTriple& iFirstTriple;
	const TAny* iCtor;
	const TText8* iName;
	};

//-=========================================================
//
// TNodeActivityMap
//
//-=========================================================
struct TNodeActivityMap
/**
*/
	{
	typedef const TNodeActivityMap& (*TStaticNodeActivityMap)();
	typedef const TNodeActivity& (*TStaticNodeActivity)();
	const TStaticNodeActivity& iFirstActivity;
	};

//-=========================================================
//
// TNodeActivityIter
//
//-=========================================================
class TNodeActivityIter
/**
*/
	{
public:
	TNodeActivityIter(const TNodeActivityMap& aActivityMap)
	:	iCurrentEntry(&aActivityMap.iFirstActivity)
		{
		}
	const TNodeActivity* operator++(TInt /*aInd*/)
		{
		return FetchActivity(); //recursive
		}

private:
	const TNodeActivity* FetchActivity();

private:
	const TNodeActivityMap::TStaticNodeActivity* iCurrentEntry;
	};

} // namespace MeshMachine

#endif // SYMBIAN_MM_ACTIVITIES_INTERNAL_H

