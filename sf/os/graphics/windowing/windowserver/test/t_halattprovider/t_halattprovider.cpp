// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// t_halattprovider.ldd Provides fake HAL attribute values on H4 platform for
// advanced pointer event testing.
// Fake values are listed in halFunction(TAny* aPtr, TInt aFunction, TAny* a1, TAny* a2).
// Only some values from the group EHalGroupDigitiser are changed.
// For not changed values from the group EHalGroupDigitiser the original HAL handler is called
// (the one loaded before this kernel extension).
// Original handler of EHalGroupDigitiser group must be registered in order
// to run this kernel extension.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <kernel/kernel.h>
#include <e32cmn.h>

static SHalEntry gOriginalHandler;
static TUint8 gNumberOfPointers;

void InitialiseDigitiserHalData()
	{
	gNumberOfPointers = 6;
	}

LOCAL_C TInt halFunction(TAny* aPtr, TInt aFunction, TAny* a1, TAny* a2)
	{
	TInt r=KErrNone;
	THalFunc function = gOriginalHandler.iFunction;
	switch(aFunction)
		{
		case EDigitiserHal3DInfo:
			{
			// execute the original handler
			r = (*function)(aPtr, EDigitiserHal3DInfo, a1, a2);
			TBool receivedFullData = (r == KErrNone);
			if (r != KErrNone)
				{
				// try to get at least TDigitiserInfoV01
				r = (*function)(aPtr, EDigitiserHalXYInfo, a1, a2);
				if (r != KErrNone)
					{
					Kern::Printf("t_halattprovider error: original handler of EDigitiserHalXYInfo returned %d", r);
					return r;
					}
				}
			
			// read data created by the original handler
			TPckgBuf<TDigitiserInfoV02> vPckg;
			TDigitiserInfoV02& data = vPckg();
			
			Kern::KUDesGet(vPckg, *(TDes8*)a1);
			
			// modify the data with the new values
			vPckg.SetLength(sizeof(TDigitiserInfoV02));
			data.iPressureSupported=1;
			data.iZRange=-100; // max proximity
			data.iProximityStep=5;
			data.iMaxPointers=8;
			data.iNumberOfPointers=gNumberOfPointers;
			data.iMaxPressure=5000;
			data.iPressureStep=500;
			
			if (!receivedFullData)
				{
				// original handler has provided only TDigitiserInfoV01, so we have to
				// fill all fields of TDigitiserInfoV02
				data.iThetaSupported=0;
				data.iPhiSupported=0;
				data.iAlphaSupported=0;
				}
			
			// return the data
			Kern::InfoCopy(*(TDes8*)a1, vPckg);
			
			break;
			}
		case EDigitiserHal3DPointer:
			{
			// Return value 1 for EPointer3D
			TBool ret = ETrue; 
			kumemput32(a1,&ret,sizeof(ret));
			break;
			}
		case EDigitiserHalSetNumberOfPointers:
			// Set request for EPointerNumberOfPointers
			
			// this line should be a correct implementation of this function but was not tested
			//kumemget32(&gNumberOfPointers,a1,sizeof(gNumberOfPointers));
			Kern::Printf("t_halattprovider error: attempt to use not implemented HAL function EDigitiserHalSetNumberOfPointers");
			r = KErrNotSupported;
			break;
		default:
			r = (*function)(aPtr, aFunction, a1, a2);
			break;
		}
	return r;
	}

DECLARE_STANDARD_EXTENSION()
	{
	TInt r = KErrNone;
	
	// Initialise Digitiser HAL Data first.
	InitialiseDigitiserHalData();
	
	NKern::ThreadEnterCS();
	
	// Find original handler whose behaviour will be overriden
	SHalEntry* originalHandlerPtr = Kern::FindHalEntry(EHalGroupDigitiser,0);
	if (originalHandlerPtr == NULL)
		{
		NKern::ThreadLeaveCS();
		Kern::Printf("t_halattprovider error: original handler of EHalGroupDigitiser group not found");
		return KErrNotFound;
		}
	gOriginalHandler = *originalHandlerPtr; // copy contents of the SHalEntry
	
	// Remove original handler
	r = Kern::RemoveHalEntry(EHalGroupDigitiser, 0);
	if (r != KErrNone)
		{
		NKern::ThreadLeaveCS();
		Kern::Printf("t_halattprovider error: unable to unregister original handler for EHalGroupDigitiser group, code:%d",r);
		return r;
		}
	
	// Add new handler
	r=Kern::AddHalEntry(EHalGroupDigitiser,halFunction,gOriginalHandler.iPtr,0);
	if (r != KErrNone)
		{
		NKern::ThreadLeaveCS();
		Kern::Printf("t_halattprovider error: unable register handler for EHalGroupDigitiser group, code:%d",r);
		return r;
		}

	NKern::ThreadLeaveCS();
	
	Kern::Printf("t_halattprovider: registered successfully");
	return r;
	}
