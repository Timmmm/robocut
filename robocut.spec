# norootforbuild

Name: robocut
Group: Productivity/Graphics/Other
Version: 1.0.10
Release: 1
License: GPL-3.0+
Summary: A program to cut SVG with CC220-20 and Silhouette SD/Cameo
Autoreqprov: on
# http://custom.ultramini.net/robocut-compatible-with-silhouette-cameo/
URL: https://github.com/nosliwneb/robocut
Source0: robocut-%{version}_1c019e7fad.zip
Source1: opensuse_chameleon_silhouette_x6.svg
BuildRoot:%{_tmppath}/%{name}-%{version}-build
BuildRequires: libqt4-devel libusb-1_0-devel unzip
# sudo apt-get install libqt4-dev libusb-1.0-0-dev git-core

%description
Robocut is a simple graphical program to allow you to cut graphics with a
Graphtec Craft Robo 2 Vinyl Cutter model CC220-20 and Sihouette SD.

It can read SVG files produced by Inkscape, but it should also work with other
SVG files.  Unlike the official programs, Robocut can run on Linux and probably
Mac OS X with a tiny bit of work.  It may work with the newer CC330-20 model if
the USB IDs are changed, but I don’t have one to test with.

Inside the “examples” folder there is also a registration marks template fully
functional (yes, the Silhouette Cameo is able to recognize registration marks
also under Robocut, just put the page with the arrow pointing toward the
plotter and align the sheet with the top left corner of the cutting mat).

Authors

	Tim Hutt, Markus Schulz

%prep
%setup -n %{name}-master

# opensuse_chameleon_silhouette_x6.svg
cp %{SOURCE1} .

%build
qmake
make

%install
install -D robocut                    $RPM_BUILD_ROOT/usr/bin/robocut
install -m 0644 -D images/robocut.xpm $RPM_BUILD_ROOT/usr/share/pixmaps/robocut.xpm
install -d                            $RPM_BUILD_ROOT/usr/share/robocut/examples
install -m 0644 *.svg examples/*      $RPM_BUILD_ROOT/usr/share/robocut/examples
install -m 0644 -D manfile.txt        $RPM_BUILD_ROOT/usr/share/man/man1/robocut.1

# cleanup

%files
%defattr(-,root,root)
%doc readme.txt changelog

%_bindir/*
%_datadir/pixmaps/*
%dir %_datadir/robocut
%_datadir/robocut/*
%_mandir/man1/*

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
