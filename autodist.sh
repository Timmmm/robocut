rm Robocut*.tar.gz
rm -r ./ddist/
rm -r ./.tmp/
#./autogen.sh
#./configure
#dpkg-depcheck -d ./configure # to find what libs to use in control file
make distclean
qmake
make -j
help2man -N ./Robocut > ./debian/Robocut.1
make dist
mkdir ddist
cp Robocut*.tar.gz ./ddist/
cd ddist
tar -xvzf Robocut*.tar.gz 
rename -v 's/\.tar\.gz$/\.orig\.tar\.gz/' *.tar.gz
rename -v 's/Robocut/robocut_/' *.tar.gz
cd Robocut*
mkdir debian
cd ../..
cp -r ./debian/* ./ddist/Robocut*/debian
cd ddist
cd Robocut*
# lucid maverick unstable
dch -i
#DIST=maverick ARCH=i386 pdebuild
#dpkg-buildpackage
debuild -S
#debuild
cd ..
lintian -Ivi *.changes
ls -l
#exit

#cd xsublim*
#DIST=sid ARCH=i386 pdebuild
#MDIST=sid ARCH=amd64 pdebuild
#cd ..

#sudo pbuilder --build robocut_1.0.0-0ubuntu1.dsc

#dput ppa:schulz-alpharesearch/xsublim xsublim_2.0.2-1_source.changes
