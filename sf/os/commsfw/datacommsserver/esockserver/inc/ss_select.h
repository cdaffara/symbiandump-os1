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

/**
 @file
 @internalTechnology
 @released
*/

#if !defined(__SS_SELECT_H__)
#define __SS_SELECT_H__

#include <e32base.h>
#include <s32std.h>

namespace ESock
{
class ISelectionNotify;
class MProviderSelector
/**
 @internalTechnology
 */
	{
public:
	virtual void Cancel() = 0;
	virtual void SelectL(ISelectionNotify& aSelectionNotify) = 0;
	virtual ~MProviderSelector() {};
	};

class CMetaConnectionProviderBase;
typedef void (*TSelectCompleteFn)(TAny*, MProviderSelector*, CMetaConnectionProviderBase*);
typedef void (*TStateChangeNotificationFn)(TAny*, MProviderSelector*, TInt, TInt);
typedef void (*TErrorFn)(TAny*, MProviderSelector*, TInt);
typedef void (*TAnyFn)(TAny*);

class ISelectionNotify
/**
 @internalTechnology
 */
	{
public:
	enum
		{
		KCallbacksCount = 3
		};
	
public:
	ISelectionNotify() :
		iThis(NULL),
		iInterfaceVTable(NULL)
		{
		}

	ISelectionNotify(TAny* aThis, const TAnyFn& aInterfaceVTable) :
		iThis( aThis ),
		iInterfaceVTable( &aInterfaceVTable )
		{
		}

	void SelectComplete(MProviderSelector* aSelector, CMetaConnectionProviderBase* aProvider)
	    {
	    if ( Check(1) )
    	    {
   	    	((TSelectCompleteFn)(iInterfaceVTable[1]))(iThis, aSelector, aProvider);
    	    }
	    }
	
	void StateChangeNotification(MProviderSelector* aSelector, TInt aStage, TInt aError)
	    {
	    if ( Check(2) )
    	    {
    	    ((TStateChangeNotificationFn)iInterfaceVTable[2])(iThis, aSelector, aStage, aError);
    	    }
	    }

	void Error(MProviderSelector* aSelector, TInt aError)
	    {
	    if ( Check(3) )
    	    {
    	    ((TErrorFn)iInterfaceVTable[3])(iThis, aSelector, aError);
    	    }
	    }

private:
	TBool Check( TInt nFn )
		{
		return iInterfaceVTable && (TInt)iInterfaceVTable[0] >= nFn && iInterfaceVTable[nFn];
		}

private:
	TAny* iThis;
	TAnyFn const* iInterfaceVTable;
	};


template <class TCLIENT>
class TSelectionNotify
/**
 @internalTechnology
 */
	{
public:
	static void SelectComplete(TAny* aThis, MProviderSelector* aSelector, CMetaConnectionProviderBase* aProvider);
	static void StateChangeNotification(TAny* aThis, MProviderSelector* aSelector, TInt aStage, TInt aError);
	static void SelectError(TAny* aThis, MProviderSelector* aSelector, TInt aError);
	};
	
template <class TCLIENT>
void TSelectionNotify<TCLIENT>::SelectComplete(TAny* aThis, MProviderSelector* aSelector, CMetaConnectionProviderBase* aProvider)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	me->SelectComplete(aSelector,aProvider);
	}

template <class TCLIENT>
void TSelectionNotify<TCLIENT>::StateChangeNotification(TAny* aThis, MProviderSelector* aSelector, TInt aStage, TInt aError)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	me->StateChangeNotification(aSelector,aStage,aError);
	}

template <class TCLIENT>
void TSelectionNotify<TCLIENT>::SelectError(TAny* aThis, MProviderSelector* aSelector, TInt aError)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	me->SelectError(aSelector,aError);
	}


} //namespace ESock

#endif //__SS_SELECT_H__

