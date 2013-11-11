/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the ?classname class.
*
*/


#ifndef __ACCREMCONPSTYPES_H__
#define __ACCREMCONPSTYPES_H__

#include <e32base.h>
#include <RemConKeyEventData.h>
#include <remconcoreapi.h>
#include <remconextensionapi.h>

const TUid 	  KTFRemConTestProperty={0x10205083}; ///Category
const TUint32 KTFRemConTriggerCmdKey       = 0x00001111;//Key for NewCommand trigger
const TUint32 KTFRemConTriggerNotifyCmdKey = 0x00011111;//Key for NewNotifyCommand trigger
const  TUint32 KTFRemConClientKey = 0x00001112; 

struct TRemConStubPSData
    {
    TRemConKeyEventData 		          iKeyEvent;
    //No keyevent converter used.
    TInt  iOperationId;
    TBuf8<20>  iData;
    TBool iUseConverter;
    };

struct TRemConClientKeyData
    {
    TRemConCoreApiOperationId iOperationId;
    TRemConCoreApiButtonAction iButtonAct;
    };

typedef TPckgBuf<TRemConClientKeyData> TRemConClientKeyDataBuf;

typedef TPckgBuf <TRemConKeyEventData> TRemConKeyEventDataBuf;



typedef TPckgBuf< TRemConStubPSData > TRemConPckgStubBuf;


const TUid 	  KTFPhoneCmdHandlerProperty={0x10200C6F}; ///Category
const TUint32 KTFPhoneCmdHandlerKey=0x00000111;//Key for future

struct TRemConPhoneCmdHandlerPsData
    {
    TUint 		iOperationId;
    };

typedef TPckgBuf <TRemConPhoneCmdHandlerPsData> TPhoneCmdHandlerKeyEventBuf;



#endif
