#!/bin/bash

HEADER=revision.h

export LC_ALL="C"

echo '#ifndef __REVISION_H' > $HEADER
echo '#define __REVISION_H' >> $HEADER

echo >> $HEADER
echo '#define __SVN_PROJECT_REV_ ' '"'$(git log | grep 'commit')'"' >> $HEADER
echo >> $HEADER

echo '#endif' >> $HEADER
