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
* Description:  This file contains definitions of Audio Clients List
*                Publish and Subscribe keys.
*
*/


#ifndef AUDIOCLIENTSLISTPSDATA_H
#define AUDIOCLIENTSLISTPSDATA_H

/**
* TClientCategory
* Client category. Internal use only.
*/
enum TClientCategory
    {
    };

/**
* KAudioPolicyMaxProcessId
* The max number of PIDs in the list.
*/
const TInt KAudioPolicyMaxNumProcess = 10;

/**
* TAudioPolicyProcessIdListStruct
* Defines the structure which holds process IDs.
*/
struct TAudioPolicyProcessIdListStruct
    {
    TInt iNumOfProcesses;
    TProcessId iProcessList[KAudioPolicyMaxNumProcess];	
    };

/**
* TAudioPolicyProcessIdList
* Package buffer which holds process IDs.
*/
typedef TPckgBuf<TAudioPolicyProcessIdListStruct> TAudioPolicyProcessIdList;

/**
* TAudioClientListStruct
* Defines the structure which holds process IDs and audio categories.
*/
struct TAudioClientListStruct
    {
    TInt iNumOfProcesses;
    TProcessId iProcessList[KAudioPolicyMaxNumProcess];
    TClientCategory iClientCategoryList[KAudioPolicyMaxNumProcess];
    };

/**
* TAudioClientList
* Package buffer which holds process IDs and audio categories.
*/
typedef TPckgBuf<TAudioClientListStruct> TAudioClientList;

#endif      // AUDIOCLIENTSLISTPSDATA_H

// End of file
