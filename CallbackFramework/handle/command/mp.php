<?php

$result  = '手机号：'.$client->message."\n";
$mpinfo  = file_get_contents('http://api.showji.com/locating/?m='.$client->message.'&output=text');
$result .= '信息：'."\n".$mpinfo;

api::send($client, $result);

?>