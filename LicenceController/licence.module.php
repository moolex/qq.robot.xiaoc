<?php

function licenceQuery($search)
{
	global $dbc;
	$passed = array();
	foreach ($search as $method => $data)
	{
		$where = array('sid'=>md5($data['callback']));
		$result = $dbc->select('licence')->where($where)->cache('h:'.$licence_cache_time)->done();
		if (!$result)
		{
			$passed[] = array('result'=>false, 'msg'=>'Not Allowed Callback URL.');
			continue;
		}
		$result = $result[0];
		if ($result['overtime'] < time())
		{
			$passed[] = array('result'=>false, 'msg'=>'Licence Expired.');
			continue;
		}
		$passed[] = array('result'=>true, 'data'=>$result);
	}

	// check passed
	$isPass = false;
	$licData = '';
	foreach ($passed as $i => $one)
	{
		if ($one['result'])
		{
			$isPass = true;
			$licData = $one['data'];
			break;
		}
		else
		{
			$licData = $one;
		}
	}
	return array('status'=>$isPass,'data'=>$licData);
}

function root_domain($domain)
{
	$d_dot = substr_count($domain, '.');
	if ($d_dot <= 1)
	{
		$root = $domain;
	}
	elseif ($d_dot >= 2)
	{
		$cn_domain_suffix = array('com', 'net', 'org', 'gov', 'edu');
		$d_sps = explode('.', $domain);
		$d_len = count($d_sps);
		$_last = $d_sps[$d_len-1];
		if ($_last == 'cn')
		{
			$_suffix = $d_sps[$d_len-2];
			if (in_array($_suffix, $cn_domain_suffix))
			{
				$pox = $d_len-3;
			}
			else
			{
				$pox = $d_len-2;
			}
		}
		else
		{
			$pox = $d_len-2;
		}
		for ($i=$pox; $i<$d_len; $i++)
		{
			$root .= $d_sps[$i].'.';
		}
	}
	if (substr($root, strlen($root)-1, 1) == '.')
	{
		$root = substr($root, 0, -1);
	}
	return $root;
}

?>