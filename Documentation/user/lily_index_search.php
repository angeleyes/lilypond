<?php
  $lang = $_REQUEST['lang'];
  $man = $_REQUEST['manual'];
  $search_string = $_REQUEST['q'];
  $bigpage = $_REQUEST['bigpage'];
  $form_submitted = $_REQUEST['form_submitted'];
  
  $relpath = "";
  if ($form_submitted) {
    if (! $bigpage) {
      $relpath = "$man/";
    }
    echo "<html><body>\n";
  }

  $filename = "./$man";
  if ($bigpage) { $filename .= "-big-page"; }
  $filename .= ".$lang.idx";

  $found = 0;
  $file = fopen($filename, "r");
  while ( (($line=fgets($file)) !== false) ) {
    $line = rtrim($line);
    $entries = split ("\t", $line);
    if (stripos ($entries[0], $search_string) !== false) {
      if ($found == 0) {
        echo "<p><b>Search results for &quot;".htmlentities($search_string, ENT_QUOTES)."&quot;:</b><br>\n";
        echo "<table id=\"search_result_table\">\n";
      } else if ($found > 50) {
        echo "<tr><td colspan=2>Too many hits, displaying only 50 results</td></tr>\n";
        break;
      }
      // format the entry and print it out
      echo "<tr><td><a href=\"$relpath$entries[2]\">$entries[1]</a></td>\n";
      echo "    <td><a href=\"$relpath$entries[4]\">$entries[3]</a></td></tr>\n";
      $found++;
    }
  }
  if ($found > 0) {
    echo "</table>\n";
  } else {
    echo "No results found in the index.\n";
  }
  echo "</p>";
  fclose($file); 
  if ($form_submitted) {
    echo "</body></html>\n";
  }
//   echo "<p><b>Search results for $search_string:</b><br>";
//   echo "Index file: $filename<br>";
//   echo "Language: $lang<br>";
//   echo "Bigpage: $bigpage<br>";
//   echo "Manual: $man</p>";
?>