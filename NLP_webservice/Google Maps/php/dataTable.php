<?php
/**
*
* Simple Data Grid Skeleton
*
* Load a table from the database and display it in HTML with pagination and sorting.
*
* Please keep in mind that this a basic skeleton mock up to get the ball rolling
*
* You really will need to modify this! This code here is merely a skeleton for making
* a datagrid presentation of db table. It will not work out of the box but should save you 
* precious time in getting off the ground.
*
* http://snipplr.com/users/brownrl/
*
**/
 
// Some variables
 
// set these to connect to you databse or even better you should use db class or wrapper

function retrive_table_from_db($strHost,$strDbName,$strDbUser,$strDbPassword,$strTableName,$strWhere){

$dbhost = $strHost;
$dbname = $strDbName;
$dbuser = $strDbUser;
$dbpass = $strDbPassword;
 
// Here we start a PHP/Mysql connection you probably already have a 
// connection or a some db object
$dbc = mysql_connect( $dbhost , $dbuser , $dbpass ) or die( "Can't connect to the DB" );
mysql_select_db( $dbname , $dbc ) or die( "Can't select DB" );
  
// The table that you want to data grid
//
$table = $strTableName;
 
 
// The fields that you want to show in the data grid
// If you want to show everything then you need to use '*'
// and make some fixes below
$fields[] = "tweet_text";
$fields[] = "screen_name";
$fields[] = "created_at";

// How many to show per page
$per_page = 10;
 
///  Done with config and variables
// Now we start to setup some other variables
// You probably do want to edit after this too 
// make cool customizations, etc...
 
// Lets get the total in the table
$q = "SELECT COUNT( ".$fields[0]." ) as total FROM ".$table."";      
$qr = mysql_query( $q ) or die( "Can't select the total" );
$qrow = mysql_fetch_assoc( $qr );
$total_records = $qrow['total'];
 
 
// Lets get an offset started amd handle if we go off the ends with prev and next button
$offset = 0;
if( $offset < 0 ) // if less than 0 just go to 0
{
	$offset = 0;
}
 
while( $offset > $total_records ) // if over the total keep subtracting till under
{
	$offset = $offset - $per_page;
}
 
 
// lets get a sort by field
// take field[0] if not in there
// 
$sort_by = "created_at";
if( ! in_array( $sort_by , $fields ) )
{
	$sort_by = $fields[0];
}
 
 
// Now we are going to define the sorting order either ASC or DESC.
$directions[] = "ASC";
$directions[] = "DESC";
 
$order = "DESC";
if( ! in_array( $order , $directions ) )
{
	$order = "ASC";  // ASC is default
}
 
// Switch directions click on the ascending, get descending vice versa
$odirections['ASC'] = "DESC";
$odirections['DESC'] = "ASC";
 
// order signals up and down arrows maybe you want to use a picture?
$osignals['ASC'] = "&uarr;";
$osignals['DESC'] = "&darr;";
 
 
// BUILDING THE QUERY
// join the fields
$fs = join( ", " , $fields );
$q = "SELECT ".$fs." FROM ".$table." "."WHERE LCASE(tweet_text) like '%".$strWhere."%'"."ORDER BY ".$sort_by." ".$order." LIMIT ".$per_page." OFFSET ".$offset;
$qr = mysql_query( $q ) or die( "Can't select for showing :: $q" );
 
 return $qr;
}
 
?>