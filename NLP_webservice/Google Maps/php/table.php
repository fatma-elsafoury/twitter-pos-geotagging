<table border="1">
<!-- first row with headers -->
	<tr>
		<?php 
			$fields[] = "tweet_text";
			$fields[] = "screen_name";
			$fields[] = "created_at";
											 
			foreach( $fields as $k => $v) { ?>
			<th>
				<?= $v?>
			</th>
		<?php } ?>
		
	</tr>
	<!-- data row -->
	<?php 
		
			require_once("./dataTable.php");
			$qr=retrive_table_from_db("127.0.0.1",
									"twitterdatabase",
									 "root",
									 "root",
									 "tweets");
			while( $qrow = mysql_fetch_assoc( $qr ) ) { ?>
			
			<tr>
				<?php foreach( $qrow as $k => $v ) { ?>
					<td><?= $v ?></td>
				<?php } ?>
			</tr>
	<?php } ?>
</table><!-- end table -->
 