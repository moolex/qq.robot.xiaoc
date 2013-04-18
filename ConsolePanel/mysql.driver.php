<?php

class mysql_driver
{
	// 默认配置
	private $_config_default = array
	(
		'debug' => false,
		'host' => 'localhost:3306',
		'username' => 'root',
		'password' => '',
		'database' => 'mysql',
		'prefix' => '',
		'charset' => 'utf-8',
		'cached' => 'file://{root}/query_cache/'
	);
	public  $CACHE_HASH_SALT = 'sql.cache.uuland.org';
	public  $CLIENT_MULTI_RESULTS = 131072;
	// 配置信息
	private $_config = array();
	private $_debug = false;
	private $_host = '';
	private $_username = '';
	private $_password = '';
	private $_database = '';
	private $_prefix = '';
	private $_charset = '';
	private $_cached = '';
	private $_fc_path = '';
	private $_mc_server = '';
	// 运行时变量
	private $_dbc_handle = null;
	private $_query_handle = null;
	public  $sql = '';
	private $_cache_key = '';
	private $_result = array();
	private $_need_cache = false;
	// 数据库操作
	private $_operate = '';
	private $_column = '';
	private $_field = '';
	private $_where = array();
	private $_order = array();
	private $_limit = '';
	private $_data = array();
	private $_cache = '';
	// 调试记录
	private $_trace = array();
	
	private static $__has_instance = false;
	// 获取实例
	public function getInstance()
	{
		if ( self::$__has_instance )
		{
			return self;
		}
		else
		{
			self::$__has_instance = true;
			return new self();
		}
	}
	// 私有化构造函数，禁止外部实例化
	private function __construct(){}
	// 卸载实例时，自动释放资源，关闭连接
	public function __destruct()
	{
		// 释放资源
		$this->free();
		// 关闭连接
		$this->close();
	}
	// 载入配置
	public function config($config)
	{
		$this->trace('public::config::load');
		$this->_config = $config;
		// 执行初始化
		$this->init();
	}
	// 初始化
	private function init()
	{
		$this->trace('private::config::init_default');
		// 配置分析
		foreach ($this->_config as $key => $val)
		{
			$mkey = '_'.$key;
			$this->$mkey = isset($this->_config[$key]) ? $this->_config[$key] : $this->_config_default[$key];
		}
		// 清理非debug模式下，之前记录的调试信息
		if (!$this->_debug) unset($this->_trace);
		// 缓存配置
		$this->trace('private::config::init_cache');
		$cache_conf = explode('://', $this->_cached);
		$this->_cached = $cache_conf[0];
		if ($this->_cached == 'file')
		{
			$this->_fc_path = str_replace('{current}', dirname(__FILE__), str_replace('{root}', $_SERVER['DOCUMENT_ROOT'], $cache_conf[1]));
			// 检测目录
			if (!is_dir($this->_fc_path))
			{
				mkdir($this->_fc_path);
			}
		}
		elseif ($this->_cached == 'memcache')
		{
			$this->_mc_server = $cache_conf[1];
		}
		unset($this->_config);
	}
	// 连接至数据库
	private function connect()
	{
		$this->trace('public::server::connect');
		// 连接服务器
		$this->_dbc_handle = mysql_connect(
			$this->_host,
			$this->_username,
			$this->_password,
			true,
			$this->CLIENT_MULTI_RESULTS
		);
		if (!$this->_dbc_handle)
		{
			$this->alert('Can\'t connect to Server [ '.$this->_username.'@'.$this->_host.' ]');
			return false;
		}
		// 选择数据库
		if (!mysql_select_db($this->_database, $this->_dbc_handle))
		{
			$this->alert('Can\'t select database ['.$this->_database.']');
			return false;
		}
		$version = mysql_get_server_info($this->_dbc_handle);
		// 设置数据库编码
		if ($version >= '4.1')
		{
			//使用UTF8存取数据库 需要mysql 4.1.0以上支持
			mysql_query('SET NAMES "'.$this->_charset.'"', $this->_dbc_handle);
		}
		//设置 sql_model
		if($version > '5.0.1')
		{
			mysql_query('SET SQL_Mode=""', $this->_dbc_handle);
		}
		return true;
	}
	// 释放数据查询
	private function free()
	{
		$this->trace('public::query::free');
		if ($this->_query_handle && $this->_operate == 'SELECT')
		{
			mysql_free_result($this->_query_handle);
		}
		unset($this->_query_handle);
		unset($this->_operate);
		unset($this->_column);
		unset($this->_field);
		unset($this->_where);
		unset($this->_order);
		unset($this->_limit);
		unset($this->_data);
		unset($this->_cache);
		unset($this->_result);
		return true;
	}
	// 关闭数据库连接
	private function close()
	{
		if ($this->_dbc_handle)
		{
			$this->trace('public::server::close');
			mysql_close($this->_dbc_handle);
			unset($this->_dbc_handle);
		}
	}
	// <![数据库操作][
	// 增改删查
	public function select($column)
	{
		$this->_operate = 'SELECT';
		$this->_column = $column;
		return $this;
	}
	public function update($column)
	{
		$this->_operate = 'UPDATE';
		$this->_column = $column;
		return $this;
	}
	public function insert($column)
	{
		$this->_operate = 'INSERT';
		$this->_column = $column;
		return $this;
	}
	public function delete($column)
	{
		$this->_operate = 'DELETE';
		$this->_column = $column;
		return $this;
	}
	// 索取字段
	public function in($field)
	{
		$this->_field = $field;
		return $this;
	}
	// 条件
	public function where($where)
	{
		$this->_where[] = $where;
		return $this;
	}
	// 排序
	public function order($order)
	{
		$this->_order[] = $order;
		return $this;
	}
	// 限制返回结果数
	public function limit($limit)
	{
		$this->_limit = $limit;
		return $this;
	}
	// 数据存储
	public function data($data)
	{
		$this->_data[] = $data;
		return $this;
	}
	// 缓存设置
	public function cache($cache)
	{
		$this->_cache = $cache;
		return $this;
	}
	// 开始执行操作
	public function done()
	{
		$this->trace('public::query::init');
		// 数据表
		$column = $this->_prefix.$this->_column;
		// 组合SQL
		switch ($this->_operate)
		{
			case 'SELECT':
				// 索取字段
				if ($this->_field)
				{
					$field = $this->_field;
				}
				else
				{
					$field = '*';
				}
				$sql = 'SELECT '.$field.' FROM `'.$column.'`'.$this->pack_where().$this->pack_order().$this->pack_limit();
				break;
			case 'UPDATE':
				$sql = 'UPDATE `'.$column.'`'.$this->pack_data().$this->pack_where();
				break;
			case 'INSERT':
				$sql = 'INSERT INTO `'.$column.'`'.$this->pack_data();
				break;
			case 'DELETE':
				$sql = 'DELETE FROM `'.$column.'`'.$this->pack_where();
				break;
			default: break;
		}
		$this->sql = $sql;
		// 缓存判断 [暂时只支持缓存查询]
		if ($this->_operate == 'SELECT' && $this->cache_check())
		{
			$return = $this->_result;
			// 清理变量池并返回
			if ($this->free()) return $return;
		}
		// 连接判断
		if (!$this->_dbc_handle) $this->connect();
		// 开始执行SQL
		$this->trace('public::query::begin['.$this->_operate.']');
		$this->_query_handle = mysql_query($sql, $this->_dbc_handle);
		if (!$this->_query_handle)
		{
			$this->alert('SQL run error.');
		}
		$this->trace('public::query::finish['.$this->_operate.']');
		if ($this->_operate == 'SELECT')
		{
			if (mysql_num_rows($this->_query_handle) > 0)
			{
				while ($one_row = mysql_fetch_assoc($this->_query_handle))
				{
					$this->_result[] = $one_row;
				}
				mysql_data_seek($this->_query_handle, 0);
			}
			else
			{
				$this->_result = null;
			}
			// 写缓存
			if ($this->_need_cache) $this->cache_write();
			$return = $this->_result;
			// 清理变量池并返回
			if ($this->free()) return $return;
		}
		else
		{
			$return = mysql_affected_rows($this->_dbc_handle);
			// 清理变量池并返回
			if ($this->free()) return $return;
		}
	}
	// 返回结果限制
	private function pack_limit()
	{
		if ($this->_limit == '') return '';
		if (is_numeric($this->_limit))
		{
			return ' LIMIT 0,'.$this->_limit;
		}
		elseif (is_string($this->_limit))
		{
			return ' LIMIT '.$this->_limit;
		}
	}
	// 条件整合
	private function pack_where()
	{
		if (!$this->_where) return '';
		$sql_where = ' WHERE ';
		foreach ($this->_where as $where)
		{
			if (is_array($where))
			{
				foreach ($where as $key => $val)
				{
					if (is_numeric($val))
					{
						$sql_where .= $key.'='.$val;
					}
					elseif (is_string($val))
					{
						$sql_where .= $key.'="'.$val.'"';
					}
					$sql_where .= ' and ';
				}
			}
			elseif (is_string($where))
			{
				$conds = explode(',', $where);
				foreach ($conds as $one_cond)
				{
					$sql_where .= $one_cond.' and ';
				}
			}
		}
		return substr($sql_where, 0, -5);
	}
	// 排序整合
	private function pack_order()
	{
		if (!$this->_order) return '';
		$sql_order = ' ORDER BY ';
		foreach ($this->_order as $order)
		{
			if (is_array($order))
			{
				foreach ($order as $key => $type)
				{
					$sql_order .= $key.' '.$type.', ';
				}
			}
			elseif (is_string($order))
			{
				$ords = explode(',', $order);
				foreach ($ords as $one_ord)
				{
					$sql_order .= str_replace('.', ' ', $one_ord).', ';
				}
			}
		}
		return substr($sql_order, 0, -2);
	}
	// 数据整合
	private function pack_data()
	{
		if (!$this->_data) return '';
		$sql_data = ' SET ';
		foreach ($this->_data as $data)
		{
			if (is_array($data))
			{
				foreach ($data as $key => $val)
				{
					if (is_numeric($val))
					{
						$sql_data .= $key.'='.$val;
					}
					elseif (is_string($val))
					{
						$sql_data .= $key.'="'.$val.'"';
					}
					$sql_data .= ', ';
				}
			}
			elseif (is_string($data))
			{
				$datas = explode(',', $data);
				foreach ($datas as $one_data)
				{
					$sql_data .= $one_data.', ';
				}
			}
		}
		return substr($sql_data, 0, -2);
	}
	// ]>
	// 缓存检测
	private function cache_check()
	{
		$this->trace('private::cache::check');
		if ($this->_cache == '') return false;
		$this->_cache_key = md5($this->sql.'@'.$this->CACHE_HASH_SALT);
		$time_calc = array
		(
			's' => 1,
			'm' => 60,
			'h' => 3600,
			'd' => 86400
		);
		$c_rule = explode(':', $this->_cache);
		$c_time = $c_rule[0];
		$c_long = (int)$c_rule[1];
		if(time() - $this->cache_time() > $time_calc[$c_time]*$c_long)
		{
			$this->_need_cache = true;
			return false;
		}
		$this->_result = $this->cache_read();
		return true;
	}
	// 获取时间
	private function cache_time()
	{
		$handle = 'cache_handle_'.$this->_cached.'_time';
		return $this->$handle($this->_cache_key);
	}
	// 读缓存
	private function cache_read()
	{
		$this->trace('private::cache::read');
		$handle = 'cache_handle_'.$this->_cached.'_value';
		return $this->$handle($this->_cache_key);
	}
	// 写缓存
	private function cache_write()
	{
		$this->trace('private::cache::write');
		$handle = 'cache_handle_'.$this->_cached.'_write';
		$this->$handle($this->_cache_key, $this->_result);
		$this->_need_cache = false;
	}
	// <![缓存方式][
	// 文件缓存
	private function cache_handle_file_time($key)
	{
		if (is_file($this->_fc_path.$key.'.sql'))
		{
			return filemtime($this->_fc_path.$key.'.sql');
		}
		else
		{
			return 0;
		}
	}
	private function cache_handle_file_value($key)
	{
		if (is_file($this->_fc_path.$key.'.sql'))
		{
			return unserialize(file_get_contents($this->_fc_path.$key.'.sql'));
		}
		else
		{
			return false;
		}
	}
	private function cache_handle_file_write($key, $val)
	{
		file_put_contents($this->_fc_path.$key.'.sql', serialize($val));
		return true;
	}
	// memcache 缓存 [这里做的不怎么好，不过平常也用不到memcache的 ^_^]
	private function cache_handle_memcache_time($key)
	{
		$mec = new Memcache();
		$mec->connect($this->_mc_server);
		$val = $mec->get($this->_cache_key.'_time');
		$mec->close();
		if ($val == '')
		{
			return 0;
		}
		else
		{
			return $val;
		}
	}
	private function cache_handle_memcache_value($key)
	{
		$mec = new Memcache();
		$mec->connect($this->_mc_server);
		$val = $mec->get($this->_cache_key.'_value');
		$mec->close();
		if ($val == '')
		{
			return false;
		}
		else
		{
			return $val['value'];
		}
	}
	private function cache_handle_memcache_write($key, $val)
	{
		$mec = new Memcache();
		$mec->connect($this->_mc_server);
		$mec->set($this->_cache_key.'_time', time());
		$mec->set($this->_cache_key.'_value', array('cached'=>true,'value'=>$val));
		$mec->close();
		return true;
	}
	// ]>
	// <![一些数据库维护，例如：清空删除数据库、表，数据表优化等][
	// 功能后续添加
	// ]>
	// 调试信息
	private function alert($message)
	{
		if (!$this->_debug) return;
		echo '<div style="border:2px solid #000;margin:10px;padding:10px;">';
		echo $message;
		echo '<hr/>';
		echo mysql_error();
		echo '</div>';
		exit;
	}
	// 记录调试
	private function trace($message)
	{
		if (!$this->_debug) return;
		$this->_trace[] = array('timer'=>microtime(), 'mmusage'=>memory_get_usage(), 'message'=>$message);
	}
	// 输出调试
	public function trace_output()
	{
		if (!$this->_debug) return;
		echo '<div style="border:2px solid #000;margin:10px;padding:10px;">';
		echo '<ul>';
		foreach ($this->_trace as $i => $trace)
		{
			$timer_e = explode(' ', $trace['timer']);
			$timer = (float)$timer_e[0];
			$mmusage = $trace['mmusage'];
			echo '<li>Time: '.$timer.' <font color="#0FC69D">+'.($timer-$last_timer).'</font> Memory: '.$trace['mmusage'].' <font color="#E56298">+'.($mmusage-$last_mmusage).'</font> Call: '.$trace['message'].'</li>';
			$last_timer = $timer;
			$last_mmusage = $mmusage;
		}
		echo '</ul>';
		echo '</div>';
	}
}

?>