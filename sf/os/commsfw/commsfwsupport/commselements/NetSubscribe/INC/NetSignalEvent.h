// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __NETSIGNALEVENT_H_
#define __NETSIGNALEVENT_H_

#include <e32base.h>
#include <e32std.h>

namespace Meta
{
struct SMetaData;
}

namespace NetSubscribe
{

const TInt32 KEventUid = 0x102055A9;

/**
 * Signal Id encapsulation Interface Id, Event Id and the instance handle, The instance
 * handle represent the context of the signal and should be obtained beforehead
 *
 * @see STypeId
 */
struct SSignalId
	{
	SSignalId( const Meta::STypeId& aTypeId, TUint32 aHandle ) :
		iTypeId(aTypeId),
		iHandle(aHandle)
		{
		}
	SSignalId( TUint32 aInterfaceId, TUint32 aEventId, TUint32 aHandle ) :
		iHandle(aHandle)
		{
		iTypeId = Meta::STypeId::CreateSTypeId(aInterfaceId, aEventId);
		}
	SSignalId( TUint32 aInterfaceId, TUint32 aEventId ) :
		iHandle(NULL)
		{
		iTypeId = Meta::STypeId::CreateSTypeId(aInterfaceId, aEventId);
		}
	inline TBool operator==(const SSignalId& obj) const
		{ 
		return iTypeId==obj.iTypeId && iHandle==obj.iHandle;
		}
	Meta::STypeId iTypeId;
	TUint32 iHandle;
	};

class TEventBase
/**
A base for all event classes

*/
	{
	};

class TEvent;
typedef void (*TSignalCallback)( TAny* , const Meta::SMetaData* );
typedef void (*TSignalErrorCallback)(TAny* , TInt);

class CNetSubscribe;
class TEvent : public TEventBase
/**
An event class to convey callback info and callback data of type SMetaData.

*/
	{
	friend class TEventClientData;
	
public:
	TEvent( TAny* aThis, TSignalCallback aSignalCallback, TSignalErrorCallback aSignalErrorCallback ) :
		iHandler( aSignalCallback ),
		iErrHandler( aSignalErrorCallback ),
		iThis( aThis )
		{
		}
	TEvent( TAny* aThis, TSignalCallback aSignalCallback ) :
		iHandler( aSignalCallback ),
		iErrHandler( NULL ),
		iThis( aThis )
		{
		}
		
	IMPORT_C void SubscribeL(CNetSubscribe& aSubscribe, const SSignalId& aSignalId );
	IMPORT_C void Cancel(CNetSubscribe& aSubscribe);

protected:
	TSignalCallback iHandler;
	TSignalErrorCallback iErrHandler;
	TAny* iThis;
	};

} // namespace NetSubscribe


#endif // __NETSUBSCRIBE_H_

