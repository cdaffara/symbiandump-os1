// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSLASCOMMON_H
#define LBSLASCOMMON_H

//************************************************************************************************************
#include <e32property.h>
#include <e32base.h>
#include <lbs/LbsLocDataSourceGpsBase.h>
#include <lbs.h>
 
//************************************************************************************************************
// Property Category
const TUid KLasPropertyCat={0x10012345};

//************************************************************************************************************
// Property Keys
enum TPropertyKeys
   {
   ELasLocServerReadyBus	=  998,
   ELasModuleInfoIndexBus	=  999,
   ELasModuleInfoBus			= 1000,
   ELasModuleStatusBus		= 2000,
   ELasLocationRequestBus	= 3000,
   ELasLocationDataBus		= 4000
   };


class TLasRequest
	{

public:
	enum TLasRequestType 
		{
		ELasRequestLocation, ELasRequestCancel, ELasRequestMode		
		};

	TInt iVersion;
	TLasRequestType iRequest;
	
	TTime iTargetTime;
	CLbsLocationSourceGpsBase::TPowerMode iMode;
	};

const TInt KMaxLasModules = 10;

class TLasModuleInfoIndex
	{
public:
	TInt    iVersion;
	TPositionModuleId iDefaultModuleId;	
	TInt    iNumModules;
	TPositionModuleId iModules[KMaxLasModules];
	};


#include <flogger.h>
#define LASDEBDIR		_L("Lbs")
#define LASDEBFILE	_L("LocManGps.txt")

#define LASLOGNEW(L)			RFileLogger::Write(LASDEBDIR, LASDEBFILE, EFileLoggingModeOverwrite, _L8(L))
#define LASLOG(L)				RFileLogger::Write(LASDEBDIR, LASDEBFILE, EFileLoggingModeAppend, _L8(L))
#define LASLOG2(L1,L2)		RFileLogger::WriteFormat(LASDEBDIR, LASDEBFILE, EFileLoggingModeAppend, _L8(L1), L2)
#define LASLOG3(L1,L2,L3)	RFileLogger::WriteFormat(LASDEBDIR, LASDEBFILE, EFileLoggingModeAppend, _L8(L1), L2, L3)
#define LASLOG4(L1,L2,L3,L4)	RFileLogger::WriteFormat(LASDEBDIR, LASDEBFILE, EFileLoggingModeAppend, _L8(L1),L2,L3,L4)

#define LASLOGDES(L)		RFileLogger::Write(LASDEBDIR, LASDEBFILE, EFileLoggingModeAppend, (L))


#endif //LBSLASCOMMON_H

