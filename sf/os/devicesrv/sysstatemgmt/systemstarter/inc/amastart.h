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
*/

#ifndef __AMASTART_H__
#define __AMASTART_H__


class CStartSafe;
class CDscItem;

#include <sysmonclisess.h>
#include <dscstore.h>

/**
 * Used to start After Market Applications (AMA).
 * @internalTechnology
 * @released
 */
class CAmaStart : public CBase
	{
public:
	IMPORT_C static CAmaStart* NewL();
	IMPORT_C void StartL(const TUid aDscId);
	IMPORT_C ~CAmaStart();
	
private:
	CAmaStart();
	void ConstructL();

	void StartDscItemL(const CDscItem& aDscItem);

private:
	RDscStore iDscStore;
	CStartSafe* iStartSafe;
	RSysMonSession iSysMon;
	};


#endif // __AMASTART_H__
