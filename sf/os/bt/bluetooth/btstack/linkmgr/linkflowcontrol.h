// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LinkFlowControlFlow.h
// HCTL flow control
// 
//

#ifndef LINKFLOWCONTROLFLOW_H__
#define LINKFLOWCONTROLFLOW_H__
//#include "hci.h"



enum TFlowControlMode
    {
    ENoFlowControl=0,

    //host controls HC buffering and informs L2CAP about HC congestion
    //Flow of data going to HC
    EFlowControlToHostControllerOnly=1, 

    //host controller tracks and respects host buffers, L2CAP informs HCI about
    //buffer consumption and HCI tells HC about it.
    // flow of data coming from HC
    EFlowControlFromHostControllerOnly=2,
    ETwoWayFlowControlEnabled=3
    };
#endif
