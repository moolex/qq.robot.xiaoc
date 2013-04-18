<?php

boot::load('driver.xiaoi');

$i = new XiaoI();

$result = $i->chat(trim($client->message));

$result = str_replace('\r', "\n", $result);
$result = str_replace('小i', '小C', $result);

api::send($client, $result);

?>