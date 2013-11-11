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
* Header RECORD.CPP
*
*/


#include "RECORD.H"

EXPORT_C Record::Record()
 :	iLabel(), iStreamId(0)
	{
	}

void RecordList::ExternalizeIds(ostream& out)
	{
	int32 i;
	int32 size = Size();
	out.write((char*) &size, sizeof(size));
	for (i = 0; i < size; i++)
		{
		::ExternalizeStreamOff(out, (*this)[i]->iStreamId);
		}
	}

void RecordList::Externalize(ostream& out)
	{
	int32 size = Size();
	out.write ((char*) &size, sizeof(size));
	for (int i = 0; i < size; i++)
		(*this)[i]->Externalize(out);
	}

void RecordList::ExternalizeComponents(ostream& out)
	{
	int32 size = Size();
	for (int i = 0; i < size; i++)
		(*this)[i]->ExternalizeComponents(out);
	}

EXPORT_C void RecordList::Add(Record* aRecord)
	{
	if (!LabelToRecord(aRecord->iLabel))
		List<Record*>::Add(aRecord);
	}

EXPORT_C Record *RecordList::LabelToRecord(const String& aLabel)
	{
	int32 size = Size();
	Record *record=NULL;
	if (aLabel.Length())
		for (int i = 0; i < size; i++)
			if ((*this)[i]->iLabel == aLabel)
				record = (*this)[i];
	return record;
	}

EXPORT_C void RecordList::Destroy()
	{
	List<Record*>::Destroy();
	}

EXPORT_C RecordList::~RecordList()
	{
	}

EXPORT_C Typeface::Typeface()
 :	iName(), iFlags(0)
	{
	}

void Typeface::Externalize(ostream& out)
	{
	iName.Externalize(out);
	out.put((char) iFlags);
	}

void Point::Externalize(ostream& out)
	{
	out.write((char*) &iX, sizeof(iX));
	out.write((char*) &iY, sizeof(iY));
	}
