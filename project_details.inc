<?php
function arduino_projects_completed()
{
	$output = "";
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('actual_completion_date', 'DESC');
	//$query->condition('is_completed', 1);
	$result = $query->execute();
	
	if ($result->rowCount() == 0)
	{
		$output .= "We welcome your contributions." ."<hr>";

	} //$result->rowCount() == 0
	else
	{
		$output .= "Work has been completed for the following projects. We welcome your contributions." . "<hr>";
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$proposal_id = $row->id;
			/*$query1 = db_select('om_pssp_submitted_abstracts_file');
			$query1->fields('om_pssp_submitted_abstracts_file');
			$query1->condition('file_approval_status', 1);
			$query1->condition('proposal_id', $proposal_id);
			$project_files = $query1->execute();
			$project_abstract = $project_files->fetchObject();*/
			$completion_date = date("Y", $row->actual_completion_date);
			$preference_rows[] = array(
				$i,
				l($row->project_title, "arduino-projects/project-details/" . $row->id),
				$row->contributor_name,
				$row->university,
				$completion_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Title of the Project',
			'Contributor Name',
			'University / Institute',
			'Year of Completion'
		);
		$output .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $output;
}
// eSim power system simulation display in progress proposals
function arduino_projects_in_progress()
{
	$page_content = "";
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('approval_status', 1);
	$query->condition('is_completed', 0);
	$result = $query->execute();
	if ($result->rowCount() == 0)
	{
		$page_content .= "You are welcome to submit projects<hr>";
	} //$result->rowCount() == 0
	else
	{
		$page_content .= "Work is in progress for the following projects<hr>";
		$preference_rows = array();
		$i = 1;
		while ($row = $result->fetchObject())
		{
			$approval_date = date("Y", $row->approval_date);
			$preference_rows[] = array(
				$i,
				$row->project_title,
				$row->contributor_name,
				$row->university,
				$approval_date
			);
			$i++;
		} //$row = $result->fetchObject()
		$preference_header = array(
			'No',
			'Title of the Project',
			'Contributor Name',
			'University / Institute',
			'Year'
		);
		$page_content .= theme('table', array(
			'header' => $preference_header,
			'rows' => $preference_rows
		));
	}
	return $page_content;
}