function trigger(id, button)
{
	let string = button.value;
	let elem = document.getElementById(id);
	if(string == "-")
	{
		elem.hidden = true;
		elem.style.display = 'none';
		button.value = "+";
		button.innerText = "+";
	}
	else if(string == "+")
	{
		elem.hidden = false;
		elem.style.display = 'inline';
		button.value = "-";
		button.innerText = "-";
	}
}

var commentsHidden = false;
var classesHidden = false;
var functionsHidden = false;

function toggleVisibility(classname)
{
	var elements = document.getElementsByClassName(classname);
	if(classname == "Comments")
	{
		if(commentsHidden)
		{
			commentsHidden = false;
		}
		else
		{
			commentsHidden = true;
		}
		var i;
		for(i=0; i<elements.length; i++)
		{
			elements[i].hidden = commentsHidden;
		}
	}
	else if(classname == "Functions")
	{
		if(functionsHidden)
			functionsHidden = false;
		else
			functionsHidden = true;
		var i;
		for(i=0; i<elements.length; i++)
		{
			elements[i].hidden = functionsHidden;
		}
	}
	else if(classname == "Classes")
	{
		if(classesHidden)
			classesHidden = false;
		else
			classesHidden = true;
		var i;
		for(i=0; i<elements.length; i++)
		{
			elements[i].hidden = classesHidden;
		}
	}
}