<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Language" content="zh-cn" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title>QQRobot API Server</title>
	<link rel="stylesheet" type="text/css" href="/css/base.css" />
</head>
<body>
<div id="header">
	<div id="logo">
		<a href="/">QQRobot API Server</a>
	</div>
	<div id="text">程序下载</div>
	<div id="menu">
		<script type="text/javascript" src="/js/menus.js"></script>
	</div>
</div>
<div id="main">
<?php

$code = 'moyo';

if (isset($_GET['code']) && $_GET['code'] == $code)
{
	echo '验证成功，开始下载';
	echo '<script type="text/javascript">window.location="QQAPIServer.zip";</script>';
}
else
{
	echo '授权码错误！您不能下载此测试版本';
}

?>
</div>
<div id="footer">
	(C) 2010 xiaoc.uuland.org
</div>
</body>
</html>