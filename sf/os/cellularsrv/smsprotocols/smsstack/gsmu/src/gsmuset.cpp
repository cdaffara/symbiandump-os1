// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsmusetTraces.h"
#endif

#include "gsmuset.h"
#include "smsstacklog.h"


/**
 *  Constructor - initialises iAlphabet and iFlags
 *  
 *  @capability None
 */
EXPORT_C TSmsUserDataSettings::TSmsUserDataSettings():
	iAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit),
	iFlags(0)
	{
	// NOP
	} // TSmsDataCodingScheme::ESmsAlphabet7Bit


/**
 *  Tests if User Data is concatenated using a 16 bit reference.
 *  
 *  @param aIs16Bit Set to true if concatenated using a 16 bit reference
 *  @return True if concatenated set as on
 *  @capability None
 */
EXPORT_C TBool TSmsUserDataSettings::TextConcatenated(TBool* aIs16Bit) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSMSUSERDATASETTINGS_TEXTCONCATENATED_1, "TSmsUserDataSettings::TextConcatenated()");

	if (aIs16Bit!=NULL)
		*aIs16Bit=iFlags&ESmsFlagConcatenatedUsing16BitReference;
	return iFlags&ESmsFlagConcatenated;
	} // TSmsUserDataSettings::TextConcatenated


/**
 *  Sets concatenation on/off.
 *  
 *  @param aConcatenated True to set concatenation on
 *  @param aIs16Bit Set to true to set 16 bit concatenation reference (default
 *  8 bit)
 *  @capability None
 */
EXPORT_C void TSmsUserDataSettings::SetTextConcatenated(TBool aConcatenated,TBool aIs16Bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSMSUSERDATASETTINGS_SETTEXTCONCATENATED_1, "TSmsUserDataSettings::SetTextConcatenated()");

	iFlags=aIs16Bit? iFlags|ESmsFlagConcatenatedUsing16BitReference: iFlags&(~ESmsFlagConcatenatedUsing16BitReference);
	iFlags=aConcatenated? iFlags|ESmsFlagConcatenated: iFlags&(~ESmsFlagConcatenated);
	} // TSmsUserDataSettings::SetTextConcatenated


/**
 *  Internalises the object.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void TSmsUserDataSettings::InternalizeL(RReadStream& aStream)
	{
	iAlphabet=(TSmsDataCodingScheme::TSmsAlphabet) aStream.ReadUint8L();
	iFlags=aStream.ReadUint32L();
	} // TSmsUserDataSettings::InternalizeL


/**
 *  Externalises the object.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void TSmsUserDataSettings::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L((TUint8) iAlphabet);
	aStream.WriteUint32L(iFlags);
	} // TSmsUserDataSettings::ExternalizeL
