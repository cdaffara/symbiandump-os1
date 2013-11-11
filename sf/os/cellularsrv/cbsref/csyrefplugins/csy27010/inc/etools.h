//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description: This file defines a tool for putting a name to the class
// *              when a memory leak occurs, you can trace it back by looking 
//                at the memory space.
//

// etools.h

/** @file etools.h
 *
 */

#ifndef ETOOLS_H__
#define ETOOLS_H__

#include <e32std.h>
#include <e32panic.h>
#include <e32svr.h>

// Tool for putting a name to the class
// when a memory leak occurs, you can trace it back
// by looking at the memory space!
// the for(;;) is a bit awkward
#define CLASSNAMEDECL(a) char __iName[sizeof(#a)];

#ifdef _DEBUG
#define CLASSNAMEINIT(a) \
	char tmp[] = #a; \
	for(TUint _i_##a=0;_i_##a<sizeof(tmp);_i_##a++) __iName[_i_##a] = tmp[_i_##a]; 
#define CLASSNAME this->__iName
#else
#define CLASSNAMEINIT(a)
#define CLASSNAME "?"
#endif

//
// put this in your constructor/destructor to see the memory usage
//
#if defined __DEBUGLOGFILE__
#define CTOR(a)	\
	_LOGF4("+++ object created at 0x%08x (%d bytes)\t%s ", this, sizeof(a) - __Align8(sizeof(__iName)), #a );
#define DTOR(a)	\
	_LOGF4("--- object deleted at 0x%08x (%d bytes)\t%s", this, sizeof(a) - (4+(sizeof(__iName) & ~0x03)), #a );
#else
#define CTOR(a)
#define DTOR(a)
#endif

//
// size of arrays
//

#ifndef ELEM_SIZE
#define ELEM_SIZE(t) (sizeof(t)/sizeof(t[0]))
#endif

#endif // ETOOLS_H__
