<?php

require_once 'load.php';

boot::load('bin.api');
boot::load('bin.conf');
boot::load('bin.input');
boot::load('bin.handle');
boot::load('bin.access');

$client = input::getInstance();
$client->init();

access::protect($client);

handle::client($client);

$type = handle::check();

if ( $type == 'unknown' )
{
	// 未知命令作为talk处理
	$type = 'talk';
}
handle::type($type);
handle::parse();
handle::run();

api::result('success');

?>