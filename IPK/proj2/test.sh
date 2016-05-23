#!/bin/sh

test_file="salad_quote1"
test_file_orig="salad_quote1_orig"
ports="10101 11011 10001"

#preklad oboch programov
make

make clean

# spustenie servera na pozadi vyber volneho portu
for connection_port in $ports; do
    ./server -p $connection_port &
    retval="$?"
    server_pid="$!"
    if [ $retval -eq 0 ]; then
        break
    fi
done

cp $test_file $test_file_orig

# upload suboru pomocou klienta
./client -h 127.0.0.1 -p $connection_port -u $test_file


diff_output=$(cmp $test_file $test_file_orig)
    if [ -z "$diff_output" ]; then
        echo "Upload: OK"
    else
        echo "Upload: FAILED:"
        echo "$diff_output"
    fi

# download suboru pomocou klienta
./client -h 127.0.0.1 -p $connection_port -d $test_file

diff_output=$(cmp $test_file $test_file_orig)
    if [ -z "$diff_output" ]; then
        echo "Download: OK"
    else
        echo "Download: FAILED:"
        echo "$diff_output"
    fi

rm $test_file_orig
kill -9 $server_pid
exit 0
