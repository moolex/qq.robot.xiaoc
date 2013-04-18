<?php

class licence
{
	private static $__hash = '';
	private static $__seckey = '';
	public function HashSalt($hash)
	{
		self::$__hash = $hash;
	}
	public function Seckey($seckey)
	{
		self::$__seckey = $seckey;
	}
	public function checkRequest()
	{
		$process = isset($_POST['process']) ? $_POST['process'] : '';
		if ($process == '')
		{
			return;
		}
		if ('api_init_server_time' == $process)
		{
			echo self::CreateSign('SCK', self::$__seckey);
		}
		exit;
	}
	public function Verify($url)
	{
		if ($_SERVER['REQUEST_METHOD'] != 'POST')
		{
			return array('status'=>false,'msg'=>'HTTP Error.');
		}
		$time = $_POST['time'];
		$timeMovs = abs(time() - (int)$time);
		if ($timeMovs > 60)
		{
			return array('status'=>false,'msg'=>'Server Times out.');
		}
		$key = $_POST['key'];
		$string = str_replace('{$time}', $time, self::$__hash).'@URI~'.$url;
		$local_key = md5($string);
		if ($key != $local_key)
		{
			return array('status'=>false,'msg'=>'Transfer Key Error.');
		}
		return array('status'=>true);
	}
	public function CreateSign($name, $value)
	{
		$time = $_POST['time'] ? $_POST['time'] : time();
		$string = str_replace('{$time}', $time, self::$__hash).'@'.$name.'~'.$value;
		$key = md5($string);
		return '<time>'.$time.'</time><key>'.$key.'</key>';
	}
	public function makeTrue($msg)
	{
		echo '<status>true</status>';
		echo self::CreateSign('STS', 'true');
		echo '<msg>'.$msg.'</msg>';
		exit;
	}
	public function makeFalse($msg)
	{
		echo '<status>false</status>';
		echo '<msg>'.$msg.'</msg>';
		exit;
	}
}

?>