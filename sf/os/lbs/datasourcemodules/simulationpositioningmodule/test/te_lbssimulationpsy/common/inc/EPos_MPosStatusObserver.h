/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//
//  EPos_MPosStatusObserver.h
//
// Copyright (c) Symbian Software Ltd 2004.  All rights reserved.
//

#ifndef __EPOS_MPOSSTATUSOBSERVER_H__
#define __EPOS_MPOSSTATUSOBSERVER_H__

#include <e32base.h>

// FORWARD DECLARATIONS
class TPositionModuleStatus;

// CLASS DECLARATION

/**
Interface implemented by the EPos server to listen to PSY status
@internalComponent
@released
*/
class MPosStatusObserver
    {
public:  // New functions
/**
Reports that the positioner status has changed.

@param aImplementationUid The UID of the CPositioner implementation 
which is the originator of the status updates.
@param aStatus The new status.
*/
	virtual void ReportStatus(const TPositionModuleId& aImplementationUid,const TPositionModuleStatus& aStatus) = 0;
    };  

#endif  // __EPOS_MPOSSTATUSOBSERVER_H__

// End of File
