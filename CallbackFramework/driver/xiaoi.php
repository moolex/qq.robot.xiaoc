<?php
date_default_timezone_set("Asia/Shanghai");

class XiaoI
{
	public $sid = '';
	public $server = '122.226.240.164';

	public function __construct($sid = false)
	{
		if(!$sid)
		{
			$this->join();
		}
		else
		{
			$this->sid = $sid;
		}
	}
	public function join()
	{
		$webbot = file_get_contents("http://webbot.xiaoi.com/engine/flashrobot2/webbot.js?encoding=utf-8");
		preg_match("/var __sessionId = \"(\d+)\";/", $webbot, $sids);
		$this->sid = $sids[1];
		preg_match("/http:\/\/(.*)\/engine/", $webbot, $ips);
		$this->server = $ips[1];

		$fp = fsockopen($this->server, 80);
		fwrite($fp, "GET /engine/flashrobot2/send.js?encoding=utf-8&SID=$this->sid&USR=$this->sid&CMD=JOIN&r= HTTP/1.1\r\nConnection: Close\r\nHost: $this->server\r\n\r\n");
		fclose($fp);

		$fpr = fsockopen($this->server, 80);
		fwrite($fpr, "GET /engine/flashrobot2/recv.js?encoding=utf-8&SID=$this->sid&USR=$this->sid&r= HTTP/1.1\r\nConnection: Close\r\nHost: $this->server\r\n\r\n");
		fclose($fpr);
	}
	public function chat($msg)
	{
		$fpr = fsockopen($this->server, 80);
		fwrite($fpr, "GET /engine/flashrobot2/recv.js?encoding=utf-8&SID=$this->sid&USR=$this->sid&r= HTTP/1.1\r\nConnection: Close\r\nHost: $this->server\r\n\r\n");
		$fps = fsockopen($this->server, 80);
		fwrite($fps, "GET /engine/flashrobot2/send.js?encoding=utf-8&SID=$this->sid&USR=$this->sid&CMD=CHAT&SIG=You&MSG=".urlencode($msg)."&FTN=&FTS=&FTC=&r= HTTP/1.1\r\nConnection: Close\r\nHost: $this->server\r\n\r\n");
		fclose($fps);

		$r = '';
		while(!feof($fpr))
		{
			$r .= fgets($fpr, 512);
		}
		fclose($fpr);

		if(preg_match('!"MSG":"(.*?)"!', str_replace(array("\r", "\n"), array(''), $r), $match))
		{
			return $match[1];
		}
		else
		{
			return false;
		}
	}
}