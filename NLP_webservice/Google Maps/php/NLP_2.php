<?php
require_once('./GeoNames_webservice/Geonames.php');
$geo = new Geonames();

//passthru('cat ./twitter_nlp-master/test.txt | python ./twitter_nlp-master/python/ner/extractEntities2.py --classify --chunk --pos',$result);
//$str=exec('python ./python.py',$result);
$str=exec('python /home/fatma/twitter_data/twitter_nlp-master/python/ner/extractEntities2.py',$result);
try {
var_dump($result);
     $strTrafficStatus=$result[1];
     $strAdress=$result[0];
     $geo->getLatandLong($strAdress,"uk",$strTrafficStatus);	
    }
catch (Exception $e) {
    error_log("Error with web service: " . $e->getMessage());
}
?>


