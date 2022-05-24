#Set here the path yo the jxs reference files
REF_XS_REP=/media/charles/Store/Content/ref_jxs
#and the path of the decoded pgx references
REF_XS_PGX_REP=/media/charles/Store/Content/ref_jxs_pgx

rm md5list.txt
for f in `ls ${REF_XS_REP} | sort`
do
	#Test the decoder
	filename=`basename $f .jxs`
	bin/linux64/jxs_decoder ${REF_XS_REP}/${f} $filename.pgx >> compression.log 2>&1
	for plane in `ls ${REF_XS_PGX_REP} | grep "^${filename}_" | grep raw`
	do
		cmp -l ./${plane} ${REF_XS_PGX_REP}/${plane} 2>&1 >> comparison.log
		if [ $? -eq 0 ]; then
			echo "TEST PASSED $f (${plane})"
		else
			echo "TEST FAILED $f (${plane})"
		fi
		rm ./${plane}
	done
	rm $filename.pgx
	rm $filename*.h

	#generate md5sum of encoded files (to be compared to md5list.txt of other revisions)
	echo $filename.pgx >> md5list.txt
	#You may adapt here the profile and bpp to be tested
	bin/linux64/jxs_encoder -p 6 -b 3 ${REF_XS_PGX_REP}/${filename}.pgx out.tco && md5sum out.tco >> md5list.txt

done
