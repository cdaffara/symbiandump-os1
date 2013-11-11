// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "W32STDGRAPHIC.H"

// TWsGraphicAnimation \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TWsGraphicAnimation::TWsGraphicAnimation():
	TWsGraphicMsgFixedBase(TUid::Uid(ETypeId),sizeof(*this)),
	iFlags(EStopped)
	{
	}
	
EXPORT_C TBool TWsGraphicAnimation::IsPlaying() const
	{
	return (State() == EPlaying);
	}

EXPORT_C TBool TWsGraphicAnimation::IsStopped() const
	{
	return (State() == EStopped);
	}

EXPORT_C TBool TWsGraphicAnimation::IsStopping() const
	{
	return (State() == EStopping);
	}

EXPORT_C TBool TWsGraphicAnimation::IsPaused() const
	{
	return (State() == EPaused);
	}
	
EXPORT_C TBool TWsGraphicAnimation::Loops() const
	{
	switch(State())
		{
		case EPlaying:
		case EPaused:
			return ((iFlags & ELoop) == ELoop);
		default:
			return EFalse;
		}
	}
	
TUint TWsGraphicAnimation::State() const
	{
	return (iFlags & EStateMask);
	}

void TWsGraphicAnimation::SetState(TUint aState)
	{
	iFlags &= ~EStateMask;
	iFlags |= (aState & EStateMask);
	}

   EXPORT_C void TWsGraphicAnimation::Play(TBool aLoop)
   	{
   	switch(State())
   		{
   		case EStopped:
   		case EStopping:
   			SetState(EPlaying);
			// fall through
		case EPlaying:
			iPlay.UniversalTime(); //Start over again
   			break;
   		case EPaused:
   			SetState(EPlaying);
			if(aLoop != Loops()) // can't resume a mismatch of looping attribute
				{
				iPlay.UniversalTime();
				}
			else // resume
				{
				TTimeIntervalMicroSeconds delta = iPauseOrStopping.MicroSecondsFrom(iPlay);
				iPlay.UniversalTime();
				iPlay = iPlay.Int64() - delta.Int64();
				break;
				}
		}
	if(aLoop)
		{
		iFlags |= ELoop;
		}
	else
		{
		iFlags &= ~ELoop;
		}
	}

EXPORT_C void TWsGraphicAnimation::Pause()
	{
	switch(State())
	{
	case EPlaying:
		case EStopping:
   			SetState(EPaused);
			iPauseOrStopping.UniversalTime();
   			break;
   		case EStopped:
   		case EPaused:
   			break;
   		}
	}

EXPORT_C void TWsGraphicAnimation::Stop(TBool aImmediately)
	{
	if(aImmediately)
		{
		SetState(EStopped);
		}
	else
		{
		switch(State())
			{
			case EPlaying:
				SetState(EStopping);
				iPauseOrStopping.UniversalTime();
				break;
			case EStopped:
				break;
			case EStopping:
				iPauseOrStopping.UniversalTime();
				break;
			case EPaused:
				SetState(EStopping);
				TTimeIntervalMicroSeconds delta = iPauseOrStopping.MicroSecondsFrom(iPlay);
				iPlay.UniversalTime();
				iPlay = iPlay.Int64() - delta.Int64();
				iPauseOrStopping.UniversalTime();
   				break;
   			}
   		}
   	}
