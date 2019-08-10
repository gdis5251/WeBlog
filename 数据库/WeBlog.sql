-- 创建数据库
create database weblog;
use weblog;

-- 创建博客表
-- 在分布式系统下生成唯一主键
--		1. 使用时间戳
-- 	2. 使用机房 id
-- 	3. 使用主机 ip
-- 	4. 使用随机数
-- 为了效率，牺牲了数据的强一致性
-- 若为了强一致性，就得加锁，但是加锁就没有了效率
-- 效率 VS 一致性 VS 容错  是相互制约的
create table if not exists blog_table(
	blog_id int primary key auto_increment comment '博客id',
	title varchar(50) comment '博客标题',
	content text comment '博客正文',
	tag_id int comment '博客分类 id',
	create_time varchar(50) comment '创建时间'
);

-- 创建标签表
-- 不写外键是因为效率的问题
create table if not exists tag_table(
	tag_id int primary key auto_increment comment '博客分类 id',
	tag_name varchar(50) comment '标签名字'
);



-- 创建用户表
create table if not exists user_info(
	user_id int primary key auto_increment comment '用户id',
	user_name varchar(32) unique comment '用户名',
	user_password varchar(32) comment '用户密码'
	);
















