export SERVER_IP=192.168.9.142
export SERVER_PORT=18080

#TEST_1 is empty db
export TEST_2_DATA_COUNT=1000
export TEST_3_DATA_COUNT=10000

export BENCH_DIR=$PWD
cd ../build

#clean previous db
echo Clean previous db
rm -fr quarks_db

#run quarks, disable stdout
echo Run quarks, disable stdout
./ocv_microservice_crow >/dev/null 2>&1 &
export QUARKS_PID=$!
echo Quarks PID: $QUARKS_PID

#wait for quarks to start up
sleep 5

#save memory before test
echo Save memory before test
cd $BENCH_DIR
pmap $QUARKS_PID > mem_1_before.txt

#empty db, send 2k requests per sec for 10 secs
echo Test 1: empty db, send 2k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 20000 --rate 2000 > test_1_empty_db_20k_requests.txt
#pmap $QUARKS_PID > mem_1_empty_db_20k_requests.txt

#empty db, send 5k requests per sec for 10 secs
echo Test 1: empty db, send 5k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 50000 --rate 5000 > test_1_empty_db_50k_requests.txt
#pmap $QUARKS_PID > mem_1_empty_db_50k_requests.txt

#empty db, send 8k requests per sec for 10 secs
echo Test 1: empty db, send 8k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 80000 --rate 8000 > test_1_empty_db_80k_requests.txt
pmap $QUARKS_PID > mem_1_empty_db_80k_requests.txt

# TEST 2
#insert dummy data
echo Test 2: insert dummy data
for i in $(seq 1 $TEST_2_DATA_COUNT);
do wget -q -nv -O set.txt http://localhost:18080/put?body="{\"key\":\"k$i\",\"value\":{\"msg\":\"m$i\"}}";
done
pmap $QUARKS_PID > mem_2_1k_db.txt

#1k db, send 2k requests per sec for 10 secs
echo Test 2: 1k db, send 2k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 20000 --rate 2000 > test_2_1k_db_20k_requests.txt
#pmap $QUARKS_PID > mem_2_1k_db_20k_requests.txt

#1k db, send 5k requests per sec for 10 secs
echo Test 2: 1k db, send 5k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 50000 --rate 5000 > test_2_1k_db_50k_requests.txt
#pmap $QUARKS_PID > mem_2_1k_db_50k_requests.txt

#1k db, send 8k requests per sec for 10 secs
echo Test 2: 1k db, send 8k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 80000 --rate 8000 > test_2_1k_db_80k_requests.txt
pmap $QUARKS_PID > mem_2_1k_db_80k_requests.txt


# TEST 3
#insert dummy data
echo Test 3: insert dummy data
for i in $(seq 1 $TEST_3_DATA_COUNT);
do wget -q -nv -O set.txt http://localhost:18080/put?body="{\"key\":\"k$i\",\"value\":{\"msg\":\"m$i\"}}";
done
pmap $QUARKS_PID > mem_3_10k_db.txt

#10k db, send 2k requests per sec for 10 secs
echo Test 3: 10k db, send 2k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 20000 --rate 2000 > test_3_10k_db_20k_requests.txt
#pmap $QUARKS_PID > mem_3_10k_db_20k_requests.txt

#10k db, send 5k requests per sec for 10 secs
echo Test 3: 10k db, send 5k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 50000 --rate 5000 > test_3_10k_db_50k_requests.txt
#pmap $QUARKS_PID > mem_3_10k_db_50k_requests.txt

#10k db, send 8k requests per sec for 10 secs
echo Test 3: 10k db, send 8k requests per sec for 10 secs
httperf --hog --server $SERVER_IP --port $SERVER_PORT --method=GET --uri=/getall?keys=u* --num-conns 80000 --rate 8000 > test_3_10k_db_80k_requests.txt
pmap $QUARKS_PID > mem_3_10k_db_80k_requests.txt

#kill quarks
echo Term Quarks
kill -9 $QUARKS_PID

