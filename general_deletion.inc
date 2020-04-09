<?php
/******************************************************************************/
/****************************** DELETION FUNCTIONS ****************************/
/******************************************************************************/
function arduino_projects_blog_abstract_delete_project($proposal_id)
{
	$status = TRUE;
	$root_path = arduino_projects_blog_path();
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $proposal_id);
	$proposal_q = $query->execute();
	$proposal_data = $proposal_q->fetchObject();
	if (!$proposal_data)
	{
		drupal_set_message('Invalid power system simulation Project.', 'error');
		return FALSE;
	} //!$proposal_data
	$query = db_select('arduino_projects_blog_submitted_abstracts_file');
	$query->fields('arduino_projects_blog_submitted_abstracts_file');
	$query->condition('proposal_id', $proposal_id);
	$abstract_q = $query->execute();
	$dir_project_files = $root_path . $proposal_data->directory_name . '/project_files';
	while ($abstract_data = $abstract_q->fetchObject())
	{
		if (is_dir($dir_project_files)){

		unlink($root_path . $proposal_data->directory_name . '/project_files/' . $abstract_data->filepath);
		}
		else
		{
			drupal_set_message('Invalid power system simulation project abstract.', 'error');
		}
		db_delete('arduino_projects_blog_submitted_abstracts_file')->condition('proposal_id', $proposal_id)->execute();
		//!arduino_projects_blog_delete_abstract_file($abstract_data->id)
	}
	$res = rmdir($root_path . $proposal_data->directory_name . '/project_files');
	$dir_path_udc = $root_path . $proposal_data->directory_name;
	if (is_dir($dir_path_udc))
	{
		unlink($root_path . $proposal_data->samplefilepath);
		$res = rmdir($dir_path_udc);
	} //is_dir($dir_path_udc)
	db_delete('arduino_projects_blog_proposal')->condition('id', $proposal_data->id)->execute();
	
	db_delete('arduino_projects_blog_submitted_abstracts')->condition('proposal_id', $proposal_id)->execute();
	return $status;
}