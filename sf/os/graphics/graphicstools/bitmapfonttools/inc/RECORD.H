/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header RECORD.H
*
*/


#ifndef __RECORD_H__
#define __RECORD_H__

#include "LEXICAL.H"
#include "LST.H"
#include "STRNG.H"

/**
@file
@publishedAll
*/

const boolean Proportional = 1; /**< WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const boolean Serif = 2; /**< WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases. */
const boolean Symbol = 4; /**< WARNING: Constant for internal use ONLY.  Compatibility is not guaranteed in future releases. */

/**
@publishedAll
WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
inline void ExternalizeStreamOff(ostream& out, streamoff aOffset)
	{
	// This will limit the file to 4gig.
	// Need to change this if all compilers support file size greater than 4gig.
	uint32 offset = static_cast<uint32>(aOffset);
	out.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}

class Record
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	IMPORT_C Record();
	virtual void Externalize(ostream& out) = 0;
	virtual void ExternalizeComponents(ostream&){};
public:
	String iLabel;
	streampos iStreamId;
	};

class RecordList : public ObjectList<Record*>
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	void ExternalizeIds(ostream& out);
	void Externalize(ostream& out);
	void ExternalizeComponents(ostream& out);
	IMPORT_C void Add(Record* aRecord);
	IMPORT_C Record *LabelToRecord(const String& aLabel);
	IMPORT_C void Destroy();
	IMPORT_C ~RecordList();
	};

class Typeface
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	IMPORT_C Typeface();
	void Externalize(ostream& out);
public:
	String iName;
	boolean iFlags;
	};

class Point
/**
@publishedAll
WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
*/
	{
public:
	void Externalize(ostream& out);
public:
	int32 iX;
	int32 iY;
	};

#endif

