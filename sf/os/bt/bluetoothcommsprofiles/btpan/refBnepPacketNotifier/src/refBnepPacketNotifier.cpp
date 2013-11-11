// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "refBnepPacketNotifier.h"

EXPORT_C MBnepPacketNotifierBase* CRefBnepPacketNotifier::NewL()
	{
	MBnepPacketNotifierBase* self = new(ELeave) CRefBnepPacketNotifier;
	return self;
	}



CRefBnepPacketNotifier::CRefBnepPacketNotifier():iCounter(8) // 8 is the current maximum number of PAN connections, so is a good number to set the granularity to.
	{
	}

CRefBnepPacketNotifier::~CRefBnepPacketNotifier() 
	{
	iCounter.Close();
	}

void CRefBnepPacketNotifier::MbpnSetLinkControl(MPanLinkControlBase& aPanLinkControl) 
	{
	iPanLinkControl=(MPanLinkControl*) aPanLinkControl.MplcbGetInterface(KUidPanLinkControlV1);
	}

void CRefBnepPacketNotifier::MbpnBnepDataTransferred(const TArray<TBnepBytesTransferred> & aBnepBytesTransferred)
	{
	for (TInt dev=0;dev<aBnepBytesTransferred.Count();dev++)  // Cycle through array passed to function
		{
		for (TInt index=0;index<iCounter.Count();index++) // Cycle through our array
			{
			TBTDevAddr & curAddr = iCounter[index].iAddr;
			if (curAddr == aBnepBytesTransferred[dev].iBTDevAddr) // If the device addresses are the same
				{
				iCounter[index].iByteCount+=aBnepBytesTransferred[dev].iBytesSent + aBnepBytesTransferred[dev].iBytesReceived; // Increment byte counter
				iCounter[index].iTickCount++; // Increment counter 
				if (iCounter[index].iTickCount == KTicksToSniff) // If it's time to decide whether to sniff/active
					{
					iCounter[index].iTickCount=0; // Reset counter
					if (iPanLinkControl) 
						{
						
						if (iCounter[index].iByteCount < KMaxBytesToSniff )  
							{
							
							iPanLinkControl->MplcRequestSniff(curAddr); // Sniff if under KMaxBytesToSniff
							
							}
						else 
							{
							iPanLinkControl->MplcRequestActive(curAddr); // Otherwise active
							}
						iCounter[index].iByteCount=0; // Reset byte count
						}
					}
				if (aBnepBytesTransferred[dev].iBytesSent + aBnepBytesTransferred[dev].iBytesReceived > KMinBytesToUnSniff && iPanLinkControl)
					{
					iPanLinkControl->MplcRequestActive(curAddr); // If data in one tick is enough, bring out of sniff
					}	
				}
			}
		}
	}

void CRefBnepPacketNotifier::MbpnDeviceAdded(const TBTDevAddr& aAddr)
	{
	TByteCount devByteCount;
	devByteCount.iByteCount=0;
	devByteCount.iTickCount=0;
	devByteCount.iAddr=aAddr;
	(void)iCounter.Append(devByteCount);
	}

void CRefBnepPacketNotifier::MbpnDeviceRemoved(const TBTDevAddr& aAddr)
	{
	for (TUint dataIndex=0; dataIndex < iCounter.Count(); ++dataIndex)
		{
		if (iCounter[dataIndex].iAddr==aAddr) 
        	{
        	iCounter.Remove(dataIndex);
        	}
        }

	}

MBnepPacketNotifier* CRefBnepPacketNotifier::Interface()
	{
	return this;
	}

TAny* CRefBnepPacketNotifier::MbpnbGetInterface(const TUid & aInterface) 
	{
	if (aInterface==KUidBnepPacketNotifierV1)
		{
		return Interface();
		}
	return NULL;
	}
	
void CRefBnepPacketNotifier::MbpnbRelease()
	{
	delete this;
	}
