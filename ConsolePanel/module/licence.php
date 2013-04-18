<?php

role::needLogin();

require 'config.php';
require 'mysql.driver.php';

// TEST
role::val('w', array('uid'=>1));

$dbc = mysql_driver::getInstance();
$dbc->config($dbconfig);

$op = isset($_GET['op']) ? $_GET['op'] : 'list';

$uid = role::val('uid');

switch ($op)
{
	case 'list'		: readList();break;
	//case 'save'		: saveData();break;
	case 'save'		: echo 'error';break;
	//case 'del'		: delData();break;
}

exit;

function readList()
{
	global $dbc, $uid;
	$result = $dbc->select('licence')->where('owner='.$uid)->done();
	if (!$result)
	{
		$ops = array('status'=>'fails');
	}
	else
	{
		$ops = array('status'=>'ok');
		foreach ($result as $i => $one)
		{
			list($type, $callback) = explode(':', $one['callback']);
			$ops['data'][] = array(
				'id' => $one['i'],
				'type' => __get_bind_type($type),
				'callback' => $callback,
				'overtime' =>__get_over_time($one['overtime'])
			);
		}
	}
	echo json_encode($ops);
}

function saveData()
{
	global $dbc, $uid;
	$newCallback = __remove_si($_POST['callback']);
	$id = __remove_si($_POST['id']);
	$id=1;
	$where = array('i'=>$id,'owner'=>$uid);
	// get type
	$result = $dbc->select('licence')->where($where)->done();
	if (!$result)
	{
		__ops_error('没有找到此授权！');
		return;
	}
	$oldCallback = $result[0]['callback'];
	list($type, $callback) = explode(':', $oldCallback);
	$newCallback = $type.':'.$newCallback;
	$data = array('sid'=>md5($newCallback),'callback'=>$newCallback);
	$result = $dbc->update('licence')->data($data)->where($where)->done();
	if ($result < 1)
	{
		__ops_error('保存失败！');
		return;
	}
	__ops_success();
}

function __ops_success()
{
	$ops = array('status'=>'ok');
	echo json_encode($ops);
}

function __ops_error($msg)
{
	$ops = array('status'=>'fails');
	$ops['msg'] = $msg;
	echo json_encode($ops);
}

function __remove_si($string)
{
	return str_replace(array('\'', '"'), '', $string);
}

function __get_bind_type($flag)
{
	$unit =  array(
		'url' => '绑定URL',
		'domain' => '绑定域名',
		'ip' => '绑定IP地址'
	);
	return $unit[$flag];
}

function __get_over_time($time)
{
	$time -= time();
	$unit = array(
		array(
			'name'=> '年',
			'long' => 31536000
		),
		array(
			'name'=> '月',
			'long' => 2592000
		),
		array(
			'name'=> '天',
			'long' => 86400
		),
		array(
			'name'=> '小时',
			'long' => 3600
		),
		array(
			'name'=> '分',
			'long' => 60
		),
		array(
			'name'=> '秒',
			'long' => 1
		),
	);
	$return = '';
	foreach ($unit as $i => $one)
	{
		if ($time > $one['long'])
		{
			$num = floor($time/$one['long']);
			$time %= $one['long'];
			$return .= $num.''.$one['name'];
		}
	}
	return $return;
}

?>