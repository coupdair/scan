#! /bin/bash

##list='scan stepper grab rs232 CImg CImg.Tool'
list='scan CImg.Tool'
#move code versions to temporary directory
for d in $list
do
  mkdir -p ../DoxIt.tmp/$d
  mv ../$d/*.v?.?.?.* ../DoxIt.tmp/$d
done
#stepper help output (xterm-color unformated)
dir=../scan
fb=scan
bin=$dir/$fb
out=$fb.help
$bin -h -I 2> $dir/$out; cat -v $dir/$out | sed 's/\^\[\[//g' | sed 's/1m//g' | sed 's/0;0;0m//g' | sed 's/0;35;59m//g' | sed 's/0;32;59m//g' | sed 's/4;31;59m//g' > $dir/$out.output;rm $dir/$out
#create documentation
cat scan.Doxygen | sed 's/\ VERSION/\ '`cat VERSION`'/g' > scan.Doxygen.version
doxygen scan.Doxygen.version
rm scan.Doxygen.version
#put back code versions
for d in $list
do
  mv ../DoxIt.tmp/$d/* ../$d/
  rmdir ../DoxIt.tmp/$d
done
rmdir ../DoxIt.tmp

