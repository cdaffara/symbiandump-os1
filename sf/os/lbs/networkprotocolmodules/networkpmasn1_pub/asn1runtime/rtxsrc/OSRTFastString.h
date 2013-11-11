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
 * @file OSRTFastString.h 
 * C++ fast string class definition.  This can be used to hold standard ASCII 
 * or UTF-8 strings.  This string class implementations directly assigns 
 * any assigned pointers to internal member variables.  It does no memory 
 * management.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTFASTSTRING_H_
#define _OSRTFASTSTRING_H_

#include "rtxsrc/rtxCommon.h"
#include "rtxsrc/rtxPrint.h"

/** 
 * C++ fast string class definition.  This can be used to hold standard ASCII 
 * or UTF-8 strings.  This string class implementations directly assigns 
 * any assigned pointers to internal member variables.  It does no memory 
 * management.
 */
class EXTERNRT OSRTFastString : public OSRTStringIF {
 protected:
   const OSUTF8CHAR* mValue;

 public:
   /**
    * The default constructor sets the internal string member variable 
    * pointer to null.
    */
   OSRTFastString();

   /**
    * This constructor initializes the string to contain the given 
    * standard ASCII string value. 
    *
    * @param strval       - Null-terminated C string value
    */
   OSRTFastString (const char* strval);

   /**
    * This constructor initializes the string to contain the given 
    * UTF-8 string value. 
    *
    * @param strval       - Null-terminated C string value
    */
   OSRTFastString (const OSUTF8CHAR* strval);

   /**
    * Copy constructor.  String data is not copied; the pointer is 
    * simply assigned to the target class member variable.
    *
    * @param str          - C++ string object to be copied.
    */
   OSRTFastString (const OSRTFastString& str);

   /**
    * The destructor does nothing.
    */
   virtual ~OSRTFastString();

   /**
    * This method creates a copy of the given string object.
    */
   virtual OSRTStringIF* clone () { return new OSRTFastString (*this); }

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
   virtual void print (const char* name) { rtxPrintCharStr (name, mValue); }

   /**
    * This method sets the string value to the given string.
    *
    * @param str - C null-terminated string.
    */
   virtual void setValue (const char* str);

   /**
    * This method sets the string value to the given UTF-8 string value.
    *
    * @param str - C null-terminated UTF-8 string.
    */
   virtual void setValue (const OSUTF8CHAR* str);

   /**
    * Assignment operator.
    */
   OSRTFastString& operator= (const OSRTFastString& original);
} ;

#endif
