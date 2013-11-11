
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
// Transport layer for event data is RProperty
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __NETPSSIGNAL_H_
#define __NETPSSIGNAL_H_

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include "NetSubscriber.h"

namespace NetSubscribe
{

class CPSSignal : public CSignalActive
/**
 * PS signal class.

 * @internalComponent
 */
	{
public:
	static CPSSignal* NewLC( const SSignalId& aSignalId );
	~CPSSignal();


protected:
	CPSSignal(const SSignalId& aSignalId) :
		CSignalActive(aSignalId)
		{
		}
	void ConstructL();
//the AO related functionality shouldn't be there see the commend above the
//CSignalActive declaration in NetSubscriber.h
	virtual void RunL();
	virtual void DoCancel();
	
	RProperty iProperty;
	
	Meta::SMetaData* iUsrData;
	};

class CPSSubscribe : public CNetSubscribe
/**
 * Specialisation of CNetSubscribe to accomodate publish/subscribe signal/transport mechanism
 *
 * @see CNetSubscribe
 * @internalComponent
 */
	{

protected:
	virtual void RegisterNewSignalL(TEventClientData& aData, const SSignalId& aSignalId );
	CPSSignal* Signal( TInt aIndex ) const
		{
		return static_cast<CPSSignal*>(iSignals[aIndex]);
		}
	};

} // namespace NetSubscribe

#endif // __NETPSSIGNAL_H_

