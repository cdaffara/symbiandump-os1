// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "LbsLasNmeaSource.h"
#include "LbsLasDataSource.h"
#include "LbsLasCommon.h"

//************************************************************************************************************
//   CLasNmeaSource
//************************************************************************************************************
CLasNmeaSource* CLasNmeaSource::NewL(MLasNmeaSourceObserver* aObserver)
	{
	CLasNmeaSource* self = new(ELeave) CLasNmeaSource(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CLasNmeaSource::CLasNmeaSource(MLasNmeaSourceObserver* aObserver)
	:
	iObserver(aObserver),
	iNmeaReadState(ENmeaWantStart)
	{
	}

void CLasNmeaSource::ConstructL()
	{
	iDataSource = CLasDataSource::NewL(this);
	}

CLasNmeaSource::~CLasNmeaSource()
	{
	delete iDataSource;
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasNmeaSource::OpenNmeaSource()
	{
	LASLOG("CLasNmeaSource::OpenNmeaSourceL - Enter");

	iDataSource->OpenDataSource();
	
	LASLOG("CLasNmeaSource::OpenNmeaSourceL - Exit");
	}


void CLasNmeaSource::CloseNmeaSource()
	{
	LASLOG("CLasNmeaSource::CloseNmeaSource - Enter");
	
	iDataSource->CloseDataSource();
	
	LASLOG("CLasNmeaSource::CloseNmeaSource - Exit");
	}

void CLasNmeaSource::MonitorNmeaSentence()
	{
	LASLOG("CLasNmeaSource::ReadNmeaSentence - Enter");

	iMonitoring = ETrue;

	iDataSource->MonitorDataSource();
	
	LASLOG("CLasNmeaSource::ReadNmeaSentence - Exit");
	}


void CLasNmeaSource::CancelMonitor()
	{
	LASLOG("CLasNmeaSource::CancelRead - Enter");

	iMonitoring    = EFalse;
	iNmeaReadState = ENmeaWantStart;
	
	iDataSource->CancelMonitor();

	LASLOG("CLasNmeaSource::CancelRead - Exit");
	}	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasNmeaSource::AppendNmeaFieldChar(TUint aChar)
	{
	if (iNmeaSentence.Length() >= iNmeaSentence.MaxLength())
		{
		return;
		}
	
	iNmeaSentence.Append(aChar);

	// If aChar is not a field delimiter then just add it to buffer and return
	if (aChar != ',' && aChar != '*')
		{
		iNmeaFieldLength++;
		return;
		}

	// Got a field delimiter increase the number of fields
	// aChar == ',' || aChar == '*'

	TPtrC8 thisField = iNmeaSentence.Mid(iNmeaFieldStart, iNmeaFieldLength);
	iNmeaFields[iNmeaNumFields].Set(thisField);
	iNmeaNumFields++;

	//TInt stop = iNmeaFieldStart + iNmeaFieldLength - 1;
	//LASLOG4("Field Start: %d, Stop: %d, Length: %d", iNmeaFieldStart, stop, iNmeaFieldLength);
	//LASLOGDES(iNmeaSentence);
	//LASLOG("0123456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H");

	// Next field starts here
	iNmeaFieldStart  = iNmeaSentence.Length();
	iNmeaFieldLength = 0;
	}

#define HEXVAL(c) ((c >= '0' && c <= '9') ? (c - '0') : (c - 'A' + 10))

void CLasNmeaSource::StoreNmeaString(const TDesC8& aData)
	{
	TInt length = aData.Length();
	
	for (TInt index = 0; index < length; index++)
		{
		TUint ch = aData[index];
	
		//<<TODO>> Still need to handle the escape char "^"
			
		switch(iNmeaReadState)
			{
			case ENmeaWantStart:
				{
				if (ch == '$' || ch == '!')
					{
					iNmeaReadState   = ENmeaWantTerminator;
					iNmeaSentence.SetLength(0);
					iNmeaCheckSum    = 0;
					iNmeaNumFields   = 0;
					iNmeaFieldStart  = 0;
					iNmeaFieldLength = 0;
					}
				break;
				}
		
			case ENmeaWantTerminator:
				{
				AppendNmeaFieldChar(ch);

				if (ch == '*') // Got the sentence terminator
					{
					iNmeaReadState = ENmeaWantCheck1;
					}
				else 
					{
					iNmeaCheckSum ^= ch;
					}
				break;
				}

			case ENmeaWantCheck1:
				{
				iNmeaCheckValue = HEXVAL(ch) << 4;	
				iNmeaReadState = ENmeaWantCheck2;
				break;
				}

			case ENmeaWantCheck2:
				{
				iNmeaCheckValue += HEXVAL(ch);	
				iNmeaReadState   = ENmeaWantStart;
				LASLOG2("CLasNmeaSource::StoreNmeaString - CheckSum: %d", iNmeaCheckSum);
				LASLOG2("CLasNmeaSource::StoreNmeaString - CheckVal: %d", iNmeaCheckValue);		
				LASLOGDES(iNmeaSentence);
				
				if (iNmeaCheckSum == iNmeaCheckValue)
					{
					if (iMonitoring)
						{
						iObserver->HandleNmeaSentence(iNmeaFields, iNmeaNumFields);					
						}
					}
					else
						{
						LASLOG("***WARNING - BAD CHECKSUM***");
						}
				break;
				}
			}
		}
	}


void CLasNmeaSource::HandleData(const TDesC8& aData, TInt aErr)
	{
	if (! aErr)
		{
		StoreNmeaString(aData);
		}
	}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
