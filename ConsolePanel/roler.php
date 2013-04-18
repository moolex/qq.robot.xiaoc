<?php

class role
{
	private static $__init_ok = false;
	private function __init()
	{
		if (!self::$__init_ok)
		{
			session_start();
			self::$__init_ok = true;
		}
	}
	public function needLogin()
	{
		self::__init();
		if (!isset($_SESSION['login']) || $_SESSION['login'] != LOGIN_FLAG)
		{
			//header('Location: '.LOGIN_URL.'?site='.$_SERVER['SERVER_NAME']);
			//exit;
		}
	}
	public function val($key, $data='')
	{
		self::__init();
		if ($data != '')
		{
			$_SESSION['USER_DATA'] = $data;
			return;
		}
		return $_SESSION['USER_DATA'][$key];
	}
}

?>