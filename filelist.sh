#! /bin/bash
#
# Automatic resource file list generation
# Created by Dimok

outFile="./src/resources/filelist.h"
count_old=$(cat $outFile 2>/dev/null | tr -d '\n\n' | sed 's/[^0-9]*\([0-9]*\).*/\1/')

count=0
if [[ $OSTYPE == darwin* ]];
then

for i in $(gfind ./data/images/ ./data/sounds/ ./data/fonts/ -maxdepth 1 -type f  \( ! -printf "%f\n" \) | sort -f)
do
	files[count]=$i
	count=$((count+1))
done

else

for i in $(find ./data/images/ ./data/sounds/ ./data/fonts/ -maxdepth 1 -type f  \( ! -printf "%f\n" \) | sort -f)
do
	files[count]=$i
	count=$((count+1))
done

fi

if [ "$count_old" != "$count" ] || [ ! -f $outFile ]
then

echo "Generating filelist.h for $count files." >&2
cat <<EOF > $outFile
/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes $count files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

typedef struct _RecourceFile
{
	const char          *filename;
	const unsigned char *DefaultFile;
	const unsigned int  &DefaultFileSize;
	unsigned char	    *CustomFile;
	unsigned int        CustomFileSize;
} RecourceFile;

EOF

echo 'static RecourceFile RecourceList[] =' >> $outFile
echo '{' >> $outFile

for i in ${files[@]}
do
	filename=${i%.*}
	extension=${i##*.}
	echo -e '\t{"'$i'", NULL, 0, NULL, 0},' >> $outFile
done

echo -e '\t{NULL, NULL, 0, NULL, 0}' >> $outFile
echo '};' >> $outFile

echo '' >> $outFile
echo '#endif' >> $outFile

fi
