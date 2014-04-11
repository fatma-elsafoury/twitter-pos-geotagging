<?php
function over_write_tweets_in_file($strFilePath,$strString){
	$myFile = $strFilePath;
	$fh = fopen($myFile, 'w') or die("can't open file");
	$stringData = $strString;
	$output=fwrite($fh, $stringData);
	fclose($fh);
	return $output;
}
?>