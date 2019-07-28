FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp -lpthread -lgtest

Test:db_test.cpp
	g++ -o $@ db_test.cpp $(FLAG)


.PHONY:clean
clean:
	rm Test
