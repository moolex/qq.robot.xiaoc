<?php

require 'config.php';
require 'licence.driver.php';
require 'mysql.driver.php';
require 'licence.module.php';

licence::HashSalt($licence_hash_salt);
licence::Seckey($licence_secure_key);
licence::checkRequest();

$url = $_POST['url'];

$verify = licence::Verify($url);

if ( false == $verify['status'] )
{
	licence::makeFalse($verify['msg']);
}

$dbc = mysql_driver::getInstance();
$dbc->config($dbconfig);

list($protocol, $url) = explode('://', $url);
list($domain, $url) = explode('/', $url);

$root = root_domain($domain);
$ip = $_SERVER['REMOTE_ADDR'];

# domain:uuland.org
# url:callback.uuland.org/?uid=1

$search = array();
$search['by-domain'] = array(
	'callback' => 'domain:'.$root
);
$search['by-url'] = array(
	'callback' => 'url:'.$domain.'/'.$url
);
$search['by-ip'] = array(
	'callback' => 'ip:'.$ip
);

$result = licenceQuery($search);
$isPassed = $result['status'];
$data = $result['data'];

if ($isPassed)
{
	
	$msg = 'User: '.$data['owner'].', Expires Date: '.date('Y-m-d H:i:s', $data['overtime']);
	licence::makeTrue($msg);
}
else
{
	$msg = $data['msg'];
	licence::makeFalse($msg);
}

?>