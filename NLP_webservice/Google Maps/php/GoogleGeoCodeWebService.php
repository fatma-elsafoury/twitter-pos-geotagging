<?php
	function getLatandLong($addr,$region){

		$url = "http://maps.google.com/maps/api/geocode/json?address=".$addr."&region=".$region."&sensor=false";
		$response = file_get_contents($url);
		if ($response == false) {
			throw new Exception("Failure to obtain data");
		}
		$places = json_decode($response);
		if (!$places) {
			throw new Exception("Invalid JSON response");
		}
		if (is_array($places->results) && count($places->results)) {  

		    $result=$places->results[0];
			$geometry=$result->{'geometry'};
			$location=$geometry->{'location'};
			$lat=$location->{'lat'};
			$long=$location->{'lng'};


			ob_start(); // ensures anything dumped out will be caught
			// do stuff here
			
			//header( "Location: $url" ); 
			//return $lat.",".$long;
			
			
			// clear out the output buffer
			while (ob_get_status()){
				ob_end_clean();
			}			 		  
		}
		else {
			return "Unknown";
		}

	}	
?>	