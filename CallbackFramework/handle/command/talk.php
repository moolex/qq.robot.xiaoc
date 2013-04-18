<?php

boot::load('bin.talk');

$cmds = explode("\r", $client->message);

$do = $cmds[0];

api::send($client, '会话命令为：'.$do);

?>