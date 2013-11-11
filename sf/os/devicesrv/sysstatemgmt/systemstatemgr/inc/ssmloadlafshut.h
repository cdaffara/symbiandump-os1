// Copyright (c)2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Shim dll used to remove the dependency on LafShutdown for creating shutdown manager.
//

#ifndef __SSMLOADLAFSHUT_H__
#define __SSMLOADLAFSHUT_H__

#include <shutdownsrv.h>
/**
MSsmLoadLafShut is used to remove the dependency on LafShutdown for creating shutdown manager.

@internalAll
*/
class MSsmLoadLafShut
    {
public:
    virtual void CreateShutdownManager(MShutdownEventObserver& aObserver) = 0;
    virtual void DeleteShutdownManager() = 0;
    };

#endif  //__SSMLOADLAFSHUT_H__

