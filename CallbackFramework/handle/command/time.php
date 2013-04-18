<?php

$result  = '当前时间：'.date('Y-m-d H:i:s')."\n".'转换时戳：'.time();

api::send($client, $result);

?>