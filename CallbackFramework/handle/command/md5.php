<?php

$result  = '输入内容：'.$client->message."\n";
$result .= 'MD5值为：'.md5($client->message);

api::send($client, $result);

?>