#!perl

# CProgressLog

# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#

package CProgressLog;

# Constructor
#
# Parameters: Verbosity - 0 for batch operation (no progress reports)
#				- 1 for interactive operation (no missing lists by default)
#				- 2 for fully verbose operation (including detailed progress)
sub New($)
	{
	my $class = shift;
	my $self = {};

	bless $self, $class;

	$self->{VERBOSITY}=shift;
	$self->{ERRORCODE}=0;
	$self->{PROGRESSING}=0;

	return $self;
	}

# isBatch
#
# Tests verbosity setting
sub isBatch()
	{
	my $self = shift;

	return ($self->getVerbosity() == 0);
	}

# isVerbose
#
# Tests verbosity setting
sub isVerbose()
	{
	my $self = shift;

	return ($self->getVerbosity() == 2);
	}

# getVerbosity
#
# Verbosity getter
sub getVerbosity()
	{
	my $self = shift;

	return $self->{VERBOSITY};
	}

# getErrorCode
#
# ErrorCode getter. For use in determining exit value when terminating script.
sub getErrorCode()
	{
	my $self = shift;

	return $self->{ERRORCODE};
	}

# Warn
#
# Print warning
sub Warn($)
	{
	my $self = shift;
	my ($warning) = @_;

	$self->_NewMessage();

	print "$warning\n";
	}

# Error
#
# Print fatal error
sub Error($)
	{
	my $self = shift;
	my ($error) = @_;

	$self->_NewMessage();
	
	print "$error\n";
	$self->{ERRORCODE}=1;
	}

# Progress
#
# Display progress report
sub Progress($)
	{
	my $self = shift;
	my ($report) = @_;

	if (!($self->isBatch()))
		{
		if ($self->isVerbose())
			{
			if ($report ne "")
				{
				$self->_NewMessage();

				print STDOUT "$report\n";
				}
			}
		else
			{
			print STDOUT ".";

			$self->{PROGRESSING}=1;
			}
		}
	}

# ListMissing
#
# Display a list of files expected but missing
sub ListMissing(@)
	{
	my $self = shift;
	my @files = @_;

	if (($self->isVerbose()) || ($self->isBatch()))
		{
		$self->_NewMessage();
		print STDOUT "The following files were not found:\n";
		
		foreach my $file (@files)
			{
			print STDOUT "$file\n";
			}
		}
	}

# New message
# 
# Prepare a new line if the previous output was progress dots
sub _NewMessage()
	{
	my $self = shift;
	
	if ($self->{PROGRESSING})
		{
		print STDOUT "\n";
		$self->{PROGRESSING}=0;
		}
	}

return 1;
