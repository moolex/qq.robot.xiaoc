<?php

$dbconfig = array
(
	'debug' => false,
	'host' => 'localhost:3306',
	'username' => 'root',
	'password' => 'moyo',
	'database' => 'qqrobot',
	'prefix' => '',
	'charset' => 'utf8',
	'cached' => 'file://{current}/cache/query/'
);

$licence_cache_time = 12;

$licence_hash_salt = 'f240a58b911fc5911a8619fc2798083b.(A).xiaoc@uuland.org.(C).{$time}.57b49186e7614a868276b80a4d813f78';
$licence_secure_key = '9f357651d400c931cafd1196e80f54b3';

date_default_timezone_set('Asia/Shanghai');

?>