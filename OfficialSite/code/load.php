<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Language" content="zh-cn" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title>QQRobot API Server</title>
	<link rel="stylesheet" type="text/css" href="/css/base.css" />
	<link rel="stylesheet" type="text/css" href="/css/index.css" />
</head>
<body>
<div id="header">
	<div id="logo">
		<a href="/">QQRobot API Server</a>
	</div>
	<div id="text">代码浏览</div>
	<div id="menu">
		<script type="text/javascript" src="/js/menus.js"></script>
	</div>
</div>
<div id="main">
<b>提示：所有的实例都必须以UTF8编码格式保存</b>
<br/><br/>
<hr/>
<br/>
<?php
if (isset($_GET['file']) && file_exists('file/'.$_GET['file'].'.code'))
{
	highlight_file('file/'.$_GET['file'].'.code');
}
else
{
	echo '0 0 ! Hello, girl, what do you finding?';
}
?>
</div>
<div id="footer">
	(C) 2010 xiaoc.uuland.org
</div>
</body>
</html>