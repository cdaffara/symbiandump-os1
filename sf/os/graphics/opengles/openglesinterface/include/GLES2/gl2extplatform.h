/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __GL2EXTPLATFORM_H__
#define __GL2EXTPLATFORM_H__

/*
 * The purpose of this header file is to allow a platform to control which
 * gl2ext.h extensions are seen, in order to enforce source-level backwards
 * compatiblity.
 * Consider the example of disabling GL_DEPTH_COMPONENT24_OES.  From gl2ext.h
 * we see this is guarded by the define GL_OES_depth24.  You need to define
 * GL_OES_depth24 in this file in order to disable this extension from being
 * seen by programs compiled against gl2.h and gl2ext.h headers.
 */

/**
@publishedAll
@released
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Place #define statements as needed below to disable gl2ext.h extensions */

#ifdef __cplusplus
}
#endif

#endif
