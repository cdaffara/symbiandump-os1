// btrace_generic.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "btrace_parser.h"

EXPORT_C CBtraceGeneric* CBtraceGeneric::NewL(CBtraceReader& aReader)
	{
	CBtraceGeneric* self = new (ELeave) CBtraceGeneric(aReader);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CBtraceGeneric::CBtraceGeneric(CBtraceReader& aReader)
:	iReader(aReader)
	{
	
	}


EXPORT_C CBtraceGeneric::~CBtraceGeneric()
	{
	iReader.RemoveObserver(KAmTraceCategory, *this);
	iObservers.Close();
	}


void CBtraceGeneric::ConstructL()
	{
	iReader.AddObserverL(KAmTraceCategory, *this);
	}


EXPORT_C void CBtraceGeneric::NotifyGenericEventL
	(MBtraceGenericObserver& aObserver)
	{
	iObservers.AppendL(&aObserver);
	}


void CBtraceGeneric::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == KAmTraceCategory && aFrame.iSubCategory > EAmTraceSubCategoryDomainEventBase)
		{
		const TInt length = aFrame.iData.Length();
		const TUint32* data = reinterpret_cast<const TUint32*>(aFrame.iData.Ptr());
		
		switch(length)
			{
			case 0:
				SeenGeneric0L(aFrame.iTickCount, aFrame.iSubCategory);
				break;
			
			case 4:
				SeenGeneric1L(aFrame.iTickCount, aFrame.iSubCategory, *data);
				break;
				
			case 8:
				SeenGeneric2L(aFrame.iTickCount, aFrame.iSubCategory, *data, *(data+1));
				break;
				
			case 12:
				SeenGeneric3L(aFrame.iTickCount, aFrame.iSubCategory, *data, *(data+1), *(data+2));
				break;
			}
		}
	}


void CBtraceGeneric::SeenGeneric0L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory)
	{
	TInt ii = iObservers.Count();
	while (--ii >= 0)
		{
		iObservers[ii]->HandleGenericEvent0L(aTickCount, aSubCategory);
		iObservers.Remove(ii);
		}	
	}


void CBtraceGeneric::SeenGeneric1L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1)
	{
	TInt ii = iObservers.Count();
	while (--ii >= 0)
		{
		iObservers[ii]->HandleGenericEvent1L(aTickCount, aSubCategory, aData1);
		iObservers.Remove(ii);
		}	
	}


void CBtraceGeneric::SeenGeneric2L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1, TUint32 aData2)
	{
	TInt ii = iObservers.Count();
	while (--ii >= 0)
		{
		iObservers[ii]->HandleGenericEvent2L
			(aTickCount, aSubCategory, aData1, aData2);
		iObservers.Remove(ii);
		}	
	}


void CBtraceGeneric::SeenGeneric3L(const TBtraceTickCount& aTickCount, TUint8 aSubCategory, TUint32 aData1, TUint32 aData2, TUint32 aData3)
	{
	TInt ii = iObservers.Count();
	while (--ii >= 0)
		{
		iObservers[ii]->HandleGenericEvent3L
			(aTickCount, aSubCategory, aData1, aData2, aData3);
		iObservers.Remove(ii);
		}	
	}


