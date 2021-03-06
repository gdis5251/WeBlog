#include <iostream>
#include <gtest/gtest.h>
#include "../db.hpp"

#if 0
// 使用这个程序来测试刚才封装的 mysql 是否正确
void TestBlogTbale()
{
    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::BlogTable blog_table(mysql);
    bool ret = false;
    Json::StyledWriter writer;

    Json::Value blog;
    
    // 单元测试 gtest google 提供的一个单元测试框架


    // 测试插入
    // blog["title"] = "My Carrer!";
    // blog["content"] = "我要拿1000W年薪！";
    // blog["tag_id"] = 1;
    // blog["create_time"] = "2019/07/27";

    // bool ret = blog_table.Insert(blog);
    // printf ("insert : %d\n", ret);

    // 测试查找
    // Json::Value blogs;
    // ret = blog_table.SelectAll(&blogs);
    // printf("select all %d\n", ret);
    // printf("'%s'\n", writer.write(blogs).c_str());
    //
    
    // 测试查找单个博客
    // ret = blog_table.SelectOne(1, &blog);
    // printf("select one %d\n", ret);
    // printf("'%s'\n", writer.write(blog).c_str());
    //
    
    // 测试修改博客
    // blog["blog_id"] = 1;
    // blog["title"] = "我的 Offers！";
    // blog["content"] = "100W\n 100W 100W 100W'''''!!!100000000W'''";
    // 
    // ret = blog_table.Update(blog);
    // printf("update %d\n", ret);
    // printf("'%s'\n", writer.write(blog).c_str());

    // 测试删除
    ret = blog_table.Delete(1);
    printf("Delete %d\n", ret);

    blog_system::MySQLRelease(mysql);
}

void TestTagTable()
{
    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::TagTable tag_table(mysql);
    bool ret = false;
    Json::StyledWriter writer;

    Json::Value tag;
    // 测试插入
    // tag["tag_name"] = "C语言";
    // ret = tag_table.Insert(tag);
    // printf("insert %d\n", ret);

    // 测试查找
    // Json::Value tags;
    // ret = tag_table.SelectAll(&tags);
    // printf("select all %d\n", ret);
    // printf("'%s'\n", writer.write(tags).c_str());

    // 测试删除
    ret = tag_table.Delete(1);
    printf("delete %d\n", ret);





    blog_system::MySQLRelease(mysql);
}
#endif 

#if 0
TEST(event, test)
{
    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::BlogTable blog_table(mysql);
    Json::StyledWriter writer;
    Json::Value blog;
    
    // 单元测试 gtest google 提供的一个单元测试框架


    // 测试插入
    blog["title"] = "My Carrer!";
    blog["content"] = "我要拿1000W年薪！";
    blog["tag_id"] = 1;
    blog["create_time"] = "2019/07/27";

    EXPECT_EQ(blog_table.Insert(blog), true);

    // 测试查找
    Json::Value blogs;
    EXPECT_EQ(blog_table.SelectAll(&blogs), true);
    
    
    // 测试查找单个博客
    EXPECT_EQ(blog_table.SelectOne(3, &blog), true);
    
    
    // 测试修改博客
    blog["blog_id"] = 3;
    blog["title"] = "我的 Offers！";
    blog["content"] = "100W\n 100W 100W 100W'''''!!!100000000W'''";
    
    EXPECT_EQ(blog_table.Update(blog), true);

    // 测试删除
    EXPECT_EQ(blog_table.Delete(3), true);

    blog_system::MySQLRelease(mysql);
}

#endif

TEST(test, tag_table)
{

    MYSQL* mysql = blog_system::MySQLInit();
    blog_system::TagTable tag_table(mysql);
    Json::StyledWriter writer;

    Json::Value tag;
    // 测试插入
    tag["tag_name"] = "C语言";
    EXPECT_TRUE(tag_table.Insert(tag));

    // 测试查找
    Json::Value tags;
    EXPECT_TRUE(tag_table.SelectAll(&tags));

    // 测试删除
    EXPECT_TRUE(tag_table.Delete(2));
}

int main()
{
    // TestBlogTbale();
    
    // TestTagTable();
    
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

