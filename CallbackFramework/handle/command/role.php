<?php

boot::load('bin.role');
role::init();

if ( !role::is($client->uid, ROLE_ROOT) )
{
	api::send($client, '抱歉，此功能只对创始人开放');
	api::result('success');
}

$explodes = explode('=', $client->message);

$who = $explodes[0];
$role = $explodes[1];

switch ($role)
{
	case 'nobody':
		$level = ROLE_NOBODY;
	break;
	case 'vip':
		$level = ROLE_VIP;
	break;
	case 'admin':
		$level = ROLE_ADMIN;
	break;
	case 'supadmin':
		$level = ROLE_SUPADMIN;
	break;
	case 'root':
		$level = ROLE_ROOT;
	break;
}

role::set($who, $level);

api::send($client, '权限设置已完成');

?>