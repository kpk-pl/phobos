#!/bin/bash

cd Phobos
cat $(ls *.cpp *.h ; find Dialogs ImageCache ImageProcessing ImportWizard PhotoContainers ProcessWizard Utils Widgets -name '*.cpp' -or -name '*.h' ;) |\
   grep -v '^$' |\
   grep -v '^#include' |\
   grep -v '^//' |\
   wc -l
