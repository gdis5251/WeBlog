// 创建一些相关的类来封装数据库的操作
// Model 部分
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

namespace blog_system
{
static MYSQL* MySQLInit() // 初始化一个 MYSQL 的句柄，并建立连接
{
    // 1. 创建一个句柄
    MYSQL* connect_fd = mysql_init(NULL);

    // 2. 建立连接
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
        return NULL;
    }

    // 3. 设定字符编码
    mysql_set_character_set(connect_fd, "utf8");

    return connect_fd;
}

static void MySQLRelease(MYSQL* connect_fd)
{
    mysql_close(connect_fd);
}

// 创建一个类，用于操作博客表的类
class BlogTable
{
public:
    BlogTable(MYSQL* connect_fd)
        :mysql_(connect_fd)
    {
        // 通过这个构造函数，获取到数据库的操作句柄
    }

    // 以下操作的相关参数都统一使用 JSON 的方式
    
    bool Insert(const Json::Value& blog)
    {
        const std::string& content = blog["content"].asString();
        // 使用指针指针防止内存泄漏
        std::unique_ptr<char> to(new char[content.size() * 2 + 1]);
        mysql_real_escape_string(mysql_, to.get(), content.c_str(), content.size());

        // 核心就是拼装 SQL 语句
        std::unique_ptr<char> sql(new char[content.size() * 2 + 4096]);
        sprintf(sql.get(), "insert into blog_table value(null, '%s', '%s', %d, '%s')", 
                blog["title"].asCString(), 
                to.get(),
                blog["tag_id"].asInt(),
                blog["create_time"].asCString());

        int ret = mysql_query(mysql_, sql.get());
        if (ret < 0)
        {
            printf("插入失败 '%s'\n", mysql_error(mysql_));
            return false;
        }
        printf("插入成功！\n");

        return true;
    }

    // blogs 作为一个输出型参数
    // tag_id 根据标签来筛选输出结果
    bool SelectAll(Json::Value* blogs, const std::string& tag_id = "")
    {
        // 查找不需要太长的 sql ，所以固定长度就行
        char sql[1024 * 4] = {0};
        if (tag_id.empty())
        {
            // 此时不需要按照 tag_id 来筛选 
            sprintf(sql, "select blog_id, title, tag_id, create_time from blog_table");
        }
        else
        {

            sprintf(sql, "select blog_id, title, tag_id, create_time from blog_table\
                    where tag_id = %d", std::stoi(tag_id));

        }

        int ret = mysql_query(mysql_, sql);
        if (ret < 0)
        {
            printf("执行查找所有博客失败 '%s'\n", mysql_error(mysql_));
        }

        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows = mysql_num_rows(result);
        // 遍历结果集合，然后把结果写到 blog 参数中，返回给调用者
        for (int i = 0; i < rows; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            Json::Value blog;
            // row[] 中的下标和上面 select 写的相关列是相关联的
            blog["blog_id"] = atoi(row[0]);
            blog["title"] = row[1];
            blog["tag_id"] = atoi(row[2]);
            blog["create_time"] = row[3];

            blogs->append(blog);
        }
        // MySQL 查询的结果集合要及时释放
        mysql_free_result(result);
        printf("执行查找博客成功! 共查找到 %d 篇博客\n", rows);

        return true;
    }

    // blog 也是输出型参数
    // 根据当前的 blog_id 在数据库中找到具体的 blog 内容
    // 通过 blog 返回给调用者
    bool SelectOne(int32_t blog_id, Json::Value* blog)
    {
        char sql[1024] = {0};
        sprintf(sql, "select blog_id, title, content, tag_id, create_time from blog_table \
                where blog_id = %d", blog_id);

        int ret = mysql_query(mysql_,sql);
        if (ret < 0)
        {
            printf("执行查找失败！ '%s'\n", mysql_error(mysql_));
            return false;

        }
        
        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows = mysql_num_rows(result);
        if (rows != 1)
        {
            printf("查找到的博客不是 1 条，实际有 %d 条！\n", rows);
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        (*blog)["blog_id"] = atoi(row[0]);
        (*blog)["title"] = row[1];
        (*blog)["content"] = row[2];
        (*blog)["tag_id"] = atoi(row[3]);
        (*blog)["create_time"] = row[4];

        return true;
    }

    bool Update(const Json::Value& blog)
    {
        const std::string& content = blog["content"].asString();
        // 使用指针指针防止内存泄漏
        std::unique_ptr<char> to(new char[content.size() * 2 + 1]);
        mysql_real_escape_string(mysql_, to.get(), content.c_str(), content.size());

        // 核心就是拼装 SQL 语句
        std::unique_ptr<char> sql(new char[content.size() * 2 + 4096]);
        sprintf(sql.get(), "update blog_table set title='%s', content='%s', tag_id=%d where blog_id = %d", \
                blog["title"].asCString(),
                to.get(),
                blog["tag_id"].asInt(),
                blog["blog_id"].asInt());

        int ret = mysql_query(mysql_, sql.get());
        if (ret != 0)
        {
            printf("更新博客失败！'%s'\n", mysql_error(mysql_));
            return false;
        }
        printf("更新博客成功！\n");

        return true;
    }

    bool Delete(int32_t blog_id)
    {
        char sql[1024 * 4] = {0};
        sprintf(sql, "delete from blog_table where blog_id = %d", blog_id);

        int ret = mysql_query(mysql_, sql);
        if (ret != 0)
        {
            printf("删除博客失败！'%s'\n", mysql_error(mysql_));
            return false;
        }
        printf("删除博客成功！\n");
        return true;
    }
private:
    MYSQL* mysql_;
};


class TagTable
{
public:
    TagTable(MYSQL* mysql)
        :mysql_(mysql)
    {

    }

    bool Insert(const Json::Value& tag)
    {
        char sql[1024 * 4] = {0};
        sprintf(sql, "insert into tag_table value(null, '%s')",
                tag["tag_name"].asCString());

        int ret = mysql_query(mysql_, sql);
        if (ret != 0)
        {
            printf("插入标签失败'%s'\n", mysql_error(mysql_));
            return false;
        }
        printf("插入标签成功！\n");


        return true;
    }

    bool Delete(int32_t tag_id)
    {
        char sql[1024 * 4] = {0};
        sprintf (sql, "delete from tag_table where tag_id = %d", tag_id);
        int ret = mysql_query(mysql_, sql);

        if (ret != 0)
        {
            printf("删除标签失败、'%s'\n", mysql_error(mysql_));
            return false;
        }
        printf("删除标签成功\n");

        return true;
    }

    bool SelectAll(Json::Value* tags)
    {
        char sql[1024 * 4] = {0};
        sprintf(sql, "select tag_id, tag_name from tag_table");

        int ret = mysql_query(mysql_, sql);
        if (ret != 0)
        {
            printf("查找标签失败！ '%s'\n", mysql_error(mysql_));
            return false;
        }

        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows = mysql_num_rows(result);
        for (int i = 0; i < rows; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            Json::Value tag;
            tag["tag_id"] = atoi(row[0]);
            tag["tag_name"]= row[1];

            tags->append(tag);
        }
        printf("查找标签成功！共 %d 条\n", rows);

        return true;
    }
private:
    MYSQL* mysql_;

};

}; // end of blog_system

