#!/bin/bash

cd Phobos
cat $(ls *.cpp *.h ; find ImageCache ImageProcessing ImportWizard PhotoContainers ProcessWizard Utils Widgets -name '*.cpp' -or -name '*.h' ;) |\
   grep -v '^$' |\
   grep -v '^#include' |\
   wc -l