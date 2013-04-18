<?php

// Root
	define(DIR_ROOT, str_replace('\\', '/', dirname(__FILE__)).'/');

// Cache
	define(DIR_CACHE, DIR_ROOT.'cache/');
// {
	define(DIR_CACHE_QUERY, DIR_CACHE.'query/');
	define(DIR_CACHE_ACCESS, DIR_CACHE.'access/');
	define(DIR_CACHE_DATA, DIR_CACHE.'data/');
	define(DIR_CACHE_REQUIRE, DIR_CACHE.'require/');
// }

// Admin
	define(DIR_ADM, DIR_ROOT.'adm/');

// Bin
	define(DIR_BIN, DIR_ROOT.'bin/');

// Data
	define(DIR_DATA, DIR_ROOT.'data/');

// Driver
	define(DIR_DRIVER, DIR_ROOT.'driver/');

// Conf
	define(DIR_CONF, DIR_ROOT.'etc/');

// Handle
	define(DIR_HANDLE, DIR_ROOT.'handle/');

// Load base file
	require_once DIR_BIN.'@boot.php';

?>