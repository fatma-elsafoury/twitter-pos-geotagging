<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
    <title>Traffic from tweets</title>
	<!--<link rel="stylesheet" href="CSS/table.css" type="text/css"/>-->
			<style type="text/css">
			@import "CSS/demo_page.css";
			@import "CSS/demo_table.css";
			@import "CSS/dataTables.scroller.css";
			/* Yes I'm inline, but for simplicities sake let it go :) */
				* {
					padding:0;
				}
				body {
					font-family:Arial,Helvetica,Tahoma,Verdana,Sans-Serif;margin:40px;
				}
				p {margin:0 auto;padding-top:40px;clear:both;}
				.btn { border:1px solid #999;padding:5px;display:inline-block;cursor:pointer;overflow:hidden;}


				/* Elements that will load the loader must have a relative postion everything else is up to you.*/
				.box-1 {
					position:relative;overflow:hidden;
					width:400px;height:200px;border:1px dashed #900;text-align:center;margin:5px;float:left;
				}
				.box-2 {
					position:relative;overflow:hidden;
					width:400px;height:200px;border:1px dashed #36F;text-align:center;margin:5px;float:left;
				}
				.box-3 {
					position:relative;overflow:hidden;
					width:400px;height:200px;border:1px dashed #0C0;text-align:center;margin:5px;float:left;
				}
				.box-4 {
					position:relative;overflow:hidden;
					width:400px;height:200px;border:1px dashed #636;text-align:center;margin:5px;float:left;
				}


				/***********************/
				/* Ajax Loader styles */
				.ajax_loader {background: url("images/spinner_squares_circle.gif") no-repeat center center transparent;width:100%;height:100%;}
				.blue-loader .ajax_loader {background: url("images/ajax-loader_blue.gif") no-repeat center center transparent;}

		</style>
    <script src="http://maps.google.com/maps?file=api&amp;v=2&amp;key=AIzaSyAVbJnObyAJLVHdXBCoPFvhE-unUcPlowI&sensor=true_or_false"
            type="text/javascript"></script>
	<script type="text/javascript" src="http://maps.google.com/maps/api/js?sensor=false"> </script>
	<script id="jquery" src="jQuery/jquery-1.4.2.min.js" type="text/javascript"></script>
	<script id="loader" src="jQuery/script.js" type="text/javascript"></script>
       		
	
	<script type="text/javascript" src="jQuery/jquery.js"></script>
	<script type="text/javascript" src="jQuery/jquery.dataTables.js"></script>
	<script type="text/javascript" src="jQuery/dataTables.scroller.js"></script>
    <script type="text/javascript" >
	
    var oTable;
	var mapLoadingBox;
	var tableloadingBox;
	//On page Load jQuery
	$(document).ready(function(){
		
		//Intialize map for London
		initialize();
		
		//intialize table 
		oTable = $('#dataGridTweets').dataTable( {
						"sScrollY": "200px",
						"sAjaxSource": "php/data/server_processing.php",
						"bServerSide": true,
						"bProcessing": true,
						"bFilter": true, 
						"sDom": "lrtip",  // default is lfrtip, where the f is the filter
						"bInfo":true,
						"bPaginate": true,
						"oScroller": {
						"loadingIndicator": true
						}
	                } );
					
		//add onChangeEvent for cboRegions
		$('#cbRegions').change(function() {
			var option = $(this).val();
			$.get('php/select.php', 
			{select:option}, 
			function(data) {
					
					$('#cbStreets').html("");//clear old options
					$('#cbStreets').get(0).add(new Option('--Select Street--','Select Street'), document.all ? i : null);
					data = eval(data);//get json array
					for (i = 0; i < data.length; i++){  //iterate over all options						
							$('#cbStreets').get(0).add(new Option(data[i],data[i]), document.all ? i : null);
					}
					if (settings.defaultValue != null){
						$('#cbStreets').val(settings.defaultValue);//select default value
					} 
					else{
						$("option:first", '#cbStreets').attr( "selected", "selected" );//select first option
					}
				});
		});
		
		//add onChangeEvent for cboStreets
		$('#cbStreets').change(function() {
		
			mapLoadingBox = new ajaxLoader('#map_canvas', {classOveride: 'blue-loader'});
			oTable.fnFilter( $(this).val(),0);	
			oTable.fnDraw();
			
			setTimeout(function(){
						var rows = oTable.fnGetNodes()
						var strTweets=''//'Accident - A4 Great West Rd at Syon Lane near Gillette Cnr - Road has been opened with lane restricted.\n Correction Accident - A4 Great West Rd at Syon Lane (Gillette Cnr) Road was closed w/b, which has been opened with a lane one restriction.'
						if(rows.length==0){
						    mapLoadingBox.remove();	
							return;
						}
						for(var i=0;i<rows.length;i++){	
							//Get HTML of 3rd column (for example) 
							if (strTweets==''){
								strTweets=$(rows[i]).find("td:eq(0)").html()
							}
							else{
								strTweets=$(rows[i]).find("td:eq(0)").html()+'\n'+strTweets
							}
						}
						
						//run php script to write the results of tweets in local file
						$.get('php/tweetScript.php', 
						{Tweets:strTweets}, 
						function(data) {
							//alert(data)
							//$('#divTweetsTable').show();
						})
						
						//run php script that calls python script to get POS of tweets
						$.get('php/NLPTool.php', 
						function(data) {
						var lat;
						var lng;
						var lat2;
						var lng2;
						var lat3;
						var lng3;
						var address1='';
						var address2='';
							var result=data.split("/");
							if (result[0]=='Location is out of interest region'){
								alert('Location is out of interest region');
								mapLoadingBox.remove();
								return;
							}
							if (result[0]=='No location found'){
								alert('No location found');
								mapLoadingBox.remove();
								return;
							}
							
							strEstimated=result[1];
							if(strEstimated != 'No estimatation was found'){
								strEstimated=strEstimated.split("|");
								lat3=strEstimated[0];
								lng3=strEstimated[1];
							}
							
							strStatus=result[2];
							
							if (strStatus=="Can't extract traffic information from tweets"){
								alert("Can't extract traffic information");
								mapLoadingBox.remove();
								return;
							}
							if (strStatus=='No status was found'){
								alert('No status was found');
								mapLoadingBox.remove();
								return;
							}
							
							strCause=result[3];
							
							if (strCause=='No cause was found'){
								alert('No cause was found');
								//mapLoadingBox.remove();
								//return;
							}
							
							var addresses=result[0];
							var n1=addresses.search("&");
							if(n1 != -1){
								addresses=result[0].split("&");
								n2=addresses[0].search("%");
								if ( n2!=-1){
									latlng=addresses[0].split("%");
									lat=latlng[0];
									lng=latlng[1];
								
								}
								else{
									address1=addresses[0]+',london,uk';
								}
								
								n3=addresses[1].search("%");
								alert("n3"+n3);
								if (n3 !=-1){
									
									latlng=addresses[1].split('%');
									alert(latlng[0]);
									lat2=latlng[0];
									lng2=latlng[1];
								}
								else{
									address2=addresses[1]+',london,uk';
								}
							}
							
							if(address1!='' && address2!=''){
							alert('c1');
							geocoder = new google.maps.Geocoder();
							geocoder.geocode( { 'address': address1}, 
												function(results, status) {
													results[0].geometry.location.lat()
													results[0].geometry.location.lng()
													geocoder.geocode( { 'address': address2}, 
																		function(results, status) {
																			lat2=results[0].geometry.location.lat()
																			lng2=results[0].geometry.location.lng()
																			//draw the route
																			Update_map_directions(lat,lng,lat2,lng2,lat3,lng3,strStatus,strCause);
																			
																		}
																	)
													
												}			
											)
											

							
			
							}
							
							else if (address1 !='' && address2==''){
							alert('c2');
								geocoder.geocode( { 'address': address1}, 
																		function(results, status) {
																			lat=results[0].geometry.location.lat()
																			lng=results[0].geometry.location.lng()
																			//draw the route
																			alert(lat+','+lng);
																			alert(lat2+','+lng2);
																			alert(lat3+','+lng3);
																			Update_map_directions(lat,lng,lat2,lng2,lat3,lng3,strStatus,strCause);
																			
																		}
												);	
							}
							else if (address2 !='' && address1==''){
							alert('c3');
								geocoder = new google.maps.Geocoder();
								geocoder.geocode( { 'address': address2}, 
																		function(results, status) {
																			lat2=results[0].geometry.location.lat()
																			lng2=results[0].geometry.location.lng()
																			//draw the route
																			Update_map_directions(lat,lng,lat2,lng2,lat3,lng3,strStatus,strCause);
																	}
												);
							}
							else if (address1=='' && address2=='') {
							alert('c4');
								//draw the route
								Update_map_directions(lat,lng,lat2,lng2,lat3,lng3,strStatus,strCause);		
							}	

							//remove loadinf bar
							mapLoadingBox.remove();			
							
						})

					}
					,1000)
				});	
		});
	
	
	function Update_map_directions(lat1,lng1,lat2,lng2,lat3,lng3,strStatus,strCause){
							
		        // init map
				var  infowindow = new google.maps.InfoWindow();
				var waypoints = [];		
				 waypoints.push({
					  location: lat3+','+lng3,
					  stopover: true
					});
                var myOptions = {
                    mapTypeId: google.maps.MapTypeId.ROADMAP,
					zoom:20
                };
                var map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);

                // init directions service
                var dirService = new google.maps.DirectionsService();
				var rendererOptions;
				
				if(strStatus=='jammed'){
					rendererOptions = {
					  suppressMarkers:true,
					  polylineOptions:{strokeColor:'#FF0000'}
					}
				}
				else if(strStatus=='crowded')	{
				
					rendererOptions = {
					  suppressMarkers:true,
					  polylineOptions:{strokeColor:'#ffff00'}
					}
				}
				else if(strStatus=='normal')	{
				//alert('d1');
					rendererOptions = {
					  suppressMarkers:true,
					  polylineOptions:{strokeColor:'#008000'}
					}
				}
                var dirRenderer = new google.maps.DirectionsRenderer(rendererOptions);
                dirRenderer.setMap(map);

                // highlight a street
                var request = {
                    origin:lat1+','+lng1, 
                    destination:lat2+','+lng2,
					waypoints:[{
						location:lat3+','+lng3,
						stopover:true
					}],
                    travelMode: google.maps.TravelMode.DRIVING
                };
				

                dirService.route(request, function(result, status) {
                    if (status == google.maps.DirectionsStatus.OK) {
                        dirRenderer.setDirections(result);

						//create info windows
						 infowindow.setContent('Traffic status:'+strStatus+'\n'+'Caus:'+strCause);
						 //infowindow.setContent('Caus:'+strCause);
						 var myLatLng = new google.maps.LatLng(lat2,lng2);
						  infowindow.setPosition(myLatLng);
						  infowindow.open(map);
                    }
					else if (status == google.maps.DirectionsStatus.INVALID_REQUEST) {
                       alert('INVALID_REQUEST');
                    }
					else if (status == google.maps.DirectionsStatus.MAX_WAYPOINTS_EXCEEDED) {
                       alert('MAX_WAYPOINTS_EXCEEDED');
                    }
					else if (status == google.maps.DirectionsStatus.NOT_FOUND) {
                       alert('NOT_FOUND');
                    }
					else if (status == google.maps.DirectionsStatus.OVER_QUERY_LIMIT) {
                       alert('OVER_QUERY_LIMIT');
                    }
					else if (status == google.maps.DirectionsStatus.REQUEST_DENIED) {
                       alert('REQUEST_DENIED');
                    }					
					else if (status == google.maps.DirectionsStatus.UNKNOWN_ERROR) {
                       alert('UNKNOWN_ERROR');
                    }
					else if (status == google.maps.DirectionsStatus.ZERO_RESULTS) {
                       alert('ZERO_RESULTS');
                    }
					else{
						alert('canot draw route');
					}
                });
	}
	
	
	function initialize() {
      if (GBrowserIsCompatible()) {
        var map = new GMap2(document.getElementById("map_canvas"));
        map.setCenter(new GLatLng(51.5171, 0.1062), 9);
        map.setUIToDefault();
      }
    }

	//function to read url parameters
	function getQueryParameter ( parameterName ) {
	  var queryString = window.top.location.search.substring(1);
	  var parameterName = parameterName + "=";
	  if ( queryString.length > 0 ) {
			begin = queryString.indexOf ( parameterName );
			if ( begin != -1 ) {
				begin += parameterName.length;
				end = queryString.indexOf ( "&" , begin );
				if ( end == -1 ) {
					end = queryString.length
				}
			return unescape ( queryString.substring ( begin, end ) );
			}
		}
	return "null";
	}

    </script>

	
  </head>
<body width="100%" height="100%">

  <!--design table -->
    <table border="0" align="center" CELLPADDING="4" CELLSPACING="0" >
	
		<!--Header row-->		
		<tr width="100%" height="2%">
			<td colspan="4" width="100%">
				<h3 width="100%"> Mointoring Traffic Status Using Twitter Messages </h3>
			</td>	
		</tr>
		
		<!--drop down box row -->	 
		<tr width="100%" height="5%" CellSpacing="0" cellpadding="0">
		
			<td width="1%">
				<form width="100%">
					<label width="100%">Select Region:</label>
				</form>
			</td>
			
			<td width="2%">
					<?php 
					    echo("<select id='cbRegions' onchange='cbo_Region_optionChange(this.value);'>");
						echo("<option value=''>--Select Region--</option>");
						require_once('php/readFile.php');
						$values=read_file_to_array ("/home/fatma/twitter_data/twitter_nlp-master/Tweets_Data/London_Regions");
					    
						foreach($values as $value){
						    //echo($value);
							echo ("<option value='$value'>$value</option>");
						}
						echo("</select>");						
					?>
				
			 </td>
		
			<td width="1%">
				<form width="100%">
					<label width="100%">Select Street:</label>
				</form>
			</td>
			 
			 <td width="2%">
					<select  name="cbStreets" id="cbStreets"> 
						<option value="Select Street">--Select Street--</option>
					</select>
				
			 </td>
		</tr>
		
		<!--Empty row spacing-->
		<tr height="50%">
			<td colspan="4">
			</td> 
		</tr>
		<!--Empty row spacing-->
		<tr height="50%">
			<td colspan="4">
			</td> 
		</tr>
		
		<!--Tweets table row-->
		
		<tr height="30%">
			<td colspan="4">
			<!--<div id="divTweetsTable" style="width: 1200px;height:280px;border:1px solid blue;" class="box-2">-->
				<table id="dataGridTweets"  cellpadding="0" cellspacing="0" border="0" class="display">
					<!-- first row with headers -->
					<thead>
						<tr>
						   <th>Tweet</th>
				           <th>User</th>
				           <th>Date</th>		
						</tr>
					</thead>
				</table>
				<!--</div>-->
			</td>
		</tr>
		
		<!--Empty row spacing-->
		<tr height="50%">
			<td colspan="4">
			</td> 
		</tr>
		<!--Empty row spacing-->
		<tr height="50%">
			<td colspan="4">
			</td> 
		</tr>
		<!--Map-->
		<tr align="center" height="10%">
			<td colspan="4">
				<div id="map_canvas" class="box-2" style="width: 1200px; height:400px;border:1px solid blue;"></div>
			
			</td>
		</tr>
	</table>
     
  </body>
</html>