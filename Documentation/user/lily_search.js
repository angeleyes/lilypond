var resObject = null;
var useAjax = (document.location.protocol.toLowerCase() == 'http:');
var previous_search = "";

function erzXMLHttpRequestObject ()
{
  var resObject = null;
  try {
    resObject = new XMLHttpRequest ();
  }
  catch (Error) {
    try {
      resObject = new ActiveXObject ("Microsoft.XMLHTTP");
    }
    catch (Error) {
      try {
        resObject = new ActiveXObject ("MSXML2.XMLHTTP");
      }
      catch (Error) {
        alert ("Unable to create XMLHttpRequest object for the search function!");
      }
    }
  }
  return resObject;
}

function searchResult (language, manual, bigpage)
{
  search_string = this.document.search_form.q.value;
  if (useAjax && previous_search != search_string) {
    if (useAjax && search_string.length >= 3) {
      var reldir = "";
      if (bigpage == 0) {
        reldir = "../"
      }
      resObject.open ('get', reldir + 'lily_index_search.php?lang=' + escape(language) + '&manual=' + escape(manual) + '&bigpage=' + bigpage + '&q=' + escape(search_string), true);
      resObject.onreadystatechange = handleResponse;
      resObject.send (null);
    } else {
      clearResults ();
    }
    previous_search = search_string;
  }
}

function handleResponse ()
{
  if (resObject.readyState == 4 ) {
    document.getElementById ('search_results').innerHTML = resObject.responseText;
  }
}

function clearResults ()
{
    document.getElementById ('search_results').innerHTML = "";
}


function print_search_field (language, manual, bigpage)
{
  if (useAjax) {
    // If the user presses enter and submits the form, also call the search 
    // script to print out the results in a separate page
    search_call = "searchResult('" + language + "', '" + manual + "', " + bigpage + ")";
    var reldir = "";
    if (bigpage == 0) {
      reldir = "../"
    }
    search_script = reldir + 'lily_index_search.php';
    document.write("<div id=\"search\">");
    document.write("<form name=\"search_form\" action=\"" + search_script + "\" onsubmit=\"" + search_call + "; return false;\">");
    document.write("<input type=\"hidden\" name=\"lang\" value=\"" + escape(language) + "\" >");
    document.write("<input type=\"hidden\" name=\"manual\" value=\"" + escape(manual) + "\" >");
    document.write("<input type=\"hidden\" name=\"bigpage\" value=\"" + bigpage + "\" >");
    document.write("<input type=\"hidden\" name=\"form_submitted\" value=\"1\" >");
    document.write("<p class=\"searchbar\">Search: ");
    document.write("  <input name=\"q\" onkeyup=\"" + search_call + "\" size=25></input>");
    document.write("    <span id=\"search_results\"></span>");
    document.write("</p>");
    document.write("  </form>");
    document.write("</div>");
  }
}
if (useAjax) {
  resObject = erzXMLHttpRequestObject ();
}