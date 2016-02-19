#!/usr/bin/perl -w
#
use strict;
use Math::BigInt;
use Fcntl "SEEK_SET";

die "Format: $0 [-s <systemmap-file>] <vmtronx-file> <keyring-file>\n"
    if ($#ARGV != 1 && $#ARGV != 3 ||
	$#ARGV == 3 && $ARGV[0] ne "-s");

my $sysmap = "";
if ($#ARGV == 3) {
    shift;
    $sysmap = $ARGV[0];
    shift;
}

my $vmtronx = $ARGV[0];
my $keyring = $ARGV[1];

#
# Parse the vmtronx section table
#
open FD, "objdump -h $vmtronx |" || die $vmtronx;
my @lines = <FD>;
close(FD) || die $vmtronx;

my @sections = ();

foreach my $line (@lines) {
    chomp($line);
    if ($line =~ /\s*([0-9]+)\s+(\S+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+2[*][*]([0-9]+)/
	) {
	my $seg  = $1;
	my $name = $2;
	my $len  = Math::BigInt->new("0x" . $3);
	my $vma  = Math::BigInt->new("0x" . $4);
	my $lma  = Math::BigInt->new("0x" . $5);
	my $foff = Math::BigInt->new("0x" . $6);
	my $align = 2 ** $7;

	push @sections, { name => $name,
			  vma => $vma,
			  len => $len,
			  foff => $foff };
    }
}

print "Have $#sections sections\n";

#
# Try and parse the vmtronx symbol table.  If the vmtronx file has been created
# from a vmtronz file with extract-vmtronx then the symbol table will be empty.
#
open FD, "nm $vmtronx 2>/dev/null |" || die $vmtronx;
@lines = <FD>;
close(FD) || die $vmtronx;

my %symbols = ();
my $nr_symbols = 0;

sub parse_symbols(@) {
    foreach my $line (@_) {
	chomp($line);
	if ($line =~ /([0-9a-f]+)\s([a-zA-Z])\s(\S+)/
	    ) {
	    my $addr = "0x" . $1;
	    my $type = $2;
	    my $name = $3;

	    $symbols{$name} = $addr;
	    $nr_symbols++;
	}
    }
}
parse_symbols(@lines);

if ($nr_symbols == 0 && $sysmap ne "") {
    print "No symbols in vmtronx, trying $sysmap\n";

    open FD, "<$sysmap" || die $sysmap;
    @lines = <FD>;
    close(FD) || die $sysmap;
    parse_symbols(@lines);
}

die "No symbols available\n"
    if ($nr_symbols == 0);

print "Have $nr_symbols symbols\n";

die "Can't find system certificate list"
    unless (exists($symbols{"__cert_list_start"}) &&
	    exists($symbols{"__cert_list_end"}));

my $start = Math::BigInt->new($symbols{"__cert_list_start"});
my $end = Math::BigInt->new($symbols{"__cert_list_end"});
my $size = $end - $start;

printf "Have %u bytes of certs at VMA 0x%x\n", $size, $start;

my $s = undef;
foreach my $sec (@sections) {
    my $s_name = $sec->{name};
    my $s_vma = $sec->{vma};
    my $s_len = $sec->{len};
    my $s_foff = $sec->{foff};
    my $s_vend = $s_vma + $s_len;

    next unless ($start >= $s_vma);
    next if ($start >= $s_vend);

    die "Cert object partially overflows section $s_name\n"
	if ($end > $s_vend);

    die "Cert object in multiple sections: ", $s_name, " and ", $s->{name}, "\n"
	if ($s);
    $s = $sec;
}

die "Cert object not inside a section\n"
    unless ($s);

print "Certificate list in section ", $s->{name}, "\n";

my $foff = $start - $s->{vma} + $s->{foff};

printf "Certificate list at file offset 0x%x\n", $foff;

open FD, "<$vmtronx" || die $vmtronx;
binmode(FD);
die $vmtronx if (!defined(sysseek(FD, $foff, SEEK_SET)));
my $buf = "";
my $len = sysread(FD, $buf, $size);
die "$vmtronx" if (!defined($len));
die "Short read on $vmtronx\n" if ($len != $size);
close(FD) || die $vmtronx;

open FD, ">$keyring" || die $keyring;
binmode(FD);
$len = syswrite(FD, $buf, $size);
die "$keyring" if (!defined($len));
die "Short write on $keyring\n" if ($len != $size);
close(FD) || die $keyring;
