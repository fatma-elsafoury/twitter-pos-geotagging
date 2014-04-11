<?php 
	require_once("./dataTable.php");
	require_once("./tweetScript.php");
	$strTweet="";
	$qr=retrive_table_from_db("127.0.0.1",
				"twitterdatabase",
				"root",
				"root",
				"tweets",
				"a41");
				while( $qrow = mysql_fetch_assoc( $qr ) ) { 
					$strTweet=$qrow['tweet_text']."\n".$strTweet;
					 ?>
						<tr>
							<?php foreach( $qrow as $k => $v ) {?>
							<td><?= $v ?></td>
							<?php } ?>
						</tr>
				<?php } over_write_tweets_in_file("/home/fatma/twitter_data/twitter_nlp-master/temp_tweets.txt",$strTweet);
				
?>