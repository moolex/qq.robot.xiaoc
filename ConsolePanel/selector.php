<?php

function select($pox)
{
	list($method, $key) = explode(':', $pox);
	$call = 'select_of_'.$method;
	return $call($key);
}

function select_of_GET($key)
{
	$action = isset($_GET[$key]) ? $_GET[$key] : '';
	switch ($action)
	{
		case 'login'	: return 'login';
		case ''			: return 'default';
		case 'licence'	: return 'licence';
		default			: return 'error';
	}
}

?>