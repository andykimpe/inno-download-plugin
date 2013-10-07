function showTooltip(a, div)
{
	var r1 = a.getBoundingClientRect();
	var r2 = div.getBoundingClientRect();
	div.style.left    = r1.left - 125;
	div.style.top     = r1.bottom + 1;
	div.style.display = 'inline';
}
 
function hideTooltip(div)
{
	div.style.display = 'none';
}