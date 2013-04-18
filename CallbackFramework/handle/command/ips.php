<?php

// 这个是一个贵宾账号才可以使用的功能
boot::load('bin.role');
role::init();

if ( !role::is($client->uid, ROLE_VIP) )
{
	api::send($client, '抱歉，此功能只对VIP以上用户开放');
	api::result('success');
}

api::send($client, '权限验证成功（功能测试）');

?>