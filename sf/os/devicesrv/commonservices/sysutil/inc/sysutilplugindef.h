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
//

#ifndef SYSUTILPLUGINDEF_H
#define SYSUTILPLUGINDEF_H

/** 
@file
@publishedPartner
@released
*/

#include <e32cmn.h>
#include <badesca.h>

namespace SysUtilPlugin
	{
	enum TPluginOrdinal
		{
		/**
		GetDeviceAttributesAsArraysL function ordinal
		*/
		EGetDeviceAttributesAsArraysLOrdinal = 1
		};
	
	/**
	 Retrieves a set of Device Type Information Attribute UIDs and a set of Device Type 
	 Information Attribute values. 
	 
	 Note: The values in the RArray<TInt32> array and the CDesC16Array& array 
	 correspond with each other and the values are sorted in UID order from low to high.
	 
	 @param RArray<TInt32>&		On return, contains an ordered set of UIDs which represent 
								device attributes.
	 @param CDesC16ArraySeg&s	On return, contains a set of device attribute values which 
								must be in the same order as aKeys, i.e. the order of 
								attributes and keys must correspond.
	 @leave -					One of the system-wide error codes.
	 
	 @capability ReadDeviceData Function implementation may access RDD protected APIs
	*/
	typedef void (*GetDeviceAttributesAsArraysFuncL)(RArray<TInt32>&, CDesC16Array&);
	}

#endif	// SYSUTILPLUGINDEF_H
