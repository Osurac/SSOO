# !/bin/bash

echo "Creating temporal folder ./tmp"
if [[ -d tmp ]] # [[  ]] = test
then rm -rf tmp/*
else mkdir tmp
fi

echo "Creating ./mountpoint"
if [[ -d mount-point ]]
then rm -rf mount-point/*
else mkdir mount-point
fi

echo "Starting fuse"
./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point' 2>outputerr.txt > output.txt &

sleep 5

echo "Copying fuseLib.c & myFS.h to ./tmp & ./mount-point"
cp src/fuseLib.c tmp/fuseLib.c
cp src/myFS.h tmp/myFS.h

cp src/fuseLib.c mount-point/fuseLib.c
cp src/myFS.h mount-point/myFS.h

#echo "Auditing disk"
#./my-fsck-static-64 virtual-disk

#sleep 5

echo "Showing differences between the original files and the copied ones"
if diff tmp/fuseLib.c mount-point/fuseLib.c
then echo "fuseLib.c [EQUALS]"
else echo "fuseLib.c [DIFFERENTS]"
fi

if diff tmp/myFS.h mount-point/myFS.h
then echo "myFS.h [EQUALS]"
else echo "myFS.h [DIFFERENTS]"
fi

echo "Modifiying the size of fuseLib.c on ./mount-point % ./tmp"
truncate -s 16384 mount-point/fuseLib.c
truncate -s 16384 tmp/fuseLib.c

echo "Comparing the modified files"
if diff tmp/fuseLib.c mount-point/fuseLib.c
then echo "fuseLib.c [EQUALS]"
else echo "fuseLib.c [DIFFERENTS]"
fi

echo "Creating a new text file in ./tmp and copiying it into ./mount-point"
echo "qwertyuiopasdfgherihuvdcksmlwodjeihnkwmdwojfihebnjklzxcvbnm" > tmp/file.txt
cp tmp/file.txt mount-point/file.txt

#echo "Auditing disk"
#./my-fsck-static-64 virtual-disk

#sleep 5

echo "Comparing the text files"
if diff tmp/file.txt mount-point/file.txt
then echo "file.txt [EQUALS]"
else echo "file.txt [DIFFERENTS]"
fi

echo "Modifying myFS.h on ./tmp & ./mount-point"
truncate -s 12288 mount-point/myFS.h
truncate -s 12288 tmp/myFS.h

#echo "Auditing disk"
#./my-fsck-static-64 virtual-disk

#sleep 5

echo "Comparing myFS.h on ./tmp & ./mount-point"
if diff tmp/myFS.h mount-point/myFS.h
then echo "myFS.h [EQUALS]"
else echo "myFS.h [DIFFERENTS]"
fi

echo "Ending fusermount"
fusermount -u mount-point

echo "Cleaning temporal folder ./tmp"
rm -rf tmp/*

echo "Cleaning ./mountpoint"
rm -rf mount-point/*

