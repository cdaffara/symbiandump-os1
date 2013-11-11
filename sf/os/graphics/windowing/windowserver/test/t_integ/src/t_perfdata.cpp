// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include <iniparser.h>
#include <s32file.h>
#include <hal.h>
#include <e32math.h>

#include "t_perfdata.h"
#include "t_wservconsts.h"

// Strings used when writing results to file to
// describe the values accordingly.
// e.g. KPerfDataFrameRate = 24
// is the calculated frame rate for the measurements taken.
_LIT(KPerfDataTrimedMean, "KPerfDataTrimedMean");
_LIT(KPerfDataMaxTime,    "KPerfDataMaxTime");
_LIT(KPerfDataMinTime,    "KPerfDataMinTime");
_LIT(KPerfDataFrameRate,  "KPerfDataFrameRate");

EXPORT_C CTPerfData*  CTPerfData::NewL()
	{
	CTPerfData* self = new(ELeave) CTPerfData();
	CleanupStack::PushL(self);
	self->Construct();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CTPerfData::StartCounter()
	{
	iCounter = User::FastCounter();
	}

EXPORT_C void CTPerfData::StopCounterL()
	{
	TUint32 counter = User::FastCounter();
	iResults.AppendL(counter-iCounter);
	iCounter=counter;
	}

EXPORT_C void CTPerfData::WriteResultsL(const TDesC& aFileName)
	{
	AnalyseResultsL();

	RFs myFs;
	User::LeaveIfError(myFs.Connect());

	RFileWriteStream writer;
	writer.PushL(); // writer on cleanup stack
	TInt err = myFs.MkDirAll(aFileName);

	if (err==KErrNone || err==KErrAlreadyExists)
		{
		User::LeaveIfError(writer.Replace(myFs, aFileName, EFileStreamText|EFileWrite));
		writer.CommitL();
		CleanupStack::PopAndDestroy(&writer); // writer

		CIniData* myData=CIniData::NewL(aFileName);
		CleanupStack::PushL(myData);

		TBuf<255> tempStore;
		_LIT(KIntData, "%d");
		tempStore.Format(KIntData,iTrimedMean);
		User::LeaveIfError(myData->AddValue(KDefaultSectionName, KPerfDataTrimedMean, tempStore));
		tempStore.Format(KIntData,iMaxTime);
		User::LeaveIfError(myData->AddValue(KDefaultSectionName, KPerfDataMaxTime, tempStore));
		tempStore.Format(KIntData,iMinTime);
		User::LeaveIfError(myData->AddValue(KDefaultSectionName, KPerfDataMinTime, tempStore));
		_LIT(KRealData, "%4.0f");
		tempStore.Format(KRealData,iFrameRate);
		User::LeaveIfError(myData->AddValue(KDefaultSectionName, KPerfDataFrameRate, tempStore));
		myData->WriteToFileL();

		CleanupStack::PopAndDestroy(myData);
		}
	else
		{
		CleanupStack::PopAndDestroy(&writer); // writer
		}

	myFs.Close();
	}

void CTPerfData::Construct()
	{
	iResults.Reset();
	}

CTPerfData::~CTPerfData()
	{
	iResults.Reset();
	}

void CTPerfData::AnalyseResultsL()
	{
	TrimedMeanL();
	MaxTime();
	MinTime();
	FrameRateL();
	}

void CTPerfData::TrimedMeanL()
	{
	TInt64 total = 0;
	if (iResults.Count() <= 50)
		{
		RDebug::Print(_L("Not enough results for trimming - need more than 50, but got %d"), iResults.Count());
		iTrimedMean = 0;
		}
	else
		{
		TReal tempVar = iResults.Count() * 0.20;
		TInt32 twentyPercentCount = 0;
		User::LeaveIfError(Math::Int(twentyPercentCount, tempVar));



		for (TInt count = twentyPercentCount; count < iResults.Count()-twentyPercentCount; count++)
			{
			total += iResults[count];
			}

		iTrimedMean = (static_cast<TUint32>(total/(iResults.Count()-(static_cast<TInt64>(twentyPercentCount)*2))));
		}

	RDebug::Print(_L("CTPerfData::TrimedMeanL - %d"), iTrimedMean);
	}

void CTPerfData::MaxTime()
	{
	TUint32 result = iResults[0];
	for(TInt i = 0; i < iResults.Count(); i++)
		{
		if(iResults[i] > result)
			{
			result = iResults[i];
			}
		}
	iMaxTime = result;

	RDebug::Print(_L("CTPerfData::MaxTime - %d"), iMaxTime);
	}

void CTPerfData::MinTime()
	{
	TUint32 result = iResults[0];
	for(TInt i = 0; i < iResults.Count(); i++)
		{
		if(iResults[i] < result)
			{
			result = iResults[i];
			}
		}
	iMinTime = result;

	RDebug::Print(_L("CTPerfData::MinTime - %d"), iMinTime);
	}

void CTPerfData::FrameRateL()
	{
	iFrameRate = 0;
	if (iTrimedMean==0)
		{
		TrimedMeanL();
		}
	TInt counterFreq = 1;
	User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, counterFreq));

	if (iTrimedMean)
		{
		iFrameRate = static_cast<TReal>(counterFreq)/iTrimedMean;
		}
	else
		{
		User::Leave(KErrAbort);
		}

	RDebug::Print(_L("CTPerfData::FrameRate - %4.0f"), iFrameRate);
	}
