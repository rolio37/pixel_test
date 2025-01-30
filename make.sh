#!/bin/sh

arch=$(uname -p)
architecture="$1"
compilation="$2"
compiler="cc"
sources="pixel_test.c"
binbase="pixel_test"


if [ "$1" = "help" ]; then
 echo "syntaxe ./make.sh [[32/64 [debug]]\n"
 exit 0
fi


if [ "$compilation" = "debug" ]; then
 opts="-Wall -Wextra -g -lGL -lGLU -lglut `pkg-config --cflags --libs glu`"
else
 opts="-Wall -Wextra -O3 -s -lGL -lGLU -lglut `pkg-config --cflags --libs glu`"
fi



# compilation 32 bits forcé par l'utilisateur
if [ "$architecture" = "32" ]; then

 echo "********************************************************************************"
 echo " $compiler $sources -o "$binbase"32 -m32 $opts"
 echo "********************************************************************************"
 
 $compiler $sources -o "$binbase"32 -m32 $opts
 
 exit 0
fi


# compilation 64 bits forcé par l'utilisateur
if [ "$architecture" = "64" ]; then

 echo "********************************************************************************"
 echo " $compiler $sources -o "$binbase"64 -m64 $opts"
 echo "********************************************************************************"
 
  $compiler $sources -o "$binbase"64 -m64 $opts 

  exit 0 
fi



# détection automatique pour la compilation 32 Bits
if [ "$arch" = "i686" ]; then
 
 echo "********************************************************************************"
 echo " $compiler $sources -o "$binbase"32 $opts"
 echo "********************************************************************************"

 $compiler $sources -o "$binbase"32 $opts

 exit 0
fi


# détection automatique pour la compilation 64 Bits
if [ "$arch" = "amd64" ]; then

 echo "********************************************************************************"
 echo " $compiler $sources -o "$binbase"64 $opts"
 echo "********************************************************************************"

 $compiler $sources -o "$binbase"64 $opts

 exit 0
fi


# tentative de compilation sur une autre achitecture de processeur
echo "********************************************************************************"
echo " $compiler $sources -o "$binbase"_current_arch $opts"
echo "********************************************************************************"
$compiler $sources -o "$binbase"_current_arch $opts





