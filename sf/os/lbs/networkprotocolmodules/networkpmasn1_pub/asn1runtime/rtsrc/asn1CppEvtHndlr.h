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

// rtsrc/asn1CppEvtHndlr.h - this .h file defines the object that is registered 
// for the C++ version of the event handler feature.

/**
 * @file rtsrc/asn1CppEvtHndlr.h 
 * Named event handler base class.  The Asn1Named Event Handler class is 
 * an abstract base class from which user-defined event handlers are derived. 
 * This class contains pure virtual function definitions for all of the 
 * methods that must be implemented to create a customized event handler  
 * class.
 */
/**
 * @defgroup Asn1NamedEventHandler Named Event Handlers
 * Named Event Handler Classes include base classes from which user-defined  
 * error handler and event handler classes are derived.
 * @ingroup cppruntime
 * @{
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1CPPEVTHNDLR_H_
#define _ASN1CPPEVTHNDLR_H_

#include "rtsrc/asn1type.h"

// The following pure abstract class defines the event handler interface 
// callback functions that are invoked as a message is parsed..
/**
 * Named event handler base class.  This is the base class from which 
 * user-defined event handler classes are derived.  These classes can be 
 * used to handle events during the parsing of an ASN.1 message.  The 
 * event callback methods that can be implemented are startElement, 
 * endElement, and contents methods.
 */
class EXTRTCLASS Asn1NamedEventHandler {
 public:
   Asn1NamedEventHandler() {}
   virtual ~Asn1NamedEventHandler() {}

   // Start element callback function.  This is invoked upon entry into 
   // an element in a constructed type (SEQUENCE, SET, SEQUENCE OF, 
   // SET OF, or CHOICE).  A single char* argument (name) is passed. 
   /**
    * This method is invoked from within a decode function when an element of a
    * SEQUENCE, SET, SEQUENCE OF, SET OF, or CHOICE construct is parsed.
    *
    * @param name         For SEQUENCE, SET, or CHOICE, this is the name of the
    *                       element as defined in the ASN.1 defination. For
    *                       SEQUENCE OF or SET OF, this is set to the name
    *                       "element".
    * @param index        For SEQUENCE, SET, or CHOICE, this is not used and is
    *                       set to the value
    *                       -1. For SEQUENCE OF or SET OF, this contains the
    *                       zero-based index of the element in the conceptual
    *                       array associated with the construct.
    * @return             - none
    */
   virtual void startElement (const char* name, int index) = 0;

   // End element callback function.  This is invoked upon exit from 
   // a constructed type.  A single char* argument (name) is passed.
   /**
    * This method is invoked from within a decode function when parsing is
    * complete on an element of a SEQUENCE, SET, SEQUENCE OF, SET OF, or CHOICE
    * construct.
    *
    * @param name         For SEQUENCE, SET, or CHOICE, this is the name of the
    *                       element as defined in the ASN.1 defination. For
    *                       SEQUENCE OF or SET OF, this is set to the name
    *                       "element".
    * @param index        For SEQUENCE, SET, or CHOICE, this is not used and is
    *                       set to the value
    *                       -1. For SEQUENCE OF or SET OF, this contains the
    *                       zero-based index of the element in the conceptual
    *                       array associated with the construct.
    * @return             - none
    */
   virtual void endElement (const char* name, int index) = 0;

   // Data callback functions.  These are invoked when data contents 
   // are parsed from a field.  A separate callback is invoked for 
   // each of the ASN.1 primitive data types..

   /**
    * This method is invoked from within a decode function when a value of the
    * BOOLEAN ASN.1 type is parsed.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void boolValue (OSBOOL value) = 0;

   /**
    * This method is invoked from within a decode function when a value of the
    * INTEGER ASN.1 type is parsed.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void intValue (OSINT32 value) = 0;

   /**
    * This method is invoked from within a decode function when a value of the
    * INTEGER ASN.1 type is parsed.
    *
    * In this case, constraints on the integer value forced the use of unsigned
    * integer C type to represent the value.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void uIntValue (OSUINT32 value) = 0;

   /**
    * This method is invoked from within a decode function when a value of the
    * 64-bit INTEGER ASN.1 type is parsed.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void int64Value (OSINT64 value) {
      intValue ((OSINT32)value);
   }

   /**
    * This method is invoked from within a decode function when a value of the
    * 64-bit INTEGER ASN.1 type is parsed.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void uInt64Value (OSUINT64 value) {
      uIntValue ((OSUINT32)value);
   }

   /**
    * This method is invoked from within a decode function when a value of the
    * BIT STRING ASN.1 type is parsed.
    *
    * @param numbits      - Number of bits in the parsed value. 
    * @param data         - Pointer to a byte array that contains the bit 
    *                         string data.
    * @return             - none
    */ 
   virtual void bitStrValue (OSUINT32 numbits, const OSOCTET* data) = 0;

   /**
    * This method is invoked from within a decode function when a value of one
    * of the OCTET STRING ASN.1 type is parsed.
    *
    * @param numocts      Number of octets in the parsed value.
    * @param data         Pointer to byte array containing the octet string
    *                       data.
    * @return             - none
    */
   virtual void octStrValue (OSUINT32 numocts, const OSOCTET* data) = 0;

   /**
    * This method is invoked from within a decode function when a value of one
    * of the 8-bit ASN.1 character string types is parsed.
    *
    * @param value        Null terminated character string value.
    * @return             - none
    */
   virtual void charStrValue (const char* value) = 0;

   /**
    * This method is invoked from within a decode function when a value of one
    * of the 16-bit ASN.1 character string types is parsed.
    *
    * This is used for the ASN.1 BmpString type.
    *
    * @param nchars       Number of characters in the parsed value.
    * @param data         Pointer to an array containing 16-bit values.
    *                       These are represented using unsigned short integer
    *                       values.
    * @return             - none
    */
   virtual void charStrValue (OSUINT32 nchars, OSUNICHAR* data) = 0;

   /**
    * This method is invoked from within a decode function when a value of one
    * of the 32-bit ASN.1 characer string types is parsed.
    *
    * This is used for the ASN.1 UniversalString type.
    *
    * @param nchars       Number of characters in the parsed value.
    * @param data         Pointer to an array containing 32-bit values.
    *                       Each 32-bit integer value is a universal character.
    * @return             - none
    */
   virtual void charStrValue (OSUINT32 nchars, OS32BITCHAR* data) = 0;

   /**
    * This method is invoked from within a decode function when a value of the
    * NULL ASN.1 type is parsed.
    *
    * @param             - none
    * @return             - none
    */
   virtual void nullValue () = 0;

   /**
    * This method is invoked from within a decode function whn a value the
    * OBJECT IDENTIFIER ASN.1 type is parsed.
    *
    * @param numSubIds    Number of subidentifiers in the object identifier.
    * @param pSubIds      Pointer to array containing the subidentifier values.
    * @return             -none
    */
   virtual void oidValue (OSUINT32 numSubIds, OSUINT32* pSubIds) = 0;

   /**
    * This method is invoked from within a decode function when a value the
    * REAL ASN.1 type is parsed.
    *
    * @param value        Parsed value.
    * @return             - none
    */
   virtual void realValue (double value) = 0;

   /**
    * This method is invoked from within a decode function when a value of the
    * ENUMERATED ASN.1 type is parsed.
    *
    * @param value        - Parsed enumerated value
    * @param text         - Textual value of enumerated identifier
    * @return             - none
    */
   virtual void enumValue (OSUINT32 value, const OSUTF8CHAR* text) = 0;

   /**
    * This value is invoked from within a decode function when an ASN.1 open
    * type is parsed.
    *
    * @param numocts      Number of octets in the parsed value.
    * @param data         Pointer to byet array contain in tencoded ASN.1
    *                       value.
    * @return             - none
    */
   virtual void openTypeValue (OSUINT32 numocts, const OSOCTET* data) = 0;

   // The following static function is called to add a new event handler 
   // to the context event handler list..

   EXTRTMETHOD static void addEventHandler (OSCTXT* pCtxt, 
                                Asn1NamedEventHandler* pHandler);

   // The following static function is called to remove an event handler 
   // from the context event handler list.  Note that it does not delete 
   // the event handler object.

   EXTRTMETHOD static void removeEventHandler (OSCTXT* pCtxt, 
                                   Asn1NamedEventHandler* pHandler);

   // The following static methods are invoked from within the generated 
   // code to call the various user-defined event handler methods ..

   EXTRTMETHOD static void invokeStartElement (OSCTXT* pCtxt, const char* name, int index);
   EXTRTMETHOD static void invokeEndElement (OSCTXT* pCtxt, const char* name, int index);
   EXTRTMETHOD static void invokeBoolValue (OSCTXT* pCtxt, OSBOOL value);
   EXTRTMETHOD static void invokeIntValue (OSCTXT* pCtxt, OSINT32 value);
   EXTRTMETHOD static void invokeUIntValue (OSCTXT* pCtxt, OSUINT32 value);
   EXTRTMETHOD static void invokeInt64Value (OSCTXT* pCtxt, OSINT64 value);
   EXTRTMETHOD static void invokeUInt64Value (OSCTXT* pCtxt, OSUINT64 value);
   EXTRTMETHOD static void invokeBitStrValue (OSCTXT* pCtxt, OSUINT32 numbits, 
                                  const OSOCTET* data);
   EXTRTMETHOD static void invokeOctStrValue (OSCTXT* pCtxt, OSUINT32 numocts, 
                                  const OSOCTET* data);
   EXTRTMETHOD static void invokeCharStrValue (OSCTXT* pCtxt, const char* value);
   EXTRTMETHOD static void invokeCharStrValue (OSCTXT* pCtxt, OSUINT32 nchars, 
                                   OSUNICHAR* data);
   EXTRTMETHOD static void invokeCharStrValue (OSCTXT* pCtxt, OSUINT32 nchars, 
                                   OS32BITCHAR* data);
   EXTRTMETHOD static void invokeNullValue (OSCTXT* pCtxt);
   EXTRTMETHOD static void invokeOidValue (OSCTXT* pCtxt, OSUINT32 numSubIds, 
                               OSUINT32* pSubIds);
   EXTRTMETHOD static void invokeRealValue (OSCTXT* pCtxt, double value);
   EXTRTMETHOD static void invokeEnumValue (OSCTXT* pCtxt, OSUINT32 value, 
                                const OSUTF8CHAR* text);
   EXTRTMETHOD static void invokeOpenTypeValue (OSCTXT* pCtxt, OSUINT32 numocts, 
                                    const OSOCTET* data);

} ;

// The following pure abstract class defines the error handler interface 
// callback functions that are invoked when a parsing error occurs..

class EXTRTCLASS ASN1MessageBuffer ;

/**
 * Error handler base class.  This is the base class from which 
 * user-defined error classes are derived.  These classes can be 
 * used to provide fault-tolerance when parsing a message.  The normal 
 * decoder behavior is to stop decoding when it encounters an error. 
 * An error handler can be used to ignore or take corrective action 
 * that will allow the decoding process to continue.
 */
class EXTRTCLASS Asn1ErrorHandler {
 public:
   Asn1ErrorHandler() {}
   virtual ~Asn1ErrorHandler() {}

   /**
    * The error handler callback method.  This is the method that the user 
    * must override to provide customized error handling.
    *
    * @param pCtxt - Pointer to a context block structure.
    * @param pCCB - Pointer to a context control block structure.
    * @param stat - The error status that caused the handler to be invoked.
    * @return - Corrected status.  Set to 0 to cause decoding to 
    *   continue or to a negative status code (most likely \c stat) to 
    *   cause decoding to terminate.
    */
   virtual int error (OSCTXT* pCtxt, ASN1CCB* pCCB, int stat) = 0;

   // The following static method is called from within the generated 
   // code to call the virtual error callback method..

   EXTRTMETHOD static int invoke (OSCTXT* pCtxt, ASN1CCB* pCCB, int stat);

   // The following static method is called from within the generated 
   // code to call the virtual error callback method..

   EXTRTMETHOD static int invoke (OSCTXT* pCtxt, OSOCTET* ptr, int len, int stat);

   /**
    * This static method is called to set the error handler 
    * within the context structure.  Note that unlike event handlers, only 
    * a single error handling object can be specified.  This must be 
    * called by the user to specify the error handling object prior to 
    * execution of the main decode function..
    *
    * @param pCtxt - Pointer to a context block structure.
    * @param pHandler - Pointer to error handler object to register.
    */
   EXTRTMETHOD static void setErrorHandler (OSCTXT* pCtxt, 
                                Asn1ErrorHandler* pHandler);

} ;
/**
 * @}Asn1NamedEventHandler
 */
#endif
