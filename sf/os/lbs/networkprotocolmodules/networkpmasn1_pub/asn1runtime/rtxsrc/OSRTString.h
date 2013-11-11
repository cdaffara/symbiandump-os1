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
 * @file OSRTString.h 
 * C++ string class definition.  This can be used to hold standard ASCII 
 * or UTF-8 strings.  The standard C++ 'new' and 'delete' operators are 
 * used to allocate/free memory for the strings.  All strings are 
 * deep-copied.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTSTRING_H_
#define _OSRTSTRING_H_

/* Disable MS VC++ Level 4 warning about unreferenced inline functions */
#ifdef _MSC_VER
#pragma warning(disable: 4514)
#endif /* _MSC_VER */

#include "rtxsrc/rtxCommon.h"
#include "rtxsrc/rtxPrint.h"
#include "rtxsrc/OSRTStringIF.h"

/** 
 * C++ string class definition.  This can be used to hold standard ASCII 
 * or UTF-8 strings.  The standard C++ 'new' and 'delete' operators are 
 * used to allocate/free memory for the strings.  All strings are 
 * deep-copied.
 */
class EXTRTCLASS OSRTString : public OSRTStringIF {
 protected:
   OSUTF8CHAR* mValue;

 public:
   /**
    * The default constructor creates an empty string.
    */
   EXTRTMETHOD OSRTString();

   /**
    * This constructor initializes the string to contain the given 
    * standard ASCII string value. 
    *
    * @param strval       - Null-terminated C string value
    */
   EXTRTMETHOD OSRTString (const char* strval);

   /**
    * This constructor initializes the string to contain the given 
    * UTF-8 string value. 
    *
    * @param strval       - Null-terminated C string value
    */
   EXTRTMETHOD OSRTString (const OSUTF8CHAR* strval);

   /**
    * Copy constructor.
    *
    * @param str          - C++ string object to be copied.
    */
   EXTRTMETHOD OSRTString (const OSRTString& str);

   /**
    * The destructor frees string memory using the standard 'delete' 
    * operator.
    */
   virtual EXTRTMETHOD ~OSRTString();

   /**
    * This method creates a copy of the given string object.
    */
   virtual OSRTStringIF* clone () { return new OSRTString (*this); }

   /**
    * This method returns the pointer to UTF-8 null terminated string 
    * as a standard ASCII string.
    */
   virtual const char* getValue () const {
      return (const char*) mValue;
   }

   /**
    * This method returns the pointer to UTF-8 null terminated string 
    * as a UTF-8 string.
    */
   virtual const OSUTF8CHAR* getUTF8Value () const {
      return mValue;
   }

   /**
    * This method prints the string value to standard output.
    * 
    * @param name - Name of generated string variable.
    */
   virtual void print (const char* name) { 
      rtxPrintCharStr (name, (const char*)mValue); 
   }

   /**
    * This method sets the string value to the given string.
    *
    * @param str - C null-terminated string.
    */
   virtual EXTRTMETHOD void setValue (const char* str);

   /**
    * This method sets the string value to the given UTF-8 string value.
    *
    * @param str - C null-terminated UTF-8 string.
    */
   virtual EXTRTMETHOD void setValue (const OSUTF8CHAR* str);

   /**
    * Assignment operator.
    */
   EXTRTMETHOD OSRTString& operator= (const OSRTString& original);
} ;

#endif
