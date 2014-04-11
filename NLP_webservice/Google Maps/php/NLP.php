<?php
require_once('./GeoNames_webservice/Geonames.php');
//require_once('./distance_estimation_webservice/distance_between_two_locations.php');
$geo = new Geonames();

//passthru('cat ./twitter_nlp-master/test.txt | python ./twitter_nlp-master/python/ner/extractEntities2.py --classify --chunk --pos',$result);
//$str=exec('python ./python.py',$result);
$str=exec('cat /home/fatma/twitter_data/twitter_nlp-master/temp_tweets.txt | python /home/fatma/twitter_data/twitter_nlp-master/python/ner/extractEntities2.py',$result);
try {
     $strTrafficStatus=$result[1];

     $strAdress=$result[0];

     $geo->getLatandLong($strAdress,"London,UK",$strTrafficStatus);  
    }
catch (Exception $e) {
    error_log("Error with web service: " . $e->getMessage());
	
	//'&nelat='.$nelat.'&nelng='.$nelng.'&swlat='.$swlat.'&swlng='.$swlng.
}
?>


