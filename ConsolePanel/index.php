<?php

require 'defines.php';
require 'selector.php';
require 'roler.php';

$call = select('GET:do');

$file = 'module/'.$call.'.php';

if (!file_exists($file))
{
	exit('Access Denied');
}

include $file;

?>