/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Trace macros of CommonEngine
*
*/


// MACROS

// DEFINES

#ifdef _DEBUG	// Debug build
	#define COMMON_RDEBUG(X)				RDebug::Print(X)		
    #define COMMON_RDEBUG_INT(X,Y)			RDebug::Print(X,Y)
    
#else	// hw environment
	#define COMMON_RDEBUG(X)
	#define COMMON_RDEBUG_INT(X,Y)	
    		
#endif

//
// Use "macro _RD_COMMONENGINE_DETAILED_TRACE"" in commonengine.mmp file 
// to enable detailed trace of Commonengine.
//
#ifdef _RD_COMMONENGINE_DETAILED_TRACE	// Debug environment
	#define COMMONENGINE_RDEBUG(X)				RDebug::Print(X)
	#define COMMONENGINE_RDEBUG_SUBSTITUTE(X)	RDebug::Print(_L("%S"), &X) 	
    #define COMMONENGINE_RDEBUG_INT(X,Y)		RDebug::Print(X,Y)
    #define COMMONENGINE_RDEBUG_INT2(X,Y,Z)		RDebug::Print(X,Y,Z)
    #define COMMONENGINE_RDEBUG_INT3(X,Y,Z,Q)	RDebug::Print(X,Y,Z,Q)
    #define COMMONENGINE_RDEBUG_INT4(X,Y,Z,Q,W)	RDebug::Print(X,Y,Z,Q,W)	
  
#else	// hw environment
	#define COMMONENGINE_RDEBUG(X)	
	#define COMMONENGINE_RDEBUG_SUBSTITUTE(X)
    #define COMMONENGINE_RDEBUG_INT(X,Y)
    #define COMMONENGINE_RDEBUG_INT2(X,Y,Z)    
    #define COMMONENGINE_RDEBUG_INT3(X,Y,Z,Q)
    #define COMMONENGINE_RDEBUG_INT4(X,Y,Z,Q,W)
	
#endif

//
// Use "macro _RD_FEATMGR_DETAILED_TRACE" in featmgr.mmp file 
// to enable detailed trace of FeatureManager.
//
#ifdef _RD_FEATMGR_DETAILED_TRACE	// Debug build
	#define FEATMGR_RDEBUG(X)						RDebug::Print(X)		
    #define FEATMGR_RDEBUG_INT(X,Y)					RDebug::Print(X,Y)
    
#else	// hw environment
	#define FEATMGR_RDEBUG(X)
	#define FEATMGR_RDEBUG_INT(X,Y)	
    		
#endif


            
// End of File
