#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>


int main()
{
    // 1. 创建一个句柄
    MYSQL* connect_fd = mysql_init(NULL);

    // 2. 连接
    if (mysql_real_connect(connect_fd, "47.101.192.120", "root", "peiban493.", "weblog", 3306, NULL, 0) == NULL)
    {
        printf("连接失败! %s\n", mysql_error(connect_fd));
        return 1;
    }
    printf("连接成功！\n");

    // 3. 设置编码方式
    //  mysql server 部分最初安装的时候已经设置成了 utf8
    //  也得在 客户端 这边也得设置成 utf8
    mysql_set_character_set(connect_fd, "utf8");

    // 4. 拼接 sql 语句
    char sql[1024 * 4] = "select * from blog_table";

    int ret = mysql_query(connect_fd, sql);
    if (ret < 0)
    {
        printf("mysql query fail! %s\n", mysql_error(connect_fd));
        mysql_close(connect_fd);
    }

    // 5. 遍历结果集合
    // MYSQL_RES 表示 select 的结果集合
    MYSQL_RES* result = mysql_store_result(connect_fd);
    //  a) 获取结果集合的行数和列数
    int rows = mysql_num_rows(result);
    int fields = mysql_num_fields(result);
    //  b) 根据行数和列数来遍历结果
    for (int i = 0; i < rows; i++)
    {
        // 一次获取一行数据
        MYSQL_ROW row = mysql_fetch_row(result);
        for (int j = 0; j < fields; j++)
        {
            printf("%s\t", row[j]);
        }
        printf("\n");
    }
    // 释放结果集合
    mysql_free_result(result);

    mysql_close(connect_fd);


    return 0;
}

