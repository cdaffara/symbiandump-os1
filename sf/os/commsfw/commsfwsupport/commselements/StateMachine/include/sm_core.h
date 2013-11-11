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
// core handling class
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef _SM_CORE_H_
#define _SM_CORE_H_

// Enable logging for debug builds or builds with the comms flogger enabled - must be defined before including e32utrace.h
#if (defined(_DEBUG) || defined(__FLOG_ACTIVE)) && !defined(SYMBIAN_TRACE_ENABLE)
#define SYMBIAN_TRACE_ENABLE
#endif

#include <e32base.h>
#include <elements/sm_statetriple.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemStateMachsmc, "ElemStateMachsmc");
#endif

//
//helper macros declaration

#define NETSM_TPL_DECLARE_CTR( type, intf, context ) \
	static intf* GetVTablePtr(TDesC8& aMem, TAny* aContext) \
		{ \
		__ASSERT_DEBUG(aContext != NULL, User::Panic(KSpecAssert_ElemStateMachsmc, 1)); \
		return new ((TAny*)aMem.Ptr())type(*((context*)aContext)); \
		}

#define NETSM_DECLARE_CTR( intf ) \
	static intf* GetVTablePtr( TDesC8& aMem, TAny* aContext );

#define NETSM_DEFINE_CTR( element, intf, context ) \
	intf* element :: GetVTablePtr( TDesC8& aMem, TAny* aContext ) \
		{ \
		__ASSERT_DEBUG(aContext != NULL, User::Panic(KSpecAssert_ElemStateMachsmc, 2)); \
		return new ((TAny*)aMem.Ptr()) element (*((context*)aContext)); \
		}

#define EXPORT_NETSM_DECLARE_CTR( intf ) \
	IMPORT_C NETSM_DECLARE_CTR( intf )

#define EXPORT_NETSM_DEFINE_CTR( element, intf, context ) \
	EXPORT_C NETSM_DEFINE_CTR( element, intf, context )


namespace NetStateMachine
{

//template to derive from to provide context member of a particular type
template <class TCONTEXT>
class AContext
	{
protected:
	explicit inline AContext(TCONTEXT& aContext) :
		iContext(aContext)
		{
		}

protected:
	TCONTEXT& iContext;
	};

//template class to use access to the context member(see AContext)
//as to the different type, related type. The related type must be accessible
//via static_cast.
template <class TCONTEXT, class TBASECONTEXT>
class TContextAccessor : public AContext<TBASECONTEXT>
	{
public:
	explicit inline TContextAccessor(TBASECONTEXT& aContext)
		: AContext<TBASECONTEXT>(aContext)
		{
		}

	inline TCONTEXT& Context()
		{
		return static_cast<TCONTEXT&>(this->iContext);
		}
	};

class ACore
	{
protected:
	explicit ACore() :
		iCurrent(NULL)
		{
		};

public:
	IMPORT_C static TBool Accept(const TStateTriple& aFirst, TAny* aContext, TInt aTransitionTag);
	IMPORT_C void Start( TAny* aContext, const TStateTriple& aFirst);
	IMPORT_C TBool Next( TAny* aContext);
	IMPORT_C void Cancel(TAny* aContext);
    IMPORT_C static const TStateTriple* FindNext( const TStateTriple& aCurrent, TInt aTransitionTag );

	// Runs the transition in the given state triple
    IMPORT_C static TInt Do(const TStateTriple& aTriple,TAny* aContext, TDes8& aMem);
    IMPORT_C void DoCurrent(TAny* aContext);
	IMPORT_C virtual TBool IsIdle() const;
	IMPORT_C virtual void SetIdle();

#ifdef SYMBIAN_TRACE_ENABLE
    const TText8* CurrentTripleName() const
        {
        if (iCurrent)
        	{
         	return (iCurrent->iName)? iCurrent->iName : _S8("Undefined");
         	}
        return _S8("Idle");
        }
#endif

private:
	TBool DoTransition(MStateFork& aStateFork, TAny* aContext, TDes8& aMem);

private:
	const TStateTriple* iCurrent;
	};

template <class TCLASS>
class ACoreFor : public ACore
	{
protected:
	ACoreFor() :
		ACore()
		{
		};

	TBool static Accept(const TStateTriple& aFirst, TAny* aContext, TInt aTransitionTag)
	    {
		return ACore::Accept( aFirst, &aContext, aTransitionTag );
	    }
	inline void Start(TCLASS& aContext, const TStateTriple& aFirst )
		{
		ACore::Start( &aContext, aFirst );
		}
	inline TBool Next( TCLASS& aContext )
	    {
	    return ACore::Next( &aContext );
	    }
	inline void Cancel(TCLASS& aContext)
		{
		ACore::Cancel( &aContext );
		}
	};

}

#endif //namespace NetStateMachine


