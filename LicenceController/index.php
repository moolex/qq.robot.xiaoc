<?php

$method = $_SERVER['REQUEST_METHOD'];
if ($method == 'GET')
{
	header('Location: about.html');
}
elseif ($method == 'POST')
{
	include 'licence.php';
}
else
{
	exit('FAILES');
}

?>