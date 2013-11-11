// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/** 
 * @file OSRTBaseType.h 
 * C++ run-time base class for structured type definitions.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTBASETYPE_H_
#define _OSRTBASETYPE_H_

#include "rtxsrc/OSRTContext.h"

/** 
 * C++ structured type base class. This is the base class for all
 * generated structured types. 
 */
class EXTRTCLASS OSRTBaseType {
 public:
   OSRTBaseType() { }
   virtual ~OSRTBaseType() {}
   virtual OSRTBaseType* clone () const { return 0; }
} ;

#endif //_OSRTBASETYPE_H_
