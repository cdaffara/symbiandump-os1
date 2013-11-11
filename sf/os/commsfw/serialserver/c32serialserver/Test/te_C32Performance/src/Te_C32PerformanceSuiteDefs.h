/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file
 @internalComponent
*/
#if (!defined TE_C32PERFORMANCE_SUITEDEFS_H)
#define TE_C32PERFORMANCE_SUITEDEFS_H

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_C32PerformanceSuiteString,"TheString");
_LIT(KTe_C32PerformanceSuiteInt,"TheInt");
_LIT(KTe_C32PerformanceSuiteBool,"TheBool");

const TInt KMaxTaskNameLength       = 15;
const TInt KMaxThreadNameLength     = 15;
const TInt KMaxThreadPriorityLength = 30;

//Total no of fields in the ini file.This value needs to be updated if total 
//no of fields in the ini file is changed. 
const TUint KTotalIniFieldsNo = 4;

//No of client threads that can be spawned.
const TUint KMaxTestClientThreads 	= 8;

//Name of the global semaphore.
_LIT(KSemaphoreName,"SemaphoreName");

_LIT(KSemaphoreCountSection,"TotalNoofSemaphores");
_LIT(KTotalNoofSemaphores,"SemaphoreCount");

//These constants are used to represent different fields in the ini file
_LIT(KThreadSection,"Thread");
_LIT(KThreadName,"Name");
_LIT(KTaskName,"Task");
_LIT(KThreadPriority,"Priority");
_LIT(KSemaphore,"Semaphore");
_LIT(KLastThread,"LastThread");

//Thread priorities
_LIT(KEPriorityNull ,"EPriorityNull");
_LIT(KEPriorityMuchLess ,"EPriorityMuchLess");
_LIT(KEPriorityLess ,"EPriorityLess");
_LIT(KEPriorityNormal ,"EPriorityNormal");
_LIT(KEPriorityMore ,"EPriorityMore");
_LIT(KEPriorityMuchMore ,"EPriorityMuchMore");
_LIT(KEPriorityRealTime ,"EPriorityRealTime");
_LIT(KEPriorityAbsoluteVeryLow ,"EPriorityAbsoluteVeryLow");
_LIT(KEPriorityAbsoluteLow ,"EPriorityAbsoluteLow");
_LIT(KEPriorityAbsoluteBackground ,"EPriorityAbsoluteBackground");
_LIT(KEPriorityAbsoluteForeground ,"EPriorityAbsoluteForeground");
_LIT(KEPriorityAbsoluteHigh ,"EPriorityAbsoluteHigh");


// For test step panics
_LIT(KTe_C32PerformanceSuitePanic,"Te_C32PerformanceSuite");

#endif

