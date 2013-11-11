// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "Te_gsmu_emsSuiteStepBase.h"
#include "Te_gsmu_emsSuiteDefs.h"

// Device driver constants

TVerdict CTe_gsmu_emsSuiteStepBase::doTestStepPreambleL()
/**
 *  @return - TVerdict
 *  Implementation of CTestStep base class virtual
 *  It is used for doing all initialisation common to derived classes in here.
 *  Make it being able to leave if there are any errors here as there's no point in
 *  trying to run a test step if anything fails.
 *  The leave will be picked up by the framework.
 */
	{
	__UHEAP_MARK;

	return TestStepResult();
	}

TVerdict CTe_gsmu_emsSuiteStepBase::doTestStepPostambleL()
/**
 *  @return - TVerdict
 *  Implementation of CTestStep base class virtual
 *  It is used for doing all after test treatment common to derived classes in here.
 *  Make it being able to leave
 *  The leave will be picked up by the framework.
 */
	{
	__UHEAP_MARKEND;

	return TestStepResult();
	}


/* Missing function definitions copied from old test code */

CSmsMessage* CTe_gsmu_emsSuiteStepBase::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, aType, buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(aAlphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);

	TSmsServiceCenterAddress telephoneNumber=_L("+358503878682"); //Test SIM
	TSmsServiceCenterAddress serviceCenterNumber=_L("+358508771010"); //Radiolinja

	smsMessage->SetToFromAddressL(telephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(serviceCenterNumber);
	buffer->InsertL(0,aDes);
	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}


TInt CTe_gsmu_emsSuiteStepBase::LoadBitmap(CFbsBitmap& aBitmap,const TDesC& aName,TUint aId)
	{
		TParse    mbfn;
		mbfn.Set(aName,&KCDrive,NULL);
		TInt ret= aBitmap.Load(mbfn.FullName(),aId);
		if(ret != KErrNone)
		{
			mbfn.Set(aName,&KZDrive,NULL);
			ret= aBitmap.Load(mbfn.FullName(),aId);
		}
		return ret;
	}

CSmsMessage* CTe_gsmu_emsSuiteStepBase::CreateVariableLengthSmsMessageLC(TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
													  TInt aLength)
	{
/*
 * Creates and returns a SMS message with random user data
 * as the payload.
 */

	if (aLength > KMaxSmsBufferLength)
		User::Leave(KErrArgument);

	// Create a buffer and populate it with random text
	HBufC* text = HBufC::NewLC(aLength);

	TInt pos = 0;
	TUint val = 0;
	TInt64 seed =0;
	TTime time;
	time.UniversalTime();

	TPtr ptr(text->Des());

	while(pos<KMaxSmsBufferLength)
		{
		val = Math::Rand(seed);
		TText text = TText((val % 26) + L'A');
		ptr.Append(text);
		++pos;
		}

	INFO_PRINTF2(_L("Creating SMS Message of length %d"), aLength);

	CSmsMessage* smsMessage = CreateSmsMessageL(ptr, aAlphabet);

	CleanupStack::PopAndDestroy(text); // Done !
	CleanupStack::PushL(smsMessage);
	return smsMessage;
	}

void CTe_gsmu_emsSuiteStepBase::CompareEmsElementsL(CSmsMessage& aOriginalMessage,CSmsMessage& aOtherMessage)
/**
 *  Compares contents of EMS information array
 */
	{
	const RPointerArray<const CEmsInformationElement>&  emsOriginalElements= aOriginalMessage.GetEMSInformationElementsL();
	RPointerArray<const CEmsInformationElement> copyOfOriginalElements;
	CleanupClosePushL(copyOfOriginalElements);

	const RPointerArray<const CEmsInformationElement>&  emsOtherElements= aOtherMessage.GetEMSInformationElementsL();
	RPointerArray<const CEmsInformationElement> copyOfOtherElements;
	CleanupClosePushL(copyOfOtherElements);

	TInt originalCount=emsOriginalElements.Count();
	TInt i=0;
	const CEmsInformationElement* ie=NULL;

	for ( i=--originalCount; i >= 0 ; --i)
		{
		ie = emsOriginalElements[i];
		copyOfOriginalElements.Append(ie);
		}

	TInt otherCount=emsOtherElements.Count();

	for ( i=--otherCount; i >= 0 ; --i)
		{
		ie = emsOtherElements[i];
		copyOfOtherElements.Append(ie);
		}

	TBool ret=EFalse;
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());

	i=0;
	while(i < originalCount)
		{
			const CEmsInformationElement& ie=*copyOfOriginalElements[i];
			if(ie.Identifier()==CSmsInformationElement::ESmsEnhancedTextFormatting)
			{
				const CEmsFormatIE& formatIE=(CEmsFormatIE&)ie;
				if(aOriginalMessage.MessageLengthL() < (TInt)(formatIE.FormatLength() + formatIE.StartPosition()))
				{
					CEmsFormatIE* newIE=(CEmsFormatIE*)ie.DuplicateL();
					CleanupStack::PushL(newIE);
					newIE->SetFormatLength(aOriginalMessage.MessageLengthL()-formatIE.StartPosition());
					ret=CompareEmsElementL(*newIE,copyOfOtherElements);
					CleanupStack::PopAndDestroy(newIE);
				}
				else
					ret=CompareEmsElementL(ie,copyOfOtherElements);
			}
			else
				ret=CompareEmsElementL(ie,copyOfOtherElements);
			if(ret)
			{
			}
			else
			{
				switch (ie.Identifier())
				{
				case CSmsInformationElement::ESmsEnhancedTextFormatting:
					{
					CEmsFormatIE& formatIe = (CEmsFormatIE&) ie;
					INFO_PRINTF6(_L("Cannot find FormatIE : start position: %d format length: %d bold: %d italic: %d alignment: %d "), formatIe.StartPosition(),formatIe.FormatLength(),formatIe.Bold(),formatIe.Italic(),formatIe.Alignment());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
					{
					CEmsSoundIE& soundIe = (CEmsSoundIE&) ie;
					INFO_PRINTF3(_L("Cannot find UserDefinedSoundIE start position: %d size: %d"),soundIe.StartPosition(),soundIe.Melody()->Size());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedPredefinedSound:
					{
					CEmsPreDefSoundIE& soundIe = (CEmsPreDefSoundIE&) ie;
					INFO_PRINTF3(_L("Cannot find PrefinedSoundIE start position: %d type: %d "),soundIe.StartPosition(),soundIe.PredefinedSound());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedSmallPicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find SmallPictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);

					}
					break;
				case CSmsInformationElement::ESmsEnhancedLargePicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find LargePictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);

					}
					break;
				case CSmsInformationElement::ESmsEnhancedVariablePicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find VariablePictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);

					}
					break;
				case CSmsInformationElement::ESmsEnhancedSmallAnimation:
					{
					CEmsAnimationIE& picIe = (CEmsAnimationIE&) ie;
					INFO_PRINTF2(_L("Cannot find SmallAnimationIE start position: %d "),picIe.StartPosition());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedLargeAnimation:
					{
					CEmsAnimationIE& picIe = (CEmsAnimationIE&) ie;
					INFO_PRINTF2(_L("Cannot find LargeAnimationIE start position %d "),picIe.StartPosition());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
					{
					CEmsPreDefAnimationIE& animIe = (CEmsPreDefAnimationIE&) ie;
					INFO_PRINTF3(_L("Cannot find PreDefinedAnimationIE start position: %d type: %d "),animIe.StartPosition(),animIe.Animation());

					}
					break;
				case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
					{
					CEmsUserPrompt& upIe = (CEmsUserPrompt&) ie;
					INFO_PRINTF3(_L("Cannot find UserPromptIE start position:%d number of objects: %d"),upIe.StartPosition(),upIe.ObjectCount() );

					}
					break;
				case CSmsInformationElement::ESmsEnhancedODI:
					{
					CEmsObjectDistribution& odiIe = (CEmsObjectDistribution&) ie;
					INFO_PRINTF4(_L("Cannot find ObjectDistributionIE start position:%d number of objects: %d forwarding: %d"),odiIe.StartPosition(),odiIe.ObjectCount(), odiIe.Forwarding() );

					}
					break;
				default:
					break;
				}
			}
			i++;
		}
	RFbsSession::Disconnect();
	CleanupStack::PopAndDestroy(&copyOfOtherElements);
	CleanupStack::PopAndDestroy(&copyOfOriginalElements);
	}

TBool CTe_gsmu_emsSuiteStepBase::CompareEmsElementL(const CEmsInformationElement& aOriginalElement,RPointerArray<const CEmsInformationElement>& aOtherElementsArray)
/**
 *  Finds a EMS information element in the array and compares contents against found object
 */
	{
	TBool ret=EFalse;
	TUint startPosition=aOriginalElement.StartPosition();
	TSmsId id=aOriginalElement.Identifier();

	TInt count=aOtherElementsArray.Count();
	TInt i=0;
	const CEmsInformationElement* ie=NULL;

		if(id == CSmsInformationElement::ESmsEnhancedTextFormatting)
		{
			for ( i=--count; ret == EFalse && i >= 0 ; --i)
			{
				ie = aOtherElementsArray[i];
				if(ie->Identifier() == CSmsInformationElement::ESmsEnhancedTextFormatting)
				{
					CEmsFormatIE& formatIe = (CEmsFormatIE&) *ie;
					if(startPosition == formatIe.StartPosition()
						&& ((CEmsFormatIE&)aOriginalElement).Bold()==formatIe.Bold()
						&& ((CEmsFormatIE&)aOriginalElement).Italic()==formatIe.Italic()
						&& ((CEmsFormatIE&)aOriginalElement).Underline()==formatIe.Underline()
						&& ((CEmsFormatIE&)aOriginalElement).Strikethrough()==formatIe.Strikethrough()
						&& ((CEmsFormatIE&)aOriginalElement).Alignment()==formatIe.Alignment()
						&& ((CEmsFormatIE&)aOriginalElement).FontSize()==formatIe.FontSize())
						{
								ret= ETrue;
						}
					if(ret)
					{
						if(((CEmsFormatIE&)aOriginalElement).FormatLength()==formatIe.FormatLength())break;
						else if(((CEmsFormatIE&)aOriginalElement).FormatLength() > formatIe.FormatLength())
						{
							CEmsFormatIE* object2 = (CEmsFormatIE*)aOriginalElement.DuplicateL();
							CleanupStack::PushL(object2);
							aOtherElementsArray.Remove(i);
							object2->SetFormatLength(formatIe.FormatLength()-formatIe.FormatLength());
							object2->SetStartPosition(formatIe.StartPosition()+formatIe.FormatLength());
							ret=CompareEmsElementL(*object2,aOtherElementsArray);
							CleanupStack::PopAndDestroy(object2);
						}
					}
				}

			}
		}
		else
		{
			for ( i=--count; ret == EFalse && i >= 0 ; --i)
			{
				ie = aOtherElementsArray[i];
				if(startPosition == ie->StartPosition() && id == ie->Identifier())
				{
				// compare contents
					switch (id)
					{
					case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
						{
						CEmsSoundIE& soundIe = (CEmsSoundIE&)*ie;
						const HBufC8* melody=soundIe.Melody();

						const HBufC8* originalMelody=((CEmsSoundIE&)aOriginalElement).Melody();

						if(Mem::Compare(melody->Ptr(),melody->Size(),
						originalMelody->Ptr(),originalMelody->Size())==0)
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedPredefinedSound:
						{
						CEmsPreDefSoundIE& soundIe = (CEmsPreDefSoundIE&) *ie;
						if(((CEmsPreDefSoundIE&)aOriginalElement).PredefinedSound()==soundIe.PredefinedSound())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedSmallPicture:
					case CSmsInformationElement::ESmsEnhancedLargePicture:
					case CSmsInformationElement::ESmsEnhancedVariablePicture:
						{
						CEmsPictureIE& picIe = (CEmsPictureIE&) *ie;
						CFbsBitmap* pic = picIe.GetBitmapL();
						CleanupStack::PushL(pic);

						CFbsBitmap* originalPic = ((CEmsPictureIE&)aOriginalElement).GetBitmapL();
						CleanupStack::PushL(originalPic);

						if(pic->SizeInPixels()==originalPic->SizeInPixels())
							{
							const HBufC8* picBuf=CreatePictureBufferL(*pic);
							const HBufC8* originalPicBuf=CreatePictureBufferL(*originalPic);

							if(Mem::Compare(picBuf->Ptr(),picBuf->Size(),
								originalPicBuf->Ptr(),originalPicBuf->Size())==0)
								{
								aOtherElementsArray.Remove(i);
								ret=ETrue;
								}
							CleanupStack::PopAndDestroy(2);
							}

						CleanupStack::PopAndDestroy(originalPic);
						CleanupStack::PopAndDestroy(pic);
						}
						break;
					case CSmsInformationElement::ESmsEnhancedSmallAnimation:
					case CSmsInformationElement::ESmsEnhancedLargeAnimation:
						{
						CEmsAnimationIE& picIe = (CEmsAnimationIE&) *ie;
						CFbsBitmap* pic = picIe.GetBitmapL();
						CleanupStack::PushL(pic);

						CFbsBitmap* originalPic = ((CEmsAnimationIE&)aOriginalElement).GetBitmapL();
						CleanupStack::PushL(originalPic);

						if(pic->SizeInPixels()==originalPic->SizeInPixels())
							{
							const HBufC8* picBuf=CreatePictureBufferL(*pic);
							const HBufC8* originalPicBuf=CreatePictureBufferL(*originalPic);

							if(Mem::Compare(picBuf->Ptr(),picBuf->Size(),
								originalPicBuf->Ptr(),originalPicBuf->Size())==0)
								{
								aOtherElementsArray.Remove(i);
								ret=ETrue;
								}
							CleanupStack::PopAndDestroy(2);
							}

						CleanupStack::PopAndDestroy(originalPic);
						CleanupStack::PopAndDestroy(pic);
						}
						break;
					case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
						{
						CEmsPreDefAnimationIE& animIe = (CEmsPreDefAnimationIE&) *ie;
						if(((CEmsPreDefAnimationIE&)aOriginalElement).Animation()==animIe.Animation())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
						{
						CEmsUserPrompt& upIe = (CEmsUserPrompt&) *ie;
						if(((CEmsUserPrompt&)aOriginalElement).ObjectCount()==upIe.ObjectCount())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedODI:
						{
						CEmsObjectDistribution& odiIe = (CEmsObjectDistribution&) *ie;
						if(((CEmsObjectDistribution&)aOriginalElement).ObjectCount()==odiIe.ObjectCount() && ((CEmsObjectDistribution&)aOriginalElement).Forwarding()==odiIe.Forwarding())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
						default:
						break;
					} //end switch id
				} // end if start position and identifier
			} //end for
		} //end else
	return ret;
	}

HBufC8* CTe_gsmu_emsSuiteStepBase::CreatePictureBufferL(const CFbsBitmap& aBitmap)
/**
 *  Creates buffer and fullfills it bny contents of the bitmap
 */
	{
	const TSize size = aBitmap.SizeInPixels();
	const TInt numLineBytes = size.iWidth/8;
	HBufC8* pictureBuf = HBufC8::NewLC(numLineBytes * size.iHeight);

	HBufC8* scanLine = HBufC8::NewLC(numLineBytes);
	TPtr8 line(scanLine->Des());

	HBufC8* working = HBufC8::NewLC(numLineBytes);
	TPtr8 convertedScanLine(working->Des());
	convertedScanLine.SetLength(numLineBytes);

	for (TInt a=0; a<size.iHeight; a++)
		{
		aBitmap.GetScanLine(line, TPoint(0,a), size.iWidth, EGray2);

		for (TInt word=0; word<numLineBytes; word++)
			{
			convertedScanLine[word] = line[word];
			}

		pictureBuf->Des().Append(convertedScanLine);
		}

	CleanupStack::PopAndDestroy(2, scanLine);
	return pictureBuf;
	}
