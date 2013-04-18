$SERVER = '/server.php';

$(document).ready
(
	function ()
	{
		loadMenus();
	}
);

function loadMenus()
{
	$('#navigate').load($SERVER+'?do=menus');
}