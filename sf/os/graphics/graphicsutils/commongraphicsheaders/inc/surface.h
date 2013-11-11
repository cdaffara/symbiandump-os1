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
// surface
// 
//

/**
 @file
 @publishedPartner
 @released
*/
#ifndef __SURFACE_H__
#define __SURFACE_H__

#include <e32def.h>

class TSurfaceId
	{
	public:
	enum
		{
		EInvalidSurface = 0,
		EScreenSurface = 1,
		EStreamHandle = 2,
		EAdaptationSurfaceBase = 33,
		};
	struct TSurfaceUsage 			 //TSurfaceId::TSurfaceUsage::
		{
		enum	
			{
			//screen usage
			EScreenField=0,
			EHalField=1,
			EHalFlippedFlag=0x80000000,
			ETypeGuidField=2,
			//stream handle usage
			EObjectRefField=0,
			//general class usage
			ETypeClassField=3,
			ETypeClassShift=24
			};
		};
	typedef TSurfaceUsage TScreenSurfaceUsage; 			 //Backward compatibility
	
	
	inline static TSurfaceId CreateNullId(void) 
		{
		TSurfaceId id = TSurfaceId();
		for (TInt i = 0; i < 4; ++i) id.iInternal[i] = 0; 
		return id;
		};
	inline TBool IsNull() const 
		{
		return ((iInternal[0] == 0) &&
				(iInternal[1] == 0) &&
				(iInternal[2] == 0) &&
				(iInternal[3] == 0));
			
		};
	inline TInt Type() const {return (TInt)(iInternal[3] >> 24);}
	public:
	TUint32 iInternal[4];
	};


inline bool operator == (const TSurfaceId& lhs, const TSurfaceId& rhs)
	{
	return ((lhs.iInternal[0] == rhs.iInternal[0]) &&
			(lhs.iInternal[1] == rhs.iInternal[1]) &&
			(lhs.iInternal[2] == rhs.iInternal[2]) &&
			(lhs.iInternal[3] == rhs.iInternal[3]));
	}

inline bool operator != (const TSurfaceId& lhs, const TSurfaceId& rhs)
	{
	return !(lhs == rhs);
	}

#endif

