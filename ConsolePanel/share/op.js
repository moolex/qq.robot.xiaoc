$_G_page_loaded = {};
$(document).ready(function (){
	bindTabClick();
	loadTab($('#tab-list .active'));
	bindTableTrHover();
});
function bindTabClick()
{
	$('#tab-list li').bind('click', function(){
		loadTab(this);
	});
}
function bindTableTrHover()
{
	var target = $('.table-data tr[class!="title"][class!="tips"]');
	$(target).bind('mouseover', function(){
		$(this).addClass('tr-hover');
	});
	$(target).bind('mouseout', function(){
		$(this).removeClass('tr-hover');
	});
}
function loadTab(target)
{
	$('#tab-list li').removeClass('active');
	$(target).addClass('active');
	$('.page .content').hide();
	var name = $(target).attr('title');
	loadPage(name);
}
function loadPage(name)
{
	var loaded = pageLoadedFlag(name);
	if (!loaded)
	{
		pageLoadedFlag(name, true);
		notice('正在加载中...');
		$.get('index.php?do='+name, function(data){
			if (!checkData(data)) return;
			eval('var data='+data);
			if (data.status == 'ok')
			{
				$.each(data.data, function (i, one){
					displays_licence(one);
				});
			}
			success('加载已完成！', true);
			$('#pc_'+name).slideDown();
		});
	}
	else
	{
		$('#pc_'+name).slideDown();
	}
}
function pageLoadedFlag(name, flag)
{
	if (undefined == flag)
	{
		eval('var loaded=$_G_page_loaded.'+name);
		return loaded;
	}
	flag = flag ? 'true' : 'false';
	eval('$_G_page_loaded.'+name+' = '+flag);
}
// private function of licence
function displays_licence(data)
{
	var html = '';
	html += '<tr>';
	html += '<td>'+data.type+'</td>';
	html += '<td id="lic_inp_callback_'+data.id+'">'+data.callback+'</td>';
	html += '<td>'+data.overtime+'</td>';
	html += '<td>'+adminLink_of_licence(data.id)+'</td>';
	html += '</tr>';
	$('#pc_licence .table-data .tips').before(html);
}
function adminLink_of_licence(id)
{
	var links = '';
	links += '<a href="#" onclick="javascript:editorShow_of_licence(this, '+id+');return false;">编辑</a> - ';
	links += '<a href="#" onclick="javascript:itemDel_of_licence('+id+');return false;">删除</a> - ';
	links += '<a href="#" onclick="javascript:Renewals_of_licence('+id+');return false;">续费</a>';
	return links;
}
function editorShow_of_licence(link, id)
{
	if ($(link).html() == '编辑')
	{
		$(link).html('保存')
		var target = $('#lic_inp_callback_'+id);
		target.html('<input id="lic_inpx_callback_'+id+'" type="text" value="'+target.text()+'" size="28" />');
	}
	else
	{
		$(link).attr('disabled', 'disabled');
		notice('正在保存...');
		$.post('index.php?do=licence&op=save',
			{id:id,callback:encodeURIComponent($('#lic_inpx_callback_'+id).val())},
			function(data){
				if (!checkData(data)) return;
				eval('var data='+data);
				if (data.status == 'ok')
				{
					$(link).html('编辑');
					$('#lic_inp_callback_'+id).html($('#lic_inpx_callback_'+id).val());
					success('保存成功！', true);
				}
				else
				{
					wrong(data.msg);
				}
				$(link).removeAttr('disabled');
		});
	}
}
function itemDel_of_licence(id)
{
	if (!confirm('确认删除此授权么？'))
	{
		return;
	}
}
function Renewals_of_licence(id)
{
}
// Msg alerts
function checkData(data)
{
	if (data.substr(0,1) == '<')
	{
		window.location = '.';
		return false;
	}
	if (data.substr(0,1) != '{')
	{
		wrong(data);
		return false;
	}
	return true;
}
function wrong(msg, autoHide)
{
	__alert('error', msg, autoHide)
}
function success(msg, autoHide)
{
	__alert('success', msg, autoHide)
}
function notice(msg, autoHide)
{
	__alert('notice', msg, autoHide)
}
function __alert(type, msg, autoHide)
{
	msgHideClear();
	if (autoHide) msgHideLazy();
	$('#alert').removeClass().addClass('msg').addClass(type).html(msg);
	if ($('#alert').css('display') == 'none')
	{
		$('#alert').fadeIn();
	}
}
var $_G_msgHideTimer;
function msgHideLazy()
{
	$_G_msgHideTimer = setTimeout(function(){$('#alert').fadeOut();}, 2000);
}
function msgHideClear()
{
	if ($_G_msgHideTimer != undefined)
	{
		clearTimeout($_G_msgHideTimer);
	}
}