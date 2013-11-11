/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Project configure file
*
*/


#ifndef ATEXT_PRJCONFIG_H
#define ATEXT_PRJCONFIG_H

/**
 * Traces are enabled in _DEBUG build, by default.
 */
#ifdef _DEBUG
#define PRJ_ENABLE_TRACE
#endif

/**
 * traces to file if this is defined.
 */
//#define PRJ_FILE_TRACE


/**
 * build the project for module test purpose if this is defined
 */
//#define PRJ_MODULETEST_BUILD

/**
 * build the project using stubs to replace the dependencies if this is defined
 */
//#define PRJ_USE_STUB


#endif // ATEXT_PRJCONFIG_H
