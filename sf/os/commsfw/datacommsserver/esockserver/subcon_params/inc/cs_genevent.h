// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic Event class.
// XEventBase exists to support interworking with CSubConNotificationEvent class.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef __CS_GENEVENT_H__
#define __CS_GENEVENT_H__

#include <comms-infras/es_event.h>
#include <es_sock.h>
#include <comms-infras/ss_parameterfamilybundle.h>

const TUint32 KEventParamsGranted      =   0x1;
const TUint32 KEventParamsChanged      =   0x2;

class XParameterSetBase;
class CEventParamsGranted : public ESock::XPlaneEvent
	{
public:
	inline static CEventParamsGranted* NewL(TUint32 aFamilyId);

	inline CEventParamsGranted();

	inline const XParameterSetBase* FindParameterSet(TUint32 aFamilyId, TUint aIndex) const;
	inline void AddParameterSetL(TUint32 aFamilyId, XParameterSetBase* aSet);
	inline TInt CountParameterSets(TUint32 aFamilyId);

	virtual ~CEventParamsGranted();

protected:
	DATA_VTABLE

	RParameterFamilyBundle iFamilyBundle;
	};


class CEventParamsChanged : public CEventParamsGranted
	{
public:
	inline static CEventParamsChanged* NewL(TUint32 aFamilyId);
	inline CEventParamsChanged();
	
	inline TInt Error() const;
	inline void SetError(TInt aError);
	
	virtual ~CEventParamsChanged();

protected:
	DATA_VTABLE

	TInt iError;
	};


class CPlaneEventFactory : public CBase
	{
public:
	static ESock::XPlaneEvent* NewL(TAny* aConstructionParameters);
	};


#include "cs_genevent.inl"

#endif
