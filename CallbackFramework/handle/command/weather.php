<?php

boot::load('bin.cache');

$city = $client->message;

$cache = cache::read('weather_'.$city, 'h:12');

if ( !$cache )
{
	$query = iconv('UTF-8', 'GB2312//IGNORE', $city);
	$weather = file_get_contents('http://api.liqwei.com/weather/?city='.urlencode($query));
	$weather = str_replace('<br/>', "\n", $weather);
	$weather = iconv('GB2312', 'UTF-8//IGNORE', $weather);
	cache::write('weather_'.$city, $weather);
}
else
{
	$weather = $cache."\n(缓存读取)";
}

api::send($client, $weather);

?>