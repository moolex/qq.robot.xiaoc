<?php

/*
	### 预定义角色 ###
	0 => 普通账号
	1 => 贵宾账号
	5 => 普通管理员
	6 => 超级管理员
	9 => 创始人账号
*/

define('ROLE_NOBODY', 0);
define('ROLE_VIP', 1);
define('ROLE_ADMIN', 5);
define('ROLE_SUPADMIN', 6);
define('ROLE_ROOT', 9);

// 本文件不建议从 conf::read 读取
return null;

?>