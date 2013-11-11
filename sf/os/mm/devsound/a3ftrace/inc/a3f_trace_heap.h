/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef _A3F_TRACE_HEAP_H_
#define _A3F_TRACE_HEAP_H_


/**
*  TraceHeap is a container holding the bitmask that defines active trace groups
*  This class is used to hold the debug mask of the inspected process
*/
class TraceHeap
    {
    public:
    	IMPORT_C static void    CreateL(const TUint aDefaultMask=0);
    	IMPORT_C static TUint32 TraceMask();
    	IMPORT_C static void	   SetTraceMask( const TUint32 aTraceMask );
    	IMPORT_C static TBool   IsMaskOn( const TUint32 aMask );
    	IMPORT_C static void	   SetLevel( const TUint32 aDbgLevel );
    	IMPORT_C static TBool   IsBelowLevel( const TUint32 aLevel );
    	IMPORT_C static TUint32 TraceLevel();
    	IMPORT_C static TBool   IsApiOnly();
    	IMPORT_C static void    SetApiOnly( const TBool aApiOnly);
    	
    private: 
    	static TUint32* 	iTraceMask;
    	static TUint32* 	iTraceLevel;
    	static TBool* 	    iApiOnly;
    	static RHeap* 		iTraceHeap;
    };

#endif      // _A3F_TRACE_HEAP_H_
            
// End of File
