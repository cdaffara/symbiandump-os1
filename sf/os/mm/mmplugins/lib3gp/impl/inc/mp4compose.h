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

#ifndef _MP4COMPOSE_H
#define _MP4COMPOSE_H


#include <3gplibrary/mp4config.h>
#include "mp4atom.h"


#define MVHD_TIMESCALE  10000

#define METADATA_COPY_BUFFERSIZE  8192   /* Must be divisible by 4 */


typedef struct trakSize
{
  mp4_u32    trak;
  mp4_u32    tkhd;
  mp4_u32    mdia;
  mp4_u32    mdhd;
  mp4_u32    hdlr;
  mp4_u32    minf;
  mp4_u32    vmhd;
  mp4_u32    smhd;
  mp4_u32    dinf;
  mp4_u32    dref;
  mp4_u32    stbl;
  mp4_u32    stsd;
  mp4_u32    stts;
  mp4_u32    stsc;
  mp4_u32    stsz;
  mp4_u32    stco;
  mp4_u32    stss;
  mp4_u32    mp4v;
  mp4_u32    mp4a;
  mp4_u32    esds;
  mp4_u32    s263;
  mp4_u32    samr;
  mp4_u32    sawb;
  mp4_u32    d263;
  mp4_u32    damr;
  mp4_u32	 avc1;
  mp4_u32	 avcc;
  mp4_u32	 btrt;
  mp4_u32	 m4ds;
  mp4_u32    sqcp;
  mp4_u32    dqcp;
  mp4_u32    udta;
  mp4_u32    sdtp;
  mp4_u32    avcp;
} trakSize;



mp4_i32 updateVideoMetaData(MP4HandleImp handle, mp4_u32 size, mp4_u32 duration, mp4_bool keyframe);
mp4_i32 updateAudioMetaData(MP4HandleImp handle, mp4_u32 size, mp4_u32 duration);
mp4_i32 writeFTYPAndMDATToFile(MP4HandleImp handle);
mp4_i32 writeDataToFile(MP4HandleImp handle);
mp4_i32 updateDecodingTimeToSample(MP4HandleImp handle, sampleTable *st, mp4_u32 duration);
mp4_i32 updateSampleSize(MP4HandleImp handle, sampleTable *st, mp4_u32 size);
mp4_i32 updateSampleToChunk(sampleTable *st);
mp4_i32 updateChunkOffset(MP4HandleImp handle, sampleTable *st);
mp4_i32 updateSyncSample(MP4HandleImp handle, sampleTable *st);
mp4_i32 determineAudioTrakMetaDataSize(MP4HandleImp handle, sampleTable *st, trakSize *ts);
mp4_i32 determineVideoTrakMetaDataSize(MP4HandleImp handle, sampleTable *st, trakSize *ts);
mp4_i32 reUpdateAudioMetaData(sampleTable *st, mp4_u32 metaDataSize);
mp4_i32 reUpdateVideoMetaData(sampleTable *st, mp4_u32 metaDataSize);
mp4_i32 writeFTYP(MP4HandleImp handle);
mp4_i32 writeMOOV(MP4HandleImp handle, mp4_u32 moovSize, mp4_bool haveAudio, mp4_bool haveVideo, trakSize *audioTrackSize, trakSize *videoTrakSize);
mp4_i32 writeMVHD(MP4HandleImp handle);
mp4_i32 writeVideoTrak(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoTKHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoMDIA(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoMDHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoHDLR(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoMINF(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVMHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeDINF(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeDREF(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTBL(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeMP4V(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoESD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeS263(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeD263(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTTS(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTTSLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSC(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSZ(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSZLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTCO(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoCO64(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTCOLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoCO64LongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSS(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSTSSLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAVC1(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAVCC(MP4HandleImp handle, trakSize *ts);

mp4_i32 writeAudioTrak(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioTKHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioMDIA(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioMDHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioHDLR(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioMINF(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeSMHD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTBL(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTSD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeMP4A(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioESD(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeSAMR(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeSAWB(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeDAMR(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTTS(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTTSLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTSC(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTSZ(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTSZLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTCO(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioCO64(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioSTCOLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAudioCO64LongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeSQCP(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeDQCP(MP4HandleImp handle, trakSize *ts);

mp4_i32 writeMediaData(MP4HandleImp handle);

mp4_i32 insertu64(mp4_u8 *buf, mp4_u64 value);
mp4_i32 insertu32(mp4_u8 *buf, mp4_u32 value);
mp4_i32 insertu16(mp4_u8 *buf, mp4_u16 value);

mp4_i32 writeMetaDataTmp(MP4HandleImp handle);
mp4_i32 calculateES_DescriptorSize(MP4HandleImp handle, mp4_u32 type);
mp4_i32 writeUDTA(MP4HandleImp handle, userDataAtom *udta);
mp4_i32 updateVideoDependencyMetaData(MP4HandleImp handle, mp4_u8 aDependsOn, mp4_u8 aIsDependentOn, mp4_u8 aHasRedundancy);
mp4_i32 writeVideoSDTP(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeVideoSDTPLongClip(MP4HandleImp handle, trakSize *ts);
mp4_i32 writeAVCP(MP4HandleImp handle, trakSize *ts);
#endif
// End of File
