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

#ifndef __NETSUBSCRIBE_H_
#define __NETSUBSCRIBE_H_

#include <e32base.h>
#include <e32std.h>
#include <elements/metadata.h>

namespace NetSubscribe
{
/**
 * Maximum length(in bytes) of TEvent data part supported by NetSubscribe. 
 * The length is returned by SMetaData::Length()
 *
 * @see SMetaData
 */
const TInt KMaxLength = 128;
const TInt KTransportUid = 0x102055AA;
enum ETransportTypes
	{
	/**
	 * Publish/Subscribe Transport type
	 */
	EPublishSubscribe
	//Another transport types should follow here
	};
	
class TEventClientData;
struct SSignalId;
class TEventBase;
class XSignal;
class CNetSubscribe : public CBase
/**
 * Client side of the signal subscription library. The class abstracts the mechanism the signals are
 * registered, trapped and the way event data are marhalled/demarshalled.
 * Derived class manages specific signal source & transport layer
 * Allows for multiple signal to be registered.
 *
 * @see XSignal
 *	@internalTechnology
 */
	{
	friend class TEvent;
	
public:
	IMPORT_C static CNetSubscribe* NewL( const Meta::STypeId& aTransportId );
	
	IMPORT_C ~CNetSubscribe();

protected:
	explicit CNetSubscribe();
	virtual void RegisterNewSignalL(TEventClientData& aData, const SSignalId& aSignalId ) = 0;

	void RegisterSignalL(TEventClientData& aData, const SSignalId& aSignalId );
	void DeRegisterSignal(TEventBase* aEvent);

protected:		
	RPointerArray<XSignal> iSignals;
	};

} // namespace NetSubscribe


#endif // __NETSUBSCRIBE_H_

