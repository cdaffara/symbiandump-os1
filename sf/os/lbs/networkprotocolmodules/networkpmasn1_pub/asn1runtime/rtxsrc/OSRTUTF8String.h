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
 * @file OSRTUTF8String.h 
 * C++ UTF-8 string class definition.
 */

/**
@file
@internalTechnology
*/

#ifndef _OSRTUTF8STRING_H_
#define _OSRTUTF8STRING_H_

#include "rtxsrc/OSRTBaseType.h"
#include "rtxsrc/rtxPrint.h"
#include "rtxsrc/rtxUTF8.h"

/** 
 * UTF-8 string. This is the base class for generated C++ data type
 * classes for XSD string types (string, token, NMTOKEN, etc.).
 */
class EXTERNRT OSRTUTF8String : public OSRTBaseType {
   const OSUTF8CHAR* mValue;

 public:
   /**
    * The default constructor creates an empty string.
    */
   OSRTUTF8String();

   /**
    * This constructor initializes the string to contain the given 
    * character string value. 
    *
    * @param strval       - String value
    */
   OSRTUTF8String (const char* strval);

   /**
    * This constructor initializes the string to contain the given 
    * UTF-8 character string value. 
    *
    * @param strval       - String value
    */
   OSRTUTF8String (const OSUTF8CHAR* strval);

   /**
    * Copy constructor.
    *
    * @param str          - C++ XML string class.
    */
   OSRTUTF8String (const OSRTUTF8String& str);

   /**
    * The destructor frees string memory if the memory ownership flag
    * is set.  
    */
   virtual ~OSRTUTF8String();

   /**
    * Clone method. Creates a copied instance and returns pointer to 
    * OSRTBaseType.
    */
   OSRTBaseType* clone () const { return new OSRTUTF8String (*this); }

   /**
    * This method copies the given string value to the internal 
    * string storage variable.  A deep-copy of the given value is done; 
    * the class will delete this memory when the object is deleted.
    *
    * @param str - C null-terminated string.
    */
   void copyValue (const char* str);

   /**
    * This method returns the pointer to C null terminated string.
    */
   inline const char* c_str () const {
      return mValue;
   }

   /**
    * This method returns the pointer to UTF-8 null terminated string.
    */
   inline const char* getValue () const {
      return mValue;
   }

   /**
    * This method prints the string value to standard output.
    * 
    * @param name - Name of generated string variable.
    */
   inline void print (const char* name) { rtxPrintCharStr (name, mValue); }

   /**
    * This method sets the string value to the given string.
    * A deep-copy of the given value is not done; the pointer is 
    * stored directly in the class member variable.
    *
    * @param str - C null-terminated string.
    */
   void setValue (const char* str);

   /**
    * Assignment operator.
    */
   OSRTUTF8String& operator= (const OSRTUTF8String& original);
} ;

#endif
