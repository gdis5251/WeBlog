// 通过这个程序使用 MySQL API 实现插入的功能
#include <iostream>
#include <cstdio>
#include <cstdlib>
// 编译器默认从 /usr/include 这个目录中查找头文件
// mysql.h 在 mysql 的目录中
#include <mysql/mysql.h>

int main(void)
{
    // 1. 创建一个数据库连接句柄
    MYSQL* connect_fd = mysql_init(NULL);
    // 2. 和数据库建立连接(在应用层建立连接)
    //      连接过程需要指定一些必要的信息
    //      a) 连接句柄
    //      b) 服务器的 ip 地址
    //      c) 用户名
    //      d) 密码
    //      e) 数据库名(we_blog)
    //      f) 服务区的端口号 数据库默认端口号 3306
    //      g) unix_sock NULL
    //      h) client_flag 0
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

    // 4. 拼接 SQL 语句
    char sql[1024 * 4] = {0};
    char title[] = "立一个 flag";
    char content[] = "我要拿 100w 年薪";
    int tag_id = 1;
    char date[] = "2019/07/25";

    sprintf(sql, "insert into blog_table value(null, '%s', '%s', %d, '%s')", title, content, tag_id, date);
    printf("sql: %s\n", sql);

    // 5. 让数据库服务器执行sql
    int ret = mysql_query(connect_fd, sql);
    if (ret < 0)
    {
        printf("执行 sql 失败!, %s\n", mysql_error(connect_fd));
        mysql_close(connect_fd);
    }
    printf("插入成功！\n");


    // 数据库使用完成断开连接
    mysql_close(connect_fd);

    return 0;
}
