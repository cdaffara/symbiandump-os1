#!/usr/local/bin/perl
#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#  This script parses trace data produced by OST from FBS, using the the FBSCLI, 
#  FBSERV and Symbian BTrace Hooks OST dictionaries, to produce a CSV output of
#  the amount of bitmap memory used per-thread over a user-definable time
#  granularity, since the start of the trace.
# 
#  To use, enable SYMBIAN_KERNEL_THREAD_IDENTIFICATION trace group in Symbian
#  BTrace Hooks OST dictionary, GRAPHICS_RESOURCE_MANAGEMENT_SEMANTICS in FBSERV
#  OST dictionary, and GRAPHICS_RESOURCE_MANAGEMENT_SEMANTICS, 
#  GRAPHICS_RESOURCE_MANAGEMENT_FUNCTIONS and GRAPHICS_CONTROL_FUNCTIONS in
#  FBSCLI OST dictionary. Once tracing is gathered, save trace output as ascii 
#  and run this script against it. The resulting file can then be imported into
#  a spreadsheet application to be visually processed.
#  
#  KNOWN DEFECTS:
#  Once the log time goes beyond midnight, snapshots will stop being taken.
#

use strict;

# Sanity checking of the command line parameters...
if ($#ARGV == -1 || $ARGV[0] eq "help" || $ARGV[0] eq "/?")
{
   print "\nusage: $0 filename [-h]\n";
   print "where\n";
   print " -h : Specifies the heartbeat in millisecs (default=10000)\n";
   exit;
}


## Modifiable constants...
my $CSV_DELIMITER = ',';

# Time after start to take first snapshot, in millisecs
my $firstHeartBeatTimeMS = 1000;

# Default heartbeat in millisecs if none specified.
my $heartBeatMS = 10000;


##
## Internal structures...
##
my $heartBeatCount = 0;
my $nextHeartBeatMS = -1;

# A hash of thread names to the amount of bitmap memory they 
# have used since the start of the trace.
my %bmpMemoryPerThread = ();

# A hash of bitmaps fully qualified by the session they belong to,
# and their local handle (because bitmaps can have the same local
# handle in different threads), mapped to their size in bytes.
my %bmpMemoryByServerHandle = ();

# Hash of FbsSessions to thread IDs.
my %SessionThreadMap = ();

# Array of the above hashes, one hash per heartbeat.
my @arrayOfSnapshots;

# Hashes of thread and process names to IDs.
my %ThreadNames;
my %ProcessNames;


##
## Command line options parsing...
## First arg is assumed to be the filename.
##
for my $i (1..$#ARGV)
{
   my $cma = $ARGV[$i];
   if ($cma =~ m/-h(\d*)/)
   {
      $heartBeatMS = $1;
   }
   else
   {
      print "Unrecognised parameter: $cma , ignoring...\n";
   }
}

## Read from the file.
## Read the log into an array line by line.
my $TRACE_FILENAME = $ARGV[0];
open(INPUT_FILE, $TRACE_FILENAME) or die $!;
my @traceLines = <INPUT_FILE>;


##
## Parse each line sequentially...
##
foreach my $line (@traceLines)
{
   my $timeFromMidnightMS;

   ## 
   ## If this line is about a new process, make a note of the name and the
   ## associated process id, so that FbsSessions can be mapped to their 
   ## thread by name.
   ##
   if ($line =~ /^.*Thread:Process name assigned;NThread:(.*);DProcess:(.*);Name:(.*)$/i)
   {
      my $threadId  = $1;
      my $processId = $2;
      my $processName = $3;
      $ProcessNames{$processId} = $processName ;
   }

   ## 
   ## If this line is about a new process, make a note of the name and the
   ## associated process id, so that FbsSessions can be mapped to their 
   ## thread by name when the csv is generated.
   ##
   if (($line =~ /^.*Thread:Thread created;NThread:(.*);DProcess:(.*);Name:(.*)$/i) ||
      ($line =~ /^.*Thread:Thread name assigned;NThread:(.*);DProcess:(.*);Name:(.*)$/i))
      {
      my $threadId  = $1;
      my $processId = $2;
      my $threadName = $3;
      my $fullThreadName = $ProcessNames{$processId} . ":" . $threadName;
      $ThreadNames{$threadId} = $fullThreadName;
   }

   ##
   ## Determine timestamp. If this time is beyond the heartbeat, 
   ## take a snapshot and 
   ##
   if ($line =~ /^(\d\d):(\d\d):(\d\d)\.(\d{3})/)
   {
      $timeFromMidnightMS = ((($1 * 3600) + ($2 * 60) + $3) * 1000) + $4;
      # Set up the time for the first snapshot.
      if ($nextHeartBeatMS == -1) 
      {
         $nextHeartBeatMS = $timeFromMidnightMS + $firstHeartBeatTimeMS;
      }
   }

   ##
   ## If heartbeat reached, take snapshot of bmp memory per thread
   ## and set next heartbeat time.
   ##
   while ($timeFromMidnightMS >= $nextHeartBeatMS)
   {
      $nextHeartBeatMS += $heartBeatMS;
      # take a snapshot of the current bitmap memory usage per thread
      while ((my $thread, my $bmpMemory) = each(%bmpMemoryPerThread))
      {
           $arrayOfSnapshots[$heartBeatCount]{$thread} = $bmpMemory;
      }
      $heartBeatCount++;
   }

   ## FBS Client-side traces.
   if ($line =~ m/\tFBSCLI: /)
   {
      ##
      ## If this line is an FBSCLI trace, and it contains iSSH then
      ## it gives a chance to map a client thread ID to a session handle.
      ## 
      if ( $line =~ m/iSSH=(\w*);.*Thread ID:(.*)$/)
      {
         my $ServerSessionHandle = $1;
         my $thread = $2;
         if ($thread ne "0x00000000")
         {
            $SessionThreadMap{$ServerSessionHandle} = $thread;
         }
      }
   }

   ## 
   ## FBS Server-side traces.
   ##
   if ($line =~ m/\tFBSERV: /)
   {
      ## The line must have a s= parameter to be useful - the session server handle.
      ## Any FBSERV line without this is not considered for parsing.
      if ($line =~ m/; iSSH=(\w*);/)
      {
         my $FbsSessionHandle = $1;
         my $thread = "Unknown Thread [Session=$FbsSessionHandle]";
         if (defined($SessionThreadMap{$FbsSessionHandle}))
         {
            $thread = $SessionThreadMap{$FbsSessionHandle};
         }
         if ($line =~ m/# Server resource destroyed; .*iH=(\w*);/)
         {
            my $bmpHandle = $1;
            my $bmpIdentifier = "$FbsSessionHandle:$bmpHandle";
            if (defined($bmpMemoryByServerHandle{$bmpIdentifier}))
            {
               $bmpMemoryPerThread{$thread} -= $bmpMemoryByServerHandle{$bmpIdentifier};
               delete $bmpMemoryByServerHandle{$bmpIdentifier};
            }
         }
         if ($line =~ m/# Server bitmap resized; .*iOldH=(\w*); iNewH=(\w*); newbytes=(\d*);/)
         {
            # When a bitmap is resized, the amount of memory held by the bitmap may change
            # and the bitmap localhandle will change.
            my $oldBmpHandle = $1;
            my $newBmpHandle = $2;
            my $newBmpBytes = $3;
            my $oldBmpIdentifier = "$FbsSessionHandle:$oldBmpHandle";
            my $newBmpIdentifier = "$FbsSessionHandle:$newBmpHandle";
            if (defined($bmpMemoryByServerHandle{$oldBmpIdentifier}))
            {
               $bmpMemoryPerThread{$thread} -= $bmpMemoryByServerHandle{$oldBmpIdentifier};
               delete $bmpMemoryByServerHandle{$oldBmpIdentifier};
            }
            $bmpMemoryPerThread{$thread} += $newBmpBytes;
            $bmpMemoryByServerHandle{$newBmpIdentifier} = $newBmpBytes;           
         }
         elsif ($line =~ m/#.*iOldH=(\w*); iNewH=(\w*);/)
         {
            # When a bitmap is compressed, cleaned or resized, the bitmap local handle changes
            my $oldBmpHandle = $1;
            my $newBmpHandle = $2;
            my $oldBmpIdentifier = "$FbsSessionHandle:$oldBmpHandle";
            my $newBmpIdentifier = "$FbsSessionHandle:$newBmpHandle";
            if (defined($bmpMemoryByServerHandle{$oldBmpIdentifier}))
            {
               my $bytes = $bmpMemoryByServerHandle{$oldBmpIdentifier};
               delete $bmpMemoryByServerHandle{$oldBmpIdentifier};
               $bmpMemoryByServerHandle{$newBmpIdentifier} = $bytes;
            }
         }
         elsif ($line =~ m/#.*iH=(\w*);.*bytes=(\d+);/)
         {
            # Duplication of a bitmap typically. When a bitmap is duplicated,
            # the memory is 'owned' by all threads that duplicate it.
            my $bmpHandle = $1;
            my $bmpBytes = $2;
            my $bmpIdentifier = "$FbsSessionHandle:$bmpHandle";
            $bmpMemoryPerThread{$thread} += $bmpBytes;
            $bmpMemoryByServerHandle{$bmpIdentifier} = $bmpBytes;
         }
      }
   }
}

close (INPUT_FILE);


##
## Make a map of unique threads across all snapshots
## This is so only one occurrence of each thread will appear
## in the csv file.
##
my %uniqueThreads = ();
for my $i (0..$#arrayOfSnapshots)
{
   for my $thread (keys %{$arrayOfSnapshots[$i]})
   {
      $uniqueThreads{$thread} = 1;
   }
}

##
## Start writing to file.
## First row, which contains the heartbeat number column headings...
##
my $OUTPUT_FILENAME = sprintf("%s.csv", $TRACE_FILENAME);
open(OUTPUT_FILE,">$OUTPUT_FILENAME") or die $!;
print OUTPUT_FILE "Session$CSV_DELIMITER";
for my $i (0..$heartBeatCount)
{
    print OUTPUT_FILE "$i$CSV_DELIMITER";
}

##
## For each subsequent row, print the first thread and the
## memory at each snapshot...
##
print OUTPUT_FILE "\n";
while ((my $thread, my $dummy) = each(%uniqueThreads))
{
    # Resolve the thread to its full name...
    print OUTPUT_FILE "$thread";
    if (defined($ThreadNames{$thread}) )
    {
       my $threadName = $ThreadNames{$thread};
       print OUTPUT_FILE ":$threadName";
    }
    print OUTPUT_FILE "$CSV_DELIMITER";

    # print the memory use per thread, for each snapshot...
    for my $i (0..$#arrayOfSnapshots)
    {
       my %snapshot = %{$arrayOfSnapshots[$i]};
       while ((my $snapshotThread, my $bmpMemory) = each(%snapshot))
       {
           if ($snapshotThread eq $thread) 
           {
              print OUTPUT_FILE "$bmpMemory";
           }
       }
       print OUTPUT_FILE "$CSV_DELIMITER";
    }
    print OUTPUT_FILE "\n";
}
close (OUTPUT_FILE);

