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
 @internalTechnology
*/

#ifndef SYMBIAN_FACTORYNOTIFY_H
#define SYMBIAN_FACTORYNOTIFY_H

#include <e32base.h>
#include <s32std.h>
#include <elements/factory.h>

namespace Factories
{

typedef void (*TInterfaceStateChangeFn)( TAny*, const TDesC8& aInfo );
typedef TInt (*TFactoryNotifyFn)( TAny*, AFactoryObject& aObject, CFactoryBase& aFactory );
typedef void (*TAnyFn)(TAny*);

//could be as a const static table part of the class
class IFactoryNotify
/**
 @internalComponent
 */
	{
public:
	TBool operator==(const IFactoryNotify& aRhs) const
		{
		return (iThis == aRhs.iThis && iInterfaceVTable == aRhs.iInterfaceVTable);
		}

	IFactoryNotify( TAny* aThis, TAnyFn const& aInterfaceVTable ) :
		iThis( aThis ),
		iInterfaceVTable( &aInterfaceVTable )
		{
		}

	void InterfaceStateChange(const TDesC8& aInfo)
		{
		if ( Check(1) )
			{
			((TInterfaceStateChangeFn)(iInterfaceVTable[1]))(iThis,aInfo);
			}
		}
	void NotifyDeletion(AFactoryObject& aObject, CFactoryBase& aFactory)
		{
		if ( Check(2) )
			{
			((TFactoryNotifyFn)(iInterfaceVTable[2]))(iThis, aObject, aFactory);
			}
		}
	TInt NotifyCreation(AFactoryObject& aObject, CFactoryBase& aFactory)
		{
		return ( Check(3) ) ?
			((TFactoryNotifyFn)(iInterfaceVTable[3]))(iThis, aObject, aFactory) :
			KErrNone;
		}

protected:
	TBool Check( TInt nFn )
		{
		return (TInt)iInterfaceVTable[0] >= nFn && iInterfaceVTable[nFn];
		}

protected:
	TAny* iThis;
	TAnyFn const* iInterfaceVTable;
	};

template <class TCLIENT>
class TFactoryNotify
/**
 @internalComponent
 */
	{
public:
	static void Notification(TAny* aThis, const TDesC8& aInfo);
	static void NotifyDeletion(TAny* aThis, AFactoryObject& aObject, CFactoryBase& aFactory);
	static TInt NotifyCreation(TAny* aThis, AFactoryObject& aObject, CFactoryBase& aFactory);

	};

template <class TCLIENT>
void TFactoryNotify<TCLIENT>::Notification(TAny* aThis, const TDesC8& aInfo)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	me->InterfaceStateChangeNotification(aInfo);
	}

template <class TCLIENT>
void TFactoryNotify<TCLIENT>::NotifyDeletion(TAny* aThis, AFactoryObject& aObject, CFactoryBase& aFactory)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	me->NotifyDeletion(aObject, aFactory);
	}

template <class TCLIENT>
TInt TFactoryNotify<TCLIENT>::NotifyCreation(TAny* aThis, AFactoryObject& aObject, CFactoryBase& aFactory)
	{
	TCLIENT* me = (TCLIENT*)aThis;
	return me->NotifyCreation(aObject, aFactory);
	}

} //namespace Factories

#endif // SYMBIAN_FACTORYNOTIFY_H

