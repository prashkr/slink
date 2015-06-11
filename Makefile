compile:
	g++ ./src/crawl_cli.cpp ./include/hiredis/libhiredis.a -o crawl_cli
	g++ ./src/crawl_server.cpp ./include/hiredis/libhiredis.a -o crawl_server

install:
	cd ./include/hiredis; make clean; sudo make install;

cleanRedis:
	cd ./include/hiredis; make clean; 

clean:
	rm -rf *.o *~ crawl_cli crawl_server schedule.csv
