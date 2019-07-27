FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp 

Test:db_test.cpp db.hpp
	g++ -o $@ db_test.cpp $(FLAG)

.PHONY:clean
clean:
	rm Test
