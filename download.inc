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
		drupal_set_message("There are no reference images in this proposal to download", 'error');
		drupal_goto('arduino-projects/manage-proposal');
	}
}

function arduino_projects_blog_download_step_images()
{
	global $user;
	$id = arg(3);
	$root_path = arduino_projects_blog_files_path();
	//var_dump($root_path);die;
	$query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('number', $id);
    $step_data = $query->execute()->fetchObject();
    //var_dump($step_data);die;
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $step_data->proposal_id);
    $proposal_data = $query->execute()->fetchObject();
	$step_images_directory_path = $proposal_data->directory_name . '/';
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
	$query = db_select('arduino_projects_blog_step_files');
	$query->fields('arduino_projects_blog_step_files');
	$query->condition('step_id', $step_data->id);
	$query->condition('filetype', 'I');
	$reference_images = $query->execute();
	while ($reference_images_files = $reference_images->fetchObject()) {
		$zip->addFile($root_path . $reference_images_files->filepath, $step_images_directory_path . str_replace(' ', '_', basename($reference_images_files->filename)));
	}
	$zip_file_count = $zip->numFiles;
	//var_dump($zip_file_count);die;
	$zip->close();
	if ($zip_file_count > 0) {
		if ($user->uid) {
			/* download zip file */
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $proposal_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			ob_end_flush();
			ob_clean();
			flush();
			readfile($zip_filename);
			unlink($zip_filename);
		} //$user->uid
		else {
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $proposal_data->project_title) . '.zip"');
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
		drupal_set_message("There are no images in this step to download", 'error');
		drupal_goto('arduino-projects/code-approval/bulk');
	}
}

function arduino_projects_blog_download_step_gifs()
{
	global $user;
	$id = arg(3);
	$root_path = arduino_projects_blog_files_path();
	//var_dump($root_path);die;
	$query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('number', $id);
    $step_data = $query->execute()->fetchObject();
    //var_dump($step_data);die;
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $step_data->proposal_id);
    $proposal_data = $query->execute()->fetchObject();
	$step_images_directory_path = $proposal_data->directory_name . '/';
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
	$query = db_select('arduino_projects_blog_step_files');
	$query->fields('arduino_projects_blog_step_files');
	$query->condition('step_id', $step_data->id);
	$query->condition('filetype', 'G');
	$reference_images = $query->execute();
	while ($reference_images_files = $reference_images->fetchObject()) {
		$zip->addFile($root_path . $reference_images_files->filepath, $step_images_directory_path . str_replace(' ', '_', basename($reference_images_files->filename)));
	}
	$zip_file_count = $zip->numFiles;
	//var_dump($zip_file_count);die;
	$zip->close();
	if ($zip_file_count > 0) {
		if ($user->uid) {
			/* download zip file */
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $proposal_data->project_title) . '.zip"');
			header('Content-Length: ' . filesize($zip_filename));
			ob_end_flush();
			ob_clean();
			flush();
			readfile($zip_filename);
			unlink($zip_filename);
		} //$user->uid
		else {
			header('Content-Type: application/zip');
			header('Content-disposition: attachment; filename="' . str_replace(' ', '_', $proposal_data->project_title) . '.zip"');
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
		drupal_set_message("There are no gifs in this step to download", 'error');
		drupal_goto('arduino-projects/code-approval/bulk');
	}
}