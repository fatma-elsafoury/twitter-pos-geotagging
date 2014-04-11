<?php
require_once('overWriteStringToFile.php');
$strString=$_GET['Tweets'];
$strFilePath='/home/fatma/twitter_data/twitter_nlp-master/temp_tweets.txt';
$output=over_write_tweets_in_file($strFilePath,$strString);
echo $output;
?>