.PHONY:all
all:insert select

insert:insert.cpp
	g++ -o $@ $^ -L/usr/lib64/mysql -lmysqlclient -g

select:select.cpp
	g++ -o $@ $^ -L/usr/lib64/mysql -lmysqlclient -g

.PHONY:clean
clean:
	rm insert select
