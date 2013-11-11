# TextServerClient.pm
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#
package FShell::TextServerClient;

use strict;
use Carp;
use Pcons;

sub new {
        my $proto = shift;
        my $class = ref($proto) || $proto;
	my $self = {
		PCONS_NAME	=> undef,
		SERVER_PROCESS	=> undef,
		SERVER_PROMPT	=> 'ok',
		PCONS_READER	=> undef,
		PCONS_WRITER	=> undef,
	};
	bless ($self, $class);
	return $self;
}

sub Connected {
	my ($self) = @_;
	return (defined $self->{PCONS_READER}) && (defined $self->{PCONS_WRITER});
}

sub PconsName {
	my ($self, $name) = @_;
	if (defined $name) {
		croak "Cannot change pcons name while connected" if ($self->Connected());
		$self->{PCONS_NAME} = $name;
	}
	return $self->{PCONS_NAME};
}

sub ServerProcess {
	my ($self, $proc) = @_;
	if (defined $proc) {
		croak "Cannot set server process while connected" if ($self->Connected());
		$self->{SERVER_PROCESS} = $proc;
	}
	return $self->{SERVER_PROCESS};
}

sub Disconnect {
	my ($self) = @_;
	undef $self->{PCONS_READER};
	undef $self->{PCONS_WRITER};
}

sub Connect {
	my ($self) = @_;
	croak "Cannot connect to server before process has been set" if (!defined $self->ServerProcess());
	croak "Cannot connect to server becore persisten console name has been set" if (!defined $self->PconsName());
	return if (Connected());

	my $reader = Pcons::OpenReader($self->{PCONS_NAME});
	if (!defined $reader) {
		$self->{SERVER_PROCESS} =~ s|^\s*||;
		my ($procName, @procArgs) = split(/\s/, $self->{SERVER_PROCESS});
		Pcons::Create($self->{PCONS_NAME}, $procName, join(' ', @procArgs)) or croak "Cannot create persistent console $self->{PCONS_NAME} with process $procName: $!";
		$reader = Pcons::OpenReader($self->{PCONS_NAME}) or croak ("Cannot open pcons reader to '$self->{PCONS_NAME}': $!");
		$self->WaitPrompt($reader) or croak ("Prompt $self->{SERVER_PROMPT} not received from server");
	}
	my $writer = Pcons::OpenWriter($self->{PCONS_NAME});
	croak ("Cannot open pcons writer to '$self->{PCONS_NAME}': $!") unless defined $writer;
	$self->{PCONS_WRITER} = $writer;
	$self->{PCONS_READER} = $reader;
	return 1;
}

sub WaitPrompt {
	my ($self, $reader) = @_;
	my @lines;
	$reader = $self->{PCONS_READER} unless defined $reader;
	while (my $line = <$reader>) {
		chomp $line;
		return \@lines if ($line eq $self->{SERVER_PROMPT});
		push @lines, $line;
	}
	return undef;
}

sub Execute {
	my ($self, $command) = @_;
	croak "Not connected to server" unless $self->Connected();
	croak "No command specified" unless (defined $command);
	my $writer = $self->{PCONS_WRITER};
	print $writer "$command\n";
	return $self->WaitPrompt();
}

1;

__END__

=head1 NAME

FShell::TextServerClient - perl interface to Fshell server commands

=head1 SYNOPSIS

	use FShell::TextServerClient;

	my $client = new Fshell::TextServerClient;
	$client->PconsName('server_console');
	$client->ServerProcess('server_executable --arguments');
	$client->Connect();

	my $output = $client->Execute('command');
	foreach (@$output) {
		process_line($_);
	}

=head1 DESCRIPTION

This module provides a perl interface to FShell command servers, i.e. those FShell commands the derive from
class C<CServerCommandBase>. This module is analgous to the C++ class C<CClientBase>, providing similar
functionality in perl.

=head1 COPYRIGHT

Copyright (c) 2009-2010 Accenture. All rights reserved.

=head1 VERSION

Version 0.01 (April 1st, 2009)

=head1 SEE ALSO

perl(1)
Pcons

=cut
