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
// Interface for Render Stage Display Mapping
// 
//
/**
@file
@publishedpartner 
@released
*/

#ifndef WSDISPLAYMAPPING_H_
#define WSDISPLAYMAPPING_H_

#include <graphics/displaymapping.h>

class MWsDisplayMapping: public MWsObjectProvider,public MDisplayMappingBase
	{
public:
	enum	
		{
		KUidWsDisplayMapping =  0x1028656C, 	 
		ETypeId=				KUidWsDisplayMapping,
		EWsObjectInterfaceId=	KUidWsDisplayMapping
		};
	enum	TOffsetFlags
		{
		KOffsetAll=	0xffffffff,
		KOffGc=		0x00000001,
		KOffMap=	0x00000002,
		KOffElement=0x00000004,
		};
public:
	virtual void SetSizeModeExtent(TRect& aExtent, TBitFlags32 aContext)=0;
	};

#endif /*WSDISPLAYMAPPING_H_*/
