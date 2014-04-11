<?php

function read_file_to_array ($strPath)
{
$select = file_get_contents($strPath);
$lines = explode("\n", $select);
return $lines;
}

//$lines= read_file_to_array("/home/fatma/twitter_nlp-master/Tweets_Data/London_Locations");
//foreach($lines as $value){
//echo($value);
//}

?>
