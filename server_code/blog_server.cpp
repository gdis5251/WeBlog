#include <iostream>
#include <signal.h>
#include <string>
#include "httplib.h"
#include "db.hpp"

MYSQL* mysql = NULL;

int main()
{
    using namespace httplib;
    using namespace blog_system;
    // 1. 先和数据库建立好连接
    mysql = blog_system::MySQLInit();

    // 如果手动释放 mysql 句柄有可能会遗忘造成内存泄漏
    // 所以使用信号捕捉，因为我本人退出服务端程序都是使用 ctrl + c
    // 所以 捕捉 SIGINT 信号，捕捉到后先释放 mysql 句柄， 在退出程序
    signal(SIGINT, [](int signo){
           (void) signo;
           blog_system::MySQLRelease(mysql);
           exit(EXIT_FAILURE);
           });

    // 2. 创建一个相关数据库处理对象
    BlogTable blog_table(mysql);
    TagTable tag_table(mysql);
    UserInfo user_info(mysql);

    // 3. 创建服务器, 并设置“路由”（HTTP 中的路由，跟IP中的路由不一样，
    // 此处的路由指的是把 方法 + path => 哪个函数关联关系声明清楚）
    Server server;


    // 新增博客
    server.Post("/blog", [&blog_table](const Request& req, Response& resp){
                printf("新增博客！\n");
                // 1. 获取到请求中的 body，并解析成 Json
                Json::Reader reader;
                Json::FastWriter writer;
                Json::Value req_json;
                Json::Value resp_json;

                bool ret = reader.parse(req.body, req_json);
                if (!ret)
                {
                    // 解析出错，给用户提示
                    printf("解析请求失败！ %s\n", req.body.c_str());

                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "Parse error!";
                    
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                // 2.对参数进行校验 
                if (req_json["title"].empty() || 
                    req_json["content"].empty() ||
                    req_json["tag_id"].empty() ||
                    req_json["create_time"].empty())
                {
                    printf("请求数据格式有错！ %s\n", req.body.c_str());
                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "Input data format error!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                // 3. 调用 MySQL  的接口来操作
                ret = blog_table.Insert(req_json);
                if (!ret)
                {
                    printf("博客插入失败！\n");
                    resp_json["ok"] = false;
                    resp_json["reason"] = "blog insert failed!";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                // 4. 构造一个正确的响应给客户
                printf("博客插入成功！\n");
                resp_json["ok"] = true;
                resp.set_content(writer.write(resp_json), "application/json");

                return;
    });

    // 查看所有博客, 列表
    server.Get("/blog", [&blog_table](const Request& req, Response& resp){
               printf("查看所有博客！\n");

               // 1. 尝试获取 tag_id, 如果 tag_id 不存在，返回空字符串
               const std::string& tag_id = req.get_param_value("tag_id");

               // 2. 调用数据库操作
               Json::Value resp_json;
               Json::FastWriter writer;
               bool ret = blog_table.SelectAll(&resp_json, tag_id);
               if(!ret)
               {
                    resp_json["ok"] = false;
                    resp_json["reason"] = "select all failed";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
               }

               // 3. 构造响应结果
               resp.set_content(writer.write(resp_json), "application/json");
    });


    // 查看某个博客
    server.Get(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp){
               // 1. 解析获取到 blog_id
               // [1] 是正则表达式的部分
               int32_t blog_id = std::stoi(req.matches[1].str());
               printf("查看 id 为 %d 的博客 \n", blog_id);

               // 2. 调用数据库操作
               Json::Value resp_json;
               Json::FastWriter writer;
               bool ret = blog_table.SelectOne(blog_id, &resp_json);
               if (!ret)
               {
                    resp_json["ok"] = false;
                    resp_json["reason"] = "查看指定博客失败！";
                    resp.status = 404;
                    resp.set_content(writer.write(resp_json), "application/json");
               }

               // 3. 构造响应给客户端
               resp.set_content(writer.write(resp_json), "application/json");
               });

    // 修改博客
    server.Put(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp) {
               // 1. 先获取到博客 id
               int32_t blog_id = std::stoi(req.matches[1].str());
               printf("修改 id 为 %d 的博客\n", blog_id);

               // 2. 获取到请求并解析结果
               Json::Reader reader;
               Json::FastWriter writer;
               Json::Value req_json;
               Json::Value resp_json;

               bool ret = reader.parse(req.body, req_json);
               if (!ret)
               {
                    // 解析出错，给用户提示
                    printf("解析请求失败！ %s\n", req.body.c_str());
                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "update blog parse request failed!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
               }

               // 3.对参数进行校验 
               if (req_json["title"].empty() || 
                   req_json["content"].empty() ||
                   req_json["tag_id"].empty())
               {
                   printf("请求数据格式有错！ %s\n", req.body.c_str());
                   // 构造一个响应对象，告诉客户端出错
                   resp_json["ok"] = false;
                   resp_json["reason"] = "Update data format error!";
                   resp.status = 400;
                   resp.set_content(writer.write(resp_json), "application/json");

                   return;

               }

               // 4. 调用 MySQL  的接口来操作
               // 从 parse 中得到的 id 设置到 req_json
               req_json["blog_id"] = blog_id;
               ret = blog_table.Update(req_json);
               if (!ret)
               {
                   printf("博客更新失败！\n");
                   resp_json["ok"] = false;
                   resp_json["reason"] = "blog update failed!";
                   resp.status = 500;
                   resp.set_content(writer.write(resp_json), "application/json");

                   return;
               }

               // 5. 构造一个正确的响应给客户
               printf("博客更新成功！\n");
               resp_json["ok"] = true;
               resp.set_content(writer.write(resp_json), "application/json");

               return;
    });

    // 删除博客
    server.Delete(R"(/blog/(\d+))", [&blog_table](const Request& req, Response& resp) {
                  // 1. 先获取到 blog_id
                  int32_t blog_id = std::stoi(req.matches[1].str());
                  printf("删除 id 为 %d 的博客\n", blog_id);

                  Json::Value resp_json;
                  Json::FastWriter writer;
                  bool ret = blog_table.Delete(blog_id);
                  if (!ret)
                  {
                        // 解析出错，给用户提示
                        printf("解析请求失败！ %s\n", req.body.c_str());
                        // 构造一个响应对象，告诉客户端出错
                        resp_json["ok"] = false;
                        resp_json["reason"] = "delete blog parse request failed!";
                        resp.status = 400;
                        resp.set_content(writer.write(resp_json), "application/json");

                        return;
                  }

                  printf("博客删除成功！\n");
                  resp_json["ok"] = true;
                  resp.set_content(writer.write(resp_json), "application/json");

                  return;
    });

    // 新增标签
    server.Post("/tag", [&tag_table](const Request& req, Response& resp) {
                printf("新增标签！\n");
                Json::FastWriter writer;
                Json::Reader reader;
                Json::Value req_json;
                Json::Value resp_json;

                bool ret = reader.parse(req.body, req_json);
                if (!ret)
                {
                    // 解析出错，给用户提示
                    printf("插入标签失败！ %s\n", req.body.c_str());
                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "insert tag parse request failed!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                if (req_json["tag_name"].empty())
                {
                    // 解析出错，给用户提示
                    printf("插入标签失败！ %s\n", req.body.c_str());
                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "insert tag format failed!";
                    resp.status = 400;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                ret = tag_table.Insert(req_json);
                if (!ret)
                {
                    // 解析出错，给用户提示
                    printf("插入标签失败！\n");
                    // 构造一个响应对象，告诉客户端出错
                    resp_json["ok"] = false;
                    resp_json["reason"] = "database failed!";
                    resp.status = 500;
                    resp.set_content(writer.write(resp_json), "application/json");

                    return;
                }

                resp_json["ok"] = true;
                resp.set_content(writer.write(resp_json), "application/json");

    });


    // 删除标签
    server.Delete(R"(/tag/(\d+))", [&tag_table](const Request& req, Response& resp) {
                  Json::Value resp_json; 
                  Json::FastWriter writer; 
                  // 1. 解析出 tag_id 
                  int tag_id = std::stoi(req.matches[1].str()); 
                  printf("删除 id 为 %d 的标签\n", tag_id);
                  bool ret = tag_table.Delete(tag_id); 
                  if (!ret)
                  {
                        resp_json["ok"] = false; 
                        resp_json["reason"] = "TagTable Delete failed!\n"; 
                        resp.status = 500; 
                        resp.set_content(writer.write(resp_json), "application/json"); 
                        return;  
                  }
                  // 3. 包装正确的结果 
                  resp_json["ok"] = true; 
                  resp.set_content(writer.write(resp_json), "application/json"); 
                  return;

                  });

    // 查看所有标签
    server.Get("/tag", [&tag_table](const Request& req, Response& resp){
               printf("查看所有标签!\n");
               (void) req; 
               Json::Reader reader; 
               Json::FastWriter writer; 
               Json::Value resp_json; 
               // 1. 调用数据库接口查询数据
               Json::Value tags; 
               bool ret = tag_table.SelectAll(&tags); 
               if (!ret)
               { 
                    resp_json["ok"] = false; 
                    resp_json["reason"] = "SelectAll failed!\n"; 
                    resp.status = 500; 
                    resp.set_content(writer.write(resp_json), "application/json"); 
                    return;  
               }
               // 2. 构造响应结果 
               resp.set_content(writer.write(tags), "application/json"); 
               return;
               });
    
    // 登录
    server.Post("/login", [&user_info](const Request& req, Response& resp) {
        printf("用户登录！\n");

        Json::FastWriter writer;
        Json::Reader reader;
        Json::Value req_json;
        Json::Value resp_json;

        bool ret = reader.parse(req.body, req_json);
        if (!ret)
        {
            printf("解析出错！ '%s'\n", req.body.c_str());

            // 构造一个响应对象给客户端
            resp_json["ok"] = false;
            resp_json["reason"] = "login parse request failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }

        if (req_json["user_name"].empty())
        {
            // 解析出错，给用户提示
            printf("查找用户失败！ %s\n", req.body.c_str());
            // 构造一个响应对象，告诉客户端出错
            resp_json["ok"] = false;
            resp_json["reason"] = "search user format failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }

        ret = user_info.Check(req_json);
        if (!ret)
        {
            printf("密码不匹配！\n");

            resp_json["ok"] = false;
            resp_json["reason"] = "password is wrong!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }

        resp_json["ok"] = true;
        resp.set_content(writer.write(resp_json), "application/json");


    });

    server.Post("/sign_in", [&user_info](const Request& req, Response& resp){
        printf("注册用户！\n");

        Json::FastWriter writer;
        Json::Reader reader;
        Json::Value req_json;
        Json::Value resp_json;

        bool ret = reader.parse(req.body, req_json);
        if (!ret)
        {
            printf("解析出错！ '%s'\n", req.body.c_str());

            // 构造一个响应对象给客户端
            resp_json["ok"] = false;
            resp_json["reason"] = "login parse request failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }

        if (req_json["user_name"].empty())
        {
            // 解析出错，给用户提示
            printf("插入用户失败！ %s\n", req.body.c_str());
            // 构造一个响应对象，告诉客户端出错
            resp_json["ok"] = false;
            resp_json["reason"] = "search user format failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }
        
        ret = user_info.Insert(req_json);
        if (!ret)
        {
            printf("插入用户失败！\n");

            resp_json["ok"] = false;
            resp_json["reason"] = "insert user failed!";
            resp.status = 400;
            resp.set_content(writer.write(resp_json), "application/json");

            return;
        }

        resp_json["ok"] = true;
        resp.set_content(writer.write(resp_json), "application/json");

    });



    server.set_base_dir("./wwwroot");
    printf("开始监听\n");
    server.listen("0.0.0.0", 9090);

    return 0;
}

