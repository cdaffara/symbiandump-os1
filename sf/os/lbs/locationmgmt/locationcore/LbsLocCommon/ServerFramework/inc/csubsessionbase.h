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
// Definition of Subsession base object 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSUBSESSIONBASE_H__
#define __LBSSUBSESSIONBASE_H__

#include <e32base.h>

// forward classes
class CSecureServerBase;

/** 
@internalTechnology
@released

Interface for release resource
 */
class MRelease
	{
public:
	virtual void VirtualRelease() = 0; // THIS MUST delete any implementation!	
	};

/** 
@internalTechnology

MRelease interface function pointer
 */
typedef void (MRelease::* TReleaseFunc)();


/** 
@internalTechnology
@released

static class for push the resource needed to be released to stack
 */
class CleanupVirtualRelease
	{
public:
	inline static void PushL(MRelease& aRef);
private:
	static void Release(TAny *aPtr);
	};

void CleanupVirtualReleasePushL(MRelease& aRef);


/** 
@internalTechnology
@released
@see MRelease

/** 
@internalTechnology
@released
@see MRelease

MRelease based Interface, interface for subsession
 */
class MSubSessionImpl : public MRelease
	{
public:
	virtual void DispatchL(const RMessage2& aMessage) = 0;
	virtual void DispatchError(const RMessage2& aMessage, TInt aError) = 0;
	virtual void CreateSubSessionL(const RMessage2& aMessage, const CSecureServerBase* aServer) = 0;
	virtual void CloseSubSession() = 0;
	};

/**
@internalTechnology
@released
@see MSubSessionImpl

 Subsession base object
 */
class CSubSessionBase : public CObject
	{
public:
	/** Panic type for subsession objects */
	enum TSubSessionBasePanic
		{
		EDispatchLNotOverriden = 1,
		EDispatchErrorNotOverriden = 2,
		ECreateSubSessionLNotOverriden = 3,
		ECloseSubSessionNotOverriden = 4
		};
public:
	CSubSessionBase();
	virtual ~CSubSessionBase();
	MSubSessionImpl* GetImplL(const RMessage2& aMessage);
	void SetImpl(MSubSessionImpl* aImpl);
	MSubSessionImpl* Impl();
protected:
	void Panic(const TSubSessionBasePanic aPanic);
protected:
	// property - later derived class may wish to acess so protected, not private
	/** Generic interface pointer to subsession implemetations */
	MSubSessionImpl* iImpl;
	};


#endif // __LBSSUBSESSIONBASE_H__
