# WeBlog

## 需求：

实现一个简单的博客系统。

1. 只支持单个用户。
2. 实现针对文章的 CURD 。
3. 实现针对标签的 CURD (分类)。

## 模块划分

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190725145020723.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

### 网页端的具体展现形式

1. 展现博客列表页面
2. 展现博客详情页面
3. 管理博客页面

当用户在网页上执行某个操作的时候，比如新增博客，此时就会由客户端给服务器发送 HTTP 请求， 请求中就包含了用户行为，服务器再根据这个行为来完成对数据的操作(对数据库的操作)。

## 设计思路

1. 先设计数据库(表结构)。
2. 设计前后端交互接口。
3. 实现服务器端和客户端的逻辑。

## 数据库设计 -- MySQL 关系型数据库

1. 创建一个数据库
2. 创建一个博客表
   - blog_id int primary key
   - title varchar(50)
   - content text
   - create_time varchar(50)
   - tag_id int comment '博客属于的标签'
3. 创建一个标签表
   - tag_id int 
   - tag_name varchar(50)

#### 使用 C 语言 MySQL API 来完成数据库操作

> MySQL 的官方文档

- ##### 头文件 #include <mysql/mysql.h>

- ##### MYSQL *mysql_init(MYSQL *mysql)

  - 注意这里参数如果填空，就返回一个句柄。空需要 NULL 不能写成 nullptr。

  - ##### Description

    Allocates or initializes a `MYSQL` object suitable for [`mysql_real_connect()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-real-connect.html). If `mysql` is a `NULL` pointer, the function allocates, initializes, and returns a new object. Otherwise, the object is initialized and the address of the object is returned. If [`mysql_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-init.html) allocates a new object, it is freed when [`mysql_close()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-close.html) is called to close the connection.

    In a nonmultithreaded environment, [`mysql_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-init.html) invokes [`mysql_library_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-library-init.html) automatically as necessary. However,[`mysql_library_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-library-init.html) is not thread-safe in a multithreaded environment, and thus neither is [`mysql_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-init.html). Before calling [`mysql_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-init.html), either call [`mysql_library_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-library-init.html) prior to spawning any threads, or use a mutex to protect the [`mysql_library_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-library-init.html) call. This should be done prior to any other client library call.

    ##### Return Values

    An initialized `MYSQL*` handler. `NULL` if there was insufficient memory to allocate a new object.

    ##### Errors

    In case of insufficient memory, `NULL` is returned.

- ##### MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)      (8 个参数！！！！)

  - **参数**

  - mysql: 刚才创建的句柄

  - host：要连接的服务器 ip

  - user：数据库用户名

  - passwd：数据库对应用户名的密码

  - db：database 名

  - port：数据库服务器端口名，默认 3306

  - unix_socket：NULL

  - client_flag：0

  - **返回值**

  - 如果成功返回跟刚才一样的句柄，如果失败返回 NULL

  - **一些错误码**

  - ##### Errors

    - [`CR_CONN_HOST_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_conn_host_error)

      Failed to connect to the MySQL server.

    - [`CR_CONNECTION_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_connection_error)

      Failed to connect to the local MySQL server.

    - [`CR_IPSOCK_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_ipsock_error)

      Failed to create an IP socket.

    - [`CR_OUT_OF_MEMORY`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_out_of_memory)

      Out of memory.

    - [`CR_SOCKET_CREATE_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_socket_create_error)

      Failed to create a Unix socket.

    - [`CR_UNKNOWN_HOST`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_unknown_host)

      Failed to find the IP address for the host name.

    - [`CR_VERSION_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_version_error)

      A protocol mismatch resulted from attempting to connect to a server with a client library that uses a different protocol version.

    - [`CR_NAMEDPIPEOPEN_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_namedpipeopen_error)

      Failed to create a named pipe on Windows.

    - [`CR_NAMEDPIPEWAIT_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_namedpipewait_error)

      Failed to wait for a named pipe on Windows.

    - [`CR_NAMEDPIPESETSTATE_ERROR`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_namedpipesetstate_error)

      Failed to get a pipe handler on Windows.

    - [`CR_SERVER_LOST`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_server_lost)

      If [`connect_timeout`](https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_connect_timeout) > 0 and it took longer than [`connect_timeout`](https://dev.mysql.com/doc/refman/8.0/en/server-system-variables.html#sysvar_connect_timeout) seconds to connect to the server or if the server died while executing the `init-command`.

    - [`CR_ALREADY_CONNECTED`](https://dev.mysql.com/doc/refman/8.0/en/client-error-reference.html#error_cr_already_connected)

      The `MYSQL` connection handler is already connected.

- ##### int mysql_set_character_set(MYSQL *mysql, const char *csname)

  - 设置字符集

  - ##### Description

    This function is used to set the default character set for the current connection. The string `csname` specifies a valid character set name. The connection collation becomes the default collation of the character set. This function works like the [`SET NAMES`](https://dev.mysql.com/doc/refman/8.0/en/set-names.html) statement, but also sets the value of `mysql->charset`, and thus affects the character set used by [`mysql_real_escape_string()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-real-escape-string.html)

    ##### Return Values

    Zero for success. Nonzero if an error occurred.

  - 例如： mysql_set_character_set(connect_fd, "utf8"); 

    - 设置默认字符集为 utf8

- ##### int mysql_query(MYSQL *mysql, const char *stmt_str)

  - 执行一个 sql 语句
  - 这里主要使用拼接 sql 语句的方式写入到 stmt_str，然后在把 stmt_str 传进去

- ##### 遍历查询结果

  - MYSQL_RES *mysql_store_result(MYSQL *mysql)

    将刚才 mysql 句柄查询的结果放进 MYSQL_RES 这个结构体中。

  - my_ulonglong mysql_num_rows(MYSQL_RES *result)

    获取结果集合的行数

  - unsigned int mysql_num_fields(MYSQL_RES *result)

    获取结果集合的列数

  - MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)

    一次获取一行数据

  - mysql_free_result()

    释放查询结果集合

- ##### void mysql_close(MYSQL *mysql)

  - 将刚才连接关闭，因为在某种情况下，就算程序退出，连接也不一定会一定关闭，所以一定要手动关闭。

  - ##### Description

    Closes a previously opened connection. [`mysql_close()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-close.html) also deallocates the connection handler pointed to by `mysql` if the handler was allocated automatically by [`mysql_init()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-init.html) or [`mysql_connect()`](https://dev.mysql.com/doc/refman/8.0/en/mysql-connect.html). Do not use the handler after it has been closed.
  
- ##### unsigned long mysql_real_escape_string(MYSQL *mysql, char *to, const char from, unsigned long length)

  - 将正文进行转义，防止正文出现特殊符号从而导致 SQL 语句出错
  - mysql 句柄
  - to 是动态开辟的一个空间，是 from.size() * 2 + 1
  - from 是正文地址
  - length 是 from.size() 的大小



## 设计前后端的交互 API

基于 HTTP 协议进行扩展，创建自定制协议。

### 一、博客管理

1. 传统的 API 设计方式：使用 query_string 来进行传递信息
2. **restful 风格的 API 设计方式**：**使用不用的 HTTP 方法来表达不同的语义**
3. **使用 path 来表示要操作的资源**
4. **使用 JSON 来组织 body 中的数据**

1. #### 新增博客

   

   使用 POST 方法表示新增

   例如：

   POST /blog

   {

   ​	title: xxxx,

   ​	content: xxxx,

   ​	create_time: xxxx,

   ​	tag_id:	xxxx

   } 

   

   HTTP/1.1 200 OK

   {

   ​	ok: true,

   ​	reason: "xxxx"

   }

2. #### 获取博客列表

   查看所有博客（标题列表）

   使用 GET 方法表示查看

   GET /blog                  	获取所有

   GET /blog?tag_id=1 	按照标签来筛选

   

   HTTP/1.1 200 OK

   [

   ​	{

   ​		blog_id: 1,

   ​		title: "My Carrer",

   ​		create_time: "2019/07/27",

   ​		tag_id: 1	

   ​	},

   ​	{

   ​		...

   ​	}

   ]

3. #### 获取某个博客的详细内容

   查看某个博客

   GET /blog/:blog_id     // :blog_id 会将 blog_id 替换成真正的 id； 类似 /blog/1

   

   HTTP/1.1 200 OK

   {

   ​	"blog_id": 1,

   ​	"title": "My Carrer",

   ​	"content": "博客正文",

   ​	"create_time": "2019/07/27",

   ​	"tag_id": 1	

   }

4. #### 修改博客

   使用 PUT 方法表示修改

   PUT /blog/:blog_id

   {

   ​	title: "修改之后的标题",

   ​	content: "修改之后的正文",

   ​	tag_id: "修改之后的 tag_id"

   }

   

   HTTP/1.1 200 OK

   {

   ​	ok: true

   }

5. #### 删除博客

   使用 DELETE 表示删除

   DELETE /blog/:blog_id

   

   HTTP/1.1 200 OK

   {

   ​	ok: true

   }

### 二、标签管理

1. #### 新增标签

   POST /tag

   {

   ​	"tag_name": "新增的标签名",

   }

   

   HTTP/1.1 200 OK

   {

   ​	ok: true

   }

2. #### 删除标签

   DELETE /tag/:tag_id

   

   HTTP/1.1 200 OK

   {

   ​	ok: true

   }

3. #### 查看所有标签

   GET /tag

   

   HTTP/1.1 200 OK

   [

   ​	{

   ​		tag_id: 1,

   ​		tag_name: "c++"

   ​	},

   ​	{},

   ​	{}

   ]

## 编码

### 1. 数据库部分

Json 作为函数参数是非常常见的

一个典型场景：跨语言调用。不同的语言对于类型的约定是不一样的，但是字符串大家都有，所以 Json 对于跨语言调用还是很方便的。



通过 sprintf 的方式来拼接 SQL 语句

'%s', Connect 万一包含了单引号咋办？

借助：mysql_real_escape_string 进行一个正文内容的转义。

**小操作：**

将 string 类型转换为整数：stoi(string)

将 C 风格字符串转换为整数：atoi(char*)

将数字类型转换为 string 类型：to_string(数字类型)

### 2. HTTP 服务器

基于 TCP 服务器，在 HTTP 协议格式的基础上来完成字符串的解析和拼装。

cpp-httplib 

#### 正则表达式

用来筛选字符串，用特殊符号规定字符串有一些特殊的特征。

用特殊符号来描述一个字符串应该具有哪些特殊特征，特征包括不限于：包含哪些特定字符串，以什么开头 ，以什么结尾，特定字符重复出现几次......



#### Postman HTTP 测试工具

#### curl 命令行版本的 HTTP 客户端 

### 3. 客户端部分