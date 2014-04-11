<?php
require_once('readFile.php');
$strSelectedRegion=$_GET['select'];
$arr_values=read_file_to_array ('/home/fatma/twitter_data/twitter_nlp-master/Tweets_Data/London_Streets/'.$strSelectedRegion);
sort($arr_values);
echo json_encode( $arr_values );
?>