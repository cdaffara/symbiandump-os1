/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef LBS_LOCMONITORDB_H
#define LBS_LOCMONITORDB_H


//-------------------------------------------------------------------------------
#include <e32base.h>
#include <lbspositioninfo.h> 
#include <lbscommon.h>
#include <lbsareainfo.h>
#include "lbslocmonitorareainfodefs.h"

class CLbsLocMonitorDbEngine;
//-------------------------------------------------------------------------------

class RLbsLocMonitorDb
	{
public:		
	RLbsLocMonitorDb();
	~RLbsLocMonitorDb();
	void OpenL();
	void Close();
	TInt SavePosition(const TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TBool aUserPosition, TRequestStatus& aStatus);
	TInt CancelSavePosition();
	TInt GetPosition(TPosition& aPosition, const RPointerArray<TLbsLocMonitorAreaInfoBase>& aAreaInfo, TPositionAreaExtendedInfo& aMatchingAreaInfo, TRequestStatus& aStatus);
	TInt CancelGetPosition();
	TInt GetLastStoredPosition(TPosition& aPosition, TRequestStatus& aStatus);
	TInt CancelGetLastStoredPosition();
	TInt ClearDatabase();
	
private:
	CLbsLocMonitorDbEngine* iDbEngine;
	};



#endif //LBS_LOCATIONMONITORDATABASE_H
