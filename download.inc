<?php
// $Id$
function arduino_projects_blog_download_reference_images()
{
	global $user;
	$id = arg(3);
	$root_path = arduino_projects_blog_files_path();
	//var_dump($root_path);die;
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $id);
	$arduino_projects_q = $query->execute();
	$arduino_projects_data = $arduino_projects_q->fetchObject();
	$arduino_projects_directory_path = $arduino_projects_data->directory_name . '/';
	/* zip filename */
	$zip_filename = $root_path . 'zip-' . time() . '-' . rand(0, 999999) . '.zip';
	/* creating zip archive on the server */
	$zip = new ZipArchive();
	$zip->open($zip_filename, ZipArchive::CREATE);
	/*$query = db_select('om_pssp_proposal');
	$query->fields('om_pssp_proposal');
	$query->condition('id', $id);
	$om_pssp_udc_q = $query->execute();
	$query = db_select('om_pssp_proposal');
	$query->fields('om_pssp_proposal');
	$query->condition('id', $id);*/
	$query = db_select('arduino_projects_blog_proposal_images');
	$query->fields('arduino_projects_blog_proposal_images');
	$query->condition('proposal_id', $id);
	$reference_images = $query->execute();
	while ($reference_images_files = $reference_images->fetchObject()) {
		$zip->addFile($root_path . $reference_images_files->path, $arduino_projects_directory_path . str_replace(' ', '_', basename($reference_images_files->name)));
	}
	$zip_file_count = $zip->numFiles;
	//var_dump($zip_file_count);die;
	$zip->close();
	if ($zip_file_count > 0) {
		if ($user->uid) {
			/* download zip file */
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $arduino_projects_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			ob_end_flush();
			ob_clean();
			flush();
			readfile($zip_filename);
			unlink($zip_filename);
		} //$user->uid
		else {
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $arduino_projects_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			header("Content-Transfer-Encoding: binary");
			header('Expires: 0');
			header('Pragma: no-cache');
			ob_end_flush();
			ob_clean();
			flush();
			readfile($zip_filename);
			unlink($zip_filename);
		}
	} //$zip_file_count > 0
	else {
		drupal_set_message("There are no power system simulation project in this proposal to download", 'error');
		drupal_goto('powersystems');
	}
}
/*
function arduino_projects_blog_download_reference_images() {
	$proposal_id = arg(3);
	$root_path = arduino_projects_blog_files_path();
	$query = db_select('arduino_projects_blog_proposal_images');
	$query->fields('arduino_projects_blog_proposal_images');
	$query->condition('proposal_id', $proposal_id);
	$result = $query->execute();
	while($arduino_projects_blog_project_files = $result->fetchObject()){
	//var_dump($arduino_projects_blog_project_files);die;
	$query1 = db_select('arduino_projects_blog_proposal');
	$query1->fields('arduino_projects_blog_proposal');
	$query1->condition('id', $proposal_id);
	$result1 = $query1->execute();
	$arduino_projects_blog = $result1->fetchObject();
	$directory_name = $arduino_projects_blog->directory_name . '/project_files/';
	$samplecodename = $arduino_projects_blog_project_files->path;
	ob_clean();
	header("Pragma: public");
	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
	header("Cache-Control: public");
	header("Content-Description: File Transfer");
	header("Content-Type: application/zip");
	header('Content-disposition: attachment; filename="' . $samplecodename . '"');
	header("Content-Length: " . filesize($root_path . $samplecodename));
	header("Content-Transfer-Encoding: binary");
	header("Expires: 0");
	header("Pragma: no-cache");
	readfile($root_path . $samplecodename);
	ob_end_flush();
	ob_clean();
}
}
*/