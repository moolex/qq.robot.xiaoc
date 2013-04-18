<?php

return array
(
	'event' => array
	(
		'buddy.message' => true,
		'qun.message' => true,
	),
	'command' => array
	(
		'start' => array('@', '#'),
		'split' => ' '
	),
	'talk' => array
	(
		'start' => array('小C'),
		'replaces' => array(' ', ',', '，', '?', '？', '!', '！', "\n")
	)
);

?>