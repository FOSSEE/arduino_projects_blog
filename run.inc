<?php
function arduino_projects_details($form, &$form_state)
{
	$options_first = _list_of_project_titles();
	$url_project_id = (int) arg(2);
	//var_dump($url_project_id);die;
	$project_data = _project_information($url_project_id);
	if ($project_data == 'Not found') {
		$url_project_id = '';
	} //$project_data == 'Not found'
	if (!$url_project_id) {
		$selected = isset($form_state['values']['arduino_project']) ? $form_state['values']['arduino_project'] : key($options_first);
	} //!$url_project_id
	elseif ($url_project_id == '') {
		$selected = 0;
	} //$url_project_id == ''
	else {
		$selected = $url_project_id;
	}
	//var_dump($selected);die;
	$form = array();
	$form['arduino_project'] = array(
		'#type' => 'select',
		'#title' => t('Title of the Project'),
		'#options' => _list_of_project_titles(),
		'#default_value' => $selected,
		'#ajax' => array(
			'callback' => 'arduino_project_details_callback'
		)
	);
	if (!$url_project_id) {
		$form['arduino_project_details'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_project_details"></div>'
		);
		$form['intro_selected_project'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_project"></div>'
		);
		$form['step'] = array(
        '#type' => 'select',
        '#title' => t('Title of the Step'),
        '#options' => _step_information($selected),
        //'#default_value' => $chapter_default_value,
        '#prefix' => '<div id="ajax_select_step_list">',
        '#suffix' => '</div>',
        '#validated' => TRUE,
        '#tree' => TRUE,
        '#ajax' => array(
            'callback' => 'ajax_bulk_step_details_list_callback'
        ),
        '#states' => array(
            'invisible' => array(
                ':input[name="arduino_project"]' => array(
                    'value' => 0
                )
            )
        )
    );
		$form['step_selected_project'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_step_selected_project"></div>'
		);
		$form['selected_step_images'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_step_images"></div>'
		);

	} //!$url_project_id
	else {
		$project_default_value = $url_project_id;
		$form['arduino_project_details'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_project_details">' . _project_details($project_default_value) . '</div>'
		);
		$form['intro_selected_project'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_project">' . _project_intro_information($project_default_value) . '</div>'
		);
		$form['step'] = array(
        '#type' => 'select',
        '#title' => t('Title of the Step'),
        '#options' => _step_information($project_default_value),
        //'#default_value' => $chapter_default_value,
        '#prefix' => '<div id="ajax_select_step_list">',
        '#suffix' => '</div>',
        '#validated' => TRUE,
        '#tree' => TRUE,
        '#ajax' => array(
            'callback' => 'ajax_bulk_step_details_list_callback'
        ),
        '#states' => array(
            'invisible' => array(
                ':input[name="arduino_project"]' => array(
                    'value' => 0
                )
            )
        )
    );
		$form['step_selected_project'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_step_selected_project"></div>'
		);
		$form['selected_step_images'] = array(
			'#type' => 'item',
			'#markup' => '<div id="ajax_selected_step_images"></div>'
		);
	}
	return $form;
}
function ajax_bulk_step_details_list_callback($form, $form_state)
{
    $commands = array();
    $project_default_value = $form_state['values']['arduino_project'];
    $step_default_value = $form_state['values']['step'];
    if ($step_default_value != 0)
    {
        $commands[] = ajax_command_html('#ajax_step_selected_project', _step_project_details($project_default_value, $step_default_value));
        $commands[] = ajax_command_html('#ajax_selected_step_images', _selected_step_image_details($step_default_value));
    } //$step_default_value != 0
    else
    {
        $commands[] = ajax_command_html('#ajax_selected_step', '');
        $commands[] = ajax_command_data('#ajax_selected_step', 'form_state_value_select', $form_state['values']['step']);
    }
    return array(
        '#type' => 'ajax',
        '#commands' => $commands
    );
}
function _step_information($project_default_value)
{
	$proposal_id = $project_default_value;
    $steps_list = array(
        '0' => 'Please select...'
    );
    /*$book_chapters_q = db_query("SELECT * FROM {textbook_companion_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_id);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_id);
    $query->condition('approval_status', 1);
    $query->orderBy('number', 'ASC');
    $steps_q = $query->execute();
    while ($steps_data = $steps_q->fetchObject()) {
        $steps_list[$steps_data->number] = $steps_data->number . '. ' . $steps_data->name;
    }
    return $steps_list;
}
function arduino_project_details_callback($form, $form_state)
{
	$commands = array();
	$project_default_value = $form_state['values']['arduino_project'];
	if ($project_default_value != 0) {
		$form['arduino_project_details']['#markup'] = _project_details($project_default_value);
		$arduino_project_details = _project_information($project_default_value);
		$provider = user_load($arduino_project_details->uid);
		if ($arduino_project_details->uid > 0) {
			$commands[] = ajax_command_html('#ajax_selected_project', _project_intro_information($project_default_value));

        $form['step']['#options'] = _step_information($project_default_value);
        $commands[] = ajax_command_replace('#ajax_select_step_list', drupal_render($form['step']));
		} //$arduino_project_details->uid > 0
		else {
			$commands[] = ajax_command_html('#ajax_selected_project', _project_intro_information($project_default_value));

        $form['step']['#options'] = _step_information($project_default_value);
        $commands[] = ajax_command_replace('#ajax_select_step_list', drupal_render($form['step']));
			//$commands[] = ajax_command_html('#ajax_selected_project', '');
		}
		$commands[] = ajax_command_html('#ajax_project_details', _project_details($project_default_value));
		$form['step']['#options'] = _step_information($project_default_value);
        $commands[] = ajax_command_replace('#ajax_select_step_list', drupal_render($form['step']));
		//$commands[] = ajax_command_html('#ajax_step_selected_project', _step_project_details($project_default_value));
		$commands[] = ajax_command_html('#ajax_selected_project', _project_intro_information($project_default_value));
	} //$project_default_value != 0
	else {
		// $form['lab_experiment_list']['#options'] = _ajax_get_experiment_list();
		//  $commands[] = ajax_command_replace('#ajax_selected_experiment', drupal_render($form['lab_experiment_list']));
		$commands[] = ajax_command_html('#ajax_project_details', '');
		$commands[] = ajax_command_html('#ajax_selected_project', '');

		//$commands[] = ajax_command_html('#ajax_selected_project', '');
		//$commands[] = ajax_command_html('#ajax_selected_project', '');
		//$commands[] = ajax_command_data('#ajax_selected_project', 'form_state_value_select', $form_state['values']['arduino_project']);
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
function bootstrap_table_format($headers, $rows)
{
	$thead = "";
	$tbody = "";
	foreach ($headers as $header) {
		$thead .= "<th>{$header}</th>";
	} //$headers as $header
	foreach ($rows as $row) {
		$tbody .= "<tr>";
		foreach ($row as $data) {
			$tbody .= "<td>{$data}</td>";
		} //$row as $data
		$tbody .= "</tr>";
	} //$rows as $row
	$table = "
            <table class='table table-bordered table-hover' style='margin-left:-140px'>
                <thead>{$thead}</thead>
                <tbody>{$tbody}</tbody>
            </table>
        ";
	return $table;
}
/*****************************************************/
function _list_of_project_titles()
{
	$arduino_project_titles = array(
		'0' => 'Please select...'
	);
	//$lab_titles_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE solution_display = 1 ORDER BY lab_title ASC");
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('approval_status', 3);
	$query->orderBy('project_title', 'ASC');
	$arduino_project_titles_q = $query->execute();
	while ($arduino_project_titles_data = $arduino_project_titles_q->fetchObject()) {
		$arduino_project_titles[$arduino_project_titles_data->id] = $arduino_project_titles_data->project_title . ' (Proposed by ' . $arduino_project_titles_data->contributor_name . ')';
	} //$arduino_project_titles_data = $arduino_project_titles_q->fetchObject()
	return $arduino_project_titles;
}
function _project_information($proposal_id)
{
	$query = db_select('arduino_projects_blog_proposal');
	$query->fields('arduino_projects_blog_proposal');
	$query->condition('id', $proposal_id);
	$query->condition('approval_status', 3);
	$arduino_project_q = $query->execute();
	$project_data = $arduino_project_q->fetchObject();
	if ($project_data) {
		return $project_data;
	} //$project_data
	else {
		return 'Not found';
	}
}

function _project_details($project_default_value)
{
	$arduino_project_details = _project_information($project_default_value);
	if ($project_default_value != 0) {
		if($arduino_project_details->reference != NULL){
		$url = '~(?:(https?)://([^\s<]+)|(www\.[^\s<]+?\.[^\s<]+))(?<![\.,:])~i';
	        $reference = preg_replace($url, '<a href="$0" target="_blank" title="$0">$0</a>', $arduino_project_details->reference);
		}else{
		$reference = 'Not provided';
		}
		$form['arduino_project_details']['#markup'] = '<span style="color: rgb(128, 0, 0);"><strong>About the Project</strong></span></td><td style="width: 35%;"><br />' . '<ul>' . '<li><strong>Proposer Name:</strong> ' . $arduino_project_details->contributor_name . '</li>' . '<li><strong>Title of the Project:</strong> ' . $arduino_project_details->project_title . '</li>' . '<li><strong>University:</strong> ' . $arduino_project_details->university . '</li>' . '<li>'.'<strong>Reference:</strong> ' . $reference .'</li>'.'</ul>';
		$details = $form['arduino_project_details']['#markup'];
		return $details;
	} //$project_default_value != 0
}

function _project_intro_information($project_default_value){
	$proposal_id = $project_default_value;
	$intro_query = db_select('arduino_projects_blog_introduction_step');
    $intro_query->fields('arduino_projects_blog_introduction_step');
    $intro_query->condition('proposal_id', $proposal_id);
    $intro_query_result  = $intro_query->execute();
    $intro_query_data = $intro_query_result->fetchObject();
    $form['intro_selected_project']['#markup'] = convertLinkToEmbed($intro_query_data->demo_video, 750, 350) . '<br/>' . $intro_query_data->description;
    $details = $form['intro_selected_project']['#markup'];
		return $details;
}

function _step_project_details($project_default_value, $step_default_value)
{
	$proposal_id = $project_default_value;
	$step_id = $step_default_value;
	//var_dump($proposal_id);die;
	$query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_id);
    $query->condition('number', $step_id);
    $query->orderBy('number', 'ASC');
    $steps_q = $query->execute();
    $steps_data = $steps_q->fetchObject();
    $step_info_query = db_select('arduino_projects_blog_step_information');
    $step_info_query->fields('arduino_projects_blog_step_information');
    $step_info_query->condition('step_id', $steps_data->id);
    $step_info_query->condition('approval_status', 1);
    $steps_info_data = $step_info_query->execute()->fetchObject();
    if($steps_info_data->video) {
    $form['step_selected_project']['#markup'] = '<p>Step ' . $steps_data->number . ': ' . $steps_data->name . '</p><p> Description: ' . $steps_info_data->description . '</p>' . convertLinkToEmbed($steps_info_data->video, 750, 350);
    }
    else{
    	$form['step_selected_project']['#markup'] = '<p>Step ' . $steps_data->number . ': ' . $steps_data->name . '</p><p> Description: ' . $steps_info_data->description . '</p>';
    }
	$details = $form['step_selected_project']['#markup'];    
		return $details;
}

function _selected_step_image_details($step_default_value){
	$id = $step_default_value;
	$query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $id);
    $query->orderBy('number', 'ASC');
    $steps_q = $query->execute();
    $steps_data = $steps_q->fetchObject();
    $step_images_query = db_select('arduino_projects_blog_step_files');
    $step_images_query->fields('arduino_projects_blog_step_files');
    $step_images_query->condition('step_id', $steps_data->id);
    $step_images_result = $step_images_query->execute();
    //var_dump($step_images_result->rowCount());die;
    while($step_images_data = $step_images_result->fetchObject()){
    if($step_images_data){
    	$form['selected_step_images' . $step_images_data->id]['#markup'] = '<img style="padding-left :20px;width: 90%" src=' . $GLOBALS['base_url'] . "/project_uploads/" . $step_images_data->filepath . ' />';
    }
    }
    	$details = $form['selected_step_images']['#markup'];    
		return $details;
    }