<?php
require_once('GoogleGeoCodeWebService.php');
$str=exec('cat /home/fatma/twitter_data/twitter_nlp-master/temp_tweets.txt | python /home/fatma/twitter_data/twitter_nlp-master/python/ner/extractEntities2.py',$result);
try {
$strAddresses='';
$strEstimatedLocations='';
$strTrafficStatus='';
$strCause='';

	if(count($result)>0){
		$strAddresses=$result[0];
		$strEstimatedLocations=$result[1];
		$strTrafficStatus=$result[2];
		$strCause=$result[3];
	}
	
	//var_dump($strEstimatedLocations);
	//check if there is a location & get lat,lng for the extracted locations
	if($strAddresses=="No Location specified"){
		$strAddresses= "Location is out of interest region";
	}
	
	elseif($strAddresses=='' || $strAddresses==' '){
		$strAddresses='No location found';
	}
	
	else{
		if(strpos($strAddresses,'//')!=false){
			$locations=explode("//",$strAddresses);
			if (count($locations)>0){
				$strAddresses='';
				for($i=0;$i<count($locations);$i++){
					if($locations[$i]!='' || $locations[$i]!=' '){
						$strLocation=$locations[$i];
						$words=explode(" ",$strLocation);						
						if(count($words)==2){
							if($words[0]==' '){
									if($words[1]!='ln' && $words[1]!='lane' && $words[1]!='road' && $words[1]!='rd' && $words[1]!='street' && $words[1]!='st' && $words[1]!='corner' && $words[1]!='cnr' && $words[1]!='place' && $words[1]!='pl'){
										if ($strAddresses==''){
											$strAddresses=$words[1];
										}
										else{
										$strAddresses=$strAddresses.'&'.$words[1];
										}	
									}
								}
							else{
								if ($strAddresses==''){
									$strAddresses=$strLocation;
								}
								else{
									$strAddresses=$strAddresses.'&'.$strLocation;
								}	
							}
						}
						else{
							if ($strAddresses==''){
									$strAddresses=$strLocation;
							}
							else{
								$strAddresses=$strAddresses.'&'.$strLocation;
							}	
						}	
					}
				}
			}
		}
	}

		
	
	//check if there is a estimated locations & get lat,lng for the extracted locations or getting distances	
	if($strEstimatedLocations=="No estimation specified"){
		$strEstimatedLocations="No estimatation was found";
	}
	if($strEstimatedLocations=="" || $strEstimatedLocations==" "){
		$strEstimatedLocations="No estimatation was found";
	}
	
	//check if the traffic status is empty return empty
	if($strTrafficStatus=="No specified traffic status"){
		$strTrafficStatus="Can't extract traffic information from tweets";
	}
	elseif($strTrafficStatus==''|| $strTrafficStatus==' '){
		$strTrafficStatus= "No status was found";
	}
	
	//check if there is acause
	if($strCause=="No specified cause" ){
		$strCause= "No cause was found";
	}
	elseif($strCause=='' || $strCause==' '){
		$strCause= "No cause was found";
		
	}
	
	$strOutput=$strAddresses.'/'.$strEstimatedLocations.'/'.$strTrafficStatus.'/'.$strCause;
	//return the output
	echo $strOutput;
	//echo $strAdress.'*'.$strEstimatedLocations.'*'.$strTrafficStatus.'*'.$strCause;
}
catch (Exception $e) {
    error_log("Error with web service: " . $e->getMessage());
}
?>