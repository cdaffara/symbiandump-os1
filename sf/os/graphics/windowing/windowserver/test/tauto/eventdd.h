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

#ifndef __EVENTDD_H__
#define __EVENTDD_H__

#ifndef __E32cmn_H__
#include <e32cmn.h>
#endif

_LIT(KName,"EventDD");


class REventDD : public RBusLogicalChannel
	{
	friend class DEventDD;
public:
	/**
	Structure for holding driver capabilities information
	(Just a version number in this example.)
	*/
	class TCaps
		{
	public:
		TVersion iVersion;
		};

public:
	inline static const TDesC& BinaryName();
	inline static const TDesC& DriverName();
	inline static TVersion VersionRequired();
	inline TInt Open();
	inline TInt SendEvent(TRawEvent& aEvent);
private:
	enum TControl
		{
		ESendEvent
		};
	};


inline const TDesC& REventDD::BinaryName()
	{
	return KName;
	}
inline const TDesC& REventDD::DriverName()
	{
	return KName;
	}
inline TVersion REventDD::VersionRequired()
	{
	return TVersion(1,0,0);
	}
inline TInt REventDD::Open()
	{
	return DoCreate(DriverName(),VersionRequired(),KNullUnit,NULL,NULL,EOwnerThread);
	}
inline TInt REventDD::SendEvent(TRawEvent& aEvent)
	{
	return DoControl(ESendEvent,static_cast<TAny*>(&aEvent));
	}

#endif	//__EVENTDD_H__
