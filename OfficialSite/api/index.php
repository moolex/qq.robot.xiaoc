<?php
if ( !isset($_GET['class']) )
{
	header('Location: ?class=login');
	exit;
}
include 'data.tree.php';
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Language" content="zh-cn" />
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<title>QQRobot API Server</title>
	<link rel="stylesheet" type="text/css" href="/css/base.css" />
	<link rel="stylesheet" type="text/css" href="/css/api.css" />
</head>
<body>
<div id="header">
	<div id="logo">
		<a href="/">QQRobot API Server</a>
	</div>
	<div id="text">API文档</div>
	<div id="menu">
		<script type="text/javascript" src="/js/menus.js"></script>
	</div>
</div>
<div id="main">
<?php
echo '<div class="api_class_box">'."\n";
foreach ($api_class as $m_class_code => $m_class_name)
{
	echo '	<div class="api_class"><a href="?class='.$m_class_code.'">'.$m_class_name.' ['.$m_class_code.']</a></div>'."\n";
}
echo '</div>'."\n";
$class_code = $_GET['class'];
$api_help_of = $api_help[$class_code];
if (is_null($api_help_of))
{
	echo '没有此类API信息！';
}
else
{
	foreach ($api_help_of as $api_name => $api)
	{
		echo '<div class="api_box">'."\n";
		echo '	<ul class="api_list">'."\n";
		echo '		<li>'."\n";
		echo '			<div class="api_name">'.$api['name'].' - API [ <b>'.$class_code.'.'.$api_name.'</b> ]</div>'."\n";
		echo '			<div class="api_arg"><font class="api_arg_title">参数列表：</font>'."\n";
		if ( is_null($api['arg']) )
		{
			echo '			<i>(无)</i>'."\n";
		}
		else
		{
			foreach ($api['arg'] as $key => $val)
			{
				echo '				<div class="api_arg_name">'.$key.'</div><div class="api_arg_info">'.$val.'</div>'."\n";
			}
		}
		echo '			</div>'."\n";
		echo '			<div class="api_result"><font class="api_result_title">返回结果：</font>'."\n";
		if ( is_null($api['result']) )
		{
			echo '			<i>(无)</i>'."\n";
		}
		else
		{
			foreach ($api['result'] as $key => $val)
			{
				echo '				<div class="api_result_name">'.$key.'</div><div class="api_result_info">'.$val.'</div>'."\n";
			}
		}
		echo '			</div>'."\n";
		echo '			<div class="api_info">'.$api['info'].'</div>'."\n";
		echo '		</li>'."\n";
		echo '	</ul>'."\n";
		echo '</div>'."\n";
	}
}
?>
</div>
<div id="footer">
	(C) 2010 xiaoc.uuland.org
</div>
</body>
</html>