<?php

$api_class = array
(
	'login' => '登陆控制',
	'buddy' => '好友操作',
	'qun' => '群操作',
	'me' => '账号控制',
	'error' => '错误信息'
);

$api_help = array
(
	'login' => array
	(
		'check' => array
		(
			'name' => '登录状态检查',
			'arg' => null,
			'result' => array
			(
				'ALREADY LOGIN' => '已登录',
				'WAIT VERIFY' => '等待验证码输入',
				'NOT LOGIN' => '未登录'
			),
			'info' => '可以检查当前API服务器的账号登录状况'
		),
		'create' => array
		(
			'name' => '创建登录会话',
			'arg' => array
			(
				'uid' => '账号',
				'password' => '密码'
			),
			'result' => array
			(
				'ALREADY LOGIN' => '已登录',
				'NEED VERIFY' => '需要验证码',
				'NEED ACTIVATE' => '需要激活',
				'NETWORK ERROR' => '网络错误',
				'PASSWORD ERROR' => '密码错误',
				'UNKNOWN ERROR' => '未知错误',
				'LOGIN SUCCESS' => '登录成功'
			),
			'info' => '可以连接API服务器登录'
		),
		'verify' => array
		(
			'name' => '输入验证码',
			'arg' => array
			(
				'verify' => '验证码'
			),
			'result' => array
			(
				'NEED VERIFY' => '需要验证码',
				'NEED ACTIVATE' => '需要激活',
				'NETWORK ERROR' => '网络错误',
				'PASSWORD ERROR' => '密码错误',
				'UNKNOWN ERROR' => '未知错误',
				'VERIFY SUCCESS' => '验证成功',
				'DONT NEED' => '不需要验证码'
			),
			'info' => '验证码可以通过访问“http://API服务器地址:服务端口/v?账号”查看'
		),
		'destroy' => array
		(
			'name' => '注销登录',
			'arg' => null,
			'result' => array
			(
				'LOGOUT SUCCESS' => '注销成功',
				'NOT LOGIN' => '还未登录'
			),
			'info' => '可以注销账号，重新登录其他账号'
		)
	),
	'buddy' => array
	(
		'name' => array
		(
			'name' => '获取昵称',
			'arg' => array
			(
				'uid' => '账号'
			),
			'result' => array
			(
				'账号昵称' => '（包含可以获取自己的）'
			),
			'info' => '查询账户昵称'
		),
		'info' => array
		(
			'name' => '获取好友信息',
			'arg' => array
			(
				'uid' => '账号'
			),
			'result' => array
			(
				'信息文本' => ''
			),
			'info' => '返回文本没有格式化，请自行处理'
		),
		'list' => array
		(
			'name' => '获取好友列表',
			'arg' => array
			(
				'online' => '是否仅获取在线列表[yes]or[no]'
			),
			'result' => array
			(
				'列表文本' => ''
			),
			'info' => '返回文本没有格式化，请自行处理'
		),
		'send' => array
		(
			'name' => '给好友发送信息',
			'arg' => array
			(
				'uid' => '目标账号',
				'message' => '信息内容（最多3000字符）'
			),
			'result' => array
			(
				'SEND SUCCESS' => '发送成功',
				'SEND FAILED' => '发送失败'
			),
			'info' => '频繁的发送信息会造成机器人反应迟钝，请做好发送优化'
		)
	),
	'qun' => array
	(
		'name' => array
		(
			'name' => '获取群名称',
			'arg' => array
			(
				'gid' => '群号'
			),
			'result' => array
			(
				'群名称' => ''
			),
			'info' => '查询群名称'
		),
		'info' => array
		(
			'name' => '获取群信息',
			'arg' => array
			(
				'gid' => '群号'
			),
			'result' => array
			(
				'信息文本' => ''
			),
			'info' => '返回文本没有格式化，请自行处理'
		),
		'list' => array
		(
			'name' => '获取群列表',
			'arg' => null,
			'result' => array
			(
				'列表文本' => ''
			),
			'info' => '返回文本没有格式化，请自行处理'
		),
		'send' => array
		(
			'name' => '发送信息到群',
			'arg' => array
			(
				'gid' => '目标群号',
				'message' => '信息内容（最多3000字符）'
			),
			'result' => array
			(
				'SEND SUCCESS' => '发送成功',
				'SEND FAILED' => '发送失败'
			),
			'info' => '频繁的发送信息会造成机器人反应迟钝，请做好发送优化'
		),
		'buddy.name' => array
		(
			'name' => '获取群内成员昵称',
			'arg' => array
			(
				'gid' => '群号',
				'uid' => '成员账号'
			),
			'result' => array
			(
				'成员昵称' => ''
			),
			'info' => '查询群内某成员昵称'
		),
		'buddy.list' => array
		(
			'name' => '获取群内成员列表',
			'arg' => array
			(
				'gid' => '群号',
				'online' => '是否仅获取在线列表[yes]or[no]'
			),
			'result' => array
			(
				'列表文本' => ''
			),
			'info' => '返回文本没有格式化，请自行处理'
		)
	),
	'me' => array
	(
		'status.update' => array
		(
			'name' => '更新账号状态',
			'arg' => array
			(
				'status' => '支持以下几个值：online，hidden，killme，busy'
			),
			'result' => array
			(
				'UPDATE SUCCESS' => '更新成功'
			),
			'info' => '更新账号在线状态'
		),
		'list.update' => array
		(
			'name' => '更新列表',
			'arg' => null,
			'result' => array
			(
				'UPDATE SUCCESS' => '更新成功',
				'UPDATE FAILED' => '更新失败'
			),
			'info' => '此操作比较慢，服务端超时10秒处理，一般不用使用'
		),
		'buddy.add' => array
		(
			'name' => '添加好友',
			'arg' => array
			(
				'uid' => '目标账号',
				'message' => '验证信息'
			),
			'result' => array
			(
				'ADD SUCCESS' => '请求成功'
			),
			'info' => 'API服务器会将结果发送到Callback地址，事件为“buddy.operate”'
		),
		'buddy.del' => array
		(
			'name' => '删除好友',
			'arg' => array
			(
				'uid' => '目标账号'
			),
			'result' => array
			(
				'DELETE SUCCESS' => '删除成功'
			),
			'info' => '删除好友，操作不可逆，谨慎使用'
		),
		'sleep' => array
		(
			'name' => '启用休眠',
			'arg' => array
			(
				'power' => '开关：on 启用，off 关闭'
			),
			'result' => array
			(
				'SWITCH SUCCESS' => '切换成功'
			),
			'info' => '休眠状态下，停止一切信息转发，只接受管理员命令（管理员账号需在配置文件中设置）'
		),
		'autoreply' => array
		(
			'name' => '启用自动回复',
			'arg' => array
			(
				'power' => '开关：on 启用，off 关闭'
			),
			'result' => array
			(
				'SWITCH SUCCESS' => '切换成功'
			),
			'info' => '自动回复只对于私聊起作用（回复信息需在配置文件中设置）'
		)
	),
	'error' => array
	(
		'denied' => array
		(
			'name' => '未授权访问',
			'arg' => null,
			'result' => array
			(
				'ACCESS DENIED' => ''
			),
			'info' => '连接密钥出错，请检查你的密钥'
		),
		'unknown' => array
		(
			'name' => '未知API命令',
			'arg' => null,
			'result' => array
			(
				'UNKNOWN API COMMAND' => ''
			),
			'info' => '不能识别的API命令，请检查你的请求信息'
		)
	)
);

?>